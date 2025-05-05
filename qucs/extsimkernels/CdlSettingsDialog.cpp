/*
 * CdlSettingsDialog.cpp - Settings for the CDL netlisting
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "CdlSettingsDialog.h"
#include "main.h"
#include "settings.h"

CdlSettingsDialog::CdlSettingsDialog(QWidget *parent) :
    QDialog(parent),
    a_btnOK(new QPushButton(tr("Apply changes"))),
    a_btnCancel(new QPushButton(tr("Cancel"))),
    a_chkResolveSpicePrefix(new QCheckBox(tr("Resolve spice prefix")))
{
    a_chkResolveSpicePrefix->setChecked(_settings::Get().item<bool>("ResolveSpicePrefix"));

    connect(a_btnOK,SIGNAL(clicked()),this,SLOT(slotApply()));
    connect(a_btnCancel,SIGNAL(clicked()),this,SLOT(reject()));

    QVBoxLayout *top = new QVBoxLayout;

    QGroupBox *gbp = new QGroupBox(this);
    gbp->setTitle(tr("CDL netlist settings"));
    QVBoxLayout *top2 = new QVBoxLayout;
    QHBoxLayout *h1 = new QHBoxLayout;
    h1->addWidget(a_chkResolveSpicePrefix);
    top2->addLayout(h1);

    gbp->setLayout(top2);
    top->addWidget(gbp);

    QHBoxLayout *h2 = new QHBoxLayout;
    h2->addWidget(a_btnOK);
    h2->addWidget(a_btnCancel);
    h2->addStretch(2);
    top->addLayout(h2);

    this->setLayout(top);
    this->setFixedWidth(500);
    this->setWindowTitle(tr("CDL Settings"));
}

void CdlSettingsDialog::slotApply()
{
    QucsSettings.ResolveSpicePrefix = a_chkResolveSpicePrefix->isChecked();
    settingsManager& qs = _settings::Get();
    qs.setItem<bool>("ResolveSpicePrefix", a_chkResolveSpicePrefix->isChecked());
    accept();
    saveApplSettings();
}

void CdlSettingsDialog::slotCancel()
{
    reject();
}

