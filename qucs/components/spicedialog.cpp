/***************************************************************************
                               spicedialog.cpp
                              -----------------
    begin                : Tue May 3 2005
    copyright            : (C) 2005 by Michael Margraf
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
#include "spicedialog.h"
#include "spicefile.h"
#include "main.h"
#include "qucs.h"
#include "schematic.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QValidator>
#include <QFileDialog>
#include <QPushButton>
#include <QCheckBox>
#include <QProcess>
#include <QMessageBox>
#include <QComboBox>
#include <QTextStream>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDebug>


SpiceDialog::SpiceDialog(QucsApp* App_, SpiceFile *c, Schematic *d)
    : QDialog(d, 0, TRUE, Qt::WDestructiveClose)
{
  App = App_; // pointer to main application

  resize(400, 250);
  setWindowTitle(tr("Edit SPICE Component Properties"));
  Comp = c;
  Doc  = d;

  all = new QVBoxLayout(); // to provide neccessary size
  this->setLayout(all);
  QWidget *myParent = this;

  Expr.setPattern("[^\"=]+");  // valid expression for property 'edit' etc
  Validator = new QRegExpValidator(Expr, this);
  Expr.setPattern("[\\w_]+");  // valid expression for property 'NameEdit' etc
  ValRestrict = new QRegExpValidator(Expr, this);

  // ...........................................................
  QGridLayout *topGrid = new QGridLayout;
  all->addLayout(topGrid);

  CompNameEdit = new QLineEdit;
  CompNameEdit->setValidator(ValRestrict);
  connect(CompNameEdit, SIGNAL(returnPressed()), SLOT(slotButtOK()));

  topGrid->addWidget(new QLabel(tr("Name:")), 0, 0);
  topGrid->addWidget(CompNameEdit, 0, 1);

  
  FileEdit = new QLineEdit;
  FileEdit->setValidator(ValRestrict);
  connect(FileEdit, SIGNAL(returnPressed()), SLOT(slotButtOK()));
  ButtBrowse = new QPushButton(tr("Browse"));
  connect(ButtBrowse, SIGNAL(clicked()), SLOT(slotButtBrowse()));

  topGrid->addWidget(new QLabel(tr("File:")), 1,0);
  topGrid->addWidget(FileEdit, 1,1);
  topGrid->addWidget(ButtBrowse, 1,2);


  FileCheck = new QCheckBox(tr("show file name in schematic"), myParent);
  ButtEdit = new QPushButton(tr("Edit"), myParent);
  connect(ButtEdit, SIGNAL(clicked()), SLOT(slotButtEdit()));

  topGrid->addWidget(FileCheck, 2, 1);
  topGrid->addWidget(ButtEdit, 2, 2);


  SimCheck = new QCheckBox(tr("include SPICE simulations"), myParent);
  topGrid->addWidget(SimCheck, 3,1);

  QHBoxLayout *hcenter = new QHBoxLayout;
  topGrid->addLayout(hcenter, 4, 1);

  hcenter->setSpacing(5);
  PrepCombo = new QComboBox();
  PrepCombo->insertItems(0, QStringList() << "none" << "ps2sp" << "spicepp" << "spiceprm" );
  QLabel *PrepLabel = new QLabel(tr("preprocessor"));
  PrepLabel->setMargin(5);
  connect(PrepCombo, SIGNAL(activated(int)), SLOT(slotPrepChanged(int)));

  hcenter->addWidget(PrepCombo);
  hcenter->addWidget(PrepLabel);

  // ...........................................................
  QGridLayout *midGrid = new QGridLayout;
  all->addLayout(midGrid);

  midGrid->addWidget(new QLabel(tr("SPICE net nodes:")), 0,0);
  midGrid->addWidget(new QLabel(tr("Component ports:")), 0,2);

  NodesList = new QListWidget();
  connect(NodesList, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
          SLOT(slotAddPort(QListWidgetItem *)));
  PortsList = new QListWidget();
  connect(PortsList, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
          SLOT(slotRemovePort(QListWidgetItem *)));
  midGrid->addWidget(NodesList, 1,0);
  midGrid->addWidget(PortsList, 1,2);

  QVBoxLayout *vcenter = new QVBoxLayout;
  vcenter->setSpacing(5);

  ButtAdd = new QPushButton(tr("Add >>"));
  connect(ButtAdd, SIGNAL(clicked()), SLOT(slotButtAdd()));
  ButtRemove = new QPushButton(tr("<< Remove"));
  connect(ButtRemove, SIGNAL(clicked()), SLOT(slotButtRemove()));

  vcenter->addWidget(ButtAdd);
  vcenter->addWidget(ButtRemove);
  vcenter->addStretch();
  midGrid->addLayout(vcenter, 1,1);

  // ...........................................................
  QHBoxLayout *hbottom = new QHBoxLayout;
  hbottom->setSpacing(5);

  ButtOK = new QPushButton(tr("OK"));
  ButtApply = new QPushButton(tr("Apply"));
  ButtCancel = new QPushButton(tr("Cancel"));
  connect(ButtOK, SIGNAL(clicked()), SLOT(slotButtOK()));
  connect(ButtApply, SIGNAL(clicked()), SLOT(slotButtApply()));
  connect(ButtCancel, SIGNAL(clicked()), SLOT(slotButtCancel()));
  hbottom->addStretch();
  hbottom->addWidget(ButtOK);
  hbottom->addWidget(ButtApply);
  hbottom->addWidget(ButtCancel);

  all->addLayout(hbottom);

  // ------------------------------------------------------------
  CompNameEdit->setText(Comp->Name);
  changed = false;

  // insert all properties into the ListBox
  Property *pp = Comp->Props.first();
  FileEdit->setText(pp->Value);
  FileCheck->setChecked(pp->display);
  SimCheck->setChecked(Comp->Props.at(2)->Value == "yes");
  for(int i=0; i<PrepCombo->count(); i++)
  {
    if(PrepCombo->text(i) == Comp->Props.at(3)->Value)
    {
      PrepCombo->setCurrentItem(i);
      currentPrep = i;
      break;
    }
  }

  loadSpiceNetList(pp->Value);  // load netlist nodes
}

SpiceDialog::~SpiceDialog()
{
  delete all;
  delete Validator;
  delete ValRestrict;
}

// -------------------------------------------------------------------------
// Is called if the "OK"-button is pressed.
void SpiceDialog::slotButtOK()
{
  slotButtApply();
  slotButtCancel();
}

// -------------------------------------------------------------------------
// Is called if the "Cancel"-button is pressed.
void SpiceDialog::slotButtCancel()
{
  if(changed) done(1);	// changed could have been done before
  else done(0);		// (by "Apply"-button)
}

//-----------------------------------------------------------------
// To get really all close events (even <Escape> key).
void SpiceDialog::reject()
{
  slotButtCancel();
}

// -------------------------------------------------------------------------
// Is called, if the "Apply"-button is pressed.
void SpiceDialog::slotButtApply()
{
  Component *pc;
  if(CompNameEdit->text().isEmpty())  CompNameEdit->setText(Comp->Name);
  else if(CompNameEdit->text() != Comp->Name)
  {
    for(pc = Doc->Components->first(); pc!=0; pc = Doc->Components->next())
      if(pc->Name == CompNameEdit->text()) {
        break;  // found component with the same name ?
      }
    if (pc) {
      CompNameEdit->setText(Comp->Name);
    }
    else {
      Comp->Name = CompNameEdit->text();
      changed = true;
    }
  }

  // apply all the new property values
  Property *pp = Comp->Props.first();
  if(pp->Value != FileEdit->text())
  {
    pp->Value = FileEdit->text();
    changed = true;
  }
  if(pp->display != FileCheck->isChecked())
  {
    pp->display = FileCheck->isChecked();
    changed = true;
  }

  QString tmp;
  for(int i=0; i<PortsList->count(); i++)
  {
    if (!tmp.isEmpty()) {
      tmp += ',';
    }
    tmp += "_net" + PortsList->item(i)->text();   // chosen ports
  }
  pp = Comp->Props.next();
  if(pp->Value != tmp)
  {
    pp->Value = tmp;
    changed = true;
  }
  pp = Comp->Props.next();
  if((pp->Value=="yes") != SimCheck->isChecked())
  {
    pp->Value = ((SimCheck->isChecked())? "yes" : "no");
    changed = true;
  }
  if(pp->Value != "yes") {
    Comp->withSim = false;
  }

  pp = Comp->Props.next();
  if(pp->Value != PrepCombo->currentText())
  {
    pp->Value = PrepCombo->currentText();
    changed = true;
  }

  if(changed || Comp->withSim)    // because of "sim" text
  {
    Doc->recreateComponent(Comp); // to apply changes to the schematic symbol
    Doc->viewport()->repaint();
  }
}

// -------------------------------------------------------------------------
void SpiceDialog::slotButtBrowse()
{
  QString s = QFileDialog::getOpenFileName(this,
                  tr("Select a file"),
                  lastDir.isEmpty() ? QString(".") : lastDir,
                  tr("SPICE netlist") + App->getSpiceFileFilter ()
                      + tr("All Files") + " (*.*)");

  if(s.isEmpty()) {
    return;
  }

  QFileInfo Info(s);
  lastDir = Info.dirPath(true);  // remember last directory

  // snip path if file in current directory
  if(QucsSettings.QucsWorkDir.exists(Info.fileName()) &&
          QucsSettings.QucsWorkDir.absPath() == Info.dirPath(true)) {
    s = Info.fileName();
  }
  FileEdit->setText(s);

  Comp->Props.at(1)->Value = "";
  loadSpiceNetList(s);
}

// -------------------------------------------------------------------------
void SpiceDialog::slotPrepChanged(int i)
{
  if(currentPrep != i)
  {
    currentPrep = i;
    PrepCombo->setCurrentItem(i);
    loadSpiceNetList(FileEdit->text());  // reload netlist nodes
  }
}

// -------------------------------------------------------------------------
bool SpiceDialog::loadSpiceNetList(const QString& s)
{
  Comp->withSim = false;
  if(s.isEmpty()) return false;
  QFileInfo FileInfo(QucsSettings.QucsWorkDir, s);

  NodesList->clear();
  PortsList->clear();
  textStatus = 0;
  Line = Error = "";

  QString preprocessor = PrepCombo->currentText();
  if (preprocessor != "none")
  {
    qDebug() << "Run spice preprocessor (perl)";
    bool piping = true;
    QString script;
#ifdef __MINGW32__
    QString interpreter = "tinyperl.exe";
#else
    QString interpreter = "perl";
#endif
    if (preprocessor == "ps2sp")
    {
      script = "ps2sp";
    }
    else if (preprocessor == "spicepp")
    {
      script = "spicepp.pl";
    }
    else if (preprocessor == "spiceprm")
    {
      script = "spiceprm";
      piping = false;
    }
    script = QucsSettings.BinDir + script;
    QString spiceCommand;
    SpicePrep = new QProcess(this);
    spiceCommand+=interpreter + " ";
    spiceCommand+=script + " ";
    spiceCommand+=FileInfo.filePath() + " ";

    QFile PrepFile;
    QFileInfo PrepInfo(QucsSettings.QucsWorkDir, s + ".pre");
    QString PrepName = PrepInfo.filePath();

    if (!piping)
    {
      spiceCommand += PrepName + " ";
      connect(SpicePrep, SIGNAL(readyReadStandardOutput()), SLOT(slotSkipOut()));
      connect(SpicePrep, SIGNAL(readyReadStandardError()), SLOT(slotGetPrepErr()));
    }
    else
    {
      connect(SpicePrep, SIGNAL(readyReadStandardOutput()), SLOT(slotGetPrepOut()));
      connect(SpicePrep, SIGNAL(readyReadStandardError()), SLOT(slotGetPrepErr()));
    }

    QMessageBox *MBox = new QMessageBox(tr("Info"),
                                        tr("Preprocessing SPICE file \"%1\".").arg(FileInfo.filePath()),
                                        QMessageBox::NoIcon, QMessageBox::Abort,
                                        QMessageBox::NoButton, QMessageBox::NoButton, this, 0, true,
                                        Qt::WStyle_DialogBorder |  Qt::WDestructiveClose);

    connect(SpicePrep, SIGNAL(finished(int, QProcess::ExitStatus)), MBox, SLOT(close()));

    if (piping)
    {
      PrepFile.setFileName(PrepName);
      if(!PrepFile.open(QIODevice::WriteOnly))
      {
        QMessageBox::critical(this, tr("Error"),
                              tr("Cannot save preprocessed SPICE file \"%1\".").
                              arg(PrepName));
        return false;
      }
      prestream = new QTextStream(&PrepFile);
    }
    SpicePrep->start(spiceCommand);
    if ((SpicePrep->state() != QProcess::Starting) &&
        (SpicePrep->state() != QProcess::Running))
    {
        QMessageBox::critical(this, tr("Error"),
                              tr("Cannot execute \"%1\".").arg(interpreter + " " + script));
        if (piping)
        {
            PrepFile.close();
            delete prestream;
        }
        return false;
    }
    //SpicePrep->closeStdin();

    MBox->exec();
    delete SpicePrep;
    if (piping)
    {
        PrepFile.close();
        delete prestream;
    }

    if(!Error.isEmpty())
    {
        QMessageBox::critical(this, tr("SPICE Preprocessor Error"), Error);
        return false;
    }
    FileInfo = QFileInfo(QucsSettings.QucsWorkDir, s + ".pre");
  }

  // Now do the spice->qucs netlist conversion using the qucsconv program ...
  QucsConv = new QProcess(this);

  QString executableSuffix = "";
#ifdef __MINGW32__
  executableSuffix = ".exe";
#endif

  QString Program;
  QStringList Arguments;
  Program = QucsSettings.BinDir + "qucsconv" + executableSuffix;
  Arguments << "-if" << "spice"
            << "-of" <<  "qucs"
            << "-i" << FileInfo.filePath();

  qDebug() << "Command :" << Program << Arguments.join(" ");

  connect(QucsConv, SIGNAL(readyReadStandardOutput()), SLOT(slotGetNetlist()));
  connect(QucsConv, SIGNAL(readyReadStandardError()), SLOT(slotGetError()));


  QMessageBox *MBox = new QMessageBox(tr("Info"),
                                      tr("Converting SPICE file \"%1\".").arg(FileInfo.filePath()),
                                      QMessageBox::NoIcon, QMessageBox::Abort,
                                      QMessageBox::NoButton, QMessageBox::NoButton, this, 0, true,
                                      Qt::WStyle_DialogBorder |  Qt::WDestructiveClose);

  connect(QucsConv, SIGNAL(finished(int, QProcess::ExitStatus)), MBox, SLOT(close()));

  QucsConv->start(Program, Arguments);

//    if((QucsConv->state() != QProcess::Starting) && (QucsConv->state()!=QProcess::Running))
  if(!QucsConv->Running)
  {
    QMessageBox::critical(this, tr("Error"),
                          tr("Cannot execute \"%1\".").arg(QucsSettings.BinDir + "qucsconv" + executableSuffix));
    return false;
  }

  MBox->exec();

  if (!Error.isEmpty()) {
      QMessageBox::critical(this, tr("QucsConv Error"), Error);
  }

  Property *pp = Comp->Props.at(1);
  if(!pp->Value.isEmpty())
  {
    PortsList->clear();
    QStringList ports = QStringList::split(',', pp->Value);
    foreach(QString port, ports) {
      PortsList->addItem(port);
    }
  }

  QString tmp;
  QList<QListWidgetItem *> plist;
  for(int i=0; i<PortsList->count(); i++)
  {
    tmp = PortsList->item(i)->text().remove(0, 4);
    PortsList->item(i)->setText(tmp);

    plist = NodesList->findItems(tmp, Qt::MatchCaseSensitive | Qt::MatchExactly);
    if (!plist.isEmpty()) {
      delete plist[0];
    } else {
      delete PortsList->item(i);
    }
  }
  return true;
}

// -------------------------------------------------------------------------
void SpiceDialog::slotSkipErr()
{
  SpicePrep->readAllStandardError ();
}

// -------------------------------------------------------------------------
void SpiceDialog::slotSkipOut()
{
  SpicePrep->readAllStandardOutput ();
}

// -------------------------------------------------------------------------
void SpiceDialog::slotGetPrepErr()
{
  Error += QString(SpicePrep->readAllStandardError ());
}

// -------------------------------------------------------------------------
void SpiceDialog::slotGetPrepOut()
{
  (*prestream) << QString(SpicePrep->readAllStandardOutput ());
}

// -------------------------------------------------------------------------
void SpiceDialog::slotGetError()
{
  Error += QString(QucsConv->readAllStandardError ());
}

// -------------------------------------------------------------------------
void SpiceDialog::slotGetNetlist()
{
  qDebug() << "slotGetNetlist";
  int i;
  QString s;
  Line += QString(QucsConv->readAllStandardOutput ());

  while((i = Line.indexOf('\n')) >= 0)
  {
    s = Line.left(i);
    Line.remove(0, i+1);
    s = s.trimmed();
    if (!s.isEmpty ())
    {
      if (s.at(0) == '.')
      {
        if (s.left(5) != ".Def:")
        {
          Comp->withSim = true;
        }
        continue;
      }
    }

    switch(textStatus)
    {
    case 0:
      if (s == "### TOPLEVEL NODELIST BEGIN")
      {
          textStatus = 1;
      }
      else if (s == "### SPICE OUTPUT NODELIST BEGIN")
      {
          textStatus = 2;
      }
      break;

    case 1:
      if (s == "### TOPLEVEL NODELIST END")
      {
          textStatus = 0;
          break;
      }

      if (s.left(2) != "# ")
      {
          break;
      }
      s.remove(0, 2);

      if(s.left(4) == "_net")
      {
          NodesList->addItem(s.remove(0, 4));
      }
      break;

    case 2:
      if(s == "### SPICE OUTPUT NODELIST END")
      {
          textStatus = 0;
          break;
      }
      if(s.left(2) != "# ")
      {
          break;
      }
      s.remove(0, 2);

      if(s.left(4) == "_net")
      {
          PortsList->addItem(s);   // prefix "_net" is removed later on
      }
      break;
    }
  }
}

// -------------------------------------------------------------------------
void SpiceDialog::slotButtEdit()
{
  Doc->App->editFile(QucsSettings.QucsWorkDir.filePath(FileEdit->text()));
}

// -------------------------------------------------------------------------
// Is called if the add button is pressed.
void SpiceDialog::slotButtAdd()
{
  QListWidgetItem *item = NodesList->currentItem();
  if (item) {
    PortsList->addItem(item->text());
    delete(item);
  }
}

// -------------------------------------------------------------------------
// Is called if the remove button is pressed.
void SpiceDialog::slotButtRemove()
{
  QListWidgetItem *item = PortsList->currentItem();
  if (item) {
    NodesList->addItem(item->text());
    delete(item);
  }
}

// -------------------------------------------------------------------------
// Is called when double-click on NodesList-Box
void SpiceDialog::slotAddPort(QListWidgetItem *Item)
{
  if(Item) slotButtAdd();
}

// -------------------------------------------------------------------------
// Is called when double-click on PortsList-Box
void SpiceDialog::slotRemovePort(QListWidgetItem *Item)
{
  if(Item) slotButtRemove();
}
