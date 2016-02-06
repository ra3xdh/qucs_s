/***************************************************************************
                           qucssettingsdialog.h
                          ----------------------
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

#ifndef QUCSSETTINGSDIALOG_H
#define QUCSSETTINGSDIALOG_H

#include "qucs.h"

#include <QDialog>
#include <QFont>
#include <QRegExp>
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
    QucsSettingsDialog(QucsApp *parent=0, const char *name=0);
    ~QucsSettingsDialog();

private slots:
    void slotOK();
    void slotApply();
    void slotFontDialog();
    void slotBGColorDialog();
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

    void slotAddPath();
    void slotAddPathWithSubFolders();
    void slotRemovePath();
    void slotPathSelectionChanged();

public:
    QucsApp *App;

    QFont Font;
    QCheckBox *checkWiring, *checkLoadFromFutureVersions,
              *checkAntiAliasing, *checkTextAntiAliasing;
    QComboBox *LanguageCombo;
    QPushButton *FontButton, *BGColorButton;
    QLineEdit *LargeFontSizeEdit, *undoNumEdit, *editorEdit, *Input_Suffix,
              *Input_Program, *homeEdit, *admsXmlEdit, *ascoEdit, *octaveEdit;
    QTableWidget *fileTypesTableWidget, *pathsTableWidget;
    QStandardItemModel *model;
    QPushButton *ColorComment, *ColorString, *ColorInteger,
                *ColorReal, *ColorCharacter, *ColorDataType, *ColorAttribute,
                *ColorDirective, *ColorTask;
    QPushButton *RemovePathButt;

    QVBoxLayout *all;
    QIntValidator *val50;
    QIntValidator *val200;
    QRegExp Expr;
    QRegExpValidator *Validator;

private:
    QStringList currentPaths;
    

private:
    void makePathTable();

};

#endif
