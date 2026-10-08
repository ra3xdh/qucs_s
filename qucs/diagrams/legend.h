#ifndef LEGEND_H
#define LEGEND_H

#include <QRectF>
#include <QPointF>
#include <QSizeF>
#include <QString>

class QPainter;
class Graph;

class Legend {
public:
    Legend();
    ~Legend() = default;

    bool isVisible() const { return m_visible; }
    void setVisible(bool vis) { m_visible = vis; }

    // selection state (for dragging)
    bool isSelected() const { return m_selected; }
    void setSelected(bool sel) { m_selected = sel; }

    // Position relative to diagram origin (cx, cy)
    QPointF position() const { return m_position; }
    void setPosition(const QPointF& pos) { m_position = pos; m_hasCustomPosition = true; }

    bool hasCustomPosition() const { return m_hasCustomPosition; }
    void setHasCustomPosition(bool custom) { m_hasCustomPosition = custom; }

    QSizeF size() const { return m_size; }
    void calculateSize(const QList<Graph*>& graphs);

    void paint(QPainter* painter, const QList<Graph*>& graphs);

    // Hit-testing in diagram-local coordinates
    bool contains(const QPointF& point) const;

    void calculateDefaultPosition(const QRectF& plotRect,
                                  const QList<Graph*>& graphs,
                                  bool is3D);

    void save(QString& s) const;
    bool load(const QString& s);

private:
    bool m_visible;
    bool m_selected;
    bool m_hasCustomPosition;
    QPointF m_position; // Local coordinates relative to (cx, cy)
    QSizeF m_size;

    // strip simulator prefix from variable name
    static QString cleanVarName(const QString& varName);
};

#endif
