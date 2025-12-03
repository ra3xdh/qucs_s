/***************************************************************************
                           qucssettingsdialog.cpp
                          ----------------------
    begin                : Fri Oct 3 2014
    copyright            : (C) 2014 by Yodalee
    email                : lc85301@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qucsshortcutdialog.h"
#include "keysequenceedit.h"
#include "qucs.h"
#include "qucsshortcutmanager.h"

#include <QDebug>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>

QucsShortcutDialog::QucsShortcutDialog(QucsApp *app, QWidget *parent)
    : QDialog(parent), App(app), m_manager(QucsShortcutManager::instance()) {
  qDebug() << "Open shortcut dialog";
  setWindowTitle(tr("Edit Qucs-S Shortcuts"));

  // Left panel: Category list
  menuList = new QListWidget();

  // Right panel: Actions table
  actionList = new QTableWidget();
  actionList->horizontalHeader()->setStretchLastSection(true);
  actionList->verticalHeader()->hide();
  actionList->setColumnCount(2);
  actionList->setHorizontalHeaderLabels(QStringList()
                                        << tr("Action") << tr("Shortcut Key"));
  actionList->setSelectionBehavior(QAbstractItemView::SelectRows);
  actionList->setColumnWidth(0, 300); // Action column
  actionList->setColumnWidth(1, 150); // Shortcut column

  // Search box
  searchBox = new QLineEdit();
  searchBox->setPlaceholderText(tr("Search shortcuts..."));
  connect(searchBox, SIGNAL(textChanged(QString)),
          SLOT(slotSearchShortcuts(QString)));

  // Input widgets
  sequenceInput = new KeySequenceEdit();
  messageLabel = new QLabel();

  // Buttons
  setButton = new QPushButton(tr("Set shortcut"));
  removeButton = new QPushButton(tr("Remove shortcut"));
  defaultButton = new QPushButton(tr("Reset Category"));
  resetAllButton = new QPushButton(tr("Reset All"));
  okButton = new QPushButton(tr("OK"));
  importButton = new QPushButton(tr("Import"));
  exportButton = new QPushButton(tr("Export"));

  // Connect signals
  connect(sequenceInput, SIGNAL(textChanged(QString)), SLOT(slotCheckKey()));
  connect(this, SIGNAL(signalValidKey()), SLOT(slotCheckKey()));
  connect(actionList, SIGNAL(itemSelectionChanged()),
          SLOT(slotItemSelectionChanged()));

  connect(setButton, SIGNAL(clicked()), SLOT(slotSetShortcut()));
  connect(removeButton, SIGNAL(clicked()), SLOT(slotRemoveShortcut()));
  connect(defaultButton, SIGNAL(clicked()), SLOT(slotDefaultShortcut()));
  connect(resetAllButton, SIGNAL(clicked()), SLOT(slotResetAllShortcuts()));
  connect(okButton, SIGNAL(clicked()), SLOT(slotOK()));
  connect(importButton, SIGNAL(clicked()), SLOT(slotImport()));
  connect(exportButton, SIGNAL(clicked()), SLOT(slotExport()));
  connect(menuList, SIGNAL(itemClicked(QListWidgetItem *)),
          SLOT(slotChooseMenu()));

  // Layout setup
  QVBoxLayout *leftLayout = new QVBoxLayout();
  leftLayout->addWidget(new QLabel(tr("Categories:")));
  leftLayout->addWidget(menuList);

  QVBoxLayout *rightLayout = new QVBoxLayout();
  rightLayout->addWidget(searchBox);
  rightLayout->addWidget(actionList);

  QHBoxLayout *topLayout = new QHBoxLayout();
  topLayout->addLayout(leftLayout, 1);
  topLayout->addLayout(rightLayout, 3);

  QVBoxLayout *btnLayout = new QVBoxLayout();
  btnLayout->addWidget(setButton);
  btnLayout->addWidget(removeButton);
  btnLayout->addWidget(defaultButton);
  btnLayout->addWidget(resetAllButton);

  QVBoxLayout *textLayout = new QVBoxLayout();
  textLayout->addWidget(new QLabel(tr("New shortcut:")));
  textLayout->addWidget(sequenceInput);
  textLayout->addWidget(messageLabel);

  QHBoxLayout *midLayout = new QHBoxLayout();
  midLayout->addLayout(textLayout, 3);
  midLayout->addLayout(btnLayout, 1);

  QHBoxLayout *bottomLayout = new QHBoxLayout();
  bottomLayout->addWidget(importButton);
  bottomLayout->addWidget(exportButton);
  bottomLayout->addStretch();
  bottomLayout->addWidget(okButton);

  QVBoxLayout *mainLayout = new QVBoxLayout();
  mainLayout->addLayout(topLayout);
  mainLayout->addLayout(midLayout);
  mainLayout->addLayout(bottomLayout);

  setLayout(mainLayout);

  // Set dialog size
  setMinimumSize(800, 600);
  resize(900, 650);

  // Fill initial values
  fillMenu();
  if (menuList->count() > 0) {
    menuList->setCurrentRow(0);
    slotChooseMenu();
  }
}

QucsShortcutDialog::~QucsShortcutDialog() {}

// ----------------------------------------------------------------------------
// Fill the category list
// ----------------------------------------------------------------------------
void QucsShortcutDialog::fillMenu() {
  menuList->clear();

  // Get all categories from manager - O(c) where c is number of categories
  QList<QString> categories = m_manager.allCategories();

  // Sort alphabetically for better UX
  categories.sort();

  for (const QString &category : qAsConst(categories)) {
    QListWidgetItem *item = new QListWidgetItem(category);
    menuList->addItem(item);
  }

  if (!categories.isEmpty()) {
    menuList->setCurrentRow(0);
  }
}

// ----------------------------------------------------------------------------
// Display shortcuts for selected category
// ----------------------------------------------------------------------------
void QucsShortcutDialog::slotChooseMenu() {
  QListWidgetItem *current = menuList->currentItem();
  if (!current) {
    return;
  }

  QString category = current->text();

  // Clear current display
  actionList->clearContents();
  actionList->setRowCount(0);

  // Get commands in this category - O(1) category lookup
  QList<QucsCommand *> commands = m_manager.commandsInCategory(category);

  // Populate table
  actionList->setRowCount(commands.size());
  for (int row = 0; row < commands.size(); ++row) {
    QucsCommand *cmd = commands[row];

    QTableWidgetItem *item_0 = new QTableWidgetItem(cmd->description());
    QTableWidgetItem *item_1 =
        new QTableWidgetItem(cmd->currentKeySequence().toString());

    // Make non-editable
    item_0->setFlags(item_0->flags() & ~Qt::ItemIsEditable);
    item_1->setFlags(item_1->flags() & ~Qt::ItemIsEditable);

    // Store command ID in item for quick access
    item_0->setData(Qt::UserRole, cmd->id());

    // Mark modified shortcuts
    if (cmd->isModified()) {
      QFont font = item_1->font();
      font.setBold(true);
      item_1->setFont(font);
      item_1->setToolTip(tr("Modified from default: %1")
                             .arg(cmd->defaultKeySequence().toString()));
    }

    actionList->setItem(row, 0, item_0);
    actionList->setItem(row, 1, item_1);
  }

  if (actionList->rowCount() > 0) {
    actionList->setCurrentCell(0, 0);
  }

  // Highlight any conflicts
  highlightConflicts();
}

// ----------------------------------------------------------------------------
// Highlight conflicting shortcuts visually
// ----------------------------------------------------------------------------
void QucsShortcutDialog::highlightConflicts() {
  // First, reset all backgrounds
  for (int row = 0; row < actionList->rowCount(); ++row) {
    actionList->item(row, 1)->setBackground(Qt::white);
    actionList->item(row, 1)->setToolTip("");
  }

  // Find all shortcuts in current view
  QHash<QString, QList<int>> shortcutRows;

  for (int row = 0; row < actionList->rowCount(); ++row) {
    QString shortcut = actionList->item(row, 1)->text();
    if (!shortcut.isEmpty()) {
      shortcutRows[shortcut].append(row);
    }
  }

  // Highlight duplicates within category
  for (auto it = shortcutRows.begin(); it != shortcutRows.end(); ++it) {
    if (it.value().size() > 1) {
      for (int row : it.value()) {
        actionList->item(row, 1)->setBackground(QColor(255, 200, 200));
        actionList->item(row, 1)->setToolTip(
            tr("⚠ Warning: This shortcut is assigned to multiple actions in "
               "this category!"));
      }
    }
  }

  // Also check for conflicts with other categories
  for (int row = 0; row < actionList->rowCount(); ++row) {
    QString shortcutStr = actionList->item(row, 1)->text();
    if (shortcutStr.isEmpty())
      continue;

    QKeySequence key(shortcutStr);
    QString currentId = actionList->item(row, 0)->data(Qt::UserRole).toString();

    // O(k) conflict check where k is conflicts (usually 0-2)
    QList<ShortcutConflict> conflicts = m_manager.findConflicts(key);

    if (conflicts.size() > 1) {
      // Build tooltip with all conflicts
      QString tooltip = tr("⚠ This shortcut conflicts with:\n");
      for (const auto &conflict : conflicts) {
        if (conflict.commandId != currentId) {
          tooltip += QString("  • %1: %2\n")
                         .arg(conflict.category, conflict.description);
        }
      }

      QTableWidgetItem *item = actionList->item(row, 1);
      if (item->background() != QColor(255, 200, 200)) {
        item->setBackground(QColor(255, 230, 200)); // Orange for cross-category
      }
      item->setToolTip(tooltip);
    }
  }
}

// ----------------------------------------------------------------------------
// Set a new shortcut
// ----------------------------------------------------------------------------
void QucsShortcutDialog::slotSetShortcut() {
  int row = actionList->currentRow();
  if (row < 0) {
    QMessageBox::information(this, tr("Info"),
                             tr("Please select an action first!"));
    return;
  }

  QKeySequence newKey = QKeySequence(sequenceInput->text());
  if (newKey.isEmpty() && !sequenceInput->text().isEmpty()) {
    QMessageBox::warning(this, tr("Warning"), tr("Invalid key sequence!"));
    return;
  }

  QString commandId = actionList->item(row, 0)->data(Qt::UserRole).toString();

  // Check for conflicts
  QList<ShortcutConflict> conflicts = m_manager.findConflicts(newKey);
  if (!conflicts.isEmpty()) {
    QString msg = tr("This shortcut is already used by:\n\n");
    for (const auto &conflict : qAsConst(conflicts)) {
      msg +=
          QString("  • %1: %2\n").arg(conflict.category, conflict.description);
    }
    msg += tr("\nDo you want to reassign it?");

    int ret = QMessageBox::question(this, tr("Conflict Detected"), msg,
                                    QMessageBox::Yes | QMessageBox::No);
    if (ret != QMessageBox::Yes) {
      return;
    }
  }

  // Apply the shortcut - O(1)
  if (m_manager.setShortcut(commandId, newKey)) {
    actionList->item(row, 1)->setText(newKey.toString());
    sequenceInput->clear();
    messageLabel->clear();
    messageLabel->setStyleSheet("");

    // Update display
    highlightConflicts();

    // Mark as modified
    QucsCommand *cmd = m_manager.command(commandId);
    if (cmd && cmd->isModified()) {
      QFont font = actionList->item(row, 1)->font();
      font.setBold(true);
      actionList->item(row, 1)->setFont(font);
    }
  } else {
    QMessageBox::warning(this, tr("Error"), tr("Failed to set shortcut!"));
  }
}

// ----------------------------------------------------------------------------
// Remove shortcut from selected action
// ----------------------------------------------------------------------------
void QucsShortcutDialog::slotRemoveShortcut() {
  int row = actionList->currentRow();
  if (row < 0) {
    return;
  }

  QString commandId = actionList->item(row, 0)->data(Qt::UserRole).toString();

  // Remove shortcut - O(1)
  m_manager.removeShortcut(commandId);

  actionList->item(row, 1)->setText("");
  sequenceInput->clear();
  messageLabel->clear();
  messageLabel->setStyleSheet("");

  highlightConflicts();
}

// ----------------------------------------------------------------------------
// Reset current category to defaults
// ----------------------------------------------------------------------------
void QucsShortcutDialog::slotDefaultShortcut() {
  QListWidgetItem *current = menuList->currentItem();
  if (!current) {
    return;
  }

  QString category = current->text();

  int ret = QMessageBox::question(
      this, tr("Reset Category"),
      tr("Reset all shortcuts in '%1' to defaults?").arg(category),
      QMessageBox::Yes | QMessageBox::No);

  if (ret == QMessageBox::Yes) {
    m_manager.resetCategory(category);
    slotChooseMenu(); // Refresh display
  }
}

// ----------------------------------------------------------------------------
// Reset ALL shortcuts to defaults
// ----------------------------------------------------------------------------
void QucsShortcutDialog::slotResetAllShortcuts() {
  int ret = QMessageBox::question(
      this, tr("Reset All Shortcuts"),
      tr("Reset ALL shortcuts to factory defaults?\n\nThis cannot be undone!"),
      QMessageBox::Yes | QMessageBox::No);

  if (ret == QMessageBox::Yes) {
    m_manager.resetToDefaults();
    slotChooseMenu(); // Refresh display
    QMessageBox::information(this, tr("Success"),
                             tr("All shortcuts have been reset to defaults."));
  }
}

// ----------------------------------------------------------------------------
// OK button - save and close
// ----------------------------------------------------------------------------
void QucsShortcutDialog::slotOK() {
  // Save to settings
  m_manager.saveToSettings();

  // Shortcuts are already applied to QActions via QucsCommand
  // No need to call App->setAllShortcut() anymore

  accept();
}

// ----------------------------------------------------------------------------
// Import shortcuts from JSON file
// ----------------------------------------------------------------------------
void QucsShortcutDialog::slotImport() {
  QString filename =
      QFileDialog::getOpenFileName(this, tr("Import Shortcut Map"), ".",
                                   "JSON Files (*.json);;All Files (*)");

  if (filename.isEmpty()) {
    return;
  }

  // Load from file - O(n) where n is total commands
  if (m_manager.loadFromFile(filename)) {
    slotChooseMenu(); // Refresh display
    QMessageBox::information(
        this, tr("Success"),
        tr("Shortcuts imported successfully from:\n%1").arg(filename));
  } else {
    QMessageBox::critical(this, tr("Error"),
                          tr("Failed to import shortcuts from:\n%1\n\n"
                             "Please check that the file is a valid Qucs-S "
                             "shortcut file.")
                              .arg(filename));
  }
}

// ----------------------------------------------------------------------------
// Export shortcuts to JSON file
// ----------------------------------------------------------------------------
void QucsShortcutDialog::slotExport() {
  QString filename =
      QFileDialog::getSaveFileName(this, tr("Export Shortcut Map"), ".",
                                   "JSON Files (*.json);;All Files (*)");

  if (filename.isEmpty()) {
    return;
  }

  // Ensure .json extension
  if (!filename.endsWith(".json", Qt::CaseInsensitive)) {
    filename += ".json";
  }

  // Save to file
  if (m_manager.saveToFile(filename)) {
    QMessageBox::information(
        this, tr("Success"),
        tr("Shortcuts exported successfully to:\n%1").arg(filename));
  } else {
    QMessageBox::critical(
        this, tr("Error"),
        tr("Failed to export shortcuts to:\n%1").arg(filename));
  }
}

// ----------------------------------------------------------------------------
// Check if entered key is valid
// ----------------------------------------------------------------------------
void QucsShortcutDialog::slotCheckKey() {
  QString keyText = sequenceInput->text();

  if (keyText.isEmpty()) {
    messageLabel->clear();
    messageLabel->setStyleSheet("");
    return;
  }

  QKeySequence key(keyText);
  QString errorMsg;

  // Validate the key
  if (!m_manager.isValidShortcut(key, errorMsg)) {
    messageLabel->setText("❌ " + errorMsg);
    messageLabel->setStyleSheet("QLabel { color: red; }");
    return;
  }

  // Check for conflicts - O(k) where k is conflicts
  QList<ShortcutConflict> conflicts = m_manager.findConflicts(key);

  if (conflicts.isEmpty()) {
    messageLabel->setText("✓ No conflicts");
    messageLabel->setStyleSheet("QLabel { color: green; }");
  } else {
    QString msg = "⚠ Conflicts with:\n";
    for (const auto &conflict : qAsConst(conflicts)) {
      msg +=
          QString("  • %1: %2\n").arg(conflict.category, conflict.description);
    }
    messageLabel->setText(msg);
    messageLabel->setStyleSheet("QLabel { color: orange; }");
  }
}

// ----------------------------------------------------------------------------
// When action selected, focus input
// ----------------------------------------------------------------------------
void QucsShortcutDialog::slotSetFocus() { sequenceInput->setFocus(); }

// ----------------------------------------------------------------------------
// When selection changes
// ----------------------------------------------------------------------------
void QucsShortcutDialog::slotItemSelectionChanged() {
  int row = actionList->currentRow();
  if (row >= 0) {
    // Show current shortcut in input field
    QString currentShortcut = actionList->item(row, 1)->text();
    sequenceInput->setText(currentShortcut);
    sequenceInput->selectAll();
    sequenceInput->setFocus();
  }
}

// ----------------------------------------------------------------------------
// Search across ALL categories and switch to matching category
// ----------------------------------------------------------------------------
void QucsShortcutDialog::slotSearchShortcuts(const QString &text) {
    if (text.isEmpty()) {
        // Show all rows in current category
        for (int row = 0; row < actionList->rowCount(); ++row) {
            actionList->setRowHidden(row, false);
        }
        return;
    }

    QString searchLower = text.toLower();

    // Search through ALL categories
    QList<QString> allCategories = m_manager.allCategories();

    QString firstMatchCategory;
    int firstMatchRow = -1;
    bool foundInCurrentCategory = false;

    // Get current category
    QListWidgetItem *currentItem = menuList->currentItem();
    QString currentCategory = currentItem ? currentItem->text() : "";

    // First, check if there's a match in the current category
    for (int row = 0; row < actionList->rowCount(); ++row) {
        QString action = actionList->item(row, 0)->text().toLower();
        QString shortcut = actionList->item(row, 1)->text().toLower();

        bool matches = action.contains(searchLower) || shortcut.contains(searchLower);

        if (matches) {
            foundInCurrentCategory = true;
            actionList->setRowHidden(row, false);
        } else {
            actionList->setRowHidden(row, true);
        }
    }

    // If found in current category, we're done
    if (foundInCurrentCategory) {
        return;
    }

    // Otherwise, search all other categories
    for (const QString &category : qAsConst(allCategories)) {
        if (category == currentCategory) {
            continue; // Already checked
        }

        QList<QucsCommand *> commands = m_manager.commandsInCategory(category);

        for (int i = 0; i < commands.size(); ++i) {
            QucsCommand *cmd = commands[i];
            QString action = cmd->description().toLower();
            QString shortcut = cmd->currentKeySequence().toString().toLower();

            if (action.contains(searchLower) || shortcut.contains(searchLower)) {
                // Found a match! Remember the first one
                if (firstMatchCategory.isEmpty()) {
                    firstMatchCategory = category;
                    firstMatchRow = i;
                }
            }
        }

        // If we found a match, no need to search further
        if (!firstMatchCategory.isEmpty()) {
            break;
        }
    }

    // If we found a match in another category, switch to it
    if (!firstMatchCategory.isEmpty()) {
        // Find the category in the list
        for (int i = 0; i < menuList->count(); ++i) {
            if (menuList->item(i)->text() == firstMatchCategory) {
                // Block signals to avoid triggering slotChooseMenu
                menuList->blockSignals(true);
                menuList->setCurrentRow(i);
                menuList->blockSignals(false);

                // Manually refresh the action list
                slotChooseMenu();

                // Now filter the newly loaded category
                for (int row = 0; row < actionList->rowCount(); ++row) {
                    QString action = actionList->item(row, 0)->text().toLower();
                    QString shortcut = actionList->item(row, 1)->text().toLower();

                    bool matches = action.contains(searchLower) || shortcut.contains(searchLower);
                    actionList->setRowHidden(row, !matches);
                }

                // Select the first matching row if possible
                if (firstMatchRow >= 0 && firstMatchRow < actionList->rowCount()) {
                    actionList->setCurrentCell(firstMatchRow, 0);
                }

                break;
            }
        }
    } else {
        // No matches found in any category
        // Hide all rows and maybe show a message
        for (int row = 0; row < actionList->rowCount(); ++row) {
            actionList->setRowHidden(row, true);
        }
    }
}
