#include <QPlainTextEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "component.h"

#include "fillfromspicedialog.h"

fillFromSpiceDialog::fillFromSpiceDialog(Component *pc, QWidget *w)
    : QDialog(w)
{
  Comp = pc;

  edtModel = new QPlainTextEdit;
  QLabel *lblModel = new QLabel(tr("Insert .MODEL text here"));
  btnOK = new QPushButton(tr("OK"));
  connect(btnOK,SIGNAL(clicked(bool)),this,SLOT(slotOK()));
  btnCancel = new QPushButton(tr("Cancel"));
  connect(btnCancel,SIGNAL(clicked(bool)),this,SLOT(reject()));
  chbNumNotation = new QCheckBox(tr("Convert number notation"));

  QVBoxLayout *top = new QVBoxLayout;
  top->addWidget(lblModel);
  top->addWidget(edtModel,4);
  top->addWidget(chbNumNotation);
  QHBoxLayout *l1 = new QHBoxLayout;
  l1->addWidget(btnOK);
  l1->addWidget(btnCancel);
  l1->addStretch();
  top->addLayout(l1);
  this->setLayout(top);
  this->setMinimumWidth(400);

}

int fillFromSpiceDialog::parseModelcard()
{
  return noError;
}

void fillFromSpiceDialog::fillCompProps()
{

}

void fillFromSpiceDialog::slotOK()
{
  accept();
}
