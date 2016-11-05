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
/*!
 * \file main.h
 * \brief Definitions and declarations for the main application.
 */

#ifndef QUCS_MAIN_H
#define QUCS_MAIN_H

#include <QFont>
#include <QColor>
#include <QStringList>
#include <QDir>

#include "wire.h"
#include "node.h"
#include "diagrams/diagram.h"
#include "extsimkernels/spicecompat.h"

class QucsApp;
class Component;

static const double pi = 3.1415926535897932384626433832795029;  /* pi   */

struct tQucsSettings {
  int DefaultSimulator;

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
  QString Qucsator;
  QString QucsatorDir;
  QString QucsatorVar;
  QString Qucsconv;
  QString BinDir;
  QString LangDir;
  QString LibDir;
  QString OctaveDir;  // m-files location
  QString ExamplesDir;
  QString DocDir;

  unsigned int NodeWiring;
  QDir QucsWorkDir;
  QDir QucsHomeDir;
  QDir AdmsXmlBinDir;  // dir of admsXml executable
  QDir AscoBinDir;     // dir of asco executable
  QString NgspiceExecutable;  // Executables of external simulators
  QString XyceExecutable;
  QString XyceParExecutable;
  QString SpiceOpusExecutable;
  QString S4Qworkdir;
  unsigned int NProcs; // Number of processors for Xyce
  QString OctaveExecutable; // OctaveExecutable location
  QString QucsOctave; // OUCS_OCTAVE variable

  // registered filename extensions with program to open the file
  QStringList FileTypes;

  // List of extensions used for spice files
  QStringList spiceExtensions;

  unsigned int numRecentDocs;
  QStringList RecentDocs;

  bool IgnoreFutureVersion;
  bool GraphAntiAliasing;
  bool TextAntiAliasing;
};

extern tQucsSettings QucsSettings;  // extern because nearly everywhere used
extern QucsApp *QucsMain;  // the Qucs application itself
extern QString lastDir;    // to remember last directory for several dialogs
extern QStringList qucsPathList;

bool loadSettings();
bool saveApplSettings();
void qucsMessageOutput(QtMsgType type, const char *msg);

#endif // ifndef QUCS_MAIN_H
