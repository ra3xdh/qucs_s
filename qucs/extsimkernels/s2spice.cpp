/***************************************************************************
                               s2spice.cpp
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


#include <QtCore>
#include <cmath>

#include "misc.h"
#include "s2spice.h"
#include "main.h"

#define MAXLINE 500
#define MAXFREQS 16001
#define MAXPORTS 8
#define MAXNAME 128

S2Spice::S2Spice()
{
    z0 = -1;
}

bool S2Spice::convertTouchstone(QTextStream *stream)
{

    auto s = new double [MAXFREQS][MAXPORTS][MAXPORTS][2];
    double freqs[MAXFREQS];

    int f;
    int ports;
    int funits = 1;
    int numf;

    double z[MAXPORTS];
    double ph, offset, mag;


    QFile ff(file);
    if (!ff.open(QIODevice::ReadOnly)) {
        err_text = "Failed to open file: " + file + "\n";
        return false;
    }
    QTextStream in_stream(&ff);

    /* Find number of ports */
    QFileInfo inf(file);
    ports = inf.suffix().mid(1,1).toInt();
    if ( (ports < 1) || (ports > MAXPORTS) ) {
        err_text = "Invalid port number in file: " + file + "\n";
        return false;
    }


    /* build first line of output file */
    (*stream) << ".SUBCKT " + device_name;
    for (int i = 0; i <= ports; i++) {
        (*stream) << QString(" %1").arg(i+1);
    }
    (*stream) << "\n";

    /* read and decode format line */

   QString next_line;
   while(in_stream.readLineInto(&next_line)) {
       if (next_line.at(0) == '#') break;
   }

    next_line = next_line.toUpper();
    funits = 1000000000;    /* GHz is the default frequency units */
    if ( next_line.contains(" HZ ") )
        funits = 1;
    else if ( next_line.contains(" KHZ ") )
        funits = 1000;
    else if ( next_line.contains(" MHZ ") )
        funits = 1000000;
    else if ( next_line.contains(" GHZ ") )
        funits = 1000000000;

    bool dB = false;
    bool RI = false;
    if ( next_line.contains(" DB " ) ) {
        dB = true;
    } else if ( next_line.contains(" RI " ) ) {
        RI = true;
    }

    if (!next_line.contains(" S " )) {
        err_text = "Wrong data in file: " + file + "\n";
        return false;
    }
    /* input impedances */

    QStringList tmp_lst = next_line.split(" ",qucs::SkipEmptyParts);
    z[0] = tmp_lst.at(tmp_lst.count()-1).toDouble();
    for (int i = 0; i < ports; i++ ) {
        if ( z0 < 0 ) {  /* takes the Z value from the input file */
            z[i] = z[0];
        } else {        /* takes the Z value from the command line */
            z[i] = z0;
        }
    }


    /* define resistances for Spice model */

    for ( int i = 0; i < ports; i++ ) {
        (*stream) << QString("R%1N %2 %3 %4\n").arg(i+1).arg(i+1).arg(10*(i+1)).arg(-z[i]);
        (*stream) << QString("R%1P %2 %3 %4\n").arg(i+1).arg(10*(i+1)).arg(10*(i+1)+1).arg(2*z[i]);
    }
    (*stream) << "\n";

    /* read S parameters into matrix */

    f = 0;
    while(in_stream.readLineInto(&next_line)) {
        if(next_line.isEmpty()) continue;
        if(next_line.at(0)=='#') continue;
        if(next_line.at(0)=='!') continue;
        tmp_lst = next_line.split(QRegularExpression("[ \\t]"), qucs::SkipEmptyParts);
        if (tmp_lst.count() < (ports*ports)+1) {
            err_text = "Touchstone file parse error!\n";
            return false;
        }
        numf = f + 1;
        freqs[f] = tmp_lst.at(0).toDouble();
        int idx = 1;
        if ( ports != 2 ) {
            for (int i = 0; i < ports; i++ )
                for (int j = 0; j < ports; j++ ) {
                    s[f][i][j][0] = tmp_lst.at(idx).toDouble();
                    idx++;
                    s[f][i][j][1] = tmp_lst.at(idx).toDouble();
                    idx++;
                }
        } else {
            s[f][0][0][0] = tmp_lst.at(idx).toDouble();
            idx++;
            s[f][0][0][1] = tmp_lst.at(idx).toDouble();
            idx++;
            s[f][1][0][0] = tmp_lst.at(idx).toDouble();
            idx++;
            s[f][1][0][1] = tmp_lst.at(idx).toDouble();
            idx++;
            s[f][0][1][0] = tmp_lst.at(idx).toDouble();
            idx++;
            s[f][0][1][1] = tmp_lst.at(idx).toDouble();
            idx++;
            s[f][1][1][0] = tmp_lst.at(idx).toDouble();
            idx++;
            s[f][1][1][1] = tmp_lst.at(idx).toDouble();
            idx++;
        }
        f++;
    }


    /* write values to output file*/

    int model_cnt = 0;
    for (int i = 0; i < ports; i++ ) {
        for (int j = 0; j < ports; j++ ) {
            //fprintf( out, "*S%d%d FREQ " FORM  PHASE "\n", i + 1, j + 1 );
            model_cnt++;
            if ( j + 1 == ports ) {
                (*stream) << QString("A%1%2 %vd(%6 %7) %vd(%3%4, %5) xfer%8\n")
                        .arg(i+1).arg(j+1).arg(i+1).arg(j+1).arg(ports + 1).arg(10 * (j + 1))
                         .arg(ports + 1).arg(model_cnt);
            } else {
                (*stream) << QString("A%1%2 %vd(%7 %8) %vd(%3%4, %5%6) xfer%9\n")
                        .arg(i + 1).arg(j + 1).arg(i + 1).arg(j + 1).arg(i + 1)
                        .arg(j + 2).arg(10 * (j + 1)).arg(ports + 1).arg(model_cnt);
            }
            (*stream) << QString(".model xfer%1 xfer R_I=true table=[\n").arg(model_cnt);

            offset = 0;
            for ( f = 0; f < numf; f++ )
            {
                double a = s[f][i][j][0];
                double b = s[f][i][j][1];
                if (RI) {
                    mag = a;
                    ph = b;
                } else {
                    if ( dB )
                        a = pow(10, a / 20.0);
                    ph = a * sin(b * pi / 180);
                    mag = a * cos(b * pi / 180);
                }
                (*stream) << QString("+ %1Hz %2 %3\n").arg(freqs[f] * funits).arg(mag).arg(ph + offset);
            }
            (*stream) << "+ ]\n\n";
        }
    }

    (*stream) << ".ENDS\n";

    delete [] s;

    return true;

}
