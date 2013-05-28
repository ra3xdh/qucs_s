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
#include <Qfile>
#include <QTextStream>
#include <QMessageBox>
#include <QDir>
#include <QFont>

#include "qucstrans.h"

tQucsSettings QucsSettings;
extern QDir QucsWorkDir;
extern struct TransUnit TransUnits[];

// Loads the settings file and stores the settings.
bool loadSettings()
{
  bool result = true;

  QFile file(QDir::homePath()+QDir::convertSeparators ("/.qucs/transrc"));
  if(!file.open(QIODevice::ReadOnly))
    result = false; // settings file doesn't exist
  else {
    QTextStream stream(&file);
    QString Line, Setting;
    while(!stream.atEnd()) {
      Line = stream.readLine();
      Setting = Line.section('=',0,0);
      Line = Line.section('=',1,1);
      if(Setting == "Mode") {
    QucsSettings.Mode = Line.simplified();
      }
      else if(Setting == "Frequency") {
    Line = Line.simplified();
    QucsSettings.freq_unit = QucsTranscalc::translateUnit(Line.toAscii(),0);
      }
      else if(Setting == "Length") {
    Line = Line.simplified();
    QucsSettings.length_unit = QucsTranscalc::translateUnit(Line.toAscii(),1);
      }
      else if(Setting == "Resistance") {
    Line = Line.simplified();
    QucsSettings.res_unit = QucsTranscalc::translateUnit(Line.toAscii(),2);
      }
      else if(Setting == "Angle") {
    Line = Line.simplified();
    QucsSettings.ang_unit = QucsTranscalc::translateUnit(Line.toAscii(),3);
      }
      else if(Setting == "TransWindow") {
	QucsSettings.x  = Line.section(",",0,0).toInt();
	QucsSettings.y  = Line.section(",",1,1).toInt();
	QucsSettings.dx = Line.section(",",2,2).toInt();
	QucsSettings.dy = Line.section(",",3,3).toInt();
	break;
      }
    }
    file.close();
  }

  file.setFileName(QDir::homePath()+QDir::convertSeparators ("/.qucs/qucsrc"));
  if(!file.open(QIODevice::ReadOnly))
    result = true; // qucs settings not necessary
  else {
    QTextStream stream(&file);
    QString Line, Setting;
    while(!stream.atEnd()) {
      Line = stream.readLine();
      Setting = Line.section('=',0,0);
      Line = Line.section('=',1,1).trimmed();
      if(Setting == "Font")
	QucsSettings.font.fromString(Line);
      else if(Setting == "Language")
	QucsSettings.Language = Line;
    }
    file.close();
  }
  return result;
}

// Saves the settings in the settings file.
bool saveApplSettings(QucsTranscalc *qucs)
{
  QFile file(QDir::homePath()+QDir::convertSeparators ("/.qucs/transrc"));
  if(!file.open(QIODevice::WriteOnly)) {
    QMessageBox::warning(0, QObject::tr("Warning"),
			QObject::tr("Cannot save settings !"));
    return false;
  }

  QString Line;
  QTextStream stream(&file);

  stream << "Settings file, QucsTranscalc " PACKAGE_VERSION "\n"
	 << "Mode=" << qucs->getMode() << "\n"
	 << "Frequency=" << TransUnits[0].units[QucsSettings.freq_unit] << "\n"
	 << "Length=" << TransUnits[1].units[QucsSettings.length_unit] << "\n"
	 << "Resistance=" << TransUnits[2].units[QucsSettings.res_unit] << "\n"
	 << "Angle=" << TransUnits[3].units[QucsSettings.ang_unit] << "\n"
	 << "TransWindow=" << qucs->x() << ',' << qucs->y() << ','
	 << qucs->width() << ',' << qucs->height() << '\n';
  qucs->saveModes(stream);

  file.close();
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
  QucsWorkDir.setPath (QDir::homePath()+QDir::convertSeparators ("/.qucs"));
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
