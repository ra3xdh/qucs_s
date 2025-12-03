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
 
#include "qucsshortcutmanager.h"
#include <QSettings>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

// ----------------------------------------------------------------------------
// QucsCommand Implementation
// ----------------------------------------------------------------------------

QucsCommand::QucsCommand(const QString &id,
                         const QString &category,
                         const QString &description,
                         QAction *action,
                         const QKeySequence &defaultKey)
    : m_id(id)
    , m_category(category)
    , m_description(description)
    , m_action(action)
    , m_defaultKey(defaultKey)
    , m_currentKey(defaultKey)
{
    // Apply the shortcut to the action
    if (m_action) {
        m_action->setShortcut(m_currentKey);
    }
}

void QucsCommand::setCurrentKeySequence(const QKeySequence &key)
{
    m_currentKey = key;
    
    // Update the associated QAction
    if (m_action) {
        m_action->setShortcut(m_currentKey);
    }
}

void QucsCommand::resetToDefault()
{
    setCurrentKeySequence(m_defaultKey);
}

// ----------------------------------------------------------------------------
// QucsShortcutManager Implementation
// ----------------------------------------------------------------------------

QucsShortcutManager::QucsShortcutManager()
{
    // Initialize reserved shortcuts (platform-specific)
    m_reservedKeys << "Esc" << "Tab" << "Return" << "Enter";
    
#ifdef Q_OS_MAC
    m_reservedKeys << "Meta+Q" << "Meta+H" << "Meta+M";
#endif
}

QucsShortcutManager::~QucsShortcutManager()
{
    // Smart pointers handle cleanup automatically
}

QucsShortcutManager& QucsShortcutManager::instance()
{
    static QucsShortcutManager instance;
    return instance;
}

void QucsShortcutManager::registerCommand(const QString &id,
                                          const QString &category,
                                          const QString &description,
                                          QAction *action,
                                          const QKeySequence &defaultKey)
{
    // Check if command already exists
    if (m_commands.contains(id)) {
        qWarning() << "Command already registered:" << id;
        return;
    }
    
    // Create the command
    auto command = std::make_shared<QucsCommand>(id, category, description, 
                                                  action, defaultKey);
    
    // Add to main registry - O(1)
    m_commands.insert(id, command);
    
    // Update category index - O(1) average
    m_categoryIndex[category].append(id);
    
    // Update shortcut index if there's a default key - O(1) average
    if (!defaultKey.isEmpty()) {
        m_shortcutIndex.insert(defaultKey, id);
    }
    
    emit commandRegistered(id);
    
    qDebug() << "Registered command:" << id 
             << "Category:" << category 
             << "Shortcut:" << defaultKey.toString();
}

QucsCommand* QucsShortcutManager::command(const QString &id) const
{
    // O(1) lookup
    auto it = m_commands.find(id);
    if (it != m_commands.end()) {
        return it.value().get();
    }
    return nullptr;
}

QList<QucsCommand*> QucsShortcutManager::commandsInCategory(const QString &category) const
{
    QList<QucsCommand*> result;
    
    // O(1) category lookup
    auto it = m_categoryIndex.find(category);
    if (it != m_categoryIndex.end()) {
        // O(k) where k is commands in category
        for (const QString &id : it.value()) {
            if (QucsCommand *cmd = command(id)) {
                result.append(cmd);
            }
        }
    }
    
    return result;
}

QList<QString> QucsShortcutManager::allCategories() const
{
    // O(c) where c is number of categories
    return m_categoryIndex.keys();
}

QList<QucsCommand*> QucsShortcutManager::allCommands() const
{
    QList<QucsCommand*> result;
    result.reserve(m_commands.size());
    
    // O(n) where n is total commands
    for (const auto &cmd : m_commands) {
        result.append(cmd.get());
    }
    
    return result;
}

bool QucsShortcutManager::setShortcut(const QString &commandId, 
                                      const QKeySequence &key)
{
    QucsCommand *cmd = command(commandId);
    if (!cmd) {
        qWarning() << "Command not found:" << commandId;
        return false;
    }
    
    // Validate the shortcut
    QString errorMsg;
    if (!key.isEmpty() && !isValidShortcut(key, errorMsg)) {
        qWarning() << "Invalid shortcut:" << key.toString() << "-" << errorMsg;
        return false;
    }
    
    // Get old key for index update
    QKeySequence oldKey = cmd->currentKeySequence();
    
    // Update the command
    cmd->setCurrentKeySequence(key);
    
    // Update the shortcut index
    updateShortcutIndex(commandId, oldKey, key);
    
    emit shortcutChanged(commandId, key);
    
    qDebug() << "Shortcut changed:" << commandId 
             << "from" << oldKey.toString() 
             << "to" << key.toString();
    
    return true;
}

void QucsShortcutManager::removeShortcut(const QString &commandId)
{
    setShortcut(commandId, QKeySequence());
}

void QucsShortcutManager::resetToDefaults()
{
    // O(n) where n is total commands
    for (const auto &cmd : qAsConst(m_commands)) {
        QKeySequence oldKey = cmd->currentKeySequence();
        cmd->resetToDefault();
        QKeySequence newKey = cmd->currentKeySequence();
        
        updateShortcutIndex(cmd->id(), oldKey, newKey);
        emit shortcutChanged(cmd->id(), newKey);
    }
    
    qDebug() << "All shortcuts reset to defaults";
}

void QucsShortcutManager::resetCategory(const QString &category)
{
    QList<QucsCommand*> commands = commandsInCategory(category);
    
    for (QucsCommand *cmd : qAsConst(commands)) {
        QKeySequence oldKey = cmd->currentKeySequence();
        cmd->resetToDefault();
        QKeySequence newKey = cmd->currentKeySequence();
        
        updateShortcutIndex(cmd->id(), oldKey, newKey);
        emit shortcutChanged(cmd->id(), newKey);
    }
    
    qDebug() << "Category shortcuts reset:" << category;
}

QList<ShortcutConflict> QucsShortcutManager::findConflicts(const QKeySequence &key) const
{
    QList<ShortcutConflict> conflicts;
    
    if (key.isEmpty()) {
        return conflicts;
    }
    
    // O(k) where k is number of commands with this key (usually 0 or 1)
    QList<QString> commandIds = m_shortcutIndex.values(key);
    
    for (const QString &id : qAsConst(commandIds)) {
        if (QucsCommand *cmd = command(id)) {
            ShortcutConflict conflict;
            conflict.commandId = cmd->id();
            conflict.category = cmd->category();
            conflict.description = cmd->description();
            conflict.keySequence = key;
            conflicts.append(conflict);
        }
    }
    
    return conflicts;
}

bool QucsShortcutManager::hasConflict(const QKeySequence &key, 
                                      const QString &excludeCommandId) const
{
    if (key.isEmpty()) {
        return false;
    }
    
    // O(k) lookup
    QList<QString> commandIds = m_shortcutIndex.values(key);
    
    for (const QString &id : qAsConst(commandIds)) {
        if (id != excludeCommandId) {
            return true;
        }
    }
    
    return false;
}

bool QucsShortcutManager::isValidShortcut(const QKeySequence &key, 
                                          QString &errorMsg) const
{
    if (key.isEmpty()) {
        return true; // Empty is valid (means no shortcut)
    }
    
    QString keyString = key.toString();
    
    // Check against reserved keys
    for (const QString &reserved : m_reservedKeys) {
        if (keyString == reserved) {
            errorMsg = QObject::tr("Shortcut '%1' is reserved and cannot be used")
                      .arg(reserved);
            return false;
        }
    }
    
    // Check if it's a valid key sequence
    if (key.isEmpty()) {
        errorMsg = QObject::tr("Invalid key sequence");
        return false;
    }
    
    return true;
}

void QucsShortcutManager::updateShortcutIndex(const QString &commandId,
                                              const QKeySequence &oldKey,
                                              const QKeySequence &newKey)
{
    // Remove old key from index - O(k) where k is commands with this key
    if (!oldKey.isEmpty()) {
        m_shortcutIndex.remove(oldKey, commandId);
    }
    
    // Add new key to index - O(1) average
    if (!newKey.isEmpty()) {
        m_shortcutIndex.insert(newKey, commandId);
    }
}

bool QucsShortcutManager::saveToFile(const QString &filename) const
{
    QJsonObject root;
    root["version"] = "1.0";
    root["application"] = "Qucs-S";
    
    QJsonObject shortcuts;
    
    // Group by category
    for (const QString &category : allCategories()) {
        QJsonObject categoryObj;
        
        for (QucsCommand *cmd : commandsInCategory(category)) {
            QJsonObject commandObj;
            commandObj["default"] = cmd->defaultKeySequence().toString();
            commandObj["current"] = cmd->currentKeySequence().toString();
            commandObj["description"] = cmd->description();
            commandObj["modified"] = cmd->isModified();
            
            // Use description as key (more readable than ID)
            categoryObj[cmd->description()] = commandObj;
        }
        
        shortcuts[category] = categoryObj;
    }
    
    root["shortcuts"] = shortcuts;
    
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file for writing:" << filename;
        return false;
    }
    
    QJsonDocument doc(root);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    qDebug() << "Shortcuts saved to:" << filename;
    return true;
}

bool QucsShortcutManager::loadFromFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file for reading:" << filename;
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qWarning() << "Invalid JSON format in file:" << filename;
        return false;
    }
    
    QJsonObject root = doc.object();
    
    // Validate version
    if (root["application"].toString() != "Qucs-S") {
        qWarning() << "Not a Qucs-S shortcut file";
        return false;
    }
    
    QJsonObject shortcuts = root["shortcuts"].toObject();
    
    // Load shortcuts
    for (const QString &category : shortcuts.keys()) {
        QJsonObject categoryObj = shortcuts[category].toObject();
        
        for (const QString &description : categoryObj.keys()) {
            QJsonObject commandObj = categoryObj[description].toObject();
            QString currentKey = commandObj["current"].toString();
            
            // Find command by description (could optimize with reverse index)
            for (QucsCommand *cmd : commandsInCategory(category)) {
                if (cmd->description() == description) {
                    setShortcut(cmd->id(), QKeySequence(currentKey));
                    break;
                }
            }
        }
    }
    
    qDebug() << "Shortcuts loaded from:" << filename;
    return true;
}

void QucsShortcutManager::saveToSettings() const
{
    QSettings settings("qucs", "qucs_s");
    settings.beginGroup("Shortcuts");
    
    for (const auto &cmd : m_commands) {
        if (cmd->isModified()) {
            settings.setValue(cmd->id(), cmd->currentKeySequence().toString());
        }
    }
    
    settings.endGroup();
}

void QucsShortcutManager::loadFromSettings()
{
    QSettings settings("qucs", "qucs_s");
    settings.beginGroup("Shortcuts");
    
    QStringList keys = settings.allKeys();
    for (const QString &id : keys) {
        QString keyString = settings.value(id).toString();
        setShortcut(id, QKeySequence(keyString));
    }
    
    settings.endGroup();
}

