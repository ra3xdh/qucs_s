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
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QGroupBox>
#include <QComboBox>
#include <QFileDialog>
#include <QPushButton>
#include <QMessageBox>
#include <QGridLayout>
#include <QDebug>
#include <QListView>

#include "importdialog.h"
#include "main.h"
#include "qucs.h"


ImportDialog::ImportDialog(QWidget *parent)
		: QDialog(parent) 
{
  setWindowTitle(tr("Convert Data File..."));

  all = new QGridLayout(this);

  QGroupBox *Group2 = new QGroupBox(tr("File specification"),this);
  
  QGridLayout *file = new QGridLayout();
  file->addWidget(new QLabel(tr("Input File:")),0, 0);
  ImportEdit = new QLineEdit();
  file->addWidget(ImportEdit, 0, 1);
  connect(ImportEdit,SIGNAL(textChanged(QString)),this,SLOT(slotValidateInput()));
  QPushButton *BrowseButt = new QPushButton(tr("Browse"));
  file->addWidget(BrowseButt, 0, 2);
  connect(BrowseButt, SIGNAL(clicked()), SLOT(slotBrowse()));

  file->addWidget(new QLabel(tr("Input Format:")), 1, 0);
  InType = new QComboBox();
  InType->addItem(tr("SPICE netlist"));
  InType->addItem(tr("VCD dataset"));
  InType->addItem(tr("CSV"));
  InType->addItem(tr("Qucs dataset"));
  InType->addItem(tr("Citi"));
  InType->addItem(tr("ZVR"));
  InType->addItem(tr("MDL"));
  InType->addItem(tr("Touchstone"));
  file->addWidget(InType,1,1);
  connect(InType,SIGNAL(currentIndexChanged(int)),this,SLOT(slotValidateOutput()));


  file->addWidget(new QLabel(tr("Output File:")), 2, 0);
  OutputEdit = new QLineEdit();
  file->addWidget(OutputEdit, 2, 1);
  QPushButton *SaveBrowseButt = new QPushButton(tr("Browse"));
  file->addWidget(SaveBrowseButt, 2, 2);
  connect(SaveBrowseButt, SIGNAL(clicked()), SLOT(slotSaveBrowse()));


  file->addWidget(new QLabel(tr("Output File:")), 3, 0);
  OutType = new QComboBox();
  OutType->addItem(tr("Qucs dataset"));
  OutType->addItem(tr("Touchstone"));
  OutType->addItem(tr("CSV"));
  OutType->addItem(tr("Qucs library"));
  OutType->addItem(tr("Qucs netlist"));
  OutType->addItem(tr("Matlab"));
  connect(OutType, SIGNAL(activated(int)), SLOT(slotType(int)));
  file->addWidget(OutType, 3, 1);

  OutputLabel = new QLabel(tr("Output Data:"));
  OutputLabel->setEnabled(false);
  file->addWidget(OutputLabel, 4, 0);
  OutputData = new QLineEdit();
  OutputData->setEnabled(false);
  file->addWidget(OutputData, 4, 1);

  Group2->setLayout(file);
  all->addWidget(Group2, 0,0,1,1);
  
  QGroupBox *Group1 = new QGroupBox(tr("Messages"));
  
  QVBoxLayout *vMess = new QVBoxLayout();
  MsgText = new QPlainTextEdit();
  vMess->addWidget(MsgText);
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
  slotValidateOutput();

}

ImportDialog::~ImportDialog()
{
  if(Process.state() == QProcess::Running)  Process.kill();
  delete all;
}

// ------------------------------------------------------------------------
void ImportDialog::slotBrowse()
{
  QString s = QFileDialog::getOpenFileName(
     this, tr("Enter a Data File Name"),
     lastImportDir.isEmpty() ? QString(".") : lastImportDir,
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
     tr("Any file")+" (*)");

  if(!s.isEmpty()) {
    QFileInfo Info(s);
    lastImportDir = Info.absolutePath();  // remember last directory
    ImportEdit->setText(s);
  }
}

void ImportDialog::slotSaveBrowse()
{
    QString s = QFileDialog::getSaveFileName(
       this, tr("Enter a Data File Name"),
       lastImportDir.isEmpty() ? QString(".") : lastImportDir,
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
       tr("Any file")+" (*)");
    OutputEdit->setText(s);
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
          QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
      {
	ImportButt->setDisabled(false);
	AbortButt->setDisabled(true);
	return;
      }

  QString Program;
  QStringList CommandLine;

  Program = QucsSettings.Qucsconv;
  CommandLine  << "-if";
  
  switch (InType->currentIndex()) {
  case 0:
      CommandLine << "spice";
      break;
  case 1:
      CommandLine << "vcd";
      break;
  case 2:
      CommandLine << "csv";
      break;
  case 3:
      CommandLine << "qucsdata";
      break;
  case 4:
      CommandLine << "citi";
      break;
  case 5:
      CommandLine << "zvr";
      break;
  case 6:
      CommandLine << "mdl";
      break;
  case 7:
      CommandLine << "touchstone";
      break;
  default:  break;
  }

  CommandLine << "-of";
  switch(OutType->currentIndex()) {
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

  MsgText->appendPlainText(tr("Running command line:")+"\n");
  MsgText->appendPlainText(Program + " " + CommandLine.join(" "));
  MsgText->appendPlainText("\n");

  qDebug() << "Command:" << Program << CommandLine.join(" ");
  Process.start(Program, CommandLine);
  Process.waitForStarted();
  
  if(Process.state() != QProcess::Running)
    MsgText->appendPlainText(tr("ERROR: Cannot start converter!"));
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
  if(Process.state() == QProcess::Running)  Process.kill();
  AbortButt->setDisabled(true);
  ImportButt->setDisabled(false);
}

// ------------------------------------------------------------------------
// Is called when the process sends an output to stdout.
void ImportDialog::slotDisplayMsg()
{
  MsgText->appendPlainText(QString(Process.readAllStandardOutput()));
}

// ------------------------------------------------------------------------
// Is called when the process sends an output to stderr.
void ImportDialog::slotDisplayErr()
{
  MsgText->appendPlainText(QString(Process.readAllStandardError()));
}

// ------------------------------------------------------------------------
// Is called when the simulation process terminates.
void ImportDialog::slotProcessEnded(int status)
{
  ImportButt->setDisabled(false);
  AbortButt->setDisabled(true);

  if(status == 0) {    
    MsgText->appendPlainText(tr("Successfully converted file!"));

    disconnect(CancelButt, SIGNAL(clicked()), 0, 0);
    connect(CancelButt, SIGNAL(clicked()), SLOT(accept()));
  }
  else
    MsgText->appendPlainText(tr("Converter ended with errors!"));
}

void ImportDialog::slotValidateInput()
{
    QString in_file = ImportEdit->text();
    if (in_file.isEmpty() || !QFile::exists(in_file)) return;

    QFileInfo inf(in_file);
    QString Suffix = inf.suffix().toLower();
    int idx = 3;
    QRegularExpression snp_expr("s[1-9]p");

    if((Suffix == "citi") || (Suffix == "cit"))
      idx = 4;
    else if(Suffix == "vcd")
      idx = 1;
    else if(Suffix == "asc")
      idx = 5;
    else if(Suffix == "mdl")
      idx = 6;
    else if(Suffix == "csv")
      idx = 2;
    else if(Suffix == "dat")
      idx = 3;
    else if(Suffix == "cir" || Suffix == "ckt" || Suffix == "sp")
      idx = 0;
    else if (snp_expr.match(Suffix).hasMatch())
      idx = 7;

    InType->setCurrentIndex(idx);

}


void ImportDialog::slotValidateOutput()
{
    QListView* view = qobject_cast<QListView *>(OutType->view());

    for (int i = 0; i < OutType->count(); i++) {
        view->setRowHidden(i,false);
    }

    switch (InType->currentIndex()) {
    case 0: // SPICE
        view->setRowHidden(0,true);
        view->setRowHidden(1,true);
        view->setRowHidden(2,true);
        view->setRowHidden(5,true);
        OutType->setCurrentIndex(3);
        break;
    case 1: // CSV
    case 2: // VCD
        view->setRowHidden(1,true);
        view->setRowHidden(2,true);
        view->setRowHidden(3,true);
        view->setRowHidden(4,true);
        view->setRowHidden(5,true);
        OutType->setCurrentIndex(0);
        break;
    case 3: // Qucsdata
        view->setRowHidden(0,true);
        view->setRowHidden(3,true);
        view->setRowHidden(4,true);
        OutType->setCurrentIndex(1);
        break;
    case 4: // Citi
    case 5: // ZVR
    case 6: // MDL
    case 7: // Touchstone
        view->setRowHidden(1,true);
        view->setRowHidden(2,true);
        view->setRowHidden(3,true);
        view->setRowHidden(4,true);
        view->setRowHidden(5,true);
        OutType->setCurrentIndex(0);
        break;
        break;
    default:
        break;
    }
}
