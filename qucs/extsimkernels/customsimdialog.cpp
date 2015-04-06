#include "customsimdialog.h"

CustomSimDialog::CustomSimDialog(QWidget *parent) :
    QDialog(parent)
{
    edtCode = new QTextEdit(this);
    btnApply = new QPushButton(tr("Apply"));
    connect(btnApply,SIGNAL(clicked()),this,SLOT(slotApply()));
    btnCancel = new QPushButton(tr("Cancel"));
    connect(btnCancel,SIGNAL(clicked()),this,SLOT(slotCancel()));
    btnOK = new QPushButton(tr("OK"));
    connect(btnOK,SIGNAL(clicked()),this,SLOT(slotOK()));

    QVBoxLayout *vl1 = new QVBoxLayout;
    QHBoxLayout *hl1 = new QHBoxLayout;

    vl1->addWidget(edtCode);
    hl1->addWidget(btnOK);
    hl1->addWidget(btnApply);
    hl1->addWidget(btnCancel);
    vl1->addLayout(hl1);

    this->setLayout(vl1);
}

void CustomSimDialog::slotApply()
{

}

void CustomSimDialog::slotOK()
{

}

void CustomSimDialog::slotCancel()
{

}
