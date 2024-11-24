#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "misc.h"
#include "main.h"
#include "component.h"

#include "schematic.h"
#include "symbolwidget.h"
#include "spicelibcompdialog.h"


SpiceLibCompDialog::SpiceLibCompDialog(Component *pc, Schematic *sch) : QDialog{sch}
{
  comp = pc;
  Doc = sch;
  symbolPinsCount = 0;
  isChanged = false;
  libError = false;
  prev_col = -1;
  prev_row = -1;

  QString file = comp->Props.at(0)->Value;
  if (!file.isEmpty()) {
    file = misc::properAbsFileName(file, Doc);
    QFileInfo inf(file);
    lastLibDir = inf.absoluteDir().path();
  } else {
    QFileInfo inf = Doc->getFileInfo();
    if (inf.absoluteFilePath().isEmpty()) { // untitled document
      lastLibDir = QucsSettings.QucsWorkDir.absolutePath();
    } else { // saved schematic
      lastLibDir = inf.absoluteDir().path();
    }
  }
  bool show_lib = comp->Props.at(0)->display;
  QString device = comp->Props.at(1)->Value;
  bool show_model = comp->Props.at(1)->display;
  QString sym = comp->Props.at(2)->Value;
  QString par = comp->Props.at(3)->Value;
  bool show_par = comp->Props.at(3)->display;
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

  listSymPattern = new QListWidget;
  QStringList lst_patterns;
  misc::getSymbolPatternsList(lst_patterns);
  listSymPattern->addItems(lst_patterns);
  if (lst_patterns.isEmpty()) {
    QString dir_name = QucsSettings.BinDir + "/../share/" QUCS_NAME "/symbols/";
    QString msg = tr("No symbol files found at the following path:\n");
    msg += dir_name;
    msg += tr("\nCheck you installation!\n");
    QMessageBox::warning(this,tr("Warning"),msg);
  }
  listSymPattern->setCurrentRow(0);
  listSymPattern->setSelectionMode(QAbstractItemView::SingleSelection);
  connect(listSymPattern,SIGNAL(currentRowChanged(int)),this,SLOT(slotSetSymbol()));

  rbAutoSymbol = new QRadioButton(tr("Automatic symbol"));
  rbSymFromTemplate = new QRadioButton(tr("Symbol from template"));
  if (lst_patterns.isEmpty()) {
    rbSymFromTemplate->setEnabled(false);
  }
  rbUserSym = new QRadioButton(tr("Symbol from file"));
  btnOpenSym = new QPushButton(tr("Open"));
  edtSymFile = new QLineEdit();
  connect(btnOpenSym,SIGNAL(clicked(bool)),this,SLOT(slotBtnOpenSym()));

  chbShowLib = new QCheckBox(tr("Show"));
  chbShowLib->setChecked(show_lib);
  chbShowModel = new QCheckBox(tr("Show"));
  chbShowModel->setChecked(show_model);
  chbShowParams = new QCheckBox(tr("Show"));
  chbShowParams->setChecked(show_par);
  connect(chbShowLib,SIGNAL(toggled(bool)),this,SLOT(slotChanged()));
  connect(chbShowModel,SIGNAL(toggled(bool)),this,SLOT(slotChanged()));
  connect(chbShowParams,SIGNAL(toggled(bool)),this,SLOT(slotChanged()));

  if (QFileInfo::exists(misc::properAbsFileName(sym, Doc))) {
    edtSymFile->setText(misc::properAbsFileName(sym, Doc));
    rbUserSym->setChecked(true);
    QFileInfo inf(misc::properAbsFileName(sym, Doc));
    lastSymbolDir = inf.absoluteDir().path();
  } else {
    QFileInfo inf = Doc->getFileInfo();
    lastSymbolDir = inf.absoluteDir().path();
    if (sym == "auto") {
      rbAutoSymbol->setChecked(true);
    } else {
      rbSymFromTemplate->setChecked(true);
    }
  }

  symbol = new SymbolWidget;
  symbol->disableDragNDrop();
  symbol->enableShowPinNumbers();
  symbol->setPaintText("");

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
  l2->addWidget(chbShowLib);
  top->addLayout(l2);

  QHBoxLayout *l7 = new QHBoxLayout;
  l7->addWidget(lbl_par);
  l7->addWidget(edtParams);
  l7->addWidget(chbShowParams);
  top->addLayout(l7);

  QGridLayout *gl1 = new QGridLayout;
  gl1->addWidget(rbAutoSymbol,0,0);
  gl1->addWidget(rbSymFromTemplate,1,0);
  gl1->addWidget(rbUserSym,2,0);
  gl1->addWidget(edtSymFile,2,1);
  gl1->addWidget(btnOpenSym,2,2);
  gl1->addWidget(symbol,0,3,3,2);
  gl1->addWidget(listSymPattern,0,5,3,2);
  top->addLayout(gl1);

  QHBoxLayout *l3 = new QHBoxLayout;
  l3->addWidget(tbwPinsTable);
  QGroupBox *gpb1 = new QGroupBox(tr("SPICE model"));
  QVBoxLayout *l8 = new QVBoxLayout;
  QHBoxLayout *l5 = new QHBoxLayout;
  l5->addWidget(lblDevice);
  l5->addWidget(cbxSelectSubcir);
  l5->addWidget(chbShowModel);
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
  auto items = listSymPattern->findItems(sym,Qt::MatchExactly);
  if (!items.isEmpty()) listSymPattern->setCurrentItem(items.at(0));
  //listSymPattern->setCurrentText(sym);

  if (!pin_list.isEmpty()) {
    QStringList pins = pin_list.split(";");
    for(int i = 0; i < pins.count(); i++) {
      QTableWidgetItem *itm = tbwPinsTable->item(i,1);
      if (itm != nullptr) {
        itm->setText(pins.at(i));
      }
    }
  }

  listSymPattern->setMinimumWidth(0.75*tbwPinsTable->minimumWidth());
  symbol->setMinimumWidth(0.75*tbwPinsTable->minimumWidth());

  btnApply->setEnabled(false);
  connect(rbAutoSymbol,SIGNAL(toggled(bool)),this,SLOT(slotSetSymbol()));
  connect(rbSymFromTemplate,SIGNAL(toggled(bool)),this,SLOT(slotSetSymbol()));
  connect(rbUserSym,SIGNAL(toggled(bool)),this,SLOT(slotSetSymbol()));
  connect(edtLibPath,SIGNAL(textChanged(QString)),this,SLOT(slotChanged()));
  connect(edtParams,SIGNAL(textChanged(QString)),this,SLOT(slotChanged()));
  connect(tbwPinsTable,SIGNAL(cellChanged(int,int)),this,SLOT(slotChanged()));
  connect(listSymPattern,SIGNAL(currentIndexChanged(int)),this,SLOT(slotChanged()));
  connect(cbxSelectSubcir,SIGNAL(currentIndexChanged(int)),this,SLOT(slotChanged()));
  connect(edtSymFile,SIGNAL(textChanged(QString)),this,SLOT(slotSetSymbol()));
  connect(edtSymFile,SIGNAL(textChanged(QString)),this,SLOT(slotChanged()));

}

void SpiceLibCompDialog::slotFillSubcirComboBox()
{
  QString libfile = edtLibPath->text();
  if (!QFile::exists(libfile)) return;
  int  r = parseLibFile(libfile);
  libError = false;
  if (r != noError) {
    libError = true;
    QString msg;
    switch (r) {
    case failedOpenFile:
      msg = tr("Failed open file: ") + libfile;
      break;
    case noSUBCKT:
      msg = tr("SPICE library parse error.\n"
               "No SUBCKT directive found in library ") + libfile;
      break;
    default:
      msg = tr("SPICE library parse error");
    }
    QMessageBox::critical(this,tr("Error"),msg);
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

int SpiceLibCompDialog::parseLibFile(const QString &filename)
{
  if (!QFileInfo::exists(filename)) return false;
  QFile f(filename);
  if (!f.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(this,tr("Error"),tr("Failed to open file: ") + filename);
    return failedOpenFile;
  }

  subcirPins.clear();
  subcirSPICE.clear();
  QTextStream ts(&f);

  bool subcir_start = false;
  bool header_start = false;
  QString last_subcir;
  QString subname;
  QString subcir_body;
  while (!ts.atEnd()) {
    QString line = ts.readLine();
    line = line.trimmed();
    line = line.toUpper();
    // semicolon may be comment start
    auto start_comment = line.indexOf(';');
    if (start_comment != -1) {
      line = line.left(start_comment);
    }

    if (header_start) {
      // line continuation
      if (line.startsWith("+")) {
        line.remove(0,1);
        QStringList pins = line.split(QRegularExpression("[ \\t]"),Qt::SkipEmptyParts);
        subcirPins[last_subcir].append(pins);
      } else {
        // end of header
        header_start = false;
      }
    }

    if (line.startsWith(".SUBCKT")) {
      subcir_start = true;
      header_start = true;
      subcir_body.clear();
      QStringList pin_names;
      QStringList tokens = line.split(QRegularExpression("[ \\t]"),Qt::SkipEmptyParts);
      if (tokens.count() > 3) {
        subname = tokens.at(1);
        last_subcir = subname;
      } else continue;
      tokens.removeFirst();
      tokens.removeFirst();
      for (const auto &s1: tokens) {
        if (s1 == "PARAMS:") header_start = false;
        if (!s1.contains('=') && (s1 != "PARAMS:")) {
          pin_names.append(s1);
        }
      }
      subcirPins[subname] = pin_names;
    }
    if (subcir_start) {
      subcir_body += line + "\n";
    }
    if (line.startsWith(".ENDS")) {
      subcir_start = false;
      subcirSPICE[subname] = subcir_body;
    }
  }

  f.close();
  if (subcirPins.isEmpty()) {
    return noSUBCKT;
  }
  return noError;

}

void SpiceLibCompDialog::slotSetSymbol()
{
  int result = -1;
  if (rbAutoSymbol->isChecked()) {
    tbwPinsTable->setEnabled(false);
    listSymPattern->setEnabled(false);
    edtSymFile->setEnabled(false);
    btnOpenSym->setEnabled(false);
    QString s1 = "";
    QString s2 = "SpLib";
    symbol->setSymbol(s1, s1, s2);
    result = 0;
    symbol->setWarning(tr("No symbol loaded"));
    symbolPinsCount = 0;
  } else if (rbSymFromTemplate->isChecked()) {
    if (listSymPattern->currentItem() == nullptr) {
      return; // empty list
    }
    tbwPinsTable->setEnabled(true);
    listSymPattern->setEnabled(true);
    edtSymFile->setEnabled(false);
    btnOpenSym->setEnabled(false);
    QString dir_name = QucsSettings.BinDir + "/../share/" QUCS_NAME "/symbols/";
    QString file = dir_name + listSymPattern->currentItem()->text() + ".sym";
    result = symbol->loadSymFile(file);
    symbolPinsCount = symbol->getPortsNumber();
  } else if (rbUserSym->isChecked()) {
    tbwPinsTable->setEnabled(true);
    listSymPattern->setEnabled(false);
    edtSymFile->setEnabled(true);
    btnOpenSym->setEnabled(true);
    if (edtSymFile->text().isEmpty()) {
      symbolPinsCount = 0;
      return;
    }
    result = symbol->loadSymFile(edtSymFile->text());
    symbolPinsCount = symbol->getPortsNumber();
  }

  if (result < 0) {
    QMessageBox::critical(this,tr("Error"),tr("Failed to load symbol file!"));
    return;
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
  if (c == 0) return; // do not edit the first column

  if (prev_col >= 0 && prev_row >= 0) { // remove combo box from previous cell
    QTableWidgetItem *itm = new QTableWidgetItem("NC");
    tbwPinsTable->removeCellWidget(prev_row, prev_col);
    tbwPinsTable->setItem(prev_row, prev_col, itm);
  }

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
  tbwPinsTable->item(r,c)->setText("");
  tbwPinsTable->setCellWidget(r,c,cbxSelectPin);
  connect(cbxSelectPin,SIGNAL(activated(int)),this,SLOT(slotSelectPin()));

  prev_col = c; // remebmebr cell with combo box
  prev_row = r;
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

  prev_col = -1; // clear cell index with combo box
  prev_row = -1;
}

void SpiceLibCompDialog::slotBtnOpenLib()
{
  QString s = QFileDialog::getOpenFileName(this, tr("Open SPICE library"),
                                           lastLibDir,
                                           tr("SPICE files (*.cir +.ckt *.sp *.lib)"));
  if (!s.isEmpty()) {
    QFileInfo inf(s);
    lastLibDir = inf.absoluteDir().path();
    edtLibPath->setText(s);
  }
}

void SpiceLibCompDialog::slotBtnOpenSym()
{
  QString s = QFileDialog::getOpenFileName(this, tr("Open symbol file"),
                                           lastSymbolDir,
                                           tr("Schematic symbol (*.sym)"));
  if (!s.isEmpty()) {
    QFileInfo inf(s);
    lastSymbolDir = inf.absoluteDir().path();
    edtSymFile->setText(s);
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

  QString  sch_dir = Doc->getFileInfo().absoluteDir().path();
  QString libpath = edtLibPath->text();
  QString sympath = edtSymFile->text();
  if (libpath.startsWith(sch_dir)) {
    libpath = QDir(sch_dir).relativeFilePath(libpath);
  }
  if (sympath.startsWith(sch_dir)) {
    sympath = QDir(sch_dir).relativeFilePath(sympath);
  }

  auto pp = comp->Props.begin();
  (*pp)->Value = libpath;
  (*pp)->display = chbShowLib->isChecked();
  pp++;
  (*pp)->Value = cbxSelectSubcir->currentText();
  (*pp)->display = chbShowModel->isChecked();
  pp++;
  if (rbAutoSymbol->isChecked()) {
    (*pp)->Value = "auto";
  } else if (rbSymFromTemplate->isChecked()) {
    (*pp)->Value = listSymPattern->currentItem()->text();
  } else if (rbUserSym->isChecked()) {
    (*pp)->Value = sympath;
  }
  pp++;
  (*pp)->Value = edtParams->text();
  (*pp)->display = chbShowParams->isChecked();
  pp++;
  (*pp)->Value = QString(pin_string);
  Doc->recreateComponent(comp);
  Doc->viewport()->repaint();
  Doc->setChanged(true,true);
  return true;
}

void SpiceLibCompDialog::slotBtnApply()
{
  if (isChanged) {
    if (libError) {
      QMessageBox::critical(this,tr("Error"),
                            tr("There were library file parse error! Cannot apply changes."));
      return;
    }
    if (setCompProps()) {
      isChanged = false;
      btnApply->setEnabled(false);
    }
  }
}

void SpiceLibCompDialog::slotBtnOK()
{
  if (isChanged) {
    if (libError) {
      QMessageBox::critical(this,tr("Error"),
                            tr("There were library file parse error! Cannot apply changes."));
      reject();
      return;
    }
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
