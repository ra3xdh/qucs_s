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

/*
  TODO:
  1. Auto update sweep step / sweep points for log sweeps
  2. Translated text?
  3. Add special property names - i.e., for log sweeps (per decade instead of step)
  4. Update components from SPICE file.
  5. Implement highlighting.
  6. Have "Export" as a check box, or option list for equations.
*/

#include "componentdialog.h"
#include "main.h"
#include "schematic.h"
#include "settings.h"
#include "misc.h"
#include "fillfromspicedialog.h"

#include <cmath>

#define L(TEXT) QStringLiteral(TEXT) 

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
// Helper to convert a number to a string with appropriate SI code.
double str2num(const QString& string)
{
  QString unit;
  double number;
  double factor;
  misc::str2num(string, number, unit, factor);
  return number * factor;
}

// -------------------------------------------------------------------------
// Convenience base class to add a label, and checkbox.
class ParamWidget
{
  public:
    ParamWidget(const QString& label, bool displayCheck, QGridLayout* layout)
    : mParam(label), mHasCheck(displayCheck)
    {
      int row = layout->rowCount();

      mLabel = new QLabel(label + ":");
      layout->addWidget(mLabel, row, 0);

      mCheckBox = new QCheckBox("display in schematic");
      // mCheckBox->setEnabled(false);
      layout->addWidget(mCheckBox, row, 2);  
    }

    void setLabel(const QString& label)
    {
      mLabel->setText(label);
    }

    void setCheck(bool checked)
    {
      mCheckBox->setCheckState((mHasCheck && checked) ? Qt::Checked : Qt::Unchecked);
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
      mCheckBox->setVisible(mHasCheck && !hidden);
    }

    virtual void setValue(const QString& value) = 0;
    virtual void setOptions(const QStringList& options) = 0;
    virtual QString value() = 0;

  protected:
    QString mParam;

  private:
    QLabel* mLabel;
    QCheckBox* mCheckBox;
    bool mHasCheck;
};

// -------------------------------------------------------------------------
// Convenience class to display label, edit, and checkbox.
class ParamLineEdit : public QLineEdit, public ParamWidget
{
  public:
    ParamLineEdit(const QString& param, QValidator* validator, bool displayCheck, QGridLayout* layout, ComponentDialog* dialog, 
                  void (ComponentDialog::* func)(const QString&, const QString&) = nullptr)
    : ParamWidget(param, displayCheck, layout)
    {
      layout->addWidget(this, layout->rowCount() - 1, 1);
      setValidator(validator);
      // Note: need to be careful about life of dialog and param here.
      if (func)
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
    ParamCombo(const QString& param, bool displayCheck, QGridLayout* layout, ComponentDialog* dialog, 
                void (ComponentDialog::* func)(const QString&, const QString&))
    : ParamWidget(param, displayCheck, layout)
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

// -------------------------------------------------------------------------
// Sets up the syntax highlighter for the equation editor.
EqnHighlighter::EqnHighlighter(const QString& keywordSet, QTextDocument* parent)
: QSyntaxHighlighter(parent)
{
  HighlightingRule rule;

  keywordFormat.setForeground(Qt::darkBlue);
  keywordFormat.setFontWeight(QFont::Bold);
  if (keywordSet == "ngspice") 
  {
    // Table from page 367 of the ngspice manual.
    const QString keywordPatterns[] {
        L("\\bmag\\b"), L("\\bph\\b"), L("\\bcph\\b"), L("\\bunwrap\\b"), L("\\bj\\b"),
        L("\\breal\\b"), L("\\bimag\\b"), L("\\bconj\\b"), L("\\bdb\\b"), L("\\blog10\\b"), L("\\bln\\b"),
        L("\\bexp\\b"), L("\\babs\\b"), L("\\bsqrt\\b"), L("\\bsin\\b"), L("\\bcos\\b"), L("\\btan\\b"),
        L("\\batan\\b"), L("\\bsinh\\b"), L("\\bcosh\\b"), L("\\btanh\\b"), L("\\batanh\\b"), L("\\bfloor\\b"),
        L("\\bceil\\b"), L("\\bnorm\\b"), L("\\bmean\\b"), L("\\bavg\\b"), L("\\bstddev\\b"), L("\\bgroup_delay\\b"),
        L("\\bvector\\b"), L("\\bcvector\\b"), L("\\bunitvec\\b"), L("\\blength\\b"), L("\\binteg\\b"), L("\\bderiv\\b"),
        L("\\bvecd\\b"), L("\\bminimum\\b"), L("\\bvecmax\\b"), L("\\bmaximum\\b"), L("\\bfft\\b"), L("\\bifft\\b"),
        L("\\bsortorder\\b"), L("\\btimer\\b"), L("\\bclock\\b")
      };
    for (const QString &pattern : keywordPatterns)
    {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }
  }

  quotationFormat.setForeground(Qt::darkGreen);
  rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
  rule.format = quotationFormat;
  highlightingRules.append(rule);

  functionFormat.setFontWeight(QFont::Bold);
  functionFormat.setForeground(Qt::darkMagenta);
  rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()"));
  rule.format = functionFormat;
  highlightingRules.append(rule);
}

void EqnHighlighter::highlightBlock(const QString& text)
{
  for (const HighlightingRule &rule : std::as_const(highlightingRules))
  {
    QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
    
    while (matchIterator.hasNext())
    {
        QRegularExpressionMatch match = matchIterator.next();
        setFormat(match.capturedStart(), match.capturedLength(), rule.format);
    }
  }
}

ComponentDialog::ComponentDialog(Component* schematicComponent, Schematic* schematic)
			: QDialog(schematic)
{
  component = schematicComponent;
  document = schematic;

  restoreGeometry(_settings::Get().item<QByteArray>("ComponentDialog/geometry"));
  setWindowTitle(tr("Edit Component Properties") + " - " + component->Description.toUpper());

  // Setup dialog layout.
  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  QGridLayout* propertiesPageLayout;
  
  // Setup validators.
  // TODO: These don't look right and don't seem to restrict the edit boxes in the way they should?
  intVal = new QIntValidator(1, 1000000, this);
  compNameVal = new QRegularExpressionValidator(QRegularExpression("[A-Za-z][A-Za-z0-9_]+"),this);
  nameVal = new QRegularExpressionValidator(QRegularExpression("[\\w_.,\\(\\) @:\\[\\]]+"), this);
  paramVal = new QRegularExpressionValidator(QRegularExpression("[^\"=]*"), this);

  // Add the component name.
  QGridLayout* nameLayout = new QGridLayout;
  mainLayout->addLayout(nameLayout);
  componentNameWidget = new ParamLineEdit("Name", compNameVal, true, nameLayout, this, nullptr);
  componentNameWidget->setValue(component->Name);
  componentNameWidget->setCheck(component->showName);

  // Try to work out what kind of component this is.
  isEquation = QStringList({"Eqn", "NutmegEq"}).contains(component->Model);
  hasSweep = QStringList({".AC", ".NOISE", ".SW", ".SP", ".TR"}).contains(component->Model);
  sweepProperties = QStringList({"Sim", "Param", "Type", "Values", "Start", "Stop", "Points"});
  hasFile = component->Props.count() > 0 && component->Props.at(0)->Name == "File";

  // Setup the dialog according to the component kind.
  if (isEquation)
  {
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
    new EqnHighlighter("ngspice", eqnEditor->document());
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
      // void (ComponentDialog::* func)(const QString&, const QString&) = &ComponentDialog::updateSweepProperty;
      sweepParamWidget["Sim"] = new ParamCombo("Sim", true, sweepPageLayout, this, &ComponentDialog::updateSweepProperty);
      sweepParamWidget["Param"] = new ParamLineEdit("Param", compNameVal, true, sweepPageLayout, this, &ComponentDialog::updateSweepProperty);
      sweepParamWidget["Type"] = new ParamCombo("Type", true, sweepPageLayout, this, &ComponentDialog::updateSweepProperty);
      sweepParamWidget["Values"] = new ParamLineEdit("Values", paramVal, true, sweepPageLayout, this, &ComponentDialog::updateSweepProperty);
      sweepParamWidget["Start"] = new ParamLineEdit("Start", paramVal, true, sweepPageLayout, this, &ComponentDialog::updateSweepProperty);
      sweepParamWidget["Stop"] = new ParamLineEdit("Stop", paramVal, true, sweepPageLayout, this, &ComponentDialog::updateSweepProperty);
      sweepParamWidget["Step"] = new ParamLineEdit("Step", paramVal, false, sweepPageLayout, this, &ComponentDialog::updateSweepProperty);
      sweepParamWidget["Points"] = new ParamLineEdit("Points", intVal, true, sweepPageLayout, this, &ComponentDialog::updateSweepProperty);

      // Setup the widget specialisations for each simulation type.    
      sweepTypeEnabledParams["lin"] = QStringList{"Sim", "Param", "Type", "Start", "Stop", "Step", "Points"};    
      sweepTypeEnabledParams["log"] = QStringList{"Sim", "Param", "Type", "Start", "Stop", "Step", "Points"};
      sweepTypeEnabledParams["list"] = QStringList{"Type", "Values"};
      sweepTypeEnabledParams["value"] = QStringList{"Type", "Values"};
      sweepTypeSpecialLabels["log"] = {{"Step:", "Points per decade"}};
      paramsHiddenBySim["Sim"] = QStringList{".AC", ".SP", ".TR"};
      paramsHiddenBySim["Param"] = QStringList{".AC", ".SP", ".TR"};

      // Setup the widgets as per the stored type.
      sweepParamWidget["Sim"]->setOptions(getSimulationList());
      sweepParamWidget["Type"]->setOptions({"lin", "log", "list", "value"});
      updateSweepProperty("All", "");

      // Create the properties page and add it to the tab widget.
      QWidget* propertiesPage = new QWidget(pageTabs);
      pageTabs->addTab(propertiesPage, tr("Properties"));
      propertiesPageLayout = new QGridLayout(propertiesPage);
    }
    
    // This component does not have sweep settings, so add properties directly to the dialog itself.
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

    // Add the file control buttons if applicable.
    if (hasFile)
    {
      QHBoxLayout* fileButtonLayout = new QHBoxLayout;
      mainLayout->addLayout(fileButtonLayout);
      QPushButton* fileBrowseButton = new QPushButton("&Open File", this);
      QPushButton* fileEditButton = new QPushButton("&Edit File", this);
      fileButtonLayout->addWidget(fileBrowseButton);
      connect(fileBrowseButton, &QPushButton::released, this, &ComponentDialog::slotBrowseFile);
      fileButtonLayout->addWidget(fileEditButton);
      connect(fileEditButton, &QPushButton::released, this, &ComponentDialog::slotEditFile);
    }
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
  delete compNameVal;
  delete intVal;
  delete nameVal;
  delete paramVal;
}

// -------------------------------------------------------------------------
// Updates all the widgets on the sweep page according to the sweep type.
void ComponentDialog::updateSweepWidgets(const QString& type)
{
  for (auto it = sweepParamWidget.keyValueBegin(); it != sweepParamWidget.keyValueEnd(); ++it) 
  {
    it->second->setHidden(paramsHiddenBySim.contains(it->first) &&
                            paramsHiddenBySim[it->first].contains(component->Model));
    it->second->setEnabled(sweepTypeEnabledParams.contains(type) && 
                            sweepTypeEnabledParams[type].contains(it->first));
  }
}

// -------------------------------------------------------------------------
// Updates all the sweep params on the sweep page according the component value.
void ComponentDialog::updateSweepProperty(const QString& property, const QString& value)
{
  qDebug() << "updateSweepProperty " << property << " = " << value;

  // Type has changed so update the widget presentation.
  if (property == "Type")
    updateSweepWidgets(sweepParamWidget["Type"]->value());

  if (property == "All")
  {
    for (auto property : component->Props)
    {
      if (sweepParamWidget.contains(property->Name))
      {
        sweepParamWidget[property->Name]->setValue(property->Value);
        sweepParamWidget[property->Name]->setCheck(property->display);
      }        
    }
  }

  if (property == "Values")
  {
    // Do nothing.
  }

  // Specialisations for updating start, stop, step, and points values.
  else 
  {
    double start = str2num(sweepParamWidget["Start"]->value());
    double stop = str2num(sweepParamWidget["Stop"]->value());

    if (property == "Start" || property == "Stop" || property == "Points" || property == "All")
    {
      double points = str2num(sweepParamWidget["Points"]->value());
      double step = (stop - start) / (points - 1.0);
      sweepParamWidget["Step"]->setValue(misc::num2str(step));
    }
    else if (property == "Step")
    {
      double step = str2num(sweepParamWidget["Step"]->value());
      double points = (stop - start) / step + 1;
      sweepParamWidget["Points"]->setValue(misc::num2str(points));
    }
  }
}

// -------------------------------------------------------------------------
// Updates the property table with the current values stored in the component.
void ComponentDialog::updatePropertyTable()
{
    // Add component properties to the properties table with the exception of the sweep properties.
    int row = 0;
    for (Property* property : component->Props)
    {
      // Check this is a sweep property (as there not added to property table).
      if (hasSweep && sweepProperties.contains(property->Name))
        continue;

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
        propertyTable->setItem(row, 1, new QTableWidgetItem("", TextEditCell));
        propertyTable->openPersistentEditor(propertyTable->item(row, 1));
        propertyTable->item(row, 1)->setText(property->Value);
      }    

      // Set check box and description.
      propertyTable->setItem(row, 2, new QTableWidgetItem(CheckBoxCell));
      propertyTable->item(row, 2)->setCheckState(property->display ? Qt::Checked : Qt::Unchecked);
      propertyTable->setItem(row, 3, new QTableWidgetItem(property->Description, LabelCell));
      propertyTable->item(row, 3)->setFlags(Qt::ItemIsEnabled);

      row++;
    }
}

// -------------------------------------------------------------------------
// Updates the equation textedit with the currently stored value.
void ComponentDialog::updateEqnEditor()
{
  QString eqnList;

  for (auto property : component->Props)
  {
    if (property->Name == "Simulation")
      eqnSimCombo->setCurrentText(property->Value);
    else
      eqnList.append(property->Name + " = " + property->Value + "\n");
  }

  eqnEditor->setPlainText(eqnList);
}

// -------------------------------------------------------------------------
// Clears the current equation component and writes the context of dialog.
void ComponentDialog::writeEquation()
{
  // Clear all old properties and free their memory.
  qDeleteAll(component->Props.begin(), component->Props.end());
  component->Props.clear();
  
  // Note: the description needs to be written as "Simulation name" because this is used when saving the file.
  component->Props.append(new Property("Simulation", eqnSimCombo->currentText(), true, "Simulation name"));

  QString text = eqnEditor->document()->toPlainText();
  QStringList lines = text.split('\n', Qt::SkipEmptyParts);
  
  for (const QString& line : lines)
  {
    QStringList parts = line.split('=');
    if (parts.count() == 2)
      component->Props.append(new Property(parts[0].trimmed(), parts[1].trimmed(), true));
  }
}

// -------------------------------------------------------------------------
// Applies all changes and closes the dialog.
void ComponentDialog::slotOKButton()
{
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
  // Update component name if valid.
  component->showName = componentNameWidget->check();
  QString name = componentNameWidget->value();
  if (!name.isEmpty() && !document->getComponentByName(name))
    component->Name = name;
  else
    componentNameWidget->setValue(component->Name);

  if (isEquation)
    writeEquation();

  else
  {
    // Walk through the Props list and update component.
    int row = 0;
    for (Property* property : component->Props)
    {
      if (hasSweep && sweepParamWidget.contains(property->Name))
      {
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
          property->Value = cellCombo->currentText();
        }

        else
          property->Value = propertyTable->item(row, 1)->text();

        property->display = (propertyTable->item(row, 2)->checkState() == Qt::Checked);
        row++;
      }
    }
  }

  document->recreateComponent(component);
  document->viewport()->repaint();

  // TODO: Why is the text being repositioned?
  // If this is needed, make it a function because something similar is called elsewhere.
  /*
  if (changed) 
  {
    int dx, dy;
    component->textSize(dx, dy);
    if(tx_Dist != 0) 
    {
      component->tx += tx_Dist-dx;
      tx_Dist = dx;
    }
    if(ty_Dist != 0) 
    {
      component->ty += ty_Dist-dy;
      ty_Dist = dy;
    }
  }
  */
}

// -------------------------------------------------------------------------
void ComponentDialog::slotBrowseFile()
{
  // current file name from the component properties
  QString currFileName = component->Props.at(0)->Value;
  QFileInfo currFileInfo(currFileName);
  // name of the schematic where component is instantiated (may be empty)
  QFileInfo schematicFileInfo = component->getSchematic()->getFileInfo();
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
    propertyTable->item(0, 1)->setText(s);
  }
}

// -------------------------------------------------------------------------
void ComponentDialog::slotEditFile()
{
  qDebug() << "editing file " << component->Props.at(0)->Value << " or " << propertyTable->item(0, 1)->text();
  document->App()->editFile(misc::properAbsFileName(component->Props.at(0)->Value, document));
}

// -------------------------------------------------------------------------
/* TODO: Need to implement mechanism to update POINTS when START, STOP, STEP is changed
   DONE: Implemented for linear sweeps, TODO: for log sweeps.
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
    Schematic *sch = component->getSchematic();
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
// Fill from SPICE - what does this do?
void ComponentDialog::slotFillFromSpice()
{
  fillFromSpiceDialog *dlg = new fillFromSpiceDialog(component, this);
  auto r = dlg->exec();
  if (r == QDialog::Accepted) {
    // updateCompPropsList();
  }
  delete dlg;
}
