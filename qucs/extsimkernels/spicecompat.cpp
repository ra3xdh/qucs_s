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

QString spicecompat::convert_functions(QString tok, bool isXyce)
{
    QStringList conv_list_ngspice; // Put here functions need to be converted
    conv_list_ngspice<<"q"<<"1.6021765e-19"
            <<"kB"<<"1.38065e-23"
            <<"pi"<<"3.1415926539"
            <<"step"<<"stp";
    QStringList conv_list_xyce = conv_list_ngspice;

    QStringList conv_list;
    if (isXyce) conv_list = conv_list_xyce;
    else conv_list = conv_list_ngspice;

    for(int i=0;i<conv_list.count();i+=2) {
        if (conv_list.at(i)==tok)
            return conv_list.at(i+1);
    }

    return tok;
}
