/***************************************************************************
                             componentdialog.cpp
                             -------------------
    begin                : Tue Sep 9 2003
    copyright            : (C) 2003, 2004 by Michael Margraf
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

#include "componentdialog.h"
#include "main.h"
#include "qucs.h"
#include "schematic.h"
#include "misc.h"

#include <cmath>

#include <QLabel>
#include <QLayout>
#include <QValidator>
#include <QTableWidget>
#include <QHeaderView>
#include <QFileDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include <QDebug>

ComponentDialog::ComponentDialog(Component *c, Schematic *d)
			: QDialog(d)
{
  resize(600, 250);
  setWindowTitle(tr("Edit Component Properties"));
  Comp  = c;
  Doc   = d;
  QString s;

  all = new QVBoxLayout(this); // to provide neccessary size
  all->setContentsMargins(1,1,1,1);
  QGridLayout *gp1;

  ValInteger = new QIntValidator(1, 1000000, this);

  Expr.setPattern("[^\"=]*");  // valid expression for property 'edit'
  Validator = new QRegExpValidator(Expr, this);
  Expr.setPattern("[^\"]*");   // valid expression for property 'edit'
  Validator2 = new QRegExpValidator(Expr, this);
  Expr.setPattern("[\\w_]+");  // valid expression for property 'NameEdit'
  ValRestrict = new QRegExpValidator(Expr, this);

  checkSim  = 0;  comboSim  = 0;  comboType  = 0;  checkParam = 0;
  editStart = 0;  editStop = 0;  editNumber = 0;

  Property *pp = 0;   // last property not to put in ListView
  // ...........................................................
  // if simulation component: .TR, .AC, .SW, (.SP ?)
  if((Comp->Model[0] == '.') &&
     (Comp->Model != ".DC") && (Comp->Model != ".HB") &&
     (Comp->Model != ".Digi") && (Comp->Model != ".ETR")) {
    QTabWidget *t = new QTabWidget(this);
    all->addWidget(t);

    QWidget *Tab1 = new QWidget(t);
    t->addTab(Tab1, tr("Sweep"));
    QGridLayout *gp = new QGridLayout(Tab1, 9,3,5,5);

    gp->addMultiCellWidget(new QLabel(Comp->Description, Tab1), 0,0,0,1);

    int row=1;
    editParam = new QLineEdit(Tab1);
    editParam->setValidator(ValRestrict);
    connect(editParam, SIGNAL(returnPressed()), SLOT(slotParamEntered()));
    checkParam = new QCheckBox(tr("display in schematic"), Tab1);

    if(Comp->Model == ".SW") {   // parameter sweep
      textSim = new QLabel(tr("Simulation:"), Tab1);
      gp->addWidget(textSim, row,0);
      comboSim = new QComboBox(Tab1);
      comboSim->setEditable(true);
      connect(comboSim, SIGNAL(activated(int)), SLOT(slotSimEntered(int)));
      gp->addWidget(comboSim, row,1);
      checkSim = new QCheckBox(tr("display in schematic"), Tab1);
      gp->addWidget(checkSim, row++,2);
    }
    else {
      editParam->setReadOnly(true);
      checkParam->setDisabled(true);

      if(Comp->Model == ".TR")    // transient simulation ?
        editParam->setText("time");
      else {
        if(Comp->Model == ".AC")    // AC simulation ?
          editParam->setText("acfrequency");
        else
          editParam->setText("frequency");
      }
    }

    gp->addWidget(new QLabel(tr("Sweep Parameter:"), Tab1), row,0);
    gp->addWidget(editParam, row,1);
    gp->addWidget(checkParam, row++,2);

    textType = new QLabel(tr("Type:"), Tab1);
    gp->addWidget(textType, row,0);
    comboType = new QComboBox(Tab1);
    comboType->insertItem(tr("linear"));
    comboType->insertItem(tr("logarithmic"));
    comboType->insertItem(tr("list"));
    comboType->insertItem(tr("constant"));
    gp->addWidget(comboType, row,1);
    connect(comboType, SIGNAL(activated(int)), SLOT(slotSimTypeChange(int)));
    checkType = new QCheckBox(tr("display in schematic"), Tab1);
    gp->addWidget(checkType, row++,2);

    textValues = new QLabel(tr("Values:"), Tab1);
    gp->addWidget(textValues, row,0);
    editValues = new QLineEdit(Tab1);
    editValues->setValidator(Validator);
    connect(editValues, SIGNAL(returnPressed()), SLOT(slotValuesEntered()));
    gp->addWidget(editValues, row,1);
    checkValues = new QCheckBox(tr("display in schematic"), Tab1);
    gp->addWidget(checkValues, row++,2);

    textStart  = new QLabel(tr("Start:"), Tab1);
    gp->addWidget(textStart, row,0);
    editStart  = new QLineEdit(Tab1);
    editStart->setValidator(Validator);
    connect(editStart, SIGNAL(returnPressed()), SLOT(slotStartEntered()));
    gp->addWidget(editStart, row,1);
    checkStart = new QCheckBox(tr("display in schematic"), Tab1);
    gp->addWidget(checkStart, row++,2);

    textStop   = new QLabel(tr("Stop:"), Tab1);
    gp->addWidget(textStop, row,0);
    editStop   = new QLineEdit(Tab1);
    editStop->setValidator(Validator);
    connect(editStop, SIGNAL(returnPressed()), SLOT(slotStopEntered()));
    gp->addWidget(editStop, row,1);
    checkStop = new QCheckBox(tr("display in schematic"), Tab1);
    gp->addWidget(checkStop, row++,2);

    textStep   = new QLabel(tr("Step:"), Tab1);
    gp->addWidget(textStep, row,0);
    editStep   = new QLineEdit(Tab1);
    editStep->setValidator(Validator);
    connect(editStep, SIGNAL(returnPressed()), SLOT(slotStepEntered()));
    gp->addWidget(editStep, row++,1);

    textNumber = new QLabel(tr("Number:"), Tab1);
    gp->addWidget(textNumber, row,0);
    editNumber = new QLineEdit(Tab1);
    editNumber->setValidator(ValInteger);
    connect(editNumber, SIGNAL(returnPressed()), SLOT(slotNumberEntered()));
    gp->addWidget(editNumber, row,1);
    checkNumber = new QCheckBox(tr("display in schematic"), Tab1);
    gp->addWidget(checkNumber, row++,2);


    if(Comp->Model == ".SW") {   // parameter sweep
      Component *pc;
      for(pc=Doc->Components->first(); pc!=0; pc=Doc->Components->next())
        if(pc != Comp)
          if(pc->Model[0] == '.')
            comboSim->insertItem(pc->Name);
      comboSim->setCurrentText(Comp->Props.first()->Value);

      checkSim->setChecked(Comp->Props.current()->display);
      s = Comp->Props.next()->Value;
      checkType->setChecked(Comp->Props.current()->display);
      editParam->setText(Comp->Props.next()->Value);
      checkParam->setChecked(Comp->Props.current()->display);
    }
    else {
      s = Comp->Props.first()->Value;
      checkType->setChecked(Comp->Props.current()->display);
    }
    pp = Comp->Props.next();
    editStart->setText(pp->Value);
    checkStart->setChecked(pp->display);
    pp = Comp->Props.next();
    editStop->setText(pp->Value);
    checkStop->setChecked(pp->display);
    pp = Comp->Props.next();  // remember last property for ListView
    editNumber->setText(pp->Value);
    checkNumber->setChecked(pp->display);

    int tNum = 0;
    if(s[0] == 'l') {
      if(s[1] == 'i') {
	if(s[2] != 'n')
	  tNum = 2;
      }
      else  tNum = 1;
    }
    else  tNum = 3;
    comboType->setCurrentItem(tNum);

    slotSimTypeChange(tNum);   // not automatically ?!?
    if(tNum > 1) {
      editValues->setText(
		editNumber->text().mid(1, editNumber->text().length()-2));
      checkValues->setChecked(Comp->Props.current()->display);
      editNumber->setText("2");
    }
    slotNumberChanged(0);

/*    connect(editValues, SIGNAL(textChanged(const QString&)),
	    SLOT(slotTextChanged(const QString&)));*/
    connect(editStart, SIGNAL(textChanged(const QString&)),
	    SLOT(slotNumberChanged(const QString&)));
    connect(editStop, SIGNAL(textChanged(const QString&)),
	    SLOT(slotNumberChanged(const QString&)));
    connect(editStep, SIGNAL(textChanged(const QString&)),
	    SLOT(slotStepChanged(const QString&)));
    connect(editNumber, SIGNAL(textChanged(const QString&)),
	    SLOT(slotNumberChanged(const QString&)));

/*    if(checkSim)
      connect(checkSim, SIGNAL(stateChanged(int)), SLOT(slotSetChanged(int)));
    connect(checkType, SIGNAL(stateChanged(int)), SLOT(slotSetChanged(int)));
    connect(checkParam, SIGNAL(stateChanged(int)), SLOT(slotSetChanged(int)));
    connect(checkStart, SIGNAL(stateChanged(int)), SLOT(slotSetChanged(int)));
    connect(checkStop, SIGNAL(stateChanged(int)), SLOT(slotSetChanged(int)));
    connect(checkNumber, SIGNAL(stateChanged(int)), SLOT(slotSetChanged(int)));*/


    QWidget *tabProperties = new QWidget(t);
    t->addTab(tabProperties, tr("Properties"));
    gp1 = new QGridLayout(tabProperties, 9,2,5,5);
  }
  else {   // no simulation component
    gp1 = new QGridLayout(0, 9,2,5,5);
    all->addLayout(gp1);
  }


  // ...........................................................
  gp1->addMultiCellWidget(new QLabel(Comp->Description), 0,0,0,1);

  QHBoxLayout *h5 = new QHBoxLayout;
  h5->setSpacing(5);

  h5->addWidget(new QLabel(tr("Name:")) );

  CompNameEdit = new QLineEdit;
  h5->addWidget(CompNameEdit);

  CompNameEdit->setValidator(ValRestrict);
  connect(CompNameEdit, SIGNAL(returnPressed()), SLOT(slotButtOK()));

  showName = new QCheckBox(tr("display in schematic"));
  h5->addWidget(showName);

  QWidget *hTop = new QWidget;
  hTop->setLayout(h5);

  gp1->addWidget(hTop,1,0);

  QGroupBox *PropertyBox = new QGroupBox(tr("Properties"));
  gp1->addWidget(PropertyBox,2,0);

  // H layout inside the GroupBox
  QHBoxLayout *hProps = new QHBoxLayout;
  PropertyBox->setLayout(hProps);

  // left pane
  QWidget *vboxPropsL = new QWidget;
  QVBoxLayout *vL = new QVBoxLayout;
  vboxPropsL->setLayout(vL);

  /// \todo column min width
  prop = new QTableWidget(0,4); //initialize
  vL->addWidget(prop);
  prop->setSelectionBehavior(QAbstractItemView::SelectRows);
  prop->setSelectionMode(QAbstractItemView::SingleSelection);
  prop->setMinimumSize(200, 150);
//  prop->horizontalHeader()->setStretchLastSection(true);
  prop->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

  QStringList headers;
  headers << tr("Name")
          << tr("Value")
          << tr("display")
          << tr("Description");
  prop->setHorizontalHeaderLabels(headers);

  // right pane
  QWidget *vboxPropsR = new QWidget;
  QVBoxLayout *v1 = new QVBoxLayout;
  vboxPropsR->setLayout(v1);

  v1->setSpacing(3);

  hProps->addWidget(vboxPropsL);
  hProps->addWidget(vboxPropsR);

  Name = new QLabel;
  v1->addWidget(Name);

  Description = new QLabel;
  v1->addWidget(Description);

  // hide, because it only replaces 'Description' in some cases
  NameEdit = new QLineEdit;
  v1->addWidget(NameEdit);
  NameEdit->setShown(false);
  NameEdit->setValidator(ValRestrict);
  connect(NameEdit, SIGNAL(returnPressed()), SLOT(slotApplyPropName()));

  edit = new QLineEdit;
  v1->addWidget(edit);
  edit->setMinimumWidth(150);
  edit->setValidator(Validator2);
  connect(edit, SIGNAL(returnPressed()), SLOT(slotApplyProperty()));

  // hide, because it only replaces 'edit' in some cases
  ComboEdit = new QComboBox;
  v1->addWidget(ComboEdit);
  ComboEdit->setShown(false);
  connect(ComboEdit, SIGNAL(activated(const QString&)),
	  SLOT(slotApplyChange(const QString&)));

  QHBoxLayout *h3 = new QHBoxLayout(v1);

  EditButt = new QPushButton(tr("Edit"));
  h3->addWidget(EditButt);
  EditButt->setEnabled(false);
  EditButt->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  connect(EditButt, SIGNAL(clicked()), SLOT(slotEditFile()));

  BrowseButt = new QPushButton(tr("Browse"));
  h3->addWidget(BrowseButt);
  BrowseButt->setEnabled(false);
  BrowseButt->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  connect(BrowseButt, SIGNAL(clicked()), SLOT(slotBrowseFile()));

  disp = new QCheckBox(tr("display in schematic"));
  v1->addWidget(disp);
  connect(disp, SIGNAL(stateChanged(int)), SLOT(slotApplyState(int)));

  QHBoxLayout *h4 = new QHBoxLayout(v1);
  h4->setSpacing(5);
  ButtAdd = new QPushButton(tr("Add"));
  h4->addWidget(ButtAdd);
  ButtAdd->setEnabled(false);
  ButtRem = new QPushButton(tr("Remove"));
  h4->addWidget(ButtRem);
  ButtRem->setEnabled(false);
  connect(ButtAdd, SIGNAL(clicked()), SLOT(slotButtAdd()));
  connect(ButtRem, SIGNAL(clicked()), SLOT(slotButtRem()));

  // ...........................................................
  QHBoxLayout *h2 = new QHBoxLayout(this);
  QWidget * hbox2 = new QWidget;
  hbox2->setLayout(h2);
  h2->setSpacing(5);
  all->addWidget(hbox2);
  QPushButton *ok = new QPushButton(tr("OK"));
  QPushButton *apply = new QPushButton(tr("Apply"));
  QPushButton *cancel = new QPushButton(tr("Cancel"));
  h2->addWidget(ok);
  h2->addWidget(apply);
  h2->addWidget(cancel);
  connect(ok,     SIGNAL(clicked()), SLOT(slotButtOK()));
  connect(apply,  SIGNAL(clicked()), SLOT(slotApplyInput()));
  connect(cancel, SIGNAL(clicked()), SLOT(slotButtCancel()));

  // ------------------------------------------------------------
  CompNameEdit->setText(Comp->Name);
  showName->setChecked(Comp->showName);
  changed = false;

  Comp->textSize(tx_Dist, ty_Dist);
  int tmp = Comp->tx+tx_Dist - Comp->x1;
  if((tmp > 0) || (tmp < -6))  tx_Dist = 0;  // remember the text position
  tmp = Comp->ty+ty_Dist - Comp->y1;
  if((tmp > 0) || (tmp < -6))  ty_Dist = 0;

  /*! Insert all \a Comp properties into the dialog \a prop list */
  int row=0; // row counter
  for(Property *p = Comp->Props.at(Comp->Props.find(pp)+1); p != 0; p = Comp->Props.next()) {

      // do not insert if already on first tab
      // this is the reason it was originally from back to front...
      // the 'pp' is the lasted property stepped over while filling the Swep tab
  //    if(p == pp)
  //      break;
      if(p->display)
        s = tr("yes");
      else
        s = tr("no");

      // add Props into TableWidget
      qDebug() << " Loading Comp->Props :" << p->Name << p->Value << p->display << p->Description ;

      prop->setRowCount(prop->rowCount()+1);

      QTableWidgetItem *cell;
      cell = new QTableWidgetItem(p->Name);
      cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
      prop->setItem(row, 0, cell);
      cell = new QTableWidgetItem(p->Value);
      cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
      prop->setItem(row, 1, cell);
      cell = new QTableWidgetItem(s);
      cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
      prop->setItem(row, 2, cell);
      cell = new QTableWidgetItem(p->Description);
      cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
      prop->setItem(row, 3, cell);

      row++;
    }

    if(prop->rowCount() > 0) {
        prop->setCurrentItem(prop->item(0,0));
        slotSelectProperty(prop->item(0,0));
    }


  /// \todo add key up/down brose and select prop
  connect(prop, SIGNAL(itemClicked(QTableWidgetItem*)),
                SLOT(slotSelectProperty(QTableWidgetItem*)));
}

ComponentDialog::~ComponentDialog()
{
  delete all;
  delete Validator;
  delete Validator2;
  delete ValRestrict;
  delete ValInteger;
}


// Updates component property list. Useful for MultiViewComponents

void ComponentDialog::updateCompPropsList()
{
    int last_prop=0; // last property not to put in ListView
        // ...........................................................
        // if simulation component: .TR, .AC, .SW, (.SP ?)
    if((Comp->Model[0] == '.') &&
       (Comp->Model != ".DC") && (Comp->Model != ".HB") &&
       (Comp->Model != ".Digi") && (Comp->Model != ".ETR")) {
        if(Comp->Model == ".SW") {   // parameter sweep
           last_prop = 2;
        } else {
            last_prop = 0;
        }
            last_prop += 4;  // remember last property for ListView
    }

    QString s;
    int row=0; // row counter
    //for(Property *p = Comp->Props.first(); p != 0; p = Comp->Props.next()) {
    for(Property *p = Comp->Props.at(last_prop); p != 0; p = Comp->Props.next()) {

      // do not insert if already on first tab
      // this is the reason it was originally from back to front...
      // the 'pp' is the lasted property stepped over while filling the Swep tab
  //    if(p == pp)
  //      break;
      if(p->display)
        s = tr("yes");
      else
        s = tr("no");

      // add Props into TableWidget
      qDebug() << " Loading Comp->Props :" << p->Name << p->Value << p->display << p->Description ;

      if (row > prop->rowCount()-1) { // Add new rows
          prop->setRowCount(prop->rowCount()+1);
      }

      QTableWidgetItem *cell;
      cell = new QTableWidgetItem(p->Name);
      cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
      prop->setItem(row, 0, cell);
      cell = new QTableWidgetItem(p->Value);
      cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
      prop->setItem(row, 1, cell);
      cell = new QTableWidgetItem(s);
      cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
      prop->setItem(row, 2, cell);
      cell = new QTableWidgetItem(p->Description);
      cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
      prop->setItem(row, 3, cell);

      row++;
    }

    if(prop->rowCount() > 0) {
        prop->setCurrentItem(prop->item(0,0));
        slotSelectProperty(prop->item(0,0));
    }

    if (row < prop->rowCount()-1) {
        prop->setRowCount(row);
    }
}

// -------------------------------------------------------------------------
// Is called if a property is selected.
// Handle the Property editor tab.
// It transfers the values to the right side for editing.
void ComponentDialog::slotSelectProperty(QTableWidgetItem *item)
{

  if(item == 0) return;
  item->setSelected(true);  // if called from elsewhere, this was not yet done

  qDebug() << "row " << item->row(); //<< item->text()

  QString name  = prop->item(item->row(),0)->text();
  QString value = prop->item(item->row(),1)->text();
  QString show  = prop->item(item->row(),2)->text();
  QString desc  = prop->item(item->row(),3)->text();

  if(show == tr("yes"))
    disp->setChecked(true);
  else
    disp->setChecked(false);

  if(name == "File") {
    EditButt->setEnabled(true);
    BrowseButt->setEnabled(true);
  }
  else {
    EditButt->setEnabled(false);
    BrowseButt->setEnabled(false);
  }

  /// \todo enable edit of description anyway...
  /// empy or "-" (no comment from verilog-a)
  if(desc.isEmpty()) {
    // show two line edit fields (name and value)
    ButtAdd->setEnabled(true);
    ButtRem->setEnabled(true);

    Name->setText("");
    NameEdit->setText(name);
    edit->setText(value);

    edit->setShown(true);
    NameEdit->setShown(true);
    Description->setShown(false);
    ComboEdit->setShown(false);

    NameEdit->setFocus();   // edit QLineEdit
  }
  else {  // show standard line edit (description and value)
    ButtAdd->setEnabled(false);
    ButtRem->setEnabled(false);

    Name->setText(name);
    edit->setText(value);

    NameEdit->setShown(false);
    NameEdit->setText(name); // perhaps used for adding properties
    Description->setShown(true);

    // handle special combobox items
    QStringList List;
    int b = desc.indexOf('[');
    int e = desc.findRev(']');
    if (e-b > 2) {
      QString str = desc.mid(b+1, e-b-1);
      str.replace( QRegExp("[^a-zA-Z0-9_,]"), "" );
      List = List.split(',',str);
    }

    QFontMetrics  metrics(QucsSettings.font);   // get size of text
    while(metrics.width(desc) > 270) {  // if description too long, cut it
      if (desc.findRev(' ') != -1)
        desc = desc.left(desc.findRev(' ', -1)) + "....";
      else
        desc = desc.left(desc.length()-5) + "....";
    }
    Description->setText(desc);

    if(List.count() >= 1) {    // ComboBox with value list or line edit ?
      ComboEdit->clear();
      ComboEdit->insertItems(0,List);

      for(int i=ComboEdit->count()-1; i>=0; i--)
       if(value == ComboEdit->text(i)) {
         ComboEdit->setCurrentItem(i);
	 break;
       }
      edit->setShown(false);
      ComboEdit->setShown(true);
    }
    else {
      edit->setShown(true);
      ComboEdit->setShown(false);
    }
    edit->setFocus();   // edit QLineEdit
  }
}

// -------------------------------------------------------------------------
void ComponentDialog::slotApplyChange(const QString& Text)
{
  /// \bug what if the table have no items?
  // pick selected row
  QTableWidgetItem *item = prop->selectedItems()[0];

  int row = item->row();

  qDebug() << Text;
  edit->setText(Text);
  // apply edit line
  prop->item(row, 1)->setText(Text);

  ComboEdit->setFocus();

  // step to next item
  if ( row < prop->rowCount()) {
    prop->setCurrentItem(prop->item(row+1,0));
    slotSelectProperty(prop->item(row+1,0));
  }
}

/*!
 Is called if the "RETURN" key is pressed in the "edit" Widget.
 The parameter is edited on the right pane.
 Return key commits the change, and steps to the next parameter in the list.
*/
void ComponentDialog::slotApplyProperty()
{
  // pick selected row
  QTableWidgetItem *item = prop->selectedItems()[0];

  int row = item->row();

  QString name  = prop->item(row, 0)->text();
  QString value = prop->item(row, 1)->text();

  if(!item)
    return;

  if(ComboEdit->isShown())   // take text from ComboBox ?
    edit->setText(ComboEdit->currentText());

  // apply edit line
  if(value != edit->text()) {
       prop->item(row, 1)->setText(edit->text());
    }

  if(NameEdit->isShown())	// also apply property name ?
    if(name != NameEdit->text()) {
//      if(NameEdit->text() == "Export")
//        item->setText(0, "Export_");   // name must not be "Export" !!!
//      else
//      item->setText(0, NameEdit->text());  // apply property name
      prop->item(row, 0)->setText(edit->text());
    }

  // step to next item
  if ( row < prop->rowCount()) {
    prop->setCurrentItem(prop->item(row+1,0));
    slotSelectProperty(prop->item(row+1,0));
  }
  else {
    slotButtOK();   // close dialog, if it was the last property
    return;
  }
}

// -------------------------------------------------------------------------
// Is called if the "RETURN"-button is pressed in the "NameEdit" Widget.
void ComponentDialog::slotApplyPropName()
{
  // pick selected row
  QTableWidgetItem *item = prop->selectedItems()[0];
  int row = item->row();

  QString name  = prop->item(row, 0)->text();

  if(name != NameEdit->text()) {
//    if(NameEdit->text() == "Export") {
//	item->setText(0, "Export_");   // name must not be "Export" !!!
//	NameEdit->setText("Export_");
//    }
//      else
    prop->item(row, 0)->setText(NameEdit->text());
  }
  edit->setFocus();   // cursor into "edit" widget
}

// -------------------------------------------------------------------------
// Is called if the checkbox is pressed (changed).
void ComponentDialog::slotApplyState(int State)
{
  // pick selected row
  QTableWidgetItem *item = prop->selectedItems()[0];
  int row = item->row();

  QString disp  = prop->item(row, 2)->text();

  if(item == 0) return;

  QString ButtonState;
  if(State)
    ButtonState = tr("yes");
  else
    ButtonState = tr("no");

  if(disp != ButtonState) {
    prop->item(row, 2)->setText(ButtonState);
  }
}

// -------------------------------------------------------------------------
// Is called if the "OK"-button is pressed.
void ComponentDialog::slotButtOK()
{
  slotApplyInput();
  slotButtCancel();
}

// -------------------------------------------------------------------------
// Is called if the "Cancel"-button is pressed.
void ComponentDialog::slotButtCancel()
{
  if(changed) done(1);	// changed could have been done before
  else done(0);		// (by "Apply"-button)
}

//-----------------------------------------------------------------
// To get really all close events (even <Escape> key).
void ComponentDialog::reject()
{
  slotButtCancel();
}

// -------------------------------------------------------------------------
// Is called, if the "Apply"-button is pressed.
void ComponentDialog::slotApplyInput()
{
  qDebug() << " \n == Apply ";
  if(Comp->showName != showName->isChecked()) {
    Comp->showName = showName->isChecked();
    changed = true;
  }

  QString tmp;
  Component *pc;
  if(CompNameEdit->text().isEmpty())  CompNameEdit->setText(Comp->Name);
  else
  if(CompNameEdit->text() != Comp->Name) {
    for(pc = Doc->Components->first(); pc!=0; pc = Doc->Components->next())
      if(pc->Name == CompNameEdit->text())
        break;  // found component with the same name ?
    if(pc)  CompNameEdit->setText(Comp->Name);
    else {
      Comp->Name = CompNameEdit->text();
      changed = true;
    }
  }

  /*! Walk the original Comp->Props and compare with the
   *  data in the dialog.
   *  The pointers to the combo, edits,... are set to 0.
   *  Only check if the widgets were created (pointers checks are 'true')
   */
  bool display;
  Property *pp = Comp->Props.first();
  // apply all the new property values

  if(comboSim) {
    display = checkSim->isChecked();
    if(pp->display != display) {
      pp->display = display;
      changed = true;
    }
    if(pp->Value != comboSim->currentText()) {
      pp->Value = comboSim->currentText();
      changed = true;
    }
    pp = Comp->Props.next();
  }
  if(comboType) {
    display = checkType->isChecked();
    if(pp->display != display) {
      pp->display = display;
      changed = true;
    }
    switch(comboType->currentItem()) {
      case 1:  tmp = "log";   break;
      case 2:  tmp = "list";  break;
      case 3:  tmp = "const"; break;
      default: tmp = "lin";   break;
    }
    if(pp->Value != tmp) {
      pp->Value = tmp;
      changed = true;
    }
    pp = Comp->Props.next();
  }
  if(checkParam) if(checkParam->isEnabled()) {
    display = checkParam->isChecked();
    if(pp->display != display) {
      pp->display = display;
      changed = true;
    }
    if(pp->Value != editParam->text()) {
      pp->Value = editParam->text();
      changed = true;
    }
    pp = Comp->Props.next();
  }
  if(editStart) {
    if(comboType->currentItem() < 2) {
      display = checkStart->isChecked();
      if(pp->display != display) {
        pp->display = display;
        changed = true;
      }
      pp->Name  = "Start";
      if(pp->Value != editStart->text()) {
        pp->Value = editStart->text();
        changed = true;
      }
      pp = Comp->Props.next();

      display = checkStop->isChecked();
      if(pp->display != display) {
        pp->display = display;
        changed = true;
      }
      pp->Name  = "Stop";
      if(pp->Value != editStop->text()) {
        pp->Value = editStop->text();
        changed = true;
      }
      pp = Comp->Props.next();

      display = checkNumber->isChecked();
      if(pp->display != display) {
        pp->display = display;
        changed = true;
      }
      if((pp->Value != editNumber->text()) || (pp->Name != "Points")) {
        pp->Value = editNumber->text();
        pp->Name  = "Points";
        changed = true;
      }
      qDebug() << "====> before ad"
               << pp->Description;

      pp = Comp->Props.next();
    }
    else {
      // If a value list is used, the properties "Start" and "Stop" are not
      // used. -> Call them "Symbol" to omit them in the netlist.
      pp->Name = "Symbol";
      pp->display = false;
      pp = Comp->Props.next();
      pp->Name = "Symbol";
      pp->display = false;
      pp = Comp->Props.next();

      display = checkValues->isChecked();
      if(pp->display != display) {
        pp->display = display;
        changed = true;
      }
      tmp = "["+editValues->text()+"]";
      if((pp->Value != tmp) || (pp->Name != "Values")) {
        pp->Value = tmp;
        pp->Name  = "Values";
        changed = true;
      }
      qDebug() << "====> before ad"
               << pp;

      pp = Comp->Props.next();
    }
  }


  // pick selected row
  QTableWidgetItem *item = 0;

  //  make sure we have one item, take selected
  if (prop->selectedItems().size()) {
    item = prop->selectedItems()[0];
  }

  /*! Walk the dialog list of 'prop'
   */
   if(item != 0) {
     int row = item->row();
     QString name  = prop->item(row, 0)->text();
     QString value = prop->item(row, 1)->text();

     // apply edit line
     if(value != edit->text())
       prop->item(row, 1)->setText(edit->text());

     // apply property name
     if(NameEdit->isShown())
       if(name != NameEdit->text())
         prop->item(row, 0)->setText(NameEdit->text());

     // apply all the new property values in the ListView
     for( int row = 0; row < prop->rowCount(); row++ ) {

       QString name  = prop->item(row, 0)->text();
       QString value = prop->item(row, 1)->text();
       QString disp = prop->item(row, 2)->text();
       QString desc = prop->item(row, 3)->text();

       qDebug() << "====>" <<name << value
                << Comp->Props.count()
                << prop->rowCount() +1
                << pp;

       display = (disp == tr("yes"));
       if( pp ) {

         if(pp->display != display) {
             pp->display = display;
             changed = true;
         }
         if(pp->Value != value) {
            pp->Value = value;
            changed = true;
         }
         if(pp->Name != name) {
           pp->Name = name;   // override if previous one was removed
           changed = true;
         }
         pp->Description = desc;
         }
       else {
          // if properties where added in the dialog
          // -> create new on the Comp
         if (Comp->Props.count() < prop->rowCount() +1) {
             qDebug() << "adding to Comp ";
             Comp->Props.append(new Property(name, value, display, desc));
             changed = true;
         }
    }
    pp = Comp->Props.next();
  }

  // original Comp still has properties? (removed some in the dialog?)
  // if more properties than in ListView -> delete the rest
  if(pp) {
    pp = Comp->Props.prev();
    Comp->Props.last();
    while(pp != Comp->Props.current())
      Comp->Props.remove();
    changed = true;
  }

 } // end if (item !=0)

  if(changed) {
    int dx, dy;
    Comp->textSize(dx, dy);
    if(tx_Dist != 0) {
      Comp->tx += tx_Dist-dx;
      tx_Dist = dx;
    }
    if(ty_Dist != 0) {
      Comp->ty += ty_Dist-dy;
      ty_Dist = dy;
    }

    Doc->recreateComponent(Comp);
    Doc->viewport()->repaint();
    if (Comp->Props.count() != prop->rowCount()) { // If props count was changed after recreation
        updateCompPropsList(); // of component we need to update properties
    }
  }

}

// -------------------------------------------------------------------------
void ComponentDialog::slotBrowseFile()
{
  QString s = QFileDialog::getOpenFileName (
          this,
          tr("Select a file"),
          QucsSettings.QucsWorkDir.path(),
          tr("All Files")+" (*.*);;"
            + tr("Touchstone files") + " (*.s?p);;"
            + tr("CSV files") + " (*.csv);;"
            + tr("SPICE files") + " (*.cir *.spi);;"
            + tr("VHDL files") + " (*.vhdl *.vhd);;"
            + tr("Verilog files")+" (*.v)"  );

  if(!s.isEmpty()) {
    // snip path if file in current directory
    QFileInfo file(s);
    if(QucsSettings.QucsWorkDir.exists(file.fileName()) &&
       QucsSettings.QucsWorkDir.absPath() == file.dirPath(true)) s = file.fileName();
    edit->setText(s);
  }
  /* FIX
  prop->currentItem()->setText(1, s); */
}

// -------------------------------------------------------------------------
void ComponentDialog::slotEditFile()
{
  Doc->App->editFile(QucsSettings.QucsWorkDir.filePath(edit->text()));
}

/*!
  Add description if missing.
  Is called if the add button is pressed. This is only possible for some
 properties.
 If desc is empy, ButtAdd is enabled, this slot handles if it is clicked.
 Used with: Equation, ?

 Original behavior for an Equation block
  - start with props
    y=1 (Name, Value)
    Export=yes
  - add equation, results
    y=1
    y2=1
    Export=yes

  If Name already exists, set it to focus
  If new name, insert item before Export

*/
void ComponentDialog::slotButtAdd()
{
  // Search if property with this name already exist.
  // loop over all items, select if found by name
  for(int row=0; row < prop->rowCount(); row++) {

    QString name  = prop->item(row, 0)->text();
    //if found, jump to it
    if( name == NameEdit->text()) {
      prop->setCurrentItem(prop->item(row,0));
      slotSelectProperty(prop->item(row,0));
      return;
    }
  }



  //if nothing selected, select last
//  prop->setCurrentItem(prop->item(prop->rowCount(),0));
//  slotSelectProperty(prop->item(prop->rowCount(),0));

  QString s = tr("no");
  if(disp->isChecked())
    s = tr("yes");


  // take last row
  QList<QTableWidgetItem*> rowItems;
  for (int col = 0; col < prop->columnCount(); ++col)  {
    rowItems << prop->takeItem(prop->rowCount()-1, col);
  }

  // set last row with current info in
  int row = prop->rowCount()-1;

  // append new row
  QTableWidgetItem *cell;
  cell = new QTableWidgetItem(NameEdit->text());
  cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
  prop->setItem(row, 0, cell);
  cell = new QTableWidgetItem(edit->text());
  cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
  prop->setItem(row, 1, cell);
  cell = new QTableWidgetItem(s);
  cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
  prop->setItem(row, 2, cell);
  // no description? add empty cell
  cell = new QTableWidgetItem("");
  cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
  prop->setItem(row, 3, cell);

  // increase list
  prop->setRowCount(prop->rowCount()+1);

  // add taken item again as last
  row = prop->rowCount()-1;
  for (int col = 0; col < prop->columnCount(); ++col)
  {
    prop->setItem(row, col, rowItems.at(col));
  }
}

/*!
 Is called if the remove button is pressed. This is only possible for
 some properties.
 If desc is empy, ButtRem is enabled, this slot handles if it is clicked.
 Used with: Equations, ?
*/
void ComponentDialog::slotButtRem()
{
  if(prop->rowCount() < 3)
    return;  // the last property cannot be removed

  QTableWidgetItem *item = prop->selectedItems()[0];
  int row = item->row();

  if(item == 0)
    return;

  // peek next, delete current, set next current
  if ( row < prop->rowCount()) {
    prop->setCurrentItem(prop->item(row+1,0));
    slotSelectProperty(prop->item(row+1,0));
    prop->removeRow(row);
   }
}

// -------------------------------------------------------------------------
void ComponentDialog::slotSimTypeChange(int Type)
{
  if(Type < 2) {  // new type is "linear" or "logarithmic"
    if(!editNumber->isEnabled()) {  // was the other mode before ?
      // this text change, did not emit the textChange signal !??!
      editStart->setText(
	editValues->text().section(';', 0, 0).trimmed());
      editStop->setText(
	editValues->text().section(';', -1, -1).trimmed());
      editNumber->setText("2");
      slotNumberChanged(0);

      checkStart->setChecked(true);
      checkStop->setChecked(true);
    }
    textValues->setDisabled(true);
    editValues->setDisabled(true);
    checkValues->setDisabled(true);
    textStart->setDisabled(false);
    editStart->setDisabled(false);
    checkStart->setDisabled(false);
    textStop->setDisabled(false);
    editStop->setDisabled(false);
    checkStop->setDisabled(false);
    textStep->setDisabled(false);
    editStep->setDisabled(false);
    textNumber->setDisabled(false);
    editNumber->setDisabled(false);
    checkNumber->setDisabled(false);
    if(Type == 1)   // logarithmic ?
      textStep->setText(tr("Points per decade:"));
    else
      textStep->setText(tr("Step:"));
  }
  else {  // new type is "list" or "constant"
    if(!editValues->isEnabled()) {   // was the other mode before ?
      editValues->setText(editStart->text() + "; " + editStop->text());
      checkValues->setChecked(true);
    }

    textValues->setDisabled(false);
    editValues->setDisabled(false);
    checkValues->setDisabled(false);
    textStart->setDisabled(true);
    editStart->setDisabled(true);
    checkStart->setDisabled(true);
    textStop->setDisabled(true);
    editStop->setDisabled(true);
    checkStop->setDisabled(true);
    textStep->setDisabled(true);
    editStep->setDisabled(true);
    textNumber->setDisabled(true);
    editNumber->setDisabled(true);
    checkNumber->setDisabled(true);
    textStep->setText(tr("Step:"));
  }
}

// -------------------------------------------------------------------------
// Is called when "Start", "Stop" or "Number" is edited.
void ComponentDialog::slotNumberChanged(const QString&)
{
  QString Unit, tmp;
  double x, y, Factor;
  if(comboType->currentItem() == 1) {   // logarithmic ?
    str2num(editStop->text(), x, Unit, Factor);
    x *= Factor;
    str2num(editStart->text(), y, Unit, Factor);
    y *= Factor;
    if(y == 0.0)  y = x / 10.0;
    if(x == 0.0)  x = y * 10.0;
    if(y == 0.0) { y = 1.0;  x = 10.0; }
    x = editNumber->text().toDouble() / log10(fabs(x / y));
    Unit = QString::number(x);
  }
  else {
    str2num(editStop->text(), x, Unit, Factor);
    x *= Factor;
    str2num(editStart->text(), y, Unit, Factor);
    y *= Factor;
    x = (x - y) / (editNumber->text().toDouble() - 1.0);

    QString step = num2str(x);

    str2num(step, x, Unit, Factor);
    if(Factor == 1.0)
        Unit = "";

    Unit = QString::number(x) + " " + Unit;
  }

  editStep->blockSignals(true);  // do not calculate step again
  editStep->setText(Unit);
  editStep->blockSignals(false);
}

// -------------------------------------------------------------------------
void ComponentDialog::slotStepChanged(const QString& Step)
{
  QString Unit;
  double x, y, Factor;
  if(comboType->currentItem() == 1) {   // logarithmic ?
    str2num(editStop->text(), x, Unit, Factor);
    x *= Factor;
    str2num(editStart->text(), y, Unit, Factor);
    y *= Factor;

    x /= y;
    str2num(Step, y, Unit, Factor);
    y *= Factor;

    x = log10(fabs(x)) * y;
  }
  else {
    str2num(editStop->text(), x, Unit, Factor);
    x *= Factor;
    str2num(editStart->text(), y, Unit, Factor);
    y *= Factor;

    x -= y;
    str2num(Step, y, Unit, Factor);
    y *= Factor;

    x /= y;
  }

  editNumber->blockSignals(true);  // do not calculate number again
  editNumber->setText(QString::number(floor(x + 1.0)));
  editNumber->blockSignals(false);
}

// -------------------------------------------------------------------------
// Is called if return is pressed in LineEdit "Parameter".
void ComponentDialog::slotParamEntered()
{
  if(editValues->isEnabled())
    editValues->setFocus();
  else
    editStart->setFocus();
}

// -------------------------------------------------------------------------
// Is called if return is pressed in LineEdit "Simulation".
void ComponentDialog::slotSimEntered(int)
{
  editParam->setFocus();
}

// -------------------------------------------------------------------------
// Is called if return is pressed in LineEdit "Values".
void ComponentDialog::slotValuesEntered()
{
  slotButtOK();
}

// -------------------------------------------------------------------------
// Is called if return is pressed in LineEdit "Start".
void ComponentDialog::slotStartEntered()
{
  editStop->setFocus();
}

// -------------------------------------------------------------------------
// Is called if return is pressed in LineEdit "Stop".
void ComponentDialog::slotStopEntered()
{
  editStep->setFocus();
}

// -------------------------------------------------------------------------
// Is called if return is pressed in LineEdit "Step".
void ComponentDialog::slotStepEntered()
{
  editNumber->setFocus();
}

// -------------------------------------------------------------------------
// Is called if return is pressed in LineEdit "Number".
void ComponentDialog::slotNumberEntered()
{
  slotButtOK();
}

