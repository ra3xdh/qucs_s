/***************************************************************************
                             resistor.cpp
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



#include <iostream>
#include <cmath>
#include "classproto.h"
using namespace std;

map<string,int> Resistor::constructColcodeMap()
{
	map<string,int> colcodeMap;
	colcodeMap["silver"]=-2;
	colcodeMap["gold"]=-1;
	colcodeMap["black"]=0;
	colcodeMap["brown"]=1;
	colcodeMap["red"]=2;
	colcodeMap["orange"]=3;
	colcodeMap["yellow"]=4;
	colcodeMap["green"]=5;
	colcodeMap["blue"]=6;
	colcodeMap["violet"]=7;
	colcodeMap["gray"]=8;
	colcodeMap["white"]=9;
	return(colcodeMap);
}

map<string,float> Resistor::constructTolcodeMap()
{
	map<string,float> tolcodeMap;
	tolcodeMap["gray"]=0.05;
	tolcodeMap["violet"]=0.1;
	tolcodeMap["blue"]=0.25;
	tolcodeMap["green"]=0.5;
	tolcodeMap["brown"]=1;
	tolcodeMap["red"]=2;
	tolcodeMap["gold"]=5;
	tolcodeMap["silver"]=10;
	tolcodeMap["none"]=20;
	return(tolcodeMap);
}

map<float,vector<float> > Resistor::constructStandardResistorMap()
{
	map<float,vector<float> > standardResistorMap;
	float std20[]={10,15,22,33,47,68};
	float std10[]={10,12,15,18,22,27,33,39,47,56,68,82};
	float std5[]={10,11,12,13,15,16,18,20,22,24,27,30,33,36,39,43,47,51,56,62,68,75,82,91};
	float std2[]={100,105,110,115,121,127,133,140,147,154,162,169,178,187,196,205,215,226,237,249,261,274,287,301,316,332,348,365,383,402,422,442,464,487,511,536,562,590,619,649,681,715,750,787,825,866,909,953};
	float std1[]={100,102,105,107,110,113,115,118,121,124,127,130,133,137,140,143,147,150,154,158,162,165,169,174,178,182,187,191,196,200,205,210,215,221,226,232,237,243,249,255,261,267,274,280,287,294,301,309,316,324,332,340,348,357,365,374,383,392,402,412,422,432,442,453,464,475,487,491,511,523,536,549,562,576,590,604,619,634,649,665,681,698,715,732,750,768,787,806,825,845,866,887,909,931,959,976};
	float stdPrecise[]={100,101,102,104,105,106,107,109,110,111,113,114,115,117,118,120,121,123,124,126,127,129,130,132,133,135,137,138,140,142,143,145,147,149,150,152,154,156,158,160,162,164,165,167,169,172,174,176,178,180,182,184,187,189,191,193,196,198,200,203,205,208,210,213,215,218,221,223,226,229,232,234,237,240,243,246,249,252,255,258,261,264,267,271,274,277,280,284,287,291,294,298,301,305,309,312,316,320,324,328,332,336,340,344,348,352,357,361,365,370,374,379,383,388,392,397,402,407,412,417,422,427,432,437,442,448,453,459,464,470,475,481,487,493,499,505,511,517,523,530,536,542,549,556,562,569,576,583,590,597,604,612,619,626,634,642,649,657,665,673,681,690,698,706,715,723,732,741,750,759,768,777,787,796,806,816,825,835,845,856,866,876,887,898,909,920,931,942,953,965,976,988};
	vector<float> vectStd20(std20,std20+sizeof(std20)/sizeof(float));
	vector<float> vectStd10(std10,std10+sizeof(std10)/sizeof(float));
	vector<float> vectStd5(std5,std5+sizeof(std5)/sizeof(float));
	vector<float> vectStd2(std2,std2+sizeof(std2)/sizeof(float));
	vector<float> vectStd1(std1,std1+sizeof(std1)/sizeof(float));
	vector<float> vectStdPrecise(stdPrecise,stdPrecise+sizeof(stdPrecise)/sizeof(float));
	standardResistorMap[20]=vectStd20;
	standardResistorMap[10]=vectStd10;
	standardResistorMap[5]=vectStd5;
	standardResistorMap[2]=vectStd2;
	standardResistorMap[1]=vectStd1;
	standardResistorMap[0.5]=vectStdPrecise;
	standardResistorMap[0.25]=vectStdPrecise;
	standardResistorMap[0.1]=vectStdPrecise;
	standardResistorMap[0.05]=vectStdPrecise;
	return(standardResistorMap);
}

map<int,string> Resistor::constructRevColcodeMap()
{
	map<int,string> revColcodeMap;
	revColcodeMap[-2]="silver";
	revColcodeMap[-1]="gold";
	revColcodeMap[0]="black";
	revColcodeMap[1]="brown";
	revColcodeMap[2]="red";
	revColcodeMap[3]="orange";
	revColcodeMap[4]="yellow";
	revColcodeMap[5]="green";
	revColcodeMap[6]="blue";
	revColcodeMap[7]="violet";
	revColcodeMap[8]="gray";
	revColcodeMap[9]="white";
	return(revColcodeMap);
}

map<float,string> Resistor::constructRevTolcodeMap()
{
	map<float,string> revTolcodeMap;
	revTolcodeMap[0.05]="gray";
	revTolcodeMap[0.1]="violet";
	revTolcodeMap[0.25]="blue";
	revTolcodeMap[0.5]="green";
	revTolcodeMap[1]="brown";
	revTolcodeMap[2]="red";
	revTolcodeMap[5]="gold";
	revTolcodeMap[10]="silver";
	revTolcodeMap[20]="none";
	return(revTolcodeMap);
}

map<float,int> Resistor::constructNoOfResBandsMap()
{
	map<float,int> noOfResBandsMap;
	noOfResBandsMap[20]=2;
	noOfResBandsMap[10]=2;
	noOfResBandsMap[5]=2;
	noOfResBandsMap[2]=3;
	noOfResBandsMap[1]=3;
	noOfResBandsMap[0.5]=3;
	noOfResBandsMap[0.25]=3;
	noOfResBandsMap[0.1]=3;
	noOfResBandsMap[0.05]=3;
	return(noOfResBandsMap);
}

void Resistor::closestStandardResistor()
{
	map<float,vector<float> > standardResistorMap=constructStandardResistorMap();
	vector<float> stdRes=standardResistorMap[tolerance];

	float err,minErr,closestStdRes;
	minErr=log10f(resistance/stdRes[0]);
	minErr=minErr-round(minErr);
	closestStdRes=stdRes[0];
	for (unsigned int i=1;i<stdRes.size();i++)
	{
		err=log10f(resistance/stdRes[i]);
		err=err-round(err);
		if (fabs(err)<fabs(minErr))
		{
			minErr=err;
			closestStdRes=stdRes[i];
		}
	}
	int multiplier=round(log10f(resistance/closestStdRes));
	resistance=closestStdRes*pow(10,multiplier);
}

void Resistor::calcResistance()
{
	map<string,int> colcodeMap=constructColcodeMap();
	unsigned int i;
	resistance=0;
	for (i=0;i<colors.size()-1;i++) {
		resistance=resistance*10+colcodeMap[colors[i]];
		colcodes.push_back(colcodeMap[colors[i]]);
	}
	resistance=resistance*pow(10,colcodeMap[colors[i]]);
	colcodes.push_back(colcodeMap[colors[i]]);
	Notation notation;
	notation.toHumanNotation(resistance,modifier);
}

void Resistor::calcTolerance()
{
	map<string,float> tolcodeMap=constructTolcodeMap();
	tolerance=tolcodeMap[tolring];
}

void Resistor::calcColors()
{
	map<float,int> noOfResBandsMap=constructNoOfResBandsMap();
	map<int,string> revColcodeMap=constructRevColcodeMap();
	int noOfResBands=noOfResBandsMap[tolerance];
	int noOfDigits=floor(log10f(resistance))+1;
	int exp=noOfDigits-noOfResBands;
	int _resistance=resistance*pow(10,-exp);
	int digit;

	for (int i=pow(10,noOfResBands-1);i>=1;i/=10)
	{
		digit=_resistance/i;
		_resistance=_resistance-digit*i;
		colors.push_back(revColcodeMap[digit]);
		colcodes.push_back(digit);
	}
	colors.push_back(revColcodeMap[exp]);
	colcodes.push_back(exp);
}

void Resistor::calcTolcolors()
{
	map<float,string> revTolcodeMap=constructRevTolcodeMap();
	tolring=revTolcodeMap[tolerance];
}

Resistor::Resistor()
{
}

Resistor::Resistor(vector<string> _colors,string _tolring)
{
	colors=_colors;
	tolring=_tolring;
	calcResistance();
	calcTolerance();
}

Resistor::Resistor(float _resistance,float _tolerance)
{
	resistance=_resistance;
	tolerance=_tolerance;
	closestStandardResistor();
	calcColors();
	calcTolcolors();
	Notation notation;
	notation.toHumanNotation(resistance,modifier);
}
