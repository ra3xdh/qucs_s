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
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3VBoxLayout>
#include "main.h"
#include "textdoc.h"
#include "schematic.h"

#include <qwidget.h>
#include <qlabel.h>
#include <q3hbox.h>
#include <qtabwidget.h>
#include <qlayout.h>
#include <qcolordialog.h>
#include <qfontdialog.h>
#include <qvalidator.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <q3listview.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qcheckbox.h>


QucsSettingsDialog::QucsSettingsDialog(QucsApp *parent, const char *name)
			: QDialog(parent, name, TRUE, Qt::WDestructiveClose)
{
  App = parent;
  setCaption(tr("Edit Qucs Properties"));

  Expr.setPattern("[\\w_]+");
  Validator  = new QRegExpValidator(Expr, this);

  all = new Q3VBoxLayout(this); // to provide the neccessary size
  QTabWidget *t = new QTabWidget(this);
  all->addWidget(t);

  // ...........................................................
  QWidget *Tab1 = new QWidget(t);
  Q3GridLayout *gp = new Q3GridLayout(Tab1,6,2,6,6);

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
  Q3GridLayout *gp3 = new Q3GridLayout(Tab3,5,3,5,5);

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
  Q3GridLayout *gp2 = new Q3GridLayout(Tab2,5,3,3,3);

  QLabel *l7 = new QLabel(
     tr("Register filename extensions here in order to\nopen files with an appropriate program.")
     , Tab2);
  gp2->addMultiCellWidget(l7,0,0,0,2);

  List_Suffix = new Q3ListView(Tab2);
  List_Suffix->addColumn(tr("Suffix"));
  List_Suffix->addColumn(tr("Program"));
  gp2->addMultiCellWidget(List_Suffix,1,4,0,0);
  connect(List_Suffix, SIGNAL(clicked(Q3ListViewItem*)),
		SLOT(slotEditSuffix(Q3ListViewItem*)));

  // fill listview with already registered file extensions
  QStringList::Iterator it = QucsSettings.FileTypes.begin();
  while(it != QucsSettings.FileTypes.end()) {
    new Q3ListViewItem(List_Suffix,
        (*it).section('/',0,0), (*it).section('/',1,1));
    it++;
  }

  QLabel *l5 = new QLabel(tr("Suffix:"), Tab2);
  gp2->addWidget(l5,1,1);
  Input_Suffix = new QLineEdit(Tab2);
  Input_Suffix->setValidator(Validator);
  gp2->addWidget(Input_Suffix,1,2);
//  connect(Input_Suffix, SIGNAL(returnPressed()), SLOT(slotGotoProgEdit()));

  QLabel *l6 = new QLabel(tr("Program:"), Tab2);
  gp2->addWidget(l6,2,1);
  Input_Program = new QLineEdit(Tab2);
  gp2->addWidget(Input_Program,2,2);

  Q3HBox *h = new Q3HBox(Tab2);
  h->setSpacing(3);
  gp2->addMultiCellWidget(h,3,3,1,2);

  QPushButton *AddButt = new QPushButton(tr("Set"), h);
  connect(AddButt, SIGNAL(clicked()), SLOT(slotAdd()));
  QPushButton *RemoveButt = new QPushButton(tr("Remove"), h);
  connect(RemoveButt, SIGNAL(clicked()), SLOT(slotRemove()));

  gp2->setRowStretch(4,5);
  t->addTab(Tab2, tr("File Types"));

  // ...........................................................
  // buttons on the bottom of the dialog (independent of the TabWidget)
  Q3HBox *Butts = new Q3HBox(this);
  Butts->setSpacing(3);
  Butts->setMargin(3);
  all->addWidget(Butts);

  QPushButton *OkButt = new QPushButton(tr("OK"), Butts);
  connect(OkButt, SIGNAL(clicked()), SLOT(slotOK()));
  QPushButton *ApplyButt = new QPushButton(tr("Apply"), Butts);
  connect(ApplyButt, SIGNAL(clicked()), SLOT(slotApply()));
  QPushButton *CancelButt = new QPushButton(tr("Cancel"), Butts);
  connect(CancelButt, SIGNAL(clicked()), SLOT(reject()));
  QPushButton *DefaultButt = new QPushButton(tr("Default Values"), Butts);
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

  resize(300, 200);
}

QucsSettingsDialog::~QucsSettingsDialog()
{
  delete all;
  delete val200;
  delete Validator;
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotEditSuffix(Q3ListViewItem *Item)
{
  if(Item) {
    Input_Suffix->setText(Item->text(0));
    Input_Program->setText(Item->text(1));
  }
  else {
    Input_Suffix->setFocus();
    Input_Suffix->setText("");
    Input_Program->setText("");
  }
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotAdd()
{
  Q3ListViewItem *Item = List_Suffix->selectedItem();
  if(Item) {
    Item->setText(0, Input_Suffix->text());
    Item->setText(1, Input_Program->text());
    return;
  }


  for(Item = List_Suffix->firstChild(); Item!=0; Item = Item->itemBelow())
    if(Item->text(0) == Input_Suffix->text()) {
      QMessageBox::critical(this, tr("Error"),
			tr("This suffix is already registered!"));
      return;
    }

  List_Suffix->ensureItemVisible(
      new Q3ListViewItem(List_Suffix, List_Suffix->lastItem(),
          Input_Suffix->text(), Input_Program->text()));
  Input_Suffix->setFocus();
  Input_Suffix->setText("");
  Input_Program->setText("");
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotRemove()
{
  Q3ListViewItem *Item = List_Suffix->selectedItem();
  if(Item == 0) return;

  List_Suffix->takeItem(Item);   // remove from ListView
  delete Item;

  Input_Suffix->setText("");
  Input_Program->setText("");
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

  if(savingFont != Font) {
    savingFont = Font;
    changed = true;
  }

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

  Q3ListViewItem *Item;
  QucsSettings.FileTypes.clear();
  for(Item = List_Suffix->firstChild(); Item!=0; Item = Item->itemBelow())
    QucsSettings.FileTypes.append(Item->text(0)+"/"+Item->text(1));


  saveApplSettings(App);  // also sets the small and large font
  if(changed)
    App->repaint();
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
  Font = QFont("Helvetica", 12);
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
