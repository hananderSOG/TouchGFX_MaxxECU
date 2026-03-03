#pragma once

#include <cstdint>
#include <cfloat>

namespace alarm {

enum class AlarmChannel : uint8_t {
    COOLANT_TEMP = 0,
    OIL_TEMP,
    OIL_PRESSURE,
    INTAKE_AIR_TEMP,
    EXHAUST_TEMP_HIGHEST,
    EGT_SPREAD,
    BATTERY_VOLTAGE,
    BOOST_PRESSURE,
    FUEL_PRESSURE,
    LAMBDA_AVG,
    ECU_TEMP,
    ECU_ERROR_COUNT,
    CAN_TIMEOUT,
    NUM_CHANNELS
};

enum class AlarmSeverity : uint8_t {
    OK   = 0,
    WARN = 1,
    CRIT = 2
};

struct AlarmThreshold {
    float        warnLow;
    float        warnHigh;
    float        critLow;
    float        critHigh;
    uint16_t     hysteresis_ms;
    const char*  label;
    const char*  unit;
};

constexpr AlarmThreshold ALARM_CONFIG[] = {
    // COOLANT_TEMP
    { -FLT_MAX, 95.0f, -FLT_MAX, 105.0f, 2000, "COOLANT", "°C" },
    // OIL_TEMP
    { -FLT_MAX, 120.0f, -FLT_MAX, 135.0f, 2000, "OIL TEMP", "°C" },
    // OIL_PRESSURE (kPa) — low pressure is danger
    { 150.0f, FLT_MAX, 100.0f, FLT_MAX, 1000, "OIL PRESS", "kPa" },
    // INTAKE_AIR_TEMP
    { -FLT_MAX, 55.0f, -FLT_MAX, 70.0f, 3000, "IAT", "°C" },
    // EXHAUST_TEMP_HIGHEST
    { -FLT_MAX, 950.0f, -FLT_MAX, 1050.0f, 500, "EGT MAX", "°C" },
    // EGT_SPREAD (difference between highest and lowest EGT)
    { -FLT_MAX, 120.0f, -FLT_MAX, 200.0f, 1000, "EGT SPRD", "°C" },
    // BATTERY_VOLTAGE
    { 11.8f, 15.5f, 11.0f, 16.0f, 2000, "BATTERY", "V" },
    // BOOST_PRESSURE (kPa absolute, warn >230, crit >260)
    { -FLT_MAX, 230.0f, -FLT_MAX, 260.0f, 200, "BOOST", "kPa" },
    // FUEL_PRESSURE
    { 250.0f, FLT_MAX, 200.0f, FLT_MAX, 500, "FUEL PRESS", "kPa" },
    // LAMBDA_AVG (warn if very rich or lean)
    { 0.80f, 1.20f, 0.70f, 1.35f, 300, "LAMBDA", "λ" },
    // ECU_TEMP
    { -FLT_MAX, 75.0f, -FLT_MAX, 85.0f, 3000, "ECU TEMP", "°C" },
    // ECU_ERROR_COUNT (warn on any errors)
    { -FLT_MAX, 0.5f, -FLT_MAX, 4.5f, 500, "ECU ERR", "" },
    // CAN_TIMEOUT (boolean mapped to 0.0/1.0)
    { -FLT_MAX, 0.5f, -FLT_MAX, 0.5f, 500, "CAN", "" },
};

} // namespace alarm
