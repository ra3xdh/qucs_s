/***************************************************************************
                             myresistancebox.h
                             ------------------
    begin                : Mar 2012
    copyright            : (C) 2012 by Sudhakar.M.K
    email                : sudhakar.m.kumar
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/



#include <qlineedit.h>
#include <q3groupbox.h>
//Added by qt3to4:
#include <QLabel>
class QLabel;
class QResistor;
class QString;
class QComboBox;
class MyResistanceBox: public Q3GroupBox 
{
	Q_OBJECT
	QString closestRes;
	QString modifier;
	QString tol;
	QLabel* stdRes;
	QLineEdit *resValueEdit; 
	QComboBox *resUnit; 
	QComboBox *tolerance; 
	public:
		MyResistanceBox( QWidget *parent=0, const char *name=0 );
		float enteredValue();
		float enteredTolerance();
	public slots:
		void update(QResistor *);

};
