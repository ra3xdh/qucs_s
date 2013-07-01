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
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QGroupBox>
#include <QComboBox>
#include <QFileDialog>
#include <QPushButton>
#include <QMessageBox>
#include <QGridLayout>
#include <QDebug>

#include "importdialog.h"
#include "main.h"
#include "qucs.h"


ImportDialog::ImportDialog(QWidget *parent)
		: QDialog(parent) //, 0, FALSE, Qt::WDestructiveClose)
{
  setWindowTitle(tr("Convert Data File..."));

  all = new QGridLayout(this);

  QGroupBox *Group2 = new QGroupBox(tr("File specification"),this);
  
  QGridLayout *file = new QGridLayout();
  file->addWidget(new QLabel(tr("Input File:")),0, 0);
  ImportEdit = new QLineEdit();
  file->addWidget(ImportEdit, 0, 1);
  QPushButton *BrowseButt = new QPushButton(tr("Browse"));
  file->addWidget(BrowseButt, 0, 2);
  connect(BrowseButt, SIGNAL(clicked()), SLOT(slotBrowse()));
  file->addWidget(new QLabel(tr("Output File:")), 1, 0);
  OutputEdit = new QLineEdit();
  file->addWidget(OutputEdit, 1, 1);
  OutputLabel = new QLabel(tr("Output Data:"));
  OutputLabel->setEnabled(false);
  file->addWidget(OutputLabel, 2, 0);
  OutputData = new QLineEdit();
  OutputData->setEnabled(false);
  file->addWidget(OutputData, 2, 1);
  OutType = new QComboBox();
  OutType->addItem(tr("Qucs dataset"));
  OutType->addItem(tr("Touchstone"));
  OutType->addItem(tr("CSV"));
  OutType->addItem(tr("Qucs library"));
  OutType->addItem(tr("Qucs netlist"));
  OutType->addItem(tr("Matlab"));
  connect(OutType, SIGNAL(activated(int)), SLOT(slotType(int)));
  file->addWidget(OutType, 2, 2);
  
  Group2->setLayout(file);
  all->addWidget(Group2, 0,0,1,1);
  
  QGroupBox *Group1 = new QGroupBox(tr("Messages"));
  
  QVBoxLayout *vMess = new QVBoxLayout();
  MsgText = new QTextEdit();
  vMess->addWidget(MsgText);
  MsgText->setTextFormat(Qt::PlainText);
  MsgText->setReadOnly(true);
  MsgText->setWordWrapMode(QTextOption::NoWrap);
  MsgText->setMinimumSize(250, 60);
  Group1->setLayout(vMess);
  all->addWidget(Group1, 1,0,1,1);

  QHBoxLayout *Butts = new QHBoxLayout();
  
  Butts->addStretch(5);
 
  ImportButt = new QPushButton(tr("Convert"));
  connect(ImportButt, SIGNAL(clicked()), SLOT(slotImport()));
  AbortButt = new QPushButton(tr("Abort"));
  AbortButt->setDisabled(true);
  connect(AbortButt, SIGNAL(clicked()), SLOT(slotAbort()));
  CancelButt = new QPushButton(tr("Close"));
  connect(CancelButt, SIGNAL(clicked()), SLOT(reject()));
  Butts->addWidget(ImportButt);
  Butts->addWidget(AbortButt);
  Butts->addWidget(CancelButt);
  
  all->addLayout(Butts,2,0,1,1);
}

ImportDialog::~ImportDialog()
{
  if(Process.Running)  Process.kill();
  delete all;
}

// ------------------------------------------------------------------------
void ImportDialog::slotBrowse()
{
  QString s = QFileDialog::getOpenFileName(
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

  QFile File(QucsSettings.QucsWorkDir.filePath(OutputEdit->text()));
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
  QString Program;
  QStringList CommandLine;
  Program = QucsSettings.BinDir + "qucsconv";
  CommandLine  << "-if";
  
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
              << "-o" << QucsSettings.QucsWorkDir.filePath(OutputEdit->text());

  Process.blockSignals(false);

  disconnect(&Process, 0, 0, 0);
  connect(&Process, SIGNAL(readyReadStandardError()), SLOT(slotDisplayErr()));
  connect(&Process, SIGNAL(readyReadStandardOutput()), SLOT(slotDisplayMsg()));
  connect(&Process, SIGNAL(finished(int)), SLOT(slotProcessEnded(int)));

  MsgText->append(tr("Running command line:")+"\n");
  MsgText->append(Program + CommandLine.join(" "));
  MsgText->append("\n");

  Process.start(Program, CommandLine);
  
  if(!Process.Running)
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
  if(Process.Running)  Process.kill();
  AbortButt->setDisabled(true);
  ImportButt->setDisabled(false);
}

// ------------------------------------------------------------------------
// Is called when the process sends an output to stdout.
void ImportDialog::slotDisplayMsg()
{
  MsgText->append(QString(Process.readAllStandardOutput()));
}

// ------------------------------------------------------------------------
// Is called when the process sends an output to stderr.
void ImportDialog::slotDisplayErr()
{
  MsgText->append(QString(Process.readAllStandardError()));
}

// ------------------------------------------------------------------------
// Is called when the simulation process terminates.
void ImportDialog::slotProcessEnded(int status)
{
  ImportButt->setDisabled(false);
  AbortButt->setDisabled(true);

  if(status == 0) {    
    MsgText->append(tr("Successfully converted file!"));

    disconnect(CancelButt, SIGNAL(clicked()), 0, 0);
    connect(CancelButt, SIGNAL(clicked()), SLOT(accept()));
  }
  else
    MsgText->append(tr("Converter ended with errors!"));
}
