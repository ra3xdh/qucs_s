#ifndef ELLIPTIC_FUNCTIONS_H
#define ELLIPTIC_FUNCTIONS_H
#include "qf_poly.h"

namespace qf {
const qf_float SN_ACC = 1e-5;        // Accuracy of sn(x) is SN_ACC^2
const qf_float K_ERR1 = 1e-8;        // Accuracy of K(k)
const qf_float K_ERR2 = 1e-20;       // Accuracy of K(k)
const qf_float K_ERR3 = 1e-6;        // Accuracy of K(k)
const qf_float K_ERR  = 4e-16;       // Accuracy of K (k)clear
const qf_float SK_ERR = sqrt(K_ERR); // Accuracy of sn (u, k)

static qf_float FMAX(qf_float x, qf_float y) {
  return ((x > y) ? x : y);
}

// This is extracted from "Handbook of mathematical functions"
// Edited by M. Abramowitz & I. A. Stegun
// U.S. National Bureau of Standards, June '64 / Dec. '72
// Complete Elliptic integral of the first kind
// K by the arithmetic/geometric mean (AGM) method
qf_float K(qf_float k) {
  qf_float a = 1, b = sqrt(1 - k * k);
  while (std::abs(a - b) > K_ERR) {
    qf_float t = a;
    a          = 0.5 * (a + b);
    b          = sqrt(t * b);
  }
  return pi / (2 * a);
}

double find_th(double Kth) {
  double th_low  = 0.0,
         th_high = 1.0; // Initial guesses for binary search bounds
  double th_mid, ratio;

  while (th_high - th_low > K_ERR) {
    th_mid = 0.5 * (th_low + th_high);
    ratio  = K(th_mid) / K(sqrt(1.0 - th_mid * th_mid));

    if (ratio >
        Kth) { // If the calculated ratio is too high, need smaller theta
      th_high = th_mid;
    } else {
      th_low = th_mid; // If the calculated ratio is too low, need larger theta
    }
  }
  return th_mid;
}

double inv_K(double K_result) {
  double k_low = 0.0, k_high = 1.0; // Initial guesses for binary search bounds
  double k_mid, K_mid;

  while (k_high - k_low > K_ERR) {
    k_mid = 0.5 * (k_low + k_high);
    K_mid = K(k_mid);

    if (K_mid >
        K_result) { // We need a smaller modulus if K(k_mid) is too large
      k_high = k_mid;
    } else {
      k_low = k_mid; // We need a larger modulus if K(k_mid) is too small
    }
  }
  return k_mid;
}

// sn (u, m) by descending Landen transforms
// m = k^2
qf_float sn(qf_float u, qf_float m) {
  if (m < SK_ERR) {
    // Final approx.
    return sin(u) - 0.25 * m * cos(u) * (u - 0.5 * sin(2 * u));
  } else {
    qf_float kp  = sqrt(1 - m);
    qf_float smu = (1 - kp) / (1 + kp);
    qf_float v   = u / (1 + smu);
    // Recurse
    qf_float sn1 = sn(v, smu * smu);
    return (1 + smu) * sn1 / (1 + smu * sn1 * sn1);
  }
}

// Computes elliptic jacobi functions
// Adapted from: Numerical receipes in C, pp. 264 et seq.

// Computes Carlson's elliptic integral of the first kind
qf_float ellip_RF(qf_float x, qf_float y, qf_float z) {
  qf_float alamb, ave, delx, dely, delz, e2, e3;
  qf_float sqrtx, sqrty, sqrtz, xt, yt, zt;

  // Constants
  const qf_float THIRD = 1.0 / 3.0;
  const qf_float C1    = 1.0 / 24.0;
  const qf_float C2    = 0.1;
  const qf_float C3    = 3.0 / 44.0;
  const qf_float C4    = 1.0 / 14.0;

  xt = x;
  yt = y;
  zt = z;
  do {
    sqrtx = sqrt(xt);
    sqrty = sqrt(yt);
    sqrtz = sqrt(zt);
    alamb = sqrtx * (sqrty + sqrtz) + sqrty * sqrtz;
    xt    = 0.25 * (xt + alamb);
    yt    = 0.25 * (yt + alamb);
    zt    = 0.25 * (zt + alamb);
    ave   = THIRD * (xt + yt + zt);
    delx  = (ave - xt) / ave;
    dely  = (ave - yt) / ave;
    delz  = (ave - zt) / ave;
  } while (FMAX(FMAX(std::abs(delx), std::abs(dely)), std::abs(delz)) > K_ERR1);

  e2 = delx * dely - delz * delz;
  e3 = delx * dely * delz;
  return (1 + (C1 * e2 - C2 - C3 * e3) * e2 + C4 * e3) / sqrt(ave);
}

// K(k) = RF(0, 1 - k^2, 1) -> complete elliptic intergral of the 1st kind
qf_float ellip_K(qf_float k) {
  return ellip_RF(0, 1 - k * k, 1);
}

// K'(k) = K(sqrt(1 - k^2)), even for small k's
qf_float Kp(qf_float k) {
  qf_float Kp;
  qf_float f1 = 1, f2, w = 1;
  qf_float kb = 1;

  Kp = f2 = 2 * ln2 - log(k); // K' = ln (4 / k')
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
qf_float ellip_sncndn(qf_float uu, qf_float emmc, qf_float& sn, qf_float& cn,
                      qf_float& dn) {
  qf_float a, b, c, d, emc, u;
  qf_float em[14], en[14];
  int i, ii, l;
  bool bo;

  emc = emmc;
  d   = 1 - emc;
  u   = uu;

  if (emc) {
    bo = (emc < 0);
    if (bo) {
      emc /= -1 / d;
      u *= (d = sqrt(d));
    }
    a  = 1;
    dn = 1;

    for (i = 1; i < 14; i++) {
      l     = i;
      em[i] = a;
      en[i] = (emc = sqrt(emc));
      c     = 0.5 * (a + emc);
      if (std::abs(a - emc) <= SN_ACC * a) {
        break;
      }
      emc *= a;
      a = c;
    }

    u *= c;
    sn = sin(u);
    cn = cos(u);

    if (sn) {
      a = cn / sn;
      c *= a;
      for (ii = l; ii > 0; ii--) {
        b = em[ii];
        a *= c;
        c *= dn;
        dn = (en[ii] + a) / (b + a);
        a  = c / b;
      }
      a  = 1 / sqrt(c * c + 1);
      sn = (sn >= 0 ? a : -a);
      cn = sn * c;
    }

    if (bo) {
      a  = dn;
      dn = cn;
      cn = a;
      sn /= d;
    }
  } else {
    cn = 1 / cosh(u);
    dn = cn;
    sn = tanh(u);
  }
  return sn;
}

qf_float ellip_sn(qf_float x, qf_float k) {
  qf_float sn, cn, dn;
  return ellip_sncndn(x, 1 - k * k, sn, cn, dn);
}
} // namespace qf
#endif // ELLIPTIC_FUNCTIONS_H