/***************************************************************************
                             qresistor.h
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



#ifndef QRESISTOR_HPP
#define QRESISTOR_HPP
#include "classproto.h"
#include <QWidget>
#include <vector>

#include <iostream>

class QResistor : public QWidget
{
	Resistor *res;
	float resistance;
	float tolerance;
	char modifier;
	string tolring;
	vector<int> colcodes;


	Q_OBJECT
	public:
		QResistor( ){};
		void QResistorModify(vector<string> _colors,string _tolring)
		{
			res=new Resistor(_colors, _tolring);
			resistance=res->resistance;
			tolerance=res->tolerance;
			modifier=res->modifier;
			colcodes=res->colcodes;
			tolring=res->tolring;
			delete res;
			emit valueModified(this);
		}
		void QResistorModify(float _resistance, float _tolerance)
		{
			res=new Resistor(_resistance, _tolerance);
			resistance=res->resistance;
			tolerance=res->tolerance;
			modifier=res->modifier;
			colcodes=res->colcodes;
			tolring=res->tolring;
			delete res;

			emit valueModified(this);
		}

		float getTolerance() const
		{
			return tolerance;
		}

		float getResistance() const
		{
			return resistance;
		}

		char getModifier() const
		{
			return modifier;
		}

		vector<int> getColors() const
		{
			return colcodes;
		}

		string getTolring() const
		{
			return tolring;
		}

	public slots:

	signals:
		void valueModified(QResistor*);
};
#endif
