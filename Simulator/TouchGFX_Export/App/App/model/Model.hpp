#pragma once

#include <cstdint>
#include "shared_data.h"

/**
 * Model that represents the dashboard data retrieved from the CM4 core.
 * TouchGFX views and presenters query this object for the latest snapshot
 * of engine/vehicle state. The model reads from shared SRAM under a
 * hardware semaphore and never writes to it.
 */
class Model
{
public:
    /**
     * Simple snapshot of only the values used by the UI.  Snapshots are
     * updated periodically by readSharedData().
     */
    struct DashboardSnapshot {
        // Engine
        uint16_t rpm;
        float    throttle_pct;
        float    map_kpa;
        float    lambda_avg;
        float    lambda_a;
        float    lambda_b;
        float    ignition_timing;

        // Speed & Gear
        float    vehicle_speed;
        uint16_t gear;

        // Boost
        float    boost_target_kpa;
        float    boost_solenoid_duty;
        float    wastegate_pressure_kpa;
        float    fuel_pressure_kpa;

        // Temperatures
        float    coolant_temp;
        float    intake_air_temp;
        float    oil_temp;
        float    oil_pressure_kpa;
        float    exhaust_temp_highest;
        float    egt[8];

        // Battery & ECU
        float    battery_voltage;
        float    ecu_temp;
        uint16_t error_code_count;

        // Fuel
        float    fuel_ethanol_pct;
        float    fuel_inj_duty;

        // Status bits
        bool     revlimit_active;
        bool     knock_detected;
        bool     launch_control_active;
        bool     antilag_active;
        bool     shiftcut_active;
        bool     can_timeout;
    };

    Model();
    ~Model();

    /**
     * Called by TouchGFX once per frame (about every 16ms).  Internally the
     * model only updates its snapshot every third tick (~50Hz) to limit
     * semaphore contention and bus traffic.
     */
    void tick();

    /**
     * Retrieve the most recent snapshot.  The returned reference remains valid
     * until the next tick() invocation.
     */
    const DashboardSnapshot& getSnapshot() const { return m_snapshot; }

    /**
     * Indicate whether the data is fresh (no CAN timeout).
     */
    bool isDataFresh() const { return !m_snapshot.can_timeout; }

private:
    DashboardSnapshot m_snapshot;
    uint32_t          m_tickCounter;

    /**
     * Perform the actual copy from shared memory.  Non-blocking: if the HSEM
     * cannot be taken the previous snapshot is retained.
     */
    void readSharedData();
};
