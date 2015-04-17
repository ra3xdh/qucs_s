#ifndef QUCS2SPICE_H
#define QUCS2SPICE_H

#include <QtCore>

namespace qucs2spice
{
    QString convert_netlist(QString netlist, bool xyce=false);
}

#endif // QUCS2SPICE_H
