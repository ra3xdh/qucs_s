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



#include "main.h"
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
 */
CustomSimDialog::CustomSimDialog(SpiceCustomSim *pc, Schematic *sch) :
    QDialog(sch),
    a_isXyceScr(false),
    a_isChanged(false),
    a_comp(pc),
    a_schematic(sch),
    a_edtCode(new QTextEdit(this)),
    a_checkCode(new QCheckBox(tr("display in schematic"), this)),
    a_btnOK(new QPushButton(tr("OK"))),
    a_btnApply(new QPushButton(tr("Apply"))),
    a_btnCancel(new QPushButton(tr("Cancel"))),
    a_btnPlotAll(new QPushButton(tr("Find all variables"))),
    a_btnFindOutputs(new QPushButton(tr("Find all outputs"))),
    a_edtVars(new QLineEdit(a_comp->Props.at(1)->Value)),
    a_edtOutputs(new QLineEdit(a_comp->Props.at(2)->Value))
{
    resize(640, 480);

    setWindowTitle(tr("Edit SPICE code"));
    QLabel* lblName = new QLabel(tr("Component: ")+a_comp->Description);

    a_edtCode->document()->setDefaultFont(QucsSettings.textFont);
    a_edtCode->setWordWrapMode(QTextOption::NoWrap);
    a_edtCode->insertPlainText(a_comp->Props.at(0)->Value);
    connect(a_edtCode, SIGNAL(textChanged()), this, SLOT(slotChanged()));

    a_checkCode->setChecked(a_comp->Props.at(0)->display);
    connect(a_checkCode, SIGNAL(stateChanged(int)), this, SLOT(slotChanged()));

    QLabel* lblVars = new QLabel(tr("Variables to plot (semicolon separated)"));
    connect(a_edtVars, SIGNAL(textChanged(const QString&)), this, SLOT(slotChanged()));

    QLabel* lblOut = new QLabel(tr("Extra outputs (semicolon separated; raw-SPICE or XYCE-STD or scalars print format)"));
    connect(a_edtOutputs, SIGNAL(textChanged(const QString&)), this, SLOT(slotChanged()));

    connect(a_btnApply,SIGNAL(clicked()),this,SLOT(slotApply()));
    connect(a_btnCancel,SIGNAL(clicked()),this,SLOT(slotCancel()));
    connect(a_btnOK,SIGNAL(clicked()),this,SLOT(slotOK()));
    connect(a_btnPlotAll,SIGNAL(clicked()),this,SLOT(slotFindVars()));
    connect(a_btnFindOutputs,SIGNAL(clicked()),this,SLOT(slotFindOutputs()));

    QVBoxLayout *vl1 = new QVBoxLayout;
    QVBoxLayout *vl2 = new QVBoxLayout;
    QHBoxLayout *hl1 = new QHBoxLayout;

    vl1->addWidget(lblName);
    QGroupBox *gpb1 = new QGroupBox(tr("SPICE code editor"));
    vl2->addWidget(a_edtCode);
    vl2->addWidget(a_checkCode);
    gpb1->setLayout(vl2);
    vl1->addWidget(gpb1);
    vl1->addWidget(lblVars);
    vl1->addWidget(a_edtVars);
    vl1->addWidget(a_btnPlotAll);
    vl1->addWidget(lblOut);
    vl1->addWidget(a_edtOutputs);
    vl1->addWidget(a_btnFindOutputs);

    hl1->addWidget(a_btnOK);
    hl1->addWidget(a_btnApply);
    hl1->addWidget(a_btnCancel);
    vl1->addLayout(hl1);

    this->setLayout(vl1);
    this->setWindowTitle(tr("Edit SPICE code"));

    if (a_comp->Model == ".XYCESCR") {
        lblVars->setEnabled(false);
        a_edtVars->setEnabled(false);
        a_btnPlotAll->setEnabled(false);
        a_isXyceScr = true;
    } else if (a_comp->Model == "INCLSCR") {
        lblVars->setEnabled(false);
        a_edtVars->setEnabled(false);
        a_btnPlotAll->setEnabled(false);
        a_btnFindOutputs->setEnabled(false);
        lblOut->setEnabled(false);
    } else a_isXyceScr = false;
}

/*!
 * \brief CustomSimDialog::slotChanged Set a_isChanged state.
 */
void CustomSimDialog::slotChanged()
{
    a_isChanged = true;
}
/*!
 * \brief CustomSimDialog::slotApply Apply changes of component properties.
 */
void CustomSimDialog::slotApply()
{
    if ( a_isChanged ) {
        a_edtVars->setText(a_edtVars->text().remove(' '));
        a_edtOutputs->setText(a_edtOutputs->text().remove(' '));

        a_comp->Props.at(0)->Value = a_edtCode->document()->toPlainText();
        a_comp->Props.at(0)->display = a_checkCode->isChecked();
        a_comp->Props.at(1)->Value = a_edtVars->text();
        a_comp->Props.at(2)->Value = a_edtOutputs->text();

        a_schematic->recreateComponent(a_comp);
        a_schematic->viewport()->repaint();

        a_isChanged = false;
    }
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
 * \brief CustomSimDialog::slotCancel Close dialog without applying changes.
 */
void CustomSimDialog::slotCancel()
{
    reject();
}

/*!
 * \brief CustomSimDialog::slotFindVars Auto-find used variables and nodes
 *        in simulation script and place them into a_edtVars line edit.
 */
void CustomSimDialog::slotFindVars()
{
    QStringList vars;
    for(Node *pn = a_schematic->a_DocNodes.first(); pn != 0; pn = a_schematic->a_DocNodes.next()) {
      if(pn->Label != 0) {
          if (!vars.contains(pn->Label->Name)) {
              vars.append(pn->Label->Name);
          }
      }
    }
    for(Wire *pw = a_schematic->a_DocWires.first(); pw != 0; pw = a_schematic->a_DocWires.next()) {
      if(pw->Label != 0) {
          if (!vars.contains(pw->Label->Name)) {
              vars.append(pw->Label->Name);
          }
      }
    }

    for(Component *pc=a_schematic->a_DocComps.first();pc!=0;pc=a_schematic->a_DocComps.next()) {
        if(!pc->Category.compare("Probes")) {
            if (!vars.contains(pc->getProbeVariable())) {
                vars.append(pc->getProbeVariable());
            }
        }
    }

    for(QStringList::iterator it = vars.begin();it != vars.end(); it++) {
        if (!(it->endsWith("#branch"))) *it=QStringLiteral("V(%1)").arg(*it);
    }



    QStringList strings = a_edtCode->toPlainText().split('\n');
    QRegularExpression let_pattern("^\\s*let\\s+[A-Za-z].*=.+");

    for (const QString& line : strings) {
        if (let_pattern.match(line).hasMatch()) {
            QString var = line.section('=',0,0);
            var.remove("let ");
            var.remove(' ');
            vars.append(var);
        }
    }

    a_edtVars->setText(vars.join(";"));
}

void CustomSimDialog::slotFindOutputs()
{
    QStringList outps;
    QString outp;
    QStringList strings = a_edtCode->toPlainText().split('\n');
    if (a_isXyceScr) {
        QRegularExpression print_ex("^\\s*\\.print\\s.*", QRegularExpression::CaseInsensitiveOption);
        QRegularExpression file_ex("\\s*file\\s*=\\s*", QRegularExpression::CaseInsensitiveOption);
        for (const QString& line : strings) {
            if (print_ex.match(line).hasMatch()) {
                //file_ex.setCaseSensitivity(Qt::CaseInsensitive);
                int p = line.indexOf(file_ex);
                p = line.indexOf('=',p);
                int l = line.size()-(p+1);
                QString sub = line.right(l);
                outp = sub.section(" ",0,0,QString::SectionSkipEmpty);
                if ( !outp.isEmpty() ) if ( !outps.contains(outp) ) outps.append(outp);
            }
        }
    } else {
        QRegularExpression write_ex("^\\s*write\\s.*");
        write_ex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
        QRegularExpression print_rx("^\\s*print\\s.*>.+");
        print_rx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
        for (const QString& line : strings) {
            if (write_ex.match(line).hasMatch()) {
                outp = line.section(QRegularExpression("\\s"),1,1,QString::SectionSkipEmpty);
                if ( !outp.isEmpty() ) if ( !outps.contains(outp) ) outps.append(outp);
            }
            else if ( print_rx.match(line).hasMatch() ) {
                outp = line.section('>', 1, 1, QString::SectionSkipEmpty).trimmed();
                if ( !outp.isEmpty() ) if ( !outps.contains(outp) ) outps.append(outp);
            }
        }
    }

    a_edtOutputs->setText(outps.join(";"));
}
