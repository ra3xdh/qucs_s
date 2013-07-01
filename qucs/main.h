/***************************************************************************
                                  main.h
                                 --------
    begin                : Mon May 24  2004
    copyright            : (C) 2003 by Michael Margraf
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

#ifndef MAIN_H
#define MAIN_H

#include <qfont.h>
#include <qcolor.h>
#include <q3ptrlist.h>
#include <qstringlist.h>

#include "wire.h"
#include "node.h"
#include "diagrams/diagram.h"
#include <QSettings>

class QucsApp;
class Component;

// constants may be missing on windows systems
#include <math.h>
#ifndef M_PI
#define M_PI     3.1415926535897932384626433832795029
#endif

struct tQucsSettings {
  int x, y, dx, dy;    // position and size of main window
  QFont font;
  float largeFontSize;
  QColor BGColor;      // background color of view area
  QString Language;

  // syntax highlighting
  QColor Comment, String, Integer, Real, Character, Type,
    Attribute, Directive, Task;

  unsigned int maxUndo;    // size of undo stack
  QString Editor;
  QString BinDir;
  QString BitmapDir;
  QString LangDir;
  QString LibDir;
  QString AscoDir;
  QString OctaveDir;
  QString ExamplesDir;
  QString DocDir;

  unsigned int NodeWiring;
  QDir QucsWorkDir;
  QDir QucsHomeDir;
  QDir OctaveBinDir;
  // registered filename extensions with program to open the file
  QStringList FileTypes;
};

extern tQucsSettings QucsSettings;  // extern because nearly everywhere used

extern QucsApp *QucsMain;  // the Qucs application itself
extern QString lastDir;    // to remember last directory for several dialogs

bool saveApplSettings(QucsApp*);

QString complexRect(double, double, int Precision=3);
QString complexDeg (double, double, int Precision=3);
QString complexRad (double, double, int Precision=3);
QString StringNum  (double, char form='g', int Precision=3);
void    str2num    (const QString&, double&, QString&, double&);
QString num2str    (double);
QString StringNiceNum(double);
void    convert2Unicode(QString&);
void    convert2ASCII(QString&);
QString properName(const QString&);
QString properAbsFileName(const QString&);
QString properFileName(const QString&);
bool    VHDL_Time(QString&, const QString&);
bool    VHDL_Delay(QString&, const QString&);
bool    Verilog_Time(QString&, const QString&);
bool    Verilog_Delay(QString&, const QString&);
QString Verilog_Param(const QString);
bool    checkVersion(QString&);

#endif
