/***************************************************************************
                              librarydialog.cpp
                             -------------------
    begin                : Sun Jun 04 2006
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <QtGui>
#include <q3hbox.h>
#include <q3vbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <q3textedit.h>
#include <q3textstream.h>
#include <qdatastream.h>
#include <qcheckbox.h>
#include <q3listview.h>
#include <qvalidator.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <q3scrollview.h>
#include <q3buttongroup.h>
//Added by qt3to4:
#include <Q3VBoxLayout>

#include "librarydialog.h"
#include "qucs.h"
#include "main.h"
#include "schematic.h"

extern SubMap FileList;

LibraryDialog::LibraryDialog(QucsApp *App_, Q3ListViewItem *SchematicList)
			: QDialog(App_, 0, TRUE, Qt::WDestructiveClose)
{
  App = App_;
  setCaption(tr("Create Library"));

  Expr.setPattern("[\\w_]+");
  Validator = new QRegExpValidator(Expr, this);

  // ...........................................................
  all = new Q3VBoxLayout(this);
  all->setMargin(5);
  all->setSpacing(6);

  Q3HBox *h1 = new Q3HBox(this);
  all->addWidget(h1);
  theLabel = new QLabel(tr("Library Name:"), h1);
  NameEdit = new QLineEdit(h1);
  NameEdit->setValidator(Validator);

  Group = new Q3VButtonGroup(tr("Choose subcircuits:"), this);
  all->addWidget(Group);
  
  // ...........................................................
  Q3HBox *h3 = new Q3HBox(Group);
  ButtSelectAll = new QPushButton(tr("Select All"), h3);
  connect(ButtSelectAll, SIGNAL(clicked()), SLOT(slotSelectAll()));
  ButtSelectNone = new QPushButton(tr("Deselect All"), h3);
  connect(ButtSelectNone, SIGNAL(clicked()), SLOT(slotSelectNone()));

  // ...........................................................
  Q3ScrollView *Dia_Scroll = new Q3ScrollView(Group);
  Dia_Scroll->setMargin(5);
  Q3VBox *Dia_Box = new Q3VBox(Dia_Scroll->viewport());
  Dia_Scroll->addChild(Dia_Box);

  ErrText = new Q3TextEdit(this);
  ErrText->setHidden(true);
  ErrText->setTextFormat(Qt::PlainText);
  ErrText->setWordWrap(Q3TextEdit::NoWrap);
  all->addWidget(ErrText);
  
  // ...........................................................
  Q3HBox *h2 = new Q3HBox(this);
  all->addWidget(h2);
  ButtCancel = new QPushButton(tr("Cancel"), h2);
  connect(ButtCancel, SIGNAL(clicked()), SLOT(reject()));
  ButtCreate = new QPushButton(tr("Next >>"), h2);
  connect(ButtCreate, SIGNAL(clicked()), SLOT(slotCreate()));
  ButtCreate->setDefault(true);

  // ...........................................................
  // insert all subcircuits of current project
  Q3ListViewItem *p = SchematicList->firstChild();
  while(p) {
    if(p->parent() == 0)
      break;
    if(!p->text(1).isEmpty())
      BoxList.append(new QCheckBox(p->text(0), Dia_Box));
    p = p->nextSibling();
  }

  QColor theColor;
  if(BoxList.isEmpty()) {
    ButtCreate->setEnabled(false);
    theColor =
       (new QLabel(tr("No subcircuits!"), Dia_Box))->paletteBackgroundColor();
  }
  else
    theColor = BoxList.current()->paletteBackgroundColor();
  Dia_Scroll->viewport()->setPaletteBackgroundColor(theColor);
}

LibraryDialog::~LibraryDialog()
{
  delete all;
  delete Validator;
}

// ---------------------------------------------------------------
void LibraryDialog::slotCreate()
{
  if(NameEdit->text().isEmpty()) {
    QMessageBox::critical(this, tr("Error"), tr("Please insert a library name!"));
    return;
  }

  int count=0;
  QCheckBox *p;
  for(p = BoxList.first(); p != 0; p = BoxList.next())
    if(p->isChecked())
      count++;

  if(count < 1) {
    QMessageBox::critical(this, tr("Error"), tr("Please choose at least one subcircuit!"));
    return;
  }


  LibDir = QDir(QucsHomeDir);
  if(!LibDir.cd("user_lib")) { // user library directory exists ?
    if(!LibDir.mkdir("user_lib")) { // no, then create it
      QMessageBox::warning(this, tr("Warning"),
                   tr("Cannot create user library directory !"));
      return;
    }
    LibDir.cd("user_lib");
  }


  LibFile.setName(QucsSettings.LibDir + NameEdit->text() + ".lib");
  if(LibFile.exists()) {
    QMessageBox::critical(this, tr("Error"), tr("A system library with this name already exists!"));
    return;
  }

  LibFile.setName(LibDir.absFilePath(NameEdit->text()) + ".lib");
  if(LibFile.exists()) {
    QMessageBox::critical(this, tr("Error"), tr("A library with this name already exists!"));
    return;
  }

  Group->setHidden(true);
  ErrText->setHidden(false);
  NameEdit->setHidden(true);
  disconnect(ButtCreate, SIGNAL(clicked()), 0, 0);
  connect(ButtCreate, SIGNAL(clicked()), SLOT(slotNext()));

  for(p = BoxList.first(); p != 0; p = BoxList.next())
    if(p->isChecked())
      break;
  theLabel->setText(tr("Enter description for \"%1\":").arg(p->text()));

  for(p = BoxList.next(); p != 0; p = BoxList.next())
    if(p->isChecked())
      break;
  if(p == 0)
    ButtCreate->setText(tr("Create"));
}

// ---------------------------------------------------------------
void LibraryDialog::intoStream(Q3TextStream &Stream, QString &tmp,
			       const char *sec)
{
  int i = tmp.find("TOP LEVEL MARK");
  if(i >= 0) {
    i = tmp.find('\n',i) + 1;
    tmp = tmp.mid(i);
  }
  Stream << "  <" << sec << ">";
  Stream << tmp;
  Stream << "  </" << sec << ">\n";
}

// ---------------------------------------------------------------
int LibraryDialog::intoFile(QString &ifn, QString &ofn, QStringList &IFiles)
{
  int error = 0;
  QFile ifile(ifn);
  if(!ifile.open(QIODevice::ReadOnly)) {
    ErrText->insert(QObject::tr("ERROR: Cannot open file \"%1\".\n").
		    arg(ifn));
    error++;
  }
  else {
    QByteArray FileContent = ifile.readAll();
    ifile.close();
    if(ifile.name().right(4) == ".lst")
      LibDir.remove(ifile.name());
    QDir LibDirSub(LibDir);
    if(!LibDirSub.cd(NameEdit->text())) {
      if(!LibDirSub.mkdir(NameEdit->text())) {
	ErrText->insert(
          QObject::tr("ERROR: Cannot create user library subdirectory !\n"));
	error++;
      }
      LibDirSub.cd(NameEdit->text());
    }
    QFileInfo Info(ofn);
    ofn = Info.fileName();
    IFiles.append(ofn);
    QFile ofile;
    ofile.setName(LibDirSub.absFilePath(ofn));
    if(!ofile.open(QIODevice::WriteOnly)) {
      ErrText->insert(
        QObject::tr("ERROR: Cannot create file \"%1\".\n").arg(ofn));
      error++;
    }
    else {
      QDataStream ds(&ofile);
      ds.writeRawBytes(FileContent.data(), FileContent.size());
      ofile.close();
    }
  }
  return error;
}

// ---------------------------------------------------------------
void LibraryDialog::slotNext()
{
  Descriptions.append(ErrText->text());
  ErrText->clear();

  QCheckBox *p = BoxList.current();
  if(p) {
    theLabel->setText(tr("Enter description for \"%1\":").arg(p->text()));
    for(p = BoxList.next(); p != 0; p = BoxList.next())
      if(p->isChecked())
        break;
    if(p == 0)
      ButtCreate->setText(tr("Create"));
    return;
  }

  theLabel->setShown(false);
  ErrText->setReadOnly(true);
  ButtCancel->setEnabled(false);
  ButtCreate->setText(tr("Close"));
  disconnect(ButtCreate, SIGNAL(clicked()), 0, 0);
  connect(ButtCreate, SIGNAL(clicked()), SLOT(accept()));

  if(!LibFile.open(QIODevice::WriteOnly)) {
    ErrText->append(tr("Error: Cannot create library!"));
    return;
  }
  Q3TextStream Stream;
  Stream.setDevice(&LibFile);
  Stream << "<Qucs Library " PACKAGE_VERSION " \""
	 << NameEdit->text() << "\">\n\n";


  bool Success = true, ret;
  QStringList::Iterator it = Descriptions.begin();
  QString tmp;
  Q3TextStream ts(&tmp, QIODevice::WriteOnly);
  for(p = BoxList.first(); p != 0; p = BoxList.next())
    if(p->isChecked()) {
      Stream << "<Component " + p->text().section('.',0,0) + ">\n"
             << "  <Description>\n"
             << *(it++)
             << "\n  </Description>\n";

      Schematic *Doc = new Schematic(0, QucsWorkDir.filePath(p->text()));
      if(!Doc->loadDocument()) {  // load document if possible
        delete Doc;
        ErrText->append(tr("Error: Cannot load subcircuit \"%1\".").
			arg(p->text()));
        break;
      }
      Doc->DocName = NameEdit->text() + "_" + p->text();
      Success = false;

      // save analog model
      tmp.truncate(0);
      Doc->isAnalog = true;
      ret = Doc->createLibNetlist(&ts, ErrText, -1);
      if(ret) {
	intoStream(Stream, tmp, "Model");
	int error = 0;
	QStringList IFiles;
	SubMap::Iterator it = FileList.begin();
	while(it != FileList.end()) {
	  QString f = it.data().File;
	  QString ifn, ofn;
	  if(it.data().Type == "SCH") {
	    ifn = f + ".lst";
	    ofn = ifn;
	  } else if(it.data().Type == "CIR") {
	    ifn = f + ".lst";
	    ofn = ifn;
	  }
	  if (!ifn.isEmpty()) error += intoFile(ifn, ofn, IFiles);
	  it++;
	}
	FileList.clear();
	if(!IFiles.isEmpty()) {
	  Stream << "  <ModelIncludes \"" << IFiles.join("\" \"") << "\">\n";
	}
	Success = error > 0 ? false : true;
      } else {
	ErrText->insert("\n");
      }

      // save verilog model
      tmp.truncate(0);
      Doc->isVerilog = true;
      Doc->isAnalog = false;
      ret = Doc->createLibNetlist(&ts, ErrText, 0);
      if(ret) {
	intoStream(Stream, tmp, "VerilogModel");
	int error = 0;
	QStringList IFiles;
	SubMap::Iterator it = FileList.begin();
	while(it != FileList.end()) {
	  QString f = it.data().File;
	  QString ifn, ofn;
	  if(it.data().Type == "SCH") {
	    ifn = f + ".lst";
	    ofn = f + ".v";
	  } else if(it.data().Type == "VER") {
	    ifn = f;
	    ofn = ifn;
	  }
	  if (!ifn.isEmpty()) error += intoFile(ifn, ofn, IFiles);
	  it++;
	}
	FileList.clear();
	if(!IFiles.isEmpty()) {
	  Stream << "  <VerilogModelIncludes \"" 
		 << IFiles.join("\" \"") << "\">\n";
	}
	Success = error > 0 ? false : true;
      } else {
	ErrText->insert("\n");
      }

      // save vhdl model
      tmp.truncate(0);
      Doc->isVerilog = false;
      Doc->isAnalog = false;
      ret = Doc->createLibNetlist(&ts, ErrText, 0);
      if(ret) {
	intoStream(Stream, tmp, "VHDLModel");
	int error = 0;
	QStringList IFiles;
	SubMap::Iterator it = FileList.begin();
	while(it != FileList.end()) {
	  QString f = it.data().File;
	  QString ifn, ofn;
	  if(it.data().Type == "SCH") {
	    ifn = f + ".lst";
	    ofn = f + ".vhdl";
	  } else if(it.data().Type == "VHD") {
	    ifn = f;
	    ofn = ifn;
	  }
	  if (!ifn.isEmpty()) error += intoFile(ifn, ofn, IFiles);
	  it++;
	}
	FileList.clear();
	if(!IFiles.isEmpty()) {
	  Stream << "  <VHDLModelIncludes \"" 
		 << IFiles.join("\" \"") << "\">\n";
	}
	Success = error > 0 ? false : true;
      } else {
	ErrText->insert("\n");
      }

      Stream << "  <Symbol>\n";
      Doc->createSubcircuitSymbol();
      Painting *pp;
      for(pp = Doc->SymbolPaints.first(); pp != 0; pp = Doc->SymbolPaints.next())
        Stream << "    <" << pp->save() << ">\n";

      Stream << "  </Symbol>\n"
             << "</Component>\n\n";

      delete Doc;
      if(!Success) break;
    }

  LibFile.close();
  if(!Success) {
    LibFile.remove();
    return;
  }

  ErrText->append(tr("Successfully created library."));
}

// ---------------------------------------------------------------
void LibraryDialog::slotSelectAll()
{
  QCheckBox *p;
  for(p = BoxList.first(); p != 0; p = BoxList.next()) p->setChecked(true);
}

// ---------------------------------------------------------------
void LibraryDialog::slotSelectNone()
{
  QCheckBox *p;
  for(p = BoxList.first(); p != 0; p = BoxList.next()) p->setChecked(false);
}
