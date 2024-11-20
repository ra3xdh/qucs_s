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
  1. DONE: Auto update sweep step / sweep points for log sweeps
  2. DONE: Translated text?
  3. DONE: Add special property names - i.e., for log sweeps (per decade instead of step)
  4. DONE: Update components from SPICE file.
  5. DONE: Implement highlighting.
  6. DONE: Have "Export" as a check box, or option list for Qucsator equations.
  7. DONE: .INCLUDE components have multiple files
  8. Should 'Lib' parameters also be able to open a file?
  9. DONE: Check for memory leaks.
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
      options << entry.trimmed(); // QString::trimmed flagged by valgrind leak check
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
// Table cell widget to hold both text and a button that allows editing
// or searching to fill in the text.
class CompoundWidget : public QWidget
{
public:
  CompoundWidget(const QString& text, ComponentDialog* dialog, void (ComponentDialog::* func)(QLineEdit*) = nullptr)
  : QWidget(dialog)
  {
    mButton = new QPushButton("...", this);
    mButton->setMinimumWidth(20);
    mButton->setMaximumWidth(20);
    mEdit = new QLineEdit(text, this);
    QLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(mEdit);
    layout->addWidget(mButton);
    setLayout(layout);

    if (func)
      connect(mButton, &QPushButton::released, [=]() { if (dialog) (dialog->*func)(mEdit); });    
  }
  ~CompoundWidget()
  {
    delete mButton;
    delete mEdit;
  }

  QString text()
  {
    return mEdit->text();
  }

private:
  QPushButton* mButton;
  QLineEdit* mEdit;
};

// -------------------------------------------------------------------------
// Convenience base class to add a label, and checkbox.
class ParamWidget
{
  public:
    ParamWidget(const QString& param, const QString& label, bool displayCheck, QGridLayout* layout)
    : mParam(param), mHasCheck(displayCheck)
    {
      int row = layout->rowCount();

      mDefaultLabel = label;
      mLabel = new QLabel(label + ":");
      layout->addWidget(mLabel, row, 0);

      mCheckBox = new QCheckBox("display in schematic");
      layout->addWidget(mCheckBox, row, 2);  
    }

    virtual ~ParamWidget() {}

    void setLabel(const QString& label)
    {
      mLabel->setText(label + ":");
    }

    QString defaultLabel()
    {
      return mDefaultLabel;
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
    QString mDefaultLabel;

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
    ParamLineEdit(const QString& param, const QString& label, QValidator* validator, bool displayCheck, QGridLayout* layout, ComponentDialog* dialog, 
                  void (ComponentDialog::* func)(const QString&) = nullptr)
    : ParamWidget(param, label, displayCheck, layout)
    {
      layout->addWidget(this, layout->rowCount() - 1, 1);
      setValidator(validator);
      
      if (func)
        connect(this, &QLineEdit::textEdited, [=]() { if (dialog) (dialog->*func)(mParam); });
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
    ParamCombo(const QString& param, const QString& label, bool displayCheck, QGridLayout* layout, ComponentDialog* dialog, 
                void (ComponentDialog::* func)(const QString&) = nullptr)
    : ParamWidget(param, label, displayCheck, layout)
    {
      layout->addWidget(this, layout->rowCount() - 1, 1);
      
      if (func)
        connect(this, &QComboBox::currentTextChanged, [=]() { if (dialog) (dialog->*func)(mParam); });
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

// -------------------------------------------------------------------------
// Sets up the syntax highlighter for the equation editor.
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

// -------------------------------------------------------------------------
// Dialog to show parameters for most components.
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
  componentNameWidget = new ParamLineEdit("Name", tr("Name"), compNameVal, true, nameLayout, this, nullptr);
  componentNameWidget->setValue(component->Name);
  componentNameWidget->setCheck(component->showName);

  // Try to work out what kind of component this is.
  isEquation = QStringList({"Eqn", "NutmegEq", "SpicePar", "SpGlobPar"}).contains(component->Model);
  hasSweep = QStringList({".AC", ".NOISE", ".SW", ".SP", ".TR"}).contains(component->Model);
  sweepProperties = QStringList({"Sim", "Type", "Param", "Start", "Stop", "Points"});
  hasFile = component->Props.count() > 0 && component->Props.at(0)->Name == "File";

  paramsHiddenBySim["Export"] = QStringList{"NutmegEq"};
  paramsHiddenBySim["Sim"] = QStringList{".AC", ".SP", ".TR", "Eqn", "SpicePar", "SpGlobPar"};
  paramsHiddenBySim["Param"] = QStringList{".AC", ".SP", ".TR"};

  // Setup the dialog according to the component kind.
  if (isEquation)
  {
    // Create the equation editor.
    QGroupBox* editorGroup = new QGroupBox(tr("Equation Editor"));
    static_cast<QVBoxLayout*>(layout())->addWidget(editorGroup, 2);
    QVBoxLayout* editorLayout = new QVBoxLayout(editorGroup);

    // Ngspice equations can be referenced to a simulation.
    if (!paramsHiddenBySim["Sim"].contains(component->Model))
    {
      eqnSimCombo = new QComboBox();
      eqnSimCombo->addItems(getSimulationList(true));
      editorLayout->addWidget(eqnSimCombo, 2);
    }
    
    QFont font("Courier", 10);   
    eqnEditor = new QTextEdit();
    eqnEditor->setFont(font);
    new EqnHighlighter("ngspice", eqnEditor->document());
    editorLayout->addWidget(eqnEditor, 2);

    // Qucsator equations can choose whether to export values.
    if (!paramsHiddenBySim["Export"].contains(component->Model))
    {
      QHBoxLayout* exportLayout = new QHBoxLayout;
      eqnExportCheck = new QCheckBox(tr("Put result in dataset"), this);
      exportLayout->addWidget(eqnExportCheck);
      exportLayout->addStretch();
      editorLayout->addLayout(exportLayout);
    }

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
      QGridLayout* sweepPageLayout = new QGridLayout(sweepPage);

      // Sweep page setup - add widgets for each possible sweep property.
      void (ComponentDialog::* func)(const QString&) = &ComponentDialog::updateSweepProperty;
      sweepParamWidget["Sim"] = new ParamCombo("Sim", tr("Simulation"), true, sweepPageLayout, this, func);
      sweepParamWidget["Param"] = new ParamLineEdit("Param", tr("Sweep Parameter"), compNameVal, true, sweepPageLayout, this, func);
      sweepParamWidget["Type"] = new ParamCombo("Type", tr("Type"), true, sweepPageLayout, this, func);
      sweepParamWidget["Values"] = new ParamLineEdit("Values", tr("Values"), paramVal, true, sweepPageLayout, this, func);
      sweepParamWidget["Start"] = new ParamLineEdit("Start", tr("Start"), paramVal, true, sweepPageLayout, this, func);
      sweepParamWidget["Stop"] = new ParamLineEdit("Stop", tr("Stop"), paramVal, true, sweepPageLayout, this, func);
      sweepParamWidget["Step"] = new ParamLineEdit("Step", tr("Step"), paramVal, false, sweepPageLayout, this, func);
      sweepParamWidget["Points"] = new ParamLineEdit("Points", tr("Number"), intVal, true, sweepPageLayout, this, func);

      // Setup the widget specialisations for each simulation type.    
      sweepTypeEnabledParams["lin"] = QStringList{"Sim", "Type", "Param", "Start", "Stop", "Step", "Points"};    
      sweepTypeEnabledParams["log"] = QStringList{"Sim", "Type", "Param", "Start", "Stop", "Step", "Points"};
      sweepTypeEnabledParams["list"] = QStringList{"Sim", "Type", "Param", "Values"};
      sweepTypeEnabledParams["value"] = QStringList{"Sim", "Type", "Param", "Values"};
      sweepTypeSpecialLabels[qMakePair(QString("log"),QString("Step"))] = {"Points per decade"};
      // sweepTypeSpecialLabels[qMakePair(QString("list"),QString("Points"))] = {"Values"};

      // Setup the widgets as per the stored type.
      sweepParamWidget["Sim"]->setOptions(getSimulationList(false));
      sweepParamWidget["Type"]->setOptions({"lin", "log", "list", "value"});
      updateSweepProperty("All");

      // Create the properties page and add it to the tab widget.
      QWidget* propertiesPage = new QWidget(pageTabs);
      pageTabs->addTab(propertiesPage, tr("Properties"));
      propertiesPageLayout = new QGridLayout(propertiesPage);
    }
    
    // This component does not have sweep settings, so add properties directly to the dialog itself.
    else 
    { 
      propertiesPageLayout = new QGridLayout;
      static_cast<QVBoxLayout*>(layout())->addLayout(propertiesPageLayout);
    }

    // Create the properties table.
    QGroupBox *propertyGroup = new QGroupBox(tr("Properties"));
    propertiesPageLayout->addWidget(propertyGroup, 2, 0);
    QVBoxLayout *propertyTableLayout = new QVBoxLayout(propertyGroup);

    // Allow populating from a spice file if appropriate.
    if (QStringList({"Diode", "_BJT", "JFET", "MOSFET"}).contains(component->Model))
    {
      QHBoxLayout *spiceButtonLayout = new QHBoxLayout;
      propertyTableLayout->addLayout(spiceButtonLayout);
      QPushButton* spiceButton = new QPushButton(tr("Populate parameters from SPICE file..."), this);
      connect(spiceButton, &QPushButton::released, this, &ComponentDialog::slotFillFromSpice);
      spiceButtonLayout->addWidget(spiceButton);
      spiceButtonLayout->addStretch();
    }

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

    updatePropertyTable(component);

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

  setTabOrder(componentNameWidget, buttonBox);
  buttonBox->setFocus();
  // componentNameWidget->setFocus();
}

ComponentDialog::~ComponentDialog()
{
  delete compNameVal;
  delete intVal;
  delete nameVal;
  delete paramVal;

  // Clean up the sweep parameter multi-widget containers. The widgets are deleted by
  // the system because they are reparented to the dialog or the dialog's subwidgets.
  for (auto it = sweepParamWidget.keyValueBegin(); it != sweepParamWidget.keyValueEnd(); ++it) 
  {
    if (it->second)
      delete it->second;
  }
}

// -------------------------------------------------------------------------
// Intercept key presses and move to next row if user presses enter in the
// property table.
void ComponentDialog::keyPressEvent(QKeyEvent* e)
{
  // Workaround for Qt+Wayland bug. Otherwise Qt::Key_Return invokes slotOKButton()
  // before registering propertyTable changes meaning the old value is retained.
  if (e->key() != Qt::Key_Return) {
    QDialog::keyPressEvent(e);
  }
}

// -------------------------------------------------------------------------
// Updates all the widgets on the sweep page according to the sweep type.
void ComponentDialog::updateSweepWidgets(const QString& type)
{
  for (auto it = sweepParamWidget.keyValueBegin(); it != sweepParamWidget.keyValueEnd(); ++it) 
  {
    it->second->setLabel(sweepTypeSpecialLabels.contains(qMakePair(type,it->first)) ? 
                          sweepTypeSpecialLabels[qMakePair(type,it->first)] : it->second->defaultLabel());
    it->second->setHidden(paramsHiddenBySim.contains(it->first) &&
                            paramsHiddenBySim[it->first].contains(component->Model));
    it->second->setEnabled(sweepTypeEnabledParams.contains(type) && 
                            sweepTypeEnabledParams[type].contains(it->first));
  }
}

// -------------------------------------------------------------------------
// Updates all the sweep params on the sweep page according the component value.
void ComponentDialog::updateSweepProperty(const QString& property)
{
  // Cache some pointers for convenience.
  ParamWidget* startEdit = sweepParamWidget["Start"];
  ParamWidget* stopEdit = sweepParamWidget["Stop"];
  ParamWidget* pointsEdit = sweepParamWidget["Points"];

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

      // Make sure text edits have sensible values.
      startEdit->setValue(startEdit->value() == "" ? "1" : startEdit->value());
      stopEdit->setValue(stopEdit->value() == "" ? "100" : stopEdit->value());
      pointsEdit->setValue(pointsEdit->value() == "" ? "100" : pointsEdit->value());
    }
  }

  if (property == "Values")
  {
    // Do nothing.
  }

  // Specialisations for updating start, stop, step, and points values.
  else 
  {
    double start = str2num(startEdit->value());
    double stop = str2num(stopEdit->value());

    if (sweepParamWidget["Type"]->value() == "log")
    {
      if (property == "Start" || property == "Stop" || property == "Points" || property == "All")
      {
        double points = str2num(pointsEdit->value());
        double step = (points - 1.0) / log10(fabs((stop < 1.0 ? 1.0 : stop) / (start < 1.0 ? 1.0 : start)));
        sweepParamWidget["Step"]->setValue(misc::num2str(step));
      }
      else if (property == "Step")
      {
        double step = str2num(sweepParamWidget["Step"]->value());
        double points = log10(fabs((stop < 1.0 ? 1.0 : stop) / (start < 1.0 ? 1.0 : start))) * step + 1.0;
        pointsEdit->setValue(QString::number(round(points), 'g', 16));
      }     
    }
    else
    {
      if (property == "Start" || property == "Stop" || property == "Points" || property == "All")
      {
        double points = str2num(pointsEdit->value());
        double step = (stop - start) / (points - 1.0);
        sweepParamWidget["Step"]->setValue(misc::num2str(step));
      }
      else if (property == "Step")
      {
        double step = str2num(sweepParamWidget["Step"]->value());
        double points = (stop - start) / step + 1.0;
        pointsEdit->setValue(QString::number(round(points), 'g', 16));
      }     
    }
  }
}

// -------------------------------------------------------------------------
// Updates the property table with the current values stored in the component.
void ComponentDialog::updatePropertyTable(const Component* updateComponent)
{
    // Add component properties to the properties table with the exception of the sweep properties.
    int row = 0;
    for (const Property* property : updateComponent->Props)
    {
      if (hasSweep && sweepProperties.contains(property->Name))
        continue;

      /* TODO: ***HACK*** to be fixed */
      if (property->Name == "Symbol" || property->Name == "Values")
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

      // Create a compound widget that selects a file.
      else if (property->Name == "File")
      {
        CompoundWidget* compound = new CompoundWidget(property->Value, this, &ComponentDialog::slotBrowseFile);
        propertyTable->setCellWidget(row, 1, compound);
        propertyTable->setItem(row, 1, new QTableWidgetItem(CompoundCell));
      }

      // Create a compound widget that provides a simple equation editor.
      // TODO: This for when parameters have a parameter type flag (see #974)
      else if (false)
      {
        CompoundWidget* compound = new CompoundWidget(property->Value, this, &ComponentDialog::simpleEditEqn);
        propertyTable->setCellWidget(row, 1, compound);
        propertyTable->setItem(row, 1, new QTableWidgetItem(CompoundCell));
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
      propertyTable->item(row, 2)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
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
    if (eqnSimCombo && property->Name == "Simulation")
      eqnSimCombo->setCurrentText(property->Value);

    else if (eqnExportCheck && property->Name == "Export")
      eqnExportCheck->setCheckState(property->Value == "yes" ? Qt::Checked : Qt::Unchecked);

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
  if (eqnSimCombo)
    component->Props.append(new Property("Simulation", eqnSimCombo->currentText(), true, "Simulation name"));

  QString text = eqnEditor->document()->toPlainText();
  QStringList lines = text.split('\n', Qt::SkipEmptyParts);
  
  for (const QString& line : lines)
  {
    QStringList parts = line.split('=');
    if (parts.count() == 2)
      component->Props.append(new Property(parts[0].trimmed(), parts[1].trimmed(), true));
  }

  if (eqnExportCheck)
    component->Props.append(new Property("Export", eqnExportCheck->checkState() == Qt::Checked ? "yes" : "no", false));
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
    int row = 0;

    // Update the components sweep properties if it has them.
    if (hasSweep)
    {
      // Note: Order is very important here. The component expects parameters in a 
      // specific order depending on which sweep parameters are valid for the component
      // type.
      for (auto param : sweepProperties) 
      {
        /* TODO: ***HACK*** to be fixed */
        QString temp = param;
        if (param == "Points" && sweepParamWidget["Type"]->value() == "list")
          temp = "Values";

        if (!(paramsHiddenBySim[param].contains(component->Model)))
        {
          component->Props.at(row)->Value = sweepParamWidget[temp]->value();
          component->Props.at(row)->display = sweepParamWidget[temp]->check();
          row++;
        }
      }
    }

    row = 0;
    for (Property* property : component->Props)
    {
      if (hasSweep && sweepParamWidget.contains(property->Name))
        continue;

      else 
      {
        QTableWidgetItem* item = propertyTable->item(row, 1);
        // TODO: If the number of items has changed because of an earlier edit, then
        // the property list needs to be updated. An example is when the number of ports
        // of an EDD is increased.
        if (!item)
          continue;

        if (item->type() == ComboBoxCell)
        {
          QComboBox* cellCombo = static_cast<QComboBox*>(propertyTable->cellWidget(row, 1));
          property->Value = cellCombo->currentText();
        }

        else if (item->type() == CompoundCell)
        {
          CompoundWidget* cellCompound = static_cast<CompoundWidget*>(propertyTable->cellWidget(row, 1));
          property->Value = cellCompound->text();
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
void ComponentDialog::slotBrowseFile(QLineEdit* lineEdit)
{
  Q_ASSERT(lineEdit);

  // current file name from the component properties
  QString currFileName = lineEdit->text();
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

    lineEdit->setText(s);
  }
}

// -------------------------------------------------------------------------
// Open a simple dialog to edit the line equation. Note, unlike the full
// equation editor used within the component dialog, this simple editor does
// not allow the parameter name to change and no = sign is needed.
void ComponentDialog::simpleEditEqn(QLineEdit* lineEdit)
{
  // Pass a local copy of the current eqn to the simple eqn editor.
  QString eqn = lineEdit->text();

  SimpleEqnDialog dialog(eqn, this);
  if (dialog.exec() == QDialog::Accepted)
    lineEdit->setText(eqn);
}

// -------------------------------------------------------------------------
// Simple text editor dialog with equation formating.
SimpleEqnDialog::SimpleEqnDialog(QString& string, QWidget* parent)
: QDialog(parent), mText(string)
{
  setMinimumSize(300, 300);
  
  // Setup dialog layout.
  QVBoxLayout* layout = new QVBoxLayout(this);

  // Add the equation text editor.
  QFont font("Courier", 10);   
  mEditor = new QTextEdit(mText, this);
  mEditor->setFont(font);
  new EqnHighlighter("ngspice", mEditor->document());
  layout->addWidget(mEditor, 2);

  // Add the dialog button widgets.
  QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | 
                                                      QDialogButtonBox::Cancel);

  connect(buttonBox, &QDialogButtonBox::accepted, this, &SimpleEqnDialog::slotOkButton);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

  layout->addWidget(buttonBox);  
}

// -------------------------------------------------------------------------
// Close the simple eqn editor dialog and copy the edited text back to the parent.
void SimpleEqnDialog::slotOkButton()
{
  mText = mEditor->toPlainText();
  
  done(QDialog::Accepted);
}

/* Removed as it doesn't make much sense to change the currently edited file
   whilst the properties dialog is open. There is already a UI to edit a 
   subcircuit file from the schematic view.
// -------------------------------------------------------------------------
void ComponentDialog::slotEditFile()
{
  qDebug() << "editing file " << component->Props.at(0)->Value << " or " << propertyTable->item(0, 1)->text();
  document->App->editFile(misc::properAbsFileName(component->Props.at(0)->Value, document));
}
*/

// -------------------------------------------------------------------------
// Get a list of simulations in the schematic.
QStringList ComponentDialog::getSimulationList(bool includeGeneric)
{
    QStringList sim_lst;
    Schematic *sch = component->getSchematic();
    if (sch == nullptr) {
        return sim_lst;
    }
    
    for (size_t i = 0; i < sch->a_DocComps.count(); i++) {
        Component *c = sch->a_DocComps.at(i);
        if (!c->isSimulation) continue;
        if (c->Model == ".FOUR") continue;
        if (c->Model == ".PZ") continue;
        if (c->Model == ".SENS") continue;
        if (c->Model == ".SENS_AC") continue;
        if (c->Model == ".SW" && !c->Props.at(0)->Value.toUpper().startsWith("DC") ) continue;
        sim_lst.append(c->Name);
    }

    if (includeGeneric) {
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

      sim_lst.prepend("ALL");
    }

    return sim_lst;
}

// -------------------------------------------------------------------------
// Fill the parameters of certain components (diode, BJT, JFET, MOSFET) 
// from a SPICE file (see #795)
void ComponentDialog::slotFillFromSpice()
{
  // Make a copy of the componenty type and properties.
  Component tempComponent;
  tempComponent.SpiceModelcards = component->SpiceModelcards;
  for (auto property : component->Props)
    tempComponent.Props.append(new Property(property->Name, property->Value, property->display, property->Description));

  // Populate the temporary component from the spice model.
  fillFromSpiceDialog *dlg = new fillFromSpiceDialog(&tempComponent, this);
  auto r = dlg->exec();

  // Update the property table with the newly populated temporary component.
  if (r == QDialog::Accepted) {
    updatePropertyTable(&tempComponent);
  }

  // Cleanup.
  for (auto property : tempComponent.Props)
    delete property; 

  delete dlg;
}
