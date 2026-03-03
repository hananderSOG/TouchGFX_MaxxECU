/*
 * maxxecu_can.c
 *
 * FDCAN1 driver implementation and decoding logic for MaxxECU.
 */

#include "maxxecu_can.h"
#include "shared_data.h"

#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_fdcan.h"
#include "stm32h7xx_hal_hsem.h"

/* macros from specification for extracting little-endian signals */
#define CAN_EXTRACT_U16_LE(buf, start_bit) \
  ((uint16_t)((buf)[(start_bit)/8] | ((buf)[(start_bit)/8 + 1] << 8)))

#define CAN_EXTRACT_I16_LE(buf, start_bit) \
  ((int16_t)CAN_EXTRACT_U16_LE(buf, start_bit))

/* private handle for FDCAN1 */
static FDCAN_HandleTypeDef hfdcan1;

/* helper to lock shared memory
   caller must supply barrier if necessary */
static inline void lock_shared(void)
{
    /* fast take will block until semaphore is available */
    HAL_HSEM_FastTake(HSEM_ID_MAXXECU);
}

static inline void unlock_shared(void)
{
    HAL_HSEM_Release(HSEM_ID_MAXXECU, 0);
}

/**
 * @brief  Initialize the CAN hardware and shared-memory primitives.
 * @note   Must be called from CM4 startup before entering the main loop.
 */
void MaxxECU_CAN_Init(void)
{
    /* initialize FDCAN1 peripheral */
    __HAL_RCC_FDCAN_CLK_ENABLE();

    hfdcan1.Instance = FDCAN1;
    hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
    hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
    hfdcan1.Init.AutoRetransmission = ENABLE;
    hfdcan1.Init.TransmitPause = DISABLE;
    hfdcan1.Init.ProtocolException = DISABLE;
    hfdcan1.Init.NominalPrescaler = 1u; /* these values assume 1 MHz CAN, adjust per clock */
    hfdcan1.Init.NominalSyncJumpWidth = 1u;
    hfdcan1.Init.NominalTimeSeg1 = 13u;
    hfdcan1.Init.NominalTimeSeg2 = 2u;
    hfdcan1.Init.DataPrescaler = 1u;
    hfdcan1.Init.DataSyncJumpWidth = 1u;
    hfdcan1.Init.DataTimeSeg1 = 1u;
    hfdcan1.Init.DataTimeSeg2 = 1u;

    HAL_FDCAN_Init(&hfdcan1);

    /* configure filter to accept IDs 0x520..0x542 */
    FDCAN_FilterTypeDef filter;
    filter.IdType = FDCAN_STANDARD_ID;
    filter.FilterIndex = 0;
    filter.FilterType = FDCAN_FILTER_RANGE;
    filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    filter.FilterID1 = 0x520u;
    filter.FilterID2 = 0x542u;
    HAL_FDCAN_ConfigFilter(&hfdcan1, &filter);

    /* enable reception into FIFO0 */
    HAL_FDCAN_Start(&hfdcan1);

    /* activate notification for new messages */
    HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);

    /* setup hardware semaphore clock (shared_data.h defines ID) */
    __HAL_RCC_HSEM_CLK_ENABLE();

    /* initialise shared data flags */
    lock_shared();
    for (uint16_t i = 0; i < MAXXECU_CAN_NUM_MSGS; ++i) {
        g_maxxecu_data.last_updated_tick[i] = 0;
    }
    g_maxxecu_data.can_timeout = false;
    unlock_shared();
}

/**
 * @brief  Decode a received FDCAN frame into the shared data structure.
 * @param  header CAN header provided by HAL
 * @param  data   8-byte payload
 */
void MaxxECU_CAN_DecodeFrame(FDCAN_RxHeaderTypeDef *header, uint8_t *data)
{
    uint16_t msgid = header->Identifier;
    uint32_t idx = msgid - MAXXECU_CAN_MSG_ID_BASE;

    lock_shared();

    switch (msgid) {
    case 0x520: {
        /* actual start bits and scalings taken from DBC should be used */
        uint16_t raw_rpm = CAN_EXTRACT_U16_LE(data, 0);
        g_maxxecu_data.EngineSpeed = raw_rpm;
        g_maxxecu_data.ThrottlePos = (float)CAN_EXTRACT_U16_LE(data, 16);
        g_maxxecu_data.MAP = (float)CAN_EXTRACT_U16_LE(data, 32);
        g_maxxecu_data.Lambda_Average = (float)CAN_EXTRACT_U16_LE(data, 48);
        break;
    }
    case 0x521: {
        g_maxxecu_data.Lambda_A = (float)CAN_EXTRACT_U16_LE(data, 0);
        g_maxxecu_data.Lambda_B = (float)CAN_EXTRACT_U16_LE(data, 16);
        g_maxxecu_data.Ignition_Timing = (float)CAN_EXTRACT_U16_LE(data, 32);
        g_maxxecu_data.IgnitionCut = (float)CAN_EXTRACT_U16_LE(data, 48);
        break;
    }
    case 0x522: {
        g_maxxecu_data.FuelInjPulsewidth = (float)CAN_EXTRACT_U16_LE(data, 0);
        g_maxxecu_data.FuelInjDuty = (float)CAN_EXTRACT_U16_LE(data, 16);
        g_maxxecu_data.FuelCut = (float)CAN_EXTRACT_U16_LE(data, 32);
        g_maxxecu_data.VehicleSpeed = (float)CAN_EXTRACT_U16_LE(data, 48);
        break;
    }
    case 0x523: {
        g_maxxecu_data.WheelSpeedAvgNonDriven = (float)CAN_EXTRACT_U16_LE(data, 0);
        g_maxxecu_data.WheelSpeedAvgDriven = (float)CAN_EXTRACT_U16_LE(data, 16);
        g_maxxecu_data.TracControl_SlipMeasured = (float)CAN_EXTRACT_U16_LE(data, 32);
        g_maxxecu_data.TracControl_SlipTarget = (float)CAN_EXTRACT_U16_LE(data, 48);
        break;
    }
    case 0x524: {
        g_maxxecu_data.TracControl_CutRequest = (float)CAN_EXTRACT_U16_LE(data, 0);
        g_maxxecu_data.Lambda_corr_A = (float)CAN_EXTRACT_U16_LE(data, 16);
        g_maxxecu_data.Lambda_corr_B = (float)CAN_EXTRACT_U16_LE(data, 32);
        g_maxxecu_data.ECU_FirmwareVersion = (float)CAN_EXTRACT_U16_LE(data, 48);
        break;
    }
    case 0x526: {
        g_maxxecu_data.RevLimit_RPM = CAN_EXTRACT_U16_LE(data, 0);
        /* status bits packed in one or more bytes; unpack individually */
        uint8_t b2 = data[2];
        g_maxxecu_data.Shiftcut_active = (b2 >> 0) & 1;
        g_maxxecu_data.Revlimit_active = (b2 >> 1) & 1;
        g_maxxecu_data.Antilag_active = (b2 >> 2) & 1;
        g_maxxecu_data.Launch_control_active = (b2 >> 3) & 1;
        g_maxxecu_data.TracControl_power_limiter_active = (b2 >> 4) & 1;
        g_maxxecu_data.Throttle_blip_active = (b2 >> 5) & 1;
        g_maxxecu_data.AC_idle_up_active = (b2 >> 6) & 1;
        g_maxxecu_data.Knock_detected = (b2 >> 7) & 1;
        uint8_t b3 = data[3];
        g_maxxecu_data.Brake_pedal_active = b3 & 1;
        g_maxxecu_data.Clutch_pedal_active = (b3 >> 1) & 1;
        g_maxxecu_data.Speed_limit_active = (b3 >> 2) & 1;
        g_maxxecu_data.GP_limiter_active = (b3 >> 3) & 1;
        g_maxxecu_data.User_cut_active = (b3 >> 4) & 1;
        g_maxxecu_data.ECU_is_logging = (b3 >> 5) & 1;
        g_maxxecu_data.Nitrous_active = (b3 >> 6) & 1;
        break;
    }
    case 0x530: {
        g_maxxecu_data.ECU_BatteryVoltage = (float)CAN_EXTRACT_U16_LE(data, 0);
        g_maxxecu_data.BaroPress = (float)CAN_EXTRACT_U16_LE(data, 16);
        g_maxxecu_data.IntakeAirTemp = (float)CAN_EXTRACT_U16_LE(data, 32);
        g_maxxecu_data.CoolantTemp = (float)CAN_EXTRACT_U16_LE(data, 48);
        break;
    }
    case 0x531: {
        g_maxxecu_data.FuelTrimTotal = (float)CAN_EXTRACT_U16_LE(data, 0);
        g_maxxecu_data.FuelEthanolContent = (float)CAN_EXTRACT_U16_LE(data, 16);
        g_maxxecu_data.Ignition_Trim_total = (float)CAN_EXTRACT_U16_LE(data, 32);
        g_maxxecu_data.ExhaustTemp1 = (float)CAN_EXTRACT_U16_LE(data, 48);
        break;
    }
    case 0x532: {
        g_maxxecu_data.ExhaustTemp2 = (float)CAN_EXTRACT_U16_LE(data, 0);
        g_maxxecu_data.ExhaustTemp3 = (float)CAN_EXTRACT_U16_LE(data, 16);
        g_maxxecu_data.ExhaustTemp4 = (float)CAN_EXTRACT_U16_LE(data, 32);
        g_maxxecu_data.ExhaustTemp5 = (float)CAN_EXTRACT_U16_LE(data, 48);
        break;
    }
    case 0x533: {
        g_maxxecu_data.ExhaustTemp6 = (float)CAN_EXTRACT_U16_LE(data, 0);
        g_maxxecu_data.ExhaustTemp7 = (float)CAN_EXTRACT_U16_LE(data, 16);
        g_maxxecu_data.ExhaustTemp8 = (float)CAN_EXTRACT_U16_LE(data, 32);
        g_maxxecu_data.ExhaustTempHighest = (float)CAN_EXTRACT_U16_LE(data, 48);
        break;
    }
    case 0x534: {
        g_maxxecu_data.ExhaustTempDifference = (float)CAN_EXTRACT_U16_LE(data, 0);
        g_maxxecu_data.ECU_Temp = (float)CAN_EXTRACT_U16_LE(data, 16);
        g_maxxecu_data.ECU_ErrorCodeCount = CAN_EXTRACT_U16_LE(data, 32);
        g_maxxecu_data.ECU_LostSyncCount = CAN_EXTRACT_U16_LE(data, 48);
        break;
    }
    case 0x535: {
        g_maxxecu_data.ECU_UserAnalogInput1 = (float)CAN_EXTRACT_U16_LE(data, 0);
        g_maxxecu_data.ECU_UserAnalogInput2 = (float)CAN_EXTRACT_U16_LE(data, 16);
        g_maxxecu_data.ECU_UserAnalogInput3 = (float)CAN_EXTRACT_U16_LE(data, 32);
        g_maxxecu_data.ECU_UserAnalogInput4 = (float)CAN_EXTRACT_U16_LE(data, 48);
        break;
    }
    case 0x536: {
        g_maxxecu_data.GearPosn = CAN_EXTRACT_U16_LE(data, 0);
        g_maxxecu_data.Boost_Solenoid_Duty = (float)CAN_EXTRACT_U16_LE(data, 16);
        g_maxxecu_data.Engine_Oil_Pressure = (float)CAN_EXTRACT_U16_LE(data, 32);
        g_maxxecu_data.Engine_Oil_Temperature = (float)CAN_EXTRACT_U16_LE(data, 48);
        break;
    }
    case 0x537: {
        g_maxxecu_data.Fuel_Pressure_1 = (float)CAN_EXTRACT_U16_LE(data, 0);
        g_maxxecu_data.Wastegate_Pressure = (float)CAN_EXTRACT_U16_LE(data, 16);
        g_maxxecu_data.Coolant_Pressure = (float)CAN_EXTRACT_U16_LE(data, 32);
        g_maxxecu_data.Boost_Target = (float)CAN_EXTRACT_U16_LE(data, 48);
        break;
    }
    case 0x540: {
        g_maxxecu_data.Active_boost_table = data[0];
        g_maxxecu_data.Active_Tune_Selector = data[1];
        g_maxxecu_data.Virtual_fuel_tank = (float)CAN_EXTRACT_U16_LE(data, 16);
        g_maxxecu_data.Transmission_Temp = (float)CAN_EXTRACT_U16_LE(data, 32);
        g_maxxecu_data.Differential_Temp = (float)CAN_EXTRACT_U16_LE(data, 48);
        break;
    }
    case 0x541: {
        g_maxxecu_data.VVT_intake_cam1_position = (float)CAN_EXTRACT_U16_LE(data, 0);
        g_maxxecu_data.VVT_exhaust_cam1_position = (float)CAN_EXTRACT_U16_LE(data, 16);
        g_maxxecu_data.VVT_intake_cam2_position = (float)CAN_EXTRACT_U16_LE(data, 32);
        g_maxxecu_data.VVT_exhaust_cam2_position = (float)CAN_EXTRACT_U16_LE(data, 48);
        break;
    }
    case 0x542: {
        g_maxxecu_data.VVT_intake_cam_target = (float)CAN_EXTRACT_U16_LE(data, 0);
        g_maxxecu_data.VVT_exhaust_cam_target = (float)CAN_EXTRACT_U16_LE(data, 16);
        break;
    }
    default:
        /* message outside of interest range */
        break;
    }

    /* update timestamp for this message */
    if (idx < MAXXECU_CAN_NUM_MSGS) {
        g_maxxecu_data.last_updated_tick[idx] = HAL_GetTick();
    }

    unlock_shared();
}

/**
 * @brief  Periodic timeout monitor, should run from CM4 main loop.
 *         If any configured message has not been updated for more than 500ms
 *         the global timeout flag is asserted.
 */
void MaxxECU_CAN_TimeoutCheck(void)
{
    uint32_t now = HAL_GetTick();
    bool timeout = false;

    lock_shared();
    for (uint16_t i = 0; i < MAXXECU_CAN_NUM_MSGS; ++i) {
        if ((now - g_maxxecu_data.last_updated_tick[i]) > 500u) {
            timeout = true;
            break;
        }
    }
    g_maxxecu_data.can_timeout = timeout;
    unlock_shared();
}

/**
 * @brief  FDCAN FIFO0 callback invoked by HAL when a new frame arrives.
 * @param  hfdcan handle pointer
 * @param  RxFifo0ITs interrupt flags
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    FDCAN_RxHeaderTypeDef header;
    uint8_t data[8];

    if (hfdcan->Instance != FDCAN1) {
        return;
    }

    /* read one message from FIFO0 */
    if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &header, data) == HAL_OK) {
        MaxxECU_CAN_DecodeFrame(&header, data);
    }
}
