#include "spicecompat.h"

QString spicecompat::check_refdes(QString &Name,QString &SpiceModel) // If starting letters of the component name
{                                       // match spice model (i.e. R1, C1, L1)returns Name.
                                        // Otherwise returns unique Spice Refdes (Name+SpiceModel)
    if (Name.startsWith(SpiceModel,Qt::CaseInsensitive)) {
        return Name;
    } else {
        return (SpiceModel + Name);
    }
}

QString spicecompat::normalize_value(QString Value)
{

}

