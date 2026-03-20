/// @file SystemCommand.cpp
/// @brief Block for calling a system command after simulation.
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Mar 20, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#include "systemcommand.h"
#include "main.h"

#include <QFontInfo>
#include <QFontMetrics>
#include <QRegularExpression>

SystemCommand::SystemCommand()
{
  isEquation = true;
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr("System Command");

  QFont f = QucsSettings.font;
  f.setWeight(QFont::Light);
  f.setPointSizeF(12.0);
  QFontMetrics  metrics(f, 0);  // use the the screen-compatible metric
  QSize r = metrics.size(0, QObject::tr("System Command"));
  int xb = r.width()  >> 1;
  int yb = r.height() >> 1;

  Lines.append(new qucs::Line(-xb, -yb, -xb,  yb,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-xb,  yb,  xb+3,yb,QPen(Qt::darkBlue,2)));
  Texts.append(new Text(-xb+4,  -yb-3, QObject::tr("System Command"),
      QColor(0,0,0), QFontInfo(f).pixelSize()));

  x1 = -xb-3;  y1 = -yb-5;
  x2 =  xb; y2 =  yb+3;

  tx = x1+4;
  ty = y2+4;
  Model = "CMD";
  Name  = "CMD";

  Props.append(new Property("cmd", "", true, "System command to run after simulation"));
  Props.append(new Property("console", "yes", true, "Open console window [yes, no]"));
  Props.append(new Property("hold", "no", false, "Keep terminal open after execution [yes, no]"));
}

SystemCommand::~SystemCommand()
{
}

Component* SystemCommand::newOne()
{
  return new SystemCommand();
}


Element* SystemCommand::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Command");
  BitmapFile = (char *) "systemcommand";

  if(getNewOne)  return new SystemCommand();
  return 0;
}
