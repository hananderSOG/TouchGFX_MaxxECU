#include "EngineView.hpp"
#include <touchgfx/Unicode.hpp>

EngineView::EngineView()
{
}

void EngineView::setupScreen()
{
    // Setup widgets positions and add them
}

void EngineView::tearDownScreen()
{
}

void EngineView::setLambda(float a, float b, float avg)
{
    Unicode::snprintf(lambdaBuf[0], 16, "%.2f", a);
    Unicode::snprintf(lambdaBuf[1], 16, "%.2f", b);
    Unicode::snprintf(lambdaBuf[2], 16, "%.2f", avg);
    // update text areas/bar values
}

void EngineView::setBoost(float target, float wastegate, float duty)
{
    Unicode::snprintf(boostBuf, 16, "%.0f", target);
    // update gauge/bar
}

void EngineView::setIgnitionTiming(float deg)
{
    Unicode::snprintf(ignBuf, 16, "%.1f", deg);
}

void EngineView::setFuelPressure(float kpa)
{
    Unicode::snprintf(fuelPresBuf, 16, "%.0f", kpa);
}

void EngineView::setFuelDuty(float pct)
{
    // update injDutyBar
}

void EngineView::setEthanolContent(float pct)
{
    // update ethanolBar
}

void EngineView::setMAP(float kpa)
{
    // maybe update boostGauge with map-100 calculation?
}

void EngineView::setThrottle(float pct)
{
    // update data grid tile
}
