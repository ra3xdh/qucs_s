/***************************************************************************
                             mycolorbox.cpp
                             ------------------
    begin                : Mar 2012
    copyright            : (C) 2012 by Sudhakar.M.K
    email                : sudhakar.m.kumar@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "mycolorbox.h"
#include "qresistor.h"

#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>

#include <vector>
#include <map>
#include <string>

MyColorBox::MyColorBox( QWidget *parent, const char *name )
: QGroupBox("Colour Codes" ,parent)
{
	//--------------------color code displaying ui-------------------------------------//
	QStringList colors;
  colors << "none" << "black" << "brown" << "red" << "orange" << "yellow" << "green"
         << "blue" << "violet" << "gray" << "white" << "gold" << "silver";

	QStringList colors2;
  colors2 << "black" << "brown" << "red" << "orange" << "yellow" << "green" << "blue"
          << "violet" << "gray" << "white";

	QObject* item = new QObject;
	for(int i=0;i<4;i++)
	{
		colorCode[i]=new QComboBox(this);
		colorCode[i]->insertStringList(colors2);
	}

	colorCode[3]->insertStringList(colors);

	QStringList tolColors;
	tolColors << "none" << "silver" << "gold" << "red" << "brown" << "green" << "blue"
            << "violet" << "gray";

	colorCode[4]=new QComboBox (this);
	colorCode[4]->insertStringList(tolColors);

  QHBoxLayout *hbox = new QHBoxLayout;
  for (int i = 0; i < 5; ++i) {
    hbox->addWidget(colorCode[i]);
  }
  this->setLayout(hbox);
}

map<string,int> MyColorBox::constructRevTolindexMap()
{
	map<string,int> revTolindexMap;
	revTolindexMap["gray"]=8;
	revTolindexMap["violet"]=7;
	revTolindexMap["blue"]=6;
	revTolindexMap["green"]=5;
	revTolindexMap["brown"]=4;
	revTolindexMap["red"]=3;
	revTolindexMap["gold"]=2;
	revTolindexMap["silver"]=1;
	revTolindexMap["none"]=0;
	return(revTolindexMap);
}
void MyColorBox:: update(QResistor* res)
{
	int i;
	vector<int> list = res-> getColors();
	int len= list.size();

	for (i=0;i<len;i++){

		colorCode[i]->setCurrentItem(list[i]<0?(9-list[i]):list[i]);
	}
	for (i=len;i<4;i++)
		colorCode[i]->setCurrentItem(0);

	map<string,int> revTolcodeMap=constructRevTolindexMap();
	colorCode[i]->setCurrentItem(revTolcodeMap[res->getTolring()]);
}

vector<string> MyColorBox:: enteredValue()
{
	vector<string> colors;
	int i;
	string str;

	for (i=0;i<3;i++)
		colors.push_back(string((colorCode[i]->currentText()).ascii()));
	if((colorCode[3])->currentItem())
	{
		colors.push_back(string((colorCode[3]->currentText()).ascii()));

	}
		return colors;
}
string MyColorBox:: enteredTolerance()
{
	return string((colorCode[4]->currentText()).ascii());
}
