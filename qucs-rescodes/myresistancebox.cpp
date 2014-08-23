/***************************************************************************
                             myresistancebox.cpp
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



#include <qvalidator.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qgroupbox.h>
#include "myresistancebox.h"
#include "qresistor.h"
#include <math.h>

MyResistanceBox::MyResistanceBox( QWidget *parent, const char *name )
: QGroupBox(3,Qt::Horizontal,"Resistance Values" ,parent, name )
{
	//--------------------resistance displaying ui ---------------------------------//
	
	resValueEdit = new QLineEdit (this, "resValueEdit");
	const QDoubleValidator *validator = new QDoubleValidator (resValueEdit,"validator");
	resValueEdit->setValidator(validator);
	resValueEdit->setMaxLength(6);
	resUnit = new QComboBox(this, "resUnit");
	tolerance = new QComboBox(this, "tolerance");
	stdRes = new QLabel("Closest standard resistance : 0 Ohms \n Tolerance : 20%",this, "stdRes");
	
	static const char* units[]={"Ohms","kOhms","MOhms",0};
	resUnit ->insertStrList(units);

	static const char* tols[]={"20%","10%","5%","2%","1%","0.5%","0.25%","0.1%","0.05%",0};
	tolerance ->insertStrList(tols);
}

void MyResistanceBox::update(QResistor *res)
{
	stdRes->setText("Closest standard resistance : "+QString::number(res->getResistance())+QString(QChar(res->getModifier()))+"Ohms \n Tolerance : "+QString::number(res->getTolerance())+"%");
}

float MyResistanceBox::enteredValue()
{
	bool ok;
	float f;
	int i=resUnit->currentItem();
	f=(resValueEdit->text()).toFloat(&ok);
	f=f*(pow(10,3*i));

	if (ok==1) return(f);
	else return(0);
}
float MyResistanceBox:: enteredTolerance()
{
	QString str=(tolerance->currentText()).remove('%');
	return (str.toFloat());
}
