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

#include <QString>
#include <q3ptrlist.h>

struct SubParameter {
  SubParameter(bool display_, const QString& Name_, const QString& Descr_)
     : display(display_), Name(Name_), Description(Descr_) { Type = ""; };
  SubParameter(bool display_, const QString& Name_, const QString& Descr_,
	       const QString& Type_)
     : display(display_), Name(Name_), Description(Descr_), Type(Type_) {};

  bool display;
  QString Name, Description, Type;
};


class ID_Text : public Painting  {
public:
  ID_Text(int cx_=0, int cy_=0);
 ~ID_Text();

  void paintScheme(Schematic*);
  void getCenter(int&, int&);
  void setCenter(int, int, bool relative=false);

  bool load(const QString&);
  QString save();
  QString saveCpp();
  void paint(ViewPainter*);
  bool getSelected(float, float, float);

  void rotate();
  void mirrorX();
  void mirrorY();
  bool Dialog();

  QString Prefix;
  Q3PtrList<SubParameter> Parameter;
};

#endif
