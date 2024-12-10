#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "misc.h"
#include "main.h"
#include "component.h"

#include "schematic.h"
#include "symbolwidget.h"
#include "spicelibcompdialog.h"


SpiceLibCompDialog::SpiceLibCompDialog(Component *pc, Schematic *sch) :
  QDialog(sch),
  a_symbolPinsCount(0),
  a_isChanged(false),
  a_libError(false),
  a_prev_row(-1),
  a_prev_col(-1),
  a_lastSymbolDir(),
  a_lastLibDir(),
  a_comp(pc),
  a_Doc(sch),
  a_symbol(new SymbolWidget),
  a_edtLibPath(new QLineEdit),
  a_edtParams(new QLineEdit),
  a_edtSymFile(new QLineEdit),
  a_edtSPICE(new QPlainTextEdit),
  a_btnOpenLib(new QPushButton(tr("Open"))),
  a_btnOK(new QPushButton(tr("OK"))),
  a_btnApply(new QPushButton(tr("Apply"))),
  a_btnCancel(new QPushButton(tr("Cancel"))),
  a_btnOpenSym(new QPushButton(tr("Open"))),
  a_tbwPinsTable(new QTableWidget),
  a_cbxSelectSubcir(new QComboBox),
  a_listSymPattern(new QListWidget),
  a_rbSymFromTemplate(new QRadioButton(tr("Symbol from template"))),
  a_rbAutoSymbol(new QRadioButton(tr("Automatic symbol"))),
  a_rbUserSym(new QRadioButton(tr("Symbol from file"))),
  a_chbShowLib(new QCheckBox(tr("Show"))),
  a_chbShowModel(new QCheckBox(tr("Show"))),
  a_chbShowParams(new QCheckBox(tr("Show"))),
  a_subcirPins(),
  a_subcirSPICE()
{
  QString file = a_comp->Props.at(0)->Value;
  if (!file.isEmpty()) {
    file = misc::properAbsFileName(file, a_Doc);
    QFileInfo inf(file);
    a_lastLibDir = inf.absoluteDir().path();
  } else {
    QFileInfo inf = a_Doc->getFileInfo();
    if (inf.absoluteFilePath().isEmpty()) { // untitled document
      a_lastLibDir = QucsSettings.QucsWorkDir.absolutePath();
    } else { // saved schematic
      a_lastLibDir = inf.absoluteDir().path();
    }
  }
  bool show_lib = a_comp->Props.at(0)->display;
  QString device = a_comp->Props.at(1)->Value;
  bool show_model = a_comp->Props.at(1)->display;
  QString sym = a_comp->Props.at(2)->Value;
  QString par = a_comp->Props.at(3)->Value;
  bool show_par = a_comp->Props.at(3)->display;
  QString pin_list = a_comp->Props.at(4)->Value;

  QLabel *lblLibfile = new QLabel("SPICE library:");
  a_edtLibPath->setText(file);
  connect(a_edtLibPath,SIGNAL(textChanged(QString)),this,SLOT(slotFillSubcirComboBox()));
  connect(a_btnOpenLib,SIGNAL(clicked(bool)),this,SLOT(slotBtnOpenLib()));

  QLabel *lbl_par = new QLabel("Component parameters");
  a_edtParams->setText(par);

  QLabel *lblDevice = new QLabel("Subcircuit:");
  connect(a_cbxSelectSubcir,SIGNAL(currentIndexChanged(int)),this,SLOT(slotFillPinsTable()));

  QStringList lst_patterns;
  misc::getSymbolPatternsList(lst_patterns);
  a_listSymPattern->addItems(lst_patterns);
  if (lst_patterns.isEmpty()) {
    QString dir_name = QucsSettings.BinDir + "/../share/" QUCS_NAME "/symbols/";
    QString msg = tr("No symbol files found at the following path:\n");
    msg += dir_name;
    msg += tr("\nCheck you installation!\n");
    QMessageBox::warning(this,tr("Warning"),msg);
  }
  a_listSymPattern->setCurrentRow(0);
  a_listSymPattern->setSelectionMode(QAbstractItemView::SingleSelection);
  connect(a_listSymPattern,SIGNAL(currentRowChanged(int)),this,SLOT(slotSetSymbol()));

  if (lst_patterns.isEmpty()) {
    a_rbSymFromTemplate->setEnabled(false);
  }
  connect(a_btnOpenSym,SIGNAL(clicked(bool)),this,SLOT(slotBtnOpenSym()));

  a_chbShowLib->setChecked(show_lib);
  a_chbShowModel->setChecked(show_model);
  a_chbShowParams->setChecked(show_par);
  connect(a_chbShowLib,SIGNAL(toggled(bool)),this,SLOT(slotChanged()));
  connect(a_chbShowModel,SIGNAL(toggled(bool)),this,SLOT(slotChanged()));
  connect(a_chbShowParams,SIGNAL(toggled(bool)),this,SLOT(slotChanged()));

  if (QFileInfo::exists(misc::properAbsFileName(sym, a_Doc))) {
    a_edtSymFile->setText(misc::properAbsFileName(sym, a_Doc));
    a_rbUserSym->setChecked(true);
    QFileInfo inf(misc::properAbsFileName(sym, a_Doc));
    a_lastSymbolDir = inf.absoluteDir().path();
  } else {
    QFileInfo inf = a_Doc->getFileInfo();
    a_lastSymbolDir = inf.absoluteDir().path();
    if (sym == "auto") {
      a_rbAutoSymbol->setChecked(true);
    } else {
      a_rbSymFromTemplate->setChecked(true);
    }
  }

  a_symbol->disableDragNDrop();
  a_symbol->enableShowPinNumbers();
  a_symbol->setPaintText("");

  a_tbwPinsTable->setColumnCount(2);
  a_tbwPinsTable->setRowCount(100);
  QStringList lbl_cols;
  lbl_cols<<"Subcircuit pin"<<"Symbol pin";
  a_tbwPinsTable->setHorizontalHeaderLabels(lbl_cols);
  a_tbwPinsTable->setMinimumWidth(2.2*a_tbwPinsTable->columnWidth(0));
  a_tbwPinsTable->setMinimumHeight(5*a_tbwPinsTable->rowHeight(0));
  connect(a_tbwPinsTable,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(slotTableCellDoubleClick()));

  a_edtSPICE->setReadOnly(true);
  a_edtSPICE->setMinimumSize(a_tbwPinsTable->minimumSize());

  connect(a_btnOK,SIGNAL(clicked(bool)),this,SLOT(slotBtnOK()));
  connect(a_btnApply,SIGNAL(clicked(bool)),this,SLOT(slotBtnApply()));
  connect(a_btnCancel,SIGNAL(clicked(bool)),this,SLOT(slotBtnCancel()));

  QVBoxLayout *top = new QVBoxLayout;
  QHBoxLayout *l2 = new QHBoxLayout;
  l2->addWidget(lblLibfile);
  l2->addWidget(a_edtLibPath,4);
  l2->addWidget(a_btnOpenLib,1);
  l2->addWidget(a_chbShowLib);
  top->addLayout(l2);

  QHBoxLayout *l7 = new QHBoxLayout;
  l7->addWidget(lbl_par);
  l7->addWidget(a_edtParams);
  l7->addWidget(a_chbShowParams);
  top->addLayout(l7);

  QGridLayout *gl1 = new QGridLayout;
  gl1->addWidget(a_rbAutoSymbol,0,0);
  gl1->addWidget(a_rbSymFromTemplate,1,0);
  gl1->addWidget(a_rbUserSym,2,0);
  gl1->addWidget(a_edtSymFile,2,1);
  gl1->addWidget(a_btnOpenSym,2,2);
  gl1->addWidget(a_symbol,0,3,3,2);
  gl1->addWidget(a_listSymPattern,0,5,3,2);
  top->addLayout(gl1);

  QHBoxLayout *l3 = new QHBoxLayout;
  l3->addWidget(a_tbwPinsTable);
  QGroupBox *gpb1 = new QGroupBox(tr("SPICE model"));
  QVBoxLayout *l8 = new QVBoxLayout;
  QHBoxLayout *l5 = new QHBoxLayout;
  l5->addWidget(lblDevice);
  l5->addWidget(a_cbxSelectSubcir);
  l5->addWidget(a_chbShowModel);
  l8->addLayout(l5);
  l8->addWidget(a_edtSPICE);
  gpb1->setLayout(l8);
  l3->addWidget(gpb1);
  top->addLayout(l3,3);


  QHBoxLayout *l4 = new QHBoxLayout;
  l4->addWidget(a_btnOK);
  l4->addWidget(a_btnApply);
  l4->addWidget(a_btnCancel);
  l4->addStretch();
  top->addLayout(l4);

  this->slotSetSymbol();
  this->setLayout(top);
  this->setWindowTitle(tr("Edit SPICE library device"));
  slotFillSubcirComboBox();

  a_cbxSelectSubcir->setCurrentText(device);
  auto items = a_listSymPattern->findItems(sym,Qt::MatchExactly);
  if (!items.isEmpty()) a_listSymPattern->setCurrentItem(items.at(0));
  //a_listSymPattern->setCurrentText(sym);

  if (!pin_list.isEmpty()) {
    QStringList pins = pin_list.split(";");
    for(int i = 0; i < pins.count(); i++) {
      QTableWidgetItem *itm = a_tbwPinsTable->item(i,1);
      if (itm != nullptr) {
        itm->setText(pins.at(i));
      }
    }
  }

  a_listSymPattern->setMinimumWidth(0.75*a_tbwPinsTable->minimumWidth());
  a_symbol->setMinimumWidth(0.75*a_tbwPinsTable->minimumWidth());

  a_btnApply->setEnabled(false);
  connect(a_rbAutoSymbol,SIGNAL(toggled(bool)),this,SLOT(slotSetSymbol()));
  connect(a_rbSymFromTemplate,SIGNAL(toggled(bool)),this,SLOT(slotSetSymbol()));
  connect(a_rbUserSym,SIGNAL(toggled(bool)),this,SLOT(slotSetSymbol()));
  connect(a_edtLibPath,SIGNAL(textChanged(QString)),this,SLOT(slotChanged()));
  connect(a_edtParams,SIGNAL(textChanged(QString)),this,SLOT(slotChanged()));
  connect(a_tbwPinsTable,SIGNAL(cellChanged(int,int)),this,SLOT(slotChanged()));
  connect(a_listSymPattern,SIGNAL(currentIndexChanged(int)),this,SLOT(slotChanged()));
  connect(a_cbxSelectSubcir,SIGNAL(currentIndexChanged(int)),this,SLOT(slotChanged()));
  connect(a_edtSymFile,SIGNAL(textChanged(QString)),this,SLOT(slotSetSymbol()));
  connect(a_edtSymFile,SIGNAL(textChanged(QString)),this,SLOT(slotChanged()));

}

void SpiceLibCompDialog::slotFillSubcirComboBox()
{
  QString libfile = a_edtLibPath->text();
  if (!QFile::exists(libfile)) return;
  int  r = parseLibFile(libfile);
  a_libError = false;
  if (r != noError) {
    a_libError = true;
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

  a_cbxSelectSubcir->blockSignals(true);
  a_cbxSelectSubcir->clear();
  for(const auto &key: a_subcirPins.keys()) {
    a_cbxSelectSubcir->addItem(key);
  }
  a_cbxSelectSubcir->blockSignals(false);
  slotFillPinsTable();
}

void SpiceLibCompDialog::slotFillPinsTable()
{
  QString subcir_name = a_cbxSelectSubcir->currentText();
  if (a_subcirPins.find(subcir_name) == a_subcirPins.end()) return;
  QStringList pins = a_subcirPins[subcir_name];
  a_tbwPinsTable->clearContents();
  a_tbwPinsTable->setRowCount(pins.count());
  for (int i = 0; i < pins.count(); i++) {
    QTableWidgetItem *itm1 = new QTableWidgetItem(pins.at(i));
    a_tbwPinsTable->setItem(i,0,itm1);
    QTableWidgetItem *itm2 = new QTableWidgetItem("NC");
    a_tbwPinsTable->setItem(i,1,itm2);
  }

  a_edtSPICE->clear();
  a_edtSPICE->setPlainText(a_subcirSPICE[subcir_name]);
}

int SpiceLibCompDialog::parseLibFile(const QString &filename)
{
  if (!QFileInfo::exists(filename)) return false;
  QFile f(filename);
  if (!f.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(this,tr("Error"),tr("Failed to open file: ") + filename);
    return failedOpenFile;
  }

  a_subcirPins.clear();
  a_subcirSPICE.clear();
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
        a_subcirPins[last_subcir].append(pins);
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
      a_subcirPins[subname] = pin_names;
    }
    if (subcir_start) {
      subcir_body += line + "\n";
    }
    if (line.startsWith(".ENDS")) {
      subcir_start = false;
      a_subcirSPICE[subname] = subcir_body;
    }
  }

  f.close();
  if (a_subcirPins.isEmpty()) {
    return noSUBCKT;
  }
  return noError;

}

void SpiceLibCompDialog::slotSetSymbol()
{
  int result = -1;
  if (a_rbAutoSymbol->isChecked()) {
    a_tbwPinsTable->setEnabled(false);
    a_listSymPattern->setEnabled(false);
    a_edtSymFile->setEnabled(false);
    a_btnOpenSym->setEnabled(false);
    QString s1 = "";
    QString s2 = "SpLib";
    a_symbol->setSymbol(s1, s1, s2);
    result = 0;
    a_symbol->setWarning(tr("No symbol loaded"));
    a_symbolPinsCount = 0;
  } else if (a_rbSymFromTemplate->isChecked()) {
    if (a_listSymPattern->currentItem() == nullptr) {
      return; // empty list
    }
    a_tbwPinsTable->setEnabled(true);
    a_listSymPattern->setEnabled(true);
    a_edtSymFile->setEnabled(false);
    a_btnOpenSym->setEnabled(false);
    QString dir_name = QucsSettings.BinDir + "/../share/" QUCS_NAME "/symbols/";
    QString file = dir_name + a_listSymPattern->currentItem()->text() + ".sym";
    result = a_symbol->loadSymFile(file);
    a_symbolPinsCount = a_symbol->getPortsNumber();
  } else if (a_rbUserSym->isChecked()) {
    a_tbwPinsTable->setEnabled(true);
    a_listSymPattern->setEnabled(false);
    a_edtSymFile->setEnabled(true);
    a_btnOpenSym->setEnabled(true);
    if (a_edtSymFile->text().isEmpty()) {
      a_symbolPinsCount = 0;
      return;
    }
    result = a_symbol->loadSymFile(a_edtSymFile->text());
    a_symbolPinsCount = a_symbol->getPortsNumber();
  }

  if (result < 0) {
    QMessageBox::critical(this,tr("Error"),tr("Failed to load symbol file!"));
    return;
  }

  for (int i = 0; i < a_tbwPinsTable->rowCount(); i++) {
    QTableWidgetItem *itm = new QTableWidgetItem("NC");
    a_tbwPinsTable->setItem(i,1,itm);
  }
  a_isChanged = true;
  a_btnApply->setEnabled(true);
}

void SpiceLibCompDialog::slotTableCellDoubleClick()
{
  int r = a_tbwPinsTable->currentRow();
  int c = a_tbwPinsTable->currentColumn();
  if (c == 0) return; // do not edit the first column

  if (a_prev_col >= 0 && a_prev_row >= 0) { // remove combo box from previous cell
    QTableWidgetItem *itm = new QTableWidgetItem("NC");
    a_tbwPinsTable->removeCellWidget(a_prev_row, a_prev_col);
    a_tbwPinsTable->setItem(a_prev_row, a_prev_col, itm);
  }

  QComboBox *cbxSelectPin = new QComboBox;
  cbxSelectPin->addItem("NC");
  for (int i = 1; i <= a_symbolPinsCount; i++) {
    bool pinAssigned = false;
    for(int j = 0; j < a_tbwPinsTable->rowCount(); j++) {
      if (j == r) continue;
      auto itm = a_tbwPinsTable->item(j,1);
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
  a_tbwPinsTable->item(r,c)->setText("");
  a_tbwPinsTable->setCellWidget(r,c,cbxSelectPin);
  connect(cbxSelectPin,SIGNAL(activated(int)),this,SLOT(slotSelectPin()));

  a_prev_col = c; // remebmebr cell with combo box
  a_prev_row = r;
}

void SpiceLibCompDialog::slotSelectPin()
{
  QComboBox *cbxSelectPin = qobject_cast<QComboBox*>(sender());
  QString pin = cbxSelectPin->currentText();
  int r = a_tbwPinsTable->currentRow();
  int c = a_tbwPinsTable->currentColumn();
  QTableWidgetItem *itm = new QTableWidgetItem(pin);
  a_tbwPinsTable->removeCellWidget(r,c);
  a_tbwPinsTable->setItem(r,c,itm);

  a_prev_col = -1; // clear cell index with combo box
  a_prev_row = -1;
}

void SpiceLibCompDialog::slotBtnOpenLib()
{
  QString s = QFileDialog::getOpenFileName(this, tr("Open SPICE library"),
                                           a_lastLibDir,
                                           tr("SPICE files (*.cir +.ckt *.sp *.lib)"));
  if (!s.isEmpty()) {
    QFileInfo inf(s);
    a_lastLibDir = inf.absoluteDir().path();
    a_edtLibPath->setText(s);
  }
}

void SpiceLibCompDialog::slotBtnOpenSym()
{
  QString s = QFileDialog::getOpenFileName(this, tr("Open symbol file"),
                                           a_lastSymbolDir,
                                           tr("Schematic symbol (*.sym)"));
  if (!s.isEmpty()) {
    QFileInfo inf(s);
    a_lastSymbolDir = inf.absoluteDir().path();
    a_edtSymFile->setText(s);
  }
}

bool SpiceLibCompDialog::setCompProps()
{
  QStringList pins;
  QString pin_string;
  if (a_rbAutoSymbol->isChecked()) {
    pin_string = "";
  } else {
    for (int i = 0; i < a_tbwPinsTable->rowCount(); i++) {
      QTableWidgetItem *itm = a_tbwPinsTable->item(i,1);
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

  if (a_rbUserSym->isChecked() &&
      !QFileInfo::exists(a_edtSymFile->text())) {
    QMessageBox::warning(this,tr("Warning"),tr("Set a valid symbol file name"));
    return false;
  }

  QString  sch_dir = a_Doc->getFileInfo().absoluteDir().path();
  QString libpath = a_edtLibPath->text();
  QString sympath = a_edtSymFile->text();
  if (libpath.startsWith(sch_dir)) {
    libpath = QDir(sch_dir).relativeFilePath(libpath);
  }
  if (sympath.startsWith(sch_dir)) {
    sympath = QDir(sch_dir).relativeFilePath(sympath);
  }

  auto pp = a_comp->Props.begin();
  (*pp)->Value = libpath;
  (*pp)->display = a_chbShowLib->isChecked();
  pp++;
  (*pp)->Value = a_cbxSelectSubcir->currentText();
  (*pp)->display = a_chbShowModel->isChecked();
  pp++;
  if (a_rbAutoSymbol->isChecked()) {
    (*pp)->Value = "auto";
  } else if (a_rbSymFromTemplate->isChecked()) {
    (*pp)->Value = a_listSymPattern->currentItem()->text();
  } else if (a_rbUserSym->isChecked()) {
    (*pp)->Value = sympath;
  }
  pp++;
  (*pp)->Value = a_edtParams->text();
  (*pp)->display = a_chbShowParams->isChecked();
  pp++;
  (*pp)->Value = QString(pin_string);
  a_Doc->recreateComponent(a_comp);
  a_Doc->viewport()->repaint();
  a_Doc->setChanged(true,true);
  return true;
}

void SpiceLibCompDialog::slotBtnApply()
{
  if (a_isChanged) {
    if (a_libError) {
      QMessageBox::critical(this,tr("Error"),
                            tr("There were library file parse error! Cannot apply changes."));
      return;
    }
    if (setCompProps()) {
      a_isChanged = false;
      a_btnApply->setEnabled(false);
    }
  }
}

void SpiceLibCompDialog::slotBtnOK()
{
  if (a_isChanged) {
    if (a_libError) {
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
  a_isChanged = true;
  a_btnApply->setEnabled(true);
}
