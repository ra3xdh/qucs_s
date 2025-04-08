#ifndef GEOMETRY_SHAPES_H
#define GEOMETRY_SHAPES_H

#include "concepts.h"
#include <QRect>
#include <cmath>
#include <QDebug>

namespace qucs_s {
namespace geom {

inline bool is_inside_ellipse(const QPoint m, const QRect& bounds, int tolerance)
{
    const auto m_rel_center = (m - bounds.center());
    const auto half_width = bounds.width() / 2.0;
    const auto half_height = bounds.height() / 2.0;

    auto oa = m_rel_center.x() / (half_width + tolerance);
    auto ob = m_rel_center.y() / (half_height + tolerance);

    return oa*oa + ob*ob <= 1.0;
}

inline bool is_near_ellipse(const QPoint m, const QRect& bounds, int tolerance)
{
    const auto m_rel_center = (m - bounds.center());
    const auto half_width = bounds.width() / 2.0;
    const auto half_height = bounds.height() / 2.0;

    // outer ellipse
    auto oa = m_rel_center.x() / (half_width + tolerance);
    auto ob = m_rel_center.y() / (half_height + tolerance);

    auto inside_outer = oa*oa + ob*ob <= 1.0;
    if (!inside_outer) return false;

    // inner ellipse
    auto ia = m_rel_center.x() / (half_width - tolerance);
    auto ib = m_rel_center.y() / (half_height - tolerance);

    return inside_outer && (ia*ia + ib*ib >= 1.0);
}
}
}

#endif