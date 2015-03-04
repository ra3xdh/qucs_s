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
    QRegExp r_pattern("^[0-9]+.*Ohm$");
    QRegExp c_pattern("^[0-9]+.*F$");
    QRegExp l_pattern("^[0-9]+.*H$");
    QRegExp v_pattern("^[0-9]+.*V$");
    QRegExp hz_pattern("^[0-9]+.*Hz$");
    QRegExp s_pattern("^[0-9]+.*S$");
    QRegExp var_pattern("^[A-Za-z].*$");

    QString s = Value.remove(' ');
    if (r_pattern.exactMatch(s)) {
        s.remove("Ohm");
        s.replace("M","Meg");
    } else if (c_pattern.exactMatch(s)) {
        s.remove("F");
        s.replace("M","Meg");
    } else if (l_pattern.exactMatch(s)) {
        s.remove("H");
        s.replace("M","Meg");
    } else if (v_pattern.exactMatch(s)) {
        s.remove("V");
        s.replace("M","Meg");
    } else if (hz_pattern.exactMatch(s)) {
        s.remove("Hz");
        s.replace("M","Meg");
    } else if (s_pattern.exactMatch(s)) {
        s.remove("S");
        s.replace("M","Meg");
    } else if (var_pattern.exactMatch(s)) {
        s = "{" + s + "}";
    }


    return s.toUpper();
}

