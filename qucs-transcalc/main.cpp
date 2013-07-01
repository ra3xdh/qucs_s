/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Sun Feb 27 2005
    copyright            : (C) 2005 by Stefan Jahn
    email                : stefan@lkcc.org
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

#include "qucstrans.h"

tQucsSettings QucsSettings;

extern struct TransUnit TransUnits[];


// #########################################################################
// Loads the settings file and stores the settings.
bool loadSettings()
{
    QSettings settings("qucs","qucs");
    settings.beginGroup("QucsTranscalc");
    if(settings.contains("x"))QucsSettings.x=settings.value("x").toInt();
    if(settings.contains("y"))QucsSettings.y=settings.value("y").toInt();
    if(settings.contains("dx"))QucsSettings.dx=settings.value("dx").toInt();
    if(settings.contains("dy"))QucsSettings.dy=settings.value("dy").toInt();
    if(settings.contains("Mode"))QucsSettings.Mode=settings.value("Mode").toString();
    if(settings.contains("FreqUnit"))QucsSettings.freq_unit=settings.value("FreqUnit").toInt();
    if(settings.contains("LengthUnit"))QucsSettings.length_unit=settings.value("LengthUnit").toInt();
    if(settings.contains("ResUnit"))QucsSettings.res_unit=settings.value("ResUnit").toInt();
    if(settings.contains("AngUnit"))QucsSettings.ang_unit=settings.value("AngUnit").toInt();

    settings.endGroup();
    if(settings.contains("font"))QucsSettings.font.fromString(settings.value("font").toString());
    if(settings.contains("Language"))QucsSettings.Language=settings.value("Language").toString();

  return true;
}


// #########################################################################
// Saves the settings in the settings file.
bool saveApplSettings(QucsTranscalc *qucs)
{
    QSettings settings ("qucs","qucs");
    settings.beginGroup("QucsTranscalc");
    settings.setValue("x", qucs->x());
    settings.setValue("y", qucs->y());
    settings.setValue("dx", qucs->width());
    settings.setValue("dy", qucs->height());
    settings.setValue("Mode", qucs->getMode());
    settings.setValue("FreqUnit", TransUnits[0].units[QucsSettings.freq_unit]);
    settings.setValue("LengthUnit", TransUnits[1].units[QucsSettings.length_unit]);
    settings.setValue("ResUnit", TransUnits[2].units[QucsSettings.res_unit]);
    settings.setValue("AngUnit", TransUnits[3].units[QucsSettings.ang_unit]);
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
  QucsSettings.dx = 540;
  QucsSettings.dy = 400;
  QucsSettings.font = QFont("Helvetica", 12);
  QucsSettings.length_unit = 0;
  QucsSettings.res_unit = 0;
  QucsSettings.ang_unit = 0;
  QucsSettings.freq_unit = 0;

  // is application relocated?
  char * var = getenv ("QUCSDIR");
  if (var != NULL) {
    QDir QucsDir = QDir (var);
    QString QucsDirStr = QucsDir.canonicalPath ();
    QucsSettings.BitmapDir =
      QDir::convertSeparators (QucsDirStr + "/share/qucs/bitmaps/");
    QucsSettings.LangDir =
      QDir::convertSeparators (QucsDirStr + "/share/qucs/lang/");
  } else {
    QucsSettings.BitmapDir = BITMAPDIR;
    QucsSettings.LangDir = LANGUAGEDIR;
  }
  QucsSettings.QucsWorkDir.setPath (QDir::homePath()+QDir::convertSeparators ("/.qucs"));
  loadSettings();

  QApplication a(argc, argv);
  a.setFont(QucsSettings.font);

  QTranslator tor( 0 );
  QString lang = QucsSettings.Language;
  if(lang.isEmpty())
    lang = QString(QLocale::system().name());
  tor.load( QString("qucs_") + lang, QucsSettings.LangDir);
  a.installTranslator( &tor );

  QucsTranscalc *qucs = new QucsTranscalc();
  qucs->raise();
  qucs->resize(QucsSettings.dx, QucsSettings.dy); // size and position ...
  qucs->move(QucsSettings.x, QucsSettings.y);     // ... before "show" !!!
  qucs->show();

  qucs->loadFile(QDir::homePath()+"/.qucs/transrc");
  qucs->setMode(QucsSettings.Mode);

  // optional file argument
  if (argc > 1) {
    int _mode = 0;
    QString File = argv[1];
    qucs->loadFile(File,&_mode);
  }

  int result = a.exec();
  saveApplSettings(qucs);
  delete qucs;
  return result;
}
