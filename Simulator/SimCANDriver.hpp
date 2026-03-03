#pragma once

#ifdef SIMULATOR

#include <cstdint>
#include <unordered_map>
#include <string>
#include "../CM4/Core/Inc/maxxecu_can.h"

enum class SimScenario : uint8_t {
    IDLE = 0,
    ACCELERATION,
    CRUISE,
    OVERHEATING,
    LOW_OIL,
    KNOCK_EVENT,
    LAUNCH,
    ALL_WARNINGS,
    ALL_CRITICAL,
    CAN_TIMEOUT
};

/**
 * Simulator CAN driver that writes directly into the shared data struct
 * `g_maxxecu_data`.  Intended to run on the PC alongside the TouchGFX
 * simulator so that CM7 code can run unchanged.
 */
class SimCANDriver {
public:
    static SimCANDriver& instance();

    /** Initialize simulator state (call once) */
    void init();

    /** Called every simulator frame with delta milliseconds since last call */
    void tick(uint32_t delta_ms);

    void setScenario(SimScenario scenario) { m_scenario = scenario; }
    SimScenario getScenario() const { return m_scenario; }

    void overrideSignal(const char* signalName, float value);
    void clearOverride(const char* signalName);
    void clearAllOverrides();

private:
    SimCANDriver();

    float   m_timeAccum_s;
    float   m_rpm;
    float   m_speed;
    float   m_coolant;
    float   m_oilTemp;
    float   m_oilPressure;
    float   m_boost;
    float   m_lambda;
    float   m_throttle;
    float   m_egt[8];
    uint8_t m_gear;
    SimScenario m_scenario;

    std::unordered_map<std::string, float> m_overrides;

    void simulateScenario(uint32_t delta_ms);
    void writeToSharedStruct(bool writeEnabled);
    float sineWave(float t_s, float min, float max) const;
    float rampFloat(float current, float target, float rate_per_s, uint32_t delta_ms) const;
};

#endif // SIMULATOR
