/***************************************************************************
                               vafile.h
                              ----------
    begin                : Sun Oct 26 2009
    copyright            : (C) 2009 by Stefan Jahn
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

#ifndef VAFILE_H
#define VAFILE_H

class VerilogA_File_Info {
public:
  VerilogA_File_Info ();
  VerilogA_File_Info (QString, bool isfile = false);
 ~VerilogA_File_Info () {};
  QString parsePorts (QString, int);

public:
  QString ModuleName;
  QString PortNames;
};

#endif
