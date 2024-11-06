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
#include "schematic.h"
#include "settings.h"
#include "misc.h"
#include "fillfromspicedialog.h"

#include <cmath>

// #include <QLabel>
// #include <QLayout>
// #include <QValidator>
// #include <QTableWidget>
// #include <QHeaderView>
// #include <QFileDialog>
// #include <QLineEdit>
// #include <QCheckBox>
// #include <QComboBox>
// #include <QGroupBox>
// #include <QPushButton>
// #include <QEvent>
// #include <QKeyEvent>
// #include <QDebug>


// TODO: Make this a member, but at the moment it is causing a segfault, probably
// because of abi incompatibility with some library found at runtime (adding a QString
// also causes the same problem)
// QMap<QPair<QString, QString>, SweepSettingUIList> sweepSpecs;


// -------------------------------------------------------------------------
// Convenience base class to add a label, name, checkbox and bind them to a variable.
class BoundControl
{
  public:
    BoundControl(const QString& label, bool check, QLayout* layout) 
    {
      mpParentLayout = layout;

      mpShowNameCheck = new QCheckBox("display in schematic");

      // qDebug() << layout->metaObject()->className();
      
      // Either add widgets to a new QHBoxLayout or to an existing QGridLayout.
      if (layout->metaObject()->className() == QString("QVBoxLayout"))
      {
        mpBoundLayout = new QHBoxLayout;
        mpBoundLayout->setSpacing(5);
        static_cast<QVBoxLayout*>(mpParentLayout)->addLayout(mpBoundLayout);

        if (!label.isEmpty())
          mpBoundLayout->addWidget(new QLabel(label));

        mpShowNameCheck->setCheckState(check ? Qt::Checked : Qt::Unchecked);
        mpBoundLayout->addWidget(mpShowNameCheck);
      }
      
      else
      {
        QGridLayout* grid = static_cast<QGridLayout*>(mpParentLayout);
        int row = grid->rowCount();

        // Set object name to the property name so it is searchable.
        QLabel* propertyLabel = new QLabel(label + ":");
        propertyLabel->setObjectName(label);
        grid->addWidget(propertyLabel, row, 0);

        mpShowNameCheck->setCheckState(check ? Qt::Checked : Qt::Unchecked);
        grid->addWidget(mpShowNameCheck, row, 2);
      }
    }

  protected:
    QLayout* mpParentLayout;
    QHBoxLayout* mpBoundLayout = nullptr;
    QCheckBox* mpShowNameCheck;
};

// -------------------------------------------------------------------------
// Line edit version of bound control. Has label | line | checkbox.
class BoundLineEdit : public QLineEdit, BoundControl
{
  public:
    // Direct QString editing.
    BoundLineEdit(const QString& label, QString& boundValue, bool& boundCheck, 
                              QLayout* layout, QValidator* validator = nullptr) 
    : BoundControl(label, boundCheck, layout)
    {
      if (validator)
        setValidator(validator);

      if (mpBoundLayout)
        mpBoundLayout->insertWidget(1, this);
      else
        mpParentLayout->addWidget(this);

      setText(boundValue);

      // Update the bound values whenever the user makes a change.
      connect(this, &QLineEdit::textChanged, [&boundValue](const QString& text) { boundValue = text; });
      connect(mpShowNameCheck, &QCheckBox::stateChanged, [&boundCheck](int state) mutable { boundCheck = state; });
    }

    // Parameter property editing.
    BoundLineEdit(Property* property, QLayout* layout, QValidator* validator = nullptr)
    : BoundControl(property->Name, property->display, layout)
    {
      if (validator)
        setValidator(validator);

      QGridLayout* grid = static_cast<QGridLayout*>(mpParentLayout);
      grid->addWidget(this, grid->rowCount() - 1, 1);
      setText(property->Value);

      // Update the bound values whenever the user makes a change.
      // connect(this, &QLineEdit::textChanged, [&boundValue](const QString& text) { boundValue = text; });
      // connect(mpShowNameCheck, &QCheckBox::stateChanged, [&boundCheck](int state) mutable { boundCheck = state; });
    }    
};

// -------------------------------------------------------------------------
// Combo box version of bound control. Has label | combo box | checkbox.
class BoundComboBox : public QComboBox, BoundControl
{
  public:
    // Parameter property editing.
    BoundComboBox(Property* property, const QStringList& options, QLayout* layout) 
    : BoundControl(property->Name, property->display, layout)
    {
      addItems(options);
      setCurrentIndex(findText(property->Value, Qt::MatchStartsWith));

      QGridLayout* grid = static_cast<QGridLayout*>(mpParentLayout);
      grid->addWidget(this, grid->rowCount() - 1, 1);

      QString& temp(property->Value);

      // Update the bound values whenever the user makes a change.
      connect(this, &QComboBox::currentTextChanged, [&temp](const QString& text) { temp = text; qDebug() << temp; });
      // connect(mpShowNameCheck, &QCheckBox::stateChanged, [&boundCheck](int state) mutable { boundCheck = state; });
    }    
};

// -------------------------------------------------------------------------
// Helper to extract option strings between [] from within a description
QStringList getOptionsFromString(const QString& description)
{
   // Check description for combo box options and create a combo box if found.
  int start = description.indexOf('[');
  int end = description.indexOf(']');

  if (start != -1 && end != -1)
    return description.mid(start + 1, end - start - 1).split(',');

  return QStringList();
}

// -------------------------------------------------------------------------
// Convenience base class to add a label, and checkbox.
class ParamWidget
{
  public:
    ParamWidget(const QString& label, QGridLayout* layout)
    : mParam(label)
    {
      int row = layout->rowCount();

      mLabel = new QLabel(label + ":");
      layout->addWidget(mLabel, row, 0);

      mCheckBox = new QCheckBox("display in schematic");
      mCheckBox->setEnabled(false);
      layout->addWidget(mCheckBox, row, 2);  
    }

    void setLabel(const QString& label)
    {
      mLabel->setText(label);
    }

    void setCheck(bool checked)
    {
      mCheckBox->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
    }

    virtual void setEnabled(bool enabled)
    {
      mLabel->setEnabled(enabled);
      mCheckBox->setEnabled(enabled);
    }

    virtual void setHidden(bool hidden)
    {
      mLabel->setVisible(!hidden);
      mCheckBox->setVisible(!hidden);
    }

    virtual void setValue(const QString& value) = 0;
    virtual void setOptions(const QStringList& options) = 0;
    virtual QString value() = 0;

  protected:
    QString mParam;

  private:
    QLabel* mLabel;
    QCheckBox* mCheckBox;
};

// -------------------------------------------------------------------------
// Convenience class to display label, edit, and checkbox.
class ParamLineEdit : public QLineEdit, public ParamWidget
{
  public:
    ParamLineEdit(const QString& param, QGridLayout* layout, ComponentDialog* dialog, 
                  void (ComponentDialog::* func)(const QString&, const QString&))
    : ParamWidget(param, layout)
    {
      layout->addWidget(this, layout->rowCount() - 1, 1);
      // Note: need to be careful about life of dialog and param here.
      connect(this, &QLineEdit::textEdited, [=](const QString& value) { if (dialog) (dialog->*func)(param, value); });
    }

    void setEnabled(bool enabled) override
    {
      ParamWidget::setEnabled(enabled);
      QLineEdit::setEnabled(enabled);
    }

    void setHidden(bool hidden) override
    {
      ParamWidget::setHidden(hidden);
      QLineEdit::setVisible(!hidden);
    }

    void setValue(const QString& value) override
    {
      setText(value);
    }

    void setOptions(const QStringList& options) override { (void)options; }

    QString value() override
    {
      return text();
    }
};

// -------------------------------------------------------------------------
// Convenience class to display label, combo, and checkbox.
class ParamCombo : public QComboBox, public ParamWidget
{
  public:
    ParamCombo(const QString& param, QGridLayout* layout, ComponentDialog* dialog, 
                void (ComponentDialog::* func)(const QString&, const QString&))
    : ParamWidget(param, layout)
    {
      layout->addWidget(this, layout->rowCount() - 1, 1);
      // Note: need to be careful about life of dialog and param here.
      connect(this, &QComboBox::currentTextChanged, [=](const QString& value) { if (dialog) (dialog->*func)(param, value); });
    }

    void setEnabled(bool enabled) override
    {
      ParamWidget::setEnabled(enabled);
      QComboBox::setEnabled(enabled);
    }

    void setHidden(bool hidden) override
    {
      ParamWidget::setHidden(hidden);
      QComboBox::setVisible(!hidden);
    }

    void setValue(const QString& value) 
    {
      setCurrentIndex(findText(value));
    }

    void setOptions(const QStringList& options) 
    {
      clear();
      addItems(options);
    }

    QString value() override
    {
      return currentText();
    }

  private:
    void (ComponentDialog::* function)(const QString&, const QString&);
    ComponentDialog* mDialog;

};

ComponentDialog::ComponentDialog(Component* component, Schematic* schematic)
			: QDialog(schematic)
{
  originalComponent = component;
  document = schematic;

  qDebug() << "Processing component: " << editComponent.Model << " name: " << editComponent.Name;

  // Make a deep copy of component for edit editing (will be discarded if user cancels).
  editComponent = *component;
  editComponent.Props = component->Props;
  changed = false;

  // sweepSpecs[qMakePair(QString("a"), QString("b"))] = {{"label", "value", {}, false, true, {{"number", StepToNumber}}}, {"label", "value", {}, true, true}};

  restoreGeometry(_settings::Get().item<QByteArray>("ComponentDialog/geometry"));
  setWindowTitle(tr("Edit Component Properties") + " - " + originalComponent->Description.toUpper());

  // Setup dialog layout.
  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  QGridLayout* propertiesPageLayout;

  // Setup validators.
  ValInteger = new QIntValidator(1, 1000000, this);
  Expr.setPattern("[^\"=]*");  // valid expression for property 'edit'
  Validator = new QRegularExpressionValidator(Expr, this);
  Expr.setPattern("[^\"]*");   // valid expression for property 'edit'
  Validator2 = new QRegularExpressionValidator(Expr, this);
  Expr.setPattern("[\\w_.,\\(\\) @:\\[\\]]+");  // valid expression for property 'NameEdit'. Space to enable Spice-style par sweep
  ValRestrict = new QRegularExpressionValidator(Expr, this);
  Expr.setPattern("[A-Za-z][A-Za-z0-9_]+");
  ValName = new QRegularExpressionValidator(Expr,this);

  mpNameLineEdit = new BoundLineEdit("Name:", editComponent.Name, editComponent.showName, mainLayout, Validator);

 
  // last property shown elsewhere outside the properties table, not to put in TableView ?
  // auto pp = originalComponent->Props.begin();

  // TODO: It would be better for simulations with sweeps to have a flag saying so.
  QStringList hasSweep({".SW", ".SP", ".AC", ".NOISE"});
  QStringList sweepProperties({"Sim", "Param", "Type", "Values", "Start", "Stop", "Points"});

  if (originalComponent->isSimulation && 
        hasSweep.contains(originalComponent->Model))
  {
    // Create tab widget to hold both sweep and parameter pages.
    QTabWidget* pageTabs = new QTabWidget(this);
    layout()->addWidget(pageTabs);

    // Simulations have a separate sweep page.
    QWidget* sweepPage = new QWidget(pageTabs);
    pageTabs->addTab(sweepPage, tr("Sweep"));
    QGridLayout* sweepPageLayout = new QGridLayout;
    sweepPage->setLayout(sweepPageLayout);

    // Sweep page setup - add widgets for each possible sweep property.  
    sweepParam["Sim"] = new ParamCombo("Sim", sweepPageLayout, this, &ComponentDialog::updateSimProperty);
    sweepParam["Param"] = new ParamLineEdit("Param", sweepPageLayout, this, &ComponentDialog::updateSimProperty);
    sweepParam["Type"] = new ParamCombo("Type", sweepPageLayout, this, &ComponentDialog::updateSimProperty);
    sweepParam["Values"] = new ParamLineEdit("Values", sweepPageLayout, this, &ComponentDialog::updateSimProperty);
    sweepParam["Start"] = new ParamLineEdit("Start", sweepPageLayout, this, &ComponentDialog::updateSimProperty);
    sweepParam["Stop"] = new ParamLineEdit("Stop", sweepPageLayout, this, &ComponentDialog::updateSimProperty);
    sweepParam["Number"] = new ParamLineEdit("Number", sweepPageLayout, this, &ComponentDialog::updateSimProperty);

    sweepParam["Type"]->setOptions({"lin", "log", "list", "value"});
    sweepParam["Type"]->setValue("lin");

    // Setup the widget specialisations for each simulation type.    
    sweepTypeEnabledParams["lin"] = QStringList{"Sim", "Param", "Type", "Start", "Stop", "Step", "Number"};    
    sweepTypeEnabledParams["log"] = QStringList{"Sim", "Param", "Type", "Start", "Stop", "Step", "Number"};
    sweepTypeEnabledParams["list"] = QStringList{"Type", "Values"};
    sweepTypeEnabledParams["value"] = QStringList{"Type", "Values"};
    sweepTypeHiddenParams["Sim"] = QStringList{".AC"};
    sweepTypeHiddenParams["Param"] = QStringList{".AC"};
    sweepTypeSpecialLabels["log"] = {{"Step:", "Points per decade"}};

    updateSimWidgets("lin"); // editComponent.getProperty("Type")->Name);
    updateSimProperty("All", "");


    /*
    
    editParam = new QLineEdit(sweepPage);
    if (originalComponent->Model != ".SW") editParam->setValidator(ValRestrict);
    connect(editParam, SIGNAL(returnPressed()), SLOT(slotParamEntered()));
    checkParam = new QCheckBox(tr("display in schematic"), sweepPage);

    if(originalComponent->Model == ".SW") {   // parameter sweep
      textSim = new QLabel(tr("Simulation:"), sweepPage);
      sweepPageLayout->addWidget(textSim, row,0);
      comboSim = new QComboBox(sweepPage);
      comboSim->setEditable(true);
      connect(comboSim, SIGNAL(activated(int)), SLOT(slotSimEntered(int)));
      sweepPageLayout->addWidget(comboSim, row,1);
      checkSim = new QCheckBox(tr("display in schematic"), sweepPage);
      sweepPageLayout->addWidget(checkSim, row++,2);
    }
    else {
      editParam->setReadOnly(true);
      checkParam->setDisabled(true);

      if(originalComponent->Model == ".TR")    // transient simulation ?
        editParam->setText("time");
      else {
        if(originalComponent->Model == ".AC")    // AC simulation ?
          editParam->setText("acfrequency");
        else
          editParam->setText("frequency");
      }
    }

    sweepPageLayout->addWidget(new QLabel(tr("Sweep Parameter:"), sweepPage), row,0);
    sweepPageLayout->addWidget(editParam, row,1);
    sweepPageLayout->addWidget(checkParam, row++,2);

*/

    /*
    comboType = new QComboBox(sweepPage);
    comboType->hide();
    checkType = new QCheckBox(tr("display in schematic"), sweepPage);
    checkType->hide();
    editValues = new QLineEdit(sweepPage);
    editValues->hide();
    checkValues = new QCheckBox(tr("display in schematic"), sweepPage);
    checkValues->hide();
    editStart  = new QLineEdit(sweepPage);
    editStart->hide();
    checkStart = new QCheckBox(tr("display in schematic"), sweepPage);
    checkStart->hide();
    editStop   = new QLineEdit(sweepPage);
    editStop->hide();
    checkStop = new QCheckBox(tr("display in schematic"), sweepPage);
    checkStop->hide();
    editStep   = new QLineEdit(sweepPage);
    editStep->hide();
    editNumber = new QLineEdit(sweepPage);
    editNumber->hide();
    checkNumber = new QCheckBox(tr("display in schematic"), sweepPage);
    checkNumber->hide();
    */

/*
    textType = new QLabel(tr("Type:"), sweepPage);
    sweepPageLayout->addWidget(textType, row,0);
    comboType->insertItems(0, QStringList({"linear", "logarithmic", "list", "constant"}));
			   
    sweepPageLayout->addWidget(comboType, row,1);
    connect(comboType, SIGNAL(activated(int)), SLOT(slotSimTypeChange(int)));
    sweepPageLayout->addWidget(checkType, row++,2);

    textValues = new QLabel(tr("Values:"), sweepPage);
    sweepPageLayout->addWidget(textValues, row,0);
    editValues->setValidator(Validator);
    connect(editValues, SIGNAL(returnPressed()), SLOT(slotValuesEntered()));
    sweepPageLayout->addWidget(editValues, row,1);
    sweepPageLayout->addWidget(checkValues, row++,2);

    textStart  = new QLabel(tr("Start:"), sweepPage);
    sweepPageLayout->addWidget(textStart, row,0);
    editStart->setValidator(Validator);
    connect(editStart, SIGNAL(returnPressed()), SLOT(slotStartEntered()));
    sweepPageLayout->addWidget(editStart, row,1);
    sweepPageLayout->addWidget(checkStart, row++,2);

    textStop   = new QLabel(tr("Stop:"), sweepPage);
    sweepPageLayout->addWidget(textStop, row,0);
    editStop->setValidator(Validator);
    connect(editStop, SIGNAL(returnPressed()), SLOT(slotStopEntered()));
    sweepPageLayout->addWidget(editStop, row,1);
    sweepPageLayout->addWidget(checkStop, row++,2);

    textStep   = new QLabel(tr("Step:"), sweepPage);
    sweepPageLayout->addWidget(textStep, row,0);
    editStep->setValidator(Validator);
    connect(editStep, SIGNAL(returnPressed()), SLOT(slotStepEntered()));
    sweepPageLayout->addWidget(editStep, row++,1);

    textNumber = new QLabel(tr("Number:"), sweepPage);
    sweepPageLayout->addWidget(textNumber, row,0);
    editNumber->setValidator(ValInteger);
    connect(editNumber, SIGNAL(returnPressed()), SLOT(slotNumberEntered()));
    sweepPageLayout->addWidget(editNumber, row,1);
    sweepPageLayout->addWidget(checkNumber, row++,2);
*/

    /*
    // TODO: Get rid of this.
    QString s;

    // Parameter sweep.
    if(false) // originalComponent->Model == ".SW")
    { 
      // Add all sweep components (starting with a .) to the combo box.

      Component* component;
      for (component = document->Components->first(); component != 0; document->Components->next()) 
      {
        if (component != originalComponent && component->Model[0] == '.')
            comboSim->insertItem(comboSim->count(), component->Name);
      }
      
      qDebug() << "[]" << (*pp)->Value;
      // set selected simulations in combo box to the currently used one
      int i = comboSim->findText((*pp)->Value);
      if (i != -1) // current simulation is in the available simulations list (normal case)
	      comboSim->setCurrentIndex(i);
      else  // current simulation not in the available simulations list
        comboSim->setEditText((*pp)->Value);

      checkSim->setChecked((*pp)->display);
      ++pp;
      s = (*pp)->Value;
      checkType->setChecked((*pp)->display);
      ++pp;
      editParam->setText((*pp)->Value);
      checkParam->setChecked((*pp)->display);
    }
    else {
      s = (*pp)->Value;
      checkType->setChecked((*pp)->display);
    }

    ++pp;
    editStart->setText((*pp)->Value);
    checkStart->setChecked((*pp)->display);
    ++pp;
    editStop->setText((*pp)->Value);
    checkStop->setChecked((*pp)->display);
    ++pp;  // remember last property for ListView
    editNumber->setText((*pp)->Value);
    checkNumber->setChecked((*pp)->display);
    */

    /* Converts sweep type text into a number and calls to update
    int tNum = 0;
    if (s[0] == 'l') 
    {
      if(s[1] == 'i') 
      {
	      if(s[2] != 'n')
	        tNum = 2;
      }
      else  tNum = 1;
    }
    else  tNum = 3;
    
    // comboType->setCurrentIndex(tNum);

    slotSimTypeChange(tNum);   // not automatically ?!?
    if (tNum > 1) 
    {
      editValues->setText(
		  editNumber->text().mid(1, editNumber->text().length() - 2));
      checkValues->setChecked((*pp)->display);
      editNumber->setText("2");
    }
    slotNumberChanged(0);
    ++pp;
    */

/*    connect(editValues, SIGNAL(textChanged(const QString&)),
	    SLOT(slotTextChanged(const QString&)));
    connect(editStart, SIGNAL(textChanged(const QString&)),
	    SLOT(slotNumberChanged(const QString&)));
    connect(editStop, SIGNAL(textChanged(const QString&)),
	    SLOT(slotNumberChanged(const QString&)));
    connect(editStep, SIGNAL(textChanged(const QString&)),
	    SLOT(slotStepChanged(const QString&)));
    connect(editNumber, SIGNAL(textChanged(const QString&)),
	    SLOT(slotNumberChanged(const QString&)));
*/
/*    if(checkSim)
      connect(checkSim, SIGNAL(stateChanged(int)), SLOT(slotSetChanged(int)));
    connect(checkType, SIGNAL(stateChanged(int)), SLOT(slotSetChanged(int)));
    connect(checkParam, SIGNAL(stateChanged(int)), SLOT(slotSetChanged(int)));
    connect(checkStart, SIGNAL(stateChanged(int)), SLOT(slotSetChanged(int)));
    connect(checkStop, SIGNAL(stateChanged(int)), SLOT(slotSetChanged(int)));
    connect(checkNumber, SIGNAL(stateChanged(int)), SLOT(slotSetChanged(int)));
*/

    // Create the properties page and add it to the tab widget.
    QWidget* propertiesPage = new QWidget(pageTabs);
    pageTabs->addTab(propertiesPage, tr("Properties"));
    propertiesPageLayout = new QGridLayout(propertiesPage);
  }
  
  // This component does not have sweep settings, add properties directly to the dialog itself.
  else 
  { 
    propertiesPageLayout = new QGridLayout(this);
    static_cast<QVBoxLayout*>(layout())->addLayout(propertiesPageLayout);
  }

  // Create the properties table.
  QGroupBox *propertyGroup = new QGroupBox(tr("Properties"));
  propertiesPageLayout->addWidget(propertyGroup, 2, 0);
  QHBoxLayout *propertyTableLayout = new QHBoxLayout(propertyGroup);

   /// \todo column min width + make widths persistent
  propertyTable = new QTableWidget(0, 4);
  propertyTable->setMinimumSize(200, 150);
  propertyTable->verticalHeader()->setVisible(false);
  propertyTable->horizontalHeader()->setStretchLastSection(true);
  propertyTable->horizontalHeader()->setSectionsClickable(false);
  propertyTable->setHorizontalHeaderLabels({tr("Name"), tr("Value"), tr("Show"), tr("Description")});
  propertyTable->setColumnWidth(0, 100);
  propertyTable->setColumnWidth(1, 150);
  propertyTable->setColumnWidth(2, 50);
  propertyTable->setSelectionMode(QAbstractItemView::SingleSelection);
  // propertyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  propertyTableLayout->addWidget(propertyTable, 2);

  // Doubleclick header to set all visible (TODO: Replace with button).
  connect(propertyTable->horizontalHeader(),SIGNAL(sectionDoubleClicked(int)),
              this, SLOT(slotHHeaderClicked(int)));

  connect(propertyTable, &QTableWidget::cellActivated, [=](int row, int column) { qDebug() << "Activated " << row << "," << column; } );
  connect(propertyTable, &QTableWidget::cellClicked, [=](int row, int column) { qDebug() << "Clicked " << row << "," << column; } );
  connect(propertyTable, &QTableWidget::cellPressed, [=](int row, int column) { qDebug() << "Pressed " << row << "," << column; } );
  connect(propertyTable, &QTableWidget::cellEntered, [=](int row, int column) { qDebug() << "Entered " << row << "," << column; } );


  /* Right panel with buttons etc.
  QWidget *vboxPropsR = new QWidget;
  QVBoxLayout *v1 = new QVBoxLayout;
  vboxPropsR->setLayout(v1);

  v1->setSpacing(3);

  // propertyTableLayout->addWidget(vboxPropsL, 5); // stretch the left pane (with the table) when resized
  propertyTableLayout->addWidget(vboxPropsR);

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
  */

  /* Fill from Spice function ***TODO*** do not delete.
  QStringList allowedFillFromSPICE({"_BJT", "JFET", "MOSFET", "_MOSFET", "Diode", "BJT"});
  ButtFillFromSpice = new QPushButton(tr("Fill from SPICE .MODEL"));
  if (!allowedFillFromSPICE.contains(originalComponent->Model)) {
    ButtFillFromSpice->setEnabled(false);
  }
  bg->addWidget(ButtFillFromSpice,2,0,1,2);
  connect(ButtFillFromSpice, SIGNAL(clicked(bool)), this, SLOT(slotFillFromSpice()));
  */

  // Add the dialog button widgets.
  QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | 
                                      QDialogButtonBox::Apply | QDialogButtonBox::Cancel);

  connect(buttonBox, &QDialogButtonBox::accepted, this, &ComponentDialog::slotOKButton);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &ComponentDialog::slotApplyButton);
  mainLayout->addWidget(buttonBox);

  /* vvvv Why does the text position need to be modified vvvv.

  // originalComponent->textSize(tx_Dist, ty_Dist);
  // int tmp = originalComponent->tx+tx_Dist - originalComponent->x1;
  // if((tmp > 0) || (tmp < -6))  tx_Dist = 0;  // remember the text position
  // tmp = originalComponent->ty+ty_Dist - originalComponent->y1;
  // if((tmp > 0) || (tmp < -6))  ty_Dist = 0;
  */

  // Add component properties to the properties table with the exception of the sweep properties.
  int row=0; // row counter

  for (Property* property : editComponent.Props)
  {
    // qDebug() << " Loading originalComponent->Props :" << (*p)->Name << (*p)->Value << (*p)->display << (*p)->Description ;
    // Check this is a sweep property.
    if (sweepProperties.contains(property->Name))
    {
      qDebug() << "Sweep property (do not add to property table): " << property->Name;
      continue;
    }

    propertyTable->setRowCount(propertyTable->rowCount() + 1);
    propertyTable->setItem(row, 0, new QTableWidgetItem(property->Name, LabelCell));
    propertyTable->item(row, 0)->setFlags(Qt::ItemIsEnabled);

    // Check description for combo box options and create a combo box if found.
    QStringList options = getOptionsFromString(property->Description);
    if (!options.isEmpty())
    {
      QComboBox* optionsCombo = new QComboBox();
      optionsCombo->addItems(options);
      propertyTable->setCellWidget(row, 1, optionsCombo);
      propertyTable->setItem(row, 1, new QTableWidgetItem(ComboBoxCell));
    }

    // Add text edit if no options found.
    else
    {
      propertyTable->setItem(row, 1, new QTableWidgetItem(property->Value, TextEditCell));
      propertyTable->openPersistentEditor(propertyTable->item(row, 1));
    }    

    // Set check box and description.
    propertyTable->setItem(row, 2, new QTableWidgetItem(CheckBoxCell));
    propertyTable->item(row, 2)->setCheckState(property->display ? Qt::Checked : Qt::Unchecked);
    propertyTable->setItem(row, 3, new QTableWidgetItem(property->Description, LabelCell));
    propertyTable->item(row, 3)->setFlags(Qt::ItemIsEnabled);

    row++;
  }

  /// \todo add key up/down browse and select propertyTable
  // connect(propertyTable, SIGNAL(itemClicked(QTableWidgetItem*)), SLOT(slotSelectProperty(QTableWidgetItem*)));
}

ComponentDialog::~ComponentDialog()
{
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


void ComponentDialog::updateSimWidgets(const QString& type)
{
  qDebug() << "Update sim widgets " << type;

  for (auto it = sweepParam.keyValueBegin(); it != sweepParam.keyValueEnd(); ++it) 
  {
    qDebug() << "Checking " << it->first;
    it->second->setHidden(sweepTypeHiddenParams.contains(it->first) &&
                            sweepTypeHiddenParams[it->first].contains(editComponent.Model));
    it->second->setEnabled(sweepTypeEnabledParams.contains(type) && 
                            sweepTypeEnabledParams[type].contains(it->first));
  }
}

void ComponentDialog::updateSimProperty(const QString& property, const QString& value)
{
  qDebug() << "Update Sim Properties " << property << " value: " << value;

  if (property == "Type")
    updateSimWidgets(sweepParam["Type"]->value());

  // Repopulate the property fields.
  if (property == "All")
  {
    for (auto property : editComponent.Props)
    {
      qDebug() << "Updating " << property->Name;
      if (sweepParam.contains(property->Name))
      {
        qDebug() << "Setting to value " << property->Value;        
        sweepParam[property->Name]->setValue(property->Value);
      }
        
    }
    /*
    for (auto& field : QStringList{"Values", "Start", "Stop", "Points"})
    {
      Property* property = editComponent.getProperty(field);   
      if (property)
        sweepParam[field]->setValue(editComponent.getProperty(field)->Name);
    }
    */
  }
}

// Updates component property list. Useful for MultiViewComponents
void ComponentDialog::updateCompPropsList()
{
  qDebug() << "Update Comp Props List";
  return;

    int last_prop=0; // last property not to put in ListView
        // ...........................................................
        // if simulation component: .TR, .AC, .SW, (.SP ?)
    if((originalComponent->Model[0] == '.') &&
       (originalComponent->Model != ".DC") && (originalComponent->Model != ".HB") &&
       (originalComponent->Model != ".Digi") && (originalComponent->Model != ".ETR")) {
        if(originalComponent->Model == ".SW") {   // parameter sweep
           last_prop = 2;
        } else {
            last_prop = 0;
        }
            last_prop += 4;  // remember last property for ListView
    }

    QString s;
    int row=0; // row counter
    //for(Property *p = originalComponent->Props.first(); p != 0; p = originalComponent->Props.next()) {
    auto &p = originalComponent->Props;
    for(int i = last_prop; i< p.size();i++) {

      // do not insert if already on first tab
      // this is the reason it was originally from back to front...
      // the 'pp' is the lasted property stepped over while filling the Swep tab
  //    if(p == pp)
  //      break;
      if(p.at(i)->display)
        s = tr("yes");
      else
        s = tr("no");

      // add Props into TableWidget
      qDebug() << " Loading originalComponent->Props :" << p.at(i)->Name << p.at(i)->Value << p.at(i)->display << p.at(i)->Description ;

      if (row > propertyTable->rowCount()-1) { // Add new rows
          propertyTable->setRowCount(propertyTable->rowCount()+1);
      }

      QTableWidgetItem *cell;
      cell = new QTableWidgetItem(p.at(i)->Name);
      cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
      propertyTable->setItem(row, 0, cell);
      cell = new QTableWidgetItem(p.at(i)->Value);
      cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
      propertyTable->setItem(row, 1, cell);
      cell = new QTableWidgetItem(s);
      cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
      propertyTable->setItem(row, 2, cell);
      cell = new QTableWidgetItem(p.at(i)->Description);
      cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
      propertyTable->setItem(row, 3, cell);

      row++;
    }

    if(propertyTable->rowCount() > 0) {
        propertyTable->setCurrentItem(propertyTable->item(0,0));
        slotSelectProperty(propertyTable->item(0,0));
    }

    if (row < propertyTable->rowCount()-1) {
        propertyTable->setRowCount(row);
    }
}

// -------------------------------------------------------------------------
// Updates sweep page widgets to set the specialised names and enabled state.
void ComponentDialog::slotUpdateSweepWidgets(const QString& sweepType, QGridLayout* sweepPageLayout)
{
  qDebug() << "Updating sweep page widgets.";

  // TODO: remove once this is implemented.
  void(sweepType);

  for (int row = 1; row < sweepPageLayout->rowCount(); row++)
  {
    QLayoutItem* item = sweepPageLayout->itemAtPosition(row, 0);

    if (item && item->widget())
    {
      if (item->widget()->objectName() == "Points")
      {
          for (int col = 0; col < 3; col++)
            sweepPageLayout->itemAtPosition(row, col)->widget()->setEnabled(false);             
      }
    }
  }
}

// -------------------------------------------------------------------------
// Is called if a property is selected.
// Handle the Property editor tab.
// It transfers the values to the right side for editing.
void ComponentDialog::slotSelectProperty(QTableWidgetItem *item)
{
  // Do not use this function as not all pointers are initialised.
  return;


  if(item == 0) return;
  item->setSelected(true);  // if called from elsewhere, this was not yet done

  qDebug() << "row " << item->row(); //<< item->text()

  QString name  = propertyTable->item(item->row(),0)->text();
  QString value = propertyTable->item(item->row(),1)->text();
  QString show  = propertyTable->item(item->row(),2)->text();
  QString desc  = propertyTable->item(item->row(),3)->text();

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

  // Empty description so assume editing an equation?
  if(desc.isEmpty()) {
    // show two line edit fields (name and value)
    ButtAdd->setEnabled(true);
    ButtRem->setEnabled(true);

    // Enable Up / Down buttons only for spice and equation components.
    QStringList spiceComponents({"Eqn", "SpicePar", "SpGlobPar", "SpiceIC", "SpiceNodeset", "NutmegEq"});
    if (spiceComponents.contains(originalComponent->Model)) 
    {
      ButtUp->setEnabled(true);
      ButtDown->setEnabled(true);
    }
    else 
    {
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
  } 
  
  // Property is an expression.
  else if (desc == "Expression") 
  { 
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
  } 
  
  // Standard property (description and value)
  else 
  {
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

    if (originalComponent->SpiceModel == "NutmegEq" && name == "Simulation") { // simulation selection required
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
  QList<QTableWidgetItem *> items = propertyTable->selectedItems();
  Q_ASSERT(!items.isEmpty());
  QTableWidgetItem *item = items.first();
  
  int row = item->row();
  
  auto Text = ComboEdit->itemText(idx);
  edit->setText(Text);
  // apply edit line
  propertyTable->item(row, 1)->setText(Text);

  ComboEdit->setFocus();

  // step to next item if not at the last line
  if ( row < (propertyTable->rowCount() - 1)) {
    propertyTable->setCurrentItem(propertyTable->item(row+1,0));
    slotSelectProperty(propertyTable->item(row+1,0));
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
  QTableWidgetItem *item = propertyTable->currentItem();
  
  if(!item)
    return;
  
  int row = item->row();

  QString name  = propertyTable->item(row, 0)->text();
  QString value = propertyTable->item(row, 1)->text();

 

  if (!ComboEdit->isHidden())   // take text from ComboBox ?
    edit->setText(ComboEdit->currentText());

  // apply edit line
  if(value != edit->text()) {
       propertyTable->item(row, 1)->setText(edit->text());
    }

  if (!NameEdit->isHidden())	// also apply property name ?
    if (name != NameEdit->text()) {
//      if(NameEdit->text() == "Export")
//        item->setText(0, "Export_");   // name must not be "Export" !!!
//      else
//      item->setText(0, NameEdit->text());  // apply property name
      propertyTable->item(row, 0)->setText(NameEdit->text());
    }

  // step to next item
  if ( row < propertyTable->rowCount()-1) {
    propertyTable->setCurrentItem(propertyTable->item(row+1,0));
    slotSelectProperty(propertyTable->item(row+1,0));
  }
  else {
    slotOKButton();   // close dialog, if it was the last property
    return;
  }
}

// -------------------------------------------------------------------------
// Is called if the "RETURN"-button is pressed in the "NameEdit" Widget.
void ComponentDialog::slotApplyPropName()
{
  // pick selected row
  QTableWidgetItem *item = propertyTable->selectedItems()[0];
  int row = item->row();

  QString name  = propertyTable->item(row, 0)->text();

  if(name != NameEdit->text()) {
//    if(NameEdit->text() == "Export") {
//	item->setText(0, "Export_");   // name must not be "Export" !!!
//	NameEdit->setText("Export_");
//    }
//      else
    propertyTable->item(row, 0)->setText(NameEdit->text());
  }
  edit->setFocus();   // cursor into "edit" widget
}

// -------------------------------------------------------------------------
// Is called if the checkbox is pressed (changed).
void ComponentDialog::slotApplyState(int State)
{
  // pick selected row
  if (propertyTable->rowCount() == 0) return;
  QTableWidgetItem *item = propertyTable->selectedItems()[0];
  int row = item->row();

  QString disp  = propertyTable->item(row, 2)->text();

  if(item == 0) return;

  QString ButtonState;
  if(State)
    ButtonState = tr("yes");
  else
    ButtonState = tr("no");

  if(disp != ButtonState) {
    propertyTable->item(row, 2)->setText(ButtonState);
  }
}

// -------------------------------------------------------------------------
// Is called if the "OK"-button is pressed.
void ComponentDialog::slotOKButton()
{
  qDebug() << "slotOK";

  QSettings settings("qucs","qucs_s");
  settings.setValue("ComponentDialog/geometry", saveGeometry());

  slotApplyButton();
  done(QDialog::Accepted);
}

// -------------------------------------------------------------------------
// Is called if the "Cancel"-button is pressed.
// void ComponentDialog::slotCancelButton()
// {
//   qDebug() << "slotCancel";

//   // if (changed) 
//   //   done(1); // changed could have been done before
//   // else 
//   //   done(0); // (by "Apply"-button)
// }

//-----------------------------------------------------------------
// To get really all close events (even <Escape> key).
// void ComponentDialog::reject()
// {
//   slotCancelButton();
// }

// -------------------------------------------------------------------------
// Is called, if the "Apply"-button is pressed.
void ComponentDialog::slotApplyButton()
{
    qDebug() << "slotApply";

  // Check the new name is not empty and not a duplicate.
  if (editComponent.Name.isEmpty() ||
      document->getComponentByName(editComponent.Name) != nullptr)
  {
    // Reset to the current name if a conflict is found.
    editComponent.Name = originalComponent->Name;
    mpNameLineEdit->setText(editComponent.Name);
  }

  for (int row = 0; row < propertyTable->rowCount(); row++)
  {
    qDebug() << "Item type: " << propertyTable->item(row, 1)->type();

    /*
    if (propertyTable->item(row, 1) && propertyTable->cellWidget(row, 1))
    {
      QComboBox* cellCombo = static_cast<QComboBox*>(propertyTable->cellWidget(row, 1));
      qDebug() << "Current combo text is: " << cellCombo->currentText();
    }
    */
      
  }


  for (auto& property : editComponent.Props)
  {
    qDebug() << "Edit component property: " << property->Name << " : " << property->Value;
  }

  originalComponent->Name = editComponent.Name;
  originalComponent->showName = editComponent.showName;
  changed = true;

  /*! Walk the original originalComponent->Props and compare with the
   *  data in the dialog.
   *  The pointers to the combo, edits,... are set to 0.
   *  Only check if the widgets were created (pointers checks are 'true')
   */
  //auto pp = originalComponent->Props.begin();
  // apply all the new property values
  QString tmp;
  Component *pc;

  int idxStart = 1;
  if (originalComponent->Model == ".SW") {
    idxStart = 3;
  }

  /*
  if(comboSim != nullptr) {
    auto pp = originalComponent->getProperty("Sim");
    bool display = checkSim->isChecked();
    QString value = comboSim->currentText();
    updateProperty(pp,value,display);
  }
  */

  /*
  if(comboType != nullptr) {
    bool display = checkType->isChecked();
    auto pp = originalComponent->getProperty("Type");
    switch(comboType->currentIndex()) {
      case 1:  tmp = "log";   break;
      case 2:  tmp = "list";  break;
      case 3:  tmp = "const"; break;
      default: tmp = "lin";   break;
    }
    updateProperty(pp,tmp,display);
  }

  if(checkParam != nullptr) {
    if(checkParam->isEnabled()) {
      auto pp = originalComponent->getProperty("Param");
      bool display = checkParam->isChecked();
      QString value = editParam->text();
      updateProperty(pp,value,display);
    }
  }
  if(editStart != nullptr) {
    if(comboType->currentIndex() < 2 ) {
      Property *pp = originalComponent->Props.at(idxStart); // Start
      bool display = checkStart->isChecked();
      QString value = editStart->text();
      updateProperty(pp,value,display);
      pp->Name = "Start";

      pp = originalComponent->Props.at(idxStart+1); // Stop
      display = checkStop->isChecked();
      value = editStop->text();
      updateProperty(pp,value,display);
      pp->Name = "Stop";

      pp = originalComponent->Props.at(idxStart+2);
      if (pp != nullptr) { // Points/Values
        display = checkNumber->isChecked();
        value = editNumber->text();
        updateProperty(pp,value,display);
        if (changed) pp->Name = "Points";
      }
      qDebug() << "====> before ad";
    } else {
      // If a value list is used, the properties "Start" and "Stop" are not
      // used. -> Call them "Symbol" to omit them in the netlist.
      Property *pp = originalComponent->Props.at(idxStart);
      pp->Name = "Symbol";
      pp->display = false;
      pp = originalComponent->Props.at(idxStart+1);
      pp->Name = "Symbol";
      pp->display = false;

      pp = originalComponent->Props.at(idxStart+2);
      bool display = checkValues->isChecked();
      QString value = "["+editValues->text()+"]";

      if(pp->display != display) {
        pp->display = display;
        changed = true;
      }
      if(pp->Value != value || pp->Name != "Values") {
        pp->Value = value;
        pp->Name  = "Values";
        changed = true;
      }
    }
  }
  */


  // pick selected row
  QTableWidgetItem *item = nullptr;

  //  make sure we have one item, take selected
  if (propertyTable->selectedItems().size() != 0) {
    item = propertyTable->selectedItems()[0];
  }

  /*! Walk the dialog list of 'propertyTable'
  if(item != nullptr) {
    int row = item->row();
    QString name  = propertyTable->item(row, 0)->text();
    QString value = propertyTable->item(row, 1)->text();

           // apply edit line
    if(value != edit->text()) {
      propertyTable->item(row, 1)->setText(edit->text());
    }

    // apply property name
    if (!NameEdit->isHidden()) {
      if (name != NameEdit->text()) {
        propertyTable->item(row, 0)->setText(NameEdit->text());
      }
    }
  }
  */

  // apply all the new property values in the ListView
  if (originalComponent->isEquation) {
    recreatePropsFromTable();
  } else {
    fillPropsFromTable();
  }

  if(changed) {
    int dx, dy;
    originalComponent->textSize(dx, dy);
    if(tx_Dist != 0) {
      originalComponent->tx += tx_Dist-dx;
      tx_Dist = dx;
    }
    if(ty_Dist != 0) {
      originalComponent->ty += ty_Dist-dy;
      ty_Dist = dy;
    }

    document->recreateComponent(originalComponent);
    document->viewport()->repaint();
    if ( (int) originalComponent->Props.count() != propertyTable->rowCount()) { // If props count was changed after recreation
      Q_ASSERT(propertyTable->rowCount() >= 0);
      updateCompPropsList(); // of component we need to update properties
    }
  }

}

// -------------------------------------------------------------------------
void ComponentDialog::slotBrowseFile()
{
  // current file name from the component properties
  QString currFileName = propertyTable->item(propertyTable->currentRow(), 1)->text();
  QFileInfo currFileInfo(currFileName);
  // name of the schematic where component is instantiated (may be empty)
  QFileInfo schematicFileInfo = originalComponent->getSchematic()->getFileInfo();
  QString schematicFileName = schematicFileInfo.fileName();
  // directory to use for the file open dialog
  QString currDir;

  if (!currFileName.isEmpty()) { // a file name is already defined
    if (currFileInfo.isRelative()) { // but has no absolute path
      if (!schematicFileName.isEmpty()) // if schematic has a filename
        currDir = schematicFileInfo.absolutePath();
      else    // use the WorkDir path
        currDir = lastDir.isEmpty() ? QucsSettings.QucsWorkDir.absolutePath() : lastDir; 
    } else {  // current file name is absolute
      currDir = currFileInfo.exists() ? currFileInfo.absolutePath() : QucsSettings.QucsWorkDir.absolutePath();
    }
  } else {    // a file name is not defined
    if (!schematicFileName.isEmpty()) { // if schematic has a filename
      currDir = schematicFileInfo.absolutePath();
    } else {  // use the WorkDir path
      currDir = lastDir.isEmpty() ? QucsSettings.QucsWorkDir.absolutePath() : lastDir; 
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
    lastDir = file.absolutePath();
    if (!schematicFileName.isEmpty()) {
      currDir = schematicFileInfo.canonicalPath();
    }

    if (!(schematicFileName.isEmpty() &&
          QucsMain->ProjName.isEmpty())) {
      // unsaved schematic outside project; only absolute file name
      // the schematic could be saved elsewhere and working directory may be changed
      if ( file.canonicalFilePath().startsWith(currDir) ) {
        s = QDir(currDir).relativeFilePath(s);
      } else if(QucsSettings.QucsWorkDir.exists(file.fileName()) &&
                 QucsSettings.QucsWorkDir.absolutePath() == file.absolutePath()) {
        s = file.fileName();
      }
    }
    edit->setText(s);
    int row = propertyTable->currentRow();
    propertyTable->item(row,1)->setText(s);
  }
}

// -------------------------------------------------------------------------
void ComponentDialog::slotEditFile()
{
  document->App->editFile(misc::properAbsFileName(edit->text(), document));
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
  for(int row=0; row < propertyTable->rowCount(); row++) {
    QString name  = propertyTable->item(row, 0)->text();
    if( name == NameEdit->text()) {
      propertyTable->setCurrentItem(propertyTable->item(row,0));
      slotSelectProperty(propertyTable->item(row,0));
      return;
    }
  }

  // toggle display flag
  QString s = tr("no");
  if(disp->isChecked())
    s = tr("yes");

  // get number for selected row
  int curRow = propertyTable->currentRow();

  // insert new row under current
  int insRow = curRow+1;
  propertyTable->insertRow(insRow);

  // append new row
  QTableWidgetItem *cell;
  cell = new QTableWidgetItem(NameEdit->text());
  cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
  propertyTable->setItem(insRow, 0, cell);
  cell = new QTableWidgetItem(edit->text());
  cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
  propertyTable->setItem(insRow, 1, cell);
  cell = new QTableWidgetItem(s);
  cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
  propertyTable->setItem(insRow, 2, cell);
  // no description? add empty cell
  cell = new QTableWidgetItem("");
  cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
  propertyTable->setItem(insRow, 3, cell);

  // select new row
  propertyTable->selectRow(insRow);
}

/*!
 Is called if the remove button is pressed. This is only possible for
 some properties.
 If desc is empty, ButtRem is enabled, this slot handles if it is clicked.
 Used with: Equations, ?
*/
void ComponentDialog::slotButtRem()
{
  if ((propertyTable->rowCount() < 3)&&
          (originalComponent->Model=="Eqn"||originalComponent->Model=="NutmegEq"))
     return;  // the last property cannot be removed
  if (propertyTable->rowCount() < 2)
     return;  // the last property cannot be removed


  QTableWidgetItem *item = propertyTable->selectedItems()[0];
  int row = item->row();

  if(item == 0)
    return;

  // peek next, delete current, set next current
  if ( row < propertyTable->rowCount()) {
      propertyTable->setCurrentItem(propertyTable->item(row-1,0)); // Shift selection up
      slotSelectProperty(propertyTable->item(row-1,0));
      propertyTable->removeRow(row);
      if (!propertyTable->selectedItems().size()) { // The first item was removed
          propertyTable->setCurrentItem(propertyTable->item(0,0)); // Select the first item
          slotSelectProperty(propertyTable->item(0,0));
      }
  }
}

/*!
 * \brief ComponentDialog::slotButtUp
 * Move a table item up. Enabled for Equation component.
 */
void ComponentDialog::slotButtUp()
{
  qDebug() << "slotButtUp" << propertyTable->currentRow() << propertyTable->rowCount();

  int curRow = propertyTable->currentRow();
  if (curRow == 0)
    return;
  if ((curRow == 1)&&(originalComponent->Model=="NutmegEq"))
    return;

  // swap current and row above it
  QTableWidgetItem *source = propertyTable->takeItem(curRow  ,0);
  QTableWidgetItem *target = propertyTable->takeItem(curRow-1,0);
  propertyTable->setItem(curRow-1, 0, source);
  propertyTable->setItem(curRow, 0, target);
  source = propertyTable->takeItem(curRow  ,1);
  target = propertyTable->takeItem(curRow-1,1);
  propertyTable->setItem(curRow-1, 1, source);
  propertyTable->setItem(curRow, 1, target);


  // select moved row
  propertyTable->selectRow(curRow-1);
}

/*!
 * \brief ComponentDialog::slotButtDown
 * Move a table item down. Enabled for Equation component.
 */
void ComponentDialog::slotButtDown()
{
  qDebug() << "slotButtDown" << propertyTable->currentRow() << propertyTable->rowCount();

  int curRow = propertyTable->currentRow();
  // Leave Export as last
  if ((curRow == propertyTable->rowCount()-2)&&(originalComponent->Model=="Eqn"))
    return;
  if ((curRow == 0)&&(originalComponent->Model=="NutmegEq")) // Don't let to shift the first property "Simulation="
    return;
  if ((curRow ==  propertyTable->rowCount()-1)) // Last property
    return;

  // swap current and row below it
  QTableWidgetItem *source = propertyTable->takeItem(curRow,0);
  QTableWidgetItem *target = propertyTable->takeItem(curRow+1,0);
  propertyTable->setItem(curRow+1, 0, source);
  propertyTable->setItem(curRow, 0, target);
  source = propertyTable->takeItem(curRow,1);
  target = propertyTable->takeItem(curRow+1,1);
  propertyTable->setItem(curRow+1, 1, source);
  propertyTable->setItem(curRow, 1, target);

  // select moved row
  propertyTable->selectRow(curRow+1);
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
    /*
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
    */

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
    /*
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
    */
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
    x = (editNumber->text().toDouble() - 1) / log10(fabs(x / y));
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
  editNumber->setText(QString::number(round(x + 1.0), 'g', 16));
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
  slotOKButton();
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
  slotOKButton();
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
  for (int row = 0; row < propertyTable->rowCount(); row++) {
    cell = propertyTable->item(row, 2);
    cell->setText(s);
  }
  setAllVisible = not setAllVisible; // toggle visibility for the next double-click
}

// -------------------------------------------------------------------------
// Get a list of simulations in the schematic.
QStringList ComponentDialog::getSimulationList()
{
    QStringList sim_lst;
    Schematic *sch = originalComponent->getSchematic();
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


void ComponentDialog::slotFillFromSpice()
{
  fillFromSpiceDialog *dlg = new fillFromSpiceDialog(originalComponent, this);
  auto r = dlg->exec();
  if (r == QDialog::Accepted) {
    updateCompPropsList();
  }
  delete dlg;
}

// Updates a component's properties from the contents of the propertyTable.
void ComponentDialog::fillPropsFromTable()
{
  /*
  for( int row = 0; row < propertyTable->rowCount(); row++ ) {
    QString name  = propertyTable->item(row, 0)->text();
    QString value = propertyTable->item(row, 1)->text();
    QString disp = propertyTable->item(row, 2)->text();
    QString desc = propertyTable->item(row, 3)->text();
    bool display = (disp == tr("yes"));
    if (originalComponent->isSimulation) {
      // Get property by name
      auto pp = originalComponent->getProperty(name);
      if (pp != nullptr) {
        updateProperty(pp,value,display);
      }
    } else {
      // Other components may have properties with duplicate names
      if (row < originalComponent->Props.count()) {
        auto pp = originalComponent->Props[row];
        if (pp->Name == name) {
          updateProperty(pp,value,display);
        }
      }
    }
  }
  */
}

// Used by equations to recreate the property list from the table.
// This is because equations have a variable number of properties
// and the order of those properties can change.
void ComponentDialog::recreatePropsFromTable()
{
  if (!originalComponent->isEquation) return; // add / remove properties allowed only for equations

  if (originalComponent->Props.size() != propertyTable->rowCount()) {
    changed = true; // Added or removed properties
  } else {
    for( int row = 0; row < propertyTable->rowCount(); row++ ) {
      QString name  = propertyTable->item(row, 0)->text();
      QString value = propertyTable->item(row, 1)->text();
      QString disp = propertyTable->item(row, 2)->text();
      bool display = (disp == tr("yes"));
      auto pp = originalComponent->Props.at(row);
      if ((pp->Name != name) || (pp->Value != value) ||
          (pp->display != display)) {
        changed = true; // reordered or edited properties
        break;
      }
    }
  }

  if (!changed) {
    return;
  }

  for (auto pp: originalComponent->Props) {
    delete pp;
    pp = nullptr;
  }
  originalComponent->Props.clear();
  for( int row = 0; row < propertyTable->rowCount(); row++ ) {
    QString name  = propertyTable->item(row, 0)->text();
    QString value = propertyTable->item(row, 1)->text();
    QString disp = propertyTable->item(row, 2)->text();
    QString desc = propertyTable->item(row, 3)->text();
    bool display = (disp == tr("yes"));

    Property *pp = new Property;
    pp->Name = name;
    pp->Value = value;
    pp->display = display;
    pp->Description = desc;
    originalComponent->Props.append(pp);
  }
}


bool ComponentDialog::propChanged(Property *pp, const QString &value, const bool display)
{
  if (pp->Value != value) return true;
  if (pp->display != display) return true;
  return false;
}

void ComponentDialog::updateProperty(Property *pp, const QString &value, const bool display)
{
  if (pp == nullptr) {
    qDebug()<<__func__<<" Warning! Trying to update NULLPTR property";
    return;
  }
  if (propChanged(pp,value,display)) {
    pp->Value = value;
    pp->display = display;
    changed = true;
  }
}
