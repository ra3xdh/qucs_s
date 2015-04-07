#include "customsimdialog.h"

CustomSimDialog::CustomSimDialog(SpiceCustomSim *pc, Schematic *sch, QWidget *parent) :
    QDialog(parent)
{
    comp = pc;
    Sch = sch;

    QLabel* lblEdt = new QLabel(tr("Spice code editor"));
    edtCode = new QTextEdit(this);
    edtCode->insertPlainText(comp->Props.at(0)->Value);

    QLabel* lblVars = new QLabel(tr("Variables to plot (semicolon separated)"));
    edtVars = new QLineEdit(comp->Props.at(1)->Value);

    btnApply = new QPushButton(tr("Apply"));
    connect(btnApply,SIGNAL(clicked()),this,SLOT(slotApply()));
    btnCancel = new QPushButton(tr("Cancel"));
    connect(btnCancel,SIGNAL(clicked()),this,SLOT(slotCancel()));
    btnOK = new QPushButton(tr("OK"));
    connect(btnOK,SIGNAL(clicked()),this,SLOT(slotOK()));
    btnPlotAll = new QPushButton(tr("Find all variables"));
    connect(btnPlotAll,SIGNAL(clicked()),this,SLOT(slotFindVars()));

    QVBoxLayout *vl1 = new QVBoxLayout;
    QHBoxLayout *hl1 = new QHBoxLayout;

    vl1->addWidget(lblEdt);
    vl1->addWidget(edtCode);
    vl1->addWidget(lblVars);
    vl1->addWidget(edtVars);
    vl1->addWidget(btnPlotAll);

    hl1->addWidget(btnOK);
    hl1->addWidget(btnApply);
    hl1->addWidget(btnCancel);
    vl1->addLayout(hl1);

    this->setLayout(vl1);
}

void CustomSimDialog::slotApply()
{
    comp->Props.at(0)->Value = edtCode->document()->toPlainText();
    comp->Props.at(1)->Value = edtVars->text();
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

void CustomSimDialog::slotFindVars()
{

}
