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

///
/// @class QucsSettingsDialog class
/// @brief Application settings dialog for Qucs-S.
/// Provides a tabbed interface for editing application-wide settings including appearance, source code editor colors,
/// file type associations, and library search paths. Changes can be applied immediately or on dialog acceptance.
///
class QucsSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    /// @brief Class constructor
    QucsSettingsDialog(QucsApp *parent=0);

    /// @brief Class destructor
    ~QucsSettingsDialog();

private slots:
    /// @brief Applies current settings and closes the dialog.
    void slotOK();

    /// @brief Applies current settings without closing the dialog.
    void slotApply();

    /// @brief Opens the font picker for the schematic font.
    void slotFontDialog();

    /// @brief Opens the font picker for the application font.
    void slotAppFontDialog();

    /// @brief Opens the font picker for the text document font.
    void slotTextFontDialog();

    /// @brief Opens the color picker for the document background color.
    void slotBGColorDialog();

    /// @brief Opens the color picker for the schematic grid color.
    void slotGridColorDialog();

    /// @brief Resets all settings fields to their default values.
    void slotDefaultValues();

    /// @brief Adds or updates a file type entry in the file types table.
    void slotAddFileType();

    /// @brief Removes the selected file type entry from the file types table.
    void slotRemoveFileType();

    /// @brief Opens the color picker for the comment syntax highlight color.
    void slotColorComment();

    /// @brief Opens the color picker for the string syntax highlight color.
    void slotColorString();

    /// @brief Opens the color picker for the integer number syntax highlight color.
    void slotColorInteger();

    /// @brief Opens the color picker for the real number syntax highlight color.
    void slotColorReal();

    /// @brief Opens the color picker for the character syntax highlight color.
    void slotColorCharacter();

    /// @brief Opens the color picker for the data type syntax highlight color.
    void slotColorDataType();

    /// @brief Opens the color picker for the attribute syntax highlight color
    void slotColorAttribute();

    /// @brief Opens the color picker for the directive syntax highlight color.
    void slotColorDirective();

    /// @brief Opens the color picker for the task syntax highlight color.
    void slotColorTask();

    /// @brief Populates the suffix and program fields when a file type row is clicked.
    void slotTableClicked(int,int);

    /// @brief Opens a directory picker to set the Qucs home directory.
    void slotHomeDirBrowse();

    /// @brief Opens a directory picker to set the AdmsXml binary directory.
    void slotAdmsXmlDirBrowse();

    /// @brief Opens a directory picker to set the ASCO binary directory.
    void slotAscoDirBrowse();

    /// @brief Opens a file picker to set the Octave executable path.
    void slotOctaveDirBrowse();

    /// @brief Opens a file picker to set the OpenVAF executable path.
    void slotOpenVAFDirBrowse();

    /// @brief Opens a file picker to set the Qucs-RFLayout executable path.
    void slotRFLayoutDirBrowse();

    /// @brief Opens a directory picker to add a single path to the search list.
    void slotAddPath();

    /// @brief Opens a directory picker and adds the selected directory and all
    /// its subdirectories to the subcircuit search path list.
    /// @see slotClearAllPaths(), makePathTable()
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

    /// @brief Pointer to the parent application instance.
    QucsApp *App;

    /// @brief Schematic font
    QFont Font;

    /// @brief Application-wide font
    QFont AppFont;

    /// @brief Text document font
    QFont TextFont;

    /// @brief Enables wiring when clicking an open node.
    QCheckBox *checkWiring;

    /// @brief Allows loading documents from newer versions of Qucs-S
    QCheckBox *checkLoadFromFutureVersions;

    /// @brief Enables flexible wire routing
    QCheckBox *allowFlexibleWires;

    /// @brief Enables anti-aliasing for diagram graphs.
    QCheckBox *checkAntiAliasing;

    /// @brief Enables anti-aliasing for text rendering.
    QCheckBox *checkTextAntiAliasing;

    /// @brief Shows full trace name prefixes on diagrams.
    QCheckBox *checkFullTraceNames;

    /// @brief Always prefixes the dataset with the simulation label.
    QCheckBox *alwaysPrefixDataset;

    /// @brief Selects the application language.
    QComboBox *LanguageCombo;

    /// @brief Selects the application Qt style.
    QComboBox *StyleCombo;

    /// @brief Opens the schematic font picker dialog.
    QPushButton *FontButton;

    /// @brief Opens the application font picker dialog.
    QPushButton *AppFontButton;

    /// @brief Opens the text document font picker dialog.
    QPushButton *TextFontButton;

    /// @brief Opens the background color picker dialog.
    QPushButton *BGColorButton;

    /// @brief Opens the grid color picker dialog.
    QPushButton *GridColorButton;

    QLineEdit *LargeFontSizeEdit;   ///< Large font size value.
    QLineEdit *undoNumEdit;         ///< Maximum number of undo operations.
    QLineEdit *editorEdit;          ///< Path or name of the external text editor.
    QLineEdit *Input_Suffix;        ///< File suffix field for file type registration.
    QLineEdit *Input_Program;       ///< Program field for file type registration.
    QLineEdit *homeEdit;            ///< Qucs home directory path.
    QLineEdit *admsXmlEdit;         ///< AdmsXml binary directory path.
    QLineEdit *ascoEdit;            ///< ASCO binary directory path.
    QLineEdit *octaveEdit;          ///< Octave executable path.
    QLineEdit *OpenVAFEdit;         ///< OpenVAF executable path.
    QLineEdit *RFLayoutEdit;        ///< Qucs-RFLayout executable path.
    QLineEdit *graphLineWidthEdit;  ///< Default graph line thickness.

    /// @brief Table displaying registered file type suffix/program pairs.
    QTableWidget *fileTypesTableWidget;

    /// @brief Table displaying the subcircuit search path list.
    QTableWidget *pathsTableWidget;
    QStandardItemModel *model;

    QPushButton *ColorComment;    ///< Sets the syntax highlight color for comments.
    QPushButton *ColorString;     ///< Sets the syntax highlight color for strings.
    QPushButton *ColorInteger;    ///< Sets the syntax highlight color for integer numbers.
    QPushButton *ColorReal;       ///< Sets the syntax highlight color for real numbers.
    QPushButton *ColorCharacter;  ///< Sets the syntax highlight color for characters.
    QPushButton *ColorDataType;   ///< Sets the syntax highlight color for data types.
    QPushButton *ColorAttribute;  ///< Sets the syntax highlight color for attributes.
    QPushButton *ColorDirective;  ///< Sets the syntax highlight color for directives.
    QPushButton *ColorTask;       ///< Sets the syntax highlight color for tasks.

    /// @brief Opens the custom shortcut configuration dialog.
    QPushButton *ShortcutButton;

    /// @brief Top-level vertical layout of the dialog
    QVBoxLayout *all;

    /// @brief Validator for integer fields with range 1–50.
    QIntValidator *val50;

    /// @brief Validator for integer fields with range 0–200.
    QIntValidator *val200;

    /// @brief Regular expression used for suffix validation.
    QRegularExpression Expr;

    /// @brief Validator based on Expr
    QRegularExpressionValidator *Validator;

private:
    QStringList currentPaths;


private:
    /// @brief Reconstructs the subcircuit search path table from @c currentPaths.
    void makePathTable();

};

#endif
