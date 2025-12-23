/***************************************************************************
                             settingsdialog.cpp
                            --------------------
    begin                : Mon Oct 20 2003
    copyright            : (C) 2003, 2004 by Michael Margraf
                           (C) 2016 Qucs Team
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "settingsdialog.h"

#include "node.h"
#include "main.h"
#include "qucs.h"
#include "mnemo.h"
#include "schematic.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>
#include <QLayout>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QTabWidget>
#include <QValidator>
#include <QPushButton>
#include <QFileSystemModel>
#include <QTreeView>
#include <QHeaderView>
#include <QModelIndex>
#include <QDebug>

SettingsDialog::SettingsDialog(Schematic *Doc_)
    : QDialog(Doc_)
{
    Doc = Doc_;
    setWindowTitle(tr("Edit File Properties"));

    all = new QVBoxLayout(this);
    QTabWidget *t = new QTabWidget();
    all->addWidget(t);

    // ...........................................................
    QWidget *Tab1 = new QWidget(t);
    QGridLayout *gp = new QGridLayout(Tab1);
    Tab1->setLayout(gp);

    QLabel *l2 = new QLabel(tr("Data Set:"), Tab1);
    gp->addWidget(l2,0,0);
    Input_DataSet = new QLineEdit(Tab1);
    gp->addWidget(Input_DataSet,0,1,1,1);
    QPushButton *DataSetBrowseButt = new QPushButton(tr("Browse"));
    gp->addWidget(DataSetBrowseButt, 0, 2, 1, 1);
    connect(DataSetBrowseButt, SIGNAL(clicked()), SLOT(slotDataSetBrowse()));

    QLabel *l1 = new QLabel(tr("Data Display:"));
    gp->addWidget(l1,1,0);
    Input_DataDisplay = new QLineEdit(Tab1);
    gp->addWidget(Input_DataDisplay,1,1,1,1);
    QPushButton *DataDisplayBrowseButt = new QPushButton(tr("Browse"));
    gp->addWidget(DataDisplayBrowseButt, 1, 2, 1, 1);
    connect(DataDisplayBrowseButt, SIGNAL(clicked()), SLOT(slotDataDisplayBrowse()));
    Check_OpenDpl = new QCheckBox(tr("open data display after simulation"),
                                  Tab1);
    gp->addWidget(Check_OpenDpl,2,0,1,2);

    QLabel *l20 = new QLabel(tr("Octave Script:"), Tab1);
    gp->addWidget(l20,3,0);
    Input_Script = new QLineEdit(Tab1);
    gp->addWidget(Input_Script,3,1,1,1);
    QPushButton *ScriptBrowseButt = new QPushButton(tr("Browse"));
    gp->addWidget(ScriptBrowseButt, 3, 2, 1, 1);
    connect(ScriptBrowseButt, SIGNAL(clicked()), SLOT(slotScriptBrowse()));

    Check_RunScript = new QCheckBox(tr("run script after simulation"),
                                    Tab1);
    gp->addWidget(Check_RunScript,4,0,1,2);

    t->addTab(Tab1, tr("Simulation"));

    // ...........................................................
    QWidget *Tab2 = new QWidget(t);
    QGridLayout *gp2 = new QGridLayout(Tab2);
    Check_GridOn = new QCheckBox(tr("show Grid"), Tab2);
    gp2->addWidget(Check_GridOn,0,0,1,1);

    valExpr = new QRegularExpressionValidator(QRegularExpression("[1-9]\\d{0,2}"), this);

    QLabel *l3 = new QLabel(tr("horizontal Grid:"), Tab2);
    gp2->addWidget(l3,1,0);
    Input_GridX = new QLineEdit(Tab2);
    Input_GridX->setValidator(valExpr);
    gp2->addWidget(Input_GridX,1,1,1,1);

    QLabel *l4 = new QLabel(tr("vertical Grid:"), Tab2);
    gp2->addWidget(l4,2,0);
    Input_GridY = new QLineEdit(Tab2);
    Input_GridY->setValidator(valExpr);
    gp2->addWidget(Input_GridY,2,1,1,1);

    t->addTab(Tab2, tr("Grid"));

    // ...........................................................
    QWidget *Tab3 = new QWidget(t);
    QGridLayout *gp3 = new QGridLayout(Tab3);
    Combo_Frame = new QComboBox(Tab3);

    // Helper to add items with explicit frame code
    // A6 format was added later, so its index must be >8 to guarantee backward compatibility with older Qucs-S
    // versions.
    // It makes sense to put all the DIN A standard formats together, so a function is needed to
    // decouple the frame index from the frame combobox
    auto addFrameItem = [this](QComboBox* cb, const QString& text, int code) {
        cb->addItem(text);
        int idx = cb->count() - 1;
        cb->setItemData(idx, code, Qt::UserRole); // code == a_showFrame / <showFrame>
    };


    // Visual order: all DIN A paper formats, then Letter

    // No frame
    addFrameItem(Combo_Frame, tr("no Frame"),          0);

    // DIN A formats
    addFrameItem(Combo_Frame, tr("DIN A3 landscape"),  5);
    addFrameItem(Combo_Frame, tr("DIN A3 portrait"),   6);
    addFrameItem(Combo_Frame, tr("DIN A4 landscape"),  3);
    addFrameItem(Combo_Frame, tr("DIN A4 portrait"),   4);
    addFrameItem(Combo_Frame, tr("DIN A5 landscape"),  1);
    addFrameItem(Combo_Frame, tr("DIN A5 portrait"),   2);
    addFrameItem(Combo_Frame, tr("DIN A6 landscape"), 9);
    addFrameItem(Combo_Frame, tr("DIN A6 portrait"),  10);

    // US Letter format
    addFrameItem(Combo_Frame, tr("Letter landscape"),  7);
    addFrameItem(Combo_Frame, tr("Letter portrait"),   8);

    gp3->addWidget(Combo_Frame, 0, 0, 1, 2);


    Input_Frame0 = new QTextEdit(Tab3);
    Input_Frame0->setWordWrapMode(QTextOption::NoWrap);
    gp3->addWidget(Input_Frame0,1,0,2,2);

    Input_Frame1 = new QLineEdit(Tab3);
    gp3->addWidget(Input_Frame1,3,0,1,2);

    Input_Frame2 = new QLineEdit(Tab3);
    gp3->addWidget(Input_Frame2,4,0);
    Input_Frame3 = new QLineEdit(Tab3);
    gp3->addWidget(Input_Frame3,4,1);

    t->addTab(Tab3, tr("Frame"));

    // ...........................................................
    // buttons on the bottom of the dialog (independent of the TabWidget)
    QHBoxLayout *Butts = new QHBoxLayout();
    Butts->setSpacing(5);
    Butts->setContentsMargins(5,5,5,5);
    all->addLayout(Butts);

    QPushButton *OkButt = new QPushButton(tr("OK"));
    Butts->addWidget(OkButt);
    connect(OkButt, SIGNAL(clicked()), SLOT(slotOK()));
    QPushButton *ApplyButt = new QPushButton(tr("Apply"));
    Butts->addWidget(ApplyButt);
    connect(ApplyButt, SIGNAL(clicked()), SLOT(slotApply()));
    QPushButton *CancelButt = new QPushButton(tr("Cancel"));
    Butts->addWidget(CancelButt);
    connect(CancelButt, SIGNAL(clicked()), SLOT(reject()));

    OkButt->setDefault(true);

    // ...........................................................
    // fill the fields with the QucsDoc-Properties

    Input_DataSet->setText(Doc->getDataSet());
    Input_DataDisplay->setText(Doc->getDataDisplay());
    Input_Script->setText(Doc->getScript());
    Check_OpenDpl->setChecked(Doc->getSimOpenDpl());
    Check_RunScript->setChecked(Doc->getSimRunScript());
    Check_GridOn->setChecked(Doc->getGridOn());
    Input_GridX->setText(QString::number(Doc->getGridX()));
    Input_GridY->setText(QString::number(Doc->getGridY()));

    //////////////////////////////////////////////////////////////////////////
    // Select index in combo according to the document's frame code
    FrameSize docFrame = Doc->getShowFrame();  // <showFrame=...> value from document
    int idxToSelect = 0;                       // default: "no Frame" (index 0)

    for (int i = 0; i < Combo_Frame->count(); ++i) {
        FrameSize itemFrame = static_cast<FrameSize>(
            Combo_Frame->itemData(i, Qt::UserRole).toInt()
            );
        if (itemFrame == docFrame) {
            idxToSelect = i;
            break;
        }
    }
    Combo_Frame->setCurrentIndex(idxToSelect);

    //////////////////////////////////////////////////////////////////////////

    QString Text_;
    decode_String(Text_ = Doc->getFrame_Text0());
    Input_Frame0->setText(Text_);
    decode_String(Text_ = Doc->getFrame_Text1());
    Input_Frame1->setText(Text_);
    decode_String(Text_ = Doc->getFrame_Text2());
    Input_Frame2->setText(Text_);
    decode_String(Text_ = Doc->getFrame_Text3());
    Input_Frame3->setText(Text_);

    resize(250, 200);
}

SettingsDialog::~SettingsDialog()
{
    delete all;
    delete valExpr;
}

void SettingsDialog::slotDataSetBrowse() {
  AuxFilesDialog *d = new AuxFilesDialog(this, "*.dat");
  if(d->exec() != QDialog::Accepted) return;

  Input_DataSet->setText(d->fileName);
}

void SettingsDialog::slotDataDisplayBrowse() {
  AuxFilesDialog *d = new AuxFilesDialog(this, "*.dpl");
  if(d->exec() != QDialog::Accepted) return;

  Input_DataDisplay->setText(d->fileName);
}

void SettingsDialog::slotScriptBrowse() {
  AuxFilesDialog *d = new AuxFilesDialog(this, "*.m");
  if(d->exec() != QDialog::Accepted) return;

  Input_Script->setText(d->fileName);
}

// -----------------------------------------------------------
// Close the dialog, applying any settings
void SettingsDialog::slotOK()
{
    slotApply();
    accept();
}

// -----------------------------------------------------------
// Applies the settings chosen in the dialog
void SettingsDialog::slotApply()
{
    bool changed = false;

    if(Doc->getDataSet() != Input_DataSet->text())
    {
        Doc->setDataSet(Input_DataSet->text());
        changed = true;
    }

    if(Doc->getDataDisplay() != Input_DataDisplay->text())
    {
        Doc->setDataDisplay(Input_DataDisplay->text());
        changed = true;
    }

    if(Doc->getScript() != Input_Script->text())
    {
        Doc->setScript(Input_Script->text());
        changed = true;
    }

    if(Doc->getSimOpenDpl() != Check_OpenDpl->isChecked())
    {
        Doc->setSimOpenDpl(Check_OpenDpl->isChecked());
        changed = true;
    }

    if(Doc->getSimRunScript() != Check_RunScript->isChecked())
    {
        Doc->setSimRunScript(Check_RunScript->isChecked());
        changed = true;
    }

    if(Doc->getGridOn() != Check_GridOn->isChecked())
    {
        Doc->setGridOn(Check_GridOn->isChecked());
        changed = true;
    }

    if(Doc->getGridX() != Input_GridX->text().toInt())
    {
        Doc->setGridX(Input_GridX->text().toInt());
        changed = true;
    }

    if(Doc->getGridY() != Input_GridY->text().toInt())
    {
        Doc->setGridY(Input_GridY->text().toInt());
        changed = true;
    }

    FrameSize current = Doc->getShowFrame(); // Current frame
    FrameSize selected = static_cast<FrameSize>(Combo_Frame->itemData(Combo_Frame->currentIndex(), Qt::UserRole).toInt()); // Selected frame

    if (current != selected) {
        Doc->setShowFrame(static_cast<int>(selected));
        changed = true;
    }


    QString t;
    encode_String(Input_Frame0->toPlainText(), t);
    if(Doc->getFrame_Text0() != t)
    {
        Doc->setFrame_Text0(t);
        changed = true;
    }

    encode_String(Input_Frame1->text(), t);
    if(Doc->getFrame_Text1() != t)
    {
        Doc->setFrame_Text1(t);
        changed = true;
    }

    encode_String(Input_Frame2->text(), t);
    if(Doc->getFrame_Text2() != t)
    {
        Doc->setFrame_Text2(t);
        changed = true;
    }

    encode_String(Input_Frame3->text(), t);
    if(Doc->getFrame_Text3() != t)
    {
        Doc->setFrame_Text3(t);
        changed = true;
    }

    if(changed)
    {
        Doc->setChanged(true);
        Doc->viewport()->repaint();
    }
}

AuxFilesDialog::AuxFilesDialog(QWidget *parent, const QString &filter) :QDialog(parent)
{
  fileName = QString();
  model = new QFileSystemModel();
  model->setFilter(QDir::Files);
  model->setRootPath(QucsSettings.QucsWorkDir.absolutePath());

  QStringList filtersList;
  filtersList << filter;
  model->setNameFilters(filtersList);
  // hide items not passing the filter, not just disable them
  model->setNameFilterDisables(false);

  tree = new QTreeView();
  tree->setModel(model);
  tree->sortByColumn(0, Qt::AscendingOrder);
  tree->setSortingEnabled(true);
  tree->setRootIndex(model->index(QucsSettings.QucsWorkDir.absolutePath()));

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(tree);
  tree->show();

  //tree->header()->setStretchLastSection(false);
  //tree->resizeColumnToContents(0);
  tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
  //tree->header()->setResizeMode(0, QHeaderView::Stretch);
  connect(tree, SIGNAL(doubleClicked(const QModelIndex &)), SLOT(slotDoubleClick(const QModelIndex &)));

  setWindowTitle("Choose a file");

  // buttons at the bottom of the dialog
  QHBoxLayout *Btns = new QHBoxLayout();
  Btns->setSpacing(5);
  Btns->setContentsMargins(5,5,5,5);
  layout->addLayout(Btns);

  Btns->addStretch();
  QPushButton *OkButt = new QPushButton(tr("Select"));
  Btns->addWidget(OkButt);
  connect(OkButt, SIGNAL(clicked()), SLOT(slotSelect()));
  QPushButton *CancelButt = new QPushButton(tr("Cancel"));
  Btns->addWidget(CancelButt);
  connect(CancelButt, SIGNAL(clicked()), SLOT(reject()));

  OkButt->setDefault(true);

  resize(600, 300);
}

void AuxFilesDialog::slotDoubleClick(const QModelIndex &index)
{
  // get the file name of the item
  fileName = model->fileInfo(index).fileName();
  accept();
}

void AuxFilesDialog::slotSelect(void)
{
  slotDoubleClick(tree->currentIndex());
}
