/***************************************************************************
                              graphictext.cpp
                             -----------------
    begin                : Mon Nov 24 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "graphictext.h"
#include "graphictextdialog.h"
#include "main.h"
#include "misc.h"
#include "mnemo.h"
#include "one_point.h"
#include "schematic.h"


GraphicText::GraphicText()
{
    Name = "Text ";
    isSelected = false;
    color = QColor(0, 0, 0);
    font = QucsSettings.font;
    cx = cy = 0;
    x1 = x2 = 0;
    y1 = y2 = 0;
    angle = 0;
}

void GraphicText::paint(QPainter* painter) {
    painter->save();

    painter->translate(x1, y1);
    painter->rotate(-angle);
    painter->setPen(color);

    QFont f = font;
    f.setPixelSize(QFontInfo{font}.pixelSize());
    painter->setFont(f);

    QRectF br;
    misc::draw_richtext(painter, 0, 0, text, &br);

    x1 = br.left();
    y1 = br.bottom();
    x2 = br.right();
    y2 = br.top();
    updateCenter();

    if (isSelected) {
        painter->setPen(QPen(Qt::darkGray, 3));
        painter->drawRect(br);
    }

    painter->restore();
}

void GraphicText::paintScheme(Schematic *p)
{
    p->PostPaintEvent(_Rect, x1, y1, x2, y2);
}

Painting* GraphicText::newOne()
{
    return new GraphicText();
}

Element *GraphicText::info(QString &Name, char *&BitmapFile, bool getNewOne)
{
    Name = QObject::tr("Text");
    BitmapFile = (char *) "text";

    if (getNewOne)
        return new GraphicText();
    return 0;
}

bool GraphicText::load(const QString &s)
{
    bool ok;

    QString n;
    n = s.section(' ', 1, 1); // x1
    x1 = n.toInt(&ok);
    if (!ok)
        return false;

    n = s.section(' ', 2, 2); // y1
    y1 = n.toInt(&ok);
    if (!ok)
        return false;

    n = s.section(' ', 3, 3); // Size
    font.setPointSize(n.toInt(&ok));
    if (!ok)
        return false;

    n = s.section(' ', 4, 4); // Color
    color = misc::ColorFromString(n);
    if (!color.isValid())
        return false;

    n = s.section(' ', 5, 5); // Angle
    angle = n.toInt(&ok);
    if (!ok)
        return false;

    text = s.mid(s.indexOf('"') + 1); // Text (can contain " !!!)
    text.truncate(text.length() - 1);
    if (text.isEmpty())
        return false;

    misc::convert2Unicode(text);

    return true;
}

QString GraphicText::save()
{
    QString t = text;
    misc::convert2ASCII(t);

    // The 'Text' property has to be the last within the line !
    QString s = Name + QString::number(x1) + " " + QString::number(y1) + " "
                + QString::number(font.pointSize()) + " " + color.name() + " "
                + QString::number(angle) + " \"" + t + "\"";
    return s;
}

QString GraphicText::saveCpp()
{
    QString t = text;
    misc::convert2ASCII(t);

    QString s = QStringLiteral("new Text (%1, %2, \"%3\", QColor (\"%4\"), %5, %6, %7)")
                    .arg(x1)
                    .arg(y1)
                    .arg(t)
                    .arg(color.name())
                    .arg(font.pointSize())
                    .arg(cos(pi * angle / 180.0))
                    .arg(sin(pi * angle / 180.0));
    s = "Texts.append (" + s + ");";
    return s;
}

QString GraphicText::saveJSON()
{
    QString t = text;
    misc::convert2ASCII(t);

    QString s = QStringLiteral("{\"type\" : \"graphictext\", "
                        "\"x\" : %1, \"y\" : %2, \"s\" : \"%3\", "
                        "\"color\" : \"%4\", \"size\" : %5, \"cos\" : %6, \"sin\" : %7},")
                    .arg(x1)
                    .arg(y1)
                    .arg(t)
                    .arg(color.name())
                    .arg(font.pointSize())
                    .arg(cos(pi * angle / 180.0))
                    .arg(sin(pi * angle / 180.0));
    return s;
}

void GraphicText::MouseMoving(const QPoint& onGrid, Schematic* sch, const QPoint& cursor)
{
    x1 = onGrid.x();
    y1 = onGrid.y();

    // paint cursor scursor.y()mbol
    sch->PostPaintEvent(_Line, cursor.x() + 15, cursor.y() + 15, cursor.x() + 20, cursor.y(), 0, 0, true);
    sch->PostPaintEvent(_Line, cursor.x() + 26, cursor.y() + 15, cursor.x() + 21, cursor.y(), 0, 0, true);
    sch->PostPaintEvent(_Line, cursor.x() + 17, cursor.y() + 8, cursor.x() + 23, cursor.y() + 8, 0, 0, true);
}

bool GraphicText::MousePressing(Schematic *sch)
{
    return Dialog(sch);
}

// Checks if the coordinates x/y point to the painting.
bool GraphicText::getSelected(const QPoint& click, int tolerance)
{
    return boundingRect()
        .marginsAdded(QMargins(tolerance, tolerance, tolerance, tolerance))
        .contains(click);
}

// Rotates around the center.
void GraphicText::rotate() noexcept
{
    angle += 90;
    angle %= 360;
}

void GraphicText::rotate(int rcx, int rcy) noexcept
{
    qucs_s::geom::rotate_point_ccw(x1, y1, rcx, rcy);
    angle += 90;
    angle %= 360;
}

bool GraphicText::Dialog(QWidget *parent)
{
    QFont f(QucsSettings.font); // to avoid wrong text width
    bool changed = false;

    auto d = std::make_unique<GraphicTextDialog>(parent);

    QPalette palette;
    palette.setColor(d->ColorButt->backgroundRole(), color);
    d->ColorButt->setPalette(palette);

    d->TextSize->setText(QString::number(font.pointSize()));
    d->Angle->setText(QString::number(angle));
    QString _Text = text;
    decode_String(_Text); // replace special characters with LaTeX commands
    d->text->setText(_Text);

    if (d->exec() == QDialog::Rejected) {
        return false;
    }

    if (color != d->ColorButt->palette().color(d->ColorButt->backgroundRole())) {
        color = d->ColorButt->palette().color(d->ColorButt->backgroundRole());
        changed = true;
    }
    f.setPointSize(d->TextSize->text().toInt()); // to avoid wrong text width
    if (font.pointSize() != d->TextSize->text().toInt()) {
        font.setPointSize(d->TextSize->text().toInt());
        changed = true;
    }
    int tmp = d->Angle->text().toInt();
    if (angle != tmp) {
        angle = tmp % 360;
        changed = true;
    }

    encode_String(d->text->toPlainText(), _Text); // create special characters
    if (!_Text.isEmpty())
        if (_Text != text) {
            text = _Text;
            changed = true;
        }

    return changed;
}
