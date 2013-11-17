#include <math.h>
#include "exportdiagramdialog.h"

ExportDiagramDialog::ExportDiagramDialog(int w, int h, QWidget *parent) :
    QDialog(parent)
{
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

    editFilename = new QLineEdit("/home/vvk/1.png");
    connect(editFilename,SIGNAL(textChanged(QString)),this,SLOT(setSvg(QString)));

    editResolutionX = new QLineEdit(QString::number(dwidth));
    QIntValidator *val = new QIntValidator(0,64000);
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
    QString nam = QFileDialog::getSaveFileName(this,tr("Export diagram to file"),QDir::homeDirPath(),
                                               "SVG vector graphics (*.svg) ;;"
                                               "PNG images (*.png) ;;"
                                               "JPEG images (*.jpg *.jpeg)");
    editFilename->setText(nam);
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
