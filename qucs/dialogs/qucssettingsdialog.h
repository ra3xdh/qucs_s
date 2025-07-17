/***************************************************************************
                           qucssettingsdialog.h
                          ----------------------
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

#ifndef QUCSSETTINGSDIALOG_H
#define QUCSSETTINGSDIALOG_H

#include "qucs.h"

#include <QDialog>
#include <QFont>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

class QLineEdit;
class QCheckBox;
class QVBoxLayout;
class QPushButton;
class QComboBox;
class QIntValidator;
class QRegExpValidator;
class QStandardItemModel;
class QTableWidget;

class QucsSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    QucsSettingsDialog(QucsApp *parent=0);
    ~QucsSettingsDialog();

private slots:
    void slotOK();
    void slotApply();
    void slotFontDialog();
    void slotAppFontDialog();
    void slotTextFontDialog();
    void slotBGColorDialog();
    void slotGridColorDialog();
    void slotDefaultValues();
    void slotAddFileType();
    void slotRemoveFileType();
    void slotColorComment();
    void slotColorString();
    void slotColorInteger();
    void slotColorReal();
    void slotColorCharacter();
    void slotColorDataType();
    void slotColorAttribute();
    void slotColorDirective();
    void slotColorTask();
    void slotTableClicked(int,int);
    void slotPathTableClicked(int,int);

    void slotHomeDirBrowse();
    void slotAdmsXmlDirBrowse();
    void slotAscoDirBrowse();
    void slotOctaveDirBrowse();
    void slotOpenVAFDirBrowse();
    void slotRFLayoutDirBrowse();

    void slotAddPath();
    void slotAddPathWithSubFolders();
    void slotRemovePath();
    void slotPathSelectionChanged();
    void slotPathTypeSelectionChanged(int);

private:
    void makePathTable();
    QStringList& getCurrentPathRef() const;

    QucsApp *a_app;

    QFont a_font;
    QFont a_appFont;
    QFont a_textFont;
    QTabWidget* a_tabWidget;
    QWidget* a_appSettingsTab;
    QWidget* a_fileTypesTab;
    QWidget* a_locationsTab;
    QWidget* a_editorTab;
    QCheckBox* a_checkWiring;
    QCheckBox* a_checkLoadFromFutureVersions;
    QCheckBox* a_allowFlexibleWires;
    QCheckBox* a_allowLayingWiresAnew;
    QCheckBox* a_checkAntiAliasing;
    QCheckBox* a_checkTextAntiAliasing;
    QCheckBox* a_checkFullTraceNames;
    QCheckBox* a_alwaysPrefixDataset;
    QComboBox* a_languageCombo;
    QComboBox* a_styleCombo;
    QPushButton* a_fontButton;
    QPushButton* a_appFontButton;
    QPushButton* a_textFontButton;
    QPushButton* a_bGColorButton;
    QPushButton* a_gridColorButton;
    QLineEdit* a_largeFontSizeEdit;
    QLineEdit* a_undoNumEdit;
    QLineEdit* a_editorEdit;
    QLineEdit* a_inputSuffix;

    QLineEdit* a_inputProgram;
    QLineEdit* a_homeEdit;
    QLineEdit* a_admsXmlEdit;
    QLineEdit* a_ascoEdit;
    QLineEdit* a_octaveEdit;

    QLineEdit* a_openVAFEdit;
    QLineEdit* a_rfLayoutEdit;
    QLineEdit* a_graphLineWidthEdit;

    QTableWidget* a_fileTypesTableWidget;
    QTableWidget* a_pathsTableWidget;
    QComboBox* a_pathsTypeCombo;
    QPushButton* a_colorComment;
    QPushButton* a_colorString;
    QPushButton* a_colorInteger;
    QPushButton* a_colorReal;
    QPushButton* a_colorCharacter;
    QPushButton* a_colorDataType;
    QPushButton* a_colorAttribute;
    QPushButton* a_colorDirective;
    QPushButton* a_colorTask;
    QPushButton* a_removePathButt;

    QVBoxLayout* a_all;
    QIntValidator* a_val50;
    QIntValidator* a_val200;
    QRegularExpression a_expr;
    QRegularExpressionValidator* a_validator;
    QStringList a_subcktPaths;
    QStringList a_xmlCompPaths;
};

#endif
