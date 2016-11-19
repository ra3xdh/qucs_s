/***************************************************************************
 * Copyright (C) 2014 Guilherme Brondani Torri <guitorri@gmail.com>        *
 *                                                                         *
 * Modified from SaveDialog and LibraryDialog                              *
 *                                                                         *
 * This is free software; you can redistribute it and/or modify            *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2, or (at your option)     *
 * any later version.                                                      *
 *                                                                         *
 * This software is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this package; see the file COPYING.  If not, write to        *
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,   *
 * Boston, MA 02110-1301, USA.                                             *
 ***************************************************************************/

#ifndef LOADDIALOG_H
#define LOADDIALOG_H

#include <QVariant>
#include <QDialog>
#include <QMap>
#include <QBoxLayout>
#include <QGridLayout>
#include <QBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QDir>

class QBoxLayout;
class QBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QucsDoc;
class QucsApp;

class LoadDialog : public QDialog
{
    Q_OBJECT
public:

    enum {
        AbortClosing = 0,
        DontSave,
        Accept
    };

    LoadDialog(QWidget* p = 0 );
    ~LoadDialog();
    void setApp(QucsApp *a);
    void initDialog();

    QStringList symbolFiles;
    QDir projDir;

    QMap<QString, QString> selectedComponents;

private slots:
    void slotSelectAll();
    void slotSelectNone();
    void slotSymbolFileClicked(QListWidgetItem *item);

protected slots:
    void reject();
    void loadSelected();
    void slotChangeIcon();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    QLabel *iconPixmap;
    QLabel* label;
    QListWidget* fileView;
    QBoxLayout* LoadDialogLayout;
    QHBoxLayout* buttonsLayout;
    QSpacerItem* spacer;
    QucsApp *app;

    QPushButton *ButtOk, *ButtCancel,
                *ButtSelectAll, *ButtSelectNone,
                *ButtChangeIcon;
};

#endif // LOADDIALOG_H
