#include "simulation.h"
#include "settings.h"
#include <vector>
#include <QPainter>

namespace {
const QPen wrongSimulatorPen{Qt::gray};
// "Air" around a simulation component title.
constexpr QMargins label_margins{5, 5, 5, 5};
// Height of component "box". See 'drawSymbol()' below for more about the "box"
constexpr QPoint box_height{5, 5};

// Wraps simulation label text on spaces to make it consist of multiple
// lines of almost the same length
QString wrapLabel(const QString &label)
{
    // Acting here like this: given a desired line length L, take every L-th
    // character, find first space before and first space after it, and then
    // replace the closest one with linebreak.

    constexpr int line_len_hint = 14;
    QString wrapped = label.trimmed();

    int prev_break = 0;
    for (int curr_ix = line_len_hint; curr_ix < wrapped.length(); curr_ix += line_len_hint) {
        const int not_found = -1;

        int prev_space = not_found;
        for (int j = curr_ix; j > prev_break; j--) {
            if (wrapped[j].isSpace()) {
                prev_space = j;
                break;
            }
        }

        int next_space = not_found;
        for (int j = curr_ix + 1; j < wrapped.length(); j++) {
            if (wrapped[j].isSpace()) {
                next_space = j;
                break;
            }
        }

        // Not a single space could be found, thus current string segment
        // cannot be broken into lines and we can exit early
        if (prev_space == not_found && next_space == not_found) {
            return wrapped;
        }

        // Locate the place to wrap the string
        if (prev_space != not_found && next_space != not_found) {
            // find the closest space
            curr_ix = curr_ix - prev_space > next_space - curr_ix
                    ? next_space
                    : prev_space;
        } else if (prev_space != not_found) {
            curr_ix = prev_space;
        } else if (next_space != not_found) {
            curr_ix = next_space;
        }

        wrapped[curr_ix] = '\n';
        prev_break = curr_ix;
    }
    return wrapped;
}

QRect selectionRect(const QRect& label_bounds)
{
    auto selection_rect = label_bounds.marginsAdded({3, 3, 3, 3});
    selection_rect.setBottomRight(selection_rect.bottomRight() + box_height);
    return selection_rect;
}

QFont labelFont()
{
    auto label_font = _settings::Get().item<QFont>("font");
    label_font.setWeight(QFont::DemiBold);
    label_font.setPointSizeF(_settings::Get().item<double>("LargeFontSize"));
    return label_font;
}
} // namespace

namespace qucs::component {

QPen SimulationComponent::pen() const
{
    auto default_sim      = _settings::Get().item<int>("DefaultSimulator");
    auto correctSimulator = (Simulator & default_sim) == default_sim;

    return correctSimulator ? QPen{color(), 2, Qt::SolidLine, Qt::FlatCap}
                            : wrongSimulatorPen;
}

void SimulationComponent::updateComponentBounds(const QRect& label_bounds)
{
    auto sr = selectionRect(label_bounds);
    x1 = sr.top();
    y1 = sr.left();
    x2 = sr.right();
    y2 = sr.bottom();
    tx = 0;
    ty = y2;
}


void SimulationComponent::initSymbol(const QString &label)
{
    label_text = wrapLabel(label);
    updateComponentBounds(QRect{{0,0}, QFontMetrics{labelFont()}.size(0, label_text)});
}


void SimulationComponent::drawSymbol(QPainter *p)
{
    const auto label_font = labelFont();

    p->save();
    p->setPen(pen());
    p->setFont(label_font);
    QRect label_bounds;
    p->drawText(0, 0, 0, 0, Qt::TextDontClip, label_text, &label_bounds);

    // Simulation component look like an isometric box
    // or a brick, with a label on its top side, being
    // observed from some strange angle, like this:
    //   B                 C
    //   +-----------------+
    //   |                 |`. E
    //   |  L  A  B  E  L  | |
    //   |                 | |
    // A +-----------------+D|
    //    `_________________`!
    //    G                   F

    const QRect ABCD = label_bounds.marginsAdded(label_margins);
    p->drawRect(ABCD);

    const std::vector<QPoint> CEFGA{
      ABCD.topRight(),
      ABCD.topRight()    + box_height,
      ABCD.bottomRight() + box_height,
      ABCD.bottomLeft()  + box_height,
      ABCD.bottomLeft()
    };
    p->drawPolyline(CEFGA.data(), CEFGA.size());

    // DF
    p->drawLine(ABCD.bottomRight(), ABCD.bottomRight() + box_height);
    p->restore();

    // Changing component properties from within the drawing routine
    // IMO is bad and semantically wrong, but this is the only way
    // to account changes made to font used to draw it. If user changes
    // the font, simulation component has to be drawn according to
    // these changes. And as the whole component appearence is built
    // around its label text, other parts of component has to be updated
    // when the text changes. E.g. when user makes font larger, we want
    // component selection box to reflect this change.
    updateComponentBounds(ABCD);
}

} // namespace qucs::component