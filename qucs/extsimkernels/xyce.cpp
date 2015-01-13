#include "xyce.h"

Xyce::Xyce(Schematic *sch_, QObject *parent) :
    AbstractSpiceKernel(sch_, AbstractSpiceKernel::Xyce, parent)
{
}

void Xyce::createNetlist(QTextStream &stream, int NumPorts, QStringList &simulations,
                    QStringList &vars, QStringList &outputs)
{

}
