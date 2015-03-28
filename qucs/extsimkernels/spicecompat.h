#ifndef SPICECOMPAT_H
#define SPICECOMPAT_H

#include <QtCore>

namespace spicecompat {
     QString check_refdes(QString &Name, QString &Model);
     QString normalize_value(QString Value);
     QString convert_functions(QString tok, bool isXyce);
}

#endif // SPICECOMPAT_H
