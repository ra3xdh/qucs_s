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
#include "extsimkernels/verilogawriter.h"

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

  Lines.append(new qucs::Line(-xb, -yb, -xb,  yb,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-xb,  yb,  xb+3,yb,QPen(Qt::darkBlue,2)));
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
  Name = QObject::tr("Qucs legacy equation");
  BitmapFile = (char *) "equation";

  if(getNewOne)  return new Equation();
  return 0;
}

QString Equation::getVAvariables()
{
    QStringList vars;
    for (unsigned int i=0;i<Props.count()-1;i++) {
        vars.append(Props.at(i)->Name);
    }

    return QString("real %1;\n").arg(vars.join(", "));
}

QString Equation::getVAExpressions()
{
    QString s;
    for (unsigned int i=0;i<Props.count()-1;i++) {
        QStringList tokens;
        spicecompat::splitEqn(Props.at(i)->Value,tokens);
        vacompat::convert_functions(tokens);
        s += QString("%1=%2;\n").arg(Props.at(i)->Name).arg(tokens.join(""));
    }
    return s;
}

/*!
 * \brief Equation::getExpression Extract equations that don't contain simulation variables
 *        (voltages/cureents) in .PARAM section of spice netlist
 * \param isXyce True if Xyce is used.
 * \return .PARAM section of spice netlist as a single string.
 */
QString Equation::getExpression(bool isXyce)
{
    if (isActive != COMP_IS_ACTIVE) return QString("");

    QStringList ng_vars,ngsims;
    getNgnutmegVars(ng_vars,ngsims);

    QString s;
    s.clear();
    for (unsigned int i=0;i<Props.count()-1;i++) {
        QStringList tokens;
        QString eqn = Props.at(i)->Value;
        spicecompat::splitEqn(eqn,tokens);
        spicecompat::convert_functions(tokens,isXyce);
        eqn = tokens.join("");
        if (isXyce) {
            eqn.replace("^","**");
            QRegExp fp_pattern("^[\\+\\-]*\\d*\\.\\d+$"); // float
            QRegExp fp_exp_pattern("^[\\+\\-]*\\d*\\.*\\d+e[\\+\\-]*\\d+$"); // float with exp
            QRegExp dec_pattern("^[\\+\\-]*\\d+$"); // integer
            QRegExp spicefp_pattern("^[\\+\\-]*\\d*\\.\\d+[A-Za-z]{,3}$"); // float and scaling suffix
            QRegExp spicedec_pattern("^[\\+\\-]*\\d+[A-Za-z]{,3}$"); // decimal and scaling suffix
            if (!(fp_pattern.exactMatch(eqn)||
                  dec_pattern.exactMatch(eqn)||
                  fp_exp_pattern.exactMatch(eqn)||
                  spicefp_pattern.exactMatch(eqn)||
                  spicedec_pattern.exactMatch(eqn))) eqn = "{" + eqn + "}"; // wrap equation if it contains vars
        }

        if (!spicecompat::containNodes(tokens,ng_vars)) {
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
    if (isActive != COMP_IS_ACTIVE) return QString("");

    QStringList ng_vars,ngsims;
    getNgnutmegVars(ng_vars,ngsims);

    QString s;
    dep_vars.clear();
    for (unsigned int i=0;i<Props.count()-1;i++) {
        QStringList tokens;
        QString eqn = Props.at(i)->Value;
        spicecompat::splitEqn(eqn,tokens);
        eqn.replace("^","**");
        if (spicecompat::containNodes(tokens,ng_vars)) {
            QString used_sim="";
            spicecompat::convertNodeNames(tokens,used_sim);
            if (used_sim.isEmpty()) {
                int idx = ng_vars.indexOf(Props.at(i)->Name);
                if (idx>=0) used_sim = ngsims.at(idx);
            }
            if ((sim == used_sim)||(used_sim=="all")) {
                eqn = tokens.join("");
                s += QString("let %1=%2\n").arg(Props.at(i)->Name).arg(eqn);
                dep_vars.append(Props.at(i)->Name);
            }
        }
    }
    return s;
}

/*!
 * \brief Equation::getNgspiceScript Duplicate variables from .PARAM section
 *        in .control .endc section of Ngspice netlist.
 * \return Ngspice script as a single string.
 */
QString Equation::getNgspiceScript()
{
    QStringList ng_vars,ngsims;
    getNgnutmegVars(ng_vars,ngsims);

    QString s;
    s.clear();
    if (isActive != COMP_IS_ACTIVE) return QString("");

    for (unsigned int i=0;i<Props.count()-1;i++) {
        QStringList tokens;
        QString eqn = Props.at(i)->Value;
        spicecompat::splitEqn(eqn,tokens);
        spicecompat::convert_functions(tokens,false);
        eqn = tokens.join("");

        if (!spicecompat::containNodes(tokens,ng_vars)) {
            s += QString("let %1=%2\n").arg(Props.at(i)->Name).arg(eqn);
        }
    }
    return s;
}

/*!
 * \brief Equation::getNgnutmegVars Extract variables and simulations that are used
 *        in Ngnutneg script in nested variables
 * \param[out] vars
 * \param[sims] simulations
 */
void Equation::getNgnutmegVars(QStringList &vars, QStringList &sims)
{
    vars.clear();
    sims.clear();
    for (unsigned int i=0;i<Props.count()-1;i++) {
        QStringList tokens;
        QString eqn = Props.at(i)->Value;
        spicecompat::splitEqn(eqn,tokens);
        if (spicecompat::containNodes(tokens,vars)) {
            vars.append(Props.at(i)->Name);
            QString used_sim="";
            spicecompat::convertNodeNames(tokens,used_sim);
            if (used_sim.isEmpty()) {
                sims.append("all");
            } else {
                sims.append(used_sim);
            }
        }
    }
    foreach (QString var,vars) {
        QString sim;
        int idx = vars.indexOf(var);
        if (idx>=0) sim = sims.at(idx);
        if (sim=="all") {
            QString eqn = getProperty(var)->Value;
            QStringList tokens;
            spicecompat::splitEqn(eqn,tokens);
            foreach (QString tok,tokens) {
                int idx1 = vars.indexOf(tok);
                if ((idx1>=0)&&(sims[idx1]!="all")) sims[idx]=sims[idx1];
            }
        }
    }
}
