/*
 *  Copyright (C) 2014 Yoda Lee (YodaLee) <lc85301@gmail.com>
 *  Copyright (C) 2025 Andrés Martínez Mera - andresmmera@protonmail.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * @file qucsshortcutdialog.h
 * @brief Dialog for editing keyboard shortcuts in Qucs-S
 * @authors Yoda Lee (original author, 2015), Andrés Martínez Mera (porting and
 * update, 2025)
 * @date Nov'25
 */

#ifndef QUCSSHORTCUTDIALOG_H_
#define QUCSSHORTCUTDIALOG_H_

#include "../qucsshortcutmanager.h"
#include <QDialog>

class QListWidget;
class QTableWidget;
class QLabel;
class QPushButton;
class QLineEdit;
class QKeyEvent;
class QucsApp;

/**
 * @class QucsShortcutDialog
 * @brief Interactive dialog for viewing and editing keyboard shortcuts
 *
 * This dialog provides a user interface for managing all keyboard shortcuts
 * in Qucs-S. Features:
 * - Category-based organization of shortcuts
 * - Visual conflict detection and highlighting
 * - Search functionality
 * - Import/export to JSON files
 * - Reset to defaults
 * - Interactive key capture for setting shortcuts
 *
 * The dialog interacts with QucsShortcutManager to persist changes.
 */

class QucsShortcutDialog : public QDialog {
  Q_OBJECT
public:
  /**
   * @brief Constructs the shortcut dialog
   * @param app Pointer to the main application instance
   * @param parent Parent widget, or nullptr for top-level dialog
   */
  explicit QucsShortcutDialog(QucsApp *app, QWidget *parent = nullptr);
  /**
   * @brief Destructor
   */
  virtual ~QucsShortcutDialog();

private slots:
  /**
   * @brief Handles category selection change
   *
   * Populates the action table with shortcuts from the selected category
   * and highlights any conflicts.
   */
  void slotChooseMenu();

  /**
   * @brief Initiates shortcut capture for selected action
   *
   * Equivalent to double-clicking the shortcut column. Enters key
   * capture mode where the next key press will be assigned as the shortcut.
   */
  void slotSetShortcut();

  /**
   * @brief Removes the shortcut from the selected action
   *
   */
  void slotRemoveShortcut();

  /**
   * @brief Resets all shortcuts in the current category to defaults
   *
   * Prompts for confirmation. This action cannot be undone except
   * by manually reconfiguring shortcuts or importing a saved file.
   */
  void slotDefaultShortcut();

  /**
   * @brief Resets all shortcuts in all categories to defaults
   *
   * Prompts for confirmation. This action cannot be undone except
   * by manually reconfiguring shortcuts or importing a saved file.
   */
  void slotResetAllShortcuts();

  /**
   * @brief Saves changes and closes the dialog
   *
   * Persists modified shortcuts to QSettings and accepts the dialog.
   */
  void slotOK();

  /**
   * @brief Imports shortcuts from a JSON file
   *
   * Opens a file dialog to select a JSON file containing previously
   * exported shortcuts. Validates the file format before loading.
   */
  void slotImport();

  /**
   * @brief Exports shortcuts to a JSON file
   *
   * Opens a file dialog to save current shortcuts to a JSON file.
   */
  void slotExport();

  /**
   * @brief Handles table selection changes
   *
   * Clears the message label when selection changes (unless capturing keys).
   */
  void slotItemSelectionChanged();

  /**
   * @brief Handles search text changes
   * @param text: search query
   *
   * Filters shortcuts in the current category. If no matches are found
   * in the current category, automatically switches to the first category
   * that contains matching shortcuts.
   */
  void slotSearchShortcuts(const QString &text);

  /**
   * @brief Handles double-click on table cell
   * @param row: Row index of the double-clicked cell
   * @param column: Column index of the double-clicked cell
   *
   * When the shortcut column (column 1) is double-clicked, enters key
   * capture mode to assign a new shortcut. Double-clicking the action
   * column has no effect.
   */
  void slotCellDoubleClicked(int row, int column);

protected:
  /**
   * @brief Captures keyboard input for shortcut assignment
   * @param event The key press event
   *
   * When in key capture mode (m_capturingKey == true), this method
   * intercepts key presses to use as the new shortcut.
   */
  void keyPressEvent(QKeyEvent *event) override;

private:
  QucsApp *App;                   /// Pointer to main application
  QucsShortcutManager &m_manager; /// Reference to shortcut manager
  bool m_capturingKey;            /// Flag indicating key capture mode

  /**
   * @brief Populates the category list widget
   *
   * Retrieves all categories from the manager, sorts them alphabetically,
   * and fills the menuList widget.
   */
  void fillMenu();

  /**
   * @brief Updates the shortcut display (currently unused)
   *
   * Placeholder for future functionality to refresh the table view.
   */
  void updateShortcutDisplay();

  // UI Components
  QListWidget *menuList;    /// List of categories
  QTableWidget *actionList; /// Table of actions and shortcuts
  QLabel *messageLabel;     /// Message display
  QLineEdit *searchBox;     /// Search input field

  QPushButton *removeButton;   /// Remove shortcut button
  QPushButton *defaultButton;  /// Reset category button
  QPushButton *resetAllButton; /// Reset all shortcuts button
  QPushButton *okButton;       /// OK/Save button
  QPushButton *importButton;   /// Import shortcuts button
  QPushButton *exportButton;   /// Export shortcuts button
};

#endif
