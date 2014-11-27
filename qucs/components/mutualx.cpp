/***************************************************************************
                                 mutualx.cpp
                                -----------
    begin                : Mon Nov 24 2014
    copyright            : (C) 2010 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
    adapted for Qucs by  : Vadim Kiznetsov
    email                : ra3xdh@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "mutualx.h"
#include "node.h"

MutualX::MutualX()
{
  Description = QObject::tr("several mutual inductors");

  Model = "MUTX";
  Name  = "Tr";

  // must be the first property!
  Props.append(new Property("coils", "4", false,
		QObject::tr("number of mutual inductances")));

  Props.append(new Property("L", "1 mH", false, " "));

  createSymbol();
}

// --------------------------------------------------------
Element* MutualX::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("N Mutual Inductors");
  BitmapFile = (char *) "mutualx";

  if(getNewOne) {
      MutualX* p =  new MutualX();
      p->Props.at(0)->Value = "4";
      p->recreate(0);
      return p;
  }
  return 0;
}

Component* MutualX::newOne()
{
    MutualX *p  = new MutualX();
    p->Props.at(0)->Value=Props.at(0)->Value;
    p->recreate(0);
    return p;
}

QString MutualX::netlist()
{
    QString s = Model + ":" + Name;

    // output all node names
    foreach(Port *p1, Ports) {
      s += " "+p1->Connection->Name;   // node names
    }

    int coils = Props.at(0)->Value.toInt();

    QString L="";
    for (int i=0;i<coils;i++) {
        L += Props.at(i+1)->Value + ";";
    }
    L.chop(1);

    QString k="";

    QVector< QVector<QString> > vec_k(coils);
    QVector<QString> vec1_k(coils);
    vec1_k.fill("0.0");
    vec_k.fill(vec1_k);


    for (int i=0, state=1;i<coils;i++) {
        for (int j=i;j<coils;j++,state++) {
            if (i==j) {
                vec_k[i][j] = "1.0"; // for self-inductance
                state--;
            } else {
                vec_k[i][j] = Props.at(coils+state)->Value; // for mutual inductances
            }                                      // Parameters need to be in prpoer sequence
                            // otherwise netlist will be broken. Maybe rewrite it?
        }
        for (int j=0;j<i;j++) {
            vec_k[i][j]= vec_k[j][i];
        }
    }

    foreach (vec1_k,vec_k) {
        QString ki;
        foreach (ki,vec1_k) {
            k += ki+";";
        }
    }
    k.chop(1);

    s += QString(" L=\"[%1]\" k=\"[%2]\"\n").arg(L,k);

    return s;
}

// --------------------------------------------------------
void MutualX::createSymbol()
{
  // adjust port number
  int Num = Props.first()->Value.toInt();
  if(Num < 1)
    Num = 1;
  else if(Num > 8)
    Num = 8;
  Props.first()->Value = QString::number(Num);

  // adjust property number
  int i, j, NumProps;
  j = Props.count();
  NumProps = Num + Num * (Num - 1) / 2 + 1;
  for(i = j; i < NumProps; i++)
    Props.append(new Property("L", "0.0", false, " "));
  for(i = NumProps; i < j; i++)
    Props.removeLast();

  // set property names and descriptions
  Property *pp = Props.first();
  for(i = 1; i <= Num; i++) {
    pp = Props.next();
    pp->Name = "L" + QString::number(i);
    pp->Description =
        QObject::tr("inductance of coil") + " " + QString::number(i);
  }
  for(i = 1; i < Num; i++)
    for(j = i+1; j <= Num; j++) {
      pp = Props.next();
      pp->Name = "k" + QString::number(i) + QString::number(j);
      pp->Description =
          QObject::tr("coupling factor between coil %1 and coil %2").arg(i).arg(j);
    }

  // draw symbol
  int x = -10 * (Num-1);
  Texts.append(new Text(x-9,-22,"1"));

  x1 = x-6;  y1 = -30;
  x2 = 10-x; y2 =  30;

  tx = x2+4;
  ty = y1+4;

  x -= 6;
  for(i=0; i<Num; i++) {
    Arcs.append(new Arc(x,-18,12,12, 16*270,16*180, QPen(Qt::darkBlue,2)));
    Arcs.append(new Arc(x, -6,12,12, 16*270,16*180, QPen(Qt::darkBlue,2)));
    Arcs.append(new Arc(x,  6,12,12, 16*270,16*180, QPen(Qt::darkBlue,2)));

	x += 6;
    Lines.append(new Line(x,-18,x,-30,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(x, 18,x, 30,QPen(Qt::darkBlue,2)));

    Ports.append(new Port(x,-30));
    Ports.append(new Port(x, 30));
	x += 14;
  }
}
