/***************************************************************************
                          graphictextdialog.cpp
                         -----------------------
    begin                : Wed Nov 26 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "graphictextdialog.h"

#include "qucs.h"

#include <q3hbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <q3textedit.h>
#include <qvalidator.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qcolordialog.h>
//Added by qt3to4:
#include <Q3VBoxLayout>


GraphicTextDialog::GraphicTextDialog(QWidget *parent, const char *name)
                                  : QDialog(parent, name)
{
  setCaption(tr("Edit Text Properties"));

  vert = new Q3VBoxLayout(this);
  vert->setMargin(3);
  vert->setSpacing(3);

  vert->addWidget(
        new QLabel(tr("Use LaTeX style for special characters, e.g. \\tau")+
		   "\n"+
		   tr("Use _{..} and ^{..} for sub- and super-positions."),
		   this));

  text = new Q3TextEdit(this);
  text->setTextFormat(Qt::PlainText);
  text->setWordWrap(Q3TextEdit::NoWrap);
  text->setMinimumSize(350,150);
  vert->addWidget(text);

  Q3HBox *h1 = new Q3HBox(this);
  h1->setSpacing(5);
  vert->addWidget(h1);


  Q3HBox *h2 = new Q3HBox(this);
  h2->setSpacing(5);
  vert->addWidget(h2);

  Q3HBox *h3 = new Q3HBox(this);
  h2->setSpacing(5);
  vert->addWidget(h3);

  // first => activated by pressing RETURN
  QPushButton *ButtOK = new QPushButton(tr("&OK"),h3);
  connect(ButtOK, SIGNAL(clicked()), SLOT(slotOkButton()));
  QPushButton *ButtCancel = new QPushButton(tr("&Cancel"),h3);
  connect(ButtCancel, SIGNAL(clicked()), SLOT(reject()));

  new QLabel(tr("Text color: "), h1);
  ColorButt = new QPushButton("        ",h1);
  ColorButt->setPaletteBackgroundColor(QColor(0,0,0));
  connect(ColorButt, SIGNAL(clicked()), SLOT(slotSetColor()));

  QWidget *place1 = new QWidget(h1); // stretchable placeholder
  h1->setStretchFactor(place1,5);

  val50 = new QIntValidator(1, 50, this);
  new QLabel(tr("Text size: "), h1);
  TextSize = new QLineEdit(h1);
  TextSize->setValidator(val50);
  TextSize->setMaximumWidth(35);
  TextSize->setText("12");

  QWidget *place2 = new QWidget(h2); // stretchable placeholder
  h2->setStretchFactor(place2,5);

  val360 = new QIntValidator(0, 359, this);
  new QLabel(tr("Rotation angle: "), h2);
  Angle = new QLineEdit(h2);
  Angle->setValidator(val360);
  Angle->setMaximumWidth(35);
  Angle->setText("0");

  text->setFocus();
}

GraphicTextDialog::~GraphicTextDialog()
{
  delete vert;
  delete val50;
  delete val360;
}

// --------------------------------------------------------------------------
void GraphicTextDialog::slotSetColor()
{
  QColor c = QColorDialog::getColor(ColorButt->paletteBackgroundColor(),this);
  if(c.isValid()) ColorButt->setPaletteBackgroundColor(c);
}

// --------------------------------------------------------------------------
void GraphicTextDialog::slotOkButton()
{
  if(text->length() < 1) {
    QMessageBox::critical(this, tr("Error"), tr("The text must not be empty!"));
    return;
  }

  accept();
}
