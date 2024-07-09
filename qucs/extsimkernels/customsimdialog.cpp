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
    QDialog(sch)
{
    comp = pc;
    Sch = sch;

    resize(640, 480);

    setWindowTitle(tr("Edit SPICE code"));
    QLabel* lblName = new QLabel(tr("Component: ")+comp->Description);

    edtCode = new QTextEdit(this);
    edtCode->document()->setDefaultFont(QucsSettings.textFont);
    edtCode->setWordWrapMode(QTextOption::NoWrap);
    edtCode->insertPlainText(comp->Props.at(0)->Value);
    connect(edtCode, SIGNAL(textChanged()), this, SLOT(slotChanged()));

    checkCode = new QCheckBox(tr("display in schematic"), this);
    checkCode->setChecked(comp->Props.at(0)->display);
    connect(checkCode, SIGNAL(stateChanged(int)), this, SLOT(slotChanged()));

    QLabel* lblVars = new QLabel(tr("Variables to plot (semicolon separated)"));
    edtVars = new QLineEdit(comp->Props.at(1)->Value);
    connect(edtVars, SIGNAL(textChanged(const QString&)), this, SLOT(slotChanged()));

    QLabel* lblOut = new QLabel(tr("Extra outputs (semicolon separated; raw-SPICE or XYCE-STD or scalars print format)"));
    edtOutputs = new QLineEdit(comp->Props.at(2)->Value);
    connect(edtOutputs, SIGNAL(textChanged(const QString&)), this, SLOT(slotChanged()));

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
    vl2->addWidget(checkCode);
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
 * \brief CustomSimDialog::slotChanged Set isChanged state.
 */
void CustomSimDialog::slotChanged()
{
    isChanged = true;
}
/*!
 * \brief CustomSimDialog::slotApply Apply changes of component properties.
 */
void CustomSimDialog::slotApply()
{
    if ( isChanged ) {
        edtVars->setText(edtVars->text().remove(' '));
        edtOutputs->setText(edtOutputs->text().remove(' '));

        comp->Props.at(0)->Value = edtCode->document()->toPlainText();
        comp->Props.at(0)->display = checkCode->isChecked();
        comp->Props.at(1)->Value = edtVars->text();
        comp->Props.at(2)->Value = edtOutputs->text();

        Sch->recreateComponent(comp);
        Sch->viewport()->repaint();

        isChanged = false;
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
    const static QRegularExpression let_pattern("^\\s*let\\s+[A-Za-z].*=.+");

    for (const QString& line : strings) {
        if (let_pattern.match(line).hasMatch()) {
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
    QString outp;
    QStringList strings = edtCode->toPlainText().split('\n');
    if (isXyceScr) {
        const static QRegularExpression print_ex("^\\s*\\.print\\s.*", QRegularExpression::CaseInsensitiveOption);
        const static  QRegularExpression file_ex("\\s*file\\s*=\\s*", QRegularExpression::CaseInsensitiveOption);
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
        static QRegularExpression write_ex("^\\s*write\\s.*");
        write_ex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
        static QRegularExpression print_rx("^\\s*print\\s.*>.+");
        print_rx.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
        for (const QString& line : strings) {
            if (write_ex.match(line).hasMatch()) {
            static QRegularExpression exp("\\s");
                outp = line.section(exp,1,1,QString::SectionSkipEmpty);
                if ( !outp.isEmpty() ) if ( !outps.contains(outp) ) outps.append(outp);
            }
            else if ( print_rx.match(line).hasMatch() ) {
                outp = line.section('>', 1, 1, QString::SectionSkipEmpty).trimmed();
                if ( !outp.isEmpty() ) if ( !outps.contains(outp) ) outps.append(outp);
            }
        }
    }

    edtOutputs->setText(outps.join(";"));
}
