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

#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QToolButton>
#include <QImage>
#include <QFont>
#include <QPixmap>
#include <QCloseEvent>
#include <QToolBar>
#include <QTextEdit>
#include <QFileDialog>
#include <QtGui>

QucsEdit::QucsEdit(const QString& FileName_, bool readOnly)
{
  // set application icon
  setWindowIcon (QPixmap(":/bitmaps/big.qucs.xpm"));
  setWindowTitle("Qucs Editor " PACKAGE_VERSION " - " + tr("File: "));

  QToolButton *ButtLoad = new QToolButton;
  ButtLoad->setIcon(QIcon((":/bitmaps/fileopen.png")));
  connect(ButtLoad, SIGNAL(clicked()), SLOT(slotLoad()));

  QToolButton *ButtSave = new QToolButton;
  ButtSave->setIcon(QIcon((":/bitmaps/filesave.png")));
  connect(ButtSave, SIGNAL(clicked()), SLOT(slotSave()));
  ButtSave->setDisabled(readOnly);

  PosText = new QLabel(tr("Line: %1  -  Column: %2").arg(1).arg(1));

  QPushButton *ButtAbout = new QPushButton(tr("About"));
  connect(ButtAbout, SIGNAL(clicked()), SLOT(slotAbout()));

  QPushButton *ButtOK = new QPushButton(tr("Quit"));
  connect(ButtOK, SIGNAL(clicked()), SLOT(slotQuit()));
  ButtOK->setFocus();

  QWidget *separator = new QWidget(this);
  separator->setSizePolicy(QSizePolicy::Expanding,
                           QSizePolicy::Expanding);

  QToolBar *tool = addToolBar("main toolbar");

  tool->addWidget(ButtLoad);
  tool->addWidget(ButtSave);
  tool->addWidget(separator);
  tool->addWidget(PosText);
  tool->addWidget(ButtAbout);
  tool->addWidget(ButtOK);

  // try using same-sized mono-spaced font in the textarea
  QFont fedit = QFont("Courier New");
  fedit.setPointSize(QucsSettings.font.pointSize()-1);
  fedit.setStyleHint(QFont::Courier);
  fedit.setFixedPitch(true);

  text = new QTextEdit(this);
  //text->setTextFormat(Qt::PlainText);
  text->setReadOnly(readOnly);
  text->setWordWrapMode(QTextOption::NoWrap);
  text->setMinimumSize(300,200);
  text->setFont(fedit);
  text->setCurrentFont(fedit);

  setCentralWidget(text);

  connect(text, SIGNAL(cursorPositionChanged()),
          SLOT(slotPrintCursorPosition()));

  // .................................................
  loadFile(FileName_);
}

// ************************************************************
void QucsEdit::slotPrintCursorPosition()
{
  QTextCursor cursor=text->textCursor();
  PosText->setText(tr("Line: %1  -  Column: %2").arg(cursor.blockNumber()+1).arg(cursor.columnNumber()+1));
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
  QString s = QFileDialog::getOpenFileName(this, tr("Enter a Filename"),
                                           lastDir.isEmpty() ? QString(".") : lastDir, "*");
  if(s.isEmpty()) return;
  lastDir = s;   // remember last directory and file
  if(!closeFile()) return;
  loadFile(s);
}

// ************************************************************
void QucsEdit::slotSave()
{
  if(FileName.isEmpty()) {
    FileName = QFileDialog::getSaveFileName(this, tr("Enter a Document Name"),
                                            ".", QString::null);
    if(FileName.isEmpty())  return;
  }

  QFile file(FileName);
  if(!file.open(QIODevice::WriteOnly)) {
    QMessageBox::critical(this, tr("Error"),
		tr("Cannot write file: ")+FileName);
    return;
  }

  QTextStream stream(&file);
  stream << text->toPlainText();
  text->document()->setModified(false);
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

  qApp->quit();

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

  QTextStream stream(&file);
  text->document()->setPlainText(stream.readAll());
  file.close();

  FileName = Name;
  setWindowTitle("Qucs Editor " PACKAGE_VERSION " - " + tr("File: ")+FileName);
  return true;
}


// ************************************************************
bool QucsEdit::closeFile()
{
  if(text->document()->isModified()) {
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
