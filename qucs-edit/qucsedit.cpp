/***************************************************************************
                               qucsedit.cpp
                              --------------
    begin                : Mon Nov 17 2003
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "qucsedit.h"

#include <q3textedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <q3hbox.h>
#include <qpushbutton.h>
#include <qfile.h>
#include <q3textstream.h>
#include <qmessagebox.h>
#include <qtoolbutton.h>
#include <qimage.h>
#include <q3filedialog.h>
#include <qfont.h>
//Added by qt3to4:
#include <QPixmap>
#include <Q3VBoxLayout>
#include <QCloseEvent>


QucsEdit::QucsEdit(const QString& FileName_, bool readOnly)
{
  // set application icon
  setIcon (QPixmap(QucsSettings.BitmapDir + "big.qucs.xpm"));
  setCaption("Qucs Editor " PACKAGE_VERSION " - " + tr("File: "));

  Q3VBoxLayout *v = new Q3VBoxLayout(this);

  Q3HBox *h = new Q3HBox(this);
  v->addWidget(h);

  QToolButton *ButtLoad = new QToolButton(h);
  ButtLoad->setIconSet(
	    QIcon((QucsSettings.BitmapDir + "fileopen.png")));
  connect(ButtLoad, SIGNAL(clicked()), SLOT(slotLoad()));

  QToolButton *ButtSave = new QToolButton(h);
  ButtSave->setIconSet(
            QIcon((QucsSettings.BitmapDir + "filesave.png")));
  connect(ButtSave, SIGNAL(clicked()), SLOT(slotSave()));
  ButtSave->setDisabled(readOnly);

  h->setStretchFactor(new QWidget(h),5); // stretchable placeholder
  PosText = new QLabel(tr("Line: %1  -  Column: %2").arg(1).arg(1), h);
  h->setStretchFactor(new QWidget(h),5); // stretchable placeholder

  QPushButton *ButtAbout = new QPushButton(tr("About"),h);
  connect(ButtAbout, SIGNAL(clicked()), SLOT(slotAbout()));

  QPushButton *ButtOK = new QPushButton(tr("Quit"),h);
  connect(ButtOK, SIGNAL(clicked()), SLOT(slotQuit()));
  ButtOK->setFocus();

  // try using same-sized mono-spaced font in the textarea
  QFont fedit = QFont("Courier New");
  fedit.setPointSize(QucsSettings.font.pointSize()-1);
  fedit.setStyleHint(QFont::Courier);
  fedit.setFixedPitch(true);

  text = new Q3TextEdit(this);
  text->setTextFormat(Qt::PlainText);
  text->setReadOnly(readOnly);
  text->setWordWrap(Q3TextEdit::NoWrap);
  text->setMinimumSize(300,200);
  text->setFont(fedit);
  text->setCurrentFont(fedit);
  v->addWidget(text);
  connect(text, SIGNAL(cursorPositionChanged(int, int)),
          SLOT(slotPrintCursorPosition(int, int)));

  // .................................................
  loadFile(FileName_);
}

QucsEdit::~QucsEdit()
{
}

// ************************************************************
void QucsEdit::slotPrintCursorPosition(int Para, int Pos)
{
  PosText->setText(tr("Line: %1  -  Column: %2").arg(Para+1).arg(Pos+1));
}

// ************************************************************
void QucsEdit::slotAbout()
{
  QMessageBox::about(this, tr("About..."),
    "QucsEdit Version " PACKAGE_VERSION+
    tr("\nVery simple text editor for Qucs\n")+
    tr("Copyright (C) 2004, 2005 by Michael Margraf\n")+
    "\nThis is free software; see the source for copying conditions."
    "\nThere is NO warranty; not even for MERCHANTABILITY or "
    "\nFITNESS FOR A PARTICULAR PURPOSE.");
}

// ************************************************************
void QucsEdit::slotLoad()
{
  static QString lastDir;  // to remember last directory and file

  QString s = Q3FileDialog::getOpenFileName(
    lastDir.isEmpty() ? QString(".") : lastDir,
    "*", this, "", tr("Enter a Filename"));
  if(s.isEmpty()) return;
  lastDir = s;   // remember last directory and file
  if(!closeFile()) return;
  loadFile(s);
}

// ************************************************************
void QucsEdit::slotSave()
{
  if(FileName.isEmpty()) {
    FileName = Q3FileDialog::getSaveFileName(".", QString::null,
	this, "", tr("Enter a Document Name"));
    if(FileName.isEmpty())  return;
  }

  QFile file(FileName);
  if(!file.open(QIODevice::WriteOnly)) {
    QMessageBox::critical(this, tr("Error"),
		tr("Cannot write file: ")+FileName);
    return;
  }

  Q3TextStream stream(&file);
  stream << text->text();
  text->setModified(false);
  file.close();
}

// ************************************************************
void QucsEdit::slotQuit()
{
  if(!closeFile()) return;

  int tmp;
  tmp = x();		// call size and position function in order to ...
  tmp = y();		// ... set them correctly before closing the ...
  tmp = width();	// dialog !!!  Otherwise the frame of the window ...
  tmp = height();	// will not be recognized (a X11 problem).

  accept();
}

// ************************************************************
// To get all close events.
void QucsEdit::closeEvent(QCloseEvent*)
{
  slotQuit();
}

// ************************************************************
bool QucsEdit::loadFile(const QString& Name)
{
  if(Name.isEmpty()) return false;
  QFile file(Name);
  if(!file.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(this, tr("Error"),
		tr("Cannot read file: ")+Name);
    return false;
  }

  Q3TextStream stream(&file);
  text->setText(stream.read());
  file.close();

  FileName = Name;
//  QFileInfo info(Name);
//  FileName = info.fileName();
  setCaption("Qucs Editor " PACKAGE_VERSION " - " + tr("File: ")+FileName);
  return true;
}


// ************************************************************
bool QucsEdit::closeFile()
{
  if(text->isModified()) {
    switch(QMessageBox::warning(this,tr("Closing document"),
      tr("The text contains unsaved changes!\n")+
      tr("Do you want to save the changes?"),
      tr("&Save"), tr("&Discard"), tr("&Cancel"), 0, 2)) {
      case 0: slotSave();
	      if(FileName.isEmpty()) return false;
	      return true;
      case 2: return false;
    }
  }
  return true;
}
