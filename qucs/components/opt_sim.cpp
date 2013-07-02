/***************************************************************************
                               opt_sim.cpp
                              -------------
    begin                : Sat Jul 22 2006
    copyright            : (C) 2006 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <QtGui>
#include <qdir.h>
#include <qfile.h>
#include <q3textstream.h>
#include <qregexp.h>
#include <qstring.h>
#include <qstringlist.h>

#include "opt_sim.h"
#include "main.h"


Optimize_Sim::Optimize_Sim()
{
  Description = QObject::tr("Optimization");

  Texts.append(new Text(0, 0, Description, Qt::darkBlue, QucsSettings.largeFontSize));

  x1 = -10; y1 = -9;
  x2 = x1+128; y2 = y1+41;

  tx = 0;
  ty = y2+1;
  Model = ".Opt";
  Name  = "Opt";

  Props.append(new Property("Sim", "", false, ""));
  Props.append(new Property("DE", "3|50|2|20|0.85|1|3|1e-6|10|100", false, ""));
}

Optimize_Sim::~Optimize_Sim()
{
}

Component* Optimize_Sim::newOne()
{
  return new Optimize_Sim();
}

Element* Optimize_Sim::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("optimization");
  BitmapFile = (char *) "optimize";

  if(getNewOne)  return new Optimize_Sim();
  return 0;
}

// -------------------------------------------------------
QString Optimize_Sim::netlist()
{
  QString s = "#\n";
  if (createASCOFiles()) {
    s += "# ASCO configuration file(s) created\n";
  } else {
    s += "# Failed to create ASCO configuration file(s)\n";
  }
  s += "#\n\n";
  return s;
}

 
// -----------------------------------------------------------
bool Optimize_Sim::createASCOFiles()
{
  Property* pp;
  QFile afile(QucsSettings.QucsHomeDir.filePath("asco_netlist.cfg"));
  if(afile.open(QIODevice::WriteOnly)) {
    Q3TextStream stream(&afile);
    stream << "*\n";
    stream << "* ASCO configuration file for '" << Name << "'\n";
    stream << "*\n\n";

    stream << "#Optimization Flow#\n";
    stream << "Alter:no\n";
    stream << "MonteCarlo:no\n";
    stream << "AlterMC cost:0.00\n";
    stream << "ExecuteRF:no\n";
    stream << "#\n\n";

    stream << "#DE#\n";
    pp = Props.at(1);
    QString val;
    val = pp->Value.section('|',0,0);
    stream << "choice of method:" << val << "\n";
    val = pp->Value.section('|',1,1);
    stream << "maximum no. of iterations:" << val << "\n";
    val = pp->Value.section('|',2,2);
    stream << "Output refresh cycle:" << val << "\n";
    val = pp->Value.section('|',3,3);
    stream << "No. of parents NP:" << val << "\n";
    val= pp->Value.section('|',4,4);
    stream << "Constant F:" << val << "\n";
    val = pp->Value.section('|',5,5);
    stream << "Crossing Over factor CR:" << val << "\n";
    val = pp->Value.section('|',6,6);
    stream << "Seed for pseudo random number generator:" << val << "\n";
    val = pp->Value.section('|',7,7);
    stream << "Minimum Cost Variance:" << val << "\n";
    val = pp->Value.section('|',8,8);
    stream << "Cost objectives:" << val << "\n";
    val = pp->Value.section('|',9,9);
    stream << "Cost constraints:" << val << "\n";
    stream << "#\n\n";

    stream << "# Parameters #\n";
    int i=1;
    for(pp = Props.at(2); pp != 0; pp = Props.next(), i++) {
      if(pp->Name == "Var") {
	stream << "Parameter " << i << ":";
	val = pp->Value.section('|',0,0);
	stream << "#" << val << "#" << ":";
	val = pp->Value.section('|',2,2);
	stream << val << ":";
	val = pp->Value.section('|',3,3);
	stream << val << ":";
	val = pp->Value.section('|',4,4);
	stream << val << ":";
	val = pp->Value.section('|',5,5);
	stream << val << ":";
	val = pp->Value.section('|',1,1);
	stream << ((val == "yes") ? "OPT" : "---") << "\n";
      }
    }
    stream << "#\n\n";

    stream << "# Measurements #\n";
    for(pp = Props.at(2); pp != 0; pp = Props.next(), i++) {
      if(pp->Name == "Goal") {
	val = pp->Value.section('|',1,1);
	QString Type, Value;
	Value = pp->Value.section('|',2,2);
	if (val == "MIN" || val == "MAX" || val == "MON") {
	  Value = "---";
	}
	Type = val;
	val = pp->Value.section('|',0,0);
	stream << val <<  ":"
	       << "---" << ":"
	       << Type << ":" << Value << "\n";
      }
    }
    stream << "#\n\n";

    stream << "# Post Processing #\n";
    stream << "#\n\n";

    afile.close();
  } else return false;

  QDir ExtractDir(QucsSettings.QucsHomeDir);
  if(!ExtractDir.cd("extract")) {
    if(!ExtractDir.mkdir("extract"))
      return false;
    if(!ExtractDir.cd("extract"))
      return false;
  }      

  for(pp = Props.at(2); pp != 0; pp = Props.next()) {
    if(pp->Name == "Goal") {
      QString VarName = pp->Value.section('|',0,0);
      QFile efile(ExtractDir.filePath(VarName));
      if(efile.open(QIODevice::WriteOnly)) {
	Q3TextStream stream(&efile);
	stream << "# Info #\n";
	stream << "#\n\n";
	stream << "# Commands #\n";
	stream << "#\n\n";
	stream << "# Post Processing #\n";
	stream << "MEASURE_VAR:#SYMBOL#:SEARCH_FOR:'<indep " << VarName
	       << "':S_COL:01:P_LINE:01:P_COL:01:31" << "\n";
	stream << "#\n\n";
	efile.close();
      }
      else return false;
    }
  }
  return true;
}

// -----------------------------------------------------------
bool Optimize_Sim::createASCOnetlist()
{
  Property* pp;
  QStringList vars;
  for(pp = Props.at(2); pp != 0; pp = Props.next()) {
    if(pp->Name == "Var") {
      vars += pp->Value.section('|',0,0);
    }
  }

  QFile infile(QucsSettings.QucsHomeDir.filePath("netlist.txt"));
  QFile outfile(QucsSettings.QucsHomeDir.filePath("asco_netlist.txt"));
  if(!infile.open(QIODevice::ReadOnly)) return false;
  if(!outfile.open(QIODevice::WriteOnly)) return false;
  Q3TextStream instream(&infile);
  Q3TextStream outstream(&outfile);
  QString Line;
  while(!instream.atEnd()) {
    Line = instream.readLine();
    for(QStringList::Iterator it = vars.begin(); it != vars.end(); ++it ) {
      if(Line.contains("Eqn:"))
      {
          QStringList splitLine = Line.split("\"");

          for(int i=1;i<splitLine.size()-3;i+=2)
          {
              if(splitLine[i].compare("yes")!=0 && splitLine[i].compare("no")!=0) //ignore last piece between quotes
              {
                  splitLine[i].replace(*it, "#"+*it+"#");
              }
          }
          Line = splitLine.join("\"");

      }
      else
      {
          QRegExp reg = QRegExp("=\"(" + *it + ")\"");
          Line.replace(reg, "=\"#\\1#\"");
      }

    }
    outstream << Line << "\n";
  }
  outfile.close();
  infile.close();
  return true;
}

// -----------------------------------------------------------
bool Optimize_Sim::loadASCOout()
{
  bool changed = false;
  Property* pp;
  QStringList vars;
  for(pp = Props.at(2); pp != 0; pp = Props.next()) {
    if(pp->Name == "Var") {
      vars += pp->Value.section('|',0,0);
    }
  }

  QFile infile(QucsSettings.QucsHomeDir.filePath("asco_out.log"));
  if(!infile.open(QIODevice::ReadOnly)) return false;
  Q3TextStream instream(&infile);
  QString Line;
  while(!instream.atEnd()) Line = instream.readLine();
  infile.close();

  QStringList entries = QStringList::split(':',Line);
  QStringList::Iterator it;
  for(it = entries.begin(); it != entries.end(); ++it ) {
    QString Name = *it;
    Name = Name.stripWhiteSpace();
    if(vars.contains(Name)) {
      for(pp = Props.at(2); pp != 0; pp = Props.next()) {
	if(pp->Name == "Var") {
	  QString val[6];
	  val[0] = pp->Value.section('|',0,0);
	  if(val[0]==Name) {
	    val[1] = pp->Value.section('|',1,1);
	    val[2] = pp->Value.section('|',2,2);
	    val[3] = pp->Value.section('|',3,3);
	    val[4] = pp->Value.section('|',4,4);
	    val[5] = pp->Value.section('|',5,5);
	    ++it;
	    QString Value = *it;
	    Value = Value.stripWhiteSpace();
	    val[2] = Value;
	    pp->Value = val[0] + "|" + val[1] + "|" + val[2] + "|" +
	      val[3] + "|" + val[4] + "|" + val[5];
	    changed = true;
	    break;
	  }
	}
      }
    }
  }
  return changed;
}
