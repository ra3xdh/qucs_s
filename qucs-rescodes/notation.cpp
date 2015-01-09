/***************************************************************************
                            notation.cpp
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



#include <stdlib.h>
#include <ctype.h>
#include <cmath>
#include "classproto.h"

map<int,char> Notation::constructExpToModMap()
{
	map<int,char> ExpToModMap;
	ExpToModMap[3]='k';
	ExpToModMap[6]='M';
	ExpToModMap[9]='G';
	return(ExpToModMap);
}

map<char,int> Notation::constructModToExpMap()
{
	map<char,int> ModToExpMap;
	ModToExpMap[' ']=0;
	ModToExpMap['k']=3;
	ModToExpMap['M']=6;
	ModToExpMap['G']=9;
	return(ModToExpMap);
}

Notation::Notation()
{
}

void Notation::toHumanNotation(float& value,char& modifier)
{
	map<int,char> ExpToModMap=constructExpToModMap();
	int exp=0;
	while (value>=1000) {
		value=value/1000;
		exp=exp+3;
	};
	modifier=ExpToModMap[exp];
}

void Notation::toAbsoluteNotation(float& value,char modifier)
{
	map<char,int> ModToExpMap=constructModToExpMap();
	value=value*pow(10,modifier);
}
