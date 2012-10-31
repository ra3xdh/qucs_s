/***************************************************************************
                             importdialog.cpp
                            ------------------
    begin                : Fri Jun 23 2006
    copyright            : (C) 2006 by Michael Margraf
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
#include <QtGui>
#include <q3hbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <q3textedit.h>
#include <q3vgroupbox.h>
#include <qcombobox.h>
#include <q3filedialog.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <Q3GridLayout>

#include "importdialog.h"
#include "main.h"
#include "qucs.h"


ImportDialog::ImportDialog(QWidget *parent)
		: QDialog(parent, 0, FALSE, Qt::WDestructiveClose)
{
  setCaption(tr("Convert Data File..."));

  all = new Q3GridLayout(this, 4, 3, 5, 3);

  Q3VGroupBox *Group2 = new Q3VGroupBox(tr("File specification"),this);
  all->addMultiCellWidget(Group2, 0,1, 0,2);
  QWidget *f = new QWidget(Group2);
  Q3GridLayout *file = new Q3GridLayout(f, 3, 3, 5);
  file->addWidget(new QLabel(tr("Input File:"), f), 0, 0);
  ImportEdit = new QLineEdit(f);
  file->addWidget(ImportEdit, 0, 1);
  QPushButton *BrowseButt = new QPushButton(tr("Browse"), f);
  file->addWidget(BrowseButt, 0, 2);
  connect(BrowseButt, SIGNAL(clicked()), SLOT(slotBrowse()));
  file->addWidget(new QLabel(tr("Output File:"), f), 1, 0);
  OutputEdit = new QLineEdit(f);
  file->addWidget(OutputEdit, 1, 1);
  OutputLabel = new QLabel(tr("Output Data:"), f);
  OutputLabel->setEnabled(false);
  file->addWidget(OutputLabel, 2, 0);
  OutputData = new QLineEdit(f);
  OutputData->setEnabled(false);
  file->addWidget(OutputData, 2, 1);
  OutType = new QComboBox(f);
  OutType->insertItem(tr("Qucs dataset"));
  OutType->insertItem(tr("Touchstone"));
  OutType->insertItem(tr("CSV"));
  OutType->insertItem(tr("Qucs library"));
  OutType->insertItem(tr("Qucs netlist"));
  OutType->insertItem(tr("Matlab"));
  connect(OutType, SIGNAL(activated(int)), SLOT(slotType(int)));
  file->addWidget(OutType, 2, 2);
  
  Q3VGroupBox *Group1 = new Q3VGroupBox(tr("Messages"),this);
  all->addMultiCellWidget(Group1, 2,2, 0,2);

  MsgText = new Q3TextEdit(Group1);
  MsgText->setTextFormat(Qt::PlainText);
  MsgText->setReadOnly(true);
  MsgText->setWordWrap(Q3TextEdit::NoWrap);
  MsgText->setMinimumSize(250, 60);

  Q3HBox *Butts = new Q3HBox(this);
  all->addMultiCellWidget(Butts, 3,3, 0,2);

  Butts->setStretchFactor(new QWidget(Butts), 5); // stretchable placeholder
  
  ImportButt = new QPushButton(tr("Convert"), Butts);
  connect(ImportButt, SIGNAL(clicked()), SLOT(slotImport()));
  AbortButt = new QPushButton(tr("Abort"), Butts);
  AbortButt->setDisabled(true);
  connect(AbortButt, SIGNAL(clicked()), SLOT(slotAbort()));
  CancelButt = new QPushButton(tr("Close"), Butts);
  connect(CancelButt, SIGNAL(clicked()), SLOT(reject()));
}

ImportDialog::~ImportDialog()
{
  if(Process.isRunning())  Process.kill();
  delete all;
}

// ------------------------------------------------------------------------
void ImportDialog::slotBrowse()
{
  QString s = Q3FileDialog::getOpenFileName(
     lastDir.isEmpty() ? QString(".") : lastDir,
     tr("All known")+
     " (*.s?p *.csv *.citi *.cit *.asc *.mdl *.vcd *.dat *.cir);;"+
     tr("Touchstone files")+" (*.s?p);;"+
     tr("CSV files")+" (*.csv);;"+
     tr("CITI files")+" (*.citi *.cit);;"+
     tr("ZVR ASCII files")+" (*.asc);;"+
     tr("IC-CAP model files")+" (*.mdl);;"+
     tr("VCD files")+" (*.vcd);;"+
     tr("Qucs dataset files")+" (*.dat);;"+
     tr("SPICE files")+" (*.cir);;"+
     tr("Any file")+" (*)",
     this, 0, tr("Enter a Data File Name"));

  if(!s.isEmpty()) {
    QFileInfo Info(s);
    lastDir = Info.dirPath(true);  // remember last directory
    ImportEdit->setText(s);

    if(OutputEdit->text().isEmpty()) {
      switch(OutType->currentItem()) {
      case 0:
	OutputEdit->setText(Info.baseName()+".dat");
	break;
      case 1:
	OutputEdit->setText(Info.baseName()+".snp");
	break;
      case 2:
	OutputEdit->setText(Info.baseName()+".csv");
	break;
      case 3:
	OutputEdit->setText(Info.baseName()+".lib");
	break;
      case 4:
	OutputEdit->setText(Info.baseName()+".txt");
	break;
      case 5:
	OutputEdit->setText(Info.baseName()+".mat");
	break;
      default:
	OutputEdit->setText(Info.baseName()+".dat");
	break;
      }
    }
  }
}

// ------------------------------------------------------------------------
void ImportDialog::slotImport()
{
  MsgText->clear();
  if (OutputEdit->text().isEmpty())
    return;

  ImportButt->setDisabled(true);
  AbortButt->setDisabled(false);

  QFile File(QucsWorkDir.filePath(OutputEdit->text()));
  if(File.exists())
    if(QMessageBox::information(this, tr("Info"),
          tr("Output file already exists!")+"\n"+tr("Overwrite it?"),
          tr("&Yes"), tr("&No"), 0,1,1))
      {
	ImportButt->setDisabled(false);
	AbortButt->setDisabled(true);
	return;
      }

  QFileInfo Info(ImportEdit->text());
  QString Suffix = Info.extension();
  QStringList CommandLine;
  CommandLine << QucsSettings.BinDir + "qucsconv" << "-if";
  
  if((Suffix == "citi") || (Suffix == "cit"))
    CommandLine << "citi";
  else if(Suffix == "vcd")
    CommandLine << "vcd";
  else if(Suffix == "asc")
    CommandLine << "zvr";
  else if(Suffix == "mdl")
    CommandLine << "mdl";
  else if(Suffix == "csv")
    CommandLine << "csv";
  else if(Suffix == "dat")
    CommandLine << "qucsdata";
  else if(Suffix == "cir")
    CommandLine << "spice";
  else for(;;) {
    if(Suffix.at(0) == 's')
      if(Suffix.at(2) == 'p')
        if(Suffix.length() == 3)
          if(Suffix.at(1).isDigit()) {
            CommandLine << "touchstone";
            break;
          }

    MsgText->append(tr("ERROR: Unknown file format! Please check file name extension!"));
    return;
  }

  CommandLine << "-of";
  switch(OutType->currentItem()) {
  case 0:
    CommandLine << "qucsdata";
    break;
  case 1:
    CommandLine << "touchstone";
    if (!OutputData->text().isEmpty())
      CommandLine << "-d" << OutputData->text();
    break;
  case 2:
    CommandLine << "csv";
    if (!OutputData->text().isEmpty())
      CommandLine << "-d" << OutputData->text();
    break;
  case 3:
    CommandLine << "qucslib";
    break;
  case 4:
    CommandLine << "qucs";
    break;
  case 5:
    CommandLine << "matlab";
    break;
  default:
    CommandLine << "qucsdata";
    break;
  }

  CommandLine << "-i" << ImportEdit->text()
              << "-o" << QucsWorkDir.filePath(OutputEdit->text());

  Process.setArguments(CommandLine);
  Process.blockSignals(false);

  disconnect(&Process, 0, 0, 0);
  connect(&Process, SIGNAL(readyReadStderr()), SLOT(slotDisplayErr()));
  connect(&Process, SIGNAL(readyReadStdout()), SLOT(slotDisplayMsg()));
  connect(&Process, SIGNAL(processExited()), SLOT(slotProcessEnded()));

  MsgText->append(tr("Running command line:")+"\n");
  MsgText->append(CommandLine.join(" "));
  MsgText->append("\n");

  if(!Process.start())
    MsgText->append(tr("ERROR: Cannot start converter!"));
}

// ------------------------------------------------------------------------
void ImportDialog::slotType(int index)
{
  switch(index) {
  case 0:
  case 3:
  case 4:
  case 5:
    OutputData->setEnabled(false);
    OutputLabel->setEnabled(false);
    break;
  case 1:
  case 2:
    OutputData->setEnabled(true);
    OutputLabel->setEnabled(true);
    break;
  default:
    OutputData->setEnabled(false);
    OutputLabel->setEnabled(false);
    break;
  }
}

// ------------------------------------------------------------------------
void ImportDialog::slotAbort()
{
  if(Process.isRunning())  Process.kill();
  AbortButt->setDisabled(true);
  ImportButt->setDisabled(false);
}

// ------------------------------------------------------------------------
// Is called when the process sends an output to stdout.
void ImportDialog::slotDisplayMsg()
{
  int par = MsgText->paragraphs();
  int idx = MsgText->paragraphLength(par-1);
  MsgText->setCursorPosition(par-1,idx);
  MsgText->insert(QString(Process.readStdout()));
}

// ------------------------------------------------------------------------
// Is called when the process sends an output to stderr.
void ImportDialog::slotDisplayErr()
{
  int par = MsgText->paragraphs();
  int idx = MsgText->paragraphLength(par-1);
  MsgText->setCursorPosition(par-1,idx);
  MsgText->insert(QString(Process.readStderr()));
}

// ------------------------------------------------------------------------
// Is called when the simulation process terminates.
void ImportDialog::slotProcessEnded()
{
  ImportButt->setDisabled(false);
  AbortButt->setDisabled(true);

  if(Process.normalExit() && (Process.exitStatus() == 0)) {
    MsgText->append(tr("Successfully converted file!"));

    disconnect(CancelButt, SIGNAL(clicked()), 0, 0);
    connect(CancelButt, SIGNAL(clicked()), SLOT(accept()));
  }
  else
    MsgText->append(tr("Converter ended with errors!"));
}
