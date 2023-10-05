#include <QtCore>
#include <cmath>

#include "misc.h"
#include "s2spice.h"

#define MAXLINE 500
#define MAXFREQS 16001
#define MAXPORTS 8
#define MAXNAME 128


#ifdef PREFER_DB
#define FORM "DB"
#define PHASE " PHASE"
#else
#define FORM "R_I"
#define PHASE
#endif

#define FMT_2 "A%d%d %%vd(%7$d %8$d) %%vd(%3$d%4$d, %5$d%6$d) xfer%9$d\n"

S2Spice::S2Spice()
{
    z0 = -1;
}

QString S2Spice::convertTouchstone()
{

    QString out_str;
    QString next_line;

    double s[MAXFREQS][MAXPORTS][MAXPORTS][2];
    double freqs[MAXFREQS];

    int f;
    int ports;
    int funits = 1;
    int numf;

    double z[MAXPORTS];
    double ph, prevph, offset, mag;


    QFile ff(file);
    if (!ff.open(QIODevice::ReadOnly)) {
        return out_str;
    }
    QTextStream in_stream(&ff);

    /* Find number of ports */
    QFileInfo inf(file);
    ports = inf.suffix().mid(1,1).toInt();
    if ( (ports < 1) || (ports > MAXPORTS) ) {
        //printf( "The circuit has %d ports\n", ports );
        return out_str;
    }


    /* build first line of output file */
    out_str = ".SUBCKT " + device_name;
    for (int i = 0; i < ports; i++) {
        out_str += QString(" %1").arg(i+1);
    }
    out_str += "\n";

    /* read and decode format line */

   while(!in_stream.readLineInto(&next_line)) {
       if (next_line.at(0) == '#') break;
   }

    funits = 1000000000;    /* GHz is the default frequency units */
    if ( next_line.contains(" Hz ") )
        funits = 1;
    else if ( next_line.contains(" kHz ") )
        funits = 1000;
    else if ( next_line.contains(" MHz ") )
        funits = 1000000;
    else if ( next_line.contains(" GHz ") )
        funits = 1000000000;

    bool dB = false;
    bool RI = false;
    if ( next_line.contains(" dB " ) ) {
        dB = true;
    } else if ( next_line.contains(" RI " ) ) {
        RI = true;
    }

    if (!next_line.contains(" S " )) {
        return out_str;
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
        out_str += QString("R%1N %2 %3 %4\n").arg(i+1).arg(i+1).arg(10*(i+1)).arg(-z[i]);
        out_str += QString("R%dP %d %d %e\n").arg(i+1).arg(10*(i+1)).arg(10*(i+1)+1).arg(2*z[i]);
    }
    out_str += "\n";

    /* read S parameters into matrix */

    f = 0;
    while(in_stream.readLineInto(&next_line)) {
        if(next_line.isEmpty()) continue;
        if(next_line.at(0)=='#') continue;
        tmp_lst = next_line.split(" ", qucs::SkipEmptyParts);
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
                out_str +=QString("A%1%2 %%vd(%6 %7) %%vd(%3%4, %5) xfer%8\n")
                        .arg(i+1).arg(j+1).arg(i+1).arg(j+1).arg(ports + 1).arg(10 * (j + 1)).arg(model_cnt);
            } else {
                out_str += QString("A%d%d %%vd(%7 %8) %%vd(%3%4, %5%6) xfer%9\n")
                        .arg(i + 1).arg(j + 1).arg(i + 1).arg(j + 1).arg(i + 1)
                        .arg(j + 2).arg(10 * (j + 1)).arg(ports + 1).arg(model_cnt);
            }
            out_str += QString(".model xfer%1 xfer " FORM "=true table=[\n").arg(model_cnt);
            offset = 0;
            prevph = 0;
            for ( f = 0; f < numf; f++ )
            {
                double a = s[f][i][j][0];
                double b = s[f][i][j][1];
#ifdef PREFER_DB
                if ( dB )
                {
                    mag = a;
                    ph = b;
                }
                else if ( RI )
                {
                    mag = 20* log10( sqrt( pow( a, 2.0 ) + pow( b, 2.0 ) ) );
                    ph = atan2( b, a ) * 180.0 / M_PI;
                }
                else    /* MA is the default */
                {
                    mag = 20 * log10( a );
                    ph = b;
                }

                if ( (absol( ph - prevph )) > 180 )
                {
                    offset = offset - 360.0 * (double) sgn( ph - prevph );
                }
                prevph = ph;
#else // Prefer RI
                if (RI) {
                    mag = a;
                    ph = b;
                } else {
                    if ( dB )
                        mag = pow(10, a / 20.0);
                    ph = mag * sin(ph);
                    mag *= cos(ph);
                }
#endif
                out_str += QString("+ %1Hz %2 %3\n").arg(freqs[f] * funits).arg(mag).arg(ph + offset);
            }
            out_str += "+ ]\n\n";
        }
    }

    out_str += ".ENDS\n";
    return out_str;

}
