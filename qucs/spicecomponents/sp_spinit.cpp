/***************************************************************************
                                sp_spinit.cpp
                               ---------------
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "sp_spinit.h"
#include "main.h"
#include "misc.h"


SpiceSpinit::SpiceSpinit()
{
  isEquation = true;
  Type = isComponent;  // Analogue and digital component.
  Description = QObject::tr(".spinit file");

  Texts.append(new Text(0, 0, Description, Qt::darkRed, QucsSettings.largeFontSize));

  x1 = -10; y1 = -9;
  x2 = x1+104; y2 = y1+59;

  tx = 0;
  ty = y2+1;

  Model = ".SPINIT";
  Name  = ".SPINIT";

  Props.append(new Property(".spinit contents", "\nngbehavior=ps\n", true,
                            "Insert .spinit contents"));
}

SpiceSpinit::~SpiceSpinit()
{
}

Component* SpiceSpinit::newOne()
{
  return new SpiceSpinit();
}

Element* SpiceSpinit::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr(".spinit contents");
  BitmapFile = (char *) "sp_options";

  if(getNewOne)  return new SpiceSpinit();
  return 0;
}

QString SpiceSpinit::spice_netlist(bool isXyce)
{
    QString s = "";
    if (isXyce) return s;
    s = Props.at(0)->Value+"\n";
    return s;
}
