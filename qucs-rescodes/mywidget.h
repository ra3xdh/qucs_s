/***************************************************************************
                               mywidget.h
                             ------------------
    begin                : Mar 2012
    copyright            : (C) 2012 by Sudhakar.M.K
    email                : sudhakar.m.kumar@gmail.com
    copyright            : (C) 2016, Qucs team (see AUTHORS file)

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QMainWindow>
class MyResistanceBox;
class MyColorBox;
class QResistor;

// Application settings.
struct tQucsSettings {
  int x, y, dx, dy;    // position and size of main window
  QFont font;          // font
  QString LangDir;     // translation directory
  QString Language;
};

//---------------------------class declarations------------------------------------//
class MyWidget: public QMainWindow
{
	Q_OBJECT
	QResistor *res;
	MyResistanceBox *resBox;
	MyColorBox *colorBox;

	public:
		MyWidget();
	public slots:

		void setResistanceValue();
		void setColorValue();
		void slotHelpAbout();
		void slotHelpAboutQt();
		void slotHelpIntro();
		void slotConfiguration();
	signals:
};

#endif /* MYWIDGET_H */
