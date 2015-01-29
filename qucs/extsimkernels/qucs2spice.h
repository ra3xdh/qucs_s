#ifndef QUCS2SPICE_H
#define QUCS2SPICE_H

#include <QtCore>

namespace qucs2spice
{
    QString convert_rcl(QString line);
    QString convert_header(QString line);
    QString convert_diode(QString line);
    QString convert_mosfet(QString line);
    QString convert_bjt(QString line);
    QString convert_cccs(QString line);
    QString convert_ccvs(QString line);
    QString convert_ccs(QString line, bool voltage);
    QString convert_vccs(QString line);
    QString convert_vcvs(QString line);
    QString convert_vcs(QString line, bool voltage);
    QString convert_netlist(QString netlist);
}

#endif // QUCS2SPICE_H
