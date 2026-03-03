#include "AlarmOverlay.hpp"
#include <touchgfx/Unicode.hpp>

AlarmOverlay::AlarmOverlay()
    : m_activeCount(0), m_scrollTick(0), m_scrollOffset(0)
{
}

void AlarmOverlay::setup()
{
    setPosition(0,0,800,40);
    m_bannerBg.setPosition(0,0,800,40);
    m_bannerBg.setVisible(false);
    add(m_bannerBg);
    m_severityIndicator.setPosition(0,0,8,40);
    add(m_severityIndicator);
    for (uint8_t i = 0; i < MAX_VISIBLE_ALARMS; ++i) {
        m_alarmTexts[i].setPosition(16 + i*260, 0, 260, 40);
        m_alarmTexts[i].setVisible(false);
        add(m_alarmTexts[i]);
    }
}

void AlarmOverlay::tick()
{
    ++m_scrollTick;
    if (m_scrollTick % 60 == 0) {
        refreshAlarmList();
    }
    if (m_scrollTick % 180 == 0 && m_activeCount > MAX_VISIBLE_ALARMS) {
        m_scrollOffset = (m_scrollOffset + 1) % m_activeCount;
        refreshAlarmList();
    }

    // blink crit text
    for (uint8_t i = 0; i < m_activeCount && i < MAX_VISIBLE_ALARMS; ++i) {
        alarm::AlarmSeverity s = alarm::AlarmManager::instance().getSeverity(m_visibleAlarms[(m_scrollOffset + i) % m_activeCount]);
        if (s == alarm::AlarmSeverity::CRIT) {
            uint8_t alpha = ((m_scrollTick / 30) % 2) ? 255 : 128;
            m_alarmTexts[i].setAlpha(alpha);
        }
    }
}

void AlarmOverlay::refreshAlarmList()
{
    alarm::AlarmChannel list[alarm::AlarmChannel::NUM_CHANNELS];
    uint8_t count = alarm::AlarmManager::instance().getActiveAlarms(list, MAX_VISIBLE_ALARMS);
    m_activeCount = count;
    for (uint8_t i = 0; i < MAX_VISIBLE_ALARMS; ++i) {
        if (i < count) {
            m_visibleAlarms[i] = list[(m_scrollOffset + i) % count];
            alarm::AlarmEntry const &entry = alarm::AlarmManager::instance().getEntry(m_visibleAlarms[i]);
            float val = entry.lastValue;
            const char* label = ALARM_CONFIG[static_cast<uint8_t>(m_visibleAlarms[i])].label;
            const char* unit = ALARM_CONFIG[static_cast<uint8_t>(m_visibleAlarms[i])].unit;
            Unicode::snprintf(m_textBuf, 32, "⚠ %s: %.1f %s", label, val, unit);
            m_alarmTexts[i].setTypedText(touchgfx::TypedText(touchgfx::TypedText::LAYOUT));
            m_alarmTexts[i].setWildcard(m_textBuf);
            m_alarmTexts[i].setVisible(true);
        } else {
            m_alarmTexts[i].setVisible(false);
        }
    }
    updateBannerColor();
}

void AlarmOverlay::updateBannerColor()
{
    if (m_activeCount == 0) {
        m_bannerBg.setVisible(false);
        return;
    }
    alarm::AlarmSeverity worst = alarm::AlarmManager::instance().getWorstSeverity();
    m_bannerBg.setColor(severityColor(worst));
    m_bannerBg.setVisible(true);
}

touchgfx::colortype AlarmOverlay::severityColor(alarm::AlarmSeverity s) const
{
    switch (s) {
    case alarm::AlarmSeverity::CRIT:
        return touchgfx::Color::getColorFrom24BitRGB(0xFF,0x00,0x00);
    case alarm::AlarmSeverity::WARN:
        return touchgfx::Color::getColorFrom24BitRGB(0xFF,0x45,0x00);
    default:
        return 0;
    }
}
