/*
 * CdlSettingsDialog.h - Settings for the CDL netlisting
 *
 * This file is part of Qucs-s
 *
 * Qucs is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Qucs.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef CDL_SETTINGS_DIALOG_H
#define CDL_SETTINGS_DIALOG_H

#include <QtGui>
#include <QtWidgets>

class CdlSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CdlSettingsDialog(QWidget *parent = 0);

private:
    QPushButton *a_btnOK;
    QPushButton *a_btnCancel;

    QCheckBox *a_chkResolveSpicePrefix;

private slots:
    void slotApply();
    void slotCancel();
};

#endif // CDL_SETTINGS_DIALOG_H
