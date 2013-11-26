/***************************************************************************
                                main.cpp
                               ----------
    begin                : Thu Jun 24  2004
    copyright            : (C) 2004 by Michael Margraf
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
#include <q3textstream.h>
#include <QMessageBox>
#include <QDir>
#include <QFont>
#include <QTextCodec>

#include "qucshelp.h"

QDir QucsHelpDir; // directory to find helps files
tQucsSettings QucsSettings; // application settings

// #########################################################################
// Loads the settings file and stores the settings.
bool loadSettings()
{
  bool result = true;

  QFile file(QDir::homeDirPath()+QDir::convertSeparators ("/.qucs/helprc"));
  if(!file.open(QIODevice::ReadOnly))
    result = false; // settings file doesn't exist
  else {
    Q3TextStream stream(&file);
    QString Line, Setting;
    while(!stream.atEnd()) {
      Line = stream.readLine();
      Setting = Line.section('=',0,0);
      Line = Line.section('=',1,1);
      if(Setting == "HelpWindow") {
	QucsSettings.x  = Line.section(",",0,0).toInt();
	QucsSettings.y  = Line.section(",",1,1).toInt();
	QucsSettings.dx = Line.section(",",2,2).toInt();
	QucsSettings.dy = Line.section(",",3,3).toInt();
	break; }
    }
    file.close();
  }

  file.setName(QDir::homeDirPath()+QDir::convertSeparators ("/.qucs/qucsrc"));
  if(!file.open(QIODevice::ReadOnly))
    result = true; // qucs settings not necessary
  else {
    Q3TextStream stream(&file);
    QString Line, Setting;
    while(!stream.atEnd()) {
      Line = stream.readLine();
      Setting = Line.section('=',0,0);
      Line = Line.section('=',1,1).stripWhiteSpace();
      if(Setting == "Font")
	QucsSettings.font.fromString(Line);
      else if(Setting == "Language")
	QucsSettings.Language = Line;
    }
    file.close();
  }
  return result;
}

// #########################################################################
// Saves the settings in the settings file.
bool saveApplSettings(QucsHelp *qucs)
{
  if(qucs->x() == QucsSettings.x)
    if(qucs->y() == QucsSettings.y)
      if(qucs->width() == QucsSettings.dx)
	if(qucs->height() == QucsSettings.dy)
	  return true;   // nothing has changed


  QFile file(QDir::homeDirPath()+QDir::convertSeparators ("/.qucs/helprc"));
  if(!file.open(QIODevice::WriteOnly)) {
    QMessageBox::warning(0, QObject::tr("Warning"),
			QObject::tr("Cannot save settings !"));
    return false;
  }

  QString Line;
  Q3TextStream stream(&file);

  stream << "Settings file, Qucs Help System " PACKAGE_VERSION "\n"
	 << "HelpWindow=" << qucs->x() << ',' << qucs->y() << ','
	 << qucs->width() << ',' << qucs->height() << '\n';

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
  QucsSettings.x = 60;
  QucsSettings.y = 30;
  QucsSettings.dx = 640;
  QucsSettings.dy = 400;
  QucsSettings.font = QFont("Helvetica", 12);

  // is application relocated?
  char * var = getenv ("QUCSDIR");
  if (var != NULL) {
    QDir QucsDir = QDir (var);
    QString QucsDirStr = QucsDir.canonicalPath ();
    QucsSettings.DocDir =
      QDir::convertSeparators (QucsDirStr + "/share/qucs/docs/");
    QucsSettings.LangDir =
      QDir::convertSeparators (QucsDirStr + "/share/qucs/lang/");
  } else {
    QucsSettings.DocDir = DOCDIR;
    QucsSettings.LangDir = LANGUAGEDIR;
  }

  loadSettings();

  QApplication a(argc, argv);
  a.setFont(QucsSettings.font);

  QTranslator tor( 0 );
  QString locale = QucsSettings.Language;
  if(locale.isEmpty())
    locale = QTextCodec::locale();
  tor.load( QString("qucs_") + locale, QucsSettings.LangDir);
  a.installTranslator( &tor );

  QucsHelpDir = QucsSettings.DocDir + locale;
  if (!QucsHelpDir.exists () || !QucsHelpDir.isReadable ()) {
    int p = locale.find ('_');
    if (p != -1) {
       QucsHelpDir = QucsSettings.DocDir + locale.left (p);
      if (!QucsHelpDir.exists () || !QucsHelpDir.isReadable ()) {
         QucsHelpDir = QucsSettings.DocDir + "en";
      }
    }
    else QucsHelpDir = QucsSettings.DocDir + "en";
  }

  QString Page;
  if(argc > 1) Page = argv[1];

  QucsHelp *qucs = new QucsHelp(Page);
  a.setMainWidget(qucs);
  qucs->resize(QucsSettings.dx, QucsSettings.dy); // size and position ...
  qucs->move(QucsSettings.x, QucsSettings.y);     // ... before "show" !!!
  qucs->show();
  int result = a.exec();
  saveApplSettings(qucs);
  return result;
}
