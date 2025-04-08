/***************************************************************************
                                id_text.h
                               -----------
    begin                : Thu Oct 14 2004
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

#ifndef ID_TEXT_H
#define ID_TEXT_H

#include "painting.h"


struct SubParameter {
  SubParameter(bool display_, const QString& name_, const QString& description_,
	       const QString& type_ = "")
     : display(display_), name(name_), description(description_), type(type_) {};

  bool display;
  QString name, description, type;
};


class ID_Text : public Painting  {
public:
  ID_Text(int x1 = 0, int y1 = 0);

  Painting* newOne() override { /* required by interface but unused */ return nullptr; }

  void paint(QPainter* painter) override;
  void paintScheme(Schematic*) override;

  bool    load(const QString&) override;
  QString save() override;
  QString saveCpp() override;
  QString saveJSON() override;

  bool getSelected(const QPoint& click, int tolerance) override;

  bool rotate(int, int) noexcept override;

  bool Dialog(QWidget* parent = nullptr) override;

  QString prefix;
  std::vector<std::unique_ptr<SubParameter>> subParameters;
};

#endif
