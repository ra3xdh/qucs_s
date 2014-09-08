/***************************************************************************
                                schcauer.h
                              ----------------
    begin                : Wed Apr 10 2014
    copyright            : (C) 2014 by Vadim Kuznetsov
    email                : ra3xdh@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SCHCAUER_H
#define SCHCAUER_H

#include <QtCore>
#include <complex>
#include <math.h>
#include "filter.h"

class SchCauer : public Filter // Cauer 2-order section
{

private:
    void createGenericSchematic(QString &s);

protected:

    void calcHighPass();
    void calcLowPass();
    void calcBandPass();
    void calcBandStop();
    void createHighPassSchematic(QString &s);
    void createLowPassSchematic(QString &s);
    void createBandPassSchematic(QString &s);
    void createBandStopSchematic(QString &s);

public:
    SchCauer(Filter::FilterFunc ffunc_, Filter::FType type_, FilterParam par);
};

#endif // SCHCAUER_H
