#include "AlarmManager.hpp"
#include <algorithm>
#include <cfloat>
#include <cstring>

using namespace alarm;

AlarmManager& AlarmManager::instance()
{
    static AlarmManager mgr;
    return mgr;
}

AlarmManager::AlarmManager()
{
    // initialize entries
    for (uint8_t i = 0; i < static_cast<uint8_t>(AlarmChannel::NUM_CHANNELS); ++i) {
        m_entries[i].severity = AlarmSeverity::OK;
        m_entries[i].activeSeverity = AlarmSeverity::OK;
        m_entries[i].zoneEnteredTick = 0;
        m_entries[i].lastValue = 0.0f;
        m_entries[i].acknowledged = false;
    }
    resetSessionSummary();
}

AlarmSeverity AlarmManager::getSeverity(AlarmChannel ch) const
{
    return m_entries[static_cast<uint8_t>(ch)].activeSeverity;
}

const AlarmEntry& AlarmManager::getEntry(AlarmChannel ch) const
{
    return m_entries[static_cast<uint8_t>(ch)];
}

AlarmSeverity AlarmManager::getWorstSeverity() const
{
    AlarmSeverity worst = AlarmSeverity::OK;
    for (uint8_t i = 0; i < static_cast<uint8_t>(AlarmChannel::NUM_CHANNELS); ++i) {
        AlarmSeverity s = m_entries[i].activeSeverity;
        if (s == AlarmSeverity::CRIT) return s;
        if (s == AlarmSeverity::WARN) worst = AlarmSeverity::WARN;
    }
    return worst;
}

uint8_t AlarmManager::getActiveAlarms(AlarmChannel* output, uint8_t max_count) const
{
    // collect CRIT first then WARN
    uint8_t count = 0;
    for (int pass = 0; pass < 2 && count < max_count; ++pass) {
        AlarmSeverity target = (pass == 0) ? AlarmSeverity::CRIT : AlarmSeverity::WARN;
        for (uint8_t i = 0; i < static_cast<uint8_t>(AlarmChannel::NUM_CHANNELS) && count < max_count; ++i) {
            if (m_entries[i].activeSeverity == target) {
                output[count++] = static_cast<AlarmChannel>(i);
            }
        }
    }
    return count;
}

void AlarmManager::acknowledge(AlarmChannel ch)
{
    m_entries[static_cast<uint8_t>(ch)].acknowledged = true;
}

void AlarmManager::acknowledgeAll()
{
    for (uint8_t i = 0; i < static_cast<uint8_t>(AlarmChannel::NUM_CHANNELS); ++i) {
        m_entries[i].acknowledged = true;
    }
}

const AlarmManager::SessionSummary& AlarmManager::getSessionSummary() const
{
    return m_session;
}

void AlarmManager::resetSessionSummary()
{
    m_session.maxCoolantTemp = -FLT_MAX;
    m_session.maxOilTemp = -FLT_MAX;
    m_session.minOilPressure = FLT_MAX;
    m_session.maxEGT = -FLT_MAX;
    m_session.maxBoost = -FLT_MAX;
    m_session.minFuelPressure = FLT_MAX;
    m_session.minBatteryVoltage = FLT_MAX;
    m_session.knockEventCount = 0;
    m_session.revlimitEventCount = 0;
}

float AlarmManager::extractValue(AlarmChannel ch, const Model::DashboardSnapshot& snap) const
{
    switch (ch) {
    case AlarmChannel::COOLANT_TEMP: return snap.coolant_temp;
    case AlarmChannel::OIL_TEMP: return snap.oil_temp;
    case AlarmChannel::OIL_PRESSURE: return snap.oil_pressure_kpa;
    case AlarmChannel::INTAKE_AIR_TEMP: return snap.intake_air_temp;
    case AlarmChannel::EXHAUST_TEMP_HIGHEST: return snap.exhaust_temp_highest;
    case AlarmChannel::EGT_SPREAD: {
        float minVal = FLT_MAX;
        for (int i = 0; i < 8; ++i) {
            float v = snap.egt[i];
            if (v > 0 && v < minVal) minVal = v;
        }
        if (minVal == FLT_MAX) return 0.0f;
        return snap.exhaust_temp_highest - minVal;
    }
    case AlarmChannel::BATTERY_VOLTAGE: return snap.battery_voltage;
    case AlarmChannel::BOOST_PRESSURE: return snap.map_kpa;
    case AlarmChannel::FUEL_PRESSURE: return snap.fuel_pressure_kpa;
    case AlarmChannel::LAMBDA_AVG: return snap.lambda_avg;
    case AlarmChannel::ECU_TEMP: return snap.ecu_temp;
    case AlarmChannel::ECU_ERROR_COUNT: return (float)snap.error_code_count;
    case AlarmChannel::CAN_TIMEOUT: return snap.can_timeout ? 1.0f : 0.0f;
    default: return 0.0f;
    }
}

AlarmSeverity AlarmManager::evaluateThreshold(AlarmChannel ch, float value) const
{
    const AlarmThreshold& th = ALARM_CONFIG[static_cast<uint8_t>(ch)];
    if (value < th.critLow || value > th.critHigh) {
        return AlarmSeverity::CRIT;
    }
    if (value < th.warnLow || value > th.warnHigh) {
        return AlarmSeverity::WARN;
    }
    return AlarmSeverity::OK;
}

void AlarmManager::updateSessionSummary(const Model::DashboardSnapshot& snap)
{
    // max/min updates
    m_session.maxCoolantTemp = std::max(m_session.maxCoolantTemp, snap.coolant_temp);
    m_session.maxOilTemp = std::max(m_session.maxOilTemp, snap.oil_temp);
    m_session.minOilPressure = std::min(m_session.minOilPressure, snap.oil_pressure_kpa);
    m_session.maxEGT = std::max(m_session.maxEGT, snap.exhaust_temp_highest);
    m_session.maxBoost = std::max(m_session.maxBoost, snap.map_kpa);
    m_session.minFuelPressure = std::min(m_session.minFuelPressure, snap.fuel_pressure_kpa);
    m_session.minBatteryVoltage = std::min(m_session.minBatteryVoltage, snap.battery_voltage);

    // event counts: detect rising edges
    static bool lastKnock = false;
    static bool lastRev = false;
    if (snap.knock_detected && !lastKnock) {
        m_session.knockEventCount++;
    }
    if (snap.revlimit_active && !lastRev) {
        m_session.revlimitEventCount++;
    }
    lastKnock = snap.knock_detected;
    lastRev = snap.revlimit_active;
}

void AlarmManager::evaluate(const Model::DashboardSnapshot& snap)
{
    uint32_t now = HAL_GetTick();
    for (uint8_t i = 0; i < static_cast<uint8_t>(AlarmChannel::NUM_CHANNELS); ++i) {
        AlarmChannel ch = static_cast<AlarmChannel>(i);
        float val = extractValue(ch, snap);
        AlarmSeverity raw = evaluateThreshold(ch, val);
        AlarmEntry &entry = m_entries[i];

        if (raw != entry.severity) {
            entry.severity = raw;
            entry.zoneEnteredTick = now;
            entry.lastValue = val;
            if (raw == AlarmSeverity::OK) {
                entry.acknowledged = false;
            }
        }

        if (raw == entry.severity) {
            uint16_t hyst = ALARM_CONFIG[i].hysteresis_ms;
            if ((now - entry.zoneEnteredTick) >= hyst) {
                entry.activeSeverity = raw;
            }
        }
    }
    updateSessionSummary(snap);
}
