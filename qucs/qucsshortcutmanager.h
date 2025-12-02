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
 
#ifndef QUCSSHORTCUTMANAGER_H
#define QUCSSHORTCUTMANAGER_H

#include <QHash>
#include <QKeySequence>
#include <QList>
#include <QMultiHash>
#include <QObject>
#include <QAction>
#include <QString>
#include <memory>

// Forward declarations
class QAction;

/**
 * @brief Represents a single command/action with its shortcuts
 * 
 * This class encapsulates a command with its associated QAction,
 * default and current key sequences, and metadata.
 */
class QucsCommand {
public:
    QucsCommand(const QString &id, 
                const QString &category,
                const QString &description,
                QAction *action,
                const QKeySequence &defaultKey = QKeySequence());
    
    // Getters
    QString id() const { return m_id; }
    QString category() const { return m_category; }
    QString description() const { return m_description; }
    QAction* action() const { return m_action; }
    QKeySequence defaultKeySequence() const { return m_defaultKey; }
    QKeySequence currentKeySequence() const { return m_currentKey; }
    
    // Setters
    void setCurrentKeySequence(const QKeySequence &key);
    void resetToDefault();
    bool isModified() const { return m_currentKey != m_defaultKey; }
    
private:
    QString m_id;              // Unique identifier (e.g., "File.Open")
    QString m_category;        // Category for grouping (e.g., "File")
    QString m_description;     // User-visible description
    QAction *m_action;         // Associated QAction
    QKeySequence m_defaultKey; // Factory default
    QKeySequence m_currentKey; // User-defined (or default)
};

/**
 * @brief Conflict information for a shortcut
 */
struct ShortcutConflict {
    QString commandId;
    QString category;
    QString description;
    QKeySequence keySequence;
};

/**
 * @brief Central registry for all shortcuts with hash-based indexing
 * 
 * This class provides O(1) lookups and fast conflict detection using
 * Qt's hash containers. It follows the singleton pattern.
 */
class QucsShortcutManager : public QObject {
    Q_OBJECT

public:
    // Singleton access
    static QucsShortcutManager& instance();
    
    // Delete copy constructor and assignment
    QucsShortcutManager(const QucsShortcutManager&) = delete;
    QucsShortcutManager& operator=(const QucsShortcutManager&) = delete;
    
    // Command registration
    void registerCommand(const QString &id,
                        const QString &category,
                        const QString &description,
                        QAction *action,
                        const QKeySequence &defaultKey = QKeySequence());
    
    // Lookups - O(1) complexity
    QucsCommand* command(const QString &id) const;
    QList<QucsCommand*> commandsInCategory(const QString &category) const;
    QList<QString> allCategories() const;
    QList<QucsCommand*> allCommands() const;
    
    // Shortcut management
    bool setShortcut(const QString &commandId, const QKeySequence &key);
    void removeShortcut(const QString &commandId);
    void resetToDefaults();
    void resetCategory(const QString &category);
    
    // Conflict detection - O(1) for single key, O(k) for finding all conflicts
    QList<ShortcutConflict> findConflicts(const QKeySequence &key) const;
    bool hasConflict(const QKeySequence &key, const QString &excludeCommandId = QString()) const;
    
    // Validation
    bool isValidShortcut(const QKeySequence &key, QString &errorMsg) const;
    QStringList reservedShortcuts() const { return m_reservedKeys; }
    
    // Persistence
    bool saveToFile(const QString &filename) const;
    bool loadFromFile(const QString &filename);
    void saveToSettings() const;
    void loadFromSettings();
    
signals:
    void shortcutChanged(const QString &commandId, const QKeySequence &newKey);
    void commandRegistered(const QString &commandId);
    
private:
    QucsShortcutManager();
    ~QucsShortcutManager();
    
    void updateShortcutIndex(const QString &commandId, 
                            const QKeySequence &oldKey,
                            const QKeySequence &newKey);
    
    // Hash tables for O(1) lookups
    QHash<QString, std::shared_ptr<QucsCommand>> m_commands;      // ID -> Command
    QHash<QString, QList<QString>> m_categoryIndex;               // Category -> [IDs]
    QMultiHash<QKeySequence, QString> m_shortcutIndex;            // KeySequence -> [IDs]
    
    // Reserved shortcuts that can't be used
    QStringList m_reservedKeys;
};

#endif
