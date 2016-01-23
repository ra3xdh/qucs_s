/***************************************************************************
                                main.cpp
                               ----------
    begin                : Sat May 28 2005
    copyright            : (C) 2005 by Michael Margraf
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>

#include <QApplication>
#include <QString>
#include <QTextCodec>
#include <QTranslator>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDir>
#include <QFont>
#include <QSettings>
#include "qucslib.h"

tQucsSettings QucsSettings;
QDir UserLibDir;


// #########################################################################
// Loads the settings file and stores the settings.
bool loadSettings()
{
    QSettings settings("qucs","qucs");
    // Qucs Library Tool specific settings
    settings.beginGroup("QucsLib");
    if(settings.contains("x"))QucsSettings.x=settings.value("x").toInt();
    if(settings.contains("y"))QucsSettings.y=settings.value("y").toInt();
    if(settings.contains("dx"))QucsSettings.dx=settings.value("dx").toInt();
    if(settings.contains("dy"))QucsSettings.dy=settings.value("dy").toInt();
    settings.endGroup();
    // Qucs general settings
    if(settings.contains("QucsHomeDir"))
      if(settings.value("QucsHomeDir").toString() != "")
         QucsSettings.QucsHomeDir.setPath(settings.value("QucsHomeDir").toString());
    if(settings.contains("font"))QucsSettings.font.fromString(settings.value("font").toString());
    if(settings.contains("Language"))QucsSettings.Language=settings.value("Language").toString();

  return true;
}


// #########################################################################
// Saves the settings in the settings file.
bool saveApplSettings(QucsLib *qucs)
{
    QSettings settings ("qucs","qucs");
    settings.beginGroup("QucsLib");
    settings.setValue("x", qucs->x());
    settings.setValue("y", qucs->y());
    settings.setValue("dx", qucs->width());
    settings.setValue("dy", qucs->height());
    settings.endGroup();
  return true;

}


// #########################################################################
// ##########                                                     ##########
// ##########                  Program Start                      ##########
// ##########                                                     ##########
// #########################################################################

int main(int argc, char *argv[])
{
  // apply default settings
  QucsSettings.x = 100;
  QucsSettings.y = 50;
  QucsSettings.dx = 600;
  QucsSettings.dy = 350;
  QucsSettings.font = QFont("Helvetica", 12);
  QucsSettings.QucsHomeDir.setPath(QDir::homePath() + "/.qucs");

  // is application relocated?
  char * var = getenv ("QUCSDIR");
  QDir QucsDir;
  if (var != NULL) {
    QucsDir = QDir(QString(var));
    QucsSettings.LangDir =     QucsDir.canonicalPath() + "/share/qucs/lang/";
    QucsSettings.LibDir =      QucsDir.canonicalPath() + "/share/qucs/library/";
  } else {
    QucsSettings.LangDir = LANGUAGEDIR;
    QucsSettings.LibDir = LIBRARYDIR;
  }

  loadSettings();

  UserLibDir.setPath(QucsSettings.QucsHomeDir.canonicalPath() + "/user_lib/");

  QApplication a(argc, argv);
  a.setFont(QucsSettings.font);

  QTranslator tor( 0 );
  QString lang = QucsSettings.Language;
  if(lang.isEmpty())
    lang = QString(QLocale::system().name());
  tor.load( QString("qucs_") + lang, QucsSettings.LangDir);
  a.installTranslator( &tor );

  QucsLib *qucs = new QucsLib();
  qucs->raise();
  qucs->resize(QucsSettings.dx, QucsSettings.dy); // size and position ...
  qucs->move(QucsSettings.x, QucsSettings.y);     // ... before "show" !!!
  qucs->show();

  int result = a.exec();
  saveApplSettings(qucs);
  delete qucs;
  return result;
}
