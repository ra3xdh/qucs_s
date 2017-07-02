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
  \file customsimdialog.cpp
  \brief Implementation of the CustomSimDialog class
*/

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

    setWindowTitle(tr("Edit SPICE code"));
    QLabel* lblName = new QLabel(tr("Component: ")+comp->Description);
    edtCode = new QTextEdit(this);
    edtCode->insertPlainText(comp->Props.at(0)->Value);

    QLabel* lblVars = new QLabel(tr("Variables to plot (semicolon separated)"));
    edtVars = new QLineEdit(comp->Props.at(1)->Value);

    QLabel* lblOut = new QLabel(tr("Extra outputs (semicolon separated; raw-SPICE or XYCE-STD format)"));
    edtOutputs = new QLineEdit(comp->Props.at(2)->Value);

    btnApply = new QPushButton(tr("Apply"));
    connect(btnApply,SIGNAL(clicked()),this,SLOT(slotApply()));
    btnCancel = new QPushButton(tr("Cancel"));
    connect(btnCancel,SIGNAL(clicked()),this,SLOT(slotCancel()));
    btnOK = new QPushButton(tr("OK"));
    connect(btnOK,SIGNAL(clicked()),this,SLOT(slotOK()));
    btnPlotAll = new QPushButton(tr("Find all variables"));
    connect(btnPlotAll,SIGNAL(clicked()),this,SLOT(slotFindVars()));
    btnFindOutputs = new QPushButton(tr("Find all outputs"));
    connect(btnFindOutputs,SIGNAL(clicked()),this,SLOT(slotFindOutputs()));

    QVBoxLayout *vl1 = new QVBoxLayout;
    QVBoxLayout *vl2 = new QVBoxLayout;
    QHBoxLayout *hl1 = new QHBoxLayout;

    vl1->addWidget(lblName);
    QGroupBox *gpb1 = new QGroupBox(tr("SPICE code editor"));
    vl2->addWidget(edtCode);
    gpb1->setLayout(vl2);
    vl1->addWidget(gpb1);
    vl1->addWidget(lblVars);
    vl1->addWidget(edtVars);
    vl1->addWidget(btnPlotAll);
    vl1->addWidget(lblOut);
    vl1->addWidget(edtOutputs);
    vl1->addWidget(btnFindOutputs);

    hl1->addWidget(btnOK);
    hl1->addWidget(btnApply);
    hl1->addWidget(btnCancel);
    vl1->addLayout(hl1);

    this->setLayout(vl1);
    this->setWindowTitle(tr("Edit SPICE code"));

    if (comp->Model == ".XYCESCR") {
        lblVars->setEnabled(false);
        edtVars->setEnabled(false);
        btnPlotAll->setEnabled(false);
        isXyceScr = true;
    } else if (comp->Model == "INCLSCR") {
        lblVars->setEnabled(false);
        edtVars->setEnabled(false);
        btnPlotAll->setEnabled(false);
        btnFindOutputs->setEnabled(false);
        lblOut->setEnabled(false);
    } else isXyceScr = false;
}

/*!
 * \brief CustomSimDialog::slotApply Aplly changes of component properties.
 */
void CustomSimDialog::slotApply()
{
    comp->Props.at(0)->Value = edtCode->document()->toPlainText();
    comp->Props.at(1)->Value = edtVars->text();
    comp->Props.at(2)->Value = edtOutputs->text();
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

    for(QStringList::iterator it = vars.begin();it != vars.end(); it++) {
        if (!(it->endsWith("#branch"))) *it=QString("V(%1)").arg(*it);
    }



    QStringList strings = edtCode->toPlainText().split('\n');
    foreach(QString line,strings) {
        QRegExp let_pattern("^\\s*let\\s+[A-Za-z]+\\w*\\s*\\=\\s*[A-Za-z]+.*$");
        if (let_pattern.exactMatch(line)) {
            QString var = line.section('=',0,0);
            var.remove("let ");
            var.remove(' ');
            vars.append(var);
        }
    }

    edtVars->setText(vars.join(";"));
}

void CustomSimDialog::slotFindOutputs()
{
    QStringList outps;
    QStringList strings = edtCode->toPlainText().split('\n');
    if (isXyceScr) {
        QRegExp print_ex("^\\s*\\.print\\s.*");
        print_ex.setCaseSensitive(false);
        foreach(QString line,strings) {
            if (print_ex.exactMatch(line)) {
                QRegExp file_ex("\\s*file\\s*=\\s*");
                file_ex.setCaseSensitive(false);
                int p = line.indexOf(file_ex);
                p = line.indexOf('=',p);
                int l = line.size()-(p+1);
                QString sub = line.right(l);
                outps.append(sub.section(" ",0,0,QString::SectionSkipEmpty));
            }
        }
    } else {
        QRegExp write_ex("^\\s*write\\s.*");
        write_ex.setCaseSensitive(false);
        foreach(QString line,strings) {
            if (write_ex.exactMatch(line)) {
                outps.append(line.section(QRegExp("\\s"),1,1,QString::SectionSkipEmpty));
            }
        }
    }

    edtOutputs->setText(outps.join(";"));
}
