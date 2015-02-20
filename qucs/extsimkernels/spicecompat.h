#ifndef SPICECOMPAT_H
#define SPICECOMPAT_H

#include <QtCore>

namespace spicecompat {
     QString check_refdes(QString &Name, QString &Model);
     QString normalize_value(QString Value);
}

#endif // SPICECOMPAT_H
