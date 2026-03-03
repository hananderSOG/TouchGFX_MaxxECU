#include "FillBar.hpp"
#include <touchgfx/Canvas.hpp>

FillBar::FillBar()
    : m_value(0), m_min(0), m_max(1), m_warn(0), m_crit(0),
      m_x(0), m_y(0), m_w(0), m_h(0), m_orient(HORIZONTAL),
      m_normColor(0), m_warnColor(0), m_critColor(0)
{
}

void FillBar::setup(int x, int y, int w, int h,
                    Orientation orientation,
                    float minVal, float maxVal,
                    float warnThreshold, float critThreshold,
                    touchgfx::colortype normalColor,
                    touchgfx::colortype warnColor,
                    touchgfx::colortype critColor)
{
    m_x = x;
    m_y = y;
    m_w = w;
    m_h = h;
    m_orient = orientation;
    m_min = minVal;
    m_max = maxVal;
    m_warn = warnThreshold;
    m_crit = critThreshold;
    m_normColor = normalColor;
    m_warnColor = warnColor;
    m_critColor = critColor;
    setPosition(x, y, w, h);
}

void FillBar::setValue(float value)
{
    if (value < m_min) value = m_min;
    if (value > m_max) value = m_max;
    if (value != m_value) {
        m_value = value;
        invalidate();
    }
}

void FillBar::draw(const touchgfx::Rect& invalidatedArea) const
{
    // placeholder: do nothing
}

touchgfx::Rect FillBar::getSolidRect() const
{
    return getRect();
}
