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
#include <QTextStream>
#include <QMessageBox>
#include <QDir>
#include <QFont>
#include <QTextCodec>
#include <QDebug>

#include "qucshelp.h"

#ifdef _WIN32
#include <Windows.h>  //for OutputDebugString
#endif

QDir QucsHelpDir; // directory to find helps files
tQucsSettings QucsSettings; // application settings

// #########################################################################
// Loads the settings file and stores the settings.
bool loadSettings()
{
  bool result = true;

  QFile file(QDir::homePath()+QDir::convertSeparators ("/.qucs/helprc"));
  if(!file.open(QIODevice::ReadOnly))
    result = false; // settings file doesn't exist
  else {
    QTextStream stream(&file);
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

// #########################################################################
// Saves the settings in the settings file.
bool saveApplSettings(QucsHelp *qucs)
{
  if(qucs->x() == QucsSettings.x)
    if(qucs->y() == QucsSettings.y)
      if(qucs->width() == QucsSettings.dx)
	if(qucs->height() == QucsSettings.dy)
	  return true;   // nothing has changed


  QFile file(QDir::homePath()+QDir::convertSeparators ("/.qucs/helprc"));
  if(!file.open(QIODevice::WriteOnly)) {
    QMessageBox::warning(0, QObject::tr("Warning"),
			QObject::tr("Cannot save settings !"));
    return false;
  }

  QString Line;
  QTextStream stream(&file);

  stream << "Settings file, Qucs Help System " PACKAGE_VERSION "\n"
	 << "HelpWindow=" << qucs->x() << ',' << qucs->y() << ','
	 << qucs->width() << ',' << qucs->height() << '\n';

  file.close();
  return true;
}

/*!
 * \brief qucsMessageOutput handles qDebug, qWarning, qCritical, qFatal.
 * \param type Message type (Qt enum)
 * \param msg Message
 *
 * The message handler is used to get control of the messages.
 * Particulary on Windows, as the messages are sent to the debugger and do not
 * show on the terminal. The handler could aslo be extended to create a log
 * mechanism.
 * <http://qt-project.org/doc/qt-4.8/debug.html#warning-and-debugging-messages>
 * <http://qt-project.org/doc/qt-4.8/qtglobal.html#qInstallMsgHandler>
 */
void qucsMessageOutput(QtMsgType type, const char *msg)
{
  switch (type) {
  case QtDebugMsg:
    fprintf(stderr, "Debug: %s\n", msg);
    break;
  case QtWarningMsg:
    fprintf(stderr, "Warning: %s\n", msg);
    break;
  case QtCriticalMsg:
    fprintf(stderr, "Critical: %s\n", msg);
    break;
  case QtFatalMsg:
    fprintf(stderr, "Fatal: %s\n", msg);
    abort();
  }

#ifdef _WIN32
  OutputDebugStringA(msg);
#endif
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
  QDir QucsDir;
  if (var != NULL) {
    QucsDir = QDir (QString(var));
    QString QucsDirStr = QucsDir.canonicalPath ();
    QucsSettings.DocDir =
      QDir::convertSeparators (QucsDirStr + "/share/qucs/docs/");
    QucsSettings.LangDir =
      QDir::convertSeparators (QucsDirStr + "/share/qucs/lang/");
  } else {
    QString QucsApplicationPath = QCoreApplication::applicationDirPath();
    #ifdef __APPLE__
    QucsDir = QDir(QucsApplicationPath.section("/bin",0,0));
    #else
    QucsDir = QDir(QucsApplicationPath);
    QucsDir.cdUp();
     #endif
    QucsSettings.DocDir  = QucsDir.canonicalPath() + "/share/qucs/docs/";
    QucsSettings.LangDir = QucsDir.canonicalPath() + "/share/qucs/lang/";
  }

  loadSettings();

  QApplication a(argc, argv);
  a.setFont(QucsSettings.font);

  QTranslator tor( 0 );
  QString locale = QucsSettings.Language;
  if(locale.isEmpty())
      locale = QString(QLocale::system().name());

  tor.load( QString("qucs_") + locale, QucsSettings.LangDir);
  a.installTranslator( &tor );

  QucsHelpDir = QucsSettings.DocDir + locale;
  if (!QucsHelpDir.exists () || !QucsHelpDir.isReadable ()) {
    int p = locale.indexOf ('_');
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

  qInstallMsgHandler(qucsMessageOutput);

  QucsHelp *qucs = new QucsHelp(Page);
  //a.setMainWidget(qucs);
  qucs->resize(QucsSettings.dx, QucsSettings.dy); // size and position ...
  qucs->move(QucsSettings.x, QucsSettings.y);     // ... before "show" !!!
  qucs->show();
  int result = a.exec();
  saveApplSettings(qucs);
  return result;
}
