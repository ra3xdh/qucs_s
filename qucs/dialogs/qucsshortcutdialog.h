/*
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

#ifndef QUCSSHORTCUTDIALOG_H_
#define QUCSSHORTCUTDIALOG_H_

#include "main.h"
#include "qucs.h"
#include "qucsshortcutmanager.h"
#include "keysequenceedit.h"
#include <QDialog>

class QListWidget;
class QTableWidget;
class QLabel;
class KeySequenceEdit;
class QPushButton;
class QLineEdit;

class QucsShortcutDialog : public QDialog {
  Q_OBJECT
public:
  explicit QucsShortcutDialog(QucsApp *app, QWidget *parent = nullptr);
  virtual ~QucsShortcutDialog();

private slots:
  void slotChooseMenu();
  void slotSetShortcut();
  void slotRemoveShortcut();
  void slotDefaultShortcut();
  void slotResetAllShortcuts();
  void slotOK();
  void slotImport();
  void slotExport();
  void slotCheckKey();
  void slotSetFocus();
  void slotItemSelectionChanged();
  void slotSearchShortcuts(const QString &text);

signals:
  void signalValidKey();

private:
  QucsApp *App;
  QucsShortcutManager &m_manager;

  void fillMenu();
  void setShortcut(const QKeySequence &key);
  void highlightConflicts();
  void updateShortcutDisplay();

  // UI Components
  QListWidget *menuList;
  QTableWidget *actionList;
  QLabel *messageLabel;
  KeySequenceEdit *sequenceInput;
  QLineEdit *searchBox;
  QPushButton *setButton, *removeButton, *defaultButton, *resetAllButton,
      *okButton, *importButton, *exportButton;
};

#endif
