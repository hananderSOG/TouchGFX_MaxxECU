#ifdef SIMULATOR

#include "SimCANDriver.hpp"
#include <cmath>
#include <cstring>
#include <chrono>
#include <random>
#include "../Shared/Inc/shared_data.h"
#include "stm32h7xx_hal.h" // for HAL_GetTick in simulator build (stubs)

using namespace std;

SimCANDriver& SimCANDriver::instance()
{
    static SimCANDriver inst;
    return inst;
}

SimCANDriver::SimCANDriver()
: m_timeAccum_s(0.0f), m_rpm(800.0f), m_speed(0.0f), m_coolant(78.0f), m_oilTemp(65.0f),
  m_oilPressure(380.0f), m_boost(100.0f), m_lambda(1.0f), m_throttle(0.0f), m_gear(0),
  m_scenario(SimScenario::IDLE)
{
    for (int i=0;i<8;++i) m_egt[i] = 200.0f;
}

void SimCANDriver::init()
{
    m_timeAccum_s = 0.0f;
}

float SimCANDriver::sineWave(float t_s, float min, float max) const
{
    float amp = (max - min) * 0.5f;
    float mid = (max + min) * 0.5f;
    return mid + amp * sinf(2.0f * M_PI * m_timeAccum_s / t_s);
}

float SimCANDriver::rampFloat(float current, float target, float rate_per_s, uint32_t delta_ms) const
{
    float dt = delta_ms / 1000.0f;
    float diff = target - current;
    float maxstep = rate_per_s * dt;
    if (fabs(diff) <= maxstep) return target;
    return current + (diff > 0 ? maxstep : -maxstep);
}

void SimCANDriver::overrideSignal(const char* signalName, float value)
{
    m_overrides[signalName] = value;
}

void SimCANDriver::clearOverride(const char* signalName)
{
    m_overrides.erase(signalName);
}

void SimCANDriver::clearAllOverrides()
{
    m_overrides.clear();
}

static std::default_random_engine rng((unsigned)std::chrono::system_clock::now().time_since_epoch().count());

void SimCANDriver::simulateScenario(uint32_t delta_ms)
{
    float dt_s = delta_ms / 1000.0f;
    m_timeAccum_s += dt_s;

    switch (m_scenario) {
    case SimScenario::IDLE: {
        m_rpm = sineWave(4.0f, 780.0f, 870.0f);
        m_throttle = sineWave(4.0f, 0.0f, 3.0f);
        m_speed = 0.0f;
        m_coolant = rampFloat(m_coolant, 88.0f, 0.5f, delta_ms);
        m_oilPressure = sineWave(2.0f, 360.0f, 420.0f);
        m_boost = sineWave(3.0f, 98.0f, 102.0f);
        m_lambda = sineWave(1.5f, 0.99f, 1.02f);
        m_gear = 0;
        m_egt[0] = 250.0f; for (int i=1;i<8;++i) m_egt[i]=200.0f + i*10.0f;
        break;
    }
    case SimScenario::ACCELERATION: {
        static float phase = 0.0f;
        // ramp 2000->7800 over 6s
        float targetRpm = fmodf(m_timeAccum_s, 9.0f) < 6.0f ? (2000.0f + (7800.0f-2000.0f) * (fmodf(m_timeAccum_s,9.0f)/6.0f)) : 2500.0f;
        m_rpm = rampFloat(m_rpm, targetRpm, 2000.0f, delta_ms);
        m_throttle = (targetRpm>3000.0f)? 98.0f : 20.0f;
        m_gear = 3;
        m_speed = m_rpm * 0.02f; // simple proportional
        m_boost = rampFloat(m_boost, (m_rpm/7800.0f)*240.0f + 100.0f, 60.0f, delta_ms);
        m_lambda = 0.90f;
        m_oilPressure = 480.0f + 40.0f * sinf(m_timeAccum_s*5.0f);
        for (int i=0;i<8;++i) m_egt[i] = rampFloat(m_egt[i], 400.0f + (m_rpm/7800.0f)*(950.0f-400.0f), 200.0f, delta_ms);
        // random knock pulse
        if ((rand()%10000) < 5) {
            g_maxxecu_data.Knock_detected = 1;
        } else {
            g_maxxecu_data.Knock_detected = 0;
        }
        break;
    }
    case SimScenario::CRUISE: {
        m_rpm = sineWave(8.0f, 2800.0f, 3100.0f);
        m_speed = sineWave(10.0f, 115.0f, 125.0f);
        m_throttle = sineWave(5.0f, 22.0f, 35.0f);
        m_boost = sineWave(6.0f, 105.0f, 130.0f);
        m_lambda = sineWave(2.0f, 0.98f, 1.03f);
        m_coolant = 92.0f + sineWave(12.0f, -1.0f, 1.0f);
        m_gear = 5;
        for (int i=0;i<8;++i) m_egt[i] = 350.0f + i*20.0f;
        break;
    }
    case SimScenario::OVERHEATING: {
        // base cruise
        simulateScenario(delta_ms); // reuse cruise values by fallthrough? avoid recursion
        // manually adjust coolant per timeline
        // compute elapsed in this scenario since set
        static uint32_t start_ms = HAL_GetTick();
        uint32_t elapsed = HAL_GetTick() - start_ms;
        if (elapsed < 15000) {
            m_coolant = 92.0f + (97.0f-92.0f) * (elapsed/15000.0f);
        } else if (elapsed < 30000) {
            m_coolant = 97.0f + (108.0f-97.0f) * ((elapsed-15000)/15000.0f);
        } else {
            m_coolant = 110.0f;
        }
        m_oilTemp = m_coolant + 15.0f;
        break;
    }
    case SimScenario::LOW_OIL: {
        // base cruise then oil pressure ramps down
        m_rpm = sineWave(8.0f, 2800.0f, 3100.0f);
        m_speed = sineWave(10.0f, 115.0f, 125.0f);
        static uint32_t start_ms = HAL_GetTick();
        uint32_t elapsed = HAL_GetTick() - start_ms;
        if (elapsed < 10000) {
            m_oilPressure = 380.0f + (160.0f-380.0f) * (elapsed/10000.0f);
        } else if (elapsed < 20000) {
            m_oilPressure = 160.0f + (90.0f-160.0f) * ((elapsed-10000)/10000.0f);
        } else {
            m_oilPressure = 80.0f + 15.0f * sinf(m_timeAccum_s*2.0f);
        }
        break;
    }
    case SimScenario::KNOCK_EVENT: {
        // mirror acceleration but generate knock every 5s
        m_rpm = sineWave(4.0f, 2000.0f, 7800.0f);
        m_throttle = 95.0f;
        if (fmod(m_timeAccum_s, 5.0f) < 0.25f) {
            g_maxxecu_data.Knock_detected = 1;
        } else {
            g_maxxecu_data.Knock_detected = 0;
        }
        break;
    }
    case SimScenario::LAUNCH: {
        float phase = fmod(m_timeAccum_s, 10.0f);
        if (phase < 2.0f) {
            m_rpm = 4500.0f;
            m_throttle = 100.0f;
            g_maxxecu_data.Launch_control_active = 1;
            g_maxxecu_data.Antilag_active = 1;
            m_boost = 180.0f;
        } else if (phase < 5.0f) {
            m_rpm = rampFloat(m_rpm, 7500.0f, 1000.0f, delta_ms);
            g_maxxecu_data.Launch_control_active = 0;
            g_maxxecu_data.Antilag_active = 0;
        } else {
            m_scenario = SimScenario::ACCELERATION;
        }
        break;
    }
    case SimScenario::ALL_WARNINGS: {
        m_coolant = 97.0f; m_oilTemp = 122.0f; m_oilPressure = 145.0f;
        m_egt[0]=960.0f; m_boost=235.0f; m_lambda=1.22f; m_speed=50.0f; m_gear=3;
        g_maxxecu_data.Knock_detected = 0;
        g_maxxecu_data.Revlimit_active = 0;
        break;
    }
    case SimScenario::ALL_CRITICAL: {
        m_coolant = 108.0f; m_oilTemp = 138.0f; m_oilPressure = 95.0f;
        m_egt[0]=1080.0f; m_boost=265.0f; m_lambda=0.68f; m_speed=60.0f; m_gear=3;
        g_maxxecu_data.Knock_detected = 1;
        g_maxxecu_data.Revlimit_active = 1;
        break;
    }
    case SimScenario::CAN_TIMEOUT: {
        // handled in writeToSharedStruct by writeEnabled flag
        m_rpm = sineWave(4.0f, 780.0f, 870.0f);
        m_throttle = sineWave(4.0f, 0.0f, 3.0f);
        m_speed = 0.0f;
        m_coolant = 88.0f;
        m_oilPressure = 380.0f;
        m_boost = 100.0f;
        m_lambda = 1.0f;
        break;
    }
    default:
        break;
    }

    // apply overrides
    for (auto &kv : m_overrides) {
        const string &k = kv.first;
        float v = kv.second;
        if (k == "RPM") m_rpm = v;
        else if (k == "SPEED") m_speed = v;
        else if (k == "COOLANT") m_coolant = v;
        else if (k == "OILP") m_oilPressure = v;
        else if (k == "BOOST") m_boost = v;
        else if (k == "LAMBDA") m_lambda = v;
        else if (k == "THROTTLE") m_throttle = v;
    }
}

void SimCANDriver::writeToSharedStruct(bool writeEnabled)
{
    if (!writeEnabled) return;
    // Write into g_maxxecu_data directly. Keep last_updated_tick fresh.
    g_maxxecu_data.EngineSpeed = static_cast<uint16_t>(m_rpm);
    g_maxxecu_data.ThrottlePos = m_throttle;
    g_maxxecu_data.MAP = m_boost;
    g_maxxecu_data.Lambda_Average = m_lambda;

    g_maxxecu_data.Lambda_A = m_lambda;
    g_maxxecu_data.Lambda_B = m_lambda;
    g_maxxecu_data.Ignition_Timing = 10.0f; // placeholder

    g_maxxecu_data.VehicleSpeed = m_speed;
    g_maxxecu_data.GearPosn = m_gear;
    g_maxxecu_data.ECU_BatteryVoltage = sineWave(3.0f, 13.8f, 14.2f);

    for (int i=0;i<8;++i) g_maxxecu_data.ExhaustTemp1 + i; // no-op to avoid warnings
    // fill EGTs
    g_maxxecu_data.ExhaustTemp1 = m_egt[0];
    g_maxxecu_data.ExhaustTemp2 = m_egt[1];
    g_maxxecu_data.ExhaustTemp3 = m_egt[2];
    g_maxxecu_data.ExhaustTemp4 = m_egt[3];
    g_maxxecu_data.ExhaustTemp5 = m_egt[4];
    g_maxxecu_data.ExhaustTemp6 = m_egt[5];
    g_maxxecu_data.ExhaustTemp7 = m_egt[6];
    g_maxxecu_data.ExhaustTemp8 = m_egt[7];
    g_maxxecu_data.ExhaustTempHighest = *std::max_element(m_egt, m_egt+8);

    g_maxxecu_data.CoolantTemp = m_coolant;
    g_maxxecu_data.Engine_Oil_Temperature = m_oilTemp;
    g_maxxecu_data.Engine_Oil_Pressure = m_oilPressure;
    g_maxxecu_data.Fuel_Pressure_1 = 300.0f; // placeholder
    g_maxxecu_data.Wastegate_Pressure = 0.0f;
    g_maxxecu_data.Boost_Target = m_boost;
    g_maxxecu_data.FuelInjDuty = 20.0f;
    g_maxxecu_data.FuelInjPulsewidth = 2.0f;

    // status bits
    g_maxxecu_data.Knock_detected = g_maxxecu_data.Knock_detected; // left as set in scenario

    // update timestamps for all messages
    uint32_t now = HAL_GetTick();
    for (uint16_t i = 0; i < MAXXECU_CAN_NUM_MSGS; ++i) {
        g_maxxecu_data.last_updated_tick[i] = now;
    }
}

void SimCANDriver::tick(uint32_t delta_ms)
{
    // For CAN_TIMEOUT scenario we stop writing after 3 seconds
    static uint32_t timeoutStart = 0;
    if (m_scenario == SimScenario::CAN_TIMEOUT) {
        if (timeoutStart == 0) timeoutStart = HAL_GetTick();
        uint32_t elapsed = HAL_GetTick() - timeoutStart;
        simulateScenario(delta_ms);
        bool writeEnabled = (elapsed < 3000);
        if (!writeEnabled) {
            // simulate no updates: do nothing
            return;
        }
        writeToSharedStruct(writeEnabled);
        return;
    }

    simulateScenario(delta_ms);
    writeToSharedStruct(true);
}

#endif // SIMULATOR
