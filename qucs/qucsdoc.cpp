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


QucsDoc::QucsDoc(QucsApp *App_, const QString& Name_)
{
  App = App_;

  GridOn = true;
  DocName = Name_;
  QFileInfo Info(DocName);
  if(!DocName.isEmpty()) {
    DocName = Info.absoluteFilePath();
    QString base = Info.baseName();
    QString ext = Info.suffix();

    if(ext == "m" || ext == "oct")
      SimTime = "1";

    DataSet = base + ".dat";       // name of the default dataset
    Script = base + ".m";          // name of the default script
    if(ext != "dpl")
      DataDisplay = base + ".dpl"; // name of default data display
    else {
      DataDisplay = base + ".sch"; // name of default schematic
      GridOn = false;              // data display without grid (per default)
    }
  }
  SimOpenDpl = true;
  SimRunScript = false;

  DocChanged = false;
  showBias = -1;  // don't show DC bias (currently for "Schematic" only)
  Scale = 1.0;
}

QString QucsDoc::fileSuffix (const QString& Name) {
  QFileInfo Info (Name);
  return Info.suffix();
}

QString QucsDoc::fileSuffix (void) {
  return fileSuffix (DocName);
}

QString QucsDoc::fileBase (const QString& Name) {
  QFileInfo Info (Name);
  return Info.baseName ();
}

QString QucsDoc::fileBase (void) {
  return fileBase (DocName);
}

