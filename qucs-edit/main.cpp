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

#include "qucsedit.h"

tQucsSettings QucsSettings;

// #########################################################################
// Loads the settings file and stores the settings.
bool loadSettings()
{
    QSettings settings("qucs","qucs");
    settings.beginGroup("QucsEdit");
    if(settings.contains("x"))QucsSettings.x=settings.value("x").toInt();
    if(settings.contains("y"))QucsSettings.y=settings.value("y").toInt();
    if(settings.contains("dx"))QucsSettings.dx=settings.value("dx").toInt();
    if(settings.contains("dy"))QucsSettings.dy=settings.value("dy").toInt();
    settings.endGroup();
    if(settings.contains("font"))QucsSettings.font.fromString(settings.value("font").toString());
    if(settings.contains("Language"))QucsSettings.Language=settings.value("Language").toString();

  return true;
}


// #########################################################################
// Saves the settings in the settings file.
bool saveApplSettings(QucsEdit *qucs)
{
    QSettings settings ("qucs","qucs");
    settings.beginGroup("QucsEdit");
    settings.setValue("x", qucs->x());
    settings.setValue("y", qucs->y());
    settings.setValue("dx", qucs->width());
    settings.setValue("dy", qucs->height());
    settings.endGroup();
  return true;

}


// #########################################################################
void showOptions()
{
    fprintf(stdout, "%s", qPrintable( QObject::tr("Qucs Editor Version ")+PACKAGE_VERSION+
    QObject::tr("\nVery simple text editor for Qucs\n")+
    QObject::tr("Copyright (C) 2004, 2005 by Michael Margraf\n")+
    QObject::tr("\nUsage:  qucsedit [-r] file\n")+
    QObject::tr("    -h  display this help and exit\n")+
    QObject::tr("    -r  open file read-only\n")));
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
    lang = QString(QLocale::system().name());
  tor.load( QString("qucs_") + lang, QucsSettings.LangDir);
  a.installTranslator( &tor );

  bool readOnly = false;
  QString FileName, s;
  for(int i=1; i<argc; i++) {
    s = argv[i];
    if(s.at(0) == '-') {
      if(s.length() != 2) {
    fprintf(stdout, "%s", qPrintable(QObject::tr("Too long command line argument!\n\n")));
	showOptions();
	return -1;
      }
      switch(s.at(1).toLatin1()) {
	case 'r': readOnly = true;
		  break;
	case 'h': showOptions();
		  return 0;
	default :
      fprintf(stdout, "%s", qPrintable(QObject::tr("Wrong command line argument!\n\n")));
	  showOptions();
	  return -1;
      }
    }
    else  if(FileName.isEmpty())  FileName = s;
	  else {
        fprintf(stdout, "%s", qPrintable(QObject::tr("Only one filename allowed!\n\n")));
	    showOptions();
	    return -1;
	  }
  }

  QucsEdit *qucs = new QucsEdit(FileName, readOnly);
  qucs->raise();
  qucs->resize(QucsSettings.dx, QucsSettings.dy); // size and position ...
  qucs->move(QucsSettings.x, QucsSettings.y);     // ... before "show" !!!
  qucs->show();
  int result = a.exec();
  saveApplSettings(qucs);
  return result;


}
