#pragma once

#include <touchgfx/containers/Container.hpp>
#include <touchgfx/widgets/TextArea.hpp>
#include <touchgfx/widgets/Box.hpp>
#include "widgets/FillBar.hpp"
#include "../alarm/AlarmOverlay.hpp"

class TempViewBase : public touchgfx::Container {
public:
    virtual void setupScreen() {}
    virtual void tearDownScreen() {}
};

class TempView : public TempViewBase {
public:
    TempView();
    virtual ~TempView() {}
    void setupScreen() override;
    void tearDownScreen() override;

    void setCoolantTemp(float c);
    void setIntakeTemp(float c);
    void setOilTemp(float c);
    void setECUTemp(float c);
    void setEGT(const float *temps, int count);
    void setExhaustHighest(float c);

private:
    // tiles
    touchgfx::TextArea coolantText;
    touchgfx::TextArea intakeText;
    touchgfx::TextArea oilText;
    touchgfx::TextArea ecuText;
    touchgfx::TextArea highestText;
    touchgfx::TextArea spreadText;
    touchgfx::TextArea batteryText;
    touchgfx::TextArea egtBars[8];

    wchar_t buf[16];

    // alarm overlay component
    AlarmOverlay m_alarmOverlay;

public:
    void updateAlarmOverlay() { m_alarmOverlay.tick(); }
};
