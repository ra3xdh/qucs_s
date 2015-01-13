#ifndef XYCE_H
#define XYCE_H

#include <QtCore>
#include "abstractspicekernel.h"
#include "ngspice.h"

class Xyce : public AbstractSpiceKernel
{
    Q_OBJECT
public:
    explicit Xyce(Schematic *sch_, QObject *parent = 0);
    
protected:
    void createNetlist(QTextStream &stream, int NumPorts, QStringList &simulations,
                  QStringList &vars, QStringList &outputs);
    
};

#endif // XYCE_H
