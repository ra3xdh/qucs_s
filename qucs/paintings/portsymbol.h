/***************************************************************************
                               portsymbol.h
                              --------------
    begin                : Sun Sep 5 2004
    copyright            : (C) 2004 by Michael Margraf
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

#ifndef PORTSYMBOL_H
#define PORTSYMBOL_H

#include "painting.h"


class PortSymbol : public Painting  {
public:
  PortSymbol(int cx_=0, int cy_=0, const QString& numberStr_="1",
                                   const QString& nameStr_="");

  void paint(QPainter* painter) override;
  void paintScheme(Schematic*) override;

  Painting* newOne() override;

  bool load(const QString&) override;
  QString save() override;
  QString saveCpp() override;
  QString saveJSON() override;

  bool getSelected(const QPoint& click, int tolerance) override;

  void MouseMoving(const QPoint& onGrid, Schematic* sch, const QPoint& cursor) override;
  bool MousePressing(Schematic*sch) override;

  void  rotate() noexcept override;
  void  rotate(int, int) noexcept override;
  void  mirrorX() noexcept override;
  void  mirrorY() noexcept override;

  bool Dialog(QWidget *Doc) override;

  QString numberStr, nameStr;
private:
  int angle;
};

#endif
