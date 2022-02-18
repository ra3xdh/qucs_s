/***************************************************************************
                                 main.cpp
                              ----------------
    begin                : Wed Apr 10 2014
    copyright            : (C) 2014 by Vadim Kuznetsov
    email                : ra3xdh@gmail.com
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
#include <QtCore>
#include <QApplication>

#include "qucsactivefilter.h"

struct tQucsSettings QucsSettings;


// #########################################################################
// Loads the settings file and stores the settings.
bool loadSettings()
{
    QSettings settings("qucs","qucs_s");
    settings.beginGroup("QucsActiveFilter");
    if(settings.contains("x"))QucsSettings.x=settings.value("x").toInt();
    if(settings.contains("y"))QucsSettings.y=settings.value("y").toInt();
    if(settings.contains("showConsole")) QucsSettings.showConsole=settings.value("showConsole").toBool();
    settings.endGroup();

    if(settings.contains("Language"))QucsSettings.Language=settings.value("Language").toString();

  return true;
}


// #########################################################################
// Saves the settings in the settings file.
bool saveApplSettings(QucsActiveFilter *qucs)
{
    QSettings settings ("qucs","qucs_s");
    settings.beginGroup("QucsActiveFilter");
    settings.setValue("x", qucs->x());
    settings.setValue("y", qucs->y());
    settings.setValue("showConsole", QucsSettings.showConsole);
    settings.endGroup();
  return true;

}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString LangDir;
    // apply default settings
    QucsSettings.x = 200;
    QucsSettings.y = 100;
    QucsSettings.showConsole = true;

    // is application relocated?
    QDir QucsDir;
    QString QucsApplicationPath = QCoreApplication::applicationDirPath();
#ifdef __APPLE__
    QucsDir = QDir(QucsApplicationPath.section("/bin",0,0));
#else
    QucsDir = QDir(QucsApplicationPath);
    QucsDir.cdUp();
#endif
    LangDir = QucsDir.canonicalPath() + "/share/" QUCS_NAME "/lang/";

    loadSettings();

    QTranslator tor( 0 );
    QString Lang = QucsSettings.Language;
    if(Lang.isEmpty())
      Lang = QString(QLocale::system().name());
    tor.load( QString("qucs_") + Lang, LangDir);
    a.installTranslator( &tor );

    QucsActiveFilter *w = new QucsActiveFilter();
    w->raise();
    w->move(QucsSettings.x, QucsSettings.y);  // position before "show" !!!
    w->show();
    
    int result = a.exec();
    saveApplSettings(w);
    return result;
}
