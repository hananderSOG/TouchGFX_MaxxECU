#pragma once

#include <touchgfx/Widget.hpp>
#include <touchgfx/Color.hpp>
#include <touchgfx/Rect.hpp>

class FillBar : public touchgfx::Widget {
public:
    enum Orientation { HORIZONTAL, VERTICAL };
    FillBar();
    void setup(int x, int y, int w, int h,
               Orientation orientation,
               float minVal, float maxVal,
               float warnThreshold, float critThreshold,
               touchgfx::colortype normalColor,
               touchgfx::colortype warnColor,
               touchgfx::colortype critColor);
    void setValue(float value);
    virtual void draw(const touchgfx::Rect& invalidatedArea) const override;
    virtual touchgfx::Rect getSolidRect() const override;

private:
    float m_value, m_min, m_max, m_warn, m_crit;
    int m_x, m_y, m_w, m_h;
    Orientation m_orient;
    touchgfx::colortype m_normColor, m_warnColor, m_critColor;
};
