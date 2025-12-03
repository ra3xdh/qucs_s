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

#ifndef QUCSSHORTCUTDIALOG_H_
#define QUCSSHORTCUTDIALOG_H_

#include "main.h"
#include "qucs.h"
#include "qucsshortcutmanager.h"
#include <QDialog>

class QListWidget;
class QTableWidget;
class QLabel;
class QPushButton;
class QLineEdit;
class QKeyEvent;

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
    void slotItemSelectionChanged();
    void slotSearchShortcuts(const QString &text);
    void slotCellDoubleClicked(int row, int column);

protected:
    void keyPressEvent(QKeyEvent *event) override;

signals:
    void signalValidKey();

private:
    QucsApp *App;
    QucsShortcutManager &m_manager;
    bool m_capturingKey;

    void fillMenu();
    void setShortcut(const QKeySequence &key);
    void highlightConflicts();
    void updateShortcutDisplay();

           // UI Components
    QListWidget *menuList;
    QTableWidget *actionList;
    QLabel *messageLabel;
    QLineEdit *searchBox;
    QPushButton *setButton, *removeButton, *defaultButton, *resetAllButton,
        *okButton, *importButton, *exportButton;
};

#endif
