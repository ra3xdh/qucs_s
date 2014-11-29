/***************************************************************************
                                 mutualx.cpp
                                -----------
    begin                : Mon Nov 24 2014
    copyright            : (C) 2010 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
    copyright            : (C) 2014 by Vadim Kuznetsov
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
#include "math.h"

MutualX::MutualX()
{
  Description = QObject::tr("several mutual inductors");

  Model = "MUTX";
  Name  = "Tr";

  const int init_coils=4; // initial number of coils
  // must be the first property!
  Props.append(new Property("coils", QString::number(init_coils), false,
		QObject::tr("number of mutual inductances")));

  for (int i=1;i<=init_coils; i++) {
      Props.append(new Property("L"+QString::number(i), "1 mH", false,
                                QObject::tr("inductance of coil") + " " + QString::number(i)));
  }

  for(int i = 1; i < init_coils; i++)
    for(int j = i+1; j <= init_coils; j++) {
       QString nam = "k" + QString::number(i) + QString::number(j);
       QString desc = QObject::tr("coupling factor between coil %1 and coil %2").arg(i).arg(j);
       Props.append(new Property(nam,"0.9",false,desc));
    }

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

    QString **k_matrix = new QString*[coils];
    for (int i=0;i<coils;i++) {
        k_matrix[i]= new QString[coils];
    }


    for (int i=0, state=1;i<coils;i++) {
        for (int j=i;j<coils;j++,state++) {
            if (i==j) {
                k_matrix[i][j] = "1.0"; // for self-inductance
                state--;
            } else {
                k_matrix[i][j] = Props.at(coils+state)->Value; // for mutual inductances
            }

        }
        for (int j=0;j<i;j++) {
            k_matrix[i][j]= k_matrix[j][i];
        }
    }

    for (int i=0;i<coils;i++) {
        for (int j=0;j<coils;j++) {
            k += k_matrix[i][j]+";";
        }
    }
    k.chop(1);

    for (int i=0;i<coils;i++) {
        delete [] k_matrix[i];
    }
    delete [] k_matrix;


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

  int NumProps,oldNumProps;
  oldNumProps = Props.count();
  NumProps = Num + Num * (Num - 1) / 2 + 1;
  if (oldNumProps!=NumProps) { // Coils count was changed
      int oldCoils = rint(0.5*(sqrt(8*oldNumProps-7)-1.0)); // calculate old number of coils
                                                            // we need to solve quadratic equation
      int dCoils = abs(oldCoils - Num);          // how many coils were added/removed?
      int k_cnt = (Num*(Num-1))/2;
      int old_k_cnt = (oldCoils*(oldCoils-1))/2;
      int delta_cnt = abs(old_k_cnt-k_cnt);

      if (oldCoils>Num) { // reduce coils number

          for (int i=0;i<dCoils;i++) {
              Props.remove(oldCoils);
          }
          for (int i=0;i<delta_cnt;i++) {
              Props.removeLast();
          }
      } else { // add new coils
          for(int i = 0; i < dCoils; i++) { // add new properties for coils
              Props.insert(oldCoils+1, new Property("L"+QString::number(Num-i), "1 mH", false,
                                                    QObject::tr("inductance of coil") + " " + QString::number(Num-i)));
          }
          for (int i=0;i<delta_cnt;i++) { // and for coupling coeffs.
              Props.append(new Property("k", "0.9", false, " "));
          }
      }

      for(int i = 1,state=1; i < Num; i++) // Adjust coupling coeffs names
        for(int j = i+1; j <= Num; j++,state++) {
          Props.at(Num+state)->Name = "k" + QString::number(i) + QString::number(j);
          Props.at(Num+state)->Description =
              QObject::tr("coupling factor between coil %1 and coil %2").arg(i).arg(j);
        }
  }

  // draw symbol
  int x = -10 * (Num-1);
  Texts.append(new Text(x-9,-22,"1"));

  x1 = x-6;  y1 = -30;
  x2 = 10-x; y2 =  30;

  tx = x2+4;
  ty = y1+4;

  x -= 6;
  for(int i=0; i<Num; i++) {
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
