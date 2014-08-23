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

#include <qlayout.h>
#include <q3hbox.h>
#include <qpushbutton.h>
#include <q3textedit.h>
//Added by qt3to4:
#include <Q3VBoxLayout>


	HelpDialog::HelpDialog(QWidget *parent)
: QDialog(parent, 0, false, Qt::WDestructiveClose)
{
	setCaption("Resistance color codes help");


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
	resize(250, 230);

	vLayout = new Q3VBoxLayout(this);

	Text = new Q3TextEdit(s, QString::null, this);
	Text->setTextFormat(Qt::PlainText);
	Text->setReadOnly(true);
	Text->setMinimumSize(200,200);
	vLayout->addWidget(Text);

	Q3HBox *h = new Q3HBox(this);
	vLayout->addWidget(h);

	h->setStretchFactor(new QWidget(h),5); // stretchable placeholder

	QPushButton *ButtonClose = new QPushButton(tr("Close"), h);
	connect(ButtonClose, SIGNAL(clicked()), SLOT(slotClose()));
	ButtonClose->setFocus();

	h->setStretchFactor(new QWidget(h),5); // stretchable placeholder
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
