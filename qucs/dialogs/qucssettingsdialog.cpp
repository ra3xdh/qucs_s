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
#include "main.h"
#include "textdoc.h"
#include "schematic.h"
#include "settings.h"
#include "module.h"

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
#include <QGridLayout>
#include <QVBoxLayout>

Q_DECLARE_METATYPE(QStringList*)

auto getFontDescription = [](const auto& a_font) -> QString {
    const QChar comma(u',');
    QString fontDescription = a_font.family() + comma +
        QString::number(a_font.pointSize());

    QString fontStyle = a_font.styleName();
    if (!fontStyle.isEmpty())
        fontDescription += comma + fontStyle;

    return fontDescription;
};

QucsSettingsDialog::QucsSettingsDialog(QucsApp *parent) :
    QDialog(parent),
    a_app(parent),
    a_font(QucsSettings.font),
    a_appFont(QucsSettings.appFont),
    a_textFont(QucsSettings.textFont),
    a_tabWidget(new QTabWidget()),
    a_appSettingsTab(new QWidget(a_tabWidget)),
    a_fileTypesTab(new QWidget(a_tabWidget)),
    a_locationsTab(new QWidget(a_tabWidget)),
    a_editorTab(new QWidget(a_tabWidget)),
    a_checkWiring(new QCheckBox(a_appSettingsTab)),
    a_checkLoadFromFutureVersions(new QCheckBox(a_appSettingsTab)),
    a_allowFlexibleWires(new QCheckBox(a_appSettingsTab)),
    a_allowLayingWiresAnew(new QCheckBox(a_appSettingsTab)),
    a_checkAntiAliasing(new QCheckBox(a_appSettingsTab)),
    a_checkTextAntiAliasing(new QCheckBox(a_appSettingsTab)),
    a_checkFullTraceNames(new QCheckBox(a_appSettingsTab)),
    a_alwaysPrefixDataset(new QCheckBox(a_appSettingsTab)),
    a_languageCombo(new QComboBox(a_appSettingsTab)),
    a_styleCombo(new QComboBox(a_appSettingsTab)),
    a_fontButton(new QPushButton(a_appSettingsTab)),
    a_appFontButton(new QPushButton(a_appSettingsTab)),
    a_textFontButton(new QPushButton(a_appSettingsTab)),
    a_bGColorButton(new QPushButton("      ", a_appSettingsTab)),
    a_gridColorButton(new QPushButton("      ", a_appSettingsTab)),
    a_largeFontSizeEdit(new QLineEdit(a_appSettingsTab)),
    a_undoNumEdit(new QLineEdit(a_appSettingsTab)),
    a_editorEdit(new QLineEdit(a_appSettingsTab)),
    a_inputSuffix(new QLineEdit(a_fileTypesTab)),
    a_inputProgram(new QLineEdit(a_fileTypesTab)),
    a_homeEdit(new QLineEdit(a_locationsTab)),
    a_admsXmlEdit(new QLineEdit(a_locationsTab)),
    a_ascoEdit(new QLineEdit(a_locationsTab)),
    a_octaveEdit(new QLineEdit(a_locationsTab)),
    a_openVAFEdit(new QLineEdit(a_locationsTab)),
    a_rfLayoutEdit(new QLineEdit(a_locationsTab)),
    a_graphLineWidthEdit(new QLineEdit(a_appSettingsTab)),
    a_fileTypesTableWidget(new QTableWidget(a_fileTypesTab)),
    a_pathsTableWidget(new QTableWidget(a_locationsTab)),
    a_pathsTypeCombo(new QComboBox(a_locationsTab)),
    a_colorComment(new QPushButton(tr("Comment"), a_editorTab)),
    a_colorString(new QPushButton(tr("String"), a_editorTab)),
    a_colorInteger(new QPushButton(tr("Integer Number"), a_editorTab)),
    a_colorReal(new QPushButton(tr("Real Number"), a_editorTab)),
    a_colorCharacter(new QPushButton(tr("Character"), a_editorTab)),
    a_colorDataType(new QPushButton(tr("Data Type"), a_editorTab)),
    a_colorAttribute(new QPushButton(tr("Attribute"), a_editorTab)),
    a_colorDirective(new QPushButton(tr("Directive"), a_editorTab)),
    a_colorTask(new QPushButton(tr("Task"), a_editorTab)),
    a_removePathButt(new QPushButton(tr("Remove Path"))),
    a_all(new QVBoxLayout(this)), // to provide the necessary size
    a_val50(new QIntValidator(1, 50, this)),
    a_val200(new QIntValidator(0, 200, this)),
    a_expr("[\\w_]+"),
    a_validator(new QRegularExpressionValidator(a_expr, this)),
    a_subcktPaths(qucsSubcktPathList),
    a_xmlCompPaths(qucsXmlCompPathList)
{
    setWindowTitle(tr("Edit Qucs Properties"));

    a_all->addWidget(a_tabWidget);

    // ...........................................................
    // The application settings tab
    QGridLayout *appSettingsGrid = new QGridLayout(a_appSettingsTab);

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

    appSettingsGrid->addWidget(new QLabel(tr("Language (set after reload):"), a_appSettingsTab) ,1, 0);
    a_languageCombo->addItems(appLanguages);
    appSettingsGrid->addWidget(a_languageCombo, 1, 1);

    appSettingsGrid->addWidget(new QLabel(tr("Maximum undo operations:"), a_appSettingsTab) ,2, 0);
    a_undoNumEdit->setValidator(a_val200);
    appSettingsGrid->addWidget(a_undoNumEdit, 2, 1);

    appSettingsGrid->addWidget(new QLabel(tr("Text editor:"), a_appSettingsTab), 3, 0);
    a_editorEdit->setToolTip(tr("Set to qucs, qucsedit or the path to your favorite text editor."));
    appSettingsGrid->addWidget(a_editorEdit, 3, 1);

    appSettingsGrid->addWidget(new QLabel(tr("Start wiring when clicking open node:"), a_appSettingsTab), 4, 0);
    appSettingsGrid->addWidget(a_checkWiring, 4, 1);

    appSettingsGrid->addWidget(new QLabel(tr("Load documents from future versions:")), 5, 0);
    a_checkLoadFromFutureVersions->setToolTip(tr("Try to load also documents created with newer versions of Qucs."));
    appSettingsGrid->addWidget(a_checkLoadFromFutureVersions, 5, 1);
    a_checkLoadFromFutureVersions->setChecked(QucsSettings.IgnoreFutureVersion);

    appSettingsGrid->addWidget(new QLabel(tr("Show trace name prefix on diagrams:")), 6, 0);
    a_checkFullTraceNames->setToolTip(tr("Show prefixes for trace names on diagrams like \"ngspice/\""));
    appSettingsGrid->addWidget(a_checkFullTraceNames, 6, 1);
    a_checkFullTraceNames->setChecked(QucsSettings.fullTraceName);

    appSettingsGrid->addWidget(new QLabel(tr("Always prefix the dataset with simulation label:")), 7, 0);
    a_alwaysPrefixDataset->setToolTip(tr("Always use the prefix for dataset, i.e. \"tr1.v(out)\" rather than \"v(out)\""));
    appSettingsGrid->addWidget(a_alwaysPrefixDataset, 7, 1);
    a_alwaysPrefixDataset->setChecked(QucsSettings.alwaysPrefixDataset);

    appSettingsGrid->addWidget(new QLabel(tr("Flexible wires (requires restart):"), a_appSettingsTab), 8, 0);
    appSettingsGrid->addWidget(a_allowFlexibleWires, 8, 1);

    appSettingsGrid->addWidget(new QLabel(tr("Lay wires anew when moving elements (requires restart):"), a_appSettingsTab), 9, 0);
    appSettingsGrid->addWidget(a_allowLayingWiresAnew, 9, 1);

    a_tabWidget->addTab(a_appSettingsTab, tr("Settings"));

    // ...........................................................
    // The appearance settings tab
    QWidget *appAppearanceTab = new QWidget(a_tabWidget);
    QGridLayout *appAppearanceGrid = new QGridLayout(appAppearanceTab);

    appAppearanceGrid->addWidget(new QLabel(tr("Schematic font (set after reload):"), a_appSettingsTab), 0, 0);
    connect(a_fontButton, SIGNAL(clicked()), SLOT(slotFontDialog()));
    appAppearanceGrid->addWidget(a_fontButton, 0, 1);

    appAppearanceGrid->addWidget(new QLabel(tr("Application font (set after reload):"), a_appSettingsTab), 1, 0);
    connect(a_appFontButton, SIGNAL(clicked()), SLOT(slotAppFontDialog()));
    appAppearanceGrid->addWidget(a_appFontButton, 1, 1);

    appAppearanceGrid->addWidget(new QLabel(tr("Text document font (set after reload):"), a_appSettingsTab), 2, 0);
    connect(a_textFontButton, SIGNAL(clicked()), SLOT(slotTextFontDialog()));
    appAppearanceGrid->addWidget(a_textFontButton, 2, 1);

    appAppearanceGrid->addWidget(new QLabel(tr("Large font size:"), a_appSettingsTab), 3, 0);
    a_largeFontSizeEdit->setValidator(a_val50);
    appAppearanceGrid->addWidget(a_largeFontSizeEdit, 3, 1);

    appAppearanceGrid->addWidget(new QLabel(tr("Document Background Color:"), a_appSettingsTab) ,4, 0);
    connect(a_bGColorButton, SIGNAL(clicked()), SLOT(slotBGColorDialog()));
    appAppearanceGrid->addWidget(a_bGColorButton, 4, 1);

    appAppearanceGrid->addWidget(new QLabel(tr("Grid Color (set after reload):"), a_appSettingsTab) ,5, 0);
    connect(a_gridColorButton, SIGNAL(clicked()), SLOT(slotGridColorDialog()));
    appAppearanceGrid->addWidget(a_gridColorButton, 5, 1);

    appAppearanceGrid->addWidget(new QLabel(tr("Draw diagrams with anti-aliasing feature:")), 6, 0);
    a_checkAntiAliasing->setToolTip(tr("Use anti-aliasing for graphs for a smoother appearance."));
    appAppearanceGrid->addWidget(a_checkAntiAliasing, 6, 1);
    a_checkAntiAliasing->setChecked(QucsSettings.GraphAntiAliasing);

    appAppearanceGrid->addWidget(new QLabel(tr("Draw text with anti-aliasing feature:")), 7, 0);
    a_checkTextAntiAliasing->setToolTip(tr("Use anti-aliasing for text for a smoother appearance."));
    appAppearanceGrid->addWidget(a_checkTextAntiAliasing, 7, 1);
    a_checkTextAntiAliasing->setChecked(QucsSettings.TextAntiAliasing);

    appAppearanceGrid->addWidget(new QLabel(tr("Default graph line thickness:"), a_appSettingsTab), 8, 0);
    a_graphLineWidthEdit->setValidator(a_val50);
    appAppearanceGrid->addWidget(a_graphLineWidthEdit, 8, 1);

    appAppearanceGrid->addWidget(new QLabel(tr("App Style:"), a_appSettingsTab), 9, 0);
    QStringList styles = QStyleFactory::keys(); // Get available styles
    a_styleCombo->addItems(styles);
    appAppearanceGrid->addWidget(a_styleCombo,9,1);


    // Retrieve the current style and set it as selected
    QString currentStyle = QApplication::style()->objectName();
    int index = a_styleCombo->findText(currentStyle, Qt::MatchFixedString);
    if (index != -1) {
        a_styleCombo->setCurrentIndex(index);
    }

    a_tabWidget->addTab(appAppearanceTab, tr("Appearance"));

    // ...........................................................
    // The source code editor settings tab
    QGridLayout *editorGrid = new QGridLayout(a_editorTab);

    editorGrid->addWidget(new QLabel(tr("Colors for Syntax Highlighting:"), a_editorTab), 0, 1, 0, 2);

    QPalette p;

    p = a_colorComment->palette();
    p.setColor(a_colorComment->foregroundRole(), QucsSettings.Comment);
    p.setColor(a_colorComment->backgroundRole(), QucsSettings.BGColor);
    a_colorComment->setPalette(p);
    connect(a_colorComment, SIGNAL(clicked()), SLOT(slotColorComment()));
    editorGrid->addWidget(a_colorComment,1,0);

    p = a_colorString->palette();
    p.setColor(a_colorString->foregroundRole(), QucsSettings.String);
    p.setColor(a_colorString->backgroundRole(), QucsSettings.BGColor);
    a_colorString->setPalette(p);
    connect(a_colorString, SIGNAL(clicked()), SLOT(slotColorString()));
    editorGrid->addWidget(a_colorString,1,1);

    p = a_colorInteger->palette();
    p.setColor(a_colorInteger->foregroundRole(), QucsSettings.Integer);
    p.setColor(a_colorInteger->backgroundRole(), QucsSettings.BGColor);
    a_colorInteger->setPalette(p);
    connect(a_colorInteger, SIGNAL(clicked()), SLOT(slotColorInteger()));
    editorGrid->addWidget(a_colorInteger,1,2);

    p = a_colorReal->palette();
    p.setColor(a_colorReal->foregroundRole(), QucsSettings.Real);
    p.setColor(a_colorReal->backgroundRole(), QucsSettings.BGColor);
    a_colorReal->setPalette(p);
    connect(a_colorReal, SIGNAL(clicked()), SLOT(slotColorReal()));
    editorGrid->addWidget(a_colorReal,2,0);

    p = a_colorCharacter->palette();
    p.setColor(a_colorCharacter->foregroundRole(), QucsSettings.Character);
    p.setColor(a_colorCharacter->backgroundRole(), QucsSettings.BGColor);
    a_colorCharacter->setPalette(p);
    connect(a_colorCharacter, SIGNAL(clicked()), SLOT(slotColorCharacter()));
    editorGrid->addWidget(a_colorCharacter,2,1);

    p = a_colorDataType->palette();
    p.setColor(a_colorDataType->foregroundRole(), QucsSettings.Type);
    p.setColor(a_colorDataType->backgroundRole(), QucsSettings.BGColor);
    a_colorDataType->setPalette(p);
    connect(a_colorDataType, SIGNAL(clicked()), SLOT(slotColorDataType()));
    editorGrid->addWidget(a_colorDataType,2,2);

    p = a_colorAttribute->palette();
    p.setColor(a_colorAttribute->foregroundRole(), QucsSettings.Attribute);
    p.setColor(a_colorAttribute->backgroundRole(), QucsSettings.BGColor);
    a_colorAttribute->setPalette(p);
    connect(a_colorAttribute, SIGNAL(clicked()), SLOT(slotColorAttribute()));
    editorGrid->addWidget(a_colorAttribute,3,0);

    p = a_colorDirective->palette();
    p.setColor(a_colorDirective->foregroundRole(), QucsSettings.Directive);
    p.setColor(a_colorDirective->backgroundRole(), QucsSettings.BGColor);
    a_colorDirective->setPalette(p);
    connect(a_colorDirective, SIGNAL(clicked()), SLOT(slotColorDirective()));
    editorGrid->addWidget(a_colorDirective,3,1);

    p = a_colorTask->palette();
    p.setColor(a_colorTask->foregroundRole(), QucsSettings.Task);
    p.setColor(a_colorTask->backgroundRole(), QucsSettings.BGColor);
    a_colorTask->setPalette(p);
    connect(a_colorTask, SIGNAL(clicked()), SLOT(slotColorTask()));
    editorGrid->addWidget(a_colorTask,3,2);


    a_tabWidget->addTab(a_editorTab, tr("Source Code Editor"));

    // ...........................................................
    // The file types tab
    QGridLayout *fileTypesGrid = new QGridLayout(a_fileTypesTab);

    QLabel *note = new QLabel(
        tr("Register filename extensions here in order to\nopen files with an appropriate program."));
    fileTypesGrid->addWidget(note,0,0,1,2);

    // the a_fileTypesTableWidget displays information on the file types
    a_fileTypesTableWidget->setColumnCount(2);

    QTableWidgetItem *item1 = new QTableWidgetItem();
    QTableWidgetItem *item2 = new QTableWidgetItem();

    a_fileTypesTableWidget->setHorizontalHeaderItem(0, item1);
    a_fileTypesTableWidget->setHorizontalHeaderItem(1, item2);

    item1->setText(tr("Suffix"));
    item2->setText(tr("Program"));

    a_fileTypesTableWidget->horizontalHeader()->setStretchLastSection(true);
    a_fileTypesTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    a_fileTypesTableWidget->horizontalHeader()->setSectionsClickable(false); // no action when clicking on the header
    a_fileTypesTableWidget->verticalHeader()->hide();
    connect(a_fileTypesTableWidget, SIGNAL(cellClicked(int,int)), SLOT(slotTableClicked(int,int)));
    fileTypesGrid->addWidget(a_fileTypesTableWidget,1,0,3,1);

    // fill listview with already registered file extensions
    QStringList::Iterator it = QucsSettings.FileTypes.begin();
    while(it != QucsSettings.FileTypes.end())
    {
        int row = a_fileTypesTableWidget->rowCount();
        a_fileTypesTableWidget->setRowCount(row+1);
        QTableWidgetItem *suffix = new QTableWidgetItem(QString((*it).section('/',0,0)));
        QTableWidgetItem *program = new QTableWidgetItem(QString((*it).section('/',1,1)));
        suffix->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        program->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        a_fileTypesTableWidget->setItem(row, 0, suffix);
        a_fileTypesTableWidget->setItem(row, 1, program);
        it++;
    }

    QLabel *l5 = new QLabel(tr("Suffix:"), a_fileTypesTab);
    fileTypesGrid->addWidget(l5,1,1);
    a_inputSuffix->setValidator(a_validator);
    fileTypesGrid->addWidget(a_inputSuffix,1,2);
//  connect(a_inputSuffix, SIGNAL(returnPressed()), SLOT(slotGotoProgEdit())); //not implemented

    QLabel *l6 = new QLabel(tr("Program:"), a_fileTypesTab);
    fileTypesGrid->addWidget(l6,2,1);
    fileTypesGrid->addWidget(a_inputProgram,2,2);

    QPushButton *AddButt = new QPushButton(tr("Set"));
    fileTypesGrid->addWidget(AddButt,3,1);
    connect(AddButt, SIGNAL(clicked()), SLOT(slotAddFileType()));
    QPushButton *RemoveButt = new QPushButton(tr("Remove"));
    fileTypesGrid->addWidget(RemoveButt,3,2);
    connect(RemoveButt, SIGNAL(clicked()), SLOT(slotRemoveFileType()));

    fileTypesGrid->setRowStretch(3,4);
    a_tabWidget->addTab(a_fileTypesTab, tr("File Types"));

    // ...........................................................
    // The locations tab
    QGridLayout *locationsGrid = new QGridLayout(a_locationsTab);

    QLabel *note2 = new QLabel(
        tr("Edit the standard paths and external applications"));
    locationsGrid->addWidget(note2,0,0,1,2);

    locationsGrid->addWidget(new QLabel(tr("Qucs Home:"), a_locationsTab) ,1,0);
    locationsGrid->addWidget(a_homeEdit,1,1);
    QPushButton *HomeButt = new QPushButton(tr("Browse"));
    locationsGrid->addWidget(HomeButt, 1, 2);
    connect(HomeButt, SIGNAL(clicked()), SLOT(slotHomeDirBrowse()));

    locationsGrid->addWidget(new QLabel(tr("AdmsXml Path:"), a_locationsTab) ,2,0);
    locationsGrid->addWidget(a_admsXmlEdit,2,1);
    QPushButton *AdmsXmlButt = new QPushButton(tr("Browse"));
    locationsGrid->addWidget(AdmsXmlButt, 2, 2);
    connect(AdmsXmlButt, SIGNAL(clicked()), SLOT(slotAdmsXmlDirBrowse()));

    locationsGrid->addWidget(new QLabel(tr("ASCO Path:"), a_locationsTab) ,3,0);
    locationsGrid->addWidget(a_ascoEdit,3,1);
    QPushButton *ascoButt = new QPushButton(tr("Browse"));
    locationsGrid->addWidget(ascoButt, 3, 2);
    connect(ascoButt, SIGNAL(clicked()), SLOT(slotAscoDirBrowse()));

    locationsGrid->addWidget(new QLabel(tr("Octave Path:"), a_locationsTab) ,4,0);
    locationsGrid->addWidget(a_octaveEdit,4,1);
    QPushButton *OctaveButt = new QPushButton(tr("Browse"));
    locationsGrid->addWidget(OctaveButt, 4, 2);
    connect(OctaveButt, SIGNAL(clicked()), SLOT(slotOctaveDirBrowse()));

    locationsGrid->addWidget(new QLabel(tr("OpenVAF Path:"), a_locationsTab) ,5,0);
    locationsGrid->addWidget(a_openVAFEdit,5,1);
    QPushButton *OpenVAFButt = new QPushButton(tr("Browse"));
    locationsGrid->addWidget(OpenVAFButt, 5, 2);
    connect(OpenVAFButt, SIGNAL(clicked()), SLOT(slotOpenVAFDirBrowse()));

    locationsGrid->addWidget(new QLabel(tr("RF Layout Path:"), a_locationsTab) ,6,0);
    locationsGrid->addWidget(a_rfLayoutEdit,6,1);
    QPushButton *RFLButt = new QPushButton(tr("Browse"));
    locationsGrid->addWidget(RFLButt, 6, 2);
    connect(RFLButt, SIGNAL(clicked()), SLOT(slotRFLayoutDirBrowse()));

    // the a_pathsTableWidget displays the path list
    a_pathsTableWidget->setColumnCount(1);

    a_pathsTableWidget->horizontalHeader()->hide();

    a_pathsTypeCombo->addItem(
            "Subcircuit Search Path List:",
            QVariant::fromValue(static_cast<QStringList*>(&a_subcktPaths)));
    a_pathsTypeCombo->addItem(
            "XML Component Search Path List:",
            QVariant::fromValue(static_cast<QStringList*>(&a_xmlCompPaths)));
    a_pathsTypeCombo->setCurrentIndex(0);
    connect(a_pathsTypeCombo, SIGNAL(currentIndexChanged(int)), SLOT(slotPathTypeSelectionChanged(int)));

    locationsGrid->addWidget(a_pathsTypeCombo,7,0,1,2);

    a_pathsTableWidget->horizontalHeader()->setStretchLastSection(true);
    // avoid drawing header text in bold when some data is selected
    a_pathsTableWidget->horizontalHeader()->setSectionsClickable(false);

    a_pathsTableWidget->verticalHeader()->hide();
    // allow multiple items to be selected
    a_pathsTableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(a_pathsTableWidget, SIGNAL(cellClicked(int,int)), SLOT(slotPathTableClicked(int,int)));
    connect(a_pathsTableWidget, SIGNAL(itemSelectionChanged()), SLOT(slotPathSelectionChanged()));
    locationsGrid->addWidget(a_pathsTableWidget,8,0,2,2);

    QPushButton *AddPathButt = new QPushButton(tr("Add Path"));
    locationsGrid->addWidget(AddPathButt, 7, 2);
    connect(AddPathButt, SIGNAL(clicked()), SLOT(slotAddPath()));

    QPushButton *AddPathSubFolButt = new QPushButton(tr("Add Path With SubFolders"));
    locationsGrid->addWidget(AddPathSubFolButt, 8, 2);
    connect(AddPathSubFolButt, SIGNAL(clicked()), SLOT(slotAddPathWithSubFolders()));

    // disable button if no paths in the table are selected
    a_removePathButt->setEnabled(false);
    locationsGrid->addWidget(a_removePathButt , 9, 2);
    connect(a_removePathButt, SIGNAL(clicked()), SLOT(slotRemovePath()));

    // create a copy of the current global path list
    makePathTable();

    a_tabWidget->addTab(a_locationsTab, tr("Locations"));

    // ...........................................................
    // buttons on the bottom of the dialog (independent of the TabWidget)

    QHBoxLayout *Butts = new QHBoxLayout();
    Butts->setSpacing(3);
    Butts->setContentsMargins(3,3,3,3);
    a_all->addLayout(Butts);

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

    // fill the fields with the Qucs-Properties

    a_fontButton->setText(getFontDescription(a_font));
    a_appFontButton->setText(getFontDescription(a_appFont));
    a_textFontButton->setText(getFontDescription(a_textFont));
    QString s = QString::number(QucsSettings.largeFontSize, 'f', 1);
    a_largeFontSizeEdit->setText(s);
    a_graphLineWidthEdit->setText(_settings::Get().item<QString>("DefaultGraphLineWidth"));

    p = a_bGColorButton->palette();
    p.setColor(a_bGColorButton->backgroundRole(), QucsSettings.BGColor);
    a_bGColorButton->setPalette(p);

    p = a_gridColorButton->palette();
    p.setColor(a_gridColorButton->backgroundRole(), _settings::Get().item<QColor>("GridColor"));
    a_gridColorButton->setPalette(p);

    a_undoNumEdit->setText(QString::number(QucsSettings.maxUndo));
    a_editorEdit->setText(QucsSettings.Editor);
    a_checkWiring->setChecked(QucsSettings.NodeWiring);
    a_allowFlexibleWires->setChecked(_settings::Get().item<bool>("AllowFlexibleWires"));
    a_allowLayingWiresAnew->setChecked(_settings::Get().item<bool>("AllowLayingWiresAnew"));

    for(int z=a_languageCombo->count()-1; z>=0; z--)
        if(a_languageCombo->itemText(z).section('(',1,1).remove(')') == QucsSettings.Language)
            a_languageCombo->setCurrentIndex(z);

    /*! Load paths from settings */
    a_homeEdit->setText(QucsSettings.qucsWorkspaceDir.canonicalPath());
    a_admsXmlEdit->setText(QucsSettings.AdmsXmlBinDir.canonicalPath());
    a_ascoEdit->setText(QucsSettings.AscoBinDir.canonicalPath());
    a_octaveEdit->setText(QucsSettings.OctaveExecutable);
    a_openVAFEdit->setText(QucsSettings.OpenVAFExecutable);
    a_rfLayoutEdit->setText(QucsSettings.RFLayoutExecutable);


    resize(300, 200);
}

QucsSettingsDialog::~QucsSettingsDialog()
{
    delete a_all;
    delete a_val50;
    delete a_val200;
    delete a_validator;
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotAddFileType()
{
    QModelIndexList indexes = a_fileTypesTableWidget->selectionModel()->selection().indexes();
    if (indexes.count())
    {
        a_fileTypesTableWidget->item(indexes.at(0).row(),0)->setText(a_inputSuffix->text());
        a_fileTypesTableWidget->item(indexes.at(0).row(),1)->setText(a_inputProgram->text());
        a_fileTypesTableWidget->selectionModel()->clear();
        return;
    }

    //check before append
    for(int r=0; r < a_fileTypesTableWidget->rowCount(); r++)
        if(a_fileTypesTableWidget->item(r,0)->text() == a_inputSuffix->text())
        {
            QMessageBox::critical(this, tr("Error"),
                                  tr("This suffix is already registered!"));
            return;
        }

    int row = a_fileTypesTableWidget->rowCount();
    a_fileTypesTableWidget->setRowCount(row+1);

    QTableWidgetItem *newSuffix = new QTableWidgetItem(QString(a_inputSuffix->text()));
    newSuffix->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    a_fileTypesTableWidget->setItem(row, 0, newSuffix);

    QTableWidgetItem *newProgram = new QTableWidgetItem(a_inputProgram->text());
    a_fileTypesTableWidget->setItem(row, 1, newProgram);
    newProgram->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    a_inputSuffix->setFocus();
    a_inputSuffix->clear();
    a_inputProgram->clear();
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotRemoveFileType()
{
    QModelIndexList indexes = a_fileTypesTableWidget->selectionModel()->selection().indexes();
    if (indexes.count())
    {
        a_fileTypesTableWidget->removeRow(indexes.at(0).row());
        a_fileTypesTableWidget->selectionModel()->clear();
        a_inputSuffix->setText("");
        a_inputProgram->setText("");
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
    if (a_homeEdit->text() != QucsSettings.qucsWorkspaceDir.path()) {
      // close all open files, asking the user whether to save the modified ones
      // if user aborts closing, just return
      if(!a_app->closeAllFiles()) return;

      QucsSettings.qucsWorkspaceDir.setPath(a_homeEdit->text());
      homeDirChanged = true;
      // later below the file tree will be refreshed
    }

    if(QucsSettings.BGColor != a_bGColorButton->palette().color(a_bGColorButton->backgroundRole()))
    {
        QucsSettings.BGColor = a_bGColorButton->palette().color(a_bGColorButton->backgroundRole());

        int No=0;
        QWidget *w;

        while((w=a_app->DocumentTab->widget(No++)) != 0) {
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

    QString selectedStyle = a_styleCombo->currentText();
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
    if (_settings::Get().item<QColor>("GridColor") != a_gridColorButton->palette().color(a_gridColorButton->backgroundRole())) {
        _settings::Get().setItem<QColor>("GridColor", a_gridColorButton->palette().color(a_gridColorButton->backgroundRole()));

        for (int tab = 0; tab < a_app->DocumentTab->count(); tab++) {
            QWidget* widget = a_app->DocumentTab->widget(tab);
            if (!QucsApp::isTextDocument(widget)) {
                static_cast<Schematic*>(widget)->setGridColor(_settings::Get().item<QColor>("GridColor"));
            }
        }

        changed = true;
    }

    QucsSettings.font=a_font;
    QucsSettings.appFont = a_appFont;
    QucsSettings.textFont = a_textFont;

    QucsSettings.Language =
        a_languageCombo->currentText().section('(',1,1).remove(')');

    if(QucsSettings.Comment != a_colorComment->palette().color(a_colorComment->foregroundRole()))
    {
        QucsSettings.Comment = a_colorComment->palette().color(a_colorComment->foregroundRole());
        changed = true;
    }
    if(QucsSettings.String != a_colorString->palette().color(a_colorString->foregroundRole()))
    {
        QucsSettings.String = a_colorString->palette().color(a_colorString->foregroundRole());
        changed = true;
    }
    if(QucsSettings.Integer != a_colorInteger->palette().color(a_colorInteger->foregroundRole()))
    {
        QucsSettings.Integer = a_colorInteger->palette().color(a_colorInteger->foregroundRole());
        changed = true;
    }
    if(QucsSettings.Real != a_colorReal->palette().color(a_colorReal->foregroundRole()))
    {
        QucsSettings.Real = a_colorReal->palette().color(a_colorReal->foregroundRole());
        changed = true;
    }
    if(QucsSettings.Character != a_colorCharacter->palette().color(a_colorCharacter->foregroundRole()))
    {
        QucsSettings.Character = a_colorCharacter->palette().color(a_colorCharacter->foregroundRole());
        changed = true;
    }
    if(QucsSettings.Type != a_colorDataType->palette().color(a_colorDataType->foregroundRole()))
    {
        QucsSettings.Type = a_colorDataType->palette().color(a_colorDataType->foregroundRole());
        changed = true;
    }
    if(QucsSettings.Attribute != a_colorAttribute->palette().color(a_colorAttribute->foregroundRole()))
    {
        QucsSettings.Attribute = a_colorAttribute->palette().color(a_colorAttribute->foregroundRole());
        changed = true;
    }
    if(QucsSettings.Directive != a_colorDirective->palette().color(a_colorDirective->foregroundRole()))
    {
        QucsSettings.Directive = a_colorDirective->palette().color(a_colorDirective->foregroundRole());
        changed = true;
    }
    if(QucsSettings.Task != a_colorTask->palette().color(a_colorTask->foregroundRole()))
    {
        QucsSettings.Task = a_colorTask->palette().color(a_colorTask->foregroundRole());
        changed = true;
    }

    bool ok;
    if(QucsSettings.maxUndo != a_undoNumEdit->text().toUInt(&ok))
    {
        QucsSettings.maxUndo = a_undoNumEdit->text().toInt(&ok);
        changed = true;
    }
    if(QucsSettings.Editor != a_editorEdit->text())
    {
        QucsSettings.Editor = a_editorEdit->text();
        changed = true;
    }
    if(QucsSettings.NodeWiring != (unsigned)a_checkWiring->isChecked())
    {
        QucsSettings.NodeWiring = a_checkWiring->isChecked();
        changed = true;
    }

    _settings::Get().setItem("AllowFlexibleWires", a_allowFlexibleWires->isChecked());
    _settings::Get().setItem("AllowLayingWiresAnew", a_allowLayingWiresAnew->isChecked());

    QucsSettings.FileTypes.clear();
    for (int row=0; row < a_fileTypesTableWidget->rowCount(); row++)
    {
        QucsSettings.FileTypes.append(a_fileTypesTableWidget->item(row,0)->text()
                                      +"/"+
                                      a_fileTypesTableWidget->item(row,1)->text());
    }

    /*! Update QucsSettings, tool paths */
    QucsSettings.AdmsXmlBinDir.setPath(a_admsXmlEdit->text());
    QucsSettings.AscoBinDir.setPath(a_ascoEdit->text());
    QucsSettings.OctaveExecutable = a_octaveEdit->text();
    QucsSettings.OpenVAFExecutable = a_openVAFEdit->text();
    QucsSettings.RFLayoutExecutable = a_rfLayoutEdit->text();

    if (QucsSettings.IgnoreFutureVersion != a_checkLoadFromFutureVersions->isChecked())
    {
      QucsSettings.IgnoreFutureVersion = a_checkLoadFromFutureVersions->isChecked();
      changed = true;
    }

    if (QucsSettings.GraphAntiAliasing != a_checkAntiAliasing->isChecked())
    {
      QucsSettings.GraphAntiAliasing = a_checkAntiAliasing->isChecked();
      changed = true;
    }

    if (QucsSettings.TextAntiAliasing != a_checkTextAntiAliasing->isChecked())
    {
      QucsSettings.TextAntiAliasing = a_checkTextAntiAliasing->isChecked();
      changed = true;
    }

    if (QucsSettings.fullTraceName != a_checkFullTraceNames->isChecked())
    {
      QucsSettings.fullTraceName = a_checkFullTraceNames->isChecked();
      changed = true;
    }

    if (QucsSettings.alwaysPrefixDataset != a_alwaysPrefixDataset->isChecked())
    {
      QucsSettings.alwaysPrefixDataset = a_alwaysPrefixDataset->isChecked();
      changed = true;
    }

    // use toDouble() as it can interpret the string according to the current locale
    if (QucsSettings.largeFontSize != a_largeFontSizeEdit->text().toDouble(&ok))
    {
        QucsSettings.largeFontSize = a_largeFontSizeEdit->text().toDouble(&ok);
        changed = true;
    }

    if (_settings::Get().item<QString>("DefaultGraphLineWidth") != a_graphLineWidthEdit->text())
    {
        _settings::Get().setItem<QString>("DefaultGraphLineWidth", a_graphLineWidthEdit->text());
        changed = true;
    }

    saveApplSettings();  // also sets the small and large font

    // if QucsHome is changed, refresh projects tree
    // do this after updating the other paths
    if (homeDirChanged) {
      // files were actuallt closed above, this will refresh the projects tree
      // and create an empty schematic
      a_app->slotMenuProjClose();
      changed = true;
    }

    if(changed)
    {
        a_app->readProjects();
        a_app->slotUpdateTreeview();
        a_app->repaint();
    }

    const bool xmlCompPathChanged(a_xmlCompPaths != qucsXmlCompPathList);

    // update the subckt/xml-comp pathlist
    Q_ASSERT(a_pathsTypeCombo->count() == 2);
    QList<QStringList*> refList = {&qucsSubcktPathList, &qucsXmlCompPathList};
    for (int idx = 0; idx < a_pathsTypeCombo->count(); ++idx)
    {
        QucsMain->updatePathList(
                *static_cast<QStringList*>(a_pathsTypeCombo->itemData(idx).value<QStringList*>()),
                *(refList[idx]));
    }

    if (xmlCompPathChanged)
    {
        Module::unregisterXmlComponents();
        Module::registerXmlComponents();
        a_app->refreshCurrentComponentList();
    }

    //QucsMain->updateSchNameHash();
    //QucsMain->updateSpiceNameHash();
}


// -----------------------------------------------------------
void QucsSettingsDialog::slotFontDialog()
{
    bool ok;
    QFont tmpFont = QFontDialog::getFont(&ok, a_font, this);
    if(ok)
    {
        a_font = tmpFont;
        a_fontButton->setText(getFontDescription(a_font));
    }
}

void QucsSettingsDialog::slotAppFontDialog()
{
    bool ok;
    QFont tmpFont = QFontDialog::getFont(&ok, a_appFont, this);
    if(ok)
    {
        a_appFont = tmpFont;
        a_appFontButton->setText(getFontDescription(a_appFont));
    }
}

void QucsSettingsDialog::slotTextFontDialog()
{
    bool ok;
    QFont tmpFont = QFontDialog::getFont(&ok, a_textFont, this);
    if(ok)
    {
        a_textFont = tmpFont;
        a_textFontButton->setText(getFontDescription(a_textFont));
    }
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotBGColorDialog()
{
    QColor c = QColorDialog::getColor(
                   a_bGColorButton->palette().color(a_bGColorButton->foregroundRole()),
                   this);
    if(c.isValid()) {
        QPalette p = a_bGColorButton->palette();
        p.setColor(a_bGColorButton->backgroundRole(), c);
        a_bGColorButton->setPalette(p);
    }
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotGridColorDialog()
{
    QColor c = QColorDialog::getColor(
                   a_gridColorButton->palette().color(a_gridColorButton->foregroundRole()),
                   this);
    if(c.isValid()) {
        QPalette p = a_gridColorButton->palette();
        p.setColor(a_gridColorButton->backgroundRole(), c);
        a_gridColorButton->setPalette(p);
    }
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotDefaultValues()
{
    QPalette p;
    a_font = QApplication::font();
    a_appFont = QucsSettings.sysDefaultFont;
    a_textFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    a_fontButton->setText(getFontDescription(a_font));
    a_appFontButton->setText(getFontDescription(a_appFont));
    a_textFontButton->setText(getFontDescription(a_textFont));
    a_largeFontSizeEdit->setText(QString::number(16.0));

    a_languageCombo->setCurrentIndex(0);

    p = a_bGColorButton->palette();
    p.setColor(a_bGColorButton->backgroundRole(), QColor(255,250,225));
    a_bGColorButton->setPalette(p);

    p = a_colorComment->palette();
    p.setColor(a_colorComment->foregroundRole(), Qt::gray);
    a_colorComment->setPalette(p);

    p = a_colorString->palette();
    p.setColor(a_colorString->foregroundRole(), Qt::red);
    a_colorString->setPalette(p);

    p = a_colorInteger->palette();
    p.setColor(a_colorInteger->foregroundRole(), Qt::blue);
    a_colorInteger->setPalette(p);

    p = a_colorReal->palette();
    p.setColor(a_colorReal->foregroundRole(), Qt::darkMagenta);
    a_colorReal->setPalette(p);

    p = a_colorCharacter->palette();
    p.setColor(a_colorCharacter->foregroundRole(), Qt::magenta);
    a_colorCharacter->setPalette(p);

    p = a_colorDataType->palette();
    p.setColor(a_colorDataType->foregroundRole(), Qt::darkRed);
    a_colorDataType->setPalette(p);

    p = a_colorAttribute->palette();
    p.setColor(a_colorAttribute->foregroundRole(), Qt::darkCyan);
    a_colorAttribute->setPalette(p);

    p = a_colorDirective->palette();
    p.setColor(a_colorDirective->foregroundRole(), Qt::darkCyan);
    a_colorDirective->setPalette(p);

    p = a_colorTask->palette();
    p.setColor(a_colorTask->foregroundRole(), Qt::darkRed);
    a_colorTask->setPalette(p);

    a_undoNumEdit->setText("20");
    a_editorEdit->setText(QucsSettings.BinDir + "qucs");
    a_checkWiring->setChecked(false);
    a_allowFlexibleWires->setChecked(_settings::Get().itemDefault<bool>("AllowFlexibleWires"));
    a_allowLayingWiresAnew->setChecked(_settings::Get().itemDefault<bool>("AllowLayingWiresAnew"));
    a_checkLoadFromFutureVersions->setChecked(false);
    a_checkAntiAliasing->setChecked(false);
    a_checkTextAntiAliasing->setChecked(true);
    a_checkFullTraceNames->setChecked(false);
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotColorComment()
{
    QColor c = QColorDialog::getColor(
                 a_colorComment->palette().color(a_colorComment->foregroundRole()),
                 this);
    if(c.isValid()) {
        QPalette p = a_colorComment->palette();
        p.setColor(a_colorComment->foregroundRole(), c);
        a_colorComment->setPalette(p);
    }
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotColorString()
{
    QColor c = QColorDialog::getColor(
                 a_colorString->palette().color(a_colorString->foregroundRole()),
                 this);
    if(c.isValid()) {
        QPalette p = a_colorString->palette();
        p.setColor(a_colorString->foregroundRole(), c);
        a_colorString->setPalette(p);
    }
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotColorInteger()
{
    QColor c = QColorDialog::getColor(
                 a_colorInteger->palette().color(a_colorInteger->foregroundRole()),
                 this);
    if(c.isValid()) {
        QPalette p = a_colorInteger->palette();
        p.setColor(a_colorInteger->foregroundRole(), c);
        a_colorInteger->setPalette(p);
    }
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotColorReal()
{
    QColor c = QColorDialog::getColor(
                 a_colorReal->palette().color(a_colorReal->foregroundRole()),
                 this);
    if(c.isValid()) {
         QPalette p = a_colorReal->palette();
         p.setColor(a_colorReal->foregroundRole(), c);
         a_colorReal->setPalette(p);
    }
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotColorCharacter()
{
    QColor c = QColorDialog::getColor(
                 a_colorCharacter->palette().color(a_colorCharacter->foregroundRole()),
                 this);
    if(c.isValid()) {
        QPalette p = a_colorCharacter->palette();
        p.setColor(a_colorCharacter->foregroundRole(), c);
        a_colorCharacter->setPalette(p);
    }
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotColorDataType()
{
    QColor c = QColorDialog::getColor(
                 a_colorDataType->palette().color(a_colorDataType->foregroundRole()),
                 this);
    if(c.isValid()) {
        QPalette p = a_colorDataType->palette();
        p.setColor(a_colorDataType->foregroundRole(), c);
        a_colorDataType->setPalette(p);
    }
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotColorAttribute()
{
    QColor c = QColorDialog::getColor(
                 a_colorAttribute->palette().color(a_colorAttribute->foregroundRole()),
                 this);
    if(c.isValid()) {
        QPalette p = a_colorAttribute->palette();
        p.setColor(a_colorAttribute->foregroundRole(), c);
        a_colorAttribute->setPalette(p);
    }
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotColorDirective()
{
    QColor c = QColorDialog::getColor(
                 a_colorDirective->palette().color(a_colorDirective->foregroundRole()),
                 this);
    if(c.isValid()) {
        QPalette p = a_colorDirective->palette();
        p.setColor(a_colorDirective->foregroundRole(), c);
        a_colorDirective->setPalette(p);
    }
}

// -----------------------------------------------------------
void QucsSettingsDialog::slotColorTask()
{
    QColor c = QColorDialog::getColor(
                 a_colorTask->palette().color(a_colorTask->foregroundRole()),
                 this);
    if(c.isValid()) {
        QPalette p = a_colorTask->palette();
        p.setColor(a_colorTask->foregroundRole(), c);
        a_colorTask->setPalette(p);
    }
}

void QucsSettingsDialog::slotTableClicked(int row, int col)
{
    Q_UNUSED(col);
    a_inputSuffix->setText(a_fileTypesTableWidget->item(row,0)->text());
    a_inputProgram->setText(a_fileTypesTableWidget->item(row,1)->text());
}

// -----------------------------------------------------------
// The locations tab slots

void QucsSettingsDialog::slotHomeDirBrowse()
{
  QString d = QFileDialog::getExistingDirectory
    (this, tr("Select the home directory"),
     a_homeEdit->text(),
     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if(!d.isEmpty())
    a_homeEdit->setText(d);
}

void QucsSettingsDialog::slotAdmsXmlDirBrowse()
{
  QString d = QFileDialog::getExistingDirectory
    (this, tr("Select the admsXml bin directory"),
     a_admsXmlEdit->text(),
     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if(!d.isEmpty())
    a_admsXmlEdit->setText(d);
}

void QucsSettingsDialog::slotAscoDirBrowse()
{
  QString d = QFileDialog::getExistingDirectory
    (this, tr("Select the ASCO bin directory"),
     a_ascoEdit->text(),
     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if(!d.isEmpty())
    a_ascoEdit->setText(d);
}

void QucsSettingsDialog::slotOctaveDirBrowse()
{
  QString d = QFileDialog::getOpenFileName(this, tr("Select the octave executable"),
                                           a_octaveEdit->text(), "All files (*)");

  if(!d.isEmpty())
    a_octaveEdit->setText(d);
}

void QucsSettingsDialog::slotOpenVAFDirBrowse()
{
  QString d = QFileDialog::getOpenFileName(this, tr("Select the OpenVAF executable"),
                                           a_openVAFEdit->text(), "All files (*)");

  if(!d.isEmpty())
    a_openVAFEdit->setText(d);
}

void QucsSettingsDialog::slotRFLayoutDirBrowse()
{
  QString d = QFileDialog::getOpenFileName(this, tr("Select the Qucs-RFLayout executable"),
                                           a_rfLayoutEdit->text(), "All files (*)");

  if(!d.isEmpty())
    a_rfLayoutEdit->setText(d);
}

/*! \brief (seems unused at present)
 */
void QucsSettingsDialog::slotPathTableClicked(int row, int col)
{
    Q_UNUSED(row);
    Q_UNUSED(col);
    //Input_Path->setText(a_fileTypesTableWidget->item(row,0)->text());
}

/* \brief enable "Remove Path" button only if something is selected
 */
void QucsSettingsDialog::slotPathSelectionChanged()
{
  bool selectionIsNotEmpty = !a_pathsTableWidget->selectedItems().isEmpty();

  a_removePathButt->setEnabled(selectionIsNotEmpty);
}

void QucsSettingsDialog::slotPathTypeSelectionChanged(int)
{
    makePathTable();
}

void QucsSettingsDialog::slotAddPath()
{
  QString d = QFileDialog::getExistingDirectory
    (this, tr("Select a directory"),
     QucsSettings.QucsWorkDir.canonicalPath(),
     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if(!d.isEmpty())
    {
        getCurrentPathRef().append(d);
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
        getCurrentPathRef().append(d);
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
                getCurrentPathRef().append(thispath.canonicalPath());
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
    //Input_Path->setText(a_fileTypesTableWidget->item(row,0)->text());
    // get the selected items from the table
    QList<QTableWidgetItem *> selectedPaths = a_pathsTableWidget->selectedItems();

    for (QTableWidgetItem * item : selectedPaths)
    {
        QString path = item->text();
        //removedPaths.append(path);
        int pathind = getCurrentPathRef().indexOf(path,0);
        if (pathind != -1)
        {
            getCurrentPathRef().removeAt(pathind);
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
    a_pathsTableWidget->clearContents();
    a_pathsTableWidget->setRowCount(0);

    // fill listview with the list of paths
    for (const QString& pathstr : getCurrentPathRef())
    {
        int row = a_pathsTableWidget->rowCount();
        a_pathsTableWidget->setRowCount(row+1);
        QTableWidgetItem *path = new QTableWidgetItem(pathstr);
        path->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        a_pathsTableWidget->setItem(row, 0, path);
    }
}

QStringList& QucsSettingsDialog::getCurrentPathRef() const
{
    return *static_cast<QStringList*>(a_pathsTypeCombo->currentData().value<QStringList*>());
}
