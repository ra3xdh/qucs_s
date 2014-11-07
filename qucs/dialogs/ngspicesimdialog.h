#ifndef NGSPICESIMDIALOG_H
#define NGSPICESIMDIALOG_H

#include <QtGui>
#include "schematic.h"

class NgspiceSimDialog : public QDialog
{
    Q_OBJECT
private:

    Schematic *Sch;

    QPushButton *buttonSimulate;
    QPushButton *buttonStopSim;
public:
    explicit NgspiceSimDialog(Schematic *sch,QWidget *parent = 0);
    ~NgspiceSimDialog();
    
signals:
    
public slots:

private slots:
    void slotSimulate();
    
};

#endif // NGSPICESIMDIALOG_H
