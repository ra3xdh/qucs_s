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
#include <iostream>
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

using namespace std;

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
    // The application setings tab
    QWidget *appSettingsTab = new QWidget(t);
    QGridLayout *appSettingsGrid = new QGridLayout(appSettingsTab);

    appSettingsGrid->addWidget(new QLabel(tr("Font (set after reload):"), appSettingsTab), 0,0);
    FontButton = new QPushButton(appSettingsTab);
    connect(FontButton, SIGNAL(clicked()), SLOT(slotFontDialog()));
    appSettingsGrid->addWidget(FontButton,0,1);

    appSettingsGrid->addWidget(new QLabel(tr("Document Background Color:"), appSettingsTab) ,1,0);
    BGColorButton = new QPushButton("      ", appSettingsTab);
    connect(BGColorButton, SIGNAL(clicked()), SLOT(slotBGColorDialog()));
    appSettingsGrid->addWidget(BGColorButton,1,1);

    appSettingsGrid->addWidget(new QLabel(tr("Language (set after reload):"), appSettingsTab) ,2,0);
    LanguageCombo = new QComboBox(appSettingsTab);
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
    appSettingsGrid->addWidget(LanguageCombo,2,1);

    val200 = new QIntValidator(0, 200, this);
    appSettingsGrid->addWidget(new QLabel(tr("maximum undo operations:"), appSettingsTab) ,3,0);
    undoNumEdit = new QLineEdit(appSettingsTab);
    undoNumEdit->setValidator(val200);
    appSettingsGrid->addWidget(undoNumEdit,3,1);

    appSettingsGrid->addWidget(new QLabel(tr("text editor:"), appSettingsTab) ,4,0);
    editorEdit = new QLineEdit(appSettingsTab);
    appSettingsGrid->addWidget(editorEdit,4,1);

    appSettingsGrid->addWidget(new QLabel(tr("start wiring when clicking open node:"), appSettingsTab) ,5,0);
    checkWiring = new QCheckBox(appSettingsTab);
    appSettingsGrid->addWidget(checkWiring,5,1);


    appSettingsGrid->addWidget(new QLabel(tr("Load documents from future versions ")));
    checkLoadFromFutureVersions = new QCheckBox(appSettingsTab);
    appSettingsGrid->addWidget(checkLoadFromFutureVersions,6,1);
    checkLoadFromFutureVersions->setChecked(QucsSettings.IgnoreFutureVersion);

    t->addTab(appSettingsTab, tr("Settings"));

    // ...........................................................
    // The source code editor settings tab
    QWidget *editorTab = new QWidget(t);
    QGridLayout *editorGrid = new QGridLayout(editorTab);

    editorGrid->addMultiCellWidget(new QLabel(tr("Colors for Syntax Highlighting:"), editorTab), 0,0,0,1);

    ColorComment = new QPushButton(tr("Comment"), editorTab);
    ColorComment->setPaletteForegroundColor(QucsSettings.Comment);
    ColorComment->setPaletteBackgroundColor(QucsSettings.BGColor);
    connect(ColorComment, SIGNAL(clicked()), SLOT(slotColorComment()));
    editorGrid->addWidget(ColorComment,1,0);

    ColorString = new QPushButton(tr("String"), editorTab);
    ColorString->setPaletteForegroundColor(QucsSettings.String);
    ColorString->setPaletteBackgroundColor(QucsSettings.BGColor);
    connect(ColorString, SIGNAL(clicked()), SLOT(slotColorString()));
    editorGrid->addWidget(ColorString,1,1);

    ColorInteger = new QPushButton(tr("Integer Number"), editorTab);
    ColorInteger->setPaletteForegroundColor(QucsSettings.Integer);
    ColorInteger->setPaletteBackgroundColor(QucsSettings.BGColor);
    connect(ColorInteger, SIGNAL(clicked()), SLOT(slotColorInteger()));
    editorGrid->addWidget(ColorInteger,1,2);

    ColorReal = new QPushButton(tr("Real Number"), editorTab);
    ColorReal->setPaletteForegroundColor(QucsSettings.Real);
    ColorReal->setPaletteBackgroundColor(QucsSettings.BGColor);
    connect(ColorReal, SIGNAL(clicked()), SLOT(slotColorReal()));
    editorGrid->addWidget(ColorReal,2,0);

    ColorCharacter = new QPushButton(tr("Character"), editorTab);
    ColorCharacter->setPaletteForegroundColor(QucsSettings.Character);
    ColorCharacter->setPaletteBackgroundColor(QucsSettings.BGColor);
    connect(ColorCharacter, SIGNAL(clicked()), SLOT(slotColorCharacter()));
    editorGrid->addWidget(ColorCharacter,2,1);

    ColorDataType = new QPushButton(tr("Data Type"), editorTab);
    ColorDataType->setPaletteForegroundColor(QucsSettings.Type);
    ColorDataType->setPaletteBackgroundColor(QucsSettings.BGColor);
    connect(ColorDataType, SIGNAL(clicked()), SLOT(slotColorDataType()));
    editorGrid->addWidget(ColorDataType,2,2);

    ColorAttribute = new QPushButton(tr("Attribute"), editorTab);
    ColorAttribute->setPaletteForegroundColor(QucsSettings.Attribute);
    ColorAttribute->setPaletteBackgroundColor(QucsSettings.BGColor);
    connect(ColorAttribute, SIGNAL(clicked()), SLOT(slotColorAttribute()));
    editorGrid->addWidget(ColorAttribute,3,0);

    ColorDirective = new QPushButton(tr("Directive"), editorTab);
    ColorDirective->setPaletteForegroundColor(QucsSettings.Directive);
    ColorDirective->setPaletteBackgroundColor(QucsSettings.BGColor);
    connect(ColorDirective, SIGNAL(clicked()), SLOT(slotColorDirective()));
    editorGrid->addWidget(ColorDirective,3,1);

    ColorTask = new QPushButton(tr("Task"), editorTab);
    ColorTask->setPaletteForegroundColor(QucsSettings.Task);
    ColorTask->setPaletteBackgroundColor(QucsSettings.BGColor);
    connect(ColorTask, SIGNAL(clicked()), SLOT(slotColorTask()));
    editorGrid->addWidget(ColorTask,3,2);


    t->addTab(editorTab, tr("Source Code Editor"));

    // ...........................................................
    // The file types tab
    QWidget *fileTypesTab = new QWidget(t);
    QGridLayout *fileTypesGrid = new QGridLayout(fileTypesTab);

    QLabel *note = new QLabel(
        tr("Register filename extensions here in order to\nopen files with an appropriate program."));
    fileTypesGrid->addWidget(note,0,0,1,2);

    // the fileTypesTableWidget displays information on the file types
    fileTypesTableWidget = new QTableWidget(fileTypesTab);
    fileTypesTableWidget->setColumnCount(2);

    QTableWidgetItem *item1 = new QTableWidgetItem();
    QTableWidgetItem *item2 = new QTableWidgetItem();

    fileTypesTableWidget->setHorizontalHeaderItem(0, item1);
    fileTypesTableWidget->setHorizontalHeaderItem(1, item2);

    item1->setText(tr("Suffix"));
    item2->setText(tr("Program"));

    fileTypesTableWidget->horizontalHeader()->setStretchLastSection(true);
    fileTypesTableWidget->verticalHeader()->hide();
    connect(fileTypesTableWidget, SIGNAL(cellClicked(int,int)), SLOT(slotTableClicked(int,int)));
    fileTypesGrid->addWidget(fileTypesTableWidget,1,0,3,1);

    // fill listview with already registered file extensions
    QStringList::Iterator it = QucsSettings.FileTypes.begin();
    while(it != QucsSettings.FileTypes.end())
    {
        int row = fileTypesTableWidget->rowCount();
        fileTypesTableWidget->setRowCount(row+1);
        QTableWidgetItem *suffix = new QTableWidgetItem(QString((*it).section('/',0,0)));
        QTableWidgetItem *program = new QTableWidgetItem(QString((*it).section('/',1,1)));
        suffix->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        program->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        fileTypesTableWidget->setItem(row, 0, suffix);
        fileTypesTableWidget->setItem(row, 1, program);
        it++;
    }

    QLabel *l5 = new QLabel(tr("Suffix:"), fileTypesTab);
    fileTypesGrid->addWidget(l5,1,1);
    Input_Suffix = new QLineEdit(fileTypesTab);
    Input_Suffix->setValidator(Validator);
    fileTypesGrid->addWidget(Input_Suffix,1,2);
//  connect(Input_Suffix, SIGNAL(returnPressed()), SLOT(slotGotoProgEdit())); //not implemented

    QLabel *l6 = new QLabel(tr("Program:"), fileTypesTab);
    fileTypesGrid->addWidget(l6,2,1);
    Input_Program = new QLineEdit(fileTypesTab);
    fileTypesGrid->addWidget(Input_Program,2,2);

    QPushButton *AddButt = new QPushButton(tr("Set"));
    fileTypesGrid->addWidget(AddButt,3,1);
    connect(AddButt, SIGNAL(clicked()), SLOT(slotAddFileType()));
    QPushButton *RemoveButt = new QPushButton(tr("Remove"));
    fileTypesGrid->addWidget(RemoveButt,3,2);
    connect(RemoveButt, SIGNAL(clicked()), SLOT(slotRemoveFileType()));

    fileTypesGrid->setRowStretch(3,4);
    t->addTab(fileTypesTab, tr("File Types"));

    // ...........................................................
    // The locations tab
    QWidget *locationsTab = new QWidget(t);
    QGridLayout *locationsGrid = new QGridLayout(locationsTab);

    QLabel *note2 = new QLabel(
        tr("Edit the standard paths and external applications"));
    locationsGrid->addWidget(note2,0,0,1,2);
    locationsGrid->addWidget(new QLabel(tr("Octave path:"), locationsTab) ,1,0);

    octaveEdit = new QLineEdit(locationsTab);
    locationsGrid->addWidget(octaveEdit,1,1);
    QPushButton *OctaveButt = new QPushButton("...");
    locationsGrid->addWidget(OctaveButt, 1, 2);
    connect(OctaveButt, SIGNAL(clicked()), SLOT(slotOctaveBrowse()));

    locationsGrid->addWidget(new QLabel(tr("Qucs Home:"), locationsTab) ,2,0);
    homeEdit = new QLineEdit(locationsTab);
    locationsGrid->addWidget(homeEdit,2,1);
    QPushButton *HomeButt = new QPushButton("...");
    locationsGrid->addWidget(HomeButt, 2, 2);
    connect(HomeButt, SIGNAL(clicked()), SLOT(slotHomeDirBrowse()));

    // the pathsTableWidget displays the path list
    pathsTableWidget = new QTableWidget(locationsTab);
    pathsTableWidget->setColumnCount(1);

    QTableWidgetItem *pitem1 = new QTableWidgetItem();

    pathsTableWidget->setHorizontalHeaderItem(0, pitem1);

    pitem1->setText(tr("Subcircuit Search Path List"));

    pathsTableWidget->horizontalHeader()->setStretchLastSection(true);
    pathsTableWidget->verticalHeader()->hide();
    // allow multiple items to be selected
    pathsTableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(pathsTableWidget, SIGNAL(cellClicked(int,int)), SLOT(slotPathTableClicked(int,int)));
    locationsGrid->addWidget(pathsTableWidget,3,0,3,2);

    QPushButton *AddPathButt = new QPushButton("Add Path");
    locationsGrid->addWidget(AddPathButt, 3, 2);
    connect(AddPathButt, SIGNAL(clicked()), SLOT(slotAddPath()));

    QPushButton *AddPathSubFolButt = new QPushButton("Add Path With SubFolders");
    locationsGrid->addWidget(AddPathSubFolButt, 4, 2);
    connect(AddPathSubFolButt, SIGNAL(clicked()), SLOT(slotAddPathWithSubFolders()));

    QPushButton *RemovePathButt = new QPushButton("Remove Path");
    locationsGrid->addWidget(RemovePathButt , 5, 2);
    connect(RemovePathButt, SIGNAL(clicked()), SLOT(slotRemovePath()));

    // create a copy of the current global path list
    currentPaths = QStringList(qucsPathList);
    makePathTable();

    t->addTab(locationsTab, tr("Locations"));

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
void QucsSettingsDialog::slotAddFileType()
{
    QModelIndexList indexes = fileTypesTableWidget->selectionModel()->selection().indexes();
    if (indexes.count())
    {
        fileTypesTableWidget->item(indexes.at(0).row(),0)->setText(Input_Suffix->text());
        fileTypesTableWidget->item(indexes.at(0).row(),1)->setText(Input_Program->text());
        fileTypesTableWidget->selectionModel()->clear();
        return;
    }

    //check before append
    for(int r=0; r < fileTypesTableWidget->rowCount(); r++)
        if(fileTypesTableWidget->item(r,0)->text() == Input_Suffix->text())
        {
            QMessageBox::critical(this, tr("Error"),
                                  tr("This suffix is already registered!"));
            return;
        }

    int row = fileTypesTableWidget->rowCount();
    fileTypesTableWidget->setRowCount(row+1);

    QTableWidgetItem *newSuffix = new QTableWidgetItem(QString(Input_Suffix->text()));
    newSuffix->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    fileTypesTableWidget->setItem(row, 0, newSuffix);

    QTableWidgetItem *newProgram = new QTableWidgetItem(Input_Program->text());
    fileTypesTableWidget->setItem(row, 1, newProgram);
    newProgram->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    Input_Suffix->setFocus();
    Input_Suffix->clear();
    Input_Program->clear();
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotRemoveFileType()
{
    QModelIndexList indexes = fileTypesTableWidget->selectionModel()->selection().indexes();
    if (indexes.count())
    {
        fileTypesTableWidget->removeRow(indexes.at(0).row());
        fileTypesTableWidget->selectionModel()->clear();
        Input_Suffix->setText("");
        Input_Program->setText("");
        return;
    }
}

// -----------------------------------------------------------
// Applies any changed settings and closes the dialog
void QucsSettingsDialog::slotOK()
{
    slotApply();
    accept();
}

// -----------------------------------------------------------
// Applies any changed settings
void QucsSettingsDialog::slotApply()
{
    bool changed = false;

    if(QucsSettings.BGColor != BGColorButton->paletteBackgroundColor())
    {
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

    if(QucsSettings.Comment != ColorComment->paletteForegroundColor())
    {
        QucsSettings.Comment = ColorComment->paletteForegroundColor();
        changed = true;
    }
    if(QucsSettings.String != ColorString->paletteForegroundColor())
    {
        QucsSettings.String = ColorString->paletteForegroundColor();
        changed = true;
    }
    if(QucsSettings.Integer != ColorInteger->paletteForegroundColor())
    {
        QucsSettings.Integer = ColorInteger->paletteForegroundColor();
        changed = true;
    }
    if(QucsSettings.Real != ColorReal->paletteForegroundColor())
    {
        QucsSettings.Real = ColorReal->paletteForegroundColor();
        changed = true;
    }
    if(QucsSettings.Character != ColorCharacter->paletteForegroundColor())
    {
        QucsSettings.Character = ColorCharacter->paletteForegroundColor();
        changed = true;
    }
    if(QucsSettings.Type != ColorDataType->paletteForegroundColor())
    {
        QucsSettings.Type = ColorDataType->paletteForegroundColor();
        changed = true;
    }
    if(QucsSettings.Attribute != ColorAttribute->paletteForegroundColor())
    {
        QucsSettings.Attribute = ColorAttribute->paletteForegroundColor();
        changed = true;
    }
    if(QucsSettings.Directive != ColorDirective->paletteForegroundColor())
    {
        QucsSettings.Directive = ColorDirective->paletteForegroundColor();
        changed = true;
    }
    if(QucsSettings.Task != ColorTask->paletteForegroundColor())
    {
        QucsSettings.Task = ColorTask->paletteForegroundColor();
        changed = true;
    }

    bool ok;
    if(QucsSettings.maxUndo != undoNumEdit->text().toUInt(&ok))
    {
        QucsSettings.maxUndo = undoNumEdit->text().toInt(&ok);
        changed = true;
    }
    if(QucsSettings.Editor != editorEdit->text())
    {
        QucsSettings.Editor = editorEdit->text();
        changed = true;
    }
    if(QucsSettings.NodeWiring != (unsigned)checkWiring->isChecked())
    {
        QucsSettings.NodeWiring = checkWiring->isChecked();
        changed = true;
    }

    QucsSettings.FileTypes.clear();
    for (int row=0; row < fileTypesTableWidget->rowCount(); row++)
    {
        QucsSettings.FileTypes.append(fileTypesTableWidget->item(row,0)->text()
                                      +"/"+
                                      fileTypesTableWidget->item(row,1)->text());
    }
    QucsSettings.OctaveBinDir = octaveEdit->text();
    QucsSettings.QucsHomeDir = homeEdit->text();

    QucsSettings.IgnoreFutureVersion = checkLoadFromFutureVersions->isChecked();

    saveApplSettings(App);  // also sets the small and large font

    if(changed)
    {
        App->readProjects();
        App->readProjectFiles();
        App->repaint();
    }

    // update the schenatic filelist hash
    QucsMain->updatePathList(currentPaths);
    QucsMain->updateSchNameHash();
    QucsMain->updateSpiceNameHash();

}


// -----------------------------------------------------------
void QucsSettingsDialog::slotFontDialog()
{
    bool ok;
    QFont tmpFont = QFontDialog::getFont(&ok, Font, this);
    if(ok)
    {
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
    editorEdit->setText(QucsSettings.BinDir + "qucs");
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

void QucsSettingsDialog::slotTableClicked(int row, int col)
{
    Input_Suffix->setText(fileTypesTableWidget->item(row,0)->text());
    Input_Program->setText(fileTypesTableWidget->item(row,1)->text());
}

// -----------------------------------------------------------
// The locations tab slots

void QucsSettingsDialog::slotOctaveBrowse()
{
    QFileDialog fileDialog( this, tr("Select the octave bin directory"), octaveEdit->text() );
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::DirectoryOnly);
    fileDialog.exec();
    octaveEdit->setText(fileDialog.selectedFile());
}


void QucsSettingsDialog::slotHomeDirBrowse()
{
    QFileDialog fileDialog( this, tr("Select the home directory"), homeEdit->text() );
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::DirectoryOnly);
    fileDialog.exec();
    homeEdit->setText(fileDialog.selectedFile());
}

void QucsSettingsDialog::slotPathTableClicked(int row, int col)
{
    //Input_Path->setText(fileTypesTableWidget->item(row,0)->text());
}

void QucsSettingsDialog::slotAddPath()
{
    QFileDialog fileDialog( this, tr("Select a directory"), QucsSettings.QucsWorkDir.canonicalPath());
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::DirectoryOnly);

    if (fileDialog.exec())
    {
        currentPaths.append(fileDialog.selectedFile());
        // reconstruct the table again
        makePathTable();
    }
    else
    {
        // user cancelled
    }
}

void QucsSettingsDialog::slotAddPathWithSubFolders()
{
    // open a file dialog to select the top level directory
    QFileDialog fileDialog( this, tr("Select a directory"), QucsSettings.QucsWorkDir.canonicalPath());
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::DirectoryOnly);

    QString path;
    QFileInfo pathfinfo;

    if (fileDialog.exec())
    {
        // Iterate through the directories
        QDirIterator pathIter(fileDialog.selectedFile(), QDirIterator::Subdirectories);
        while (pathIter.hasNext())
        {
            path = pathIter.next();
            pathfinfo = pathIter.fileInfo();

            if (pathfinfo.isDir() & !pathfinfo.isSymLink() & !path.endsWith("."))
            {
                QDir thispath(path);
                currentPaths.append(thispath.canonicalPath());
            }
        }
        makePathTable();
    }
    else
    {
        // user cancelled
    }


}

void QucsSettingsDialog::slotRemovePath()
{
    //Input_Path->setText(fileTypesTableWidget->item(row,0)->text());
    // get the selected items from the table
    QList<QTableWidgetItem *> selectedPaths = pathsTableWidget->selectedItems();

    foreach (QTableWidgetItem * item, selectedPaths)
    {
        QString path = item->text();
        //removedPaths.append(path);
        int pathind = currentPaths.indexOf(path,0);
        if (pathind != -1)
        {
            currentPaths.removeAt(pathind);
        }
    }

    makePathTable();
}

// makePathTable()
//
// Reconstructs the table containing the list of search paths
// in the locations tab
void QucsSettingsDialog::makePathTable()
{
    // remove all the paths from the table if present
    pathsTableWidget->clearContents();
    pathsTableWidget->setRowCount(0);

    // fill listview with the list of paths
    foreach (QString pathstr, currentPaths)
    {
        int row = pathsTableWidget->rowCount();
        pathsTableWidget->setRowCount(row+1);
        QTableWidgetItem *path = new QTableWidgetItem(pathstr);
        path->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        pathsTableWidget->setItem(row, 0, path);
    }
}
