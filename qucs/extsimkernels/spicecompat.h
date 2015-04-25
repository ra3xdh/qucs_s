#ifndef SPICECOMPAT_H
#define SPICECOMPAT_H

#include <QtCore>

namespace spicecompat {
     QString check_refdes(QString &Name, QString &Model);
     QString normalize_value(QString Value);
     QString convert_functions(QString tok, bool isXyce);
     void splitEqn(QString &eqn, QStringList &tokens);
     bool containNodes(QStringList &tokens);
     void convertNodeNames(QStringList &tokens, QString &sim);
}

#endif // SPICECOMPAT_H
