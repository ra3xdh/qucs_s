/***************************************************************************
                               qf_filter.cpp
                             -----------------
    begin                : Mon Jan 02 2006
    copyright            : (C) 2006 by Vincent Habchi, F5RCS
    email                : 10.50@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <cmath>
#include <iostream>
#include <sstream>
#include <stdlib.h>

#include "qucsfilter.h"
#include "qf_filter.h"
#include "../qucs/extsimkernels/spicecompat.h"

namespace qf {

filter::filter(qfk kind, qft ttype, qf_float imp, qf_float fc = 1,
               qf_float bw = 0, bool is_tee = false)
    : type_(ttype), kind_(kind), is_tee_(is_tee), fc_(fc), bw_(bw), imp_(imp),
      n_comp_(0) {}

// Destructor of a filter
filter::~filter(void) {
}

// Extraction routines

// Extract finite attenuation pole
// Result is a parallel cap, and a serial resonator (L // C)
void filter::extract_pole_pCsLC(qf_float p, qf_float Ws) {
  qf_float comp[3];
  BN_.disp("BN");
  BD_.disp("BD");

  qf_float pl   = -p * p;
  qf_float bdpl = BD_.evalX2(pl);

  // Partial removal of infinite pole (first // cap)
  // c = [B(s)/s] (s^2 = - O^2)
  qf_float c = ((BN_ << 1).evalX2(pl)) / bdpl;
  comp[0] = c * Ws;
  poly cS(c, 0, 0, 1);
  BN_ = BN_ - (BD_ * cS); // B = B - cs
  BN_.disp("BN");
  BN_.div(0, p);

  // Full removal of finite pole
  // c1 = (s B(s) / (s^2 + O^2)) @ s^2 = - O^2
  BN_.disp("BN");
  qf_float c1      = (BN_ >> 1).evalX2(pl) / bdpl;
  comp[1] = c1;
  comp[2] = -Ws / (c1 * pl);
  comp[1] *= Ws;

  // 1/B = 1/B - (s/c1) / (s^2 + O^2)
  BD_ = BD_ - (BN_ >> 1) * (1 / c1);
  BD_.div(0, p);

  BN_.disp("BN");
  BD_.disp("BD");
  subsection fst_subsec;
  fst_subsec.wiring  = Wiring::SHUNT;
  fst_subsec.content = Content::CAPA;
  fst_subsec.capa_v  = comp[0];
  proto_subsecs_.push_back(fst_subsec);
  subsection snd_subsec;
  snd_subsec.wiring  = Wiring::SERIES;
  snd_subsec.content = Content::PARA_CAPA_INDUC;
  snd_subsec.capa_v  = comp[1];
  snd_subsec.indc_v  = comp[2];
  proto_subsecs_.push_back(snd_subsec);
}

// User readable value string.
QString filter::num2str(qf_float num) {
  char c       = 0;
  qf_float cal = std::abs(num);
  if (cal > 1e-20) {
    cal = log10(cal) / 3.0;
    if (cal < -0.2) {
      cal -= 0.98;
    }
    int expo = int(cal);

    if (expo >= -5) {
      if (expo <= 4) {
        switch (expo) {
        case -5:
          c = 'f';
          break;
        case -4:
          c = 'p';
          break;
        case -3:
          c = 'n';
          break;
        case -2:
          c = 'u';
          break;
        case -1:
          c = 'm';
          break;
        case 1:
          c = 'k';
          break;
        case 2:
          c = 'M';
          break;
        case 3:
          c = 'G';
          break;
        case 4:
          c = 'T';
          break;
        }
      }
    }
    if (c) {
      num /= pow(10.0, (qf_float)(3 * expo));
    }
  }
  QString str = QString::number((double)num);
  if (c) {
    str.append(c);
  }
  return str;
}

QString filter::to_qucs() {
  QString compos = "";
  QString wires  = "";
  int space      = 100;
  int x          = 0;
  // Draw left power source

  compos += QStringLiteral("<Pac P1 1 %1 290 18 -26 0 1 \"1\" 1 \"%2 Ohm\" 1 \"0 "
                    "dBm\" 0 \"1 GHz\" 0>\n")
                .arg(x)
                .arg((double)imp_);
  compos += QStringLiteral("<GND * 1 %1 320 0 0 0 0>\n").arg(x);
  wires += QStringLiteral("<0 200 %1 200 \"\" 0 0 0>\n").arg(space / 2);
  wires += QStringLiteral("<%1 200 %1 260 \"\" 0 0 0>\n").arg(x);
  // Draw filter sections
  for (auto& subsec : subsecs_) {
    x += space;
    if (subsec.wiring == SERIES) {
      if (subsec.content == INDUC) {
        wires +=
            QStringLiteral("<%1 200 %2 200 0 0 0>\n").arg(x - space / 2).arg(x - 30);
        compos += QStringLiteral("<L L1 1 %1 %2 -26 -50 0 0 \"%3H\" 1>\n")
                      .arg(x)
                      .arg(200)
                      .arg(num2str(subsec.indc_v));
        wires +=
            QStringLiteral("<%1 200 %2 200 0 0 0>\n").arg(x + 30).arg(x + space / 2);
      } else if (subsec.content == CAPA) {
        wires +=
            QStringLiteral("<%1 200 %2 200 0 0 0>\n").arg(x - space / 2).arg(x - 30);
        compos += QStringLiteral("<C C1 1 %1 %2 -26 -50 0 0 \"%3F\" 1>\n")
                      .arg(x)
                      .arg(200)
                      .arg(num2str(subsec.capa_v));
        wires +=
            QStringLiteral("<%1 200 %2 200 0 0 0>\n").arg(x + 30).arg(x + space / 2);
      } else if (subsec.content == PARA_CAPA_INDUC) {
        wires +=
            QStringLiteral("<%1 200 %2 200 0 0 0>\n").arg(x - space / 2).arg(x - 30);
        compos += QStringLiteral("<C C1 1 %1 %2 -26 -80 0 0 \"%3F\" 1>\n")
                      .arg(x)
                      .arg(200)
                      .arg(num2str(subsec.capa_v));
        wires += QStringLiteral("<%1 %2 %1 %3 0 0 0>\n").arg(x - 30).arg(200).arg(170);
        wires += QStringLiteral("<%1 %2 %1 %3 0 0 0>\n").arg(x + 30).arg(200).arg(170);
        compos += QStringLiteral("<L L1 1 %1 %2 -26 -90 0 0 \"%3H\" 1>\n")
                      .arg(x)
                      .arg(170)
                      .arg(num2str(subsec.indc_v));
        wires +=
            QStringLiteral("<%1 200 %2 200 0 0 0>\n").arg(x + 30).arg(x + space / 2);
      } else if (subsec.content == SERIES_CAPA_INDUC) {
        wires +=
            QStringLiteral("<%1 200 %2 200 0 0 0>\n").arg(x - space / 2).arg(x - 30);
        compos += QStringLiteral("<C C1 1 %1 %2 -26 -50 0 0 \"%3F\" 1>\n")
                      .arg(x)
                      .arg(200)
                      .arg(num2str(subsec.capa_v));
        wires +=
            QStringLiteral("<%1 200 %2 200 0 0 0>\n").arg(x + 30).arg(x + space / 2);
        x += space;
        wires +=
            QStringLiteral("<%1 200 %2 200 0 0 0>\n").arg(x - space / 2).arg(x - 30);
        compos += QStringLiteral("<L L1 1 %1 %2 -26 -50 0 0 \"%3H\" 1>\n")
                      .arg(x)
                      .arg(200)
                      .arg(num2str(subsec.indc_v));
        wires +=
            QStringLiteral("<%1 200 %2 200 0 0 0>\n").arg(x + 30).arg(x + space / 2);
      }
    } else if (subsec.wiring == SHUNT) {
      if (subsec.content == INDUC) {
        wires += QStringLiteral("<%1 200 %2 200 0 0 0>\n")
                     .arg(x - space / 2)
                     .arg(x + space / 2);
        wires += QStringLiteral("<%1 %2 %1 %3 0 0 0>\n").arg(x).arg(200).arg(260);
        compos += QStringLiteral("<L L1 1 %1 %2 17 -26 0 1 \"%3H\" 1>\n")
                      .arg(x)
                      .arg(290)
                      .arg(num2str(subsec.indc_v));
        compos += QStringLiteral("<GND * 1 %1 320 0 0 0 0>\n").arg(x);
      } else if (subsec.content == CAPA) {
        wires += QStringLiteral("<%1 200 %2 200 0 0 0>\n")
                     .arg(x - space / 2)
                     .arg(x + space / 2);
        wires += QStringLiteral("<%1 %2 %1 %3 0 0 0>\n").arg(x).arg(200).arg(260);
        compos += QStringLiteral("<C C1 1 %1 %2 17 -26 0 1 \"%3F\" 1>\n")
                      .arg(x)
                      .arg(290)
                      .arg(num2str(subsec.capa_v));
        compos += QStringLiteral("<GND * 1 %1 320 0 0 0 0>\n").arg(x);
      } else if (subsec.content == SERIES_CAPA_INDUC) {
        wires += QStringLiteral("<%1 200 %2 200 0 0 0>\n")
                     .arg(x - space / 2)
                     .arg(x + space / 2);
        compos += QStringLiteral("<L L1 1 %1 %2 17 -26 0 1 \"%3H\" 1>\n")
                      .arg(x)
                      .arg(230)
                      .arg(num2str(subsec.indc_v));
        compos += QStringLiteral("<C C1 1 %1 %2 17 -26 0 1 \"%3F\" 1>\n")
                      .arg(x)
                      .arg(290)
                      .arg(num2str(subsec.capa_v));
        compos += QStringLiteral("<GND * 1 %1 320 0 0 0 0>\n").arg(x);
      } else if (subsec.content == PARA_CAPA_INDUC) {
        wires += QStringLiteral("<%1 200 %2 200 0 0 0>\n")
                     .arg(x - space / 2)
                     .arg(x + space / 2);
        wires += QStringLiteral("<%1 %2 %1 %3 0 0 0>\n").arg(x).arg(200).arg(260);
        compos += QStringLiteral("<L L1 1 %1 %2 47 16 0 1 \"%3H\" 1>\n")
                      .arg(x)
                      .arg(290)
                      .arg(num2str(subsec.indc_v));
        wires += QStringLiteral("<%1 260 %2 260 0 0 0>\n").arg(x).arg(x + 30);
        compos += QStringLiteral("<C C1 1 %1 %2 17 -26 0 1 \"%3F\" 1>\n")
                      .arg(x + 30)
                      .arg(290)
                      .arg(num2str(subsec.capa_v));
        wires += QStringLiteral("<%1 320 %2 320 0 0 0>\n").arg(x).arg(x + 30);
        compos += QStringLiteral("<GND * 1 %1 320 0 0 0 0>\n").arg(x);
      }
    }
  }
  // Draw right power source
  x += space;
  wires += QStringLiteral("<%1 200 %2 200 0 0 0>\n").arg(x - space / 2).arg(x);
  compos += QStringLiteral("<Pac P2 1 %1 290 18 -26 0 1 \"2\" 1 \"%2 Ohm\" 1 \"0 "
                    "dBm\" 0 \"1 GHz\" 0>\n")
                .arg(x)
                .arg((double)imp_);
  compos += QStringLiteral("<GND * 1 %1 320 0 0 0 0>\n").arg(x);
  wires += QStringLiteral("<%1 200 %1 260 \"\" 0 0 0>\n").arg(x);

  QString s = "";
  s += QStringLiteral("<Qucs Schematic " PACKAGE_VERSION ">\n");
  s += "<Components>\n";
  s += compos;

  float Value = fc_ / 10.0;
  float Value2 = 10.0 * fc_;
  s += "<.SP SP1 1 70 " + QString::number(400)+ " 0 50 0 0 \"log\" 1 \"";
  s += num2str(Value) + "Hz\" 1 \"" + num2str(Value2);
  s += "Hz\" 1 \"200\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n";

  QString eqn_string;
  switch (QucsSettings.DefaultSimulator) {
  case spicecompat::simQucsator:
    eqn_string += "<Eqn Eqn1 1 260 " + QString::number(410);
    eqn_string += " -28 15 0 0 \"dBS21=dB(S[2,1])\" 1 ";
    eqn_string += "\"dBS11=dB(S[1,1])\" 1 \"yes\" 0>\n";
    break;
  case spicecompat::simNgspice :
    eqn_string = QStringLiteral("<NutmegEq NutmegEq1 1 260 410 -28 15 0 0 \"SP1\" 1 \"dBS21=dB(S_2_1)\" 1 \"dBS11=dB(S_1_1)\" 1>\n");
    break;
  case spicecompat::simSpiceOpus:
  case spicecompat::simXyce:
  default: break;
  }

  s+= eqn_string;
  
  s += "</Components>\n";
  s += "<Wires>\n";
  s += wires;
  s += "</Wires>\n";
  s += "<Diagrams>\n";
  s += "</Diagrams>\n";
  s += "<Paintings>\n";
  s += "<Text 400 " + QString::number(400) + " 12 #000000 0 \"";

  switch (kind_) {
  case CAUER:
    s += "Cauer ";
    break;
  case BUTT:
    s += "Butterworth ";
    break;
  case CHEB:
    s += "Chebichev ";
    break;
  case ICHEB:
    s += "Inverse Chebichev ";
    break;
  case BESS:
    s += "Bessel ";
    break;
  case UNDEF:
    s += "Undefined ";
    break;
  }
  switch (type_) {
  case LOWPASS:
    s += "low-pass filter\\n" + num2str(fc_) + "Hz cutoff";
    break;
  case HIGHPASS:
    s += "high-pass filter\\n" + num2str(fc_) + "Hz cutoff";
    break;
  case BANDPASS:
    s += "band-pass filter\\n" + num2str(fc_ - bw_ / 2) + "Hz ... " +
         num2str(fc_ + bw_ / 2) + "Hz";
    break;
  case BANDSTOP:
    s += "band-reject filter\\n" + num2str(fc_ - bw_ / 2) + "Hz ... " +
         num2str(fc_ + bw_ / 2) + "Hz";
    break;
  }
  s += is_tee_ ? "\\n Tee-type" : "\\n PI-type";
  s += "\\nimpedance matching " + num2str(imp_) + " Ohm\">\n";
  s += "</Paintings>\n";

  // s = "wl-copy \'" + s + "\'";
  // std::system(s.toStdString().c_str());
  // std::cout << s.toStdString();
  return s;
}
} // namespace qf
