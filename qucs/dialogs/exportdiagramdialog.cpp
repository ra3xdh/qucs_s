#include "exportdiagramdialog.h"

ExportDiagramDialog::ExportDiagramDialog(QWidget *parent) :
    QDialog(parent)
{
    lblFilename = new QLabel(tr("Save to File (Graphics Format by Extension)"));
    lblResolutionX = new QLabel(tr("X-axis size (pixels)"));
    lblResolutionY = new QLabel(tr("X-axis size (pixels)"));

    ExportButt = new QPushButton(tr("Export"));
    connect(ExportButt,SIGNAL(clicked()),this,SLOT(accept()));
    CancelButt = new QPushButton(tr("Cancel"));
    connect(CancelButt,SIGNAL(clicked()),this,SLOT(reject()));
    SaveButt = new QPushButton(tr("File"));
    connect(SaveButt,SIGNAL(clicked()),this,SLOT(setFileName()));

    editFilename = new QLineEdit("/home/vvk/1.png");

    editResolutionX = new QLineEdit("400");
    QIntValidator *val = new QIntValidator(0,64000);
    editResolutionX->setValidator(val);
    editResolutionX->setEnabled(false);
    editResolutionY = new QLineEdit("300");
    editResolutionY->setValidator(val);
    editResolutionY->setEnabled(false);

    cbResolution = new QCheckBox(tr("Original Resolution"));
    connect(cbResolution,SIGNAL(toggled(bool)),editResolutionX,SLOT(setDisabled(bool)));
    connect(cbResolution,SIGNAL(toggled(bool)),editResolutionY,SLOT(setDisabled(bool)));
    cbResolution->setChecked(true);

    top = new QVBoxLayout;
    lower1 = new QHBoxLayout;
    lower2 = new QHBoxLayout;
    lower3 = new QHBoxLayout;

    top->addWidget(lblFilename);
    lower1->addWidget(editFilename);
    lower1->addWidget(SaveButt);
    top->addLayout(lower1);
    top->addWidget(cbResolution);
    top->addWidget(lblResolutionX);
    top->addWidget(editResolutionX);
    top->addWidget(lblResolutionY);
    top->addWidget(editResolutionY);
    lower2->addWidget(ExportButt);
    lower2->addWidget(CancelButt);
    top->addLayout(lower2);
    this->setLayout(top);


}

QString ExportDiagramDialog::FileToSave()
{
    return editFilename->text();
}

bool ExportDiagramDialog::OriginalSize()
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
    QString nam = QFileDialog::getSaveFileName(this,tr("Export diagram to file"),"$HOME",
                                               "Images (*.png *.jpg *.bmp *.gif)");
    editFilename->setText(nam);
}

