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
#include "exportdialog.h"

ExportDialog::ExportDialog(int w, int h, int wsel, int hsel, QString filename_, bool nosel_, QWidget *parent) :
    QDialog(parent)
{

    setCaption(tr("Export graphics"));
    dwidth = w;
    dheight = h;
    dwidthsel = wsel;
    dheightsel = hsel;
    svg = false;
    noselected = nosel_;

    filename = filename_;

    lblFilename = new QLabel(tr("Save to file (Graphics format by extension)"));
    lblResolutionX = new QLabel(tr("Width  in pixels"));
    lblResolutionY = new QLabel(tr("Height in pixels"));
    lblRatio = new QLabel(tr("Scale factor: "));
    lblFormat = new QLabel(tr("Image format:"));

    ExportButt = new QPushButton(tr("Export"));
    connect(ExportButt,SIGNAL(clicked()),this,SLOT(accept()));
    CancelButt = new QPushButton(tr("Cancel"));
    connect(CancelButt,SIGNAL(clicked()),this,SLOT(reject()));
    SaveButt = new QPushButton(tr("File"));
    connect(SaveButt,SIGNAL(clicked()),this,SLOT(setFileName()));

    editFilename = new QLineEdit(filename);
    connect(editFilename,SIGNAL(textChanged(QString)),this,SLOT(setSvg(QString)));

    editResolutionX = new QLineEdit(QString::number(dwidth));
    QIntValidator *val = new QIntValidator(0,64000,this);
    editResolutionX->setValidator(val);
    editResolutionX->setEnabled(false);
    editResolutionY = new QLineEdit(QString::number(dheight));
    editResolutionY->setValidator(val);
    editResolutionY->setEnabled(false);
    editScale = new QLineEdit(QString::number(1.0));
    QDoubleValidator *val1 = new QDoubleValidator(0,20.0,2,this);
    editScale->setValidator(val1);

    cbxImgType = new QComboBox(this);
    QStringList lst;
    lst<<"Colour"<<"Monochrome";
    cbxImgType->addItems(lst);

    cbRatio = new QCheckBox(tr("Original width to height ratio"));
    cbRatio->setChecked(true);
    connect(cbRatio,SIGNAL(toggled(bool)),this,SLOT(recalcRatio()));

    cbResolution = new QCheckBox(tr("Original size"));
    connect(cbResolution,SIGNAL(toggled(bool)),editResolutionX,SLOT(setDisabled(bool)));
    connect(cbResolution,SIGNAL(toggled(bool)),editResolutionY,SLOT(setDisabled(bool)));
    connect(cbResolution,SIGNAL(toggled(bool)),cbRatio,SLOT(setDisabled(bool)));
    connect(cbResolution,SIGNAL(toggled(bool)),editScale,SLOT(setDisabled(bool)));
    connect(cbResolution,SIGNAL(toggled(bool)),this,SLOT(restoreOriginalWtoH()));
    cbResolution->setChecked(true);

    connect(editResolutionX,SIGNAL(textEdited(QString)),this,SLOT(calcHeight()));
    connect(editResolutionY,SIGNAL(textEdited(QString)),this,SLOT(calcWidth()));
    connect(editScale,SIGNAL(textChanged(QString)),this,SLOT(recalcScale()));

    cbSelected = new QCheckBox(tr("Export selected only"));
    connect(cbSelected,SIGNAL(toggled(bool)),this,SLOT(setSelectedWH()));
    cbSelected->setChecked(false);
    if (noselected) cbSelected->setDisabled(true);

    //cbResolution->setEnabled(false);
    cbRatio->setEnabled(false);


    top = new QVBoxLayout;
    lower1 = new QHBoxLayout;
    lower2 = new QHBoxLayout;
    lower3 = new QHBoxLayout;
    lower4 = new QHBoxLayout;

    top->addWidget(lblFilename);
    lower1->addWidget(editFilename);
    lower1->addWidget(SaveButt);
    top->addLayout(lower1);
    lower4->addWidget(lblFormat);
    lower4->addWidget(cbxImgType);
    top->addLayout(lower4);
    top->addWidget(cbResolution);
    //top->addWidget(cbRatio);
    lower3->addWidget(lblRatio);
    lower3->addWidget(editScale);
    top->addLayout(lower3);
    top->addWidget(lblResolutionX);
    top->addWidget(editResolutionX);
    top->addWidget(lblResolutionY);
    top->addWidget(editResolutionY);
    top->addWidget(cbSelected);

    lower2->addWidget(ExportButt);
    lower2->addWidget(CancelButt);
    top->addLayout(lower2);
    this->setLayout(top);

    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
    this->setWindowTitle(tr("Export schematic to raster or vector image"));
}

QString ExportDialog::FileToSave()
{
    return editFilename->text();
}

bool ExportDialog::isOriginalSize()
{
    return cbResolution->isChecked();
}

int ExportDialog::Xpixels()
{
    return editResolutionX->text().toInt();
}

int ExportDialog::Ypixels()
{
    return editResolutionY->text().toInt();
}

void ExportDialog::setFileName()
{
    /*QString nam = dialog.getSaveFileName(this,tr("Export diagram to file"),QDir::homeDirPath(),
                                         "SVG vector graphics (*.svg) ;;"
                                         "PNG images (*.png) ;;"
                                         "JPEG images (*.jpg *.jpeg)");
    */
    //QFileInfo inf(filename);
    QFileDialog dialog(this, tr("Export diagram to file"), editFilename->text(),
                       "SVG vector graphics (*.svg) ;;"
                       "PNG images (*.png) ;;"
                       "JPEG images (*.jpg *.jpeg) ;;"
                       "PDF (*.pdf) ;;"
                       "PDF + LaTeX (*.pdf_tex) ;;"
                       "EPS Encapsulated Postscript (*.eps)");
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if(dialog.exec())
    {
        QString nam = dialog.selectedFile();
        QString extension;
        if(dialog.selectedNameFilter().contains("*.png")) extension=QString(".png");
        if(dialog.selectedNameFilter().contains("*.jpg")) extension=QString(".jpg");
        if(dialog.selectedNameFilter().contains("*.svg")) extension=QString(".svg");
        if(dialog.selectedNameFilter().contains("*.pdf")) extension=QString(".pdf");
        if(dialog.selectedNameFilter().contains("*.pdf_tex")) extension=QString(".pdf_tex");
        if(dialog.selectedNameFilter().contains("*.eps")) extension=QString(".eps");
        if(nam.toLower().section("/",-1,-1).contains(".")) //has the user specified an extension?
            editFilename->setText(nam); //yes, just leave unchanged
        else
            editFilename->setText(nam+extension); //no, add extension
    }
}

void ExportDialog::calcWidth()
{
    if (cbRatio->isChecked()) {
        float h = editResolutionY->text().toFloat();
        float w =  round((h*dwidth)/dheight);
        editResolutionX->setText(QString::number(w));
    }
}


void ExportDialog::calcHeight()
{
    if (cbRatio->isChecked()) {
        float w = editResolutionX->text().toFloat();
        float h =  round((w*dheight)/dwidth);
        editResolutionY->setText(QString::number(h));
    }

}

void ExportDialog::recalcRatio()
{
    if (cbRatio->isChecked()) {
        calcHeight();
    }
}

void ExportDialog::restoreOriginalWtoH()
{
    if (cbResolution->isChecked()) {
        editScale->setText(QString::number(1.0));
        editResolutionX->setText(QString::number(dwidth));
        editResolutionY->setText(QString::number(dheight));
    }
}

bool ExportDialog::isSvg()
{
    return svg;
}

bool ExportDialog::isExportSelected()
{
    return cbSelected->isChecked();
}

void ExportDialog::setSvg(QString filename)
{
    QFileInfo graphics_file(filename);
    QString ext = graphics_file.suffix();
    ext = ext.toLower();
    if ((ext=="svg")||(ext=="pdf")||(ext=="eps")||(ext=="pdf_tex")) {
        svg = true;
        cbResolution->setChecked(true);
        cbResolution->setDisabled(true);
        cbxImgType->setDisabled(true);
        cbRatio->setChecked(true);
    } else {
        svg = false;
        cbResolution->setEnabled(true);
        cbxImgType->setEnabled(true);
    }
}

bool ExportDialog::isValidFilename()
{
    QString nam = editFilename->text();
    QStringList filetypes;
    QFileInfo inf(nam);
    filetypes<<"png"<<"svg"<<"jpeg"<<"jpg"<<"pdf"<<"pdf_tex"<<"eps"
            <<"PNG"<<"JPG"<<"SVG"<<"JPEG"<<"PDF"
            <<"PDF_TEX"<<"EPS";

    if (filetypes.contains(inf.suffix())) {
        return true;
    } else {
        return false;
    }
}

bool ExportDialog::needsInkscape()
{
    QString nam = editFilename->text();
    QStringList filetypes;
    QFileInfo inf(nam);
    filetypes<<"pdf"<<"pdf_tex"<<"eps"<<"PDF"<<"PDF_TEX"<<"EPS";

    if (filetypes.contains(inf.suffix())) {
        return true;
    } else {
        return false;
    }
}

bool ExportDialog::isPdf()
{
    QFileInfo inf(editFilename->text());
    if (inf.suffix().toLower()=="pdf") return true;
    else return false;
}

bool ExportDialog::isPdf_Tex()
{
    QFileInfo inf(editFilename->text());
    if (inf.suffix().toLower()=="pdf_tex") return true;
    else return false;
}

bool ExportDialog::isEps()
{
    QFileInfo inf(editFilename->text());
    if (inf.suffix().toLower()=="eps") return true;
    else return false;
}

void ExportDialog::setSelectedWH()
{
    if (cbSelected->isChecked()) {
        editResolutionX->setText(QString::number(dwidthsel));
        editResolutionY->setText(QString::number(dheightsel));
    } else {
        editResolutionX->setText(QString::number(dwidth));
        editResolutionY->setText(QString::number(dheight));
    }
    recalcScale();
}

void ExportDialog::setDiagram()
{
    cbSelected->setChecked(true);
    cbSelected->setDisabled(true);
    this->setWindowTitle(tr("Export diagram to raster or vector image"));
}

float ExportDialog::getScale()
{
    scale = editScale->text().toFloat();
    return scale;
}

void ExportDialog::recalcScale()
{
    scale = editScale->text().toFloat();
    if (cbSelected->isChecked()) {
        editResolutionX->setText(QString::number(scale*dwidthsel));
        editResolutionY->setText(QString::number(scale*dheightsel));
    } else {
        editResolutionX->setText(QString::number(scale*dwidth));
        editResolutionY->setText(QString::number(scale*dheight));
    }

}

ExportDialog::ImgFormat ExportDialog::getImgFormat()
{
    switch (cbxImgType->currentIndex()) {
    case 0 : return ExportDialog::Coloured;
        break;
    case 1 : return ExportDialog::Monochrome;
        break;
    default : break;
    }
}
