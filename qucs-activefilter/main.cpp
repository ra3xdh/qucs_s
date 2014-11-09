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

int main(int argc, char *argv[])
{
    QString Lang,LangDir;
    QSettings settings("qucs","qucs");
    if(settings.contains("Language")) {
        Lang=settings.value("Language").toString();
    }

    char * var = getenv ("QUCSDIR");
    if (var != NULL) {
      QDir QucsDir = QDir (var);
      QString QucsDirStr = QucsDir.canonicalPath ();
      LangDir =
        QDir::convertSeparators (QucsDirStr + "/share/qucs/lang/");
    } else {
      LangDir = LANGUAGEDIR;
    }

    QApplication a(argc, argv);

    QTranslator tor( 0 );
    if(Lang.isEmpty())
      Lang = QString(QLocale::system().name());
    tor.load( QString("qucs_") + Lang, LangDir);
    a.installTranslator( &tor );

    QucsActiveFilter w;
    w.show();
    
    return a.exec();
}
