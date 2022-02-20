/***************************************************************************
                               eqndefined.cpp
                              ----------------
    begin                : Thu Apr 19 2007
    copyright            : (C) 2007 by Stefan Jahn
    email                : stefan@lkcc.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "eqndefined.h"
#include "main.h"
#include "schematic.h"
#include "extsimkernels/spicecompat.h"
#include "extsimkernels/verilogawriter.h"

#include <QString>
#include <QStringList>
#include <QList>
#include <QRegExp>
#include <QFontMetrics>

#include <QFileInfo>

EqnDefined::EqnDefined()
{
  Description = QObject::tr("equation defined device");

  Model = "EDD";
  Name  = "D";
  SpiceModel = "B";

  // first properties !!!
  Props.append(new Property("Type", "explicit", false,
		QObject::tr("type of equations")+" [explicit, implicit]"));
  Props.append(new Property("Branches", "1", false,
		QObject::tr("number of branches")));

  // last properties
  Props.append(new Property("I1", "0", true,
		QObject::tr("current equation") + " 1"));
  Props.append(new Property("Q1", "0", false,
		QObject::tr("charge equation") + " 1"));

  createSymbol();
}

// -------------------------------------------------------
Component* EqnDefined::newOne()
{
  EqnDefined* p = new EqnDefined();
  p->Props.at(0)->Value = Props.at(0)->Value;
  p->Props.at(1)->Value = Props.at(1)->Value;
  p->recreate(0);
  return p;
}

// -------------------------------------------------------
Element* EqnDefined::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Equation Defined Device");
  BitmapFile = (char *) "edd";

  if(getNewOne) {
    EqnDefined* p = new EqnDefined();
    p->Props.at(0)->Value = "explicit";
    p->Props.at(1)->Value = "1";
    p->recreate(0);
    return p;
  }
  return 0;
}

// -------------------------------------------------------
QString EqnDefined::netlist()
{
  QString s = Model+":"+Name;
  QString e = "\n";

  // output all node names
  foreach(Port *p1, Ports)
    s += " "+p1->Connection->Name;   // node names

  // output all properties
  Property *p2 = Props.at(2);
  while(p2) {
    s += " "+p2->Name+"=\""+Name+"."+p2->Name+"\"";
    e += "  Eqn:Eqn"+Name+p2->Name+" "+
      Name+"."+p2->Name+"=\""+p2->Value+"\" Export=\"no\"\n";
    p2 = Props.next();
  }

  return s+e;
}

QString EqnDefined::spice_netlist(bool isXyce)
{    
    QString s;

    QList<int> used_currents;
    used_currents.clear();

    if (Props.at(0)->Value=="explicit") {
        int Nbranch = Props.at(1)->Value.toInt();

        for (int i=0;i<Nbranch;i++) {
            QStringList tokens;
            QString Ieqn = Props.at(2*(i+1))->Value; // parse current equation
            tokens.clear();
            spicecompat::splitEqn(Ieqn,tokens);
            findCurrents(tokens,used_currents);
            QString Qeqn = Props.at(2*(i+1)+1)->Value;
            tokens.clear();
            spicecompat::splitEqn(Qeqn,tokens);
            findCurrents(tokens,used_currents);
        }

        for (int i=0;i<Nbranch;i++) {
            QString Ieqn = Props.at(2*(i+1))->Value; // parse current equation
            Ieqn.replace("^","**");
            QStringList Itokens;
            spicecompat::splitEqn(Ieqn,Itokens);
            spicecompat::convert_functions(Itokens,isXyce);
            subsVoltages(Itokens,Nbranch);
            subsCurrents(Itokens);
            QString plus = Ports.at(2*i)->Connection->Name;
            QString minus = Ports.at(2*i+1)->Connection->Name;
            if (used_currents.contains(i)) { // if current is used add sensing source V=0
                s += QString("V_%1sens_%2 %3 %4 DC 0\n").arg(Name).arg(i).arg(plus).arg(plus+"_sens");
                plus = plus+"_sens";
            }
            s += QString("B%1I%2 %3 %4 I=%5\n").arg(Name).arg(i).arg(plus)
                    .arg(minus).arg(Itokens.join(""));

            QString Qeqn = Props.at(2*(i+1)+1)->Value; // parse charge equation only for Xyce
            if (Qeqn!="0") {
            //if (isXyce) {
                Qeqn.replace("^","**");
                QStringList Qtokens;
                spicecompat::splitEqn(Qeqn,Qtokens);
                spicecompat::convert_functions(Qtokens,isXyce);
                subsVoltages(Qtokens,Nbranch);
                subsCurrents(Qtokens);
                s += QString("G%1Q%2 %3 %4 n%1Q%2 %4 1.0\n").arg(Name).arg(i).arg(plus).arg(minus);
                s += QString("L%1Q%2 n%1Q%2 %3 1.0\n").arg(Name).arg(i).arg(minus);
                s += QString("B%1Q%2 n%1Q%2 %3 I=-(%4)\n").arg(Name).arg(i).arg(minus).arg(Qtokens.join(""));
            }
        }
    } else {
        s = "";
    }
    return s;
}

QString EqnDefined::va_code()
{
    QString s;

    if (Props.at(0)->Value=="explicit") {
        int Nbranch = Props.at(1)->Value.toInt();

        for (int i=0;i<Nbranch;i++) {
            QString Ieqn = Props.at(2*(i+1))->Value; // parse current equation
            QString plus = Ports.at(2*i)->Connection->Name;
            QString minus = Ports.at(2*i+1)->Connection->Name;
            QString Ipm = vacompat::normalize_current(plus,minus,true);
            if (Ieqn!="0") { // check for default
                QStringList Itokens;
                spicecompat::splitEqn(Ieqn,Itokens);
                vacompat::convert_functions(Itokens);
                subsVoltages(Itokens,Nbranch);
                if (plus=="gnd") s += QString("%1 <+ -(%2);\n").arg(Ipm).arg(Itokens.join(""));
                else s += QString("%1 <+ %2;\n").arg(Ipm).arg(Itokens.join(""));
            }
            QString Qeqn = Props.at(2*(i+1)+1)->Value; // parse charge equation only for Xyce
            if (Qeqn!="0") {
                QStringList Qtokens;
                spicecompat::splitEqn(Qeqn,Qtokens);
                vacompat::convert_functions(Qtokens);
                subsVoltages(Qtokens,Nbranch);
                if (plus=="gnd") s += QString("%1 <+ -ddt( %2 );\n").arg(Ipm).arg(Qtokens.join(""));
		else s += QString("%1 <+ ddt( %2 );\n").arg(Ipm).arg(Qtokens.join(""));
            }
        }
    } else {
        s = "";
    }
    return s;
}

/*!
 * \brief EqnDefined::subsVoltages Substitute volatges in spice Notation in token list
 * \param[in/out] tokens Token list. Should be obtained from spicecompat::splitEqn().
 *                This list is modified.
 * \param[in] Nbranch Number of branched of EDD
 */
void EqnDefined::subsVoltages(QStringList &tokens, int Nbranch)
{
    QRegExp volt_pattern("^V[0-9]+$");
    for (QStringList::iterator it = tokens.begin();it != tokens.end();it++) {
        if (volt_pattern.exactMatch(*it)) {
            QString volt = *it;
            volt.remove('V');
            int branch = volt.toInt();
            if (branch<=Nbranch) {
                QString plus = Ports.at(2*(branch-1))->Connection->Name;
                QString minus = Ports.at(2*(branch-1)+1)->Connection->Name;
                *it = vacompat::normalize_voltage(plus,minus);
            }
        }
    }
}

/*!
 * \brief EqnDefined::findCurrents Finds used currents (I1, I2, ...) in equation
 * \param tokens[in] Token list. Should be obtained from spicecompat::splitEqn().
 *                This list is not modified.
 * \param branches[out] The list of numbers of branches that currents are in use.
 *                      Branches are numbered from zero.
 */
void EqnDefined::findCurrents(QStringList &tokens, QList<int> &branches)
{
    QRegExp curr_pattern("^I[0-9]+$");
    for (QStringList::iterator it = tokens.begin();it != tokens.end();it++) {
        if (curr_pattern.exactMatch(*it)) {
            QString curr = *it;
            int num = curr.remove(0,1).toInt();
            if (!branches.contains(num)) branches.append(num-1);
        }
    }
}

/*!
 * \brief EqnDefined::subsCurrents Substitute currents in spice Notation in token list
 * \param tokens  Token list. Should be obtained from spicecompat::splitEqn().
 *                This list is modified.
 */
void EqnDefined::subsCurrents(QStringList &tokens)
{
    QRegExp curr_pattern("^I[0-9]+$");
    for (QStringList::iterator it = tokens.begin();it != tokens.end();it++) {
        if (curr_pattern.exactMatch(*it)) {
            QString curr = *it;
            int branch = curr.remove('I').toInt();
            *it = QString("i(V_%1sens_%2)").arg(Name).arg(branch-1);
        }
    }
}

// -------------------------------------------------------
void EqnDefined::createSymbol()
{  
  QFont Font(QucsSettings.font); // default application font
  // symbol text is smaller (10 pt default)
  //Font.setPointSizeF(Font.pointSizeF()/1.2);  // symbol text size proportional to default font size
  Font.setPointSize(10); // symbol text size fixed at 10 pt
  // get the small font size; use the screen-compatible metric
  QFontMetrics  smallmetrics(Font, 0); 
  int fHeight = smallmetrics.lineSpacing();
  QString tmp;
  int i, PortDistance = 60;

  // adjust branch number
  int Num = Props.at(1)->Value.toInt();
  if(Num < 1) Num = 1;
  else if(Num > 4) {
    PortDistance = 40;
    if(Num > 20) Num = 20;
  }
  Props.at(1)->Value = QString::number(Num);

  // adjust actual number of properties
  int NumProps = (Props.count() - 2) / 2; // current number of properties
  if (NumProps < Num) {
    for(i = NumProps; i < Num; i++) {
      Props.append(new Property("I"+QString::number(i+1), "0", false,
		QObject::tr("current equation") + " " +QString::number(i+1)));
      Props.append(new Property("Q"+QString::number(i+1), "0", false,
		QObject::tr("charge equation") + " " +QString::number(i+1)));
    }
  } else {
    for(i = Num; i < NumProps; i++) {
      Props.removeLast();
      Props.removeLast();
    }
  }

  // adjust property names
  Property * p1 = Props.at(2);
  for(i = 1; i <= Num; i++) {
    p1->Name = "I"+QString::number(i);
    p1 = Props.next();
    p1->Name = "Q"+QString::number(i);
    p1 = Props.next();
  }

  // draw symbol
  int h = (PortDistance/2)*((Num-1)) + PortDistance/2; // total component half-height
  Lines.append(new qucs::Line(-15, -h, 15, -h,QPen(Qt::darkBlue,2))); // top side
  Lines.append(new qucs::Line( 15, -h, 15,  h,QPen(Qt::darkBlue,2))); // right side
  Lines.append(new qucs::Line(-15,  h, 15,  h,QPen(Qt::darkBlue,2))); // bottom side
  Lines.append(new qucs::Line(-15, -h,-15,  h,QPen(Qt::darkBlue,2))); // left side

  i=0;
  int y = PortDistance/2-h, yh; // y is the actual vertical center
  while(i<Num) { // for every branch
    i++;
    // left connection with port
    Lines.append(new qucs::Line(-30, y,-15, y,QPen(Qt::darkBlue,2)));
    Ports.append(new Port(-30, y));
    // small black arrow inside the box
    Lines.append(new qucs::Line( 7,y-3, 10, y,QPen(Qt::black,1)));
    Lines.append(new qucs::Line( 7,y+3, 10, y,QPen(Qt::black,1)));
    Lines.append(new qucs::Line(-10, y, 10, y,QPen(Qt::black,1)));

    if (i > 1) {
      yh = y-PortDistance/2; // bottom of the branch box
      // draw horizontal separation between boxes
      Lines.append(new qucs::Line(-15, yh, 15, yh, QPen(Qt::darkBlue,2)));
    }
    // right connection with port
    Lines.append(new qucs::Line( 15, y, 30, y,QPen(Qt::darkBlue,2)));
    Ports.append(new Port( 30, y));
    // add branch number near the right connection port
    Texts.append(new Text(25,y-fHeight-2,QString::number(i))); // left-aligned
    // move the vertical center down for the next branch
    y += PortDistance;
  }

  x1 = -30; y1 = -h-2;
  x2 =  30; y2 =  h+2;
  // compute component name text position - normal size font
  QFontMetrics  metrics(QucsSettings.font, 0);  // use the screen-compatible metric
  tx = x1+4;
  ty = y1 - 2*metrics.lineSpacing() - 4;
}
