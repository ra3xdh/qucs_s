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
#include <QtGui>
#include "componentdialog.h"
#include "main.h"
#include "qucs.h"
//Added by qt3to4:
#include <q3button.h>
#include <QLabel>
#include <Q3GridLayout>
#include <Q3VBoxLayout>
#include "schematic.h"

#include <q3hbox.h>
#include <q3vbox.h>
#include <qlayout.h>
#include <q3hgroupbox.h>
#include <qvalidator.h>
#include <qtabwidget.h>
#include <q3filedialog.h>

#include <math.h>


ComponentDialog::ComponentDialog(Component *c, Schematic *d)
			: QDialog(d, 0, TRUE, Qt::WDestructiveClose)
{
  resize(400, 250);
  setCaption(tr("Edit Component Properties"));
  Comp  = c;
  Doc   = d;
  QString s;

  all = new Q3VBoxLayout(this); // to provide neccessary size
  Q3GridLayout *gp1;
  QWidget *myParent = this;
  ValInteger = new QIntValidator(1, 1000000, this);

  Expr.setPattern("[^\"=]*");  // valid expression for property 'edit'
  Validator = new QRegExpValidator(Expr, this);
  Expr.setPattern("[^\"]*");   // valid expression for property 'edit'
  Validator2 = new QRegExpValidator(Expr, this);
  Expr.setPattern("[\\w_]+");  // valid expression for property 'NameEdit'
  ValRestrict = new QRegExpValidator(Expr, this);

  checkSim  = 0;  editSim  = 0;  comboType  = 0;  checkParam = 0;
  editStart = 0;  editStop = 0;  editNumber = 0;

  Property *pp = 0;   // last property not to put in ListView
  // ...........................................................
  // if simulation component
  if((Comp->Model[0] == '.') &&
     (Comp->Model != ".DC") && (Comp->Model != ".HB") &&
     (Comp->Model != ".Digi")) {
    QTabWidget *t = new QTabWidget(this);
    all->addWidget(t);

    QWidget *Tab1 = new QWidget(t);
    t->addTab(Tab1, tr("Sweep"));
    Q3GridLayout *gp = new Q3GridLayout(Tab1, 9,3,5,5);

    gp->addMultiCellWidget(new QLabel(Comp->Description, Tab1), 0,0,0,1);

    int row=1;
    editParam = new QLineEdit(Tab1);
    editParam->setValidator(ValRestrict);
    connect(editParam, SIGNAL(returnPressed()), SLOT(slotParamEntered()));
    checkParam = new QCheckBox(tr("display in schematic"), Tab1);
    if(Comp->Model == ".SW") {   // parameter sweep
      textSim = new QLabel(tr("Simulation:"), Tab1);
      gp->addWidget(textSim, row,0);
      editSim = new QComboBox(Tab1);
      editSim->setEditable(true);
      connect(editSim, SIGNAL(activated(int)), SLOT(slotSimEntered(int)));
      gp->addWidget(editSim, row,1);
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
            editSim->insertItem(pc->Name);
      editSim->setCurrentText(Comp->Props.first()->Value);

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


    QWidget *Tab2 = new QWidget(t);
    t->addTab(Tab2, tr("Properties"));
    gp1 = new Q3GridLayout(Tab2, 9,2,5,5);
    myParent = Tab2;
  }
  else {   // no simulation component
    gp1 = new Q3GridLayout(0, 9,2,5,5);
    all->addLayout(gp1);
  }


  // ...........................................................
  gp1->addMultiCellWidget(new QLabel(Comp->Description, myParent), 0,0,0,1);

  Q3HBox *h5 = new Q3HBox(myParent);
  h5->setSpacing(5);
  gp1->addWidget(h5, 1,0);
  new QLabel(tr("Name:"), h5);
  CompNameEdit = new QLineEdit(h5);
  CompNameEdit->setValidator(ValRestrict);
  connect(CompNameEdit, SIGNAL(returnPressed()), SLOT(slotButtOK()));

  showName = new QCheckBox(tr("display in schematic"), myParent);
  gp1->addWidget(showName, 1,1);

  Q3HGroupBox *PropertyBox = new Q3HGroupBox(tr("Properties"), myParent);
  gp1->addMultiCellWidget(PropertyBox, 2,2,0,1);

  prop = new Q3ListView(PropertyBox);
  prop->setMinimumSize(200, 150);
  prop->addColumn(tr("Name"));
  prop->addColumn(tr("Value"));
  prop->addColumn(tr("display"));
  prop->addColumn(tr("Description"));
  prop->setSorting(-1);   // no sorting

  Q3VBox *v1 = new Q3VBox(PropertyBox);
  v1->setSpacing(3);

  Name = new QLabel(v1);

  Description = new QLabel(v1);

  // hide, because it only replaces 'Description' in some cases
  NameEdit = new QLineEdit(v1);
  NameEdit->setShown(false);
  NameEdit->setValidator(ValRestrict);
  connect(NameEdit, SIGNAL(returnPressed()), SLOT(slotApplyPropName()));

  edit = new QLineEdit(v1);
  edit->setMinimumWidth(150);
  edit->setValidator(Validator2);
  connect(edit, SIGNAL(returnPressed()), SLOT(slotApplyProperty()));

  // hide, because it only replaces 'edit' in some cases
  ComboEdit = new QComboBox(false, v1);
  ComboEdit->setShown(false);
  connect(ComboEdit, SIGNAL(activated(const QString&)),
	  SLOT(slotApplyChange(const QString&)));

  Q3HBox *h3 = new Q3HBox(v1);
  h3->setStretchFactor(new QWidget(h3),5); // stretchable placeholder
  EditButt = new QPushButton(tr("Edit"),h3);
  EditButt->setEnabled(false);
  EditButt->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  connect(EditButt, SIGNAL(clicked()), SLOT(slotEditFile()));
  BrowseButt = new QPushButton(tr("Browse"),h3);
  BrowseButt->setEnabled(false);
  BrowseButt->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  connect(BrowseButt, SIGNAL(clicked()), SLOT(slotBrowseFile()));

  disp = new QCheckBox(tr("display in schematic"), v1);
  connect(disp, SIGNAL(stateChanged(int)), SLOT(slotApplyState(int)));

  v1->setStretchFactor(new QWidget(v1),5); // stretchable placeholder

  Q3HBox *h4 = new Q3HBox(v1);
  h4->setSpacing(5);
  ButtAdd = new QPushButton(tr("Add"),h4);
  ButtAdd->setEnabled(false);
  ButtRem = new QPushButton(tr("Remove"),h4);
  ButtRem->setEnabled(false);
  connect(ButtAdd, SIGNAL(clicked()), SLOT(slotButtAdd()));
  connect(ButtRem, SIGNAL(clicked()), SLOT(slotButtRem()));

  // ...........................................................
  Q3HBox *h2 = new Q3HBox(this);
  h2->setSpacing(5);
  all->addWidget(h2);
  connect(new QPushButton(tr("OK"),h2), SIGNAL(clicked()),
	  SLOT(slotButtOK()));
  connect(new QPushButton(tr("Apply"),h2), SIGNAL(clicked()),
	  SLOT(slotApplyInput()));
  connect(new QPushButton(tr("Cancel"),h2), SIGNAL(clicked()),
	  SLOT(slotButtCancel()));

  // ------------------------------------------------------------
  CompNameEdit->setText(Comp->Name);
  showName->setChecked(Comp->showName);
  changed = false;

  Comp->textSize(tx_Dist, ty_Dist);
  int tmp = Comp->tx+tx_Dist - Comp->x1;
  if((tmp > 0) || (tmp < -6))  tx_Dist = 0;  // remember the text position
  tmp = Comp->ty+ty_Dist - Comp->y1;
  if((tmp > 0) || (tmp < -6))  ty_Dist = 0;

  // insert all properties into the ListBox
  for(Property *p = Comp->Props.last(); p != 0; p = Comp->Props.prev()) {
    if(p == pp)  break;   // do not insert if already on first tab
    if(p->display) s = tr("yes");
    else s = tr("no");
    new Q3ListViewItem(prop, p->Name, p->Value, s, p->Description);
  }

  if(prop->childCount() > 0) {
    prop->setCurrentItem(prop->firstChild());
    slotSelectProperty(prop->firstChild());
  }

  connect(prop, SIGNAL(clicked(Q3ListViewItem*)),
	  SLOT(slotSelectProperty(Q3ListViewItem*)));
}

ComponentDialog::~ComponentDialog()
{
  delete all;
  delete Validator;
  delete Validator2;
  delete ValRestrict;
  delete ValInteger;
}

// -------------------------------------------------------------------------
// Is called if a property is selected. It transfers the values to the right
// side for editing.
void ComponentDialog::slotSelectProperty(Q3ListViewItem *item)
{
  if(item == 0) return;
  item->setSelected(true);  // if called from elsewhere, this was not yet done

  if(item->text(2) == tr("yes")) disp->setChecked(true);
  else disp->setChecked(false);

  if(item->text(0) == "File") {
    EditButt->setEnabled(true);
    BrowseButt->setEnabled(true);
  }
  else {
    EditButt->setEnabled(false);
    BrowseButt->setEnabled(false);
  }

  QString PropDesc = item->text(3);
  if(PropDesc.isEmpty()) {
    // show two line edit fields (name and value)
    ButtAdd->setEnabled(true);
    ButtRem->setEnabled(true);

    Name->setText("");
    NameEdit->setText(item->text(0));
    edit->setText(item->text(1));

    edit->setShown(true);
    NameEdit->setShown(true);
    Description->setShown(false);
    ComboEdit->setShown(false);

    NameEdit->setFocus();   // edit QLineEdit
  }
  else {  // show standard line edit (description and value)
    ButtAdd->setEnabled(false);
    ButtRem->setEnabled(false);

    Name->setText(item->text(0));
    edit->setText(item->text(1));

    NameEdit->setShown(false);
    NameEdit->setText(item->text(0));  // perhaps used for adding properties
    Description->setShown(true);

    // handle special combobox items
    QStringList List;
    int b = PropDesc.find('[');
    int e = PropDesc.findRev(']');
    if (e-b > 2) {
      QString str = PropDesc.mid(b+1, e-b-1);
      str.replace( QRegExp("[^a-zA-Z0-9_,]"), "" );
      List = List.split(',',str);
    }

    QFontMetrics  metrics(QucsSettings.font);   // get size of text
    while(metrics.width(PropDesc) > 270) {  // if description too long, cut it
      if (PropDesc.findRev(' ') != -1)
	PropDesc = PropDesc.left(PropDesc.findRev(' ', -1)) + "....";
      else
	PropDesc = PropDesc.left(PropDesc.length()-5) + "....";
    }
    Description->setText(PropDesc);

    if(List.count() >= 1) {    // ComboBox with value list or line edit ?
      ComboEdit->clear();
      ComboEdit->insertStringList(List);

      for(int i=ComboEdit->count()-1; i>=0; i--)
       if(item->text(1) == ComboEdit->text(i)) {
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
//    edit->deselect();  // doesn't work ?!?
  }
}

// -------------------------------------------------------------------------
void ComponentDialog::slotApplyChange(const QString& Text)
{
  edit->setText(Text);
  prop->currentItem()->setText(1, Text);	// apply edit line

  ComboEdit->setFocus();
  Q3ListViewItem *item = prop->currentItem()->itemBelow();
  if(item == 0) return;

  prop->setSelected(item, true);
  slotSelectProperty(item);   // switch to the next property
}

// -------------------------------------------------------------------------
// Is called if the "RETURN"-button is pressed in the "edit" Widget.
void ComponentDialog::slotApplyProperty()
{
  Q3ListViewItem *item = prop->currentItem();
  if(!item) return;

  if(ComboEdit->isShown())   // take text from ComboBox ?
    edit->setText(ComboEdit->currentText());

  if(item->text(1) != edit->text())
    item->setText(1, edit->text());    // apply edit line

  if(NameEdit->isShown())	// also apply property name ?
    if(item->text(0) != NameEdit->text()) {
//      if(NameEdit->text() == "Export")
//        item->setText(0, "Export_");   // name must not be "Export" !!!
//      else
      item->setText(0, NameEdit->text());  // apply property name
    }

  item = item->itemBelow();
  if(!item) {
    slotButtOK();   // close dialog, if it was the last property
    return;
  }

  prop->setSelected(item, true);
  prop->ensureItemVisible(item);
  slotSelectProperty(item);   // switch to the next property
}

// -------------------------------------------------------------------------
// Is called if the "RETURN"-button is pressed in the "NameEdit" Widget.
void ComponentDialog::slotApplyPropName()
{
  Q3ListViewItem *item = prop->currentItem();
  if(item->text(0) != NameEdit->text()) {
//    if(NameEdit->text() == "Export") {
//	item->setText(0, "Export_");   // name must not be "Export" !!!
//	NameEdit->setText("Export_");
//    }
//      else
    item->setText(0, NameEdit->text());  // apply property name
  }
  edit->setFocus();   // cursor into "edit" widget
}

// -------------------------------------------------------------------------
// Is called if the checkbox is pressed (changed).
void ComponentDialog::slotApplyState(int State)
{
  Q3ListViewItem *item = prop->currentItem();
  if(item == 0) return;

  QString ButtonState;
  if(State) ButtonState = tr("yes");
  else ButtonState = tr("no");

  if(item->text(2) != ButtonState) {
    item->setText(2, ButtonState);
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

  bool display;
  Property *pp = Comp->Props.first();
  // apply all the new property values
  if(editSim) {
    display = checkSim->isChecked();
    if(pp->display != display) {
      pp->display = display;
      changed = true;
    }
    if(pp->Value != editSim->currentText()) {
      pp->Value = editSim->currentText();
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
      pp = Comp->Props.next();
    }
  }


  Q3ListViewItem *item = prop->firstChild();
 if(item != 0) {

  item = prop->currentItem();
  if(item->text(1) != edit->text())
    item->setText(1, edit->text());    // apply edit line
  if(NameEdit->isShown())	// also apply property name ?
    if(item->text(0) != NameEdit->text())
      item->setText(0, NameEdit->text());  // apply property name


  // apply all the new property values in the ListView
  for(item = prop->firstChild(); item != 0; item = item->itemBelow()) {
    display = (item->text(2) == tr("yes"));
    if(pp) {
      if(pp->display != display) {
        pp->display = display;
        changed = true;
      }
      if(pp->Value != item->text(1)) {
        pp->Value = item->text(1);
        changed = true;
      }
      if(pp->Name != item->text(0)) {
        pp->Name = item->text(0);   // override if previous one was removed
        changed = true;
      }
      pp->Description = item->text(3);
    }
    else {  // if less properties than in ListView -> create new
      Comp->Props.append(new
	  Property(item->text(0), item->text(1), display, item->text(3)));
      changed = true;
    }
    pp = Comp->Props.next();
  }
  if(pp) {  // if more properties than in ListView -> delete the rest
    pp = Comp->Props.prev();
    Comp->Props.last();
    while(pp != Comp->Props.current())
      Comp->Props.remove();
    changed = true;
  }
 }

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
  }
}

// -------------------------------------------------------------------------
void ComponentDialog::slotBrowseFile()
{
  QString s = Q3FileDialog::getOpenFileName(QucsSettings.QucsWorkDir.path(),
          tr("All Files")+" (*.*);;"+tr("Touchstone files")+" (*.s?p);;"+
          tr("CSV files")+" (*.csv);;"+
          tr("SPICE files")+" (*.cir *.spi);;"+
          tr("VHDL files")+" (*.vhdl *.vhd);;"+
          tr("Verilog files")+" (*.v)",
          this, "", tr("Select a file"));
  if(!s.isEmpty()) {
    // snip path if file in current directory
    QFileInfo file(s);
    if(QucsSettings.QucsWorkDir.exists(file.fileName()) &&
       QucsSettings.QucsWorkDir.absPath() == file.dirPath(true)) s = file.fileName();
    edit->setText(s);
  }
  prop->currentItem()->setText(1, s);
}

// -------------------------------------------------------------------------
void ComponentDialog::slotEditFile()
{
  Doc->App->editFile(QucsSettings.QucsWorkDir.filePath(edit->text()));
}

// -------------------------------------------------------------------------
// Is called if the add button is pressed. This is only possible for some
// properties.
void ComponentDialog::slotButtAdd()
{
  Q3ListViewItem *item;
  // Search if property with this name already exist.
  for(item = prop->firstChild(); item != 0; item = item->itemBelow())
    if(item->text(0) == NameEdit->text()) {
      prop->setSelected(item, true);
      slotSelectProperty(item);
      return;
    }

  item = prop->selectedItem();
  if(item == 0) item = prop->lastItem();

  QString s = tr("no");
  if(disp->isChecked()) s = tr("yes");

  prop->setSelected(new Q3ListViewItem(prop, item,
			NameEdit->text(), edit->text(), s), true);
}

// -------------------------------------------------------------------------
// Is called if the remove button is pressed. This is only possible for
// some properties.
void ComponentDialog::slotButtRem()
{
  if(prop->childCount() < 3) return;  // the last property cannot be removed
  Q3ListViewItem *item = prop->selectedItem();
  if(item == 0) return;

  Q3ListViewItem *next_item = item->itemBelow();
  if(next_item == 0) next_item = item->itemAbove();
  prop->takeItem(item);     // remove from ListView
  delete item;              // delete item

  slotSelectProperty(next_item);
}

// -------------------------------------------------------------------------
void ComponentDialog::slotSimTypeChange(int Type)
{
  if(Type < 2) {  // new type is "linear" or "logarithmic"
    if(!editNumber->isEnabled()) {  // was the other mode before ?
      // this text change, did not emit the textChange signal !??!
      editStart->setText(
	editValues->text().section(';', 0, 0).stripWhiteSpace());
      editStop->setText(
	editValues->text().section(';', -1, -1).stripWhiteSpace());
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
  double x, y, Factor, ftmp;
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
