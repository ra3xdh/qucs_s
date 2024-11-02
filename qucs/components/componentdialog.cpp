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
  QStringList list, options;

  if (start != -1 && end != -1)
  {
    list = description.mid(start + 1, end - start - 1).split(',');
    for(auto entry : list)
      options << entry.trimmed();
  }

  return options;
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

    bool check()
    {
      return (mCheckBox->checkState() == Qt::Checked);
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

  qDebug() << "Processing component: " << component->Model << " name: " << component->Name;

  // Make a deep copy of component for edit editing (will be discarded if user cancels).
  editComponent = *component;
  editComponent.Props = component->Props;
  changed = false;

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

  // Try to work out what kind of component this is.
  isEquation = QStringList({"Eqn", "NutmegEq"}).contains(originalComponent->Model);
  hasSweep = QStringList({".AC", ".NOISE", ".SW", ".SP", ".TR"}).contains(originalComponent->Model);
  sweepProperties = QStringList({"Sim", "Param", "Type", "Values", "Start", "Stop", "Points"});

  if (isEquation)
  {
    qDebug() << "This is an equation";

    // Create the equation editor.
    QGroupBox *editorGroup = new QGroupBox(tr("Equation Editor"));
    static_cast<QVBoxLayout*>(layout())->addWidget(editorGroup, 2);
    QVBoxLayout *editorLayout = new QVBoxLayout(editorGroup);

    eqnSimCombo = new QComboBox();
    eqnSimCombo->addItems(getSimulationList());
    editorLayout->addWidget(eqnSimCombo, 2);
    
    QFont font("Courier", 10);   
    eqnEditor = new QTextEdit();
    eqnEditor->setFont(font);
    editorLayout->addWidget(eqnEditor, 2);

    updateEqnEditor();
  }

  else 
  {
    if (hasSweep)
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
      sweepParamWidget["Sim"] = new ParamCombo("Sim", sweepPageLayout, this, &ComponentDialog::updateSweepProperty);
      sweepParamWidget["Param"] = new ParamLineEdit("Param", sweepPageLayout, this, &ComponentDialog::updateSweepProperty);
      sweepParamWidget["Type"] = new ParamCombo("Type", sweepPageLayout, this, &ComponentDialog::updateSweepProperty);
      sweepParamWidget["Values"] = new ParamLineEdit("Values", sweepPageLayout, this, &ComponentDialog::updateSweepProperty);
      sweepParamWidget["Start"] = new ParamLineEdit("Start", sweepPageLayout, this, &ComponentDialog::updateSweepProperty);
      sweepParamWidget["Stop"] = new ParamLineEdit("Stop", sweepPageLayout, this, &ComponentDialog::updateSweepProperty);
      sweepParamWidget["Points"] = new ParamLineEdit("Points", sweepPageLayout, this, &ComponentDialog::updateSweepProperty);

      // Setup the widget specialisations for each simulation type.    
      sweepTypeEnabledParams["lin"] = QStringList{"Sim", "Param", "Type", "Start", "Stop", "Step", "Points"};    
      sweepTypeEnabledParams["log"] = QStringList{"Sim", "Param", "Type", "Start", "Stop", "Step", "Points"};
      sweepTypeEnabledParams["list"] = QStringList{"Type", "Values"};
      sweepTypeEnabledParams["value"] = QStringList{"Type", "Values"};
      sweepTypeSpecialLabels["log"] = {{"Step:", "Points per decade"}};
      paramsHiddenBySim["Sim"] = QStringList{".AC", ".SP"};
      paramsHiddenBySim["Param"] = QStringList{".AC", ".SP"};

      // Setup the widgets as per the stored type.
      sweepParamWidget["Type"]->setOptions({"lin", "log", "list", "value"});
      updateSweepProperty("All", "");

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
    propertyTableLayout->addWidget(propertyTable, 2);

    updatePropertyTable();

    // Try to move the cursor to the editable cell if any cell is clicked.
    connect(propertyTable, &QTableWidget::cellClicked, 
                [=](int row, int column) { (void)column; propertyTable->setCurrentCell(row, 1); } );
  }

  // Add the dialog button widgets.
  QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | 
                                      QDialogButtonBox::Apply | QDialogButtonBox::Cancel);

  connect(buttonBox, &QDialogButtonBox::accepted, this, &ComponentDialog::slotOKButton);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &ComponentDialog::slotApplyButton);
  mainLayout->addWidget(buttonBox);
}

ComponentDialog::~ComponentDialog()
{
  delete Validator;
  delete Validator2;
  delete ValRestrict;
  delete ValInteger;
}

// -------------------------------------------------------------------------
// Updates all the widgets on the sweep page according to the sweep type.
void ComponentDialog::updateSweepWidgets(const QString& type)
{
  qDebug() << "Update sim widgets " << type;

  for (auto it = sweepParamWidget.keyValueBegin(); it != sweepParamWidget.keyValueEnd(); ++it) 
  {
    qDebug() << "Checking " << it->first;
    it->second->setHidden(paramsHiddenBySim.contains(it->first) &&
                            paramsHiddenBySim[it->first].contains(editComponent.Model));
    it->second->setEnabled(sweepTypeEnabledParams.contains(type) && 
                            sweepTypeEnabledParams[type].contains(it->first));
  }
}

// -------------------------------------------------------------------------
// Updates all the sweep params on the sweep page according the component value.
void ComponentDialog::updateSweepProperty(const QString& property, const QString& value)
{
  qDebug() << "Update sweep property " << property << " value: " << value;

  // Type has changed so update the widgets first.
  if (property == "Type")
    updateSweepWidgets(sweepParamWidget["Type"]->value());

  // Repopulate the property fields.
  if (property == "All")
  {
    for (auto property : editComponent.Props)
    {
      qDebug() << "Updating " << property->Name;
      if (sweepParamWidget.contains(property->Name))
      {
        qDebug() << "Setting to value " << property->Value;        
        sweepParamWidget[property->Name]->setValue(property->Value);
        sweepParamWidget[property->Name]->setCheck(property->display);
      }        
    }
  }
}

// -------------------------------------------------------------------------
// Updates the equation textedit with the currently stored value.
void ComponentDialog::updatePropertyTable()
{
    // Add component properties to the properties table with the exception of the sweep properties.
    int row = 0;
    for (Property* property : editComponent.Props)
    {
      qDebug() << " Loading originalComponent->Props :" << property->Name << property->Value << property->display << property->Description;
      // Check this is a sweep property.
      if (hasSweep && sweepProperties.contains(property->Name))
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
        optionsCombo->setCurrentText(property->Value);
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
    qDebug() << row << " rows added to propertyTable";
}

// -------------------------------------------------------------------------
// Updates the equation textedit with the currently stored value.
void ComponentDialog::updateEqnEditor()
{
  QString eqnList;

  for (auto property : editComponent.Props)
  {
    if (property->Name == "Simulation")
    {
      eqnSimCombo->setCurrentText(property->Value);
    }
    else
      eqnList.append(property->Name + " = " + property->Value + "\n");
  }

  qDebug() << "Equation list is: " << eqnList;
  eqnEditor->setPlainText(eqnList);
}

// -------------------------------------------------------------------------
// Clears the current equation component and writes the context of dialog.
void ComponentDialog::writeEquation()
{
  qDebug() << "writeEquation";
  originalComponent->Props.clear();
  
  // Property(const QString& _Name="", const QString& _Value="", bool _display=false, const QString& Desc="")
  qDebug() << "Writing simulation: " << eqnSimCombo->currentText();
  originalComponent->Props.append(new Property("Simulation", eqnSimCombo->currentText(), true));

  QString text = eqnEditor->document()->toPlainText();
  QStringList lines = text.split('\n', Qt::SkipEmptyParts);
  
  for (const QString& line : lines)
  {
    QStringList parts = line.split('=');

    Q_ASSERT(parts.count() == 2);

    qDebug() << parts[0] << " : " << parts[1];
    originalComponent->Props.append(new Property(parts[0].trimmed(), parts[1].trimmed(), true));
  }
}

// -------------------------------------------------------------------------
// Applies all changes and closes the dialog.
void ComponentDialog::slotOKButton()
{
  qDebug() << "slotOK";

  QSettings settings("qucs","qucs_s");
  settings.setValue("ComponentDialog/geometry", saveGeometry());

  slotApplyButton();
  done(QDialog::Accepted);
}

// -------------------------------------------------------------------------
// Applies all changes and updates the fields if they have changed as the
// result of the applied changes.
void ComponentDialog::slotApplyButton()
{
  qDebug() << "slotApplyButton";

  // Check the new component name is not empty and not a duplicate.
  if (editComponent.Name.isEmpty() ||
      document->getComponentByName(editComponent.Name) != nullptr)
  {
    // Reset to the current name if a conflict is found.
    editComponent.Name = originalComponent->Name;
    mpNameLineEdit->setText(editComponent.Name);
  }

  if (isEquation)
  {
    qDebug() << "Writing equation";
    writeEquation();
  }

  else
  {
    // Walk through the Props list and update component.
    int row = 0;
    for (Property* property : originalComponent->Props)
    {
      qDebug() << "Found property " << property->Name;

      if (hasSweep && sweepParamWidget.contains(property->Name))
      {
        qDebug() << "Setting sweepParamWidget " << property->Name << " to " << sweepParamWidget[property->Name]->value() 
                                            << " and checked to " << sweepParamWidget[property->Name]->check();
        property->Value = sweepParamWidget[property->Name]->value();
        property->display = sweepParamWidget[property->Name]->check();
      }

      else 
      {
        QTableWidgetItem* item = propertyTable->item(row, 1);
        Q_ASSERT(item);

        if (item->type() == ComboBoxCell)
        {
          QComboBox* cellCombo = static_cast<QComboBox*>(propertyTable->cellWidget(row, 1));
          qDebug() << "Setting property #" << row << " to " << cellCombo->currentText();
          property->Value = cellCombo->currentText();
        }

        else
        {
          qDebug() << "Setting property #" << row << " to " << propertyTable->item(row, 1)->text();
          property->Value = propertyTable->item(row, 1)->text();
        }

        property->display = (propertyTable->item(row, 2)->checkState() == Qt::Checked);
        row++;
      }
    }
  }

  document->recreateComponent(originalComponent);
  document->viewport()->repaint();

  // TODO: Why is the text being repositioned?
  // If this is needed, make it a function because something similar is called elsewhere.
  if (changed) 
  {
    int dx, dy;
    originalComponent->textSize(dx, dy);
    if(tx_Dist != 0) 
    {
      originalComponent->tx += tx_Dist-dx;
      tx_Dist = dx;
    }
    if(ty_Dist != 0) 
    {
      originalComponent->ty += ty_Dist-dy;
      ty_Dist = dy;
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
    // edit->setText(s);
    int row = propertyTable->currentRow();
    propertyTable->item(row,1)->setText(s);
  }
}

// -------------------------------------------------------------------------
void ComponentDialog::slotEditFile()
{
  // document->App->editFile(misc::properAbsFileName(edit->text(), document));
}

// -------------------------------------------------------------------------
/* TODO: Need to implement mechanism to update POINTS when START, STOP, STEP is changed
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
*/

// -------------------------------------------------------------------------
/* TODO: Need to implement mechanism to update STEP when POINTS is changed
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
*/

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

// -------------------------------------------------------------------------
// 
void ComponentDialog::slotFillFromSpice()
{
  fillFromSpiceDialog *dlg = new fillFromSpiceDialog(originalComponent, this);
  auto r = dlg->exec();
  if (r == QDialog::Accepted) {
    // updateCompPropsList();
  }
  delete dlg;
}
