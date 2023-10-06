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
    double z0;
    QString file;
    QString device_name;

public:
    void setZ0(double z0_) { z0 = z0_; }
    void setFile(const QString &file_) { file = file_; }
    void setDeviceName(const QString &name_) {device_name = name_; }

    bool convertTouchstone(QTextStream *stream);


    S2Spice();
    virtual ~S2Spice() {}
};

#endif // S2SPICE_H
