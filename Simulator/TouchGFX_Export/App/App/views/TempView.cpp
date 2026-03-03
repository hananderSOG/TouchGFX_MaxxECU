#include "TempView.hpp"
#include <touchgfx/Unicode.hpp>

TempView::TempView()
{
}

void TempView::setupScreen()
{
    // layout code here
}

void TempView::tearDownScreen()
{
}

void TempView::setCoolantTemp(float c)
{
    Unicode::snprintf(buf, 16, "%.1f°C", c);
    coolantText.invalidate();
}

void TempView::setIntakeTemp(float c)
{
    Unicode::snprintf(buf, 16, "%.1f°C", c);
    intakeText.invalidate();
}

void TempView::setOilTemp(float c)
{
    Unicode::snprintf(buf, 16, "%.1f°C", c);
    oilText.invalidate();
}

void TempView::setECUTemp(float c)
{
    Unicode::snprintf(buf, 16, "%.1f°C", c);
    ecuText.invalidate();
}

void TempView::setEGT(const float *temps, int count)
{
    for (int i = 0; i < count && i < 8; ++i) {
        Unicode::snprintf(buf, 16, "%.0f", temps[i]);
        egtBars[i].invalidate();
    }
}

void TempView::setExhaustHighest(float c)
{
    // could draw a line; update summary text
}
