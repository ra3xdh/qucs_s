#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "misc.h"
#include "main.h"
#include "component.h"

#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QFileDialog>
#include <QPlainTextEdit>
#include <QGroupBox>
#include <QRadioButton>

#include "schematic.h"
#include "symbolwidget.h"
#include "spicelibcompdialog.h"


SpiceLibCompDialog::SpiceLibCompDialog(Component *pc, Schematic *sch) : QDialog{sch}
{
  comp = pc;
  Doc = sch;
  symbolPinsCount = 0;
  isChanged = false;

  QString file = comp->Props.at(0)->Value;
  if (!file.isEmpty()) {
    file = misc::properAbsFileName(file);
  }
  QString device = comp->Props.at(1)->Value;
  QString sym = comp->Props.at(2)->Value;
  QString par = comp->Props.at(3)->Value;
  QString pin_list = comp->Props.at(4)->Value;

  QLabel *lblLibfile = new QLabel("SPICE library:");
  edtLibPath = new QLineEdit;
  edtLibPath->setText(file);
  connect(edtLibPath,SIGNAL(textChanged(QString)),this,SLOT(slotFillSubcirComboBox()));
  btnOpenLib = new QPushButton(tr("Open"));
  connect(btnOpenLib,SIGNAL(clicked(bool)),this,SLOT(slotBtnOpenLib()));

  QLabel *lbl_par = new QLabel("Component parameters");
  edtParams = new QLineEdit;
  edtParams->setText(par);

  QLabel *lblDevice = new QLabel("Subcircuit:");
  cbxSelectSubcir = new QComboBox;
  connect(cbxSelectSubcir,SIGNAL(currentIndexChanged(int)),this,SLOT(slotFillPinsTable()));

  cbxSymPattern = new QComboBox;
  QStringList lst_patterns;
  lst_patterns.append("auto");
  misc::getSymbolPatternsList(lst_patterns);
  cbxSymPattern->addItems(lst_patterns);
  connect(cbxSymPattern,SIGNAL(currentIndexChanged(int)),this,SLOT(slotSetSymbol()));

  rbAutoSymbol = new QRadioButton(tr("Automatic symbol"));
  rbSymFromTemplate = new QRadioButton(tr("Symbol from template"));
  rbUserSym = new QRadioButton(tr("Symbol from file"));
  btnOpenSym = new QPushButton(tr("Open"));
  edtSymFile = new QLineEdit();
  connect(btnOpenSym,SIGNAL(clicked(bool)),this,SLOT(slotBtnOpenSym()));
  connect(edtSymFile,SIGNAL(textChanged(QString)),this,SLOT(slotSetSymbol()));
  connect(edtSymFile,SIGNAL(textChanged(QString)),this,SLOT(slotChanged()));

  if (QFileInfo::exists(misc::properAbsFileName(sym))) {
    edtSymFile->setText(sym);
    rbUserSym->setChecked(true);
  } else {
    if (sym == "auto") {
      rbAutoSymbol->setChecked(true);
    } else {
      rbSymFromTemplate->setChecked(true);
    }
  }

  symbol = new SymbolWidget;
  symbol->disableDragNDrop();
  symbol->enableShowPinNumbers();

  tbwPinsTable = new QTableWidget;
  tbwPinsTable->setColumnCount(2);
  tbwPinsTable->setRowCount(100);
  QStringList lbl_cols;
  lbl_cols<<"Subcircuit pin"<<"Symbol pin";
  tbwPinsTable->setHorizontalHeaderLabels(lbl_cols);
  tbwPinsTable->setMinimumWidth(2.2*tbwPinsTable->columnWidth(0));
  tbwPinsTable->setMinimumHeight(5*tbwPinsTable->rowHeight(0));
  connect(tbwPinsTable,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(slotTableCellDoubleClick()));

  edtSPICE = new QPlainTextEdit;
  edtSPICE->setReadOnly(true);
  edtSPICE->setMinimumSize(tbwPinsTable->minimumSize());

  btnOK = new QPushButton(tr("OK"));
  connect(btnOK,SIGNAL(clicked(bool)),this,SLOT(slotBtnOK()));
  btnApply = new QPushButton(tr("Apply"));
  connect(btnApply,SIGNAL(clicked(bool)),this,SLOT(slotBtnApply()));
  btnCancel = new QPushButton(tr("Cancel"));
  connect(btnCancel,SIGNAL(clicked(bool)),this,SLOT(slotBtnCancel()));

  QVBoxLayout *top = new QVBoxLayout;
  QHBoxLayout *l2 = new QHBoxLayout;
  l2->addWidget(lblLibfile);
  l2->addWidget(edtLibPath,4);
  l2->addWidget(btnOpenLib,1);
  top->addLayout(l2);

  QHBoxLayout *l7 = new QHBoxLayout;
  l7->addWidget(lbl_par);
  l7->addWidget(edtParams);
  top->addLayout(l7);

  QGridLayout *gl1 = new QGridLayout;
  gl1->addWidget(rbAutoSymbol,0,0);
  gl1->addWidget(rbSymFromTemplate,1,0);
  gl1->addWidget(rbUserSym,2,0);
  gl1->addWidget(cbxSymPattern,1,1);
  gl1->addWidget(edtSymFile,2,1);
  gl1->addWidget(btnOpenSym,2,2);
  gl1->addWidget(symbol,0,3,3,2);
  top->addLayout(gl1);

  QHBoxLayout *l3 = new QHBoxLayout;
  l3->addWidget(tbwPinsTable);
  QGroupBox *gpb1 = new QGroupBox(tr("SPICE model"));
  QVBoxLayout *l8 = new QVBoxLayout;
  QHBoxLayout *l5 = new QHBoxLayout;
  l5->addWidget(lblDevice);
  l5->addWidget(cbxSelectSubcir);
  l8->addLayout(l5);
  l8->addWidget(edtSPICE);
  gpb1->setLayout(l8);
  l3->addWidget(gpb1);
  top->addLayout(l3,3);


  QHBoxLayout *l4 = new QHBoxLayout;
  l4->addWidget(btnOK);
  l4->addWidget(btnApply);
  l4->addWidget(btnCancel);
  l4->addStretch();
  top->addLayout(l4);

  this->slotSetSymbol();
  this->setLayout(top);
  this->setWindowTitle(tr("Edit SPICE library device"));
  slotFillSubcirComboBox();

  cbxSelectSubcir->setCurrentText(device);
  cbxSymPattern->setCurrentText(sym);

  if (!pin_list.isEmpty()) {
    QStringList pins = pin_list.split(";");
    for(int i = 0; i < pins.count(); i++) {
      QTableWidgetItem *itm = tbwPinsTable->item(i,1);
      if (itm != nullptr) {
        itm->setText(pins.at(i));
      }
    }
  }

  btnApply->setEnabled(false);
  connect(rbAutoSymbol,SIGNAL(toggled(bool)),this,SLOT(slotSetSymbol()));
  connect(rbSymFromTemplate,SIGNAL(toggled(bool)),this,SLOT(slotSetSymbol()));
  connect(rbUserSym,SIGNAL(toggled(bool)),this,SLOT(slotSetSymbol()));
  connect(edtLibPath,SIGNAL(textChanged(QString)),this,SLOT(slotChanged()));
  connect(edtParams,SIGNAL(textChanged(QString)),this,SLOT(slotChanged()));
  connect(tbwPinsTable,SIGNAL(cellChanged(int,int)),this,SLOT(slotChanged()));
  connect(cbxSymPattern,SIGNAL(currentIndexChanged(int)),this,SLOT(slotChanged()));
  connect(cbxSelectSubcir,SIGNAL(currentIndexChanged(int)),this,SLOT(slotChanged()));

}

void SpiceLibCompDialog::slotFillSubcirComboBox()
{
  QString libfile = edtLibPath->text();
  if (!QFile::exists(libfile)) return;
  if (!parseLibFile(libfile)) {
    QMessageBox::critical(this,tr("Error"),tr("SPICE library parse error"));
    return;
  }

  cbxSelectSubcir->blockSignals(true);
  cbxSelectSubcir->clear();
  for(const auto &key: subcirPins.keys()) {
    cbxSelectSubcir->addItem(key);
  }
  cbxSelectSubcir->blockSignals(false);
  slotFillPinsTable();
}

void SpiceLibCompDialog::slotFillPinsTable()
{
  QString subcir_name = cbxSelectSubcir->currentText();
  if (subcirPins.find(subcir_name) == subcirPins.end()) return;
  QStringList pins = subcirPins[subcir_name];
  tbwPinsTable->clearContents();
  tbwPinsTable->setRowCount(pins.count());
  for (int i = 0; i < pins.count(); i++) {
    QTableWidgetItem *itm1 = new QTableWidgetItem(pins.at(i));
    tbwPinsTable->setItem(i,0,itm1);
    QTableWidgetItem *itm2 = new QTableWidgetItem("NC");
    tbwPinsTable->setItem(i,1,itm2);
  }

  edtSPICE->clear();
  edtSPICE->setPlainText(subcirSPICE[subcir_name]);
}

bool SpiceLibCompDialog::parseLibFile(const QString &filename)
{
  if (!QFileInfo::exists(filename)) return false;
  QFile f(filename);
  if (!f.open(QIODevice::ReadOnly)) {
    return false;
  }

  subcirPins.clear();
  subcirSPICE.clear();
  QTextStream ts(&f);

  bool subcir_start = false;
  QString subname;
  QString subcir_body;
  while (!ts.atEnd()) {
    QString line = ts.readLine();
    line = line.trimmed();
    line = line.toUpper();
    if (line.startsWith(".SUBCKT")) {
      subcir_start = true;
      subcir_body.clear();
      QStringList pin_names;
      QStringList tokens = line.split(QRegularExpression("[ \\t]"),Qt::SkipEmptyParts);
      if (tokens.count() > 3) {
        subname = tokens.at(1);
      } else continue;
      tokens.removeFirst();
      tokens.removeFirst();
      for (const auto &s1: tokens) {
        if (!s1.contains('=') && (s1 != "PARAMS:")) {
          pin_names.append(s1);
        }
      }
      subcirPins[subname] = pin_names;
    }
    if (subcir_start) {
      subcir_body += line + "\n";
    }
    if (line == ".ENDS") {
      subcir_start = false;
      subcirSPICE[subname] = subcir_body;
    }
  }

  f.close();
  if (subcirPins.isEmpty()) {
    return false;
  }
  return true;

}

void SpiceLibCompDialog::slotSetSymbol()
{
  if (rbAutoSymbol->isChecked()) {
    tbwPinsTable->setEnabled(false);
    cbxSymPattern->setEnabled(false);
    edtSymFile->setEnabled(false);
    btnOpenSym->setEnabled(false);
    QString s1 = "";
    QString s2 = "SpLib";
    symbol->setSymbol(s1, s1, s2);
    symbolPinsCount = 0;
  } else if (rbSymFromTemplate->isChecked()) {
    tbwPinsTable->setEnabled(true);
    cbxSymPattern->setEnabled(true);
    edtSymFile->setEnabled(false);
    btnOpenSym->setEnabled(false);
    QString dir_name = QucsSettings.BinDir + "/../share/" QUCS_NAME "/symbols/";
    QString file = dir_name + cbxSymPattern->currentText() + ".sym";
    symbol->loadSymFile(file);
    symbolPinsCount = symbol->getPortsNumber();
  } else if (rbUserSym->isChecked()) {
    tbwPinsTable->setEnabled(true);
    cbxSymPattern->setEnabled(false);
    edtSymFile->setEnabled(true);
    btnOpenSym->setEnabled(true);
    symbol->loadSymFile(edtSymFile->text());
    symbolPinsCount = symbol->getPortsNumber();
  }
  for (int i = 0; i < tbwPinsTable->rowCount(); i++) {
    QTableWidgetItem *itm = new QTableWidgetItem("NC");
    tbwPinsTable->setItem(i,1,itm);
  }
  isChanged = true;
  btnApply->setEnabled(true);
}

void SpiceLibCompDialog::slotTableCellDoubleClick()
{
  int r = tbwPinsTable->currentRow();
  int c = tbwPinsTable->currentColumn();
  if (c == 0) return; // do not edit the forst column
  QComboBox *cbxSelectPin = new QComboBox;
  cbxSelectPin->addItem("NC");
  for (int i = 1; i <= symbolPinsCount; i++) {
    bool pinAssigned = false;
    for(int j = 0; j < tbwPinsTable->rowCount(); j++) {
      if (j == r) continue;
      auto itm = tbwPinsTable->item(j,1);
      if (itm == nullptr) continue;
      QString s = itm->text();
      if (s.isEmpty()) continue;
      int pin_num = s.toInt();
      if (pin_num == i) {
        pinAssigned = true;
        break;
      }
    }
    if (!pinAssigned) {
      cbxSelectPin->addItem(QString::number(i));
    }
  }
  tbwPinsTable->setCellWidget(r,c,cbxSelectPin);
  connect(cbxSelectPin,SIGNAL(activated(int)),this,SLOT(slotSelectPin()));
}

void SpiceLibCompDialog::slotSelectPin()
{
  QComboBox *cbxSelectPin = qobject_cast<QComboBox*>(sender());
  QString pin = cbxSelectPin->currentText();
  int r = tbwPinsTable->currentRow();
  int c = tbwPinsTable->currentColumn();
  QTableWidgetItem *itm = new QTableWidgetItem(pin);
  tbwPinsTable->removeCellWidget(r,c);
  tbwPinsTable->setItem(r,c,itm);
}

void SpiceLibCompDialog::slotBtnOpenLib()
{
  QString s = QFileDialog::getOpenFileName(this, tr("Open SPICE library"),
                                           QDir::homePath(),
                                           tr("SPICE files (*.cir +.ckt *.sp *.lib)"));
  if (!s.isEmpty()) edtLibPath->setText(s);
}

void SpiceLibCompDialog::slotBtnOpenSym()
{
  QString s = QFileDialog::getOpenFileName(this, tr("Open symbol file"),
                                           QDir::homePath(),
                                           tr("Schematic symbol (*.sym)"));
  if (!s.isEmpty()) edtSymFile->setText(s);
}

void SpiceLibCompDialog::slotBtnApply()
{
  if (isChanged) {
    if (setCompProps()) {
      isChanged = false;
      btnApply->setEnabled(false);
    }
  }
}

bool SpiceLibCompDialog::setCompProps()
{
  QStringList pins;
  QString pin_string;
  if (rbAutoSymbol->isChecked()) {
    pin_string = "";
  } else {
    for (int i = 0; i < tbwPinsTable->rowCount(); i++) {
      QTableWidgetItem *itm = tbwPinsTable->item(i,1);
      if (itm == nullptr) continue;
    QString s = itm->text();
      if (s == "NC") {
        QMessageBox::warning(this,tr("Warning"),tr("All pins must be assigned"));
        return false;
      }
      pins.append(s);
    }
    pin_string = pins.join(";");
  }

  if (rbUserSym->isChecked() &&
      !QFileInfo::exists(edtSymFile->text())) {
    QMessageBox::warning(this,tr("Warning"),tr("Set a valid symbol file name"));
    return false;
  }

  Property *pp = comp->Props.first();
  pp->Value = edtLibPath->text();
  pp = comp->Props.next();
  pp->Value = cbxSelectSubcir->currentText();
  pp = comp->Props.next();
  if (rbAutoSymbol->isChecked()) {
    pp->Value = cbxSymPattern->currentText();
  } else if (rbSymFromTemplate->isChecked()) {
    pp->Value = "auto";
  } else if (rbUserSym->isChecked()) {
    pp->Value = edtSymFile->text();
  }
  pp = comp->Props.next();
  pp->Value = edtParams->text();
  pp = comp->Props.next();
  pp->Value = QString(pin_string);
  Doc->recreateComponent(comp);
  Doc->viewport()->repaint();
  Doc->setChanged(true,true);
  return true;
}

void SpiceLibCompDialog::slotBtnOK()
{
  if (isChanged) {
    if (setCompProps()) accept();
  } else {
    accept();
  }
}

void SpiceLibCompDialog::slotBtnCancel()
{
  reject();
}

void SpiceLibCompDialog::slotChanged()
{
  isChanged = true;
  btnApply->setEnabled(true);
}
