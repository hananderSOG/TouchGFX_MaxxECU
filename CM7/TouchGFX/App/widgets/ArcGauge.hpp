#pragma once

#include <touchgfx/Widget.hpp>
#include <touchgfx/Color.hpp>
#include <touchgfx/Rect.hpp>

/**
 * Simple circular arc gauge widget; color gradient and red zone handled in draw().
 */
class ArcGauge : public touchgfx::Widget {
public:
    ArcGauge();
    void setup(int cx, int cy, int radius, int arcWidth,
               float minVal, float maxVal,
               float redZoneStart,
               touchgfx::colortype normalColor,
               touchgfx::colortype dangerColor);
    void setValue(float value);
    virtual void draw(const touchgfx::Rect& invalidatedArea) const override;
    virtual touchgfx::Rect getSolidRect() const override;

private:
    float m_value, m_min, m_max, m_redZone;
    int m_cx, m_cy, m_radius, m_arcWidth;
    touchgfx::colortype m_normalColor, m_dangerColor;
};
