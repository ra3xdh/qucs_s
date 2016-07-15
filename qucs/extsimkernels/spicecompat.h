#ifndef SPICECOMPAT_H
#define SPICECOMPAT_H

#include <QtCore>

/*!
  \brief spicecompat namespace contains definitions responsible
         for conversion of variables and node names from Qucs
         notation to Spice notation
 *
 */
namespace spicecompat {
     QString check_refdes(QString &Name, QString &Model);
     QString normalize_value(QString Value);
     QString convert_function(QString tok, bool isXyce);
     void convert_functions(QStringList &tokens, bool isXyce);
     void splitEqn(QString &eqn, QStringList &tokens);
     bool containNodes(QStringList &tokens, QStringList &vars);
     void convertNodeNames(QStringList &tokens, QString &sim);
     QString normalize_node_name(QString nod);
     QString convert_relative_filename(QString filename);
     int getPins(const QString &file, const QString &compname, QStringList &pin_names);
     QString getSubcktName(QString subfilename);
     QString convert_sweep_type(QString sweep);
     bool check_nodename(QString &node);

     enum Simulator {simNgspice = 0, simXyceSer = 1, simXycePar = 2, simSpiceOpus = 3, simQucsator = 4, simNotSpecified=10};
}

#endif // SPICECOMPAT_H
