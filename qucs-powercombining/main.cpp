/*
 * main.cpp - Power combining tool main
 *
 * copyright (C) 2016 Andres Martinez-Mera <andresmartinezmera@gmail.com>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *
 */

#include <QApplication>
#include "qucspowercombiningtool.h"

struct tQucsSettings QucsSettings;



// #########################################################################
// Loads the settings file and stores the settings.
bool loadSettings()
{
    QSettings settings("qucs","qucs_s");
    settings.beginGroup("QucsPowercombining");
    if(settings.contains("x"))QucsSettings.x=settings.value("x").toInt();
    if(settings.contains("y"))QucsSettings.y=settings.value("y").toInt();
    settings.endGroup();
    if(settings.contains("Language"))QucsSettings.Language=settings.value("Language").toString();
    if(settings.contains("DefaultSimulator"))
        QucsSettings.DefaultSimulator = settings.value("DefaultSimulator").toInt();
    else QucsSettings.DefaultSimulator = spicecompat::simNotSpecified;

  return true;
}


// #########################################################################
// Saves the settings in the settings file.
bool saveApplSettings(QucsPowerCombiningTool *qucs)
{
    QSettings settings ("qucs","qucs_s");
    settings.beginGroup("QucsPowercombining");
    settings.setValue("x", qucs->x());
    settings.setValue("y", qucs->y());
    settings.endGroup();
  return true;

}

int main(int argc, char *argv[])
{
    // apply default settings
    QucsSettings.x = 200;
    QucsSettings.y = 100;

    QApplication app(argc, argv);

    loadSettings();

    QTranslator tor( 0 );
    QString lang = QucsSettings.Language;
    if(lang.isEmpty())
      lang = QString(QLocale::system().name());
    tor.load( QString("qucs_") + lang, QucsSettings.LangDir);
    app.installTranslator( &tor );

    QucsPowerCombiningTool *PowerCombiningTool = new QucsPowerCombiningTool();
    PowerCombiningTool->raise();
    PowerCombiningTool->resize(350, 350);
    PowerCombiningTool->move(QucsSettings.x, QucsSettings.y);
    PowerCombiningTool->show();
    int result = app.exec();
    saveApplSettings(PowerCombiningTool);
    return result;
}
