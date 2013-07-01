/***************************************************************************
                           qucssettingsdialog.cpp
                          ------------------------
    begin                : Sun May 23 2004
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
#include <QtGui>
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "qucssettingsdialog.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include "main.h"
#include "textdoc.h"
#include "schematic.h"

#include <QWidget>
#include <QLabel>
#include <QTabWidget>
#include <QLayout>
#include <QColorDialog>
#include <QFontDialog>
#include <QValidator>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QMessageBox>
#include <QCheckBox>


QucsSettingsDialog::QucsSettingsDialog(QucsApp *parent, const char *name)
			: QDialog(parent, name)
{
  App = parent;
  setWindowTitle(tr("Edit Qucs Properties"));

  Expr.setPattern("[\\w_]+");
  Validator  = new QRegExpValidator(Expr, this);

  all = new QVBoxLayout(this); // to provide the neccessary size
  QTabWidget *t = new QTabWidget();
  all->addWidget(t);

  // ...........................................................
  QWidget *Tab1 = new QWidget(t);
  QGridLayout *gp = new QGridLayout(Tab1);

  gp->addWidget(new QLabel(tr("Font (set after reload):"), Tab1), 0,0);
  FontButton = new QPushButton(Tab1);
  connect(FontButton, SIGNAL(clicked()), SLOT(slotFontDialog()));
  gp->addWidget(FontButton,0,1);

  gp->addWidget(new QLabel(tr("Document Background Color:"), Tab1) ,1,0);
  BGColorButton = new QPushButton("      ", Tab1);
  connect(BGColorButton, SIGNAL(clicked()), SLOT(slotBGColorDialog()));
  gp->addWidget(BGColorButton,1,1);

  gp->addWidget(new QLabel(tr("Language (set after reload):"), Tab1) ,2,0);
  LanguageCombo = new QComboBox(Tab1);
  LanguageCombo->insertItem(tr("system language"));
  LanguageCombo->insertItem(tr("English")+" (en)");
  LanguageCombo->insertItem(tr("German")+" (de)");
  LanguageCombo->insertItem(tr("French")+" (fr)");
  LanguageCombo->insertItem(tr("Spanish")+" (es)");
  LanguageCombo->insertItem(tr("Italian")+" (it)");
  LanguageCombo->insertItem(tr("Polish")+" (pl)");
  LanguageCombo->insertItem(tr("Romanian")+" (ro)");
  LanguageCombo->insertItem(tr("Japanese")+" (jp)");
  LanguageCombo->insertItem(tr("Swedish")+" (sv)");
  LanguageCombo->insertItem(tr("Hungarian")+" (hu)");
  LanguageCombo->insertItem(tr("Hebrew")+" (he)");
  LanguageCombo->insertItem(tr("Portuguese")+" (pt)");
  LanguageCombo->insertItem(tr("Turkish")+" (tr)");
  LanguageCombo->insertItem(tr("Ukrainian")+" (uk)");
  LanguageCombo->insertItem(tr("Russian")+" (ru)");
  LanguageCombo->insertItem(tr("Czech")+" (cs)");
  LanguageCombo->insertItem(tr("Catalan")+" (ca)");
  LanguageCombo->insertItem(tr("Arabic")+" (ar)");
  LanguageCombo->insertItem(tr("Kazakh")+" (kk)");
  gp->addWidget(LanguageCombo,2,1);

  val200 = new QIntValidator(0, 200, this);
  gp->addWidget(new QLabel(tr("maximum undo operations:"), Tab1) ,3,0);
  undoNumEdit = new QLineEdit(Tab1);
  undoNumEdit->setValidator(val200);
  gp->addWidget(undoNumEdit,3,1);

  gp->addWidget(new QLabel(tr("text editor:"), Tab1) ,4,0);
  editorEdit = new QLineEdit(Tab1);
  gp->addWidget(editorEdit,4,1);

  gp->addWidget(new QLabel(tr("start wiring when clicking open node:"), Tab1) ,5,0);
  checkWiring = new QCheckBox(Tab1);
  gp->addWidget(checkWiring,5,1);


  t->addTab(Tab1, tr("Settings"));

  // ...........................................................
  QWidget *Tab3 = new QWidget(t);
  QGridLayout *gp3 = new QGridLayout(Tab3);

  gp3->addMultiCellWidget(new QLabel(tr("Colors for Syntax Highlighting:"), Tab3), 0,0,0,1);

  ColorComment = new QPushButton(tr("Comment"), Tab3);
  ColorComment->setPaletteForegroundColor(QucsSettings.Comment);
  ColorComment->setPaletteBackgroundColor(QucsSettings.BGColor);
  connect(ColorComment, SIGNAL(clicked()), SLOT(slotColorComment()));
  gp3->addWidget(ColorComment,1,0);

  ColorString = new QPushButton(tr("String"), Tab3);
  ColorString->setPaletteForegroundColor(QucsSettings.String);
  ColorString->setPaletteBackgroundColor(QucsSettings.BGColor);
  connect(ColorString, SIGNAL(clicked()), SLOT(slotColorString()));
  gp3->addWidget(ColorString,1,1);

  ColorInteger = new QPushButton(tr("Integer Number"), Tab3);
  ColorInteger->setPaletteForegroundColor(QucsSettings.Integer);
  ColorInteger->setPaletteBackgroundColor(QucsSettings.BGColor);
  connect(ColorInteger, SIGNAL(clicked()), SLOT(slotColorInteger()));
  gp3->addWidget(ColorInteger,1,2);

  ColorReal = new QPushButton(tr("Real Number"), Tab3);
  ColorReal->setPaletteForegroundColor(QucsSettings.Real);
  ColorReal->setPaletteBackgroundColor(QucsSettings.BGColor);
  connect(ColorReal, SIGNAL(clicked()), SLOT(slotColorReal()));
  gp3->addWidget(ColorReal,2,0);

  ColorCharacter = new QPushButton(tr("Character"), Tab3);
  ColorCharacter->setPaletteForegroundColor(QucsSettings.Character);
  ColorCharacter->setPaletteBackgroundColor(QucsSettings.BGColor);
  connect(ColorCharacter, SIGNAL(clicked()), SLOT(slotColorCharacter()));
  gp3->addWidget(ColorCharacter,2,1);

  ColorDataType = new QPushButton(tr("Data Type"), Tab3);
  ColorDataType->setPaletteForegroundColor(QucsSettings.Type);
  ColorDataType->setPaletteBackgroundColor(QucsSettings.BGColor);
  connect(ColorDataType, SIGNAL(clicked()), SLOT(slotColorDataType()));
  gp3->addWidget(ColorDataType,2,2);

  ColorAttribute = new QPushButton(tr("Attribute"), Tab3);
  ColorAttribute->setPaletteForegroundColor(QucsSettings.Attribute);
  ColorAttribute->setPaletteBackgroundColor(QucsSettings.BGColor);
  connect(ColorAttribute, SIGNAL(clicked()), SLOT(slotColorAttribute()));
  gp3->addWidget(ColorAttribute,3,0);

  ColorDirective = new QPushButton(tr("Directive"), Tab3);
  ColorDirective->setPaletteForegroundColor(QucsSettings.Directive);
  ColorDirective->setPaletteBackgroundColor(QucsSettings.BGColor);
  connect(ColorDirective, SIGNAL(clicked()), SLOT(slotColorDirective()));
  gp3->addWidget(ColorDirective,3,1);

  ColorTask = new QPushButton(tr("Task"), Tab3);
  ColorTask->setPaletteForegroundColor(QucsSettings.Task);
  ColorTask->setPaletteBackgroundColor(QucsSettings.BGColor);
  connect(ColorTask, SIGNAL(clicked()), SLOT(slotColorTask()));
  gp3->addWidget(ColorTask,3,2);


  t->addTab(Tab3, tr("Source Code Editor"));

  // ...........................................................
  QWidget *Tab2 = new QWidget(t);
  QGridLayout *gp2 = new QGridLayout(Tab2);

  QLabel *note = new QLabel(
     tr("Register filename extensions here in order to\nopen files with an appropriate program."));
  gp2->addWidget(note,0,0,1,2);

  tableWidget = new QTableWidget(Tab2);
  tableWidget->setColumnCount(2);

  QTableWidgetItem *item1 = new QTableWidgetItem();
  QTableWidgetItem *item2 = new QTableWidgetItem();

  tableWidget->setHorizontalHeaderItem(0, item1);
  tableWidget->setHorizontalHeaderItem(1, item2);

  item1->setText(tr("Suffix"));
  item2->setText(tr("Program"));

  tableWidget->horizontalHeader()->setStretchLastSection(true);
  tableWidget->verticalHeader()->hide();
  connect(tableWidget, SIGNAL(cellClicked(int,int)), SLOT(slotTableCliked(int,int)));
  gp2->addWidget(tableWidget,1,0,3,1);

  // fill listview with already registered file extensions
  QStringList::Iterator it = QucsSettings.FileTypes.begin();
  while(it != QucsSettings.FileTypes.end()) {
    int row = tableWidget->rowCount();
    tableWidget->setRowCount(row+1);
    QTableWidgetItem *suffix = new QTableWidgetItem(QString((*it).section('/',0,0)));
    QTableWidgetItem *program = new QTableWidgetItem(QString((*it).section('/',1,1)));
    suffix->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    program->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    tableWidget->setItem(row, 0, suffix);
    tableWidget->setItem(row, 1, program);
    it++;
  }

  QLabel *l5 = new QLabel(tr("Suffix:"), Tab2);
  gp2->addWidget(l5,1,1);
  Input_Suffix = new QLineEdit(Tab2);
  Input_Suffix->setValidator(Validator);
  gp2->addWidget(Input_Suffix,1,2);
//  connect(Input_Suffix, SIGNAL(returnPressed()), SLOT(slotGotoProgEdit())); //not implemented

  QLabel *l6 = new QLabel(tr("Program:"), Tab2);
  gp2->addWidget(l6,2,1);
  Input_Program = new QLineEdit(Tab2);
  gp2->addWidget(Input_Program,2,2);

  QPushButton *AddButt = new QPushButton(tr("Set"));
  gp2->addWidget(AddButt,3,1);
  connect(AddButt, SIGNAL(clicked()), SLOT(slotAdd()));
  QPushButton *RemoveButt = new QPushButton(tr("Remove"));
  gp2->addWidget(RemoveButt,3,2);
  connect(RemoveButt, SIGNAL(clicked()), SLOT(slotRemove()));

  gp2->setRowStretch(3,4);
  t->addTab(Tab2, tr("File Types"));

  // ...........................................................
  QWidget *Tab4 = new QWidget(t);
  QGridLayout *gp4 = new QGridLayout(Tab4);

  QLabel *note2 = new QLabel(
     tr("Edit the standard paths and external applications"));
  gp4->addWidget(note2,0,0,1,2);
  gp4->addWidget(new QLabel(tr("Octave path:"), Tab4) ,2,0);
  octaveEdit = new QLineEdit(Tab4);
  gp4->addWidget(octaveEdit,2,1);
  QPushButton *OctaveButt = new QPushButton("...");
  gp4->addWidget(OctaveButt, 2, 2);
  connect(OctaveButt, SIGNAL(clicked()), SLOT(slotOctaveBrowse()));

  gp4->addWidget(new QLabel(tr("Qucs Home:"), Tab4) ,3,0);
  homeEdit = new QLineEdit(Tab4);
  gp4->addWidget(homeEdit,3,1);
  QPushButton *HomeButt = new QPushButton("...");
  gp4->addWidget(HomeButt, 3, 2);
  connect(HomeButt, SIGNAL(clicked()), SLOT(slotHomeDirBrowse()));



  t->addTab(Tab4, tr("Locations"));
  // ...........................................................


  // buttons on the bottom of the dialog (independent of the TabWidget)
  QHBoxLayout *Butts = new QHBoxLayout();
  Butts->setSpacing(3);
  Butts->setMargin(3);
  all->addLayout(Butts);

  QPushButton *OkButt = new QPushButton(tr("OK"));
  Butts->addWidget(OkButt);
  connect(OkButt, SIGNAL(clicked()), SLOT(slotOK()));
  QPushButton *ApplyButt = new QPushButton(tr("Apply"));
  Butts->addWidget(ApplyButt);
  connect(ApplyButt, SIGNAL(clicked()), SLOT(slotApply()));
  QPushButton *CancelButt = new QPushButton(tr("Cancel"));
  Butts->addWidget(CancelButt);
  connect(CancelButt, SIGNAL(clicked()), SLOT(reject()));
  QPushButton *DefaultButt = new QPushButton(tr("Default Values"));
  Butts->addWidget(DefaultButt);
  connect(DefaultButt, SIGNAL(clicked()), SLOT(slotDefaultValues()));



  OkButt->setDefault(true);

  // ...........................................................
  // fill the fields with the Qucs-Properties
  Font  = QucsSettings.font;
  FontButton->setText(Font.toString());
  BGColorButton->setPaletteBackgroundColor(QucsSettings.BGColor);
  undoNumEdit->setText(QString::number(QucsSettings.maxUndo));
  editorEdit->setText(QucsSettings.Editor);
  checkWiring->setChecked(QucsSettings.NodeWiring);

  for(int z=LanguageCombo->count()-1; z>=0; z--)
    if(LanguageCombo->text(z).section('(',1,1).remove(')') == QucsSettings.Language)
      LanguageCombo->setCurrentItem(z);
  octaveEdit->setText(QucsSettings.OctaveBinDir.canonicalPath());
  homeEdit->setText(QucsSettings.QucsHomeDir.canonicalPath());

  resize(300, 200);
}

QucsSettingsDialog::~QucsSettingsDialog()
{
  delete all;
  delete val200;
  delete Validator;
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotAdd()
{
    QModelIndexList indexes = tableWidget->selectionModel()->selection().indexes();
    if (indexes.count()){
      tableWidget->item(indexes.at(0).row(),0)->setText(Input_Suffix->text());
      tableWidget->item(indexes.at(0).row(),1)->setText(Input_Program->text());
      tableWidget->selectionModel()->clear();
      return;
    }

    //check before append
    for(int r=0; r < tableWidget->rowCount(); r++)
      if(tableWidget->item(r,0)->text() == Input_Suffix->text()) {
        QMessageBox::critical(this, tr("Error"),
              tr("This suffix is already registered!"));
        return;
      }

    int row = tableWidget->rowCount();
    tableWidget->setRowCount(row+1);

    QTableWidgetItem *newSuffix = new QTableWidgetItem(QString(Input_Suffix->text()));
    newSuffix->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    tableWidget->setItem(row, 0, newSuffix);

    QTableWidgetItem *newProgram = new QTableWidgetItem(Input_Program->text());
    tableWidget->setItem(row, 1, newProgram);
    newProgram->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    Input_Suffix->setFocus();
    Input_Suffix->clear();
    Input_Program->clear();
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotRemove()
{
    QModelIndexList indexes = tableWidget->selectionModel()->selection().indexes();
    if (indexes.count()){
      tableWidget->removeRow(indexes.at(0).row());
      tableWidget->selectionModel()->clear();
      Input_Suffix->setText("");
      Input_Program->setText("");
      return;
    }
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotOK()
{
  slotApply();
  accept();
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotApply()
{
  bool changed = false;

  if(QucsSettings.BGColor != BGColorButton->paletteBackgroundColor()) {
    QucsSettings.BGColor = BGColorButton->paletteBackgroundColor();

    int No=0;
    QWidget *w;
    while((w=App->DocumentTab->page(No++)) != 0)
      if(w->inherits("QTextEdit"))
        ((TextDoc*)w)->viewport()->setPaletteBackgroundColor(
					QucsSettings.BGColor);
      else
        ((Schematic*)w)->viewport()->setPaletteBackgroundColor(
					QucsSettings.BGColor);
    changed = true;
  }

  QucsSettings.font=Font;

  QucsSettings.Language =
      LanguageCombo->currentText().section('(',1,1).remove(')');

  if(QucsSettings.Comment != ColorComment->paletteForegroundColor()) {
    QucsSettings.Comment = ColorComment->paletteForegroundColor();
    changed = true;
  }
  if(QucsSettings.String != ColorString->paletteForegroundColor()) {
    QucsSettings.String = ColorString->paletteForegroundColor();
    changed = true;
  }
  if(QucsSettings.Integer != ColorInteger->paletteForegroundColor()) {
    QucsSettings.Integer = ColorInteger->paletteForegroundColor();
    changed = true;
  }
  if(QucsSettings.Real != ColorReal->paletteForegroundColor()) {
    QucsSettings.Real = ColorReal->paletteForegroundColor();
    changed = true;
  }
  if(QucsSettings.Character != ColorCharacter->paletteForegroundColor()) {
    QucsSettings.Character = ColorCharacter->paletteForegroundColor();
    changed = true;
  }
  if(QucsSettings.Type != ColorDataType->paletteForegroundColor()) {
    QucsSettings.Type = ColorDataType->paletteForegroundColor();
    changed = true;
  }
  if(QucsSettings.Attribute != ColorAttribute->paletteForegroundColor()) {
    QucsSettings.Attribute = ColorAttribute->paletteForegroundColor();
    changed = true;
  }
  if(QucsSettings.Directive != ColorDirective->paletteForegroundColor()) {
    QucsSettings.Directive = ColorDirective->paletteForegroundColor();
    changed = true;
  }
  if(QucsSettings.Task != ColorTask->paletteForegroundColor()) {
    QucsSettings.Task = ColorTask->paletteForegroundColor();
    changed = true;
  }

  bool ok;
  if(QucsSettings.maxUndo != undoNumEdit->text().toUInt(&ok)) {
    QucsSettings.maxUndo = undoNumEdit->text().toInt(&ok);
    changed = true;
  }
  if(QucsSettings.Editor != editorEdit->text()) {
    QucsSettings.Editor = editorEdit->text();
    changed = true;
  }
  if(QucsSettings.NodeWiring != (unsigned)checkWiring->isChecked()) {
    QucsSettings.NodeWiring = checkWiring->isChecked();
    changed = true;
  }

  QucsSettings.FileTypes.clear();
  for (int row=0; row < tableWidget->rowCount(); row++){
      QucsSettings.FileTypes.append(tableWidget->item(row,0)->text()
                                    +"/"+
                                    tableWidget->item(row,1)->text());
  }
  QucsSettings.OctaveBinDir = octaveEdit->text();
  QucsSettings.QucsHomeDir = homeEdit->text();

  saveApplSettings(App);  // also sets the small and large font

  if(changed)
  {
      App->readProjects();
      App->readProjectFiles();
      App->repaint();
  }
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotOctaveBrowse()
{
    QFileDialog fileDialog( this, tr("Select the octave bin directory"), octaveEdit->text() );
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::DirectoryOnly);
    fileDialog.exec();
    octaveEdit->setText(fileDialog.selectedFile());
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotHomeDirBrowse()
{
    QFileDialog fileDialog( this, tr("Select the home directory"), homeEdit->text() );
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::DirectoryOnly);
    fileDialog.exec();
    homeEdit->setText(fileDialog.selectedFile());
}


// -----------------------------------------------------------
void QucsSettingsDialog::slotFontDialog()
{
  bool ok;
  QFont tmpFont = QFontDialog::getFont(&ok, Font, this);
  if(ok) {
    Font = tmpFont;
    FontButton->setText(Font.toString());
  }
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotBGColorDialog()
{
  QColor c = QColorDialog::getColor(
		BGColorButton->paletteBackgroundColor(), this);
  if(c.isValid())
    BGColorButton->setPaletteBackgroundColor(c);
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotDefaultValues()
{
  //Font = QFont("Helvetica", 12);
  FontButton->setText(Font.toString());
  LanguageCombo->setCurrentItem(0);
  BGColorButton->setPaletteBackgroundColor(QColor(255,250,225));

  ColorComment->setPaletteForegroundColor(Qt::gray);
  ColorString->setPaletteForegroundColor(Qt::red);
  ColorInteger->setPaletteForegroundColor(Qt::blue);
  ColorReal->setPaletteForegroundColor(Qt::darkMagenta);
  ColorCharacter->setPaletteForegroundColor(Qt::magenta);
  ColorDataType->setPaletteForegroundColor(Qt::darkRed);
  ColorAttribute->setPaletteForegroundColor(Qt::darkCyan);
  ColorDirective->setPaletteForegroundColor(Qt::darkCyan);
  ColorTask->setPaletteForegroundColor(Qt::darkRed);

  undoNumEdit->setText("20");
  editorEdit->setText(QucsSettings.BinDir + "qucsedit");
  checkWiring->setChecked(false);
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotColorComment()
{
  QColor c = QColorDialog::getColor(
		ColorComment->paletteForegroundColor(), this);
  if(c.isValid())
    ColorComment->setPaletteForegroundColor(c);
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotColorString()
{
  QColor c = QColorDialog::getColor(
		ColorString->paletteForegroundColor(), this);
  if(c.isValid())
    ColorString->setPaletteForegroundColor(c);
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotColorInteger()
{
  QColor c = QColorDialog::getColor(
		ColorInteger->paletteForegroundColor(), this);
  if(c.isValid())
    ColorInteger->setPaletteForegroundColor(c);
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotColorReal()
{
  QColor c = QColorDialog::getColor(
		ColorReal->paletteForegroundColor(), this);
  if(c.isValid())
    ColorReal->setPaletteForegroundColor(c);
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotColorCharacter()
{
  QColor c = QColorDialog::getColor(
		ColorCharacter->paletteForegroundColor(), this);
  if(c.isValid())
    ColorCharacter->setPaletteForegroundColor(c);
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotColorDataType()
{
  QColor c = QColorDialog::getColor(
		ColorDataType->paletteForegroundColor(), this);
  if(c.isValid())
    ColorDataType->setPaletteForegroundColor(c);
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotColorAttribute()
{
  QColor c = QColorDialog::getColor(
		ColorAttribute->paletteForegroundColor(), this);
  if(c.isValid())
    ColorAttribute->setPaletteForegroundColor(c);
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotColorDirective()
{
  QColor c = QColorDialog::getColor(
		ColorDirective->paletteForegroundColor(), this);
  if(c.isValid())
    ColorDirective->setPaletteForegroundColor(c);
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotColorTask()
{
  QColor c = QColorDialog::getColor(
		ColorTask->paletteForegroundColor(), this);
  if(c.isValid())
      ColorTask->setPaletteForegroundColor(c);
}

void QucsSettingsDialog::slotTableCliked(int row, int col)
{
    Input_Suffix->setText(tableWidget->item(row,0)->text());
    Input_Program->setText(tableWidget->item(row,1)->text());
}
