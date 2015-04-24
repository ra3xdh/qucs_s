/***************************************************************************
                             qucs2spice.h
                             ----------------
    begin                : Mon Dec 1 2014
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



#ifndef QUCS2SPICE_H
#define QUCS2SPICE_H

#include <QtCore>

/*! \brief qucs2spice namespace contains functions responsible for
 *         conversion of Qucs netlist into Spice netlist
 *
 */
namespace qucs2spice
{
    QString convert_netlist(QString netlist, bool xyce=false);
}

#endif // QUCS2SPICE_H
