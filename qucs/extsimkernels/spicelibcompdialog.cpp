#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "misc.h"
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
  btnOpenLib = new QPushButton(tr("Open"));
  connect(btnOpenLib,SIGNAL(clicked(bool)),this,SLOT(slotBtnOpenLib()));

  QLabel *lblDevice = new QLabel("Subcircuit:");
  cbxSelectSubcir = new QComboBox;

  QLabel *lblPattern = new QLabel("Symbol pattern");
  cbxSymPattern = new QComboBox;

  symbol = new SymbolWidget;
  symbol->setAcceptDrops(false);

  tbwPinsTable = new QTableWidget;
  tbwPinsTable->setColumnCount(2);
  tbwPinsTable->setRowCount(100);
  QStringList lbl_cols;
  lbl_cols<<"Symbol pin"<<"Subcircuit pin";
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

  this->setLayout(top);

}

void SpiceLibCompDialog::fillSubcirComboBox()
{
  if (!QFileInfo::exists(edtLibPath->text())) return;
}

void SpiceLibCompDialog::slotBtnOpenLib()
{

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
