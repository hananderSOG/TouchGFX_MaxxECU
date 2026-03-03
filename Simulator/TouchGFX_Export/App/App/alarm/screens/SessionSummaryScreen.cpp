#include "SessionSummaryScreen.hpp"
#include <touchgfx/Unicode.hpp>

SessionSummaryScreen::SessionSummaryScreen()
{
    m_resetButton.setAction(touchgfx::Callback<SessionSummaryScreen, const touchgfx::AbstractButton&>(this, &SessionSummaryScreen::onResetPressed));
}

void SessionSummaryScreen::setupScreen()
{
    setPosition(0,0,800,480);
    // layout cells in 2 columns of 5 rows
    for (int i = 0; i < 9; ++i) {
        m_cells[i].setPosition((i%2)*400 + 10, (i/2)*50 + 10, 380, 40);
        add(m_cells[i]);
    }
    m_resetButton.setXY(650,420);
    add(m_resetButton);
    refresh();
}

void SessionSummaryScreen::tearDownScreen()
{
}

void SessionSummaryScreen::refresh()
{
    auto const &s = alarm::AlarmManager::instance().getSessionSummary();
    // fill each cell with label + value
    int idx=0;
    Unicode::snprintf(m_buf,32,"Max COOLANT: %.1f°C", s.maxCoolantTemp);
    m_cells[idx++].setTypedText(touchgfx::TypedText(touchgfx::TypedText::LAYOUT));
    m_cells[idx++].setTypedText(touchgfx::TypedText(touchgfx::TypedText::LAYOUT));
    // continue for others...
}

void SessionSummaryScreen::setSummaryVisible(bool visible)
{
    setVisible(visible);
    if (visible) refresh();
}

void SessionSummaryScreen::onResetPressed(const touchgfx::AbstractButton&)
{
    alarm::AlarmManager::instance().resetSessionSummary();
    refresh();
}
