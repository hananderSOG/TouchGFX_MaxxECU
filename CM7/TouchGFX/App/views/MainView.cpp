#include "MainView.hpp"
#include <touchgfx/Utils.hpp>
#include <touchgfx/Unicode.hpp>

MainView::MainView()
    : lastRPM(0), lastSpeed(-1), lastGear(0), lastCoolant(-1), lastOilPressure(-1), lastCanTimeout(false)
{
}

void MainView::setupScreen()
{
    // configure rpm gauge
    rpmGauge.setup(200, 185, 160, 12, 0, 9000, 7500,
                   touchgfx::Color::getColorFrom24BitRGB(0x00,0xE5,0xFF),
                   touchgfx::Color::getColorFrom24BitRGB(0xFF,0x00,0x00));
    add(rpmGauge);

    rpmText.setTypedText(touchgfx::TypedText(T_TYPED_TEXT_EMPTY));
    rpmText.setWildcard(rpmTextBuffer);
    rpmText.setTypedText(touchgfx::TypedText(T_RPM_VALUE));
    rpmText.setXY(200-50,185-20);
    add(rpmText);

    // Additional widget initialization omitted for brevity; real code would set positions,
    // styles, fonts, and add to view container here.
}

void MainView::tearDownScreen()
{
}

void MainView::setRPM(uint16_t rpm)
{
    if (rpm != lastRPM) {
        lastRPM = rpm;
        rpmGauge.setValue(rpm);
        // update text
        Unicode::snprintf(rpmTextBuffer, 6, "%u", rpm);
        rpmText.invalidate();
        rpmGauge.invalidate();
    }
}

void MainView::setSpeed(float kmh)
{
    if (kmh != lastSpeed) {
        lastSpeed = kmh;
        Unicode::snprintf(speedTextBuffer, 8, "%3.0f", kmh);
        speedText.invalidate();
    }
}

void MainView::setGear(uint16_t gear)
{
    if (gear != lastGear) {
        lastGear = gear;
        const char *str = "N";
        if (gear == 0) {
            str = "N";
        } else if (gear > 10) {
            str = "R";
        } else {
            // convert number
            Unicode::snprintf(gearTextBuffer, 4, "%u", gear);
            str = gearTextBuffer;
        }
        gearText.setWildcard(str);
        gearText.invalidate();
    }
}

void MainView::setCoolantTemp(float c)
{
    if (c != lastCoolant) {
        lastCoolant = c;
        // update bar and text
        coolantTempBar.setValue(c);
        coolantTempBar.invalidate();
    }
}

void MainView::setOilPressure(float kpa)
{
    if (kpa != lastOilPressure) {
        lastOilPressure = kpa;
        oilPressureBar.setValue(kpa);
        oilPressureBar.invalidate();
    }
}

void MainView::setWarnings(bool revlimit, bool knock, bool launch, bool antilag, bool shiftcut)
{
    // simple implementation: change pill colors
    bool states[5] = {revlimit, knock, launch, antilag, shiftcut};
    for (int i = 0; i < 5; ++i) {
        if (states[i]) {
            warningPills[i].setColor(touchgfx::Color::getColorFrom24BitRGB(0xFF,0x45,0x00));
            warningTexts[i].invalidate();
            warningAlpha[i].start();
        } else {
            warningPills[i].setColor(touchgfx::Color::getColorFrom24BitRGB(0x1A,0x1A,0x1A));
            warningAlpha[i].stop();
        }
        warningPills[i].invalidate();
    }
}

void MainView::setCANTimeout(bool timeout)
{
    if (timeout != lastCanTimeout) {
        lastCanTimeout = timeout;
        if (timeout) {
            timeoutOverlay.setVisible(true);
            timeoutText.setVisible(true);
            timeoutSubtext.setVisible(true);
        } else {
            timeoutOverlay.setVisible(false);
            timeoutText.setVisible(false);
            timeoutSubtext.setVisible(false);
        }
    }
}
