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

#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QValidator>
#include <QPushButton>
#include <QMessageBox>
#include <QColorDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "misc.h"


GraphicTextDialog::GraphicTextDialog(QWidget *parent, const char *name)
                                  : QDialog(parent)
{
  Q_UNUSED(name);
  setWindowTitle(tr("Edit Text Properties"));

  vert = new QVBoxLayout(this);
  vert->setMargin(3);
  vert->setSpacing(3);

  vert->addWidget(
        new QLabel(tr("Use LaTeX style for special characters, e.g. \\tau")+
		   "\n"+
		   tr("Use _{..} and ^{..} for sub- and super-positions."),
		   this));

  text = new QTextEdit(this);
  text->toPlainText();
  text->setWordWrapMode(QTextOption::NoWrap);
  text->setMinimumSize(350,150);
  vert->addWidget(text);

  QWidget *h1 = new QWidget(this);
  QHBoxLayout *h1Layout = new QHBoxLayout();
  h1Layout->setSpacing(5);
  h1->setLayout(h1Layout);
  vert->addWidget(h1);

  QWidget *h3 = new QWidget(this);
  QHBoxLayout *h3Layout = new QHBoxLayout();
  h3Layout->setSpacing(5);
  h3->setLayout(h3Layout);
  vert->addWidget(h3);

  // first => activated by pressing RETURN
  QPushButton *ButtOK = new QPushButton(tr("&OK"));
  h3Layout->addWidget(ButtOK);
  connect(ButtOK, SIGNAL(clicked()), SLOT(slotOkButton()));
  QPushButton *ButtCancel = new QPushButton(tr("&Cancel"));
  h3Layout->addWidget(ButtCancel);
  connect(ButtCancel, SIGNAL(clicked()), SLOT(reject()));

  QLabel *tc = new QLabel(tr("Text color: "));
  ColorButt = new QPushButton("        ");
  h1Layout->addWidget(tc);
  h1Layout->addWidget(ColorButt);
  misc::setPickerColor(ColorButt,QColor(0,0,0));
  connect(ColorButt, SIGNAL(clicked()), SLOT(slotSetColor()));

  QWidget *place1 = new QWidget(h1); // stretchable placeholder
  h1Layout->setStretchFactor(place1,5);

  val50 = new QIntValidator(1, 50, this);
  QLabel *ts = new QLabel(tr("  Text size: "));
  h1Layout->addWidget(ts);
  TextSize = new QLineEdit(this);
  TextSize->setValidator(val50);
  TextSize->setMaximumWidth(35);
  TextSize->setText("12");
  h1Layout->addWidget(TextSize);

  val360 = new QIntValidator(0, 359, this);
  QLabel *ra = new QLabel(tr("  Rotation angle: "));
  h1Layout->addWidget(ra);
  Angle = new QLineEdit(this);
  Angle->setValidator(val360);
  Angle->setMaximumWidth(35);
  Angle->setText("0");
  h1Layout->addWidget(Angle);

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
  QColor c = QColorDialog::getColor(misc::getWidgetBackgroundColor(ColorButt),this);
  if(c.isValid()) misc::setPickerColor(ColorButt,c);
}

// --------------------------------------------------------------------------
void GraphicTextDialog::slotOkButton()
{
  if(text->toPlainText().size() < 1) {
    QMessageBox::critical(this, tr("Error"), tr("The text must not be empty!"));
    return;
  }

  accept();
}
