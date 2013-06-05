/***************************************************************************
                                vhdlfile.h
                               ------------
    begin                : Sat Apr 15 2006
    copyright            : (C) 2006 by Michael Margraf
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

#ifndef VHDLFILE_H
#define VHDLFILE_H

#include "component.h"
//Added by qt3to4:
#include <Q3TextStream>


class VHDL_File : public MultiViewComponent  {
public:
  VHDL_File();
 ~VHDL_File() {};
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);

  bool createSubNetlist(QTextStream *);
  QString getErrorText() { return ErrText; }
  QString getSubcircuitFile();

protected:
  QString vhdlCode(int);
  void createSymbol();
  QString loadFile();

  QString EntityName;
  QString ErrText;
  QString TypeNames;
  QString GenTypes;
  QString GenNames;
  QString GenDefs;
};

class VHDL_File_Info {
public:
  VHDL_File_Info();
  VHDL_File_Info(QString, bool isfile = false);
 ~VHDL_File_Info() {};
  QString parsePorts(QString, int);
  QString parseGenerics(QString, int);

public:
  QString EntityName;
  QString PortNames;
  QString TypeNames;
  QString GenTypes;
  QString GenNames;
  QString GenDefs;
};

#endif
