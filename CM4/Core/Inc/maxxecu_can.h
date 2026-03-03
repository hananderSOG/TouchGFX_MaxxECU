/*
 * maxxecu_can.h
 *
 * FDCAN driver and data definitions for MaxxECU messages.
 *
 * Generated automatically.
 */

#ifndef MAXXECU_CAN_H
#define MAXXECU_CAN_H

#include <stdint.h>
#include <stdbool.h>

/* number of messages we care about (range 0x520..0x542 inclusive) */
#define MAXXECU_CAN_MSG_ID_BASE     ((uint16_t)0x520u)
#define MAXXECU_CAN_MSG_ID_LAST     ((uint16_t)0x542u)
#define MAXXECU_CAN_NUM_MSGS        (MAXXECU_CAN_MSG_ID_LAST - MAXXECU_CAN_MSG_ID_BASE + 1)

/* shared data structure containing every signal from the DBC */
typedef struct
{
    /* 0x520 */
    uint16_t EngineSpeed;          /* rpm */
    float    ThrottlePos;          /* % */
    float    MAP;                  /* kPa */
    float    Lambda_Average;       /* lambda */

    /* 0x521 */
    float    Lambda_A;
    float    Lambda_B;
    float    Ignition_Timing;      /* deg */
    float    IgnitionCut;          /* % */

    /* 0x522 */
    float    FuelInjPulsewidth;    /* ms */
    float    FuelInjDuty;          /* % */
    float    FuelCut;              /* % */
    float    VehicleSpeed;         /* km/h */

    /* 0x523 */
    float    WheelSpeedAvgNonDriven;
    float    WheelSpeedAvgDriven;
    float    TracControl_SlipMeasured;
    float    TracControl_SlipTarget;

    /* 0x524 */
    float    TracControl_CutRequest;
    float    Lambda_corr_A;
    float    Lambda_corr_B;
    float    ECU_FirmwareVersion;

    /* 0x526 */
    uint16_t RevLimit_RPM;
    uint8_t  Shiftcut_active;
    uint8_t  Revlimit_active;
    uint8_t  Antilag_active;
    uint8_t  Launch_control_active;
    uint8_t  TracControl_power_limiter_active;
    uint8_t  Throttle_blip_active;
    uint8_t  AC_idle_up_active;
    uint8_t  Knock_detected;
    uint8_t  Brake_pedal_active;
    uint8_t  Clutch_pedal_active;
    uint8_t  Speed_limit_active;
    uint8_t  GP_limiter_active;
    uint8_t  User_cut_active;
    uint8_t  ECU_is_logging;
    uint8_t  Nitrous_active;

    /* 0x530 */
    float    ECU_BatteryVoltage;   /* V */
    float    BaroPress;            /* kPa */
    float    IntakeAirTemp;        /* °C */
    float    CoolantTemp;          /* °C */

    /* 0x531 */
    float    FuelTrimTotal;        /* % */
    float    FuelEthanolContent;   /* % */
    float    Ignition_Trim_total;  /* deg */
    float    ExhaustTemp1;         /* °C */

    /* 0x532 */
    float    ExhaustTemp2;
    float    ExhaustTemp3;
    float    ExhaustTemp4;
    float    ExhaustTemp5;

    /* 0x533 */
    float    ExhaustTemp6;
    float    ExhaustTemp7;
    float    ExhaustTemp8;
    float    ExhaustTempHighest;

    /* 0x534 */
    float    ExhaustTempDifference;
    float    ECU_Temp;
    uint16_t ECU_ErrorCodeCount;
    uint16_t ECU_LostSyncCount;

    /* 0x535 */
    float    ECU_UserAnalogInput1;
    float    ECU_UserAnalogInput2;
    float    ECU_UserAnalogInput3;
    float    ECU_UserAnalogInput4;

    /* 0x536 */
    uint16_t GearPosn;
    float    Boost_Solenoid_Duty;   /* % */
    float    Engine_Oil_Pressure;   /* kPa */
    float    Engine_Oil_Temperature;/* °C */

    /* 0x537 */
    float    Fuel_Pressure_1;       /* kPa */
    float    Wastegate_Pressure;    /* kPa */
    float    Coolant_Pressure;      /* kPa */
    float    Boost_Target;          /* kPa */

    /* 0x540 */
    uint8_t  Active_boost_table;
    uint8_t  Active_Tune_Selector;
    float    Virtual_fuel_tank;     /* L */
    float    Transmission_Temp;     /* °C */
    float    Differential_Temp;     /* °C */

    /* 0x541 */
    float    VVT_intake_cam1_position;   /* deg */
    float    VVT_exhaust_cam1_position;
    float    VVT_intake_cam2_position;
    float    VVT_exhaust_cam2_position;

    /* 0x542 */
    float    VVT_intake_cam_target;      /* deg */
    float    VVT_exhaust_cam_target;

    /* bookkeeping */
    uint32_t last_updated_tick[MAXXECU_CAN_NUM_MSGS];
    bool     can_timeout;
} MaxxECU_Data_t;

/* initialization and main handlers */
void MaxxECU_CAN_Init(void);
void MaxxECU_CAN_DecodeFrame(FDCAN_RxHeaderTypeDef *header, uint8_t *data);
void MaxxECU_CAN_TimeoutCheck(void);

#endif /* MAXXECU_CAN_H */
