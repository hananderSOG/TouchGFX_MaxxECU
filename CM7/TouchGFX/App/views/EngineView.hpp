#pragma once

#include <touchgfx/containers/Container.hpp>
#include <touchgfx/widgets/TextArea.hpp>
#include <touchgfx/widgets/Box.hpp>
#include <touchgfx/widgets/canvas/Canvas.hpp>
#include "widgets/ArcGauge.hpp"
#include "widgets/FillBar.hpp"
#include "../alarm/AlarmOverlay.hpp"

class EngineViewBase : public touchgfx::Container {
public:
    virtual void setupScreen() {}
    virtual void tearDownScreen() {}
};

class EngineView : public EngineViewBase {
public:
    EngineView();
    virtual ~EngineView() {}
    void setupScreen() override;
    void tearDownScreen() override;

    void setLambda(float a, float b, float avg);
    void setBoost(float target, float wastegate, float duty);
    void setIgnitionTiming(float deg);
    void setFuelPressure(float kpa);
    void setFuelDuty(float pct);
    void setEthanolContent(float pct);
    void setMAP(float kpa);
    void setThrottle(float pct);

private:
    // widgets declarations (placeholders)
    ArcGauge boostGauge;
    FillBar boostTargetMarker; // maybe use Canvas
    FillBar lambdaABars[2];
    ArcGauge ignitionGauge; // reuse for numeric display or simple textbox
    FillBar injDutyBar;
    FillBar ethanolBar;
    touchgfx::TextArea fuelPressureText;
    touchgfx::TextArea wgPressureText;
    touchgfx::TextArea boostSolText;
    touchgfx::TextArea dataGridTiles[6];
    
    // buffers
    wchar_t lambdaBuf[3][16];
    wchar_t boostBuf[16];
    wchar_t ignBuf[16];
    wchar_t fuelPresBuf[16];
    wchar_t wgBuf[16];
    wchar_t boostSolBuf[16];
    wchar_t gridBuf[6][16];

    // alarm overlay component
    AlarmOverlay m_alarmOverlay;

public:
    void updateAlarmOverlay() { m_alarmOverlay.tick(); }
};
