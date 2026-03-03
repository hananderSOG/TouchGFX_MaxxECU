#include "ArcGauge.hpp"
#include <touchgfx/Canvas.hpp>
#include <touchgfx/Color.hpp>
#include <cmath>

ArcGauge::ArcGauge()
  : m_value(0), m_min(0), m_max(1), m_redZone(1),
    m_cx(0), m_cy(0), m_radius(0), m_arcWidth(0),
    m_normalColor(0), m_dangerColor(0)
{
}

void ArcGauge::setup(int cx, int cy, int radius, int arcWidth,
                     float minVal, float maxVal,
                     float redZoneStart,
                     touchgfx::colortype normalColor,
                     touchgfx::colortype dangerColor)
{
    m_cx = cx;
    m_cy = cy;
    m_radius = radius;
    m_arcWidth = arcWidth;
    m_min = minVal;
    m_max = maxVal;
    m_redZone = redZoneStart;
    m_normalColor = normalColor;
    m_dangerColor = dangerColor;
    setPosition(cx - radius - arcWidth, cy - radius - arcWidth, (radius+arcWidth)*2, (radius+arcWidth)*2);
}

void ArcGauge::setValue(float value)
{
    if (value < m_min) value = m_min;
    if (value > m_max) value = m_max;
    if (value != m_value) {
        m_value = value;
        invalidate();
    }
}

void ArcGauge::draw(const touchgfx::Rect& invalidatedArea) const
{
    // simple placeholder drawing: draw full circle outline and a partial arc representing value
    touchgfx::Canvas canvas;
    canvas.setCanvasBuffer((uint16_t*)0); // no actual buffer
    // real implementation would use CanvasWidgetRenderer to draw arcs
}

touchgfx::Rect ArcGauge::getSolidRect() const
{
    return getRect();
}
