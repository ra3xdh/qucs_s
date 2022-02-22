/***************************************************************************
                                  misc.h
                                 --------
    begin                : Wed Nov 12 2004
    copyright            : (C) 2014 by YodaLee
    email                : lc85301@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef MISC_H
#define MISC_H

/*!
 * \file misc.h
 * \Declaration of some miscellaneous function
 */

class QString;

#include <QWidget>
#include <QPalette>
#include <QIcon>
#include <QFileInfo>
#include <QPushButton>

#include <stdint.h>

#define Q_UINT32 uint32_t

namespace misc {
  QString complexRect(double, double, int Precision=3);
  QString complexDeg (double, double, int Precision=3);
  QString complexRad (double, double, int Precision=3);
  QString StringNum  (double, char form='g', int Precision=3);
  void    str2num    (const QString&, double&, QString&, double&);
  QString num2str    (double);
  QString StringNiceNum(double);
  void    convert2Unicode(QString&);
  void    convert2ASCII(QString&);
  QString properName(const QString&);
  QString properAbsFileName(const QString&);
  QString properFileName(const QString&);
  bool    VHDL_Time(QString&, const QString&);
  bool    VHDL_Delay(QString&, const QString&);
  bool    Verilog_Time(QString&, const QString&);
  bool    Verilog_Delay(QString&, const QString&);
  QString Verilog_Param(const QString);
  bool    checkVersion(QString&);

  inline const QColor getWidgetForegroundColor(const QWidget *q)
  { return q->palette().color(q->foregroundRole()); }

  inline const QColor getWidgetBackgroundColor(const QWidget *q)
  { return q->palette().color(q->backgroundRole()); }

  inline void setWidgetForegroundColor(QWidget *q, const QColor &c)
  { QPalette p = q->palette(); p.setColor(q->foregroundRole(), c); q->setPalette(p); }

  inline void setWidgetBackgroundColor(QWidget *q, const QColor &c)
  { QPalette p = q->palette(); p.setColor(q->backgroundRole(), c); q->setPalette(p); }

  inline void setPickerColor(QPushButton *p, const QColor &c)
    {
        // set color, to be able to get it later
        setWidgetBackgroundColor(p, c);

        // draw pixmap, background color is not being rendered on some platforms
        QPixmap pixmap(35, 10);
        pixmap.fill(c);
        QIcon icon(pixmap);
        p->setIcon(icon);
        p->setIconSize(pixmap.rect().size());
    }

  QString getIconPath(const QString &file);

  bool isDarkTheme();
}

/*! handle the application version string
 *
 *  loosely modeled after the standard Semantic Versioning
 */
class VersionTriplet {
 public:
  VersionTriplet();
  VersionTriplet(const QString&);

  bool operator==(const VersionTriplet& v2);
  bool operator>(const VersionTriplet& v2);
  bool operator<(const VersionTriplet& v2);
  bool operator>=(const VersionTriplet& v2);
  bool operator<=(const VersionTriplet& v2);

  QString toString();

 private:
  int major, minor, patch;
};

#endif
