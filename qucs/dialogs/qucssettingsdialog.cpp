/***************************************************************************
                           qucssettingsdialog.cpp
                          ------------------------
    begin                : Sun May 23 2004
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
    copyright            : (C) 2016 by Qucs Team (see AUTHORS file)

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*!
 * \file qucssettingsdialog.cpp
 * \brief Implementation of the Application Settings dialog
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "qucssettingsdialog.h"
#include <iostream>
#include <cmath>
#include <QGridLayout>
#include <QVBoxLayout>
#include "main.h"
#include "textdoc.h"
#include "schematic.h"
#include "settings.h"

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
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QFileDialog>
#include <QDirIterator>
#include <QDebug>
#include <QObject>
#include <QString>
#include <QList>

using namespace std;

auto getFontDescription = [](const auto& Font) -> QString {
    const QChar comma(u',');
    QString fontDescription = Font.family() + comma +
        QString::number(Font.pointSize());

    QString fontStyle = Font.styleName();
    if (!fontStyle.isEmpty())
        fontDescription += comma + fontStyle;

    return fontDescription;
};

QucsSettingsDialog::QucsSettingsDialog(QucsApp *parent)
    : QDialog(parent)
{
    App = parent;
    setWindowTitle(tr("Edit Qucs Properties"));

    Expr.setPattern("[\\w_]+");
    Validator  = new QRegularExpressionValidator(Expr, this);

    all = new QVBoxLayout(this); // to provide the necessary size
    QTabWidget *t = new QTabWidget();
    all->addWidget(t);

    // ...........................................................
    // The application settings tab
    QWidget *appSettingsTab = new QWidget(t);
    QGridLayout *appSettingsGrid = new QGridLayout(appSettingsTab);

    const QStringList appLanguages = {
        tr("system language"),
        tr("English") + " (en)",
        tr("Arabic") + " (ar)",
        tr("Catalan") + " (ca)",
        tr("Chinese") + " (zh_CN)",
        tr("Czech") + " (cs)",
        tr("French") + " (fr)",
        tr("German") + " (de)",
        tr("Hebrew") + " (he)",
        tr("Hungarian") + " (hu)",
        tr("Italian") + " (it)",
        tr("Japanese") + " (jp)",
        tr("Kazakh") + " (kk)",
        tr("Polish") + " (pl)",
        tr("Portuguese-BR") + " (pt_BR)",
        tr("Portuguese-PT") + " (pt_PT)",
        tr("Romanian") + " (ro)",
        tr("Russian") + " (ru)",
        tr("Spanish") + " (es)",
        tr("Swedish") + " (sv)",
        tr("Turkish") + " (tr)",
        tr("Ukrainian") + " (uk)"
    };

    appSettingsGrid->addWidget(new QLabel(tr("Language (set after reload):"), appSettingsTab) ,1, 0);
    LanguageCombo = new QComboBox(appSettingsTab);
    LanguageCombo->addItems(appLanguages);
    appSettingsGrid->addWidget(LanguageCombo, 1, 1);

    val200 = new QIntValidator(0, 200, this);
    appSettingsGrid->addWidget(new QLabel(tr("Maximum undo operations:"), appSettingsTab) ,2, 0);
    undoNumEdit = new QLineEdit(appSettingsTab);
    undoNumEdit->setValidator(val200);
    appSettingsGrid->addWidget(undoNumEdit, 2, 1);

    appSettingsGrid->addWidget(new QLabel(tr("Text editor:"), appSettingsTab), 3, 0);
    editorEdit = new QLineEdit(appSettingsTab);
    editorEdit->setToolTip(tr("Set to qucs, qucsedit or the path to your favorite text editor."));
    appSettingsGrid->addWidget(editorEdit, 3, 1);

    appSettingsGrid->addWidget(new QLabel(tr("Start wiring when clicking open node:"), appSettingsTab), 4, 0);
    checkWiring = new QCheckBox(appSettingsTab);
    appSettingsGrid->addWidget(checkWiring, 4, 1);

    appSettingsGrid->addWidget(new QLabel(tr("Load documents from future versions:")), 5, 0);
    checkLoadFromFutureVersions = new QCheckBox(appSettingsTab);
    checkLoadFromFutureVersions->setToolTip(tr("Try to load also documents created with newer versions of Qucs."));
    appSettingsGrid->addWidget(checkLoadFromFutureVersions, 5, 1);
    checkLoadFromFutureVersions->setChecked(QucsSettings.IgnoreFutureVersion);

    appSettingsGrid->addWidget(new QLabel(tr("Show trace name prefix on diagrams:")), 6, 0);
    checkFullTraceNames = new QCheckBox(appSettingsTab);
    checkFullTraceNames->setToolTip(tr("Show prefixes for trace names on diagrams like \"ngspice/\""));
    appSettingsGrid->addWidget(checkFullTraceNames, 6, 1);
    checkFullTraceNames->setChecked(QucsSettings.fullTraceName);

    t->addTab(appSettingsTab, tr("Settings"));

    // ...........................................................
    // The appearance settings tab
    QWidget *appAppearanceTab = new QWidget(t);
    QGridLayout *appAppearanceGrid = new QGridLayout(appAppearanceTab);

    appAppearanceGrid->addWidget(new QLabel(tr("Schematic font (set after reload):"), appSettingsTab), 0, 0);
    FontButton = new QPushButton(appSettingsTab);
    connect(FontButton, SIGNAL(clicked()), SLOT(slotFontDialog()));
    appAppearanceGrid->addWidget(FontButton, 0, 1);

    appAppearanceGrid->addWidget(new QLabel(tr("Application font (set after reload):"), appSettingsTab), 1, 0);
    AppFontButton = new QPushButton(appSettingsTab);
    connect(AppFontButton, SIGNAL(clicked()), SLOT(slotAppFontDialog()));
    appAppearanceGrid->addWidget(AppFontButton, 1, 1);

    appAppearanceGrid->addWidget(new QLabel(tr("Text document font (set after reload):"), appSettingsTab), 2, 0);
    TextFontButton = new QPushButton(appSettingsTab);
    connect(TextFontButton, SIGNAL(clicked()), SLOT(slotTextFontDialog()));
    appAppearanceGrid->addWidget(TextFontButton, 2, 1);

    val50 = new QIntValidator(1, 50, this);
    appAppearanceGrid->addWidget(new QLabel(tr("Large font size:"), appSettingsTab), 3, 0);
    LargeFontSizeEdit = new QLineEdit(appSettingsTab);
    LargeFontSizeEdit->setValidator(val50);
    appAppearanceGrid->addWidget(LargeFontSizeEdit, 3, 1);

    appAppearanceGrid->addWidget(new QLabel(tr("Document Background Color:"), appSettingsTab) ,4, 0);
    BGColorButton = new QPushButton("      ", appSettingsTab);
    connect(BGColorButton, SIGNAL(clicked()), SLOT(slotBGColorDialog()));
    appAppearanceGrid->addWidget(BGColorButton, 4, 1);

    appAppearanceGrid->addWidget(new QLabel(tr("Grid Color (set after reload):"), appSettingsTab) ,5, 0);
    GridColorButton = new QPushButton("      ", appSettingsTab);
    connect(GridColorButton, SIGNAL(clicked()), SLOT(slotGridColorDialog()));
    appAppearanceGrid->addWidget(GridColorButton, 5, 1);

    appAppearanceGrid->addWidget(new QLabel(tr("Draw diagrams with anti-aliasing feature:")), 6, 0);
    checkAntiAliasing = new QCheckBox(appSettingsTab);
    checkAntiAliasing->setToolTip(tr("Use anti-aliasing for graphs for a smoother appearance."));
    appAppearanceGrid->addWidget(checkAntiAliasing, 6, 1);
    checkAntiAliasing->setChecked(QucsSettings.GraphAntiAliasing);

    appAppearanceGrid->addWidget(new QLabel(tr("Draw text with anti-aliasing feature:")), 7, 0);
    checkTextAntiAliasing = new QCheckBox(appSettingsTab);
    checkTextAntiAliasing->setToolTip(tr("Use anti-aliasing for text for a smoother appearance."));
    appAppearanceGrid->addWidget(checkTextAntiAliasing, 7, 1);
    checkTextAntiAliasing->setChecked(QucsSettings.TextAntiAliasing);

    appAppearanceGrid->addWidget(new QLabel(tr("Default graph line thickness:"), appSettingsTab), 8, 0);
    graphLineWidthEdit = new QLineEdit(appSettingsTab);
    graphLineWidthEdit->setValidator(val50);
    appAppearanceGrid->addWidget(graphLineWidthEdit, 8, 1);

    appAppearanceGrid->addWidget(new QLabel(tr("App Style:"), appSettingsTab), 9, 0);
    QStringList styles = QStyleFactory::keys(); // Get available styles
    StyleCombo = new QComboBox(appSettingsTab);
    StyleCombo->addItems(styles);
    appAppearanceGrid->addWidget(StyleCombo,9,1);


    // Retrieve the current style and set it as selected
    QString currentStyle = QApplication::style()->objectName();
    int index = StyleCombo->findText(currentStyle, Qt::MatchFixedString);
    if (index != -1) {
        StyleCombo->setCurrentIndex(index);
    }      

    t->addTab(appAppearanceTab, tr("Appearance"));

    // ...........................................................
    // The source code editor settings tab
    QWidget *editorTab = new QWidget(t);
    QGridLayout *editorGrid = new QGridLayout(editorTab);

    editorGrid->addWidget(new QLabel(tr("Colors for Syntax Highlighting:"), editorTab), 0, 1, 0, 2);

    QPalette p;

    ColorComment = new QPushButton(tr("Comment"), editorTab);
    p = ColorComment->palette();
    p.setColor(ColorComment->foregroundRole(), QucsSettings.Comment);
    p.setColor(ColorComment->backgroundRole(), QucsSettings.BGColor);
    ColorComment->setPalette(p);
    connect(ColorComment, SIGNAL(clicked()), SLOT(slotColorComment()));
    editorGrid->addWidget(ColorComment,1,0);

    ColorString = new QPushButton(tr("String"), editorTab);
    p = ColorString->palette();
    p.setColor(ColorString->foregroundRole(), QucsSettings.String);
    p.setColor(ColorString->backgroundRole(), QucsSettings.BGColor);
    ColorString->setPalette(p);
    connect(ColorString, SIGNAL(clicked()), SLOT(slotColorString()));
    editorGrid->addWidget(ColorString,1,1);

    ColorInteger = new QPushButton(tr("Integer Number"), editorTab);
    p = ColorInteger->palette();
    p.setColor(ColorInteger->foregroundRole(), QucsSettings.Integer);
    p.setColor(ColorInteger->backgroundRole(), QucsSettings.BGColor);
    ColorInteger->setPalette(p);
    connect(ColorInteger, SIGNAL(clicked()), SLOT(slotColorInteger()));
    editorGrid->addWidget(ColorInteger,1,2);

    ColorReal = new QPushButton(tr("Real Number"), editorTab);
    p = ColorReal->palette();
    p.setColor(ColorReal->foregroundRole(), QucsSettings.Real);
    p.setColor(ColorReal->backgroundRole(), QucsSettings.BGColor);
    ColorReal->setPalette(p);
    connect(ColorReal, SIGNAL(clicked()), SLOT(slotColorReal()));
    editorGrid->addWidget(ColorReal,2,0);

    ColorCharacter = new QPushButton(tr("Character"), editorTab);
    p = ColorCharacter->palette();
    p.setColor(ColorCharacter->foregroundRole(), QucsSettings.Character);
    p.setColor(ColorCharacter->backgroundRole(), QucsSettings.BGColor);
    ColorCharacter->setPalette(p);
    connect(ColorCharacter, SIGNAL(clicked()), SLOT(slotColorCharacter()));
    editorGrid->addWidget(ColorCharacter,2,1);

    ColorDataType = new QPushButton(tr("Data Type"), editorTab);
    p = ColorDataType->palette();
    p.setColor(ColorDataType->foregroundRole(), QucsSettings.Type);
    p.setColor(ColorDataType->backgroundRole(), QucsSettings.BGColor);
    ColorDataType->setPalette(p);
    connect(ColorDataType, SIGNAL(clicked()), SLOT(slotColorDataType()));
    editorGrid->addWidget(ColorDataType,2,2);

    ColorAttribute = new QPushButton(tr("Attribute"), editorTab);
    p = ColorAttribute->palette();
    p.setColor(ColorAttribute->foregroundRole(), QucsSettings.Attribute);
    p.setColor(ColorAttribute->backgroundRole(), QucsSettings.BGColor);
    ColorAttribute->setPalette(p);
    connect(ColorAttribute, SIGNAL(clicked()), SLOT(slotColorAttribute()));
    editorGrid->addWidget(ColorAttribute,3,0);

    ColorDirective = new QPushButton(tr("Directive"), editorTab);
    p = ColorDirective->palette();
    p.setColor(ColorDirective->foregroundRole(), QucsSettings.Directive);
    p.setColor(ColorDirective->backgroundRole(), QucsSettings.BGColor);
    ColorDirective->setPalette(p);
    connect(ColorDirective, SIGNAL(clicked()), SLOT(slotColorDirective()));
    editorGrid->addWidget(ColorDirective,3,1);

    ColorTask = new QPushButton(tr("Task"), editorTab);
    p = ColorTask->palette();
    p.setColor(ColorTask->foregroundRole(), QucsSettings.Task);
    p.setColor(ColorTask->backgroundRole(), QucsSettings.BGColor);
    ColorTask->setPalette(p);
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
    fileTypesTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    fileTypesTableWidget->horizontalHeader()->setSectionsClickable(false); // no action when clicking on the header
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

    locationsGrid->addWidget(new QLabel(tr("Qucs Home:"), locationsTab) ,1,0);
    homeEdit = new QLineEdit(locationsTab);
    locationsGrid->addWidget(homeEdit,1,1);
    QPushButton *HomeButt = new QPushButton("Browse");
    locationsGrid->addWidget(HomeButt, 1, 2);
    connect(HomeButt, SIGNAL(clicked()), SLOT(slotHomeDirBrowse()));

    locationsGrid->addWidget(new QLabel(tr("AdmsXml Path:"), locationsTab) ,2,0);
    admsXmlEdit = new QLineEdit(locationsTab);
    locationsGrid->addWidget(admsXmlEdit,2,1);
    QPushButton *AdmsXmlButt = new QPushButton("Browse");
    locationsGrid->addWidget(AdmsXmlButt, 2, 2);
    connect(AdmsXmlButt, SIGNAL(clicked()), SLOT(slotAdmsXmlDirBrowse()));

    locationsGrid->addWidget(new QLabel(tr("ASCO Path:"), locationsTab) ,3,0);
    ascoEdit = new QLineEdit(locationsTab);
    locationsGrid->addWidget(ascoEdit,3,1);
    QPushButton *ascoButt = new QPushButton("Browse");
    locationsGrid->addWidget(ascoButt, 3, 2);
    connect(ascoButt, SIGNAL(clicked()), SLOT(slotAscoDirBrowse()));

    locationsGrid->addWidget(new QLabel(tr("Octave Path:"), locationsTab) ,4,0);
    octaveEdit = new QLineEdit(locationsTab);
    locationsGrid->addWidget(octaveEdit,4,1);
    QPushButton *OctaveButt = new QPushButton("Browse");
    locationsGrid->addWidget(OctaveButt, 4, 2);
    connect(OctaveButt, SIGNAL(clicked()), SLOT(slotOctaveDirBrowse()));

    locationsGrid->addWidget(new QLabel(tr("OpenVAF Path:"), locationsTab) ,5,0);
    OpenVAFEdit = new QLineEdit(locationsTab);
    locationsGrid->addWidget(OpenVAFEdit,5,1);
    QPushButton *OpenVAFButt = new QPushButton("Browse");
    locationsGrid->addWidget(OpenVAFButt, 5, 2);
    connect(OpenVAFButt, SIGNAL(clicked()), SLOT(slotOpenVAFDirBrowse()));

    locationsGrid->addWidget(new QLabel(tr("RF Layout Path:"), locationsTab) ,6,0);
    RFLayoutEdit = new QLineEdit(locationsTab);
    locationsGrid->addWidget(RFLayoutEdit,6,1);
    QPushButton *RFLButt = new QPushButton("Browse");
    locationsGrid->addWidget(RFLButt, 6, 2);
    connect(RFLButt, SIGNAL(clicked()), SLOT(slotRFLayoutDirBrowse()));

    locationsGrid->addWidget(new QLabel(tr("PDKs root:"), locationsTab), 7, 0);
    PDKDirEdit = new QLineEdit(locationsTab);
    locationsGrid->addWidget(PDKDirEdit, 7,1);
    QPushButton *PDK_Butt = new QPushButton("Browse");
    locationsGrid->addWidget(PDK_Butt, 7, 2);
    connect(PDK_Butt, SIGNAL(clicked()), SLOT(slotPDKDirBrowse()));

    locationsGrid->addWidget(new QLabel(tr("OSDI files directory:"), locationsTab), 8, 0);
    OSDIDirEdit = new QLineEdit(locationsTab);
    locationsGrid->addWidget(OSDIDirEdit, 8, 1);
    QPushButton *OSDI_Butt = new QPushButton("Browse");
    locationsGrid->addWidget(OSDI_Butt, 8, 2);
    connect(OSDI_Butt, SIGNAL(clicked()), SLOT(slotOSDI_Files_DirBrowse()));


    // the pathsTableWidget displays the path list
    pathsTableWidget = new QTableWidget(locationsTab);
    pathsTableWidget->setColumnCount(1);

    QTableWidgetItem *pitem1 = new QTableWidgetItem();

    pathsTableWidget->setHorizontalHeaderItem(0, pitem1);

    pitem1->setText(tr("Subcircuit Search Path List"));

    pathsTableWidget->horizontalHeader()->setStretchLastSection(true);
    // avoid drawing header text in bold when some data is selected
    pathsTableWidget->horizontalHeader()->setSectionsClickable(false);

    pathsTableWidget->verticalHeader()->hide();
    // allow multiple items to be selected
    pathsTableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(pathsTableWidget, SIGNAL(cellClicked(int,int)), SLOT(slotPathTableClicked(int,int)));
    connect(pathsTableWidget, SIGNAL(itemSelectionChanged()), SLOT(slotPathSelectionChanged()));
    locationsGrid->addWidget(pathsTableWidget,9,0,3,2);

    QPushButton *AddPathButt = new QPushButton("Add Path");
    locationsGrid->addWidget(AddPathButt, 9, 2);
    connect(AddPathButt, SIGNAL(clicked()), SLOT(slotAddPath()));

    QPushButton *AddPathSubFolButt = new QPushButton("Add Path With SubFolders");
    locationsGrid->addWidget(AddPathSubFolButt, 10, 2);
    connect(AddPathSubFolButt, SIGNAL(clicked()), SLOT(slotAddPathWithSubFolders()));

    RemovePathButt = new QPushButton("Remove Path");
    // disable button if no paths in the table are selected
    RemovePathButt->setEnabled(false);
    locationsGrid->addWidget(RemovePathButt , 11, 2);
    connect(RemovePathButt, SIGNAL(clicked()), SLOT(slotRemovePath()));

    // create a copy of the current global path list
    currentPaths = QStringList(qucsPathList);
    makePathTable();

    t->addTab(locationsTab, tr("Locations"));

    // ...........................................................
    // buttons on the bottom of the dialog (independent of the TabWidget)

    QHBoxLayout *Butts = new QHBoxLayout();
    Butts->setSpacing(3);
    Butts->setContentsMargins(3,3,3,3);
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
    AppFont = QucsSettings.appFont;
    TextFont = QucsSettings.textFont;



    FontButton->setText(getFontDescription(Font));
    AppFontButton->setText(getFontDescription(AppFont));
    TextFontButton->setText(getFontDescription(TextFont));
    QString s = QString::number(QucsSettings.largeFontSize, 'f', 1);
    LargeFontSizeEdit->setText(s);
    graphLineWidthEdit->setText(_settings::Get().item<QString>("DefaultGraphLineWidth"));

    p = BGColorButton->palette();
    p.setColor(BGColorButton->backgroundRole(), QucsSettings.BGColor);
    BGColorButton->setPalette(p);

    p = GridColorButton->palette();
    p.setColor(GridColorButton->backgroundRole(), _settings::Get().item<QColor>("GridColor"));
    GridColorButton->setPalette(p);

    undoNumEdit->setText(QString::number(QucsSettings.maxUndo));
    editorEdit->setText(QucsSettings.Editor);
    checkWiring->setChecked(QucsSettings.NodeWiring);

    for(int z=LanguageCombo->count()-1; z>=0; z--)
        if(LanguageCombo->itemText(z).section('(',1,1).remove(')') == QucsSettings.Language)
            LanguageCombo->setCurrentIndex(z);

    /*! Load paths from settings */
    homeEdit->setText(QucsSettings.qucsWorkspaceDir.canonicalPath());
    admsXmlEdit->setText(QucsSettings.AdmsXmlBinDir.canonicalPath());
    ascoEdit->setText(QucsSettings.AscoBinDir.canonicalPath());
    octaveEdit->setText(QucsSettings.OctaveExecutable);
    OpenVAFEdit->setText(QucsSettings.OpenVAFExecutable);
    RFLayoutEdit->setText(QucsSettings.RFLayoutExecutable);
    PDKDirEdit->setText(QucsSettings.PDKDir.canonicalPath());
    OSDIDirEdit->setText(QucsSettings.OSDI_FilesPath.canonicalPath());

    resize(300, 200);
}

QucsSettingsDialog::~QucsSettingsDialog()
{
    delete all;
    delete val50;
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
/// \todo simplify the conditionals involving `changed = true`
///  if user hit apply, save settings and refresh everything
void QucsSettingsDialog::slotApply()
{
    bool changed = false;
    bool homeDirChanged = false;

    // check QucsHome is changed, will require to close all files and refresh tree
    if (homeEdit->text() != QucsSettings.qucsWorkspaceDir.path()) {
      // close all open files, asking the user whether to save the modified ones
      // if user aborts closing, just return
      if(!App->closeAllFiles()) return;

      QucsSettings.qucsWorkspaceDir.setPath(homeEdit->text());
      homeDirChanged = true;
      // later below the file tree will be refreshed
    }

    if(QucsSettings.BGColor != BGColorButton->palette().color(BGColorButton->backgroundRole()))
    {
        QucsSettings.BGColor = BGColorButton->palette().color(BGColorButton->backgroundRole());

        int No=0;
        QWidget *w;

        while((w=App->DocumentTab->widget(No++)) != 0) {
          QWidget *vp;
          if(QucsApp::isTextDocument(w)) {
            vp = ((TextDoc*)w)->viewport();
          } else {
            vp = ((Schematic*)w)->viewport();
          }
          QPalette p = vp->palette();
          p.setColor(vp->backgroundRole(), QucsSettings.BGColor);
          vp->setPalette(p);

        }
        changed = true;
    }

    QString selectedStyle = StyleCombo->currentText();
    if (_settings::Get().item<QString>("AppStyle") != selectedStyle )
    {
        QStyle* style = QStyleFactory::create(selectedStyle);
        if (style) {
          QApplication::setStyle(style);
          _settings::Get().setItem<QString>("AppStyle",  selectedStyle);
          changed = true;  
        } 
    }

    // Update all open schematics with the new grid color.
    if (_settings::Get().item<QColor>("GridColor") != GridColorButton->palette().color(GridColorButton->backgroundRole())) {
        _settings::Get().setItem<QColor>("GridColor", GridColorButton->palette().color(GridColorButton->backgroundRole()));

        for (int tab = 0; tab < App->DocumentTab->count(); tab++) {
            QWidget* widget = App->DocumentTab->widget(tab);
            if (!QucsApp::isTextDocument(widget)) {
                static_cast<Schematic*>(widget)->setGridColor(_settings::Get().item<QColor>("GridColor"));
            }
        }

        changed = true;
    }

    QucsSettings.font=Font;
    QucsSettings.appFont = AppFont;
    QucsSettings.textFont = TextFont;

    QucsSettings.Language =
        LanguageCombo->currentText().section('(',1,1).remove(')');

    if(QucsSettings.Comment != ColorComment->palette().color(ColorComment->foregroundRole()))
    {
        QucsSettings.Comment = ColorComment->palette().color(ColorComment->foregroundRole());
        changed = true;
    }
    if(QucsSettings.String != ColorString->palette().color(ColorString->foregroundRole()))
    {
        QucsSettings.String = ColorString->palette().color(ColorString->foregroundRole());
        changed = true;
    }
    if(QucsSettings.Integer != ColorInteger->palette().color(ColorInteger->foregroundRole()))
    {
        QucsSettings.Integer = ColorInteger->palette().color(ColorInteger->foregroundRole());
        changed = true;
    }
    if(QucsSettings.Real != ColorReal->palette().color(ColorReal->foregroundRole()))
    {
        QucsSettings.Real = ColorReal->palette().color(ColorReal->foregroundRole());
        changed = true;
    }
    if(QucsSettings.Character != ColorCharacter->palette().color(ColorCharacter->foregroundRole()))
    {
        QucsSettings.Character = ColorCharacter->palette().color(ColorCharacter->foregroundRole());
        changed = true;
    }
    if(QucsSettings.Type != ColorDataType->palette().color(ColorDataType->foregroundRole()))
    {
        QucsSettings.Type = ColorDataType->palette().color(ColorDataType->foregroundRole());
        changed = true;
    }
    if(QucsSettings.Attribute != ColorAttribute->palette().color(ColorAttribute->foregroundRole()))
    {
        QucsSettings.Attribute = ColorAttribute->palette().color(ColorAttribute->foregroundRole());
        changed = true;
    }
    if(QucsSettings.Directive != ColorDirective->palette().color(ColorDirective->foregroundRole()))
    {
        QucsSettings.Directive = ColorDirective->palette().color(ColorDirective->foregroundRole());
        changed = true;
    }
    if(QucsSettings.Task != ColorTask->palette().color(ColorTask->foregroundRole()))
    {
        QucsSettings.Task = ColorTask->palette().color(ColorTask->foregroundRole());
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

    /*! Update QucsSettings, tool paths */
    QucsSettings.AdmsXmlBinDir.setPath(admsXmlEdit->text());
    QucsSettings.AscoBinDir.setPath(ascoEdit->text());
    QucsSettings.OctaveExecutable = octaveEdit->text();
    QucsSettings.OpenVAFExecutable = OpenVAFEdit->text();
    QucsSettings.RFLayoutExecutable = RFLayoutEdit->text();
    QucsSettings.PDKDir = PDKDirEdit->text();
    QucsSettings.OSDI_FilesPath = OSDIDirEdit->text();

    if (QucsSettings.IgnoreFutureVersion != checkLoadFromFutureVersions->isChecked())
    {
      QucsSettings.IgnoreFutureVersion = checkLoadFromFutureVersions->isChecked();
      changed = true;
    }

    if (QucsSettings.GraphAntiAliasing != checkAntiAliasing->isChecked())
    {
      QucsSettings.GraphAntiAliasing = checkAntiAliasing->isChecked();
      changed = true;
    }

    if (QucsSettings.TextAntiAliasing != checkTextAntiAliasing->isChecked())
    {
      QucsSettings.TextAntiAliasing = checkTextAntiAliasing->isChecked();
      changed = true;
    }

    if (QucsSettings.fullTraceName != checkFullTraceNames->isChecked())
    {
      QucsSettings.fullTraceName = checkFullTraceNames->isChecked();
      changed = true;
    }

    // use toDouble() as it can interpret the string according to the current locale
    if (QucsSettings.largeFontSize != LargeFontSizeEdit->text().toDouble(&ok))
    {
        QucsSettings.largeFontSize = LargeFontSizeEdit->text().toDouble(&ok);
        changed = true;
    }

    if (_settings::Get().item<QString>("DefaultGraphLineWidth") != graphLineWidthEdit->text())
    {
        _settings::Get().setItem<QString>("DefaultGraphLineWidth", graphLineWidthEdit->text());
        changed = true;
    }

    saveApplSettings();  // also sets the small and large font

    // if QucsHome is changed, refresh projects tree
    // do this after updating the other paths
    if (homeDirChanged) {;
      // files were actuallt closed above, this will refresh the projects tree
      // and create an empty schematic
      App->slotMenuProjClose();
      changed = true;
    }

    if(changed)
    {
        App->readProjects();
        App->slotUpdateTreeview();
        App->repaint();
    }

    // update the schenatic filelist hash
    QucsMain->updatePathList(currentPaths);
    //QucsMain->updateSchNameHash();
    //QucsMain->updateSpiceNameHash();

}


// -----------------------------------------------------------
void QucsSettingsDialog::slotFontDialog()
{
    bool ok;
    QFont tmpFont = QFontDialog::getFont(&ok, Font, this);
    if(ok)
    {
        Font = tmpFont;
        FontButton->setText(getFontDescription(Font));
    }
}

void QucsSettingsDialog::slotAppFontDialog()
{
    bool ok;
    QFont tmpFont = QFontDialog::getFont(&ok, AppFont, this);
    if(ok)
    {
        AppFont = tmpFont;
        AppFontButton->setText(getFontDescription(AppFont));
    }
}

void QucsSettingsDialog::slotTextFontDialog()
{
    bool ok;
    QFont tmpFont = QFontDialog::getFont(&ok, TextFont, this);
    if(ok)
    {
        TextFont = tmpFont;
        TextFontButton->setText(getFontDescription(TextFont));
    }
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotBGColorDialog()
{
    QColor c = QColorDialog::getColor(
                   BGColorButton->palette().color(BGColorButton->foregroundRole()),
                   this);
    if(c.isValid()) {
        QPalette p = BGColorButton->palette();
        p.setColor(BGColorButton->backgroundRole(), c);
        BGColorButton->setPalette(p);
    }
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotGridColorDialog()
{
    QColor c = QColorDialog::getColor(
                   GridColorButton->palette().color(GridColorButton->foregroundRole()),
                   this);
    if(c.isValid()) {
        QPalette p = GridColorButton->palette();
        p.setColor(GridColorButton->backgroundRole(), c);
        GridColorButton->setPalette(p);
    }
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotDefaultValues()
{
    QPalette p;
    Font = QApplication::font();
    AppFont = QucsSettings.sysDefaultFont;
    TextFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    FontButton->setText(getFontDescription(Font));
    AppFontButton->setText(getFontDescription(AppFont));
    TextFontButton->setText(getFontDescription(TextFont));
    LargeFontSizeEdit->setText(QString::number(16.0));

    LanguageCombo->setCurrentIndex(0);

    p = BGColorButton->palette();
    p.setColor(BGColorButton->backgroundRole(), QColor(255,250,225));
    BGColorButton->setPalette(p);

    p = ColorComment->palette();
    p.setColor(ColorComment->foregroundRole(), Qt::gray);
    ColorComment->setPalette(p);

    p = ColorString->palette();
    p.setColor(ColorString->foregroundRole(), Qt::red);
    ColorString->setPalette(p);

    p = ColorInteger->palette();
    p.setColor(ColorInteger->foregroundRole(), Qt::blue);
    ColorInteger->setPalette(p);

    p = ColorReal->palette();
    p.setColor(ColorReal->foregroundRole(), Qt::darkMagenta);
    ColorReal->setPalette(p);

    p = ColorCharacter->palette();
    p.setColor(ColorCharacter->foregroundRole(), Qt::magenta);
    ColorCharacter->setPalette(p);

    p = ColorDataType->palette();
    p.setColor(ColorDataType->foregroundRole(), Qt::darkRed);
    ColorDataType->setPalette(p);

    p = ColorAttribute->palette();
    p.setColor(ColorAttribute->foregroundRole(), Qt::darkCyan);
    ColorAttribute->setPalette(p);

    p = ColorDirective->palette();
    p.setColor(ColorDirective->foregroundRole(), Qt::darkCyan);
    ColorDirective->setPalette(p);

    p = ColorTask->palette();
    p.setColor(ColorTask->foregroundRole(), Qt::darkRed);
    ColorTask->setPalette(p);

    undoNumEdit->setText("20");
    editorEdit->setText(QucsSettings.BinDir + "qucs");
    checkWiring->setChecked(false);
    checkLoadFromFutureVersions->setChecked(false);
    checkAntiAliasing->setChecked(false);
    checkTextAntiAliasing->setChecked(true);
    checkFullTraceNames->setChecked(false);
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotColorComment()
{
    QColor c = QColorDialog::getColor(
                 ColorComment->palette().color(ColorComment->foregroundRole()),
                 this);
    if(c.isValid()) {
        QPalette p = ColorComment->palette();
        p.setColor(ColorComment->foregroundRole(), c);
        ColorComment->setPalette(p);
    }
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotColorString()
{
    QColor c = QColorDialog::getColor(
                 ColorString->palette().color(ColorString->foregroundRole()),
                 this);
    if(c.isValid()) {
        QPalette p = ColorString->palette();
        p.setColor(ColorString->foregroundRole(), c);
        ColorString->setPalette(p);
    }
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotColorInteger()
{
    QColor c = QColorDialog::getColor(
                 ColorInteger->palette().color(ColorInteger->foregroundRole()),
                 this);
    if(c.isValid()) {
        QPalette p = ColorInteger->palette();
        p.setColor(ColorInteger->foregroundRole(), c);
        ColorInteger->setPalette(p);
    }
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotColorReal()
{
    QColor c = QColorDialog::getColor(
                 ColorReal->palette().color(ColorReal->foregroundRole()),
                 this);
    if(c.isValid()) {
         QPalette p = ColorReal->palette();
         p.setColor(ColorReal->foregroundRole(), c);
         ColorReal->setPalette(p);
    }
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotColorCharacter()
{
    QColor c = QColorDialog::getColor(
                 ColorCharacter->palette().color(ColorCharacter->foregroundRole()),
                 this);
    if(c.isValid()) {
        QPalette p = ColorCharacter->palette();
        p.setColor(ColorCharacter->foregroundRole(), c);
        ColorCharacter->setPalette(p);
    }
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotColorDataType()
{
    QColor c = QColorDialog::getColor(
                 ColorDataType->palette().color(ColorDataType->foregroundRole()),
                 this);
    if(c.isValid()) {
        QPalette p = ColorDataType->palette();
        p.setColor(ColorDataType->foregroundRole(), c);
        ColorDataType->setPalette(p);
    }
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotColorAttribute()
{
    QColor c = QColorDialog::getColor(
                 ColorAttribute->palette().color(ColorAttribute->foregroundRole()),
                 this);
    if(c.isValid()) {
        QPalette p = ColorAttribute->palette();
        p.setColor(ColorAttribute->foregroundRole(), c);
        ColorAttribute->setPalette(p);
    }
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotColorDirective()
{
    QColor c = QColorDialog::getColor(
                 ColorDirective->palette().color(ColorDirective->foregroundRole()),
                 this);
    if(c.isValid()) {
        QPalette p = ColorDirective->palette();
        p.setColor(ColorDirective->foregroundRole(), c);
        ColorDirective->setPalette(p);
    }
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotColorTask()
{
    QColor c = QColorDialog::getColor(
                 ColorTask->palette().color(ColorTask->foregroundRole()),
                 this);
    if(c.isValid()) {
        QPalette p = ColorTask->palette();
        p.setColor(ColorTask->foregroundRole(), c);
        ColorTask->setPalette(p);
    }
}

void QucsSettingsDialog::slotTableClicked(int row, int col)
{
    Q_UNUSED(col);
    Input_Suffix->setText(fileTypesTableWidget->item(row,0)->text());
    Input_Program->setText(fileTypesTableWidget->item(row,1)->text());
}

// -----------------------------------------------------------
// The locations tab slots

void QucsSettingsDialog::slotHomeDirBrowse()
{
  QString d = QFileDialog::getExistingDirectory
    (this, tr("Select the home directory"),
     homeEdit->text(),
     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if(!d.isEmpty())
    homeEdit->setText(d);
}

void QucsSettingsDialog::slotAdmsXmlDirBrowse()
{
  QString d = QFileDialog::getExistingDirectory
    (this, tr("Select the admsXml bin directory"),
     admsXmlEdit->text(),
     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if(!d.isEmpty())
    admsXmlEdit->setText(d);
}

void QucsSettingsDialog::slotAscoDirBrowse()
{
  QString d = QFileDialog::getExistingDirectory
    (this, tr("Select the ASCO bin directory"),
     ascoEdit->text(),
     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if(!d.isEmpty())
    ascoEdit->setText(d);
}

void QucsSettingsDialog::slotOctaveDirBrowse()
{
  QString d = QFileDialog::getOpenFileName(this, tr("Select the octave executable"),
                                           octaveEdit->text(), "All files (*)");

  if(!d.isEmpty())
    octaveEdit->setText(d);
}

void QucsSettingsDialog::slotOpenVAFDirBrowse()
{
  QString d = QFileDialog::getOpenFileName(this, tr("Select the OpenVAF executable"),
                                           OpenVAFEdit->text(), "All files (*)");

  if(!d.isEmpty())
    OpenVAFEdit->setText(d);
}

void QucsSettingsDialog::slotRFLayoutDirBrowse()
{
  QString d = QFileDialog::getOpenFileName(this, tr("Select the Qucs-RFLayout executable"),
                                           RFLayoutEdit->text(), "All files (*)");

  if(!d.isEmpty())
    RFLayoutEdit->setText(d);
}

void QucsSettingsDialog::slotPDKDirBrowse()
{
  QString d = QFileDialog::getExistingDirectory
      (this, tr("Select the PDKs root directory"),
       PDKDirEdit->text(),
       QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if(!d.isEmpty())
    PDKDirEdit->setText(d);
}

void QucsSettingsDialog::slotOSDI_Files_DirBrowse()
{
  QString d = QFileDialog::getExistingDirectory
      (this, tr("Select the OSDI files directory"),
       OSDIDirEdit->text(),
       QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if(!d.isEmpty())
    OSDIDirEdit->setText(d);
}

/*! \brief (seems unused at present)
 */
void QucsSettingsDialog::slotPathTableClicked(int row, int col)
{
    Q_UNUSED(row);
    Q_UNUSED(col);
    //Input_Path->setText(fileTypesTableWidget->item(row,0)->text());
}

/* \brief enable "Remove Path" button only if something is selected
 */
void QucsSettingsDialog::slotPathSelectionChanged()
{
  bool selectionIsNotEmpty = !pathsTableWidget->selectedItems().isEmpty();

  RemovePathButt->setEnabled(selectionIsNotEmpty);
}

void QucsSettingsDialog::slotAddPath()
{
  QString d = QFileDialog::getExistingDirectory
    (this, tr("Select a directory"),
     QucsSettings.QucsWorkDir.canonicalPath(),
     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if(!d.isEmpty())
    {
        currentPaths.append(d);
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
    QString d = QFileDialog::getExistingDirectory
      (this, tr("Select a directory"),
       QucsSettings.QucsWorkDir.canonicalPath(),
       QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    QString path;
    QFileInfo pathfinfo;

    if(!d.isEmpty())
    {
        // add the selected path
        currentPaths.append(d);
        // Iterate through the directories
        QDirIterator pathIter(d, QDirIterator::Subdirectories);
        while (pathIter.hasNext())
        {
            path = pathIter.next();
            pathfinfo = pathIter.fileInfo();

            if (pathfinfo.isDir() && !pathfinfo.isSymLink() &&
                pathIter.fileName() != "." && pathIter.fileName() != "..")
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

    for (QTableWidgetItem * item : selectedPaths)
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
    for (const QString& pathstr : currentPaths)
    {
        int row = pathsTableWidget->rowCount();
        pathsTableWidget->setRowCount(row+1);
        QTableWidgetItem *path = new QTableWidgetItem(pathstr);
        path->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        pathsTableWidget->setItem(row, 0, path);
    }
}
