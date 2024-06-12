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
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QFileDialog>

#include "symbolwidget.h"
#include "spicelibcompdialog.h"


SpiceLibCompDialog::SpiceLibCompDialog(Component *pc, QWidget* parent) : QDialog{parent}
{
  comp = pc;
  QString file = comp->Props.at(0)->Value;
  if (!file.isEmpty()) {
    file = misc::properAbsFileName(file);
  }
  QString device = comp->Props.at(1)->Value;
  QString sym = comp->Props.at(2)->Value;

  QLabel *lblLibfile = new QLabel("SPICE library:");
  edtLibPath = new QLineEdit;
  edtLibPath->setText(file);
  connect(edtLibPath,SIGNAL(textChanged(QString)),this,SLOT(slotFillSubcirComboBox()));
  btnOpenLib = new QPushButton(tr("Open"));
  connect(btnOpenLib,SIGNAL(clicked(bool)),this,SLOT(slotBtnOpenLib()));

  QLabel *lblDevice = new QLabel("Subcircuit:");
  cbxSelectSubcir = new QComboBox;
  connect(cbxSelectSubcir,SIGNAL(currentIndexChanged(int)),this,SLOT(slotFillPinsTable()));

  QLabel *lblPattern = new QLabel("Symbol pattern");
  cbxSymPattern = new QComboBox;
  QStringList lst_patterns;
  misc::getSymbolPatternsList(lst_patterns);
  cbxSymPattern->addItems(lst_patterns);
  connect(cbxSymPattern,SIGNAL(currentIndexChanged(int)),this,SLOT(slotSetSymbol()));

  symbol = new SymbolWidget;
  symbol->disableDragNDrop();

  tbwPinsTable = new QTableWidget;
  tbwPinsTable->setColumnCount(2);
  tbwPinsTable->setRowCount(100);
  QStringList lbl_cols;
  lbl_cols<<"Subcircuit pin"<<"Symbol pin";
  tbwPinsTable->setHorizontalHeaderLabels(lbl_cols);

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
  QHBoxLayout *l5 = new QHBoxLayout;
  l5->addWidget(lblDevice);
  l5->addWidget(cbxSelectSubcir);
  top->addLayout(l5);
  QHBoxLayout *l6 = new QHBoxLayout;
  l6->addWidget(lblPattern);
  l6->addWidget(cbxSymPattern);
  top->addLayout(l6);
  QHBoxLayout *l3 = new QHBoxLayout;
  l3->addWidget(tbwPinsTable);
  l3->addWidget(symbol);
  top->addLayout(l3,3);
  QHBoxLayout *l4 = new QHBoxLayout;
  l4->addWidget(btnOK);
  l4->addWidget(btnApply);
  l4->addWidget(btnCancel);
  l4->addStretch();
  top->addLayout(l4);

  this->slotSetSymbol();
  this->setLayout(top);

}

void SpiceLibCompDialog::slotFillSubcirComboBox()
{
  if (!parseLibFile(edtLibPath->text())) {
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
  for (int i = 0; i < pins.count(); i++) {
    QTableWidgetItem *itm = new QTableWidgetItem(pins.at(i));
    tbwPinsTable->setItem(i,0,itm);
  }
}

bool SpiceLibCompDialog::parseLibFile(const QString &filename)
{
  if (!QFileInfo::exists(filename)) return false;
  QFile f(filename);
  if (!f.open(QIODevice::ReadOnly)) {
    return false;
  }

  subcirPins.clear();
  QTextStream ts(&f);

  while (!ts.atEnd()) {
    QString line = ts.readLine();
    line = line.trimmed();
    line = line.toUpper();
    if (line.startsWith(".SUBCKT")) {
      QStringList pin_names;
      QString subname;
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
  }

  f.close();
  if (subcirPins.isEmpty()) {
    return false;
  }
  return true;

}

void SpiceLibCompDialog::slotSetSymbol()
{
  QString dir_name = QucsSettings.BinDir + "/../share/" QUCS_NAME "/symbols/";
  QString file = dir_name + cbxSymPattern->currentText() + ".sym";
  symbol->loadSymFile(file);
}

void SpiceLibCompDialog::slotBtnOpenLib()
{
  QString s = QFileDialog::getOpenFileName(this, tr("Open SPICE library"),
                                           QDir::homePath(),
                                           tr("SPICE files (*.cir +.ckt *.sp *.lib)"));
  edtLibPath->setText(s);
}

void SpiceLibCompDialog::slotBtnApply()
{

}

void SpiceLibCompDialog::slotBtnOK()
{
  slotBtnApply();
  accept();
}

void SpiceLibCompDialog::slotBtnCancel()
{
  reject();
}
