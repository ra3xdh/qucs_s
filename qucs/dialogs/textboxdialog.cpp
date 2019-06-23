/***************************************************************************
                             textboxdialog.cpp
                            ------------------
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/



#include "textboxdialog.h"

/*!
  \file textboxdialog.cpp
  \brief Implementation of the TextBoxDialog class
*/

/*!
 * \brief TextBoxDialog::TextBoxDialog class constructor
 * \param window_title[in] Title of the dialog window.
 * \param pc[in] Component that need to be edit.
 * \param parent[in] Parent object.
 */
TextBoxDialog::TextBoxDialog(const char* window_title, Component *pc, QWidget *parent) :
    QDialog(parent)
{
    comp = pc;

    setWindowTitle(tr(window_title));
    QLabel* lblName = new QLabel(tr("Component: ")+comp->Description);
    edtCode = new QTextEdit(this);
    edtCode->insertPlainText(comp->Props.at(0)->Value);

    btnApply = new QPushButton(tr("Apply"));
    connect(btnApply,SIGNAL(clicked()),this,SLOT(slotApply()));
    btnCancel = new QPushButton(tr("Cancel"));
    connect(btnCancel,SIGNAL(clicked()),this,SLOT(slotCancel()));
    btnOK = new QPushButton(tr("OK"));
    connect(btnOK,SIGNAL(clicked()),this,SLOT(slotOK()));

    QVBoxLayout *vl1 = new QVBoxLayout;
    QVBoxLayout *vl2 = new QVBoxLayout;
    QHBoxLayout *hl1 = new QHBoxLayout;

    vl1->addWidget(lblName);
    QGroupBox *gpb1 = new QGroupBox(tr("Editor"));
    vl2->addWidget(edtCode);
    gpb1->setLayout(vl2);
    vl1->addWidget(gpb1);

    hl1->addWidget(btnOK);
    hl1->addWidget(btnApply);
    hl1->addWidget(btnCancel);
    vl1->addLayout(hl1);

    this->setLayout(vl1);
    this->setWindowTitle(tr(window_title));
}

/*!
 * \brief CustomSimDialog::slotApply Aplly changes of component properties.
 */
void TextBoxDialog::slotApply()
{
    comp->Props.at(0)->Value = edtCode->document()->toPlainText();
}

/*!
 * \brief CustomSimDialog::slotOK Apply changes and exit.
 */
void TextBoxDialog::slotOK()
{
    slotApply();
    accept();
}

/*!
 * \brief CustomSimDialog::slotCancel Close dialog without apply chages.
 */
void TextBoxDialog::slotCancel()
{
    reject();
}
