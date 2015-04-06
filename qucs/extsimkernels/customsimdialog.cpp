#include "customsimdialog.h"

CustomSimDialog::CustomSimDialog(SpiceCustomSim *pc, Schematic *sch, QWidget *parent) :
    QDialog(parent)
{
    comp = pc;
    Sch = sch;

    edtCode = new QTextEdit(this);
    edtCode->insertPlainText(comp->Props.at(0)->Value);
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
    comp->Props.at(0)->Value = edtCode->document()->toPlainText();
}

void CustomSimDialog::slotOK()
{
    slotApply();
    accept();
}

void CustomSimDialog::slotCancel()
{
    reject();
}
