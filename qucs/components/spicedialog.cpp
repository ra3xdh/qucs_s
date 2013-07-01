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
#include <QtGui>
#include "spicedialog.h"
#include "spicefile.h"
#include "main.h"
#include "qucs.h"
//Added by qt3to4:
#include <Q3TextStream>
#include <Q3GridLayout>
#include <Q3VBoxLayout>
#include "schematic.h"

#include <qlabel.h>
#include <qlayout.h>
#include <q3hbox.h>
#include <q3vbox.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <q3filedialog.h>
#include <qpushbutton.h>
#include <q3listbox.h>
#include <qcheckbox.h>
#include <QProcess>
#include <qmessagebox.h>
#include <qcombobox.h>


SpiceDialog::SpiceDialog(SpiceFile *c, Schematic *d)
			: QDialog(d, 0, TRUE, Qt::WDestructiveClose)
{
  resize(400, 250);
  setCaption(tr("Edit SPICE Component Properties"));
  Comp = c;
  Doc  = d;

  all = new Q3VBoxLayout(this); // to provide neccessary size
  QWidget *myParent = this;

  Expr.setPattern("[^\"=]+");  // valid expression for property 'edit' etc
  Validator = new QRegExpValidator(Expr, this);
  Expr.setPattern("[\\w_]+");  // valid expression for property 'NameEdit' etc
  ValRestrict = new QRegExpValidator(Expr, this);


  // ...........................................................
  Q3GridLayout *topGrid = new Q3GridLayout(0, 4,3,3,3);
  all->addLayout(topGrid);

  topGrid->addWidget(new QLabel(tr("Name:"), myParent), 0,0);
  CompNameEdit = new QLineEdit(myParent);
  CompNameEdit->setValidator(ValRestrict);
  topGrid->addWidget(CompNameEdit, 0,1);
  connect(CompNameEdit, SIGNAL(returnPressed()), SLOT(slotButtOK()));

  topGrid->addWidget(new QLabel(tr("File:"), myParent), 1,0);
  FileEdit = new QLineEdit(myParent);
  FileEdit->setValidator(ValRestrict);
  topGrid->addWidget(FileEdit, 1,1);
  connect(FileEdit, SIGNAL(returnPressed()), SLOT(slotButtOK()));

  ButtBrowse = new QPushButton(tr("Browse"), myParent);
  topGrid->addWidget(ButtBrowse, 1,2);
  connect(ButtBrowse, SIGNAL(clicked()), SLOT(slotButtBrowse()));

  ButtEdit = new QPushButton(tr("Edit"), myParent);
  topGrid->addWidget(ButtEdit, 2,2);
  connect(ButtEdit, SIGNAL(clicked()), SLOT(slotButtEdit()));

  FileCheck = new QCheckBox(tr("show file name in schematic"), myParent);
  topGrid->addWidget(FileCheck, 2,1);

  SimCheck = new QCheckBox(tr("include SPICE simulations"), myParent);
  topGrid->addWidget(SimCheck, 3,1);

  Q3HBox *h1 = new Q3HBox(myParent);
  h1->setSpacing(5);
  PrepCombo = new QComboBox(h1);
  PrepCombo->insertItem("none");
  PrepCombo->insertItem("ps2sp");
  PrepCombo->insertItem("spicepp");
  PrepCombo->insertItem("spiceprm");
  QLabel * PrepLabel = new QLabel(tr("preprocessor"), h1);
  PrepLabel->setMargin(5);
  topGrid->addWidget(h1, 4,1);
  connect(PrepCombo, SIGNAL(activated(int)), SLOT(slotPrepChanged(int)));

  // ...........................................................
  Q3GridLayout *midGrid = new Q3GridLayout(0, 2,3,5,5);
  all->addLayout(midGrid);

  midGrid->addWidget(new QLabel(tr("SPICE net nodes:"), myParent), 0,0);
  NodesList = new Q3ListBox(myParent);
  midGrid->addWidget(NodesList, 1,0);
  connect(NodesList, SIGNAL(doubleClicked(Q3ListBoxItem*)),
	SLOT(slotAddPort(Q3ListBoxItem*)));
  
  Q3VBox *v0 = new Q3VBox(myParent);
  v0->setSpacing(5);
  midGrid->addWidget(v0, 1,1);
  ButtAdd = new QPushButton(tr("Add >>"), v0);
  connect(ButtAdd, SIGNAL(clicked()), SLOT(slotButtAdd()));
  ButtRemove = new QPushButton(tr("<< Remove"), v0);
  connect(ButtRemove, SIGNAL(clicked()), SLOT(slotButtRemove()));
  v0->setStretchFactor(new QWidget(v0), 5); // stretchable placeholder

  midGrid->addWidget(new QLabel(tr("Component ports:"), myParent), 0,2);
  PortsList = new Q3ListBox(myParent);
  midGrid->addWidget(PortsList, 1,2);
  connect(PortsList, SIGNAL(doubleClicked(Q3ListBoxItem*)),
	SLOT(slotRemovePort(Q3ListBoxItem*)));
  

  // ...........................................................
  Q3HBox *h0 = new Q3HBox(this);
  h0->setSpacing(5);
  all->addWidget(h0);
  connect(new QPushButton(tr("OK"),h0), SIGNAL(clicked()),
	  SLOT(slotButtOK()));
  connect(new QPushButton(tr("Apply"),h0), SIGNAL(clicked()),
	  SLOT(slotButtApply()));
  connect(new QPushButton(tr("Cancel"),h0), SIGNAL(clicked()),
	  SLOT(slotButtCancel()));

  // ------------------------------------------------------------
  CompNameEdit->setText(Comp->Name);
  changed = false;

  // insert all properties into the ListBox
  Property *pp = Comp->Props.first();
  FileEdit->setText(pp->Value);
  FileCheck->setChecked(pp->display);
  SimCheck->setChecked(Comp->Props.at(2)->Value == "yes");
  for(int i=0; i<PrepCombo->count(); i++) {
    if(PrepCombo->text(i) == Comp->Props.at(3)->Value) {
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
  else
  if(CompNameEdit->text() != Comp->Name) {
    for(pc = Doc->Components->first(); pc!=0; pc = Doc->Components->next())
      if(pc->Name == CompNameEdit->text())
        break;  // found component with the same name ?
    if(pc)  CompNameEdit->setText(Comp->Name);
    else {
      Comp->Name = CompNameEdit->text();
      changed = true;
    }
  }


  // apply all the new property values
  Property *pp = Comp->Props.first();
  if(pp->Value != FileEdit->text()) {
    pp->Value = FileEdit->text();
    changed = true;
  }
  if(pp->display != FileCheck->isChecked()) {
    pp->display = FileCheck->isChecked();
    changed = true;
  }

  QString tmp;
  for(unsigned int i=0; i<PortsList->count(); i++) {
    if(!tmp.isEmpty())  tmp += ',';
    tmp += "_net" + PortsList->text(i);   // chosen ports
  }
  pp = Comp->Props.next();
  if(pp->Value != tmp) {
    pp->Value = tmp;
    changed = true;
  }
  pp = Comp->Props.next();
  if((pp->Value=="yes") != SimCheck->isChecked()) {
    if(SimCheck->isChecked()) pp->Value = "yes";
    else pp->Value = "no";
    changed = true;
  }
  if(pp->Value != "yes")  Comp->withSim = false;

  pp = Comp->Props.next();
  if(pp->Value != PrepCombo->currentText()) {
    pp->Value = PrepCombo->currentText();
    changed = true;
  }

  if(changed || Comp->withSim) {  // because of "sim" text
    Doc->recreateComponent(Comp); // to apply changes to the schematic symbol
    Doc->viewport()->repaint();
  }
}

// -------------------------------------------------------------------------
void SpiceDialog::slotButtBrowse()
{
  QString s = Q3FileDialog::getOpenFileName(
		lastDir.isEmpty() ? QString(".") : lastDir,
		tr("SPICE netlist")+" (*.cir);;"+tr("All Files")+" (*.*)",
		this, "", tr("Select a file"));
  if(s.isEmpty()) return;

  QFileInfo Info(s);
  lastDir = Info.dirPath(true);  // remember last directory

  // snip path if file in current directory
  if(QucsSettings.QucsWorkDir.exists(Info.fileName()) &&
     QucsSettings.QucsWorkDir.absPath() == Info.dirPath(true)) s = Info.fileName();
  FileEdit->setText(s);

  Comp->Props.at(1)->Value = "";
  loadSpiceNetList(s);
}

// -------------------------------------------------------------------------
void SpiceDialog::slotPrepChanged(int i)
{
  if(currentPrep != i) {
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
  if (preprocessor != "none") {
    bool piping = true;
    QString script;
#ifdef __MINGW32__
    QString interpreter = "tinyperl.exe";
#else
    QString interpreter = "perl";
#endif
    if (preprocessor == "ps2sp") {
      script = "ps2sp";
    } else if (preprocessor == "spicepp") {
      script = "spicepp.pl";
    } else if (preprocessor == "spiceprm") {
      script = "spiceprm";
      piping = false;
    }
    script = QucsSettings.BinDir + script;
    QString spiceCommand;
    SpicePrep = new QProcess(this);
    spiceCommand+=interpreter+" ";
    spiceCommand+=script+" ";
    spiceCommand+=FileInfo.filePath()+" ";

    QFile PrepFile;
    QFileInfo PrepInfo(QucsSettings.QucsWorkDir, s + ".pre");
    QString PrepName = PrepInfo.filePath();

    if (!piping) {
      spiceCommand+=PrepName+" ";
      connect(SpicePrep, SIGNAL(readyReadStandardOutput()), SLOT(slotSkipOut()));
      connect(SpicePrep, SIGNAL(readyReadStandardError()), SLOT(slotGetPrepErr()));
    } else {
      connect(SpicePrep, SIGNAL(readyReadStandardOutput()), SLOT(slotGetPrepOut()));
      connect(SpicePrep, SIGNAL(readyReadStandardError()), SLOT(slotGetPrepErr()));
    }

    QMessageBox *MBox = new QMessageBox(tr("Info"),
	       tr("Preprocessing SPICE file \"%1\".").arg(FileInfo.filePath()),
               QMessageBox::NoIcon, QMessageBox::Abort,
               QMessageBox::NoButton, QMessageBox::NoButton, this, 0, true,
	       Qt::WStyle_DialogBorder |  Qt::WDestructiveClose);
    connect(SpicePrep, SIGNAL(processExited()), MBox, SLOT(close()));

    if (piping) {
      PrepFile.setName(PrepName);
      if(!PrepFile.open(QIODevice::WriteOnly)) {
	QMessageBox::critical(this, tr("Error"),
          tr("Cannot save preprocessed SPICE file \"%1\".").
	  arg(PrepName));
	return false;
      }
      prestream = new Q3TextStream(&PrepFile);
    }
    SpicePrep->start(spiceCommand);
    if(SpicePrep->state()!=QProcess::Starting&&SpicePrep->state()!=QProcess::Running) {
      QMessageBox::critical(this, tr("Error"),
        tr("Cannot execute \"%1\".").arg(interpreter + " " + script));
      if (piping) {
	PrepFile.close();
	delete prestream;
      }
      return false;
    }
    //SpicePrep->closeStdin();

    MBox->exec();
    delete SpicePrep;
    if (piping) {
      PrepFile.close();
      delete prestream;
    }

    if(!Error.isEmpty()) {
      QMessageBox::critical(this, tr("SPICE Preprocessor Error"), Error);
      return false;
    }
    FileInfo = QFileInfo(QucsSettings.QucsWorkDir, s + ".pre");
  }

  // first call Qucsconv ............
  QucsConv = new QProcess(this);
  QString QucsconvCmd = QucsSettings.BinDir + "qucsconv -if spice -of qucs -i"+
          FileInfo.filePath();
  connect(QucsConv, SIGNAL(readyReadStandardOutput()), SLOT(slotGetNetlist()));
  connect(QucsConv, SIGNAL(readyReadStandardError()), SLOT(slotGetError()));

  QMessageBox *MBox = new QMessageBox(tr("Info"),
	       tr("Converting SPICE file \"%1\".").arg(FileInfo.filePath()),
               QMessageBox::NoIcon, QMessageBox::Abort,
               QMessageBox::NoButton, QMessageBox::NoButton, this, 0, true,
	       Qt::WStyle_DialogBorder |  Qt::WDestructiveClose);
  connect(QucsConv, SIGNAL(processExited()), MBox, SLOT(close()));
  QucsConv->start(QucsconvCmd);
  if(QucsConv->state()!=QProcess::Starting&&QucsConv->state()!=QProcess::Running) {
    QMessageBox::critical(this, tr("Error"),
      tr("Cannot execute \"%1\".").arg(QucsSettings.BinDir + "qucsconv"));
    return false;
  }
  //QucsConv->closeStdin();

  MBox->exec();
  delete QucsConv;

  if(!Error.isEmpty())
    QMessageBox::critical(this, tr("QucsConv Error"), Error);

  Property *pp = Comp->Props.at(1);
  if(!pp->Value.isEmpty()) {
    PortsList->clear();
    PortsList->insertStringList(QStringList::split(',', pp->Value));
  }

  QString tmp;
  Q3ListBoxItem *pi;
  for(unsigned int i=0; i<PortsList->count(); i++) {
    tmp = PortsList->text(i).remove(0, 4);
    PortsList->changeItem(tmp, i);

    pi = NodesList->findItem(tmp, Qt::CaseSensitive | QKeySequence::ExactMatch);
    if(pi) delete pi;
    else PortsList->removeItem(i--);
  }
  return true;
}

// -------------------------------------------------------------------------
void SpiceDialog::slotSkipErr()
{
  SpicePrep->read(9999);
}

// -------------------------------------------------------------------------
void SpiceDialog::slotSkipOut()
{
  SpicePrep->read(9999);
}

// -------------------------------------------------------------------------
void SpiceDialog::slotGetPrepErr()
{
  Error += QString(SpicePrep->read(9999));
}

// -------------------------------------------------------------------------
void SpiceDialog::slotGetPrepOut()
{
  (*prestream) << QString(SpicePrep->read(9999));
}

// -------------------------------------------------------------------------
void SpiceDialog::slotGetError()
{
  Error += QString(QucsConv->read(9999));
}

// -------------------------------------------------------------------------
void SpiceDialog::slotGetNetlist()
{
  int i;
  QString s;
  Line += QString(QucsConv->read(9999));

  while((i = Line.find('\n')) >= 0) {

    s = Line.left(i);
    Line.remove(0, i+1);


    s = s.stripWhiteSpace();
    if(s.at(0) == '.') {
      if(s.left(5) != ".Def:")  Comp->withSim = true;
      continue;
    }

    switch(textStatus) {
      case 0:
	if(s == "### TOPLEVEL NODELIST BEGIN")
	  textStatus = 1;
	else if(s == "### SPICE OUTPUT NODELIST BEGIN")
	  textStatus = 2;
	break;

      case 1:
	if(s == "### TOPLEVEL NODELIST END") {
	  textStatus = 0;
	  break;
	}
	if(s.left(2) != "# ") break;
	s.remove(0, 2);

	if(s.left(4) == "_net")
	  NodesList->insertItem(s.remove(0, 4));
	break;

      case 2:
	if(s == "### SPICE OUTPUT NODELIST END") {
	  textStatus = 0;
	  break;
	}
	if(s.left(2) != "# ") break;
	s.remove(0, 2);

	if(s.left(4) == "_net")
	  PortsList->insertItem(s); // prefix "_net" is removed later on
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
  int i = NodesList->currentItem();
  if(i < 0) return;
  PortsList->insertItem(NodesList->currentText());
  NodesList->removeItem(i);
}

// -------------------------------------------------------------------------
// Is called if the remove button is pressed.
void SpiceDialog::slotButtRemove()
{
  int i = PortsList->currentItem();
  if(i < 0) return;
  NodesList->insertItem(PortsList->currentText());
  PortsList->removeItem(i);
}

// -------------------------------------------------------------------------
// Is called when double-click on NodesList-Box
void SpiceDialog::slotAddPort(Q3ListBoxItem *Item)
{
  if(Item) slotButtAdd();
}

// -------------------------------------------------------------------------
// Is called when double-click on PortsList-Box
void SpiceDialog::slotRemovePort(Q3ListBoxItem *Item)
{
  if(Item) slotButtRemove();
}
