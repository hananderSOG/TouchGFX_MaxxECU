#pragma once

#include <cstdint>
#include "AlarmConfig.hpp"
#include "../model/Model.hpp"
#include "../../Shared/Inc/shared_data.h" // for HAL_GetTick() if needed?

namespace alarm {

struct AlarmEntry {
    AlarmSeverity  severity;
    AlarmSeverity  activeSeverity;
    uint32_t       zoneEnteredTick;
    float          lastValue;
    bool           acknowledged;
};

class AlarmManager {
public:
    static AlarmManager& instance();

    void evaluate(const Model::DashboardSnapshot& snap);

    AlarmSeverity getSeverity(AlarmChannel ch) const;
    const AlarmEntry& getEntry(AlarmChannel ch) const;

    AlarmSeverity getWorstSeverity() const;

    uint8_t getActiveAlarms(AlarmChannel* output, uint8_t max_count) const;

    void acknowledge(AlarmChannel ch);
    void acknowledgeAll();

    struct SessionSummary {
        float maxCoolantTemp;
        float maxOilTemp;
        float minOilPressure;
        float maxEGT;
        float maxBoost;
        float minFuelPressure;
        float minBatteryVoltage;
        uint16_t knockEventCount;
        uint16_t revlimitEventCount;
    };

    const SessionSummary& getSessionSummary() const;
    void resetSessionSummary();

private:
    AlarmManager();
    AlarmEntry    m_entries[static_cast<uint8_t>(AlarmChannel::NUM_CHANNELS)];
    SessionSummary m_session;

    float extractValue(AlarmChannel ch, const Model::DashboardSnapshot& snap) const;
    AlarmSeverity evaluateThreshold(AlarmChannel ch, float value) const;
    void updateSessionSummary(const Model::DashboardSnapshot& snap);
};

} // namespace alarm
