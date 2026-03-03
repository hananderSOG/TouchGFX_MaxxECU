#include "Model.hpp"
#include <cstring> // for memcpy
#include "shared_data.h"

Model::Model()
    : m_tickCounter(0)
{
    // initialize snapshot to zero
    std::memset(&m_snapshot, 0, sizeof(m_snapshot));
}

Model::~Model() {}

void Model::tick()
{
    ++m_tickCounter;
    if ((m_tickCounter % 3) == 0) {
        readSharedData();
    }
}

void Model::readSharedData()
{
    /* try to acquire the semaphore once without blocking */
    if (SHARED_HSEM_TRY_TAKE()) {
        /* ensure prior reads from shared memory are ordered before copy */
        SHARED_DMB();

        /* copy fields of interest */
        const MaxxECU_Data_t &src = g_maxxecu_data;
        DashboardSnapshot &dst = m_snapshot;

        dst.rpm = src.EngineSpeed;
        dst.throttle_pct = src.ThrottlePos;
        dst.map_kpa = src.MAP;
        dst.lambda_avg = src.Lambda_Average;
        dst.lambda_a = src.Lambda_A;
        dst.lambda_b = src.Lambda_B;
        dst.ignition_timing = src.Ignition_Timing;

        dst.vehicle_speed = src.VehicleSpeed;
        dst.gear = src.GearPosn;

        dst.boost_target_kpa = src.Boost_Target;
        dst.boost_solenoid_duty = src.Boost_Solenoid_Duty;
        dst.wastegate_pressure_kpa = src.Wastegate_Pressure;
        dst.fuel_pressure_kpa = src.Fuel_Pressure_1;

        dst.coolant_temp = src.CoolantTemp;
        dst.intake_air_temp = src.IntakeAirTemp;
        dst.oil_temp = src.Engine_Oil_Temperature;
        dst.oil_pressure_kpa = src.Engine_Oil_Pressure;
        dst.exhaust_temp_highest = src.ExhaustTempHighest;
        dst.egt[0] = src.ExhaustTemp1;
        dst.egt[1] = src.ExhaustTemp2;
        dst.egt[2] = src.ExhaustTemp3;
        dst.egt[3] = src.ExhaustTemp4;
        dst.egt[4] = src.ExhaustTemp5;
        dst.egt[5] = src.ExhaustTemp6;
        dst.egt[6] = src.ExhaustTemp7;
        dst.egt[7] = src.ExhaustTemp8;

        dst.battery_voltage = src.ECU_BatteryVoltage;
        dst.ecu_temp = src.ECU_Temp;
        dst.error_code_count = src.ECU_ErrorCodeCount;

        dst.fuel_ethanol_pct = src.FuelEthanolContent;
        dst.fuel_inj_duty = src.FuelInjDuty;

        dst.revlimit_active = src.Revlimit_active;
        dst.knock_detected = src.Knock_detected;
        dst.launch_control_active = src.Launch_control_active;
        dst.antilag_active = src.Antilag_active;
        dst.shiftcut_active = src.Shiftcut_active;

        /* copy timeout flag last (not data critical) */
        dst.can_timeout = src.can_timeout;

        SHARED_DSB();

        SHARED_HSEM_RELEASE();
    }
    /* if semaphore was not acquired, leave m_snapshot unchanged */
}
