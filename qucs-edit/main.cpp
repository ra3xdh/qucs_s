/***************************************************************************
                                 main.cpp
                                ----------
    begin                : Thu Aug 28 2003
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

#include <qapplication.h>
#include <qstring.h>
#include <qtextcodec.h>
#include <qtranslator.h>
#include <qfile.h>
#include <q3textstream.h>
#include <qmessagebox.h>
#include <qdir.h>
#include <qfont.h>

#include "qucsedit.h"

tQucsSettings QucsSettings;

// #########################################################################
// Loads the settings file and stores the settings.
bool loadSettings()
{
  bool result = true;

  QFile file(QDir::homeDirPath()+QDir::convertSeparators ("/.qucs/editrc"));
  if(!file.open(QIODevice::ReadOnly))
    result = false; // settings file doesn't exist
  else {
    Q3TextStream stream(&file);
    QString Line, Setting;
    while(!stream.atEnd()) {
      Line = stream.readLine();
      Setting = Line.section('=',0,0);
      Line = Line.section('=',1,1);
      if(Setting == "EditWindow") {
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
bool saveApplSettings(QucsEdit *qucs)
{
  if(qucs->x() == QucsSettings.x)
    if(qucs->y() == QucsSettings.y)
      if(qucs->width() == QucsSettings.dx)
	if(qucs->height() == QucsSettings.dy)
	  return true;   // nothing has changed


  QFile file(QDir::homeDirPath()+QDir::convertSeparators ("/.qucs/editrc"));
  if(!file.open(QIODevice::WriteOnly)) {
    QMessageBox::warning(0, QObject::tr("Warning"),
			QObject::tr("Cannot save settings !"));
    return false;
  }

  QString Line;
  Q3TextStream stream(&file);

  stream << "Settings file, Qucs Editor " PACKAGE_VERSION "\n"
	 << "EditWindow=" << qucs->x() << ',' << qucs->y() << ','
	 << qucs->width() << ',' << qucs->height() << '\n';

  file.close();
  return true;
}

// #########################################################################
void showOptions()
{
  fprintf(stdout, QObject::tr("Qucs Editor Version ")+PACKAGE_VERSION+
    QObject::tr("\nVery simple text editor for Qucs\n")+
    QObject::tr("Copyright (C) 2004, 2005 by Michael Margraf\n")+
    QObject::tr("\nUsage:  qucsedit [-r] file\n")+
    QObject::tr("    -h  display this help and exit\n")+
    QObject::tr("    -r  open file read-only\n"));
}


// #########################################################################
// ##########                                                     ##########
// ##########                  Program Start                      ##########
// ##########                                                     ##########
// #########################################################################

int main(int argc, char *argv[])
{
  // apply default settings
  QucsSettings.x = 200;
  QucsSettings.y = 100;
  QucsSettings.dx = 400;
  QucsSettings.dy = 400;
  QucsSettings.font = QFont("Helvetica", 12);

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

  loadSettings();

  QApplication a(argc, argv);
  a.setFont(QucsSettings.font);

  QTranslator tor( 0 );
  QString lang = QucsSettings.Language;
  if(lang.isEmpty())
    lang = QTextCodec::locale();
  tor.load( QString("qucs_") + lang, QucsSettings.LangDir);
  a.installTranslator( &tor );

  bool readOnly = false;
  QString FileName, s;
  for(int i=1; i<argc; i++) {
    s = argv[i];
    if(s.at(0) == '-') {
      if(s.length() != 2) {
	fprintf(stdout, QObject::tr("Too long command line argument!\n\n"));
	showOptions();
	return -1;
      }
      switch(s.at(1).latin1()) {
	case 'r': readOnly = true;
		  break;
	case 'h': showOptions();
		  return 0;
	default :
	  fprintf(stderr, QObject::tr("Wrong command line argument!\n\n"));
	  showOptions();
	  return -1;
      }
    }
    else  if(FileName.isEmpty())  FileName = s;
	  else {
	    fprintf(stderr, QObject::tr("Only one filename allowed!\n\n"));
	    showOptions();
	    return -1;
	  }
  }

  QucsEdit *qucs = new QucsEdit(FileName, readOnly);
  a.setMainWidget(qucs);
  qucs->resize(QucsSettings.dx, QucsSettings.dy); // size and position ...
  qucs->move(QucsSettings.x, QucsSettings.y);     // ... before "show" !!!
  qucs->show();
  int result = a.exec();
  saveApplSettings(qucs);
  return result;
}
