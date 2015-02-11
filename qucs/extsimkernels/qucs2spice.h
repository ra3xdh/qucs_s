#ifndef QUCS2SPICE_H
#define QUCS2SPICE_H

#include <QtCore>

namespace qucs2spice
{
    QString convert_rcl(QString line);
    QString convert_header(QString line);
    QString convert_diode(QString line,bool xyce=false);
    QString convert_jfet(QString line, bool xyce=false);
    QString convert_mosfet(QString line, bool xyce=false);
    QString convert_bjt(QString line);
    QString convert_cccs(QString line);
    QString convert_ccvs(QString line);
    QString convert_ccs(QString line, bool voltage);
    QString convert_vccs(QString line);
    QString convert_vcvs(QString line);
    QString convert_vcs(QString line, bool voltage);
    QString convert_dc_src(QString line);
    QString convert_netlist(QString netlist, bool xyce=false);
}

#endif // QUCS2SPICE_H
