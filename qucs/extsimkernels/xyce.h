#ifndef XYCE_H
#define XYCE_H

#include <QtCore>
#include "abstractspicekernel.h"
#include "ngspice.h"

class Xyce : public AbstractSpiceKernel
{
    Q_OBJECT
private:
    QStringList simulationsQueue;
    QStringList netlistQueue;
    void determineUsedSimulations();
    void nextSimulation();

public:
    explicit Xyce(Schematic *sch_, QObject *parent = 0);
    
protected:
    void createNetlist(QTextStream &stream, int NumPorts, QStringList &simulations,
                  QStringList &vars, QStringList &outputs);
protected slots:
    void slotFinished();

public slots:
    void slotSimulate();
    
};

#endif // XYCE_H
