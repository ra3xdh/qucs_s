/***************************************************************************
                            optimizedialog.cpp
                           --------------------
    begin                : Sun Jul 30 2006
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
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "optimizedialog.h"
#include "opt_sim.h"
#include "schematic.h"

#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <Q3ListView>
#include <QTabWidget>
#include <QValidator>
#include <QPushButton>
#include <QMessageBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>


OptimizeDialog::OptimizeDialog(Optimize_Sim *c_, Schematic *d_)
			: QDialog(d_, 0, TRUE, Qt::WDestructiveClose)
{
  Comp = c_;
  Doc  = d_;
  changed = false;
  setCaption(tr("Edit Optimization Properties"));

  Expr.setPattern("[\\w_]+");
  Validator = new QRegExpValidator(Expr, this);
  numVal = new QDoubleValidator(this);
  intVal = new QIntValidator(this);

  all = new QVBoxLayout(); // to provide the neccessary size
  QTabWidget *t = new QTabWidget();
  all->addWidget(t);
  this->setLayout(all);

  // ...........................................................
  QWidget *Tab1 = new QWidget(t);
  QGridLayout *gp1 = new QGridLayout();
  Tab1->setLayout(gp1);

  NameEdit = new QLineEdit();
  NameEdit->setValidator(Validator);
  SimEdit = new QComboBox(Tab1);
  SimEdit->setEditable(true);

  gp1->addWidget(new QLabel(tr("Name:"), Tab1), 0, 0);
  gp1->addWidget(NameEdit, 0, 1);
  gp1->addWidget(new QLabel(tr("Simulation:"), Tab1), 1, 0);
  gp1->addWidget(SimEdit, 1, 1);

  t->addTab(Tab1, tr("General"));

  // ...........................................................
  QWidget *Tab2 = new QWidget(t);
  QGridLayout *gp2 = new QGridLayout();
  Tab2->setLayout(gp2);

  MethodCombo = new QComboBox();
  MethodCombo->insertItem("DE/best/1/exp");
  MethodCombo->insertItem("DE/rand/1/exp");
  MethodCombo->insertItem("DE/rand-to-best/1/exp");
  MethodCombo->insertItem("DE/best/2/exp");
  MethodCombo->insertItem("DE/rand/1/exp");
  MethodCombo->insertItem("DE/best/1/bin");
  MethodCombo->insertItem("DE/rand/1/bin");
  MethodCombo->insertItem("DE/rand-to-best/1/bin");
  MethodCombo->insertItem("DE/best/2/bin");
  MethodCombo->insertItem("DE/rand/2/bin");

  gp2->addWidget(new QLabel(tr("Method:")), 0,0);
  gp2->addWidget(MethodCombo,0,1);

  IterEdit = new QLineEdit();
  IterEdit->setValidator(intVal);

  gp2->addWidget(new QLabel(tr("Maximum number of iterations:")), 1,0);
  gp2->addWidget(IterEdit,1,1);

  RefreshEdit = new QLineEdit();
  RefreshEdit->setValidator(intVal);

  gp2->addWidget(new QLabel(tr("Output refresh cycle:")), 2,0);
  gp2->addWidget(RefreshEdit,2,1);

  ParentsEdit = new QLineEdit();
  ParentsEdit->setValidator(intVal);

  gp2->addWidget(new QLabel(tr("Number of parents:")), 3,0);
  gp2->addWidget(ParentsEdit,3,1);

  ConstEdit = new QLineEdit();
  ConstEdit->setValidator(new QDoubleValidator(0.0,2.0,20,ConstEdit));

  gp2->addWidget(new QLabel(tr("Constant F:")), 4,0);
  gp2->addWidget(ConstEdit,4,1);

  CrossEdit = new QLineEdit();
  CrossEdit->setValidator(new QDoubleValidator(0.0,1.0,20,CrossEdit));

  gp2->addWidget(new QLabel(tr("Crossing over factor:")), 5,0);
  gp2->addWidget(CrossEdit,5,1);

  SeedEdit = new QLineEdit();
  SeedEdit->setValidator(numVal);

  gp2->addWidget(new QLabel(tr("Pseudo random number seed:")), 6,0);
  gp2->addWidget(SeedEdit,6,1);

  CostVarEdit = new QLineEdit();
  CostVarEdit->setValidator(numVal);

  gp2->addWidget(new QLabel(tr("Minimum cost variance:")), 7,0);
  gp2->addWidget(CostVarEdit,7,1);

  CostObjEdit = new QLineEdit();
  CostObjEdit->setValidator(numVal);

  gp2->addWidget(new QLabel(tr("Cost objectives:")), 8,0);
  gp2->addWidget(CostObjEdit,8,1);

  CostConEdit = new QLineEdit();
  CostConEdit->setValidator(numVal);

  gp2->addWidget(new QLabel(tr("Cost constraints:")), 9,0);
  gp2->addWidget(CostConEdit,9,1);

  t->addTab(Tab2, tr("Algorithm"));

  // ...........................................................
  QWidget *Tab3 = new QWidget(t);
  QGridLayout *gp3 = new QGridLayout();
  Tab3->setLayout(gp3);

  VarList = new Q3ListView();
  VarList->addColumn(tr("Name"));
  VarList->addColumn(tr("active"));
  VarList->addColumn(tr("initial"));
  VarList->addColumn(tr("min"));
  VarList->addColumn(tr("max"));
  VarList->addColumn(tr("Type"));
  VarList->setSorting(-1);   // no sorting
  gp3->addMultiCellWidget(VarList,0,0,0,2);
  connect(VarList, SIGNAL(selectionChanged(Q3ListViewItem*)),
                   SLOT(slotEditVariable(Q3ListViewItem*)));

  VarNameEdit = new QLineEdit();
  VarNameEdit->setValidator(Validator);
  connect(VarNameEdit, SIGNAL(textChanged(const QString&)),
          SLOT(slotChangeVarName(const QString&)));
  VarActiveCheck = new QCheckBox(tr("active"));
  VarActiveCheck->setChecked(true);
  connect(VarActiveCheck, SIGNAL(toggled(bool)),
          SLOT(slotChangeVarActive(bool)));

  QHBoxLayout *VarLine = new QHBoxLayout();
  VarLine->setSpacing(3);
  VarLine->addWidget(new QLabel(tr("Name:")));
  VarLine->addWidget(VarNameEdit);
  VarLine->addWidget(VarActiveCheck);
  gp3->addMultiCellLayout(VarLine, 1,1,0,2);

  VarInitEdit = new QLineEdit();
  VarInitEdit->setValidator(numVal);
  connect(VarInitEdit, SIGNAL(textChanged(const QString&)),
          SLOT(slotChangeVarInit(const QString&)));
  VarMinEdit = new QLineEdit();
  VarMinEdit->setValidator(numVal);
  connect(VarMinEdit, SIGNAL(textChanged(const QString&)),
          SLOT(slotChangeVarMin(const QString&)));
  VarMaxEdit = new QLineEdit(Tab3);
  VarMaxEdit->setValidator(numVal);
  connect(VarMaxEdit, SIGNAL(textChanged(const QString&)),
          SLOT(slotChangeVarMax(const QString&)));

  gp3->addWidget(new QLabel(tr("initial:")), 2,0);
  gp3->addWidget(new QLabel(tr("min:")), 2,1);
  gp3->addWidget(new QLabel(tr("max:")), 2,2);
  gp3->addWidget(VarInitEdit,3,0);
  gp3->addWidget(VarMinEdit,3,1);
  gp3->addWidget(VarMaxEdit,3,2);

  VarTypeCombo = new QComboBox();
  VarTypeCombo->insertItem(tr("linear double"));
  VarTypeCombo->insertItem(tr("logarithmic double"));
  VarTypeCombo->insertItem(tr("linear integer"));
  VarTypeCombo->insertItem(tr("logarithmic integer"));
  connect(VarTypeCombo, SIGNAL(activated(const QString&)),
          SLOT(slotChangeVarType(const QString&)));

  QPushButton *AddVar_Butt = new QPushButton(tr("Add"));
  connect(AddVar_Butt, SIGNAL(clicked()), SLOT(slotAddVariable()));
  QPushButton *DelVar_Butt = new QPushButton(tr("Delete"));
  connect(DelVar_Butt, SIGNAL(clicked()), SLOT(slotDeleteVariable()));

  QHBoxLayout *VarButtons = new QHBoxLayout();
  VarButtons->setSpacing(3);
  VarButtons->addWidget(new QLabel(tr("Type:")));
  VarButtons->addWidget(VarTypeCombo);
  VarButtons->addStretch();
  VarButtons->addWidget(AddVar_Butt);
  VarButtons->addWidget(DelVar_Butt);
  gp3->addMultiCellLayout(VarButtons, 4,4,0,2);

  t->addTab(Tab3, tr("Variables"));

  // ...........................................................
  QWidget *Tab4 = new QWidget(t);
  QGridLayout *gp4 = new QGridLayout();
  Tab4->setLayout(gp4);

  GoalList = new Q3ListView();
  GoalList->addColumn(tr("Name"));
  GoalList->addColumn(tr("Type"));
  GoalList->addColumn(tr("Value"));
  GoalList->setSorting(-1);   // no sorting
  connect(GoalList, SIGNAL(selectionChanged(Q3ListViewItem*)),
                    SLOT(slotEditGoal(Q3ListViewItem*)));
  gp4->addMultiCellWidget(GoalList,0,0,0,2);

  GoalNameEdit = new QLineEdit();
  GoalNameEdit->setValidator(Validator);
  connect(GoalNameEdit, SIGNAL(textChanged(const QString&)),
          SLOT(slotChangeGoalName(const QString&)));

  gp4->addWidget(new QLabel(tr("Name:")), 1,0);
  gp4->addWidget(GoalNameEdit,1,1);

  GoalNumEdit = new QLineEdit();
  GoalNumEdit->setValidator(numVal);
  connect(GoalNumEdit, SIGNAL(textChanged(const QString&)),
          SLOT(slotChangeGoalNum(const QString&)));

  gp4->addWidget(new QLabel(tr("Value:")), 2,0);
  gp4->addWidget(GoalNumEdit,2,1);

  GoalTypeCombo = new QComboBox();
  GoalTypeCombo->insertItem(tr("minimize"));
  GoalTypeCombo->insertItem(tr("maximize"));
  GoalTypeCombo->insertItem(tr("less"));
  GoalTypeCombo->insertItem(tr("greater"));
  GoalTypeCombo->insertItem(tr("equal"));
  GoalTypeCombo->insertItem(tr("monitor"));
  connect(GoalTypeCombo, SIGNAL(activated(const QString&)),
          SLOT(slotChangeGoalType(const QString&)));
  gp4->addWidget(GoalTypeCombo,2,2);

  QPushButton *AddGoal_Butt = new QPushButton(tr("Add"));
  connect(AddGoal_Butt, SIGNAL(clicked()), SLOT(slotAddGoal()));
  QPushButton *DelGoal_Butt = new QPushButton(tr("Delete"));
  connect(DelGoal_Butt, SIGNAL(clicked()), SLOT(slotDeleteGoal()));

  QHBoxLayout *GoalButtons = new QHBoxLayout();
  GoalButtons->setSpacing(3);
  GoalButtons->addStretch();
  GoalButtons->addWidget(AddGoal_Butt);
  GoalButtons->addWidget(DelGoal_Butt);
  gp4->addMultiCellLayout(GoalButtons, 3,3,0,2);

  t->addTab(Tab4, tr("Goals"));

  // ...........................................................
  // buttons on the bottom of the dialog (independent of the TabWidget)
  QHBoxLayout *Butts = new QHBoxLayout();
  Butts->setSpacing(3);
  Butts->setMargin(3);

  QPushButton *OkButt = new QPushButton(tr("OK"));
  connect(OkButt, SIGNAL(clicked()), SLOT(slotOK()));
  QPushButton *ApplyButt = new QPushButton(tr("Apply"));
  connect(ApplyButt, SIGNAL(clicked()), SLOT(slotApply()));
  QPushButton *CancelButt = new QPushButton(tr("Cancel"));
  connect(CancelButt, SIGNAL(clicked()), SLOT(slotCancel()));

  Butts->addStretch();
  Butts->addWidget(OkButt);
  Butts->addWidget(ApplyButt);
  Butts->addWidget(CancelButt);
  all->addLayout(Butts);

  OkButt->setFocus();

  // ...........................................................

  Component *pc;
  for(pc=Doc->Components->first(); pc!=0; pc=Doc->Components->next())
    if(pc != Comp)
      if(pc->Model[0] == '.' && pc->Model != ".Opt")
        SimEdit->insertItem(pc->Name);

  Property *pp;
  pp = Comp->Props.at(0);
  if(!pp->Value.isEmpty())
    SimEdit->setCurrentText(pp->Value);

  pp = Comp->Props.at(1);
  if(!pp->Value.isEmpty()) {
    MethodCombo->setCurrentItem(pp->Value.section('|',0,0).toInt()-1);
    IterEdit->setText(pp->Value.section('|',1,1));
    RefreshEdit->setText(pp->Value.section('|',2,2));
    ParentsEdit->setText(pp->Value.section('|',3,3));
    ConstEdit->setText(pp->Value.section('|',4,4));
    CrossEdit->setText(pp->Value.section('|',5,5));
    SeedEdit->setText(pp->Value.section('|',6,6));
    CostVarEdit->setText(pp->Value.section('|',7,7));
    CostObjEdit->setText(pp->Value.section('|',8,8));
    CostConEdit->setText(pp->Value.section('|',9,9));
  }

  NameEdit->setText(Comp->Name);

  for(pp = Comp->Props.at(2); pp != 0; pp = Comp->Props.next()) {
    if(pp->Name == "Var") {
      new Q3ListViewItem(VarList, pp->Value.section('|',0,0),
        pp->Value.section('|',1,1) == "yes" ? tr("yes") : tr("no"),
	pp->Value.section('|',2,2),
        pp->Value.section('|',3,3), pp->Value.section('|',4,4),
        ((pp->Value.section('|',5,5)=="LIN_DOUBLE")?tr("linear double") :
        ((pp->Value.section('|',5,5)=="LOG_DOUBLE")?tr("logarithmic double") :
        ((pp->Value.section('|',5,5)=="LIN_INT")?tr("linear integer") :
	 tr("logarithmic integer")))));
    }
    if(pp->Name == "Goal") {
      new Q3ListViewItem(GoalList, pp->Value.section('|',0,0),
        ((pp->Value.section('|',1,1) == "MIN") ? tr("minimize") :
        ((pp->Value.section('|',1,1) == "MAX") ? tr("maximize") :
        ((pp->Value.section('|',1,1) == "LE") ? tr("less") :
        ((pp->Value.section('|',1,1) == "GE") ? tr("greater") :
        ((pp->Value.section('|',1,1) == "EQ") ? tr("equal") :
	 tr("monitor")))))),
	pp->Value.section('|',2,2));
    }
  }

  resize(300, 250);
}

OptimizeDialog::~OptimizeDialog()
{
  delete all;
  delete numVal;
  delete intVal;
  delete Validator;
}

// -----------------------------------------------------------
void OptimizeDialog::slotEditVariable(Q3ListViewItem *Item)
{
  VarNameEdit->blockSignals(true);

  if(Item == 0) {
    VarNameEdit->clear();
    VarActiveCheck->setChecked(true);
    VarInitEdit->clear();
    VarMinEdit->clear();
    VarMaxEdit->clear();
    VarTypeCombo->setCurrentItem(0);
    VarNameEdit->blockSignals(false);
    return;
  }

  VarNameEdit->setText(Item->text(0));
  VarActiveCheck->setChecked(Item->text(1) == tr("yes"));
  VarInitEdit->setText(Item->text(2));
  VarMinEdit->setText(Item->text(3));
  VarMaxEdit->setText(Item->text(4));
  VarTypeCombo->setCurrentText(Item->text(5));
  VarNameEdit->blockSignals(false);
}

// -----------------------------------------------------------
void OptimizeDialog::slotAddVariable()
{
  if(VarNameEdit->text().isEmpty() || VarInitEdit->text().isEmpty() ||
        VarMinEdit->text().isEmpty() || VarMaxEdit->text().isEmpty()) {
    QMessageBox::critical(this, tr("Error"),
       tr("Every text field must be non-empty!"));
    return;
  }

  Q3ListViewItem *item;
  for(item = VarList->firstChild(); item != 0; item = item->itemBelow())
    if(item->text(0) == VarNameEdit->text()) {
      QMessageBox::critical(this, tr("Error"),
         tr("Variable \"%1\" already in list!").arg(VarNameEdit->text()));
      return;
    }


  new Q3ListViewItem(VarList, VarNameEdit->text(),
      VarActiveCheck->isChecked() ? tr("yes") : tr("no"),
      VarInitEdit->text(), VarMinEdit->text(), VarMaxEdit->text(),
      VarTypeCombo->currentText());

  slotEditVariable(0);   // clear entry fields
  VarList->clearSelection();
}

// -----------------------------------------------------------
void OptimizeDialog::slotDeleteVariable()
{
  Q3ListViewItem *next_item = 0;

  Q3ListViewItem *Item = VarList->selectedItem();
  if(Item) {
    next_item = Item->itemBelow();
    if(next_item == 0) next_item = Item->itemAbove();
    VarList->takeItem(Item);  // remove from ListView
    delete Item;              // delete item
  }

  slotEditVariable(next_item);
}

// -----------------------------------------------------------
void OptimizeDialog::slotChangeVarActive(bool On)
{
  Q3ListViewItem *Item = VarList->selectedItem();
  if(Item == 0) return;

  Item->setText(1, On ? tr("yes") : tr("no"));
}

// -----------------------------------------------------------
void OptimizeDialog::slotChangeVarName(const QString&)
{
  VarList->clearSelection();
}

// -----------------------------------------------------------
void OptimizeDialog::slotChangeVarInit(const QString& Text)
{
  Q3ListViewItem *Item = VarList->selectedItem();
  if(Item == 0) return;

  Item->setText(2, Text);
}

// -----------------------------------------------------------
void OptimizeDialog::slotChangeVarMin(const QString& Text)
{
  Q3ListViewItem *Item = VarList->selectedItem();
  if(Item == 0) return;

  Item->setText(3, Text);
}

// -----------------------------------------------------------
void OptimizeDialog::slotChangeVarMax(const QString& Text)
{
  Q3ListViewItem *Item = VarList->selectedItem();
  if(Item == 0) return;

  Item->setText(4, Text);
}

// -----------------------------------------------------------
void OptimizeDialog::slotChangeVarType(const QString& Text)
{
  Q3ListViewItem *Item = VarList->selectedItem();
  if(Item == 0) return;

  Item->setText(5, Text);
}

// -----------------------------------------------------------
void OptimizeDialog::slotEditGoal(Q3ListViewItem *Item)
{
  GoalNameEdit->blockSignals(true);

  if(Item == 0) {
    GoalNameEdit->clear();
    GoalTypeCombo->setCurrentItem(0);
    GoalNumEdit->clear();
    GoalNameEdit->blockSignals(false);
    return;
  }

  GoalNameEdit->setText(Item->text(0));
  GoalTypeCombo->setCurrentText(Item->text(1));
  GoalNumEdit->setText(Item->text(2));
  GoalNameEdit->blockSignals(false);
}

// -----------------------------------------------------------
void OptimizeDialog::slotAddGoal()
{
  if(GoalNameEdit->text().isEmpty() || GoalNumEdit->text().isEmpty()) {
    QMessageBox::critical(this, tr("Error"),
       tr("Every text field must be non-empty!"));
    return;
  }

  Q3ListViewItem *item;
  for(item = GoalList->firstChild(); item != 0; item = item->itemBelow())
    if(item->text(0) == GoalNameEdit->text()) {
      QMessageBox::critical(this, tr("Error"),
         tr("Goal \"%1\" already in list!").arg(GoalNameEdit->text()));
      return;
    }


  new Q3ListViewItem(GoalList, GoalNameEdit->text(),
      GoalTypeCombo->currentText(), GoalNumEdit->text());

  slotEditGoal(0);    // clear entry fields
  GoalList->clearSelection();
}

// -----------------------------------------------------------
void OptimizeDialog::slotDeleteGoal()
{
  Q3ListViewItem *next_item = 0;

  Q3ListViewItem *Item = GoalList->selectedItem();
  if(Item) {
    next_item = Item->itemBelow();
    if(next_item == 0) next_item = Item->itemAbove();
    GoalList->takeItem(Item); // remove from ListView
    delete Item;              // delete item
  }

  slotEditGoal(next_item);
}

// -----------------------------------------------------------
void OptimizeDialog::slotChangeGoalName(const QString&)
{
  GoalList->clearSelection();
}

// -----------------------------------------------------------
void OptimizeDialog::slotChangeGoalType(const QString& Text)
{
  Q3ListViewItem *Item = GoalList->selectedItem();
  if(Item == 0) return;

  Item->setText(1, Text);
}

// -----------------------------------------------------------
void OptimizeDialog::slotChangeGoalNum(const QString& Text)
{
  Q3ListViewItem *Item = GoalList->selectedItem();
  if(Item == 0) return;

  Item->setText(2, Text);
}

// -----------------------------------------------------------
void OptimizeDialog::slotOK()
{
  slotApply();
  slotCancel();
}

// -----------------------------------------------------------
void OptimizeDialog::slotApply()
{
  Component *pc;
  if(NameEdit->text().isEmpty())
    NameEdit->setText(Comp->Name);
  else
  if(NameEdit->text() != Comp->Name) {
    for(pc = Doc->Components->first(); pc!=0; pc = Doc->Components->next())
      if(pc->Name == NameEdit->text())
        break;  // found component with the same name ?
    if(pc)
      NameEdit->setText(Comp->Name);
    else {
      Comp->Name = NameEdit->text();
      changed = true;
    }
  }

  QString Prop;
  if(SimEdit->currentText() != Comp->Props.at(0)->Value) {
    Comp->Props.at(0)->Value = SimEdit->currentText();
    changed = true;
  }
  Prop = QString::number(MethodCombo->currentItem()+1) + "|" +
    IterEdit->text() + "|" +
    RefreshEdit->text() + "|" +
    ParentsEdit->text() + "|" +
    ConstEdit->text() + "|" +
    CrossEdit->text() + "|" +
    SeedEdit->text() + "|" +
    CostVarEdit->text() + "|" +
    CostObjEdit->text() + "|" +
    CostConEdit->text();
  if(Prop != Comp->Props.at(1)->Value) {
    Comp->Props.at(1)->Value = Prop;
    changed = true;
  }

  Q3ListViewItem *item;
  Property *pp = Comp->Props.at(2);
  // apply all the new property values in the ListView
  for(item = VarList->firstChild(); item != 0; item = item->itemBelow()) {
    Prop = item->text(0) + "|" + 
           ((item->text(1) == tr("yes")) ? "yes" : "no") + "|" +
           item->text(2) + "|" + item->text(3) + "|" +
           item->text(4) + "|" +
           ((item->text(5) == tr("linear double")) ? "LIN_DOUBLE" :
           ((item->text(5) == tr("logarithmic double")) ? "LOG_DOUBLE" :
           ((item->text(5) == tr("linear integer")) ? "LIN_INT" : "LOG_INT")));

    if(pp) {
      if(pp->Name != "Var") {
        pp->Name = "Var";
        changed = true;
      }
      if(pp->Value != Prop) {
        pp->Value = Prop;
        changed = true;
      }
    }
    else {
      Comp->Props.append(new Property("Var", Prop, false, ""));
      changed = true;
    }
    pp = Comp->Props.next();
  }

  for(item = GoalList->firstChild(); item != 0; item = item->itemBelow()) {
    Prop = item->text(0) + "|" +
           ((item->text(1) == tr("minimize")) ? "MIN" :
           ((item->text(1) == tr("maximize")) ? "MAX" :
           ((item->text(1) == tr("less")) ? "LE" :
           ((item->text(1) == tr("greater")) ? "GE" :
           ((item->text(1) == tr("equal")) ? "EQ" : "MON"))))) + "|" +
           item->text(2);

    if(pp) {
      if(pp->Name != "Goal") {
        pp->Name = "Goal";
        changed = true;
      }
      if(pp->Value != Prop) {
        pp->Value = Prop;
        changed = true;
      }
    }
    else {
      Comp->Props.append(new Property("Goal", Prop, false, ""));
      changed = true;
    }
    pp = Comp->Props.next();
  }

  // if more properties than in ListView -> delete the rest
  if(pp) {
    pp = Comp->Props.prev();
    Comp->Props.last();
    while(pp != Comp->Props.current())
      Comp->Props.remove();
    changed = true;
  }

  if(changed)
    Doc->viewport()->repaint();
}

// -------------------------------------------------------------------------
// Is called if the "Cancel"-button is pressed.
void OptimizeDialog::slotCancel()
{
  if(changed) done(1); // changed could have been done before
  else done(0);        // (by "Apply"-button)
}
