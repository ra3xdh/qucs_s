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

/**
 * @file qucsshortcutmanager.h
 * @brief Keyboard shortcut management system for Qucs-S
 * @author Andrés Martínez Mera
 * @date 2025
 */

#ifndef QUCSSHORTCUTMANAGER_H
#define QUCSSHORTCUTMANAGER_H

#include <QAction>
#include <QHash>
#include <QKeySequence>
#include <QList>
#include <QMultiHash>
#include <QObject>
#include <QString>
#include <memory>

// Forward declarations
class QAction;
/**
 * @class QucsCommand
 * @brief Represents a single command/action with its shortcut
 *
 * This class encapsulates a command with its associated QAction,
 * default and current key sequences, and metadata. Each command
 * has a unique ID, belongs to a category, and maintains both
 * the factory default and user-modified shortcut.
 */
class QucsCommand {
public:
  /**
   * @brief QucsCommand constructor
   * @param id: Unique identifier for the command (Format: <catgegory>.<action>
   * e.g., "File.Open")
   * @param category: Category of the command (e.g., "File")
   * @param description: Description of the command
   * @param action: Associated QAction that will receive the shortcut
   * @param defaultKey: Factory default key sequence
   */
  QucsCommand(const QString &id, const QString &category,
              const QString &description, QAction *action,
              const QKeySequence &defaultKey = QKeySequence());

  // Getters
  /**
   * @brief Gets the unique command identifier
   * @return The command ID string
   */
  QString id() const { return m_id; }

  /**
   * @brief Gets the command category
   * @return The category string
   */
  QString category() const { return m_category; }

  /**
   * @brief Gets the command description
   * @return Command description (text for the user)
   */
  QString description() const { return m_description; }

  /**
   * @brief Gets the associated QAction
   * @return Pointer to the QAction, or nullptr if none
   */
  QAction *action() const { return m_action; }

  /**
   * @brief Gets the default key sequence
   * @return The default QKeySequence
   */
  QKeySequence defaultKeySequence() const { return m_defaultKey; }

  /**
   * @brief Gets the current (possibly user-modified) key sequence
   * @return The current QKeySequence
   */
  QKeySequence currentKeySequence() const { return m_currentKey; }

  // Setters

  /**
   * @brief Sets a new key sequence for the command
   * @param key: The new key sequence to assign
   *
   * This method updates both the internal state and the associated
   * QAction's shortcut property.
   */
  void setCurrentKeySequence(const QKeySequence &key);

  /**
   * @brief Resets the shortcut to its default
   */
  void resetToDefault();

  /**
   * @brief Checks if the current shortcut differs from the default
   * @return true if the shortcut has been modified, false otherwise
   */
  bool isModified() const { return m_currentKey != m_defaultKey; }

private:
  QString m_id;              /// Unique identifier (e.g., "File.Open")
  QString m_category;        /// Category for grouping (e.g., "File")
  QString m_description;     /// User-visible description
  QAction *m_action;         /// Associated QAction
  QKeySequence m_defaultKey; /// Factory default
  QKeySequence m_currentKey; /// User-defined (or default)
};

/**
 * @struct ShortcutConflict
 * @brief Information about a shortcut conflict
 *
 * This structure contains details about a command that uses
 * a particular key sequence, used when detecting conflicts.
 */
struct ShortcutConflict {
  QString commandId;        /// Unique command identifier
  QString category;         /// Command category
  QString description;      /// Description
  QKeySequence keySequence; /// The conflicting key sequence
};

/**
 * @class QucsShortcutManager
 * @brief Central registry for all shortcuts with hash-based indexing
 *
 * This singleton class provides fast lookups using Qt's hash containers. It
 * manages all keyboard shortcuts in the application, handles conflicts,
 * validates shortcuts, and provides persistence through files and QSettings.
 *
 * The manager maintains three indices:
 * - Command ID to command mapping
 * - Category to command list mapping
 * - Key sequence to command ID mapping (or conflict detection
 *
 */
class QucsShortcutManager : public QObject {
  Q_OBJECT

public:
  /**
   * @brief Gets the singleton instance
   * @return Reference to the global QucsShortcutManager instance
   */
  static QucsShortcutManager &instance();

  /**
   * @brief Deleted copy constructor (singleton pattern)
   */
  QucsShortcutManager(const QucsShortcutManager &) = delete;

  /**
   * @brief Deleted assignment operator (singleton pattern)
   */
  QucsShortcutManager &operator=(const QucsShortcutManager &) = delete;

  /**
   * @brief Registers a new command with the manager
   * @param id Unique identifier for the command
   * @param category Category for grouping
   * @param description User-visible description
   * @param action Associated QAction
   * @param defaultKey Factory default key sequence
   *
   * If a command with the same ID already exists, a warning is logged
   * and the registration is ignored.
   */
  void registerCommand(const QString &id, const QString &category,
                       const QString &description, QAction *action,
                       const QKeySequence &defaultKey = QKeySequence());

  /**
   * @brief Looks up a command by ID
   * @param id The command identifier
   * @return Pointer to the command, or nullptr if not found
   */
  QucsCommand *command(const QString &id) const;

  /**
   * @brief Gets all commands in a specific category
   * @param category: The category name
   * @return List of commands in that category
   */
  QList<QucsCommand *> commandsInCategory(const QString &category) const;

  /**
   * @brief Gets all registered categories
   * @return List of category names
   */
  QList<QString> allCategories() const;

  /**
   * @brief Gets all registered commands
   * @return List of all commands
   */
  QList<QucsCommand *> allCommands() const;

  /**
   * @brief Sets a new shortcut for a command
   * @param commandId: Command identifier
   * @param key: New key sequence
   * @return true if successful, false if command not found or key invalid
   *
   * This method validates the shortcut and updates all internal indices.
   * The associated QAction is automatically updated.
   */
  bool setShortcut(const QString &commandId, const QKeySequence &key);

  /**
   * @brief Removes the shortcut from a command
   * @param commandId The command identifier
   *
   * Equivalent to calling setShortcut() with an empty QKeySequence.
   */
  void removeShortcut(const QString &commandId);

  /**
   * @brief Resets all shortcuts to their factory defaults
   */
  void resetToDefaults();

  /**
   * @brief Resets all shortcuts in a category to defaults
   * @param category The category to reset
   */
  void resetCategory(const QString &category);

  /**
   * @brief Finds all commands that use a specific key sequence
   * @param key The key sequence to search for
   * @return List of conflicts (all commands using this key)
   * @note Time complexity: O(k) where k is the number of conflicts
   */
  QList<ShortcutConflict> findConflicts(const QKeySequence &key) const;

  /**
   * @brief Checks if a key sequence has any conflicts
   * @param key The key sequence to check
   * @param excludeCommandId Optional command ID to exclude from conflict check
   * @return true if there are conflicts (excluding the specified command)
   * @note Time complexity: O(k) where k is the number of commands using this
   * key
   */
  bool hasConflict(const QKeySequence &key,
                   const QString &excludeCommandId = QString()) const;

  // Validation
  /**
   * @brief Validates a key sequence
   * @param key: The key sequence to validate
   * @param errorMsg: Output parameter for error message
   * @return true if the key is valid and not reserved
   *
   * Checks if the key sequence is not empty, not reserved by the system,
   * and is a valid Qt key sequence.
   */
  bool isValidShortcut(const QKeySequence &key, QString &errorMsg) const;

  /**
   * @brief Gets the list of reserved shortcuts
   * @return List of reserved key sequence strings
   *
   * Reserved shortcuts are platform-specific and cannot be assigned
   * to commands (e.g., Cmd+Q on macOS, Alt+F4 on Windows).
   */
  QStringList reservedShortcuts() const { return m_reservedKeys; }

  // Load and save settings

  /**
   * @brief Saves all shortcuts to a JSON file
   * @param filename Path to the output file
   * @return true if successful, false on error
   *
   * The file format includes version information and is human-readable.
   */
  bool saveToFile(const QString &filename) const;

  /**
   * @brief Loads shortcuts from a JSON file
   * @param filename Path to the input file
   * @return true if successful, false on error
   *
   * Validates the file format and version before loading.
   */
  bool loadFromFile(const QString &filename);

  /**
   * @brief Saves modified shortcuts to QSettings
   *
   * Only shortcuts that differ from defaults are saved.
   */
  void saveToSettings() const;

  /**
   * @brief Loads shortcuts from QSettings
   *
   * Applies any previously saved shortcut modifications.
   */
  void loadFromSettings();

signals:
  /**
   * @brief Emitted when a shortcut is changed
   * @param commandId The command whose shortcut changed
   * @param newKey The new key sequence
   */
  void shortcutChanged(const QString &commandId, const QKeySequence &newKey);

  /**
   * @brief Emitted when a new command is registered
   * @param commandId The ID of the newly registered command
   */
  void commandRegistered(const QString &commandId);

private:
  /**
   * @brief Private constructor (singleton pattern)
   *
   * Initializes reserved shortcuts based on the platform.
   */
  QucsShortcutManager();

  /**
   * @brief Destructor
   */
  ~QucsShortcutManager();

  /**
   * @brief Updates the shortcut index when a key changes
   * @param commandId The command being updated
   * @param oldKey The previous key sequence
   * @param newKey The new key sequence
   *
   * Removes the old key from the index and inserts the new one.
   */
  void updateShortcutIndex(const QString &commandId, const QKeySequence &oldKey,
                           const QKeySequence &newKey);

  // Hash tables for lookups
  QHash<QString, std::shared_ptr<QucsCommand>> m_commands; /// ID -> Command
  QHash<QString, QList<QString>> m_categoryIndex;          /// Category -> [IDs]
  QMultiHash<QKeySequence, QString> m_shortcutIndex; /// KeySequence -> [IDs]

  QStringList
      m_reservedKeys; /// Reserved shortcuts that can't be used (Esc, Ret, etc.)
};

#endif
