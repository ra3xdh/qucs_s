/***************************************************************************
                               exportdiagramdialog.cpp
                              ------------------
    begin                : Thu Nov 28 2013
    copyright            : (C) 2013 by Vadim Kuznetzov
    email                : <ra3xdh@gmail.com>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <math.h>
#include "exportdiagramdialog.h"

ExportDiagramDialog::ExportDiagramDialog(int w, int h, QWidget *parent) :
    QDialog(parent)
{

    setCaption(tr("Export graphics"));
    dwidth = w;
    dheight = h;
    svg = false;

    lblFilename = new QLabel(tr("Save to file (Graphics format by extension)"));
    lblResolutionX = new QLabel(tr("Width  in pixels"));
    lblResolutionY = new QLabel(tr("Height in pixels"));

    ExportButt = new QPushButton(tr("Export"));
    connect(ExportButt,SIGNAL(clicked()),this,SLOT(accept()));
    CancelButt = new QPushButton(tr("Cancel"));
    connect(CancelButt,SIGNAL(clicked()),this,SLOT(reject()));
    SaveButt = new QPushButton(tr("File"));
    connect(SaveButt,SIGNAL(clicked()),this,SLOT(setFileName()));

    editFilename = new QLineEdit("");
    connect(editFilename,SIGNAL(textChanged(QString)),this,SLOT(setSvg(QString)));

    editResolutionX = new QLineEdit(QString::number(dwidth));
    QIntValidator *val = new QIntValidator(0,64000,this);
    editResolutionX->setValidator(val);
    editResolutionX->setEnabled(false);
    editResolutionY = new QLineEdit(QString::number(dheight));
    editResolutionY->setValidator(val);
    editResolutionY->setEnabled(false);

    cbRatio = new QCheckBox(tr("Original width to height ratio"));
    cbRatio->setChecked(true);
    connect(cbRatio,SIGNAL(toggled(bool)),this,SLOT(recalcRatio()));

    cbResolution = new QCheckBox(tr("Original size"));
    connect(cbResolution,SIGNAL(toggled(bool)),editResolutionX,SLOT(setDisabled(bool)));
    connect(cbResolution,SIGNAL(toggled(bool)),editResolutionY,SLOT(setDisabled(bool)));
    connect(cbResolution,SIGNAL(toggled(bool)),cbRatio,SLOT(setDisabled(bool)));
    connect(cbResolution,SIGNAL(toggled(bool)),this,SLOT(restoreOriginalWtoH()));
    cbResolution->setChecked(true);

    connect(editResolutionX,SIGNAL(textEdited(QString)),this,SLOT(calcHeight()));
    connect(editResolutionY,SIGNAL(textEdited(QString)),this,SLOT(calcWidth()));

    top = new QVBoxLayout;
    lower1 = new QHBoxLayout;
    lower2 = new QHBoxLayout;
    lower3 = new QHBoxLayout;

    top->addWidget(lblFilename);
    lower1->addWidget(editFilename);
    lower1->addWidget(SaveButt);
    top->addLayout(lower1);
    top->addWidget(cbResolution);
    //top->addWidget(cbRatio);
    top->addWidget(lblResolutionX);
    top->addWidget(editResolutionX);
    top->addWidget(lblResolutionY);
    top->addWidget(editResolutionY);
    lower2->addWidget(ExportButt);
    lower2->addWidget(CancelButt);
    top->addLayout(lower2);
    this->setLayout(top);

    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
}

QString ExportDiagramDialog::FileToSave()
{
    return editFilename->text();
}

bool ExportDiagramDialog::isOriginalSize()
{
    return cbResolution->isChecked();
}

int ExportDiagramDialog::Xpixels()
{
    return editResolutionX->text().toInt();
}

int ExportDiagramDialog::Ypixels()
{
    return editResolutionY->text().toInt();
}

void ExportDiagramDialog::setFileName()
{
    /*QString nam = dialog.getSaveFileName(this,tr("Export diagram to file"),QDir::homeDirPath(),
                                         "SVG vector graphics (*.svg) ;;"
                                         "PNG images (*.png) ;;"
                                         "JPEG images (*.jpg *.jpeg)");
    */
    QFileDialog dialog(this, tr("Export diagram to file"), QDir::homeDirPath(),
                       "SVG vector graphics (*.svg) ;;"
                       "PNG images (*.png) ;;"
                       "JPEG images (*.jpg *.jpeg)" );
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if(dialog.exec())
    {
        QString nam = dialog.selectedFile();
        QString extension;
        if(dialog.selectedNameFilter().contains("*.png")) extension=QString(".png");
        if(dialog.selectedNameFilter().contains("*.jpg")) extension=QString(".jpg");
        if(dialog.selectedNameFilter().contains("*.svg")) extension=QString(".svg");
        if(nam.toLower().section("/",-1,-1).contains(".")) //has the user specified an extension?
            editFilename->setText(nam); //yes, just leave unchanged
        else
            editFilename->setText(nam+extension); //no, add extension
    }
}

void ExportDiagramDialog::calcWidth()
{
    if (cbRatio->isChecked()) {
        float h = editResolutionY->text().toFloat();
        float w =  round((h*dwidth)/dheight);
        editResolutionX->setText(QString::number(w));
    }
}


void ExportDiagramDialog::calcHeight()
{
    if (cbRatio->isChecked()) {
        float w = editResolutionX->text().toFloat();
        float h =  round((w*dheight)/dwidth);
        editResolutionY->setText(QString::number(h));
    }

}

void ExportDiagramDialog::recalcRatio()
{
    if (cbRatio->isChecked()) {
        calcHeight();
    }
}

void ExportDiagramDialog::restoreOriginalWtoH()
{
    if (cbResolution->isChecked()) {
        editResolutionX->setText(QString::number(dwidth));
        editResolutionY->setText(QString::number(dheight));
    }
}

bool ExportDiagramDialog::isSvg()
{
    return svg;
}

void ExportDiagramDialog::setSvg(QString filename)
{
    QFileInfo graphics_file(filename);
    QString ext = graphics_file.suffix();
    if ((ext=="svg")||(ext=="SVG")) {
        svg = true;
        cbResolution->setChecked(true);
        cbResolution->setDisabled(true);
        cbRatio->setChecked(true);
    } else {
        svg = false;
        cbResolution->setEnabled(true);
    }
}
