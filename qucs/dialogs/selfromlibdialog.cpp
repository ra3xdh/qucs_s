#include <QListWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPlainTextEdit>

#include "main.h"

#include "component.h"
#include "qucslib_common.h"

#include "selfromlibdialog.h"

SelFromLibDialog::SelFromLibDialog(Component *c)
{
  comp = c;

  QLabel *lblLib = new QLabel(tr("Library"));
  QLabel *lblComp =  new QLabel(tr("Component"));
  QLabel *lblDescr = new QLabel(tr("Description"));
  cbxLib = new QComboBox;
  lstComps = new QListWidget;
  edtDescr = new QPlainTextEdit;
  edtDescr->setReadOnly(true);

  btnOK = new QPushButton(tr("OK"));
  connect(btnOK,SIGNAL(clicked(bool)),this,SLOT(slotOK()));
  btnApply = new QPushButton(tr("Apply"));
  connect(btnApply,SIGNAL(clicked(bool)),this,SLOT(slotApply()));
  btnCancel = new QPushButton(tr("Cancel"));
  connect(btnCancel,SIGNAL(clicked(bool)),this,SLOT(reject()));

  QVBoxLayout *top = new QVBoxLayout;
  top->addWidget(lblLib);
  top->addWidget(cbxLib);
  top->addWidget(lblComp);
  top->addWidget(lstComps);
  top->addWidget(lblDescr);
  top->addWidget(edtDescr);

  QHBoxLayout *hl1 = new QHBoxLayout;
  hl1->addWidget(btnOK);
  hl1->addWidget(btnApply);
  hl1->addWidget(btnCancel);
  top->addLayout(hl1);

  this->setLayout(top);
}


void SelFromLibDialog::slotApply()
{

}

void SelFromLibDialog::slotOK()
{
  slotApply();
  accept();
}
