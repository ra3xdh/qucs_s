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

#include "qucsshortcutdialog.h"
#include "qucs.h"
#include "qucsshortcutmanager.h"

#include <QDebug>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

QucsShortcutDialog::QucsShortcutDialog(QucsApp *app, QWidget *parent)
    : QDialog(parent), App(app), m_manager(QucsShortcutManager::instance()),
      m_capturingKey(false) {
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
  actionList->setEditTriggers(QAbstractItemView::NoEditTriggers);

  // Search box
  searchBox = new QLineEdit();
  searchBox->setPlaceholderText(tr("Search shortcuts..."));
  connect(searchBox, SIGNAL(textChanged(QString)),
          SLOT(slotSearchShortcuts(QString)));

  // Message label for feedback
  messageLabel = new QLabel();
  messageLabel->setWordWrap(true);
  messageLabel->setMinimumHeight(60);

  // Buttons
  removeButton = new QPushButton(tr("Remove shortcut"));
  defaultButton = new QPushButton(tr("Reset Category"));
  resetAllButton = new QPushButton(tr("Reset All"));
  okButton = new QPushButton(tr("OK"));
  importButton = new QPushButton(tr("Import"));
  exportButton = new QPushButton(tr("Export"));

  // Connect signals
  connect(actionList, SIGNAL(itemSelectionChanged()),
          SLOT(slotItemSelectionChanged()));
  connect(actionList, SIGNAL(cellDoubleClicked(int, int)),
          SLOT(slotCellDoubleClicked(int, int)));

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
  btnLayout->addWidget(removeButton);
  btnLayout->addWidget(defaultButton);
  btnLayout->addWidget(resetAllButton);

  QVBoxLayout *instructionLayout = new QVBoxLayout();
  instructionLayout->addWidget(
      new QLabel(tr("<b>Note:</b> Double-click a shortcut to change it")));
  instructionLayout->addWidget(messageLabel);

  QHBoxLayout *midLayout = new QHBoxLayout();
  midLayout->addLayout(instructionLayout, 3);
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

  // Prevent buttons from taking focus.
  // This is recommended because after setting the shortcut, there is the
  // "temptation" to press the ENTER button. If focus is set on a button, it may
  // lead to a unexpected behaviour
  okButton->setFocusPolicy(Qt::NoFocus);
  removeButton->setFocusPolicy(Qt::NoFocus);
  defaultButton->setFocusPolicy(Qt::NoFocus);
  resetAllButton->setFocusPolicy(Qt::NoFocus);
  importButton->setFocusPolicy(Qt::NoFocus);
  exportButton->setFocusPolicy(Qt::NoFocus);

  // Set dialog size
  setMinimumSize(700, 500);
  resize(700, 500);

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

  // Get all categories from manager
  QList<QString> categories = m_manager.allCategories();

  // Sort alphabetically
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

  // Get commands in this category
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

  // Highlight conflicts
  highlightConflicts();
}

// ----------------------------------------------------------------------------
// Highlight conflicting shortcuts visually
// The user cannot enter a duplicated shortcut, but this was introduced on an
// early stage and it's useful to find hardcoded shortcut conflicts
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
      for (int row : qAsConst(it.value())) {
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

    QList<ShortcutConflict> conflicts = m_manager.findConflicts(key);

    if (conflicts.size() > 1) {
      // Build tooltip with all conflicts
      QString tooltip = tr("⚠ This shortcut conflicts with:\n");
      for (const auto &conflict : qAsConst(conflicts)) {
        if (conflict.commandId != currentId) {
          tooltip += QString("  • %1: %2\n")
                         .arg(conflict.category, conflict.description);
        }
      }

      QTableWidgetItem *item = actionList->item(row, 1);
      if (item->background() != QColor(255, 200, 200)) {
        item->setBackground(QColor(255, 230, 200));
      }
      item->setToolTip(tooltip);
    }
  }
}

// ----------------------------------------------------------------------------
// Remove shortcut from selected action
// ----------------------------------------------------------------------------
void QucsShortcutDialog::slotRemoveShortcut() {
  int row = actionList->currentRow();
  if (row < 0) {
    QMessageBox::information(this, tr("Info"),
                             tr("Please select an action first!"));
    return;
  }

  QString commandId = actionList->item(row, 0)->data(Qt::UserRole).toString();

  // Remove shortcut
  m_manager.removeShortcut(commandId);

  actionList->item(row, 1)->setText("");
  messageLabel->setText(tr("✓ Shortcut removed"));
  messageLabel->setStyleSheet("QLabel { color: green; }");

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

  // Load from file
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
// When selection changes
// ----------------------------------------------------------------------------
void QucsShortcutDialog::slotItemSelectionChanged() {
  // Clear message when selection changes
  if (!m_capturingKey) {
    messageLabel->clear();
    messageLabel->setStyleSheet("");
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

    bool matches =
        action.contains(searchLower) || shortcut.contains(searchLower);

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

          bool matches =
              action.contains(searchLower) || shortcut.contains(searchLower);
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
    // Hide all rows
    for (int row = 0; row < actionList->rowCount(); ++row) {
      actionList->setRowHidden(row, true);
    }
  }
}

// ----------------------------------------------------------------------------
// Handle double-click on table cell
// ----------------------------------------------------------------------------
void QucsShortcutDialog::slotCellDoubleClicked(int row, int column) {
  // Only capture keys for the shortcut column
  if (column != 1) {
    return;
  }

  m_capturingKey = true;
  actionList->setCurrentCell(row, column);

  // Visual feedback
  QTableWidgetItem *item = actionList->item(row, column);
  QString originalText = item->text();
  item->setText(tr("⌨ Press key combination..."));
  item->setBackground(QColor(255, 255, 200)); // Light yellow

  // Store original text temporarily
  item->setData(Qt::UserRole + 1, originalText);

  messageLabel->setText(
      tr("ℹ️ Press your desired key combination (or ESC to cancel)"));
  messageLabel->setStyleSheet("QLabel { color: blue; font-weight: bold; }");

  // Set focus to the dialog so it catches key events
  setFocus();
}

// ----------------------------------------------------------------------------
// Override keyPressEvent to capture shortcuts directly in dialog
// ----------------------------------------------------------------------------
void QucsShortcutDialog::keyPressEvent(QKeyEvent *event) {
  if (!m_capturingKey) {
    QDialog::keyPressEvent(event);
    return;
  }

  int row = actionList->currentRow();
  if (row < 0) {
    m_capturingKey = false;
    return;
  }

  int keyInt = event->key();
  Qt::Key key = static_cast<Qt::Key>(keyInt);

  // Handle ESC to cancel
  if (key == Qt::Key_Escape) {
    QTableWidgetItem *item = actionList->item(row, 1);
    QString originalText = item->data(Qt::UserRole + 1).toString();
    item->setText(originalText);
    item->setBackground(Qt::white);
    m_capturingKey = false;
    messageLabel->setText(tr("❌ Cancelled"));
    messageLabel->setStyleSheet("QLabel { color: gray; }");
    QTimer::singleShot(2000, this, [this]() {
      messageLabel->clear();
      messageLabel->setStyleSheet("");
    });
    return;
  }

  // Ignore
  if (key == Qt::Key_Control || key == Qt::Key_Shift || key == Qt::Key_Alt ||
      key == Qt::Key_Meta) {
    return;
  }

  // Build the key combination
  Qt::KeyboardModifiers modifiers = event->modifiers();
  if (modifiers & Qt::ShiftModifier)
    keyInt += Qt::SHIFT;
  if (modifiers & Qt::ControlModifier)
    keyInt += Qt::CTRL;
  if (modifiers & Qt::AltModifier)
    keyInt += Qt::ALT;
  if (modifiers & Qt::MetaModifier)
    keyInt += Qt::META;

  QKeySequence newKey(keyInt);
  QString commandId = actionList->item(row, 0)->data(Qt::UserRole).toString();

  // Validate the shortcut
  QString errorMsg;
  if (!m_manager.isValidShortcut(newKey, errorMsg)) {
    QMessageBox::warning(this, tr("Invalid Shortcut"), errorMsg);
    QTableWidgetItem *item = actionList->item(row, 1);
    QString originalText = item->data(Qt::UserRole + 1).toString();
    item->setText(originalText);
    item->setBackground(Qt::white);
    m_capturingKey = false;
    messageLabel->setText(tr("❌ Invalid shortcut"));
    messageLabel->setStyleSheet("QLabel { color: red; }");
    return;
  }

  // Check for conflicts
  QList<ShortcutConflict> conflicts = m_manager.findConflicts(newKey);
  if (!conflicts.isEmpty()) {
    QString msg = tr("This shortcut is already used by:\n\n");
    for (const auto &conflict : qAsConst(conflicts)) {
      msg +=
          QString("  • %1: %2\n").arg(conflict.category, conflict.description);
    }
    msg += tr("\nPlease assign other shortcut");

    QMessageBox msgBox(QMessageBox::Question, tr("Conflict Detected"), msg,
                       QMessageBox::Ok, this);
    msgBox.exec();
    int ret = msgBox.result();

    if (ret == QMessageBox::Ok) {
      QTableWidgetItem *item = actionList->item(row, 1);
      QString originalText = item->data(Qt::UserRole + 1).toString();
      item->setText(originalText);
      item->setBackground(Qt::white);
      m_capturingKey = false;
      messageLabel->setText(tr("❌ Cancelled"));
      messageLabel->setStyleSheet("QLabel { color: gray; }");
      return;
    }
  }

  // Apply the shortcut
  if (m_manager.setShortcut(commandId, newKey)) {
    QTableWidgetItem *item = actionList->item(row, 1);
    item->setText(newKey.toString());
    item->setBackground(Qt::white);

    // Mark as modified
    QucsCommand *cmd = m_manager.command(commandId);
    if (cmd && cmd->isModified()) {
      QFont font = item->font();
      font.setBold(true);
      item->setFont(font);
      item->setToolTip(tr("Modified from default: %1")
                           .arg(cmd->defaultKeySequence().toString()));
    }

    messageLabel->setText(tr("✓ Shortcut set to: %1").arg(newKey.toString()));
    messageLabel->setStyleSheet("QLabel { color: green; font-weight: bold; }");

    highlightConflicts();
  } else {
    QMessageBox::warning(this, tr("Error"), tr("Failed to set shortcut!"));
    QTableWidgetItem *item = actionList->item(row, 1);
    QString originalText = item->data(Qt::UserRole + 1).toString();
    item->setText(originalText);
    item->setBackground(Qt::white);
    messageLabel->setText(tr("❌ Failed to set shortcut"));
    messageLabel->setStyleSheet("QLabel { color: red; }");
  }

  m_capturingKey = false;
}

void QucsShortcutDialog::slotSetShortcut() {
  int row = actionList->currentRow();
  if (row < 0) {
    QMessageBox::information(
        this, tr("Info"),
        tr("Please select an action first to set a shortcut!"));
    return;
  }

  // Since the double-click handles the key capturing, this function
  // would logically trigger the same mechanism for the selected row.
  // We call the double-click logic for the shortcut column (column 1).
  slotCellDoubleClicked(row, 1);
}
