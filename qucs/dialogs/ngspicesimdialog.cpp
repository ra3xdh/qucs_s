#include "ngspicesimdialog.h"

NgspiceSimDialog::NgspiceSimDialog(Schematic *sch,QWidget *parent) :
    QDialog(parent)
{
    Sch = sch;
    buttonSimulate = new QPushButton(tr("Simulate"));
    connect(buttonSimulate,SIGNAL(clicked()),this,SLOT(slotSimulate()));
    buttonStopSim = new QPushButton(tr("Exit"));
    connect(buttonStopSim,SIGNAL(clicked()),this,SLOT(reject()));
    QHBoxLayout *top = new QHBoxLayout;
    top->addWidget(buttonSimulate);
    top->addWidget(buttonStopSim);
    this->setLayout(top);
}

NgspiceSimDialog::~NgspiceSimDialog()
{

}

void NgspiceSimDialog::slotSimulate()
{
    int num=0;
    QStringList vars,sims;
    QString tmp_path = QDir::homePath()+"/.qucs/spice4qucs.cir";
    QFile spice_file(tmp_path);
    if (spice_file.open(QFile::WriteOnly)) {
        QTextStream stream(&spice_file);
        Sch->createSpiceNetlist(stream,num,sims,vars);
        spice_file.close();
    }
    qDebug()<<sims<<vars;

    QString old_dir = QDir::currentPath(); // Execute ngspice
    QDir::setCurrent(QDir::homePath()+"/.qucs");
    QProcess::execute("ngspice "+tmp_path);
    QDir::setCurrent(old_dir);
}
