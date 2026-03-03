#pragma once

#include <touchgfx/containers/Container.hpp>
#include <touchgfx/widgets/TextArea.hpp>
#include <touchgfx/widgets/Button.hpp>
#include "AlarmManager.hpp"

class SessionSummaryScreen : public touchgfx::Container {
public:
    SessionSummaryScreen();
    void setupScreen();
    void tearDownScreen();

    void setSummaryVisible(bool visible);

private:
    touchgfx::TextArea m_cells[9];
    touchgfx::Button m_resetButton;
    wchar_t m_buf[32];

    void refresh();
    void onResetPressed(const touchgfx::AbstractButton&);
};
