/***************************************************************************
                                s2spice.h
                             ----------------
    begin                : Thu Oct 05 2023
    copyright            : (C) 2023 by Vadim Kuznetsov
    based on S2Spice utility by Dan Dickey and Jim Mittel
    see https://sourceforge.net/p/ngspice/discussion/120973/thread/51228e0b01/
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


#ifndef S2SPICE_H
#define S2SPICE_H

#include <QtCore>

class S2Spice
{

private:
    double a_z0;
    QString a_file;
    QString a_device_name;
    QString a_err_text;

public:
    void setZ0(double z0) { a_z0 = z0; }
    void setFile(const QString& file) { a_file = file; }
    void setDeviceName(const QString& name) {a_device_name = name; }

    QString getErrText() const { return a_err_text; }

    bool convertTouchstone(QTextStream *stream);

    S2Spice();
    virtual ~S2Spice() {}
};

#endif // S2SPICE_H
