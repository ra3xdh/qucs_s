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
#include <QEvent>
#include <QKeyEvent>
#include <QDebug>

ComponentDialog::ComponentDialog(Component *c, Schematic *d)
			: QDialog(d)
{
  QSettings settings("qucs","qucs_s");
  restoreGeometry(settings.value("ComponentDialog/geometry").toByteArray());

  setWindowTitle(tr("Edit Component Properties"));
  Comp  = c;
  Doc   = d;
  QString s;
  setAllVisible = true; // state when toggling properties visibility

  all = new QVBoxLayout; // to provide necessary size
  this->setLayout(all);
  all->setContentsMargins(1,1,1,1);
  QGridLayout *gp1;

  ValInteger = new QIntValidator(1, 1000000, this);

  Expr.setPattern("[^\"=]*");  // valid expression for property 'edit'
  Validator = new QRegularExpressionValidator(Expr, this);
  Expr.setPattern("[^\"]*");   // valid expression for property 'edit'
  Validator2 = new QRegularExpressionValidator(Expr, this);
  Expr.setPattern("[\\w_.,\\(\\) @:\\[\\]]+");  // valid expression for property 'NameEdit'. Space to enable Spice-style par sweep
  ValRestrict = new QRegularExpressionValidator(Expr, this);

  checkSim  = 0;  comboSim  = 0;  comboType  = 0;  checkParam = 0;
  editStart = 0;  editStop = 0;  editNumber = 0;
  
  Property *pp = 0; // last property shown elsewhere outside the properties table, not to put in TableView
  // ...........................................................
  // if simulation component: .TR, .AC, .SW, (.SP ?)
  if((Comp->Model[0] == '.') &&
     (Comp->Model != ".DC") && (Comp->Model != ".HB") &&
     (Comp->Model != ".Digi") && (Comp->Model != ".ETR") &&
     (Comp->Model != ".FOURIER") && (Comp->Model != ".FFT") &&
     (Comp->Model != ".PZ") && (Comp->Model != ".SENS") &&
     (Comp->Model != ".SENS_AC") && (Comp->Model != ".SENS_XYCE") &&
     (Comp->Model != ".SENS_TR_XYCE")) {
    QTabWidget *t = new QTabWidget(this);
    all->addWidget(t);

    QWidget *Tab1 = new QWidget(t);
    t->addTab(Tab1, tr("Sweep"));
    QGridLayout *gp = new QGridLayout;
    Tab1->setLayout(gp);

    gp->addWidget(new QLabel(Comp->Description, Tab1), 0,0,1,2);

    int row=1;
    editParam = new QLineEdit(Tab1);
    if (Comp->Model != ".SW") editParam->setValidator(ValRestrict);
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

    QStringList sweeptypes;
    sweeptypes << tr("linear") 
	       << tr("logarithmic") 
	       << tr("list") 
	       << tr("constant");
    comboType->insertItems(0, sweeptypes);
			   
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
      for(pc=Doc->Components->first(); pc!=0; pc=Doc->Components->next()) {
	// insert all schematic available simulations in the Simulation combo box
        if(pc != Comp)
          if(pc->Model[0] == '.')
            comboSim->insertItem(comboSim->count(), pc->Name);
      }
      qDebug() << "[]" << Comp->Props.first()->Value;
      // set selected simulations in combo box to the currently used one
      int i = comboSim->findText(Comp->Props.first()->Value);
      if (i != -1) // current simulation is in the available simulations list (normal case)
	comboSim->setCurrentIndex(i);
      else  // current simulation not in the available simulations list
	comboSim->setEditText(Comp->Props.first()->Value);

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
    comboType->setCurrentIndex(tNum);

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
    //gp1 = new QGridLayout(tabProperties, 9,2,5,5);
    gp1 = new QGridLayout(tabProperties);
  }
  else {   // no simulation component
    //gp1 = new QGridLayout(0, 9,2,5,5);
    gp1 = new QGridLayout();
    all->addLayout(gp1);
  }


  // ...........................................................
  gp1->addWidget(new QLabel(Comp->Description), 0,0,1,2);

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
  prop->verticalHeader()->setVisible(false);
  prop->setSelectionBehavior(QAbstractItemView::SelectRows);
  prop->setSelectionMode(QAbstractItemView::SingleSelection);
  prop->setMinimumSize(200, 150);
  prop->horizontalHeader()->setStretchLastSection(true);
  // set automatic resize so all content will be visible, 
  //  horizontal scrollbar will appear if table becomes too large
  prop->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  prop->horizontalHeader()->setSectionsClickable(false); // no action when clicking on the header

  connect(prop->horizontalHeader(),SIGNAL(sectionDoubleClicked(int)),
              this, SLOT(slotHHeaderClicked(int)));

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

  hProps->addWidget(vboxPropsL, 5); // stretch the left pane (with the table) when resized
  hProps->addWidget(vboxPropsR);

  Name = new QLabel;
  v1->addWidget(Name);

  Description = new QLabel;
  v1->addWidget(Description);

  // hide, because it only replaces 'Description' in some cases
  NameEdit = new QLineEdit;
  v1->addWidget(NameEdit);
  NameEdit->setVisible(false);
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
  ComboEdit->setVisible(false);
  ComboEdit->installEventFilter(this); // to catch Enter keypress
  connect(ComboEdit, SIGNAL(activated(int)),
      SLOT(slotApplyChange(int)));

  QHBoxLayout *h3 = new QHBoxLayout;
  v1->addLayout(h3);

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

  // keep group above together
  v1->addStretch(5);

  QGridLayout *bg = new QGridLayout;
  v1->addLayout(bg);
  ButtAdd = new QPushButton(tr("Add"));
  bg->addWidget(ButtAdd, 0, 0);
  ButtAdd->setEnabled(false);
  ButtRem = new QPushButton(tr("Remove"));
  bg->addWidget(ButtRem, 0, 1);
  ButtRem->setEnabled(false);
  connect(ButtAdd, SIGNAL(clicked()), SLOT(slotButtAdd()));
  connect(ButtRem, SIGNAL(clicked()), SLOT(slotButtRem()));
  // Buttons to move equations up/down on the list
  ButtUp = new QPushButton(tr("Move Up"));
  bg->addWidget(ButtUp, 1, 0);
  ButtDown = new QPushButton(tr("Move Down"));
  bg->addWidget(ButtDown, 1, 1);
  connect(ButtUp,   SIGNAL(clicked()), SLOT(slotButtUp()));
  connect(ButtDown, SIGNAL(clicked()), SLOT(slotButtDown()));


  // ...........................................................
  QHBoxLayout *h2 = new QHBoxLayout;
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
 

  /// \todo add key up/down browse and select prop
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

// check if Enter is pressed while the ComboEdit has focus
// in case, behave as for the LineEdits
// (QComboBox by default does not handle the Enter/Return key)
bool ComponentDialog::eventFilter(QObject *obj, QEvent *event)
{
  if (obj == ComboEdit) {
    if (event->type() == QEvent::KeyPress) {
      QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
      if ((keyEvent->key() == Qt::Key_Return) ||
          (keyEvent->key() == Qt::Key_Enter)) {
        slotApplyProperty();
        return true;
      }
    }
  }
  return QDialog::eventFilter(obj, event); // standard event processing
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
  /// empty or "-" (no comment from verilog-a)
  if(desc.isEmpty()) {
    // show two line edit fields (name and value)
    ButtAdd->setEnabled(true);
    ButtRem->setEnabled(true);

    QStringList eqns_mods;
    eqns_mods<<"Eqn"<<"SpicePar"<<"SpGlobPar"
             <<"SpiceIC"<<"SpiceNodeset"<<"NutmegEq";
    // enable Up/Down buttons only for the Equation component
    if (eqns_mods.contains(Comp->Model)) {
      ButtUp->setEnabled(true);
      ButtDown->setEnabled(true);
    }
    else {
      ButtUp->setEnabled(false);
      ButtDown->setEnabled(false);
    }
    Name->setText("");
    NameEdit->setText(name);
    edit->setText(value);

    edit->setVisible(true);
    NameEdit->setVisible(true);
    Description->setVisible(false);
    ComboEdit->setVisible(false);

    NameEdit->setFocus();   // edit QLineEdit
  } else if (desc=="Expression") { // Single expression
      // show two line edit fields (name and value)
      // And disable buttons

      Name->setText("");
      NameEdit->setText(name);
      edit->setText(value);

      edit->setVisible(true);
      NameEdit->setVisible(true);
      Description->setVisible(false);
      ComboEdit->setVisible(false);

      NameEdit->setFocus();   // edit QLineEdit
  } else {  // show standard line edit (description and value)
    ButtAdd->setEnabled(false);
    ButtRem->setEnabled(false);
    ButtUp->setEnabled(false);
    ButtDown->setEnabled(false);

    Name->setText(name);
    edit->setText(value);

    NameEdit->setVisible(false);
    NameEdit->setText(name); // perhaps used for adding properties
    Description->setVisible(true);

    // handle special combobox items
    QStringList List;
    int b = desc.indexOf('[');
    int e = desc.lastIndexOf(']');
    if (e-b > 2) {
      QString str = desc.mid(b+1, e-b-1);
      str.replace( QRegularExpression("[^a-zA-Z0-9_,]"), "" );
      List = str.split(',');
      qDebug() << "List = " << List;
    }

    // use the screen-compatible metric
    QFontMetrics metrics(QucsSettings.font, 0);   // get size of text
    qDebug() << "desc = " << desc << metrics.boundingRect(desc).width();
    while(metrics.boundingRect(desc).width() > 270) {  // if description too long, cut it nicely
      // so 270 above will be the maximum size of the name label and associated edit line widget 
      if (desc.lastIndexOf(' ') != -1)
        desc = desc.left(desc.lastIndexOf(' ')) + "....";
      else
        desc = desc.left(desc.length()-5) + "....";
    }

    if (Comp->SpiceModel == "NutmegEq" && name == "Simulation") { // simulation selection required
        List = getSimulationList();
    }

    Description->setText(desc);

    if(List.count() >= 1) {    // ComboBox with value list or line edit ?
      ComboEdit->clear();
      ComboEdit->insertItems(0,List);

      for(int i=ComboEdit->count()-1; i>=0; i--)
       if(value == ComboEdit->itemText(i)) {
         ComboEdit->setCurrentIndex(i);
	 break;
       }
      edit->setVisible(false);
      ComboEdit->setVisible(true);
      ComboEdit->setFocus();
    }
    else {
      edit->setVisible(true);
      ComboEdit->setVisible(false);
      edit->setFocus();   // edit QLineEdit
    }
  }
}

// -------------------------------------------------------------------------
void ComponentDialog::slotApplyChange(int idx)
{
  /// \bug what if the table have no items?
  // pick selected row
  QList<QTableWidgetItem *> items = prop->selectedItems();
  Q_ASSERT(!items.isEmpty());
  QTableWidgetItem *item = items.first();
  
  int row = item->row();
  
  auto Text = ComboEdit->itemText(idx);
  edit->setText(Text);
  // apply edit line
  prop->item(row, 1)->setText(Text);

  ComboEdit->setFocus();

  // step to next item if not at the last line
  if ( row < (prop->rowCount() - 1)) {
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
  QTableWidgetItem *item = prop->currentItem();
  
  if(!item)
    return;
  
  int row = item->row();

  QString name  = prop->item(row, 0)->text();
  QString value = prop->item(row, 1)->text();

 

  if (!ComboEdit->isHidden())   // take text from ComboBox ?
    edit->setText(ComboEdit->currentText());

  // apply edit line
  if(value != edit->text()) {
       prop->item(row, 1)->setText(edit->text());
    }

  if (!NameEdit->isHidden())	// also apply property name ?
    if (name != NameEdit->text()) {
//      if(NameEdit->text() == "Export")
//        item->setText(0, "Export_");   // name must not be "Export" !!!
//      else
//      item->setText(0, NameEdit->text());  // apply property name
      prop->item(row, 0)->setText(NameEdit->text());
    }

  // step to next item
  if ( row < prop->rowCount()-1) {
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
  if (prop->rowCount() == 0) return;
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
  QSettings settings("qucs","qucs_s");
  settings.setValue("ComponentDialog/geometry", saveGeometry());

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
    switch(comboType->currentIndex()) {
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
    if(comboType->currentIndex() < 2) {
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
     if (!NameEdit->isHidden())
       if (name != NameEdit->text())
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
         Q_ASSERT(prop->rowCount() >= 0);
         if ( (int) Comp->Props.count() < prop->rowCount() +1) {
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
    if ( (int) Comp->Props.count() != prop->rowCount()) { // If props count was changed after recreation
      Q_ASSERT(prop->rowCount() >= 0);
      updateCompPropsList(); // of component we need to update properties
    }
  }

}

// -------------------------------------------------------------------------
void ComponentDialog::slotBrowseFile()
{
  // current file name from the component properties
  QString currFileName = prop->item(prop->currentRow(), 1)->text();
  QFileInfo currFileInfo(currFileName);
  // name of the schematic where component is instantiated (may be empty)
  QFileInfo schematicFileInfo = Comp->getSchematic()->getFileInfo();
  QString schematicFileName = schematicFileInfo.fileName();
  // directory to use for the file open dialog
  QString currDir;

  if (!currFileName.isEmpty()) { // a file name is already defined
    if (currFileInfo.isRelative()) { // but has no absolute path
      if (!schematicFileName.isEmpty()) { // if schematic has a filename
	// build the an absolute file name using the schematic path
	currDir = schematicFileInfo.absolutePath() + 
	          QDir::separator() +
                  currFileInfo.fileName();
      } else { // no absolute paths around
	// use the WorkDir path
	currDir = QucsSettings.QucsWorkDir.path() + 
	          QDir::separator() +
	  currFileInfo.fileName();
      }
    } else { // current file name is absolute
      // use it
      currDir = currFileName;
    }
  } else { // a file name is not defined
    if (!schematicFileName.isEmpty()) { // if schematic has a filename
      // use the schematic absolute path
      currDir = schematicFileInfo.absolutePath();
    } else { // no absolute paths around
      // use the WorkDir path
      currDir = QucsSettings.QucsWorkDir.path();
    }
  }
  
  QString s = QFileDialog::getOpenFileName (
          this,
          tr("Select a file"),
          currDir,
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
       QucsSettings.QucsWorkDir.absolutePath() == file.absolutePath()) s = file.fileName();
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
 If desc is empty, ButtAdd is enabled, this slot handles if it is clicked.
 Used with: Equation, ?

 Original behavior for an Equation block
  - start with props
    y=1 (Name, Value)
    Export=yes
  - add equation, results
    y=1
    y2=1
    Export=yes

  Behavior:
   If Name already exists, set it to focus
   If new name, insert item after selected, set it to focus

*/
void ComponentDialog::slotButtAdd()
{
  // Set existing equation into focus, return
  for(int row=0; row < prop->rowCount(); row++) {
    QString name  = prop->item(row, 0)->text();
    if( name == NameEdit->text()) {
      prop->setCurrentItem(prop->item(row,0));
      slotSelectProperty(prop->item(row,0));
      return;
    }
  }

  // toggle display flag
  QString s = tr("no");
  if(disp->isChecked())
    s = tr("yes");

  // get number for selected row
  int curRow = prop->currentRow();

  // insert new row under current
  int insRow = curRow+1;
  prop->insertRow(insRow);

  // append new row
  QTableWidgetItem *cell;
  cell = new QTableWidgetItem(NameEdit->text());
  cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
  prop->setItem(insRow, 0, cell);
  cell = new QTableWidgetItem(edit->text());
  cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
  prop->setItem(insRow, 1, cell);
  cell = new QTableWidgetItem(s);
  cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
  prop->setItem(insRow, 2, cell);
  // no description? add empty cell
  cell = new QTableWidgetItem("");
  cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
  prop->setItem(insRow, 3, cell);

  // select new row
  prop->selectRow(insRow);
}

/*!
 Is called if the remove button is pressed. This is only possible for
 some properties.
 If desc is empty, ButtRem is enabled, this slot handles if it is clicked.
 Used with: Equations, ?
*/
void ComponentDialog::slotButtRem()
{
  if ((prop->rowCount() < 3)&&
          (Comp->Model=="Eqn"||Comp->Model=="NutmegEq"))
     return;  // the last property cannot be removed
  if (prop->rowCount() < 2)
     return;  // the last property cannot be removed


  QTableWidgetItem *item = prop->selectedItems()[0];
  int row = item->row();

  if(item == 0)
    return;

  // peek next, delete current, set next current
  if ( row < prop->rowCount()) {
      prop->setCurrentItem(prop->item(row-1,0)); // Shift selection up
      slotSelectProperty(prop->item(row-1,0));
      prop->removeRow(row);
      if (!prop->selectedItems().size()) { // The first item was removed
          prop->setCurrentItem(prop->item(0,0)); // Select the first item
          slotSelectProperty(prop->item(0,0));
      }
  }
}

/*!
 * \brief ComponentDialog::slotButtUp
 * Move a table item up. Enabled for Equation component.
 */
void ComponentDialog::slotButtUp()
{
  qDebug() << "slotButtUp" << prop->currentRow() << prop->rowCount();

  int curRow = prop->currentRow();
  if (curRow == 0)
    return;
  if ((curRow == 1)&&(Comp->Model=="NutmegEq"))
    return;

  // swap current and row above it
  QTableWidgetItem *source = prop->takeItem(curRow  ,0);
  QTableWidgetItem *target = prop->takeItem(curRow-1,0);
  prop->setItem(curRow-1, 0, source);
  prop->setItem(curRow, 0, target);
  source = prop->takeItem(curRow  ,1);
  target = prop->takeItem(curRow-1,1);
  prop->setItem(curRow-1, 1, source);
  prop->setItem(curRow, 1, target);


  // select moved row
  prop->selectRow(curRow-1);
}

/*!
 * \brief ComponentDialog::slotButtDown
 * Move a table item down. Enabled for Equation component.
 */
void ComponentDialog::slotButtDown()
{
  qDebug() << "slotButtDown" << prop->currentRow() << prop->rowCount();

  int curRow = prop->currentRow();
  // Leave Export as last
  if ((curRow == prop->rowCount()-2)&&(Comp->Model=="Eqn"))
    return;
  if ((curRow == 0)&&(Comp->Model=="NutmegEq")) // Don't let to shift the first property "Simulation="
    return;
  if ((curRow ==  prop->rowCount()-1)) // Last property
    return;

  // swap current and row below it
  QTableWidgetItem *source = prop->takeItem(curRow,0);
  QTableWidgetItem *target = prop->takeItem(curRow+1,0);
  prop->setItem(curRow+1, 0, source);
  prop->setItem(curRow, 0, target);
  source = prop->takeItem(curRow,1);
  target = prop->takeItem(curRow+1,1);
  prop->setItem(curRow+1, 1, source);
  prop->setItem(curRow, 1, target);

  // select moved row
  prop->selectRow(curRow+1);
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
  if(comboType->currentIndex() == 1) {   // logarithmic ?
    misc::str2num(editStop->text(), x, Unit, Factor);
    x *= Factor;
    misc::str2num(editStart->text(), y, Unit, Factor);
    y *= Factor;
    if(y == 0.0)  y = x / 10.0;
    if(x == 0.0)  x = y * 10.0;
    if(y == 0.0) { y = 1.0;  x = 10.0; }
    x = editNumber->text().toDouble() / log10(fabs(x / y));
    Unit = QString::number(x);
  }
  else {
    misc::str2num(editStop->text(), x, Unit, Factor);
    x *= Factor;
    misc::str2num(editStart->text(), y, Unit, Factor);
    y *= Factor;
    x = (x - y) / (editNumber->text().toDouble() - 1.0);

    QString step = misc::num2str(x);

    misc::str2num(step, x, Unit, Factor);
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
  if(comboType->currentIndex() == 1) {   // logarithmic ?
    misc::str2num(editStop->text(), x, Unit, Factor);
    x *= Factor;
    misc::str2num(editStart->text(), y, Unit, Factor);
    y *= Factor;

    x /= y;
    misc::str2num(Step, y, Unit, Factor);
    y *= Factor;

    x = log10(fabs(x)) * y;
  }
  else {
    misc::str2num(editStop->text(), x, Unit, Factor);
    x *= Factor;
    misc::str2num(editStart->text(), y, Unit, Factor);
    y *= Factor;

    x -= y;
    misc::str2num(Step, y, Unit, Factor);
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

// if clicked on 'display' header toggle visibility for all items
void ComponentDialog::slotHHeaderClicked(int headerIdx)
{
  if (headerIdx != 2) return; // clicked on header other than 'display'

  QString s;
  QTableWidgetItem *cell;

  if (setAllVisible) {
    s = tr("yes");
    disp->setChecked(true);
  } else {
    s = tr("no");
    disp->setChecked(false);
  }

  // go through all the properties table and set the visibility cell
  for (int row = 0; row < prop->rowCount(); row++) {
    cell = prop->item(row, 2);
    cell->setText(s);
  }
  setAllVisible = not setAllVisible; // toggle visibility for the next double-click
}


QStringList ComponentDialog::getSimulationList()
{
    QStringList sim_lst;
    Schematic *sch = Comp->getSchematic();
    if (sch == nullptr) {
        return sim_lst;
    }
    sim_lst.append("ALL");
    for (size_t i = 0; i < sch->DocComps.count(); i++) {
        Component *c = sch->DocComps.at(i);
        if (!c->isSimulation) continue;
        if (c->Model == ".FOUR") continue;
        if (c->Model == ".PZ") continue;
        if (c->Model == ".SENS") continue;
        if (c->Model == ".SENS_AC") continue;
        if (c->Model == ".SW" && !c->Props.at(0)->Value.toUpper().startsWith("DC") ) continue;
        sim_lst.append(c->Name);
    }
    QStringList sim_wo_numbers = sim_lst;
    for(auto &s: sim_wo_numbers) {
        s.remove(QRegularExpression("[0-9]+$"));
    }
    for(const auto &s: sim_wo_numbers) {
        int cnt = sim_wo_numbers.count(s);
        if (cnt > 1 && ! sim_lst.contains(s)) {
            sim_lst.append(s);
        }
    }
    return sim_lst;
}
