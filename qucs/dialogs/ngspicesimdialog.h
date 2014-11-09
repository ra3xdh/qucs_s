#ifndef NGSPICESIMDIALOG_H
#define NGSPICESIMDIALOG_H

#include <QtGui>
#include "schematic.h"

class NgspiceSimDialog : public QDialog
{
    Q_OBJECT
private:

    Schematic *Sch;

    QProcess *ngspice;

    QPushButton *buttonSimulate;
    QPushButton *buttonStopSim;

    QTextEdit *editSimConsole;


    void startNgSpice(QString netlist);

public:
    explicit NgspiceSimDialog(Schematic *sch,QWidget *parent = 0);
    ~NgspiceSimDialog();
    
signals:
    
public slots:

private slots:
    void slotSimulate();
    void killThemAll();
    void slotProcessNgSpiceOutput(int exitCode);
    
};

#endif // NGSPICESIMDIALOG_H
