/***************************************************************************
                             mycolorbox.h
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



#include <qgroupbox.h>
#include <string>
#include <map>
#include <vector>
using namespace std;

class QResistor;
class QComboBox;

class MyColorBox: public QGroupBox 
{
	Q_OBJECT
	
	QComboBox *colorCode[5];
	map<string,int> constructRevTolindexMap();
	public:
		MyColorBox( QWidget *parent=0, const char *name=0 );
		string enteredTolerance();
		vector<string> enteredValue();

	public slots:
		void update(QResistor*);
};
