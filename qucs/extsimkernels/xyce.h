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
    void determineUsedSimulations();

public:
    explicit Xyce(Schematic *sch_, QObject *parent = 0);
    
protected:
    void createNetlist(QTextStream &stream, int NumPorts, QStringList &simulations,
                  QStringList &vars, QStringList &outputs);
public slots:
    void slotSimulate();
    
};

#endif // XYCE_H
