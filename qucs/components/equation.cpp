/***************************************************************************
                          equation.cpp  -  description
                             -------------------
    begin                : Sat Aug 23 2003
    copyright            : (C) 2003 by Michael Margraf
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
#include "equation.h"
#include "main.h"
#include "extsimkernels/spicecompat.h"

#include <QFontMetrics>

Equation::Equation()
{
  isEquation = true;
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr("equation");

  QFont f = QucsSettings.font;
  f.setWeight(QFont::Light);
  f.setPointSizeF(12.0);
  QFontMetrics  metrics(f, 0);  // use the the screen-compatible metric
  QSize r = metrics.size(0, QObject::tr("Equation"));
  int xb = r.width()  >> 1;
  int yb = r.height() >> 1;

  Lines.append(new Line(-xb, -yb, -xb,  yb,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-xb,  yb,  xb+3,yb,QPen(Qt::darkBlue,2)));
  Texts.append(new Text(-xb+4,  -yb-3, QObject::tr("Equation"),
			QColor(0,0,0), 12.0));

  x1 = -xb-3;  y1 = -yb-5;
  x2 =  xb+9; y2 =  yb+3;

  tx = x1+4;
  ty = y2+4;
  Model = "Eqn";
  Name  = "Eqn";

  Props.append(new Property("y", "1", true));
  Props.append(new Property("Export", "yes", false,
  		QObject::tr("put result into dataset")+" [yes, no]"));
}

Equation::~Equation()
{
}

// -------------------------------------------------------
QString Equation::verilogCode(int)
{
  QString s;
  // output all equations
  for(Property *pr = Props.first(); pr != 0; pr = Props.next())
    if(pr->Name != "Export")
      s += "  real "+pr->Name+"; initial "+pr->Name+" = "+pr->Value+";\n";
  return s;
}

// -------------------------------------------------------
QString Equation::vhdlCode(int)
{
  QString s;
  // output all equations
  for(Property *pr = Props.first(); pr != 0; pr = Props.next())
    if(pr->Name != "Export")
      s += "  constant "+pr->Name+" : time := "+pr->Value+";\n";
  return s;
}

Component* Equation::newOne()
{
  return new Equation();
}

Element* Equation::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Equation");
  BitmapFile = (char *) "";

  if(getNewOne)  return new Equation();
  return 0;
}

/*!
 * \brief Equation::getExpression Extract equations that don't contain simualtion variables
 *        (voltages/cureents) in .PARAM section of spice netlist
 * \param isXyce True if Xyce is used.
 * \return .PARAM section of spice netlist as a single string.
 */
QString Equation::getExpression(bool isXyce)
{
    QString s;
    s.clear();
    for (unsigned int i=0;i<Props.count()-1;i++) {
        QStringList tokens;
        QString eqn = Props.at(i)->Value;
        spicecompat::splitEqn(eqn,tokens);
        for(QStringList::iterator it = tokens.begin();it != tokens.end(); it++) {
            qDebug()<<spicecompat::convert_functions(*it,isXyce);
            *it = spicecompat::convert_functions(*it,isXyce);
        }
        eqn = tokens.join("");
        if (isXyce) eqn.replace("^","**");
        QRegExp fp_pattern("^[\\+\\-]*\\d*\\.\\d+$"); // float
        QRegExp fp_exp_pattern("^[\\+\\-]*\\d*\\.*\\d+e[\\+\\-]*\\d+$"); // float with exp
        QRegExp dec_pattern("^[\\+\\-]*\\d+$"); // integer
        if (!(fp_pattern.exactMatch(eqn)||
              dec_pattern.exactMatch(eqn)||
              fp_exp_pattern.exactMatch(eqn))) eqn = "{" + eqn + "}"; // wrap equation if it contains vars
        if (!spicecompat::containNodes(tokens)) {
            s += QString(".PARAM %1=%2\n").arg(Props.at(i)->Name).arg(eqn);
        }
    }
    return s;
}

/*!
 * \brief Equation::getEquations Convert equations that contains simulation variables
 *        (voltages/currents) into ngspice script. This script is placed between
 *        .control .endc sections of ngspice netlist. Also determines output variables
 *        that need to be plot.
 * \param[in] sim Used simulation (i.e "ac", "dc", "tran" )
 * \param[out] dep_vars The list of variables that need to place in dataset and to plot.
 * \return Ngspice script as a single string.
 */
QString Equation::getEquations(QString sim, QStringList &dep_vars)
{
    QString s;
    dep_vars.clear();
    for (unsigned int i=0;i<Props.count()-1;i++) {
        QStringList tokens;
        QString eqn = Props.at(i)->Value;
        spicecompat::splitEqn(eqn,tokens);
        eqn.replace("^","**");
        if (spicecompat::containNodes(tokens)) {
            QString used_sim;
            spicecompat::convertNodeNames(tokens,used_sim);
            if (sim == used_sim) {
                eqn = tokens.join("");
                s += QString("let %1=%2\n").arg(Props.at(i)->Name).arg(eqn);
                dep_vars.append(Props.at(i)->Name);
            }
        }
    }
    qDebug()<<sim;
    return s;
}

/*!
 * \brief Equation::getNgspiceScript Duplicate variables from .PARAM section
 *        in .control .endc section of Ngspice netlist.
 * \return Ngspice script as a single string.
 */
QString Equation::getNgspiceScript()
{
    QString s;
    s.clear();
    for (unsigned int i=0;i<Props.count()-1;i++) {
        QStringList tokens;
        QString eqn = Props.at(i)->Value;
        spicecompat::splitEqn(eqn,tokens);
        for(QStringList::iterator it = tokens.begin();it != tokens.end(); it++) {
            *it = spicecompat::convert_functions(*it,false);
        }
        eqn = tokens.join("");

        if (!spicecompat::containNodes(tokens)) {
            s += QString("let %1=%2\n").arg(Props.at(i)->Name).arg(eqn);
        }
    }
    return s;
}
