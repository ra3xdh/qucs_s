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
#include <QList>
#include <QRegularExpression>
#include <cmath>
#include <complex>
#include <QPointF>

enum Units { Capacitance, Inductance, Length, Resistance, Degrees, NoUnits };

QString RoundVariablePrecision(double);
QString num2str(double, Units);
QString num2str(double);
std::complex<double> Str2Complex(QString);
QString ConvertLengthFromM(QString, double);
void convert_MA_RI_to_dB(double * S_1, double * S_2, double *S_3, double *S_4, QString format);
double getFreqScale(QString frequency_unit);
void getMinMaxValues(QString filename, QString tracename, qreal& minX, qreal& maxX, qreal& minY, qreal& maxY);
int findClosestIndex(const QList<double>& list, double value);
double getFreqScale(QString frequency_unit);
int findClosestIndex(const QList<double>& list, double value);
double getFreqFromText(QString freq);
QPointF findClosestPoint(const QList<double>& xValues, const QList<double>& yValues, double targetX);

#endif // GENERAL_H
