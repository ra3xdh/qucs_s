/***************************************************************************
                             classproto.h
                             ------------------
    begin                : Mar 2012
    copyright            : (C) 2012 by Arun I
    email                : theroarofthedragon@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/



#ifndef CLASSPROTO_HPP
#define CLASSPROTO_HPP
#include <vector>
#include <map>
#include <string>
using namespace std;

class Resistor
{
	private:
		//map constructing functions
		map<string,int> constructColcodeMap();
		map<string,float> constructTolcodeMap();
		map<float,vector<float> > constructStandardResistorMap();
		map<int,string> constructRevColcodeMap();
		map<float,string> constructRevTolcodeMap();
		map<float,int> constructNoOfResBandsMap();

		void closestStandardResistor();
		void calcResistance();
		void calcTolerance();
		void calcColors();
		void calcTolcolors();
	public:
		//data
		vector<string> colors;
		vector<int> colcodes;
		string tolring;
		float resistance;
		float tolerance;
		char modifier;

		//class constructors
		Resistor();
		Resistor(vector<string> _colors,string _tolring);
		Resistor(float _resistance,float _tolerance);
};

class Notation
{
	private:
		map<int,char> constructExpToModMap();
		map<char,int> constructModToExpMap();
	public:
		Notation();
		void toHumanNotation(float& value,char& modifier);
		void toAbsoluteNotation(float& value,char modifier);
};


#endif
