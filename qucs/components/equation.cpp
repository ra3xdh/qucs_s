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

void Equation::splitEqn(QString &eqn, QStringList &tokens)
{
    tokens.clear();
    QString tok = "";
    for (QString::iterator it=eqn.begin();it!=eqn.end();it++) {
        QString delim = "=()*/+-";
        if (it->isSpace()) continue;
        if (delim.contains(*it)) {
            if (!tok.isEmpty()) tokens.append(tok);
            tokens.append(*it);
            tok.clear();
            continue;
        }
        tok += *it;
    }
    if (!tok.isEmpty()) tokens.append(tok);
}

bool Equation::containNodes(QStringList &tokens)
{
    QRegExp var_pattern("^[\\w]+\\.([IV]t|[iv]|vn|Vb|[IV])$");
    foreach (QString tok,tokens) {
        if (var_pattern.exactMatch(tok)) return true;
    }
    return false;
}

void Equation::convertNodeNames(QStringList &tokens, QStringList &dep_vars)
{
    QRegExp var_pattern("^[\\w]+\\.([IV]t|[iv]|vn|Vb|[IV])$");
    for (QStringList::iterator it=tokens.begin();it!=tokens.end();it++) {
        if (var_pattern.exactMatch(*it))  {
            QString prefix;
            if (it->endsWith(".v")) prefix = "ac";
            if (it->endsWith(".Vt")) prefix = "tran";
            int idx = it->indexOf('.');
            int cnt = it->count();
            it->chop(cnt-idx);
            *it = QString("%1.V(%2)").arg(prefix).arg(*it);
        }
        if (dep_vars.contains(*it)) {
            *it = QString("V(%1)").arg(*it);
        }
    }
}

QString Equation::spice_netlist(bool isXyce)
{
    QString s;
    s.clear();
    QStringList dep_vars,tokens;

    getDepVars(dep_vars);

    for (unsigned int i=0;i<Props.count()-1;i++) {
        tokens.clear();
        QString eqn = Props.at(i)->Value;
        splitEqn(eqn,tokens);
        eqn.replace("^","**");
        if (containNodes(tokens)) {
            convertNodeNames(tokens,dep_vars);
            eqn = tokens.join("");
            s += QString("B%1 %2 0 V=%3\n").arg(Name).arg(Props.at(i)->Name).arg(eqn);
        }
    }
    return s;
}

QString Equation::getExpression(bool isXyce)
{
    QString s;
    s.clear();
    for (unsigned int i=0;i<Props.count()-1;i++) {
        QStringList tokens;
        QString eqn = Props.at(i)->Value;
        splitEqn(eqn,tokens);
        eqn.replace("^","**");
        if (!containNodes(tokens)) {
            s += QString(".PARAM %1=%2\n").arg(Props.at(i)->Name).arg(eqn);
        }
    }
    return s;
}

void Equation::getDepVars(QStringList &dep_vars)
{
    dep_vars.clear();
    for (unsigned int i=0;i<Props.count()-1;i++) {
        QStringList tokens;
        QString eqn = Props.at(i)->Value;
        splitEqn(eqn,tokens);
        if (containNodes(tokens)) {
            dep_vars.append(Props.at(i)->Name);
        }
    }

}

QString Equation::getEquations()
{
    QString s;
    for (unsigned int i=0;i<Props.count()-1;i++) {
        QStringList tokens;
        QString eqn = Props.at(i)->Value;
        splitEqn(eqn,tokens);
        eqn.replace("^","**");
        QStringList dep_vars;
        dep_vars.clear();
        if (containNodes(tokens)) {
            convertNodeNames(tokens,dep_vars);
            eqn = tokens.join("");
            s += QString("let %1=%2\n").arg(Props.at(i)->Name).arg(eqn);
        }
    }
    return s;
}
