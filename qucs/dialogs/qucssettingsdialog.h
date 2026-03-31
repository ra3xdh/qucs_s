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
#include "qucsshortcutdialog.h"

#include <QDialog>
#include <QFont>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QVBoxLayout>

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

    ///
    /// @brief Clears all entries from the subcircuit search path list.
    /// Prompts the user for confirmation before removing all paths from
    /// currentPaths and rebuilding the path table. This is useful when
    /// a large number of paths have been added unintentionally, e.g. via
    /// slotAddPathWithSubFolders()
    /// @see makePathTable(), ClearAllPathsButt
    ///
    void slotClearAllPaths();

public:
    QucsApp *App;

    QFont Font;
    QFont AppFont;
    QFont TextFont;
    QCheckBox *checkWiring, *checkLoadFromFutureVersions,
              *allowFlexibleWires,
              *checkAntiAliasing, *checkTextAntiAliasing,
              *checkFullTraceNames,  *alwaysPrefixDataset;
    QComboBox *LanguageCombo,
              *StyleCombo;
    QPushButton *FontButton, *AppFontButton, *TextFontButton, *BGColorButton, *GridColorButton;
    QLineEdit *LargeFontSizeEdit, *undoNumEdit, *editorEdit, *Input_Suffix,
              *Input_Program, *homeEdit, *admsXmlEdit, *ascoEdit, *octaveEdit,
              *OpenVAFEdit, *RFLayoutEdit, *graphLineWidthEdit;
    QTableWidget *fileTypesTableWidget, *pathsTableWidget;
    QStandardItemModel *model;
    QPushButton *ColorComment, *ColorString, *ColorInteger,
                *ColorReal, *ColorCharacter, *ColorDataType, *ColorAttribute,
                *ColorDirective, *ColorTask;

    QPushButton *ShortcutButton;

    QVBoxLayout *all;
    QIntValidator *val50;
    QIntValidator *val200;
    QRegularExpression Expr;
    QRegularExpressionValidator *Validator;

private:
    QStringList currentPaths;


private:
    void makePathTable();

};

#endif
