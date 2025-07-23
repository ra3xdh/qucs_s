/***************************************************************************
                                general.h
                                ----------

    author                :  2019 Andres Martinez-Mera
    email                  :  andresmmera@protonmail.com
 ***************************************************************************/

/***************************************************************************
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 ***************************************************************************/
#ifndef GENERAL_H
#define GENERAL_H

#include <QString>
#include <cmath>
#include <complex>
enum Units { Capacitance, Inductance, Length, Resistance, Degrees, NoUnits };

QString RoundVariablePrecision(double);
QString num2str(double, Units);
QString num2str(double);
std::complex<double> Str2Complex(QString);
QString ConvertLengthFromM(QString, double);

#endif // GENERAL_H
