/***************************************************************************
                                qucsdoc.cpp
                               -------------
    begin                : Wed Sep 3 2003
    copyright            : (C) 2003, 2004 by Michael Margraf
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

#include <QFileInfo>

#include "qucsdoc.h"
#include "qucs.h"


QucsDoc::QucsDoc(QucsApp *App_, const QString& Name_) :
  a_DocName(Name_),
  a_DataSet(),
  a_DataDisplay(),
  a_Script(),
  a_SimTime(),
  a_lastSaved(),
  a_Scale(1.0),
  a_App(App_),
  a_DocChanged(false),
  a_SimOpenDpl(false),
  a_SimRunScript(false),
  a_showBias(-1),   // don't show DC bias (currently for "Schematic" only)
  a_GridOn(true),
  a_tmpPosX(0),
  a_tmpPosY(0)
{
  QFileInfo Info(a_DocName);
  if(!a_DocName.isEmpty()) {
    a_DocName = Info.absoluteFilePath();
    QString base = Info.completeBaseName();
    QString ext = Info.suffix();

    if(ext == "m" || ext == "oct")
      a_SimTime = "1";

    a_DataSet = base + ".dat";       // name of the default dataset
    a_Script = base + ".m";          // name of the default script
    if(ext != "dpl")
      a_DataDisplay = base + ".dpl"; // name of default data display
    else {
      a_DataDisplay = base + ".sch"; // name of default schematic
      a_GridOn = false;              // data display without grid (per default)
    }
  }
}

QString QucsDoc::fileSuffix (const QString& Name) {
  QFileInfo Info (Name);
  return Info.suffix();
}

QString QucsDoc::fileSuffix (void) {
  return fileSuffix (a_DocName);
}

QString QucsDoc::fileBase (const QString& Name) {
  QFileInfo Info (Name);
  return Info.baseName ();
}

QString QucsDoc::fileBase (void) {
  return fileBase (a_DocName);
}

