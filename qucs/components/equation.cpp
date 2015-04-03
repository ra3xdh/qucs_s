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

void Equation::splitEqn(QString &eqn, QStringList &tokens)
{
    tokens.clear();
    QString tok = "";
    for (QString::iterator it=eqn.begin();it!=eqn.end();it++) {
        QString delim = "=()*/+-^";
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

void Equation::convertNodeNames(QStringList &tokens, QString &sim)
{
    QRegExp var_pattern("^[\\w]+\\.([IV]t|[iv]|vn|Vb|[IV])$");
    for (QStringList::iterator it=tokens.begin();it!=tokens.end();it++) {
        if (var_pattern.exactMatch(*it))  {
            if (it->endsWith(".v")) sim = "ac";
            if (it->endsWith(".Vt")) sim = "tran";
            if (it->endsWith(".V")) sim = "dc";
            int idx = it->indexOf('.');
            int cnt = it->count();
            it->chop(cnt-idx);
            *it = QString("V(%2)").arg(*it);
        }
    }
}


QString Equation::getExpression(bool isXyce)
{
    QString s;
    s.clear();
    for (unsigned int i=0;i<Props.count()-1;i++) {
        QStringList tokens;
        QString eqn = Props.at(i)->Value;
        splitEqn(eqn,tokens);
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
        if (!containNodes(tokens)) {
            s += QString(".PARAM %1=%2\n").arg(Props.at(i)->Name).arg(eqn);
        }
    }
    return s;
}

QString Equation::getEquations(QString sim, QStringList &dep_vars)
{
    QString s;
    dep_vars.clear();
    for (unsigned int i=0;i<Props.count()-1;i++) {
        QStringList tokens;
        QString eqn = Props.at(i)->Value;
        splitEqn(eqn,tokens);
        eqn.replace("^","**");
        if (containNodes(tokens)) {
            QString used_sim;
            convertNodeNames(tokens,used_sim);
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
