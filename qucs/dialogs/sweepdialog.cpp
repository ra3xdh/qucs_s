/***************************************************************************
                               sweepdialog.cpp
                              -----------------
    begin                : Sat Aug 13 2005
    copyright            : (C) 2005 by Michael Margraf
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
#include "sweepdialog.h"
#include "schematic.h"
#include "qucs.h"

#include <QGridLayout>
#include "main.h"
#include "../diagrams/graph.h"

#include <QLabel>
#include <QLineEdit>
#include <QValidator>
#include <QPushButton>

// SpinBoxes are used to show the calculated bias points at the given set of sweep points
mySpinBox::mySpinBox(int Min, int Max, int Step, double *Val, QWidget *Parent)
          : QSpinBox(Parent)
{
  setMinimum(Min);
  setMaximum(Max);
  setSingleStep(Step); 
  Values = Val;
  ValueSize = Max;
  //editor()->
	//	setReadOnly(true);
}


#include <iostream>
using namespace std;
QString mySpinBox::textFromValue(int Val)
{
  cout<<"Values + Val"<<*(Values+Val)<<endl;
  return QString::number(*(Values+Val));
}

QValidator::State mySpinBox::validate ( QString & text, int & pos ) const
{
  if(pos>ValueSize)return QValidator::Invalid; 
  if(QString::number(*(Values+pos))==text)
  return QValidator::Acceptable;
  else return QValidator::Invalid;
}


SweepDialog::SweepDialog(Schematic *Doc_)
			: QDialog(Doc_)//, 0, TRUE, Qt::WDestructiveClose)
{
  Doc = Doc_;

  pGraph = setBiasPoints();
  // if simulation has no sweeps, terminate dialog before showing it
  if(pGraph->cPointsX.count() == 0) {
    reject();
    return;
  }
  if(pGraph->cPointsX.count() <= 1)
    if(pGraph->cPointsX.getFirst()->count <= 1) {
      reject();
      return;
    }


  setWindowTitle(tr("Bias Points"));

  int i = 0;
  // ...........................................................
  QGridLayout *all = new QGridLayout(this);//, pGraph->cPointsX.count()+2,2,3,3);
  all->setMargin(5);
  all->setSpacing(5);
  all->setColStretch(1,5);

  DataX *pD;
  mySpinBox *Box;
  
  for(pD = pGraph->cPointsX.first(); pD!=0; pD = pGraph->cPointsX.next()) {
    all->addWidget(new QLabel(pD->Var, this), i,0);
  //cout<<"count: "<<pD->count-1<<", points: "<<*pD->Points<<endl;
    //works only for linear:
    /*double Min = pD->Points[0];
    double Max = pD->Points[pD->count-1];
    double Step = (Max-Min)/(pD->count);
    cout<<"Min: "<<Min<<", Max: "<<Max<<", Step: "<<Step<<endl;
    Box = new mySpinBox(Min, Max, Step, pD->Points, this);*/
    Box = new mySpinBox(0, pD->count-1, 1, pD->Points, this);
    Box->setValue(0);  
    all->addWidget(Box, i++,1);
    connect(Box, SIGNAL(valueChanged(int)), SLOT(slotNewValue(int)));
    BoxList.append(Box);
  }

  // ...........................................................
  all->setRowStretch(i,5);
  QPushButton *ButtClose = new QPushButton(tr("Close"), this);
  all->addMultiCellWidget(ButtClose, i+1,i+1, 0,1);
  connect(ButtClose, SIGNAL(clicked()), SLOT(accept()));
  show();
}

SweepDialog::~SweepDialog()
{
  delete pGraph;

  for(double *p = ValueList.first(); p!=0; p = ValueList.next())
    delete p;
}

// ---------------------------------------------------------------
void SweepDialog::slotNewValue(int)
{
  DataX *pD = pGraph->cPointsX.first();
  int Factor = 1, Index = 0;
  for(mySpinBox *pb = BoxList.first(); pb!=0; pb = BoxList.next()) {
    Index  += pb->value() * Factor;
    Factor *= pD->count;
  }
  Index *= 2;  // because of complex values

  double *p = ValueList.first();
  for(Node *pn = NodeList.first(); pn!=0; pn = NodeList.next()) {
    pn->Name = num2str(*(p+Index));
    if(pn->x1 & 0x10)
      pn->Name += "A";
    else
      pn->Name += "V";
    p = ValueList.next();
  }

  Doc->viewport()->update();
}

// ---------------------------------------------------
Graph* SweepDialog::setBiasPoints()
{
  // When this function is entered, a simulation was performed.
  // Thus, the node names are still in "node->Name".

  bool hasNoComp;
  Graph *pg = new Graph("");
  Diagram *Diag = new Diagram();
  QFileInfo Info(Doc->DocName);
  QString DataSet = Info.dirPath() + QDir::separator() + Doc->DataSet;

  Node *pn;
  Element *pe;
  // create DC voltage for all nodes
  for(pn = Doc->Nodes->first(); pn != 0; pn = Doc->Nodes->next()) {
    if(pn->Name.isEmpty()) continue;

    pn->x1 = 0;
    if(pn->Connections.count() < 2) {
      pn->Name = "";  // no text at open nodes
      continue;
    }
    else {
      hasNoComp = true;
      for(pe = pn->Connections.first(); pe!=0; pe = pn->Connections.next())
        if(pe->Type == isWire) {
          if( ((Wire*)pe)->isHorizontal() )  pn->x1 |= 2;
        }
        else {
          if( ((Component*)pe)->Model == "GND" ) {
            hasNoComp = true;   // no text at ground symbol
            break;
          }

          if(pn->cx < pe->cx)  pn->x1 |= 1;  // to the right is no room
          hasNoComp = false;
        }
      if(hasNoComp) {  // text only were a component is connected
        pn->Name = "";
        continue;
      }
    }

    pg->Var = pn->Name + ".V";
    if(Diag->loadVarData(DataSet, pg)) {
      pn->Name = num2str(*(pg->cPointsY)) + "V";
      NodeList.append(pn);             // remember node ...
      ValueList.append(pg->cPointsY);  // ... and all of its values
      pg->cPointsY = 0;   // do not delete it next time !
    }
    else
      pn->Name = "0V";


    for(pe = pn->Connections.first(); pe!=0; pe = pn->Connections.next())
      if(pe->Type == isWire) {
        if( ((Wire*)pe)->Port1 != pn )  // no text at next node
          ((Wire*)pe)->Port1->Name = "";
        else  ((Wire*)pe)->Port2->Name = "";
      }
  }


  // create DC current through each probe
  Component *pc;
  for(pc = Doc->Components->first(); pc != 0; pc = Doc->Components->next())
    if(pc->Model == "IProbe") {
      pn = pc->Ports.first()->Connection;
      if(!pn->Name.isEmpty())   // preserve node voltage ?
        pn = pc->Ports.at(1)->Connection;

      pn->x1 = 0x10;   // mark current
      pg->Var = pc->Name + ".I";
      if(Diag->loadVarData(DataSet, pg)) {
        pn->Name = num2str(*(pg->cPointsY)) + "A";
        NodeList.append(pn);             // remember node ...
        ValueList.append(pg->cPointsY);  // ... and all of its values
        pg->cPointsY = 0;   // do not delete it next time !
      }
      else
        pn->Name = "0A";

      for(pe = pn->Connections.first(); pe!=0; pe = pn->Connections.next())
        if(pe->Type == isWire) {
          if( ((Wire*)pe)->isHorizontal() )  pn->x1 |= 2;
        }
        else {
          if(pn->cx < pe->cx)  pn->x1 |= 1;  // to the right is no room
        }
    }


  Doc->showBias = 1;
  delete Diag;
  return pg;
}
