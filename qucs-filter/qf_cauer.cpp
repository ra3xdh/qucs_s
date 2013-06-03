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

#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#ifndef M_1_PI
#define M_1_PI    0.3183098861837906715377675267450287
#endif
#ifndef M_LN2
#define M_LN2     0.6931471805599453094172321214581766
#endif
#ifndef M_PI
#define M_PI      3.1415926535897932384626433832795029
#endif

#undef _QF_CAUER_DEBUG
//#define _QF_CAUER_DEBUG 1

#include "qf_poly.h"
#include "qf_filter.h"
#include "qf_cauer.h"

qf_cauer::qf_cauer (unsigned n, qf_double_t r, qf_double_t t) :
  qf_filter (CAUER, LOWPASS, 1, M_1_PI / 2, 0), rho (r) {
  ord = n;
  th = sin (t);
  a = new qf_double_t[ord + 1];
  xfer ();
  values ();
  synth (LOWPASS);
}

qf_cauer::qf_cauer (qf_double_t amin, qf_double_t amax, qf_double_t fc,
		    qf_double_t fs, qf_double_t r = 1, qf_double_t b = 0,
		    qft type = LOWPASS) :
  qf_filter (CAUER, type, r, fc, b), a (NULL) {
  if (amin > amax)
    return;

  if (amin > 3 || amax < 3)
    return;

  if ((fc > fs && type == LOWPASS) || (fc < fs && type == HIGHPASS))
    return;

  if ((type == BANDPASS || type == BANDSTOP) &&
      fabs (fs - (fc * fc) / fs) < bw)
    return;

  normalize (amin, amax, fs, type);
  xfer ();
  values ();
  synth (type);
}

qf_cauer::~qf_cauer (void) {
  if (a != NULL)
    delete[] a;
}

static qf_double_t FMAX (qf_double_t x, qf_double_t y) {
  return ((x > y) ? x : y);
}

// This is extracted from "Handbook of mathematical functions"
// Edited by M. Abramowitz & I. A. Stegun
// U.S. National Bureau of Standards, June '64 / Dec. '72

// K by the arithmetic/geometric mean (AGM) method
qf_double_t qf_cauer::K (qf_double_t k) {
  qf_double_t a = 1, b = sqrt (1 - k * k);
  while (fabs (a - b) > K_ERR) {
    qf_double_t t = a;
    a = 0.5 * (a + b);
    b = sqrt (t * b);
  }
  return M_PI / (2 * a);
}

// sn (u, m) by descending Landen transforms
// m = k^2
qf_double_t qf_cauer::sn (qf_double_t u, qf_double_t m) {
  if (m < SK_ERR) {
    // Final approx.
    return sin (u) - 0.25 * m * cos (u) * (u - 0.5 * sin (2 * u));
  } else {
    qf_double_t kp = sqrt (1 - m);
    qf_double_t smu = (1 - kp) / (1 + kp);
    qf_double_t v = u / (1 + smu);
    // Recurse
    qf_double_t sn1 = sn (v, smu * smu);
    return (1 + smu) * sn1 / (1 + smu * sn1 * sn1);
  }
}

// Computes elliptic jacobi functions
// Adapted from: Numerical receipes in C, pp. 264 et seq.

// Computes Carlson's elliptic integral of the first kind
qf_double_t qf_cauer::ellip_RF (qf_double_t x, qf_double_t y, qf_double_t z) {
  qf_double_t alamb, ave, delx, dely, delz, e2, e3;
  qf_double_t sqrtx, sqrty, sqrtz, xt, yt, zt;

  // Constants
  const qf_double_t THIRD = 1.0 / 3.0;
  const qf_double_t C1 = 1.0 / 24.0;
  const qf_double_t C2 = 0.1;
  const qf_double_t C3 = 3.0 / 44.0;
  const qf_double_t C4 = 1.0 / 14.0;

  xt = x;
  yt = y;
  zt = z;
  do {
    sqrtx = sqrt (xt);
    sqrty = sqrt (yt);
    sqrtz = sqrt (zt);
    alamb = sqrtx * (sqrty + sqrtz) + sqrty * sqrtz;
    xt = 0.25 * (xt + alamb);
    yt = 0.25 * (yt + alamb);
    zt = 0.25 * (zt + alamb);
    ave = THIRD * (xt + yt + zt);
    delx = (ave - xt) / ave;
    dely = (ave - yt) / ave;
    delz = (ave - zt) / ave;
  }
  while (FMAX (FMAX (fabs (delx), fabs (dely)), fabs (delz)) > K_ERR1);

  e2 = delx * dely - delz * delz;
  e3 = delx * dely * delz;
  return (1 + (C1 * e2 - C2 - C3 * e3) * e2 + C4 * e3) / sqrt (ave);
}

// K(k) = RF(0, 1 - k^2, 1) -> complete elliptic intergral of the 1st kind
qf_double_t qf_cauer::ellip_K (qf_double_t k) {
  return ellip_RF (0, 1 - k * k, 1);
}

// K'(k) = K(sqrt(1 - k^2)), even for small k's
qf_double_t qf_cauer::Kp (qf_double_t k) {
  qf_double_t Kp;
  qf_double_t f1 = 1, f2, w = 1;
  qf_double_t kb = 1;

  Kp = f2 = 2 * M_LN2 - log (k);	// K' = ln (4 / k')
  while (kb > K_ERR2) {
    kb *= k * k;
    f1 *= (w / (w + 1));
    f2 -= 2 / (w * (w + 1));
    Kp += f1 * f2 * kb;
    w += 2;
  }
  return Kp;
}

// Compute the Jacobian elliptic functions sn(u,k), cn(u,k) and dn(u,k).
qf_double_t
qf_cauer::ellip_sncndn (qf_double_t uu, qf_double_t emmc,
			qf_double_t& sn, qf_double_t& cn, qf_double_t& dn) {
  qf_double_t a, b, c, d, emc, u;
  qf_double_t em[14], en[14];
  int i, ii, l;
  bool bo;

  emc = emmc;
  d = 1 - emc;
  u = uu;

  if (emc) {
    bo = (emc < 0);
    if (bo) {
      emc /= -1 / d;
      u *= (d = sqrt (d));
    }
    a = 1;
    dn = 1;

    for (i = 1; i < 14; i++) {
      l = i;
      em[i] = a;
      en[i] = (emc = sqrt (emc));
      c = 0.5 * (a + emc);
      if (fabs (a - emc) <= SN_ACC * a)
	break;
      emc *= a;
      a = c;
    }

    u *= c;
    sn = sin (u);
    cn = cos (u);

    if (sn) {
      a = cn / sn;
      c *= a;
      for (ii = l; ii > 0; ii--) {
	b = em[ii];
	a *= c;
	c *= dn;
	dn = (en[ii] + a) / (b + a);
	a = c / b;
      }
      a = 1 / sqrt (c * c + 1);
      sn = (sn >= 0 ? a : -a);
      cn = sn * c;
    }

    if (bo) {
      a = dn;
      dn = cn;
      cn = a;
      sn /= d;
    }
  } else {
    cn = 1 / cosh (u);
    dn = cn;
    sn = tanh (u);
  }
  return sn;
}

qf_double_t qf_cauer::ellip_sn (qf_double_t x, qf_double_t k) {
  qf_double_t sn, cn, dn;
  return ellip_sncndn (x, 1 - k * k, sn, cn, dn);
}

// Arc sin in degrees
static qf_double_t ASIND (qf_double_t ang) {
  return (180 * asin (ang) / M_PI);
}

// Normalize the filter parameters to Z = 1 O and w = 1 rad/s
// and computes order
void qf_cauer::normalize (qf_double_t amin, qf_double_t amax,
			  qf_double_t fs, qft type) {
  qf_double_t Amax = pow (10, -amin / 10);
  qf_double_t Amin = pow (10, -amax / 10);
  qf_double_t Aemax = 1 - Amin;
  qf_double_t Aemin = 1 - Amax;
  qf_double_t sAmin = -10 * log10 (Aemax) + amin;
  qf_double_t sAmax = -10 * log10 (Aemin) + amax;
  qf_double_t sdiff = sAmax - sAmin;

#ifdef _QF_CAUER_DEBUG
  std::cout << "amin + aemin = " << sAmin << " dB\n";
  std::cout << "amax + aemax = " << sAmax << " dB\n";
  std::cout << "D(a) = " << sdiff << " dB\n";
#endif

  qf_double_t kA = pow (10, -sdiff / 20);
  qf_double_t KA;

  if (kA < 0.001)
    KA = Kp (kA) / K (kA);
  else
    KA = K (sqrt (1 - kA * kA)) / K (kA);

  rho = sqrt (Aemin);

  switch (type) {
  case LOWPASS:
    th = fc / fs;
    break;
  case HIGHPASS:
    th = fs / fc;
    break;
  case BANDPASS:
    th = bw / fabs (fs - (fc * fc) / fs);
    break;
  case BANDSTOP :
    th = fabs (fs * bw / (fs * fs - fc * fc));
    bw = fabs (fs - (fc * fc) / fs);
    break;
  }

  // Calculate order
  qf_double_t Kth = K (th) / K (sqrt (1 - th * th));
  ord = (unsigned) ceil (Kth * KA);
  if ((ord % 2) == 0)
    ord++;

#ifdef _QF_CAUER_DEBUG
  std::cout << "K'/K = " << Kth << ", K1/K'1 = " << KA << '\n';
  std::cout << "Order = " << ord << '\n';
#endif

  a = new qf_double_t[ord + 1];
}

// A Cauer (or elliptic) filter has a symetric D(O)
// D(O) = F (O) / P (O) = K * O * Prod {(O^2 + a^2(i)) / (a^2(i) * O^2 + 1)}
// So that it is Chebichev in the passband and in the stopband
void qf_cauer::xfer (void) {
  int m = (ord - 1) / 2;
  qf_double_t Ws = a[ord] = sqrt (th);
  qf_double_t k = K (th);
  int u;

#ifdef _QF_CAUER_DEBUG
  std::cerr << "Computing filter of order " << ord << " with ";
  std::cerr << "rho = " << rho << " and theta = " << ASIND (th) << "°\n";
  std::cerr << "k = " << k << '\n';
#endif

  for (unsigned i = 1; i < ord; i++) {
    qf_double_t j = (qf_double_t) i / (qf_double_t) ord;
    a[i] = Ws * sn (j * k, th);
#ifdef _QF_CAUER_DEBUG
    std::cerr << "a(" << i << ") = " << a[i] << '\n';
#endif
  }

#ifdef _QF_CAUER_DEBUG
  std::cerr << "Norm. puls. (Ws) = " << Ws << '\n';
#endif

  qf_double_t delta = 1;
  for (u = 1; u < m + 2; u++)
    delta *= a[2 * u - 1] * a[2 * u - 1];
  delta /= Ws;
  qf_double_t c = delta * sqrt (1 / (rho * rho) - 1);

#ifdef _QF_CAUER_DEBUG
  std::cerr << "D = " << delta << '\n';
  std::cerr << "c = " << c << '\n';
#endif

  // Computes D
  F = qf_poly (1, 0, 0, 1);	// F(X) = X
  P = qf_poly (c, 0, 0, 0);	// P(X) = c

  for (u = 1; u < m + 1; u++) {
    qf_poly MF (1, 0, a[2 * u] * a[2 * u], 2);
    qf_poly MP (a[2 * u] * a[2 * u], 0, 1, 2);

    MF.disp ("MF");
    MP.disp ("MP");
    F *= MF;
    P *= MP;
  }

  F.disp ("F");
  P.disp ("P");

  // E(x) * E(-x) = F(x) * F(-x) + P(x) * P(-x)
  E = F.hsq () + P.hsq ();

  E.disp ("E");
  E.hurw ();
  E.disp ("E");

  BN = E.odd () + F.odd ();
  BD = E.even () - F.even ();
}

void qf_cauer::values (void) {

  ncomp = (3 * ord) / 2;
  Comp = (qfc *) malloc (sizeof (qfc) * ncomp);

  // For each zero of transmission, we apply the method as in 
  // Saal & Ulbrich p. 288 or Zverev pp. 129 et seq.
  qf_double_t Ws = sqrt (th);
  for (unsigned k = 0, l = 2; k < (ncomp - 1); k += 3) {
#ifdef _QF_CAUER_DEBUG
    std::cerr << "Pole (" << l << ") = " << (1 / (a[l] * Ws)) << "\n";
#endif
    extract_pole_pCsLC (1 / a[l], &Comp[k], Ws);

    // Zeros mangeling 
    l = ord - l + 1;
    if (l < (ord + 1) / 2)
      l += 2;
  }

  // Final removal of inifite pole
  Comp[ncomp - 1].val = Ws * BN.eval (1) / BD.eval (1);
}

void qf_cauer::synth (qft type) {
  qf_double_t cnrm = 1 / (2 * M_PI * fc * imp);
  qf_double_t lnrm = imp / (2 * M_PI * fc);
  unsigned i, node;

  switch (type) {
  case LOWPASS:
    for (i = 0, node = 1;;) {
      Comp[i].comp = CAP;	// Parallel capa first
      Comp[i].val *= cnrm;	// and last !
      Comp[i].node1 = node;
      Comp[i].node2 = 0;
      i++;
      if (i == ncomp)
	break;
      Comp[i].comp = CAP;	// Resonator cap
      Comp[i].val *= cnrm;
      Comp[i].node1 = node;
      Comp[i].node2 = node + 1;
      i++;
      Comp[i].comp = IND;
      Comp[i].val *= lnrm;
      Comp[i].node1 = node;
      Comp[i].node2 = ++node;
      i++;
    }
    break;
  case HIGHPASS:
    // First comes a serial cap
    Comp[0].comp = CAP;
    Comp[0].node1 = 1;
    Comp[0].node2 = 2;
    Comp[0].val = cnrm / Comp[0].val;
    for (i = 1, node = 2; i < ncomp;) {
      // Then a serial resonant circuit
      Comp[i].comp = CAP;
      Comp[i].node1 = node;
      Comp[i].node2 = node + 1;
      Comp[i].val = cnrm / Comp[i].val;
      i++;
      Comp[i].comp = IND;
      Comp[i].node1 = node + 1;
      Comp[i].node2 = 0;
      Comp[i].val = lnrm / Comp[i].val;
      i++;
      // Then another serial cap
      Comp[i].comp = CAP;
      Comp[i].node1 = node;
      node += 2;
      Comp[i].node2 = node;
      Comp[i].val = cnrm / Comp[i].val;
      i++;
    }
    break;
  case BANDPASS: {
    // We double the number of components
    ncomp = 3 * ord - 1;
    qfc * Comp2 = (qfc *) malloc (sizeof (qfc) * ncomp);
    qf_double_t q = fc / bw;

    for (unsigned i = 0, j = 0, node = 1;;) {
      Comp2[j].comp = CAP;
      Comp2[j].node1 = node;
      Comp2[j].node2 = 0;
      Comp2[j++].val = Comp[i].val * q * cnrm;

      Comp2[j].comp = IND;
      Comp2[j].node1 = node;
      Comp2[j].node2 = 0;
      Comp2[j++].val = lnrm / (q * Comp[i++].val);
      if (j == ncomp)
	break;

      qf_double_t c = Comp[i].val;
      qf_double_t l = Comp[i + 1].val;
      qf_double_t iw2 = l * c;
#ifdef _QF_CAUER_DEBUG
      std::cout << "O(inf) = " << sqrt (1 / iw2) << '\n';
#endif
      qf_double_t b = sqrt (1 + 4 * q * q * iw2);

#ifdef _QF_CAUER_DEBUG
      std::cout << "b = " << b << '\n';
#endif

      Comp2[j + 1].comp = IND;
      Comp2[j + 1].node1 = node;
      Comp2[j + 1].node2 = node + 1;
      Comp2[j + 1].val = (b - 1) / (q * c * 2 * b);

      Comp2[j + 3].comp = IND;
      Comp2[j + 3].node1 = node + 1;
      Comp2[j + 3].node2 = node + 2;
      Comp2[j + 3].val = (b + 1) / (q * c * 2 * b);

      Comp2[j].comp = CAP;
      Comp2[j].node1 = node;
      Comp2[j].node2 = node + 1;
      Comp2[j].val = 1 / Comp2[j + 3].val;

      Comp2[j + 2].comp = CAP;
      Comp2[j + 2].node1 = node + 1;
      Comp2[j + 2].node2 = node + 2;
      Comp2[j + 2].val = 1 / Comp2[j + 1].val;
      Comp2[j].val *= cnrm;
      Comp2[j + 2].val *= cnrm;
      Comp2[j + 1].val *= lnrm;
      Comp2[j + 3].val *= lnrm;
      j += 4;
      i += 2;
      node += 2;
    }

    delete[] Comp;
    Comp = Comp2;
    break;
  }
  case BANDSTOP: {
    ncomp = 3 * ord - 1;
    qfc * Comp2 = (qfc *) malloc (sizeof (qfc) * ncomp);
    qf_double_t q = fc / bw;

    for (unsigned i = 0, j = 0, node = 1;;) {
      Comp2[j].comp = CAP;
      Comp2[j].node1 = node;
      Comp2[j].node2 = node + 1;
      Comp2[j++].val = cnrm * Comp[i].val / q;

      Comp2[j].comp = IND;
      Comp2[j].node1 = node + 1;
      Comp2[j].node2 = 0;
      Comp2[j++].val = lnrm * q / Comp[i++].val;
      if (j == ncomp)
	break;

      qf_double_t c = Comp[i].val;
      qf_double_t l = Comp[i + 1].val;
      qf_double_t w2 = 1 / (l * c);
#ifdef _QF_CAUER_DEBUG
      std::cout << "O(inf) = " << sqrt (w2) << "; q = " << q << '\n';
#endif
      qf_double_t b = sqrt (1 + 4 * q * q * w2);

#ifdef _QF_CAUER_DEBUG
      std::cout << "b = " << b << '\n';
#endif
      Comp2[j + 1].comp = IND;
      Comp2[j + 1].node1 = node;
      Comp2[j + 1].node2 = node + 2;
      Comp2[j + 1].val = l * (b - 1) / (2 * b * q);

      Comp2[j + 3].comp = IND;
      Comp2[j + 3].node1 = node + 2;
      Comp2[j + 3].node2 = node + 3;
      Comp2[j + 3].val = l * (b + 1) / (2 * b * q);

      Comp2[j].comp = CAP;
      Comp2[j].node1 = node;
      Comp2[j].node2 = node + 2;
      Comp2[j].val = 1 / Comp2[j + 3].val;

      Comp2[j + 2].comp = CAP;
      Comp2[j + 2].node1 = node + 2;
      Comp2[j + 2].node2 = node + 3;
      Comp2[j + 2].val = 1 / Comp2[j + 1].val;
      Comp2[j].val *= cnrm;
      Comp2[j + 2].val *= cnrm;
      Comp2[j + 1].val *= lnrm;
      Comp2[j + 3].val *= lnrm;
      j += 4;
      i += 2;
      node += 3;
    }

    delete[] Comp;
    Comp = Comp2;
    break;
  }
  }
}

void qf_cauer::dump () {
  if (Comp == NULL)
    return;			// Not initialized

  std::cout << "qf_cauer::dump ()\n";

  switch (type) {
  case LOWPASS:
    std::cout << "Lowpass ";
    break;
  case HIGHPASS:
    std::cout << "Highpass ";
    break;
  case BANDPASS:
    std::cout << "Bandpass ";
    break;
  case BANDSTOP:
    std::cout << "Bandstop ";
    break;
  }
  std::cout << "of order " << ord << ", theta = "
	    << ASIND (th) << "\x00B0, rho = " << rho << '\n';
  dump_cout ();
}

void CC (void) {
  unsigned o;
  qf_double_t t, r;

  do {
    std::cout << "Order : ";
    std::cin >> o;
    if (o == 0)
      return;
    std::cout << "Reflexion (%) : ";
    std::cin >> r;
    r /= 100.0;
    std::cout << "Angle (\x00B0) : ";
    std::cin >> t;
    t = M_PI * t / 180.0;
    qf_cauer F (o, r, t);
    F.dump ();
  }
  while (true);
}

#if TEST
int main (void) {
  qf_double_t amin, amax, fc, fs, bw, r;

  while (true) {
    std::cout << "Enter cutoff (Hz) [0 to end]: ";
    std::cin >> fc;
    if (fc == 0)
      return 0;
    std::cout << "Enter ripple in passband (dB): ";
    std::cin >> amin;
    std::cout << "Enter stopband frequency (Hz): ";
    std::cin >> fs;
    std::cout << "Enter minimal attenuation in stopband (dB): ";
    std::cin >> amax;
    std::cout << "Enter bandwith (0 for high- or low-pass) (Hz): ";
    std::cin >> bw;
    std::cout << "Enter reference impedance (Ohm): ";
    std::cin >> r;

    if (bw == 0) {
      if (fs > fc) {
	qf_cauer F (amin, amax, fc, fs, r, 0, LOWPASS);
	F.dump ();
      }
      else {
	qf_cauer F (amin, amax, fc, fs, r, 0, HIGHPASS);
	F.dump ();
      }
    }

    else {
      if (amin < amax) {
	qf_cauer F (amin, amax, fc, fs, r, bw, BANDPASS);
	F.dump ();
      }
      else {
	qf_cauer F (amax, amin, fc, fs, r, bw, BANDSTOP);
	F.dump ();
      }
    }
  }
}
#endif /* TEST */
