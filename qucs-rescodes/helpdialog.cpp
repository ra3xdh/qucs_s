/***************************************************************************
  helpdialog.cpp
  ------------------
begin                : Mar 2012
copyright            : (C) 2012 by Sudhakar.M.K
email                : sudhakar.m.kumar@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/



#include "helpdialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTextEdit>


HelpDialog::HelpDialog(QWidget *parent)
: QDialog(parent)
{
	setWindowTitle("Resistance color codes help");


	// --------  set help text into dialog  ------------
	QString s(tr("A program to compute color codes for resistors "
				"and resistance values for color codes. "
				"To obtain the color codes, simply enter the "
				"resistance and tolerance values and press the "
				"\"To Colors\" button. "
				"Alternatively to obtain the resistance, "
				"select the appropriate color bands and "
				"press the \"To Resistance\" button. "
				"The program computes the closest standard "
				"resistor value. You can paste the computed resistor "
			        "in the schematic (by pressing ctrl+v). Have fun!"));


	// --------  create dialog widgets  ------------
	resize(300, 200);

	vLayout = new QVBoxLayout(this);

	Text = new QTextEdit(s, this);
    
/// \todo  "Do this way?"
    Text->setAcceptRichText(false);
	//Text->setTextFormat(Qt::PlainText);
    
	Text->setReadOnly(true);
	Text->setMinimumSize(200,200);
	vLayout->addWidget(Text);

  QHBoxLayout *h = new QHBoxLayout;
	vLayout->addLayout(h);

	QPushButton *ButtonClose = new QPushButton(tr("Close"), this);
  h->addWidget(ButtonClose);

	connect(ButtonClose, SIGNAL(clicked()), SLOT(slotClose()));
	ButtonClose->setFocus();
}

HelpDialog::~HelpDialog()
{
	delete vLayout;
}

// ************************************************************
void HelpDialog::slotClose()
{
	accept();
}
