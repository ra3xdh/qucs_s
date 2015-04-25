/***************************************************************************
                             customdialog.cpp
                             ----------------
    begin                : Mon Apr 13 2015
    copyright            : (C) 2015 by Vadim Kuznetsov
    email                : ra3xdh@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/



#include "customsimdialog.h"
#include "node.h"

/*!
 * \brief CustomSimDialog::CustomSimDialog class constructor
 * \param pc[in] Component that need to be edit.
 * \param sch[in] Schematic on which component presents.
 * \param parent[in] Parent object.
 */
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

/*!
 * \brief CustomSimDialog::slotApply Aplly changes of component properties.
 */
void CustomSimDialog::slotApply()
{
    comp->Props.at(0)->Value = edtCode->document()->toPlainText();
    comp->Props.at(1)->Value = edtVars->text();
}

/*!
 * \brief CustomSimDialog::slotOK Apply changes and exit.
 */
void CustomSimDialog::slotOK()
{
    slotApply();
    accept();
}

/*!
 * \brief CustomSimDialog::slotCancel Close dialog without apply chages.
 */
void CustomSimDialog::slotCancel()
{
    reject();
}

/*!
 * \brief CustomSimDialog::slotFindVars Auto-find used variables and nodes
 *        in simulation script and place them into edtVars line edit.
 */
void CustomSimDialog::slotFindVars()
{
    QStringList vars;
    for(Node *pn = Sch->DocNodes.first(); pn != 0; pn = Sch->DocNodes.next()) {
      if(pn->Label != 0) {
          if (!vars.contains(pn->Label->Name)) {
              vars.append(pn->Label->Name);
          }
      }
    }
    for(Wire *pw = Sch->DocWires.first(); pw != 0; pw = Sch->DocWires.next()) {
      if(pw->Label != 0) {
          if (!vars.contains(pw->Label->Name)) {
              vars.append(pw->Label->Name);
          }
      }
    }

    for(Component *pc=Sch->DocComps.first();pc!=0;pc=Sch->DocComps.next()) {
        if(pc->isProbe) {
            if (!vars.contains(pc->getProbeVariable())) {
                vars.append(pc->getProbeVariable());
            }
        }
    }

    qDebug()<<vars;
    for(QStringList::iterator it = vars.begin();it != vars.end(); it++) {
        if (!(it->endsWith("#branch"))) *it=QString("V(%1)").arg(*it);
    }



    QStringList strings = edtCode->toPlainText().split('\n');
    foreach(QString line,strings) {
        QRegExp let_pattern("^\\s*let\\s+[A-Za-z]+\\w*\\s*\\=\\s*[A-Za-z]+.*$");
        if (let_pattern.exactMatch(line)) {
            qDebug()<<line;
            QString var = line.section('=',0,0);
            var.remove("let ");
            var.remove(' ');
            vars.append(var);
        }
    }

    edtVars->setText(vars.join(";"));
}
