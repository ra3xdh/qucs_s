#ifndef SPICECOMPAT_H
#define SPICECOMPAT_H

#include <QString>
#include <QStringList>
#include <QRegularExpression>
#include "component.h"

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
     QString getSubcktName(const QString& subfilename);
     QString convert_sweep_type(const QString& sweep);
     bool check_nodename(QString &node);
     QString getDefaultSimulatorName(int simulator);

     enum Simulator : int {
         simNotSpecified = 0b00000000,
         simNgspice      = 0b00000001,
         simXyce         = 0b00000010,
         simSpiceOpus    = 0b00000100,
         simQucsator     = 0b00001000,
         simSpice        = 0b00000111,
         simAll          = 0b11111111};
     enum CMgen_mode {cmgenSUBifs = 0, cmgenEDDifs = 1, cmgenSUBmod = 2, cmgenEDDmod = 3};
}

#endif // SPICECOMPAT_H
