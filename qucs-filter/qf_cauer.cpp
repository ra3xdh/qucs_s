/***************************************************************************
                               qf_cauer.cpp
                             ----------------
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

// Elliptic (Cauer) filters, odd order

#include "qf_cauer.h"
#include "qf_elliptic_functions.h"
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

namespace qf {
cauer::cauer(qf_float amin, qf_float amax, qf_float fc, qf_float fs,
             qf_float imp = 1, qf_float bw = 0, qft type = LOWPASS,
             bool is_tee = false)
    : filter(CAUER, type, imp, fc, bw, is_tee), a_(NULL) {
  if (amin > amax || (amin > 3 || amax < 3) ||
      ((fc > fs && type_ == LOWPASS) || (fc < fs && type_ == HIGHPASS)) ||
      ((type_ == BANDPASS || type_ == BANDSTOP) &&
       std::abs(fs - (fc * fc) / fs) < bw)) {
    return;
  }

  normalize(amin, amax, fs);
  xfer();
  values();
  synth();
}

cauer::~cauer(void) {
  if (a_ != NULL) {
    delete[] a_;
  }
}

// Normalize the filter parameters to Z = 1 O and w = 1 rad/s
// and computes order
void cauer::normalize(qf_float amin, qf_float amax, qf_float fs) {
  qf_float Amax  = pow(10, -amin / 10);
  qf_float Amin  = pow(10, -amax / 10);
  qf_float Aemax = 1 - Amin;
  qf_float Aemin = 1 - Amax;
  qf_float sAmin = -10 * log10(Aemax) + amin;
  qf_float sAmax = -10 * log10(Aemin) + amax;
  qf_float sdiff = sAmax - sAmin;
  qf_float kA    = pow(10, -sdiff / 20);
  qf_float KA;

  if (kA < 0.001) {
    KA = Kp(kA) / K(kA);
  } else {
    KA = K(sqrt(1 - kA * kA)) / K(kA);
  }

  rho_ = sqrt(Aemin);

  switch (type_) {
  case LOWPASS:
    th_ = fc_ / fs;
    break;
  case HIGHPASS:
    th_ = fs / fc_;
    break;
  case BANDPASS:
    th_ = bw_ / std::abs(fs - (fc_ * fc_) / fs);
    break;
  case BANDSTOP:
    th_ = std::abs(fs * bw_ / (fs * fs - fc_ * fc_));
    bw_ = std::abs(fs - (fc_ * fc_) / fs);
    break;
  }

  // Calculate order
  qf_float Kth = K(th_) / K(sqrt(1 - th_ * th_));
  ord_         = (unsigned)ceil(Kth * KA);
  if ((ord_ % 2) == 0) {
    ord_++;
  }

  a_ = new qf_float[ord_ + 1];
}

// A Cauer (or elliptic) filter has a symetric D(O)
// D(O) = F (O) / P (O) = K * O * Prod {(O^2 + a^2(i)) / (a^2(i) * O^2 + 1)}
// So that it is Chebichev in the passband and in the stopband
void cauer::xfer(void) {
  int m       = (ord_ - 1) / 2;
  qf_float Ws = a_[ord_] = sqrt(th_);
  qf_float k             = K(th_);
  int u;

  for (unsigned i = 1; i < ord_; i++) {
    qf_float j = (qf_float)i / (qf_float)ord_;
    a_[i]      = Ws * sn(j * k, th_);
  }

  qf_float delta = 1;
  for (u = 1; u < m + 2; u++) {
    delta *= a_[2 * u - 1] * a_[2 * u - 1];
  }
  delta /= Ws;
  qf_float c = delta * sqrt(1 / (rho_ * rho_) - 1);

  // Computes D
  F_ = poly(1, 0, 0, 1); // F(X) = X
  P_ = poly(c, 0, 0, 0); // P(X) = c

  for (u = 1; u < m + 1; u++) {
    poly MF(1, 0, a_[2 * u] * a_[2 * u], 2);
    poly MP(a_[2 * u] * a_[2 * u], 0, 1, 2);

    MF.disp("MF");
    MP.disp("MP");
    F_ *= MF;
    P_ *= MP;
  }

  F_.disp("F");
  P_.disp("P");

  // E(x) * E(-x) = F(x) * F(-x) + P(x) * P(-x)
  E_ = F_.hsq() + P_.hsq();

  E_.disp("E");
  E_.hurw();
  E_.disp("E");

  BN_ = E_.odd() + F_.odd();
  BD_ = E_.even() - F_.even();
}

void cauer::values(void) {
  n_comp_ = (3 * ord_) / 2;

  // For each zero of transmission, we apply the method as in
  // Saal & Ulbrich p. 288 or Zverev pp. 129 et seq.
  qf_float Ws = sqrt(th_);
  for (unsigned k = 0, l = 2; k < (n_comp_ - 1); k += 3) {
    extract_pole_pCsLC(1 / a_[l], Ws);

    // Zeros mangeling
    l = ord_ - l + 1;
    if (l < (ord_ + 1) / 2) {
      l += 2;
    }
  }
  // Final removal of inifite pole
  subsection final_capa;
  final_capa.wiring  = Wiring::SHUNT;
  final_capa.content = Content::CAPA;
  final_capa.capa_v  = Ws * BN_.eval (1) / BD_.eval (1);
  proto_subsecs_.push_back(final_capa);
}

void cauer::synth() {
  switch (type_) {
  case LOWPASS:
    for (subsection& v : proto_subsecs_) {
      v.transform_lp(subsecs_, imp_, fc_);
    }
    break;
  case HIGHPASS:
    for (subsection& v : proto_subsecs_) {
      v.transform_hp(subsecs_, imp_, fc_);
    }
    break;
  case BANDPASS:
    for (subsection& v : proto_subsecs_) {
      v.transform_bp(subsecs_, imp_, fc_, bw_);
    }
    break;
  case BANDSTOP:
    for (subsection& v : proto_subsecs_) {
      v.transform_bs(subsecs_, imp_, fc_, bw_);
    }
    break;
  }
  if (is_tee_) {
    for (subsection& v : subsecs_) {
      v.pi_tee_switch();
    }
  }
}
} // namespace qf
