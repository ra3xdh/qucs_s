#include <QRegularExpression>
#include "legend.h"
#include "graph.h"
#include "main.h"

#include <QPainter>
#include <QFontMetrics>

Legend::Legend() 
    : m_visible(false)
    , m_selected(false)
    , m_hasCustomPosition(false)
    , m_position(0, 0)
    , m_size(0, 0)
{
}

QString Legend::cleanVarName(const QString& varName)
{
    QString name = varName;
    int slashPos = name.indexOf('/');
    if (slashPos >= 0) {
        name = name.mid(slashPos + 1);
    }
    return name;
}

void Legend::calculateSize(const QList<Graph*>& graphs)
{
    QFontMetrics metrics(QucsSettings.font, nullptr);
    int lineSpacing = metrics.lineSpacing();
    int padding = 8;
    int sampleLength = 24;
    int sampleGap = 8;
    
    int maxTextWidth = 0;
    int count = 0;
    
    for (Graph* g : graphs) {
        if (!g || g->Var.isEmpty()) continue;
        
        QString name = cleanVarName(g->Var);
        int w = metrics.boundingRect(name).width();
        if (w > maxTextWidth) maxTextWidth = w;
        count++;
    }
    
    if (count == 0) {
        m_size = QSizeF(0, 0);
        return;
    }
    
    m_size = QSizeF(
        padding + sampleLength + sampleGap + maxTextWidth + padding,
        padding + count * lineSpacing + padding
    );
}

void Legend::calculateDefaultPosition(const QRectF& plotRect,
                                      const QList<Graph*>& graphs,
                                      bool is3D)
{
    calculateSize(graphs);
    if (m_size.isEmpty()) return;

    // For both 2D and 3D Cartesian diagrams, place outside to the right of the diagram,
    // aligned with the top of the plot, so it never obstructs graph data curves.
    int margin = is3D ? 30 : 20;
    m_position = QPointF(
        plotRect.right() + margin,
        plotRect.top()
    );
}

void Legend::paint(QPainter* painter, const QList<Graph*>& graphs)
{
    if (!m_visible) return;
    calculateSize(graphs);
    if (m_size.isEmpty()) return;
    
    painter->save();

    QRectF rect(m_position, m_size);
    QColor bgColor(255, 255, 255, m_selected ? 250 : 225);
    QPen borderPen(m_selected ? QColor(0, 102, 204) : Qt::black,
                   m_selected ? 2 : 1,
                   m_selected ? Qt::DashLine : Qt::SolidLine);
    painter->setPen(borderPen);
    painter->setBrush(bgColor);
    painter->drawRect(rect);

    QFontMetrics metrics(QucsSettings.font, nullptr);
    painter->setFont(QucsSettings.font);
    int lineSpacing = metrics.lineSpacing();
    int padding = 8;
    int sampleLength = 24;
    int sampleGap = 8;
    
    int y = int(m_position.y()) + padding;
    
    for (Graph* g : graphs) {
        if (!g || g->Var.isEmpty()) continue;
        
        QString name = cleanVarName(g->Var);

        QPen pen(g->Color, g->Thick > 0 ? g->Thick : 1, Qt::SolidLine);

        if (g->Style == GRAPHSTYLE_DASH) {
            pen.setDashPattern({10.0, 6.0});
        } else if (g->Style == GRAPHSTYLE_DOT) {
            pen.setDashPattern({2.0, 4.0});
        } else if (g->Style == GRAPHSTYLE_LONGDASH) {
            pen.setDashPattern({24.0, 8.0});
        }
        
        painter->setPen(pen);
        
        int sampleY = y + lineSpacing / 2;
        int sampleX = int(m_position.x()) + padding;

        if (g->Style == GRAPHSTYLE_STAR) {
            painter->save();
            painter->translate(sampleX + sampleLength / 2, sampleY);
            painter->drawLine(-5, 0, 5, 0);
            painter->rotate(60);
            painter->drawLine(-5, 0, 5, 0);
            painter->rotate(-120);
            painter->drawLine(-5, 0, 5, 0);
            painter->restore();
        } else if (g->Style == GRAPHSTYLE_CIRCLE) {
            painter->drawEllipse(QPointF(sampleX + sampleLength / 2, sampleY), 4, 4);
        } else if (g->Style == GRAPHSTYLE_ARROW) {
            int ax = sampleX + sampleLength / 2;
            painter->drawLine(ax, sampleY + 4, ax, sampleY - 4);
            painter->drawLine(ax - 4, sampleY - 2, ax, sampleY - 4);
            painter->drawLine(ax + 4, sampleY - 2, ax, sampleY - 4);
        } else {
            painter->drawLine(sampleX, sampleY, sampleX + sampleLength, sampleY);
        }

        painter->setPen(Qt::black);
        painter->drawText(
            sampleX + sampleLength + sampleGap,
            y + metrics.ascent(),
            name
        );
        
        y += lineSpacing;
    }
    
    painter->restore();
}

bool Legend::contains(const QPointF& point) const
{
    if (!m_visible) return false;
    return QRectF(m_position, m_size).contains(point);
}

void Legend::save(QString& s) const
{
    s += QString("  <legend vis=\"%1\" x=\"%2\" y=\"%3\"/>\n")
         .arg(m_visible ? 1 : 0)
         .arg(static_cast<int>(m_position.x()))
         .arg(static_cast<int>(m_position.y()));
}

bool Legend::load(const QString& s)
{
    QRegularExpression rx("vis=\"(\\d+)\"\\s+x=\"(-?\\d+)\"\\s+y=\"(-?\\d+)\"");
    QRegularExpressionMatch match = rx.match(s);
    if (match.hasMatch()) {
        m_visible = (match.captured(1).toInt() != 0);
        m_position = QPointF(match.captured(2).toDouble(), match.captured(3).toDouble());
        m_hasCustomPosition = true;
        return true;
    }
    return false;
}
