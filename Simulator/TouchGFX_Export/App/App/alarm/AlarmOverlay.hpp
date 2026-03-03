#pragma once

#include <touchgfx/containers/Container.hpp>
#include <touchgfx/widgets/TextArea.hpp>
#include <touchgfx/widgets/Box.hpp>
#include "AlarmConfig.hpp"
#include "AlarmManager.hpp"

class AlarmOverlay : public touchgfx::Container {
public:
    AlarmOverlay();
    void setup();
    void tick();

private:
    static constexpr uint8_t MAX_VISIBLE_ALARMS = 3;

    touchgfx::Box            m_bannerBg;
    touchgfx::TextArea       m_alarmTexts[MAX_VISIBLE_ALARMS];
    touchgfx::Box            m_severityIndicator;

    alarm::AlarmChannel  m_visibleAlarms[MAX_VISIBLE_ALARMS];
    uint8_t       m_activeCount;
    uint32_t      m_scrollTick;
    uint8_t       m_scrollOffset;

    void refreshAlarmList();
    void updateBannerColor();
    touchgfx::colortype severityColor(alarm::AlarmSeverity s) const;
    wchar_t m_textBuf[32];
};
