/***************************************************************************
                              xyce_script.cpp
                               ------------
    begin                : Wed Jun 22 2016
    copyright            : (C) 2016 by Vadim Kuznetsov
    email                : ra3xdh@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "xyce_script.h"


XyceScript::XyceScript()
{
  isSimulation = true;
  Description = QObject::tr("XYCE script");
  Simulator = spicecompat::simXyce;
  initSymbol(Description);
  Model = ".XYCESCR";
  Name  = "XYCESCR";
  SpiceModel = "XYCESCR";

  // The index of the first 4 properties must not changed. Used in recreate().
  Props.append(new Property("SpiceCode", "\n"
                            ".AC LIN 2000 100 10MEG\n"
                            ".PRINT AC format=raw file=ac.txt V(1)", true,
                                         "Insert spice code here"));
  Props.append(new Property("","",false,"Vars to plot"));
  Props.append(new Property("Outputs","ac.txt",false,"Extra outputs to parse"));

}

XyceScript::~XyceScript()
{
}

Component* XyceScript::newOne()
{
  return new XyceScript();
}

Element* XyceScript::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("XYCE script");
  BitmapFile = (char *) "xyce_script";

  if(getNewOne)  return new XyceScript();
  return 0;
}

QString XyceScript::spice_netlist(spicecompat::SpiceDialect dialect /* = spicecompat::SPICEDefault */)
{
    QString s = "";
    if (dialect != spicecompat::SPICEXyce) return s;
    s = Props.at(0)->Value+"\n";
    return s;
}
