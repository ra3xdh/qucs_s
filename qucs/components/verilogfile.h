/***************************************************************************
                               verilogfile.h
                               -------------
    begin                : Sat Mar 31 2007
    copyright            : (C) 2007 by Stefan Jahn
    email                : stefa@lkcc.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef VERILOGFILE_H
#define VERILOGFILE_H

#include "component.h"
//Added by qt3to4:
#include <Q3TextStream>


class Verilog_File : public MultiViewComponent  {
public:
  Verilog_File();
 ~Verilog_File() {};
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);

  bool createSubNetlist(Q3TextStream *);
  QString getErrorText() { return ErrText; }
  QString getSubcircuitFile();

protected:
  QString verilogCode(int);
  void createSymbol();
  QString loadFile();

  QString ModuleName;
  QString ErrText;
};

class Verilog_File_Info {
public:
  Verilog_File_Info();
  Verilog_File_Info(QString, bool isfile = false);
 ~Verilog_File_Info() {};
  QString parsePorts(QString, int);

public:
  QString ModuleName;
  QString PortNames;
};

#endif
