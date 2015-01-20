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

#undef	_QF_CAUER_DEBUG

#include "qf_common.h"
#include "qf_poly.h"
#include "qf_comp.h"
#include "qf_capacity.h"
#include "qf_filter.h"
#include "qf_cauer.h"

static const qf_double_t K_ERR =  4e-16;	// K (k) accuracy
static const qf_double_t SK_ERR = sqrt (K_ERR);	// sn accuracy
static const qf_double_t PL_ERR = 0.001;	// pole accuracy
static const qf_double_t hugedbl = numeric_limits <qf_double_t>::max ();

// Static functions 

// Arc sin in degrees
inline qf_double_t ASIND (qf_double_t ang) {
  return 180 * asin (ang) / pi;
}

// Computes elliptic jacobi functions K and sn
// This is extracted from "Handbook of mathematical functions"
// Edited by M. Abramowitz & I. A. Stegun
// U.S. National Bureau of Standards, June '64 / Dec. '72
// K by the arithmetic/geometric mean (AGM) method

static qf_double_t K (qf_double_t k)
{
  qf_double_t a = 1, b = sqrt (1 - k * k);

  while (fabs (a - b) > K_ERR) {

    qf_double_t temp = a;
    a = 0.5 * (a + b);
    b = sqrt (temp * b);
  }
  return pi / (2 * a);
}

// sn (u, m) by descending Landen transforms
// m = k^2
static qf_double_t sn (qf_double_t u, qf_double_t m) {

  if (m < SK_ERR) {
    // Final approx.
    return sin (u) - 0.25 * m * cos (u) * (u - 0.5 * sin (2 * u));
  }
  else {
    qf_double_t kp = sqrt (1 - m);
    qf_double_t smu = (1 - kp) / (1 + kp);
    qf_double_t v = u / (1 + smu);
    // Recurse
    qf_double_t sn1 = sn (v, smu * smu);
    return (1 + smu) * sn1 / (1 + smu * sn1 * sn1);
  }
}

// K'(k) = K(sqrt(1 - k^2)) , even for small k's
static qf_double_t Kp (qf_double_t k) {

  qf_double_t  Kp;
  qf_double_t  f1 = 1, f2, w = 1;
  qf_double_t  kb = 1;

  Kp = f2 = 2 * ln2 - log (k);	  // K' = ln (4 / k')
  while (kb > K_ERR) {
    kb *= k * k;
    f1 *= (w / (w + 1));
    f2 -= 2 / (w * (w + 1));
    Kp += f1 * f2 * kb;
    w += 2;
  } 
  return Kp;
}

// Calculate the an coefficient of function K = E / P.

static void calc_an (Rvector* a, qf_double_t th, unsigned o) {
  qf_double_t	kth = K(th);
  qf_double_t	sqrth = sqrt (th); 

  for (unsigned n = 1; n < o; n ++) {
    qf_double_t j = (qf_double_t) n / (qf_double_t) o;
    (*a) [n] = sqrth * sn (j * kth, th * th);
#ifdef _QF_CAUER_DEBUG
    std::cout << "a(" << n << ") = " << (*a)[n] << endl;
#endif
  }
  (*a) [o] = sqrth;
# ifdef _QF_CAUER_DEBUG
  std::cout << "a(" << o << ") = " << (*a)[o] << endl;
# endif
}

// Member functions
// Constructor
 
qf_cauer::qf_cauer (qf_spec* Pspec): 
      qf_filter (Pspec), m (0), Ws (0), rho (0), zr (NULL) {
  
  // If we have specified two different impedances, we ignore the ripple given
  // and we proceed with the ripple determined by r1/r2.
  if (Pspec -> r1 != Pspec -> r2)
    rho = fabs (Pspec -> r1 - Pspec -> r2) / (Pspec -> r1 + Pspec -> r2);

  else {
    qf_double_t Amax = pow (10, - Pspec -> amin / 10);
    qf_double_t Aemin = 1 - Amax;
    rho = sqrt (Aemin);
  }

  // Compute order
  if (Pspec -> ord_given) {
    Pspec -> th = sin (Pspec -> ang);
    if (Pspec -> ord % 2) Pspec -> subord = ' ';
  }

  else {
    // We compute a "true" filter, i.e. we determine the order out of the
    // various parameters
    Pspec -> th = 1 / Pspec -> th;

    // fs is either the first pole (infinite attenuation) or the stopband limit
    if (Pspec -> stop_is_pole) {
      if (!iorder ()) return; // Filter too big
    }
    else if (!order ()) return;

    // Only even b filters can have different termination impedances
    if (Pspec -> r1 != Pspec -> r2) {
      if (Pspec -> ord % 2 == 1) Pspec -> ord ++;
      Pspec -> subord = 'b';
      Pspec -> dual = Pspec -> r1 < Pspec -> r2;
    }
    else
      Pspec -> subord = Pspec -> ord % 2 ? ' ' : 'c';
  }

  // Various strange cases
  // There are no order 1, 2 or 4 filter
  if ((Pspec -> subord == 'b') && (Pspec -> ord < 6)) Pspec -> ord = 6;
    else if (Pspec -> ord < 3) Pspec -> ord = 3;
      else if (Pspec -> ord == 4) Pspec -> ord = 5;

  // Compute susceptance

  zr = new Rvector (Pspec -> ord + 1);	// List of poles

# ifdef _QF_CAUER_DEBUG
  std::cout << "Theta: " << Pspec -> th << "\t";
  std::cout << "rho: " << rho << "\t";
# endif

  if ((Pspec -> ord % 2) == 1) scpt_odd ();
  else if (Pspec -> subord == 'b') scpt_b ();
    else scpt_c ();
}

// Given amin, amax, fs and the type, calculates the order of the filter
bool qf_cauer::order (void) {
  // First normalizes
  qf_double_t Amax, Aemin;

  if (rho == 0) {
    Amax = pow (10, - Pspec -> amin / 10);
    Aemin = 1 - Amax;
    rho = sqrt (Aemin);
  }
  else {
    Aemin = rho * rho;
    Amax = 1 - Aemin;
    Pspec -> amin = - 10 * log10 (Amax);
  }
   
  qf_double_t  Amin = pow (10, - Pspec -> amax / 10);
  qf_double_t  Aemax = 1 - Amin;
  qf_double_t  sAmin = -10 * log10 (Aemax) + Pspec -> amin;
  qf_double_t  sAmax = -10 * log10 (Aemin) + Pspec -> amax;
  qf_double_t  sdiff = sAmax - sAmin;

#ifdef _QF_CAUER_DEBUG
  std::cout << "amin + aemin = " << sAmin << " dB\n";
  std::cout << "amax + aemax = " << sAmax << " dB\n";
  std::cout << "delta(a) = " << sdiff << " dB\n";
#endif

  // Computes order
  qf_double_t kA = pow (10, - sdiff / 20);
  qf_double_t KA;

  if (kA < 0.001)
    KA = Kp (kA) / K (kA);
  else
    KA = K (sqrt (1 - kA * kA)) / K (kA);

  // Computes angle out of frequencies and transformation
  qf_double_t kth = K (Pspec -> th);
  qf_double_t Kth = kth / K (sqrt (1 - Pspec -> th * Pspec -> th));

  Pspec -> ord = static_cast <unsigned> (ceil (Kth * KA));

  // If order is odd and we compute a zigzag transform, increase the
  // order by one -- order must be at least 6.
  if (Pspec -> is_zigzag) {
    if (Pspec -> ord % 2 == 1)
      Pspec -> ord ++;

    if (Pspec -> ord < 6) Pspec -> ord = 6;
  }

  if (Pspec -> ord > QF_MAX_ORD) return false;

#ifdef _QF_CAUER_DEBUG
  std::cout << "Computing CC " << Pspec -> ord << ' ' << rho * 100 << ' '
	    << ASIND (Pspec -> th) << endl;
  std::cout << "With k = " << kth << '\n';
#endif
  return true;
}

// Given amin, amax, a pole and the type, calculates the order of the filter

bool qf_cauer::iorder (void) {
  // First normalizes
  qf_double_t Amax, Aemin;

  Amax = pow (10, - Pspec -> amin / 10);
  Aemin = 1 - Amax;
  rho = sqrt (Aemin);
   
#ifdef _QF_CAUER_DEBUG
  std::cout << "rho = " << rho * 100 << " %\n";
#endif

  // Computes order
  // A rough start estimation of the right angle is the standard angle
  // This estimation is obviously too low (the first pole lies in the
  // stopband!)

  qf_double_t thp = Pspec -> th;  // Default

  // Iterates by increasing order, until the attenuation required is reached
  qf_double_t n = 3;
  qf_double_t theta = 1.0;

  while (true) {
    Pspec -> ord = static_cast <unsigned> (n);
    qf_double_t th1 = thp;
    qf_double_t th2 = 1.0;
    qf_double_t ro = (n - 1) / n;
    qf_double_t re = (n - 2) / n;
    qf_double_t p;

    // Now finds the new suitable angle th
    // between thp and the prevoius th, by dichotomy
    // The relative error is controled by the PL_ERR constant

    do {
      theta = 0.5 * (th1 + th2);
      if (Pspec -> ord % 2) {
	// Odd case
      p = theta * sn (ro * K(theta), theta * theta);
      }
      else {
	// Even 'c' type case
	qf_double_t nrm = sn (ro * K(theta), theta * theta);
	p = sqrt (sn (re * K(theta), theta * theta));
	p *= theta * nrm;
      }

      if (p < thp)
	th1 = theta;
      else
	th2 = theta;
    } while (fabs (thp / p - 1) > PL_ERR);

    // Computes max attenuation, and see if we match the criterion
    zr = new Rvector (Pspec -> ord);
    m = (Pspec -> ord + 1) / 2;
    qf_double_t	d = 1.0;

    if (Pspec -> ord % 2) d /= sqrt (theta);
 
    calc_an (zr, theta, Pspec -> ord);
  
    for (unsigned u = 1; u <= m; u++)
      d *= (*zr) [2*u-1] * (*zr) [2*u-1];

    qf_double_t as = 10 *
      log10 (1 + 1 / (d * d * d * d * (1 / (rho * rho) - 1)));

#ifdef	_QF_CAUER_DEBUG
  std::cout << "Order: " << Pspec -> ord << " theta: " << ASIND(theta) << 
               " As: " << as << " Amax: " << Pspec -> amax << '\n';
#endif

    if (as >= Pspec -> amax) break ;

    // If the max attenuation is not sufficient, goes to the next order
    n ++ ;
  }

  if (Pspec -> ord > QF_MAX_ORD) {
    return false; 
  }

#ifdef _QF_CAUER_DEBUG
  std::cout << "Computing CC " << Pspec -> ord << ' ' << rho * 100 << ' '
	    << ASIND (theta) << '\n';
  std::cout << "With k = " << K(theta) << '\n';
#endif

  Pspec -> th = theta;
  return true;
}

//
// This routines synthezise even order filters
//

bool  qf_cauer::synth_even (void) {

  struct qf_trp	trp;
  qf_double_t	v;

  BN = BNc;
  BD = BDc;

  // Clears the component list
  lcmp. clear ();

  for (unsigned k = 0; k < m; k ++) {
    qf_double_t pl;

    if (Pspec -> subord == 'b')	
      pl = (*zr) [2*pole[k] + 3]; // b type pole
    else
      pl = sqrt ((*zr) [2*pole[k] + 2] * (*zr) [2*pole[k] + 4]);

#ifdef _QF_CAUER_DEBUG
    std::cout << "Pole (" << 2 * pole[k] + 3 << ") = " 
	      << (1 / (pl * Ws)) << endl;
#endif
     // Remove finite pole. This produces two normal components
      // one capacitor, one parallel LC for the standard form
      // one inductor, one serial LC for the dual form

      // If we optimize and the procedure spits out a negative component,
      // we abort
    if (pole_finite (1 / pl, Ws, &trp) && Pspec -> optc) return false;

    if (!Pspec -> dual) {
      // Shunt cap (to ground)
      lcmp. insert (new qf_cap (trp. v, true, true));
      // Serial L || C (direct)
      lcmp. insert (new qf_plc (trp. w1, trp. w2, false, false));
    }

    else {
      // Serial ind
      lcmp. insert (new qf_ind (trp. v, false, false));
      // Shunt L + C
      lcmp. insert (new qf_slc (trp. w2, trp. w1, true, true));
    }

    BN.slfeven ();
    BD.slfodd ();
  }

  // Extract final cell
  if (pole_inf_c (v, Ws) && Pspec -> optc) return false;

  if (!Pspec -> dual) {
    lcmp. insert (new qf_cap (v, true, true));
    //  pole_sL (ind, Ws);
    lcmp. insert (new qf_ind (BD.eval (1) * Ws / BN. eval (1), 
		  false, false)); // Hack
  }
  else {
    lcmp. insert (new qf_ind (v, false, false));

    // pole_pC (ind, Ws);
    lcmp. insert (new qf_cap (BD. eval (1) * Ws / BN. eval (1),
		    true, true));
  }

  lcmp. insert (new qf_end());
  return true;
}

bool qf_cauer::do_synth_even (void) {

  scptX1 (F, P, BNc, BDc);	// Calculate susceptance B = BN/BD

  BNc. slfeven ();		// In case of roundoff errors ?
  BDc. slfodd ();

#ifdef _QF_CAUER_DEBUG
  BNc. disp ("BN");
  BDc. disp ("BD");
#endif

  return synth_even ();
}

void qf_cauer::scpt_b (void) {

  // Zeros of transmission
  Rvector	a (Pspec -> ord + 1);
  Rvector	bp (Pspec -> ord + 1);

  m = Pspec -> ord / 2;

  // Calculate poles, zeros and the transmission function

  calc_an (&a, Pspec -> th, Pspec -> ord);

  qf_double_t a12 = a[1] * a[1];
  qf_double_t w = sqrt ((1 - a12 * Pspec -> th) * 
		        (1 - a12 / Pspec -> th));

  for (unsigned n = 1; n <= Pspec -> ord; n ++) {
    bp [n] = sqrt (w / (1 / (a [n] * a [n]) - a12));
    (*zr) [n] = sqrt ((a [n] * a [n] - a12) / w);
  }
  
  qf_double_t delta = 1;
  for (unsigned u = 1; u <= m; u++)
    delta *= bp [2*u - 1] * bp [2*u - 1];

  qf_double_t c = delta * sqrt (1 / (rho * rho) - 1);

  // Normalization Wc = 1
  Ws = sqrt (a [Pspec -> ord] * a [Pspec -> ord - 1]);

#ifdef _QF_CAUER_DEBUG
  std::cout << "Norm. puls. (1/Ws) = " << 1 / Ws << '\n';
  std::cout << "c = " << c << '\n';
#endif

  // Set the rational transmission function D = F/P

  F = qf_poly (1, 0, bp[1] * bp[1]);  // At first, F = x^2 + bp1^2
  P = qf_poly (0, 0, c);	      // and P = c

  for (unsigned u = 2; u <= m; u++) {
    qf_poly MF (1, 0, bp [2*u - 1] * bp [2*u - 1]);
    qf_poly MP ((*zr) [2*u - 1] * (*zr) [2*u - 1], 0, 1);

#ifdef _QF_CAUER_DEBUG
    F. disp ("F");
    P. disp ("P");
    MF. disp ("F'");
    MP. disp ("P'");
#endif
    F *= MF;
    P *= MP;
  }

#ifdef _QF_CAUER_DEBUG
  F. disp ("F");
  P. disp ("P");
#endif
}

//
// This routine synthezises even order 'c' type filters
// Basically, this is the same as above, except for the value of the
// poles which are slightly different.
// Let's recall that 'c' filters have equal termination impedances
//

void qf_cauer::scpt_c (void) {

  m = Pspec -> ord / 2;

  calc_an (zr, Pspec -> th, Pspec -> ord);

#ifdef _QF_CAUER_DEBUG
  std::cout << "theta = " << Pspec -> th << endl;
#endif
  
  qf_double_t delta = 1;
  for (unsigned u = 1; u <= m; u ++)
    delta *= (*zr) [2*u - 1] * (*zr) [2*u - 1];

  qf_double_t c = delta * sqrt (1 / (rho * rho) - 1);
  Ws = (*zr) [Pspec -> ord - 1];	// Normalisation, Wc = 1

#ifdef _QF_CAUER_DEBUG
  std::cout << "Norm. puls. (1/Ws) = " << 1 / Ws << '\n';
  std::cout << "c = " << c << endl;
#endif

  // Set the rational transmission function D = F/P

  F = qf_poly (1, 0, 0);	    // At first, F = x^2
  P = qf_poly (0, 0, c);	    // and P = c

  for (unsigned u = 2; u <= m; u++) {
    qf_poly MF (1, 0, (*zr) [2*u - 2] * (*zr) [2*u]);
    qf_poly MP ((*zr) [2*u - 2] * (*zr) [2*u], 0, 1);

#ifdef _QF_CAUER_DEBUG
    F. disp ("F");
    P. disp ("P");
    MF. disp ("F'");
    MP. disp ("P'");
#endif
    F *= MF;
    P *= MP;
  }

#ifdef _QF_CAUER_DEBUG
  F. disp ("F");
  P. disp ("P");
#endif
}

//
// These routines synthezise odd order filters
// Once again, the principle is the same, except that for odd filters
// we do not have to extract a final serial inductance
// The poles are also easier to compute
//

bool qf_cauer::synth_odd (void) {

  struct qf_trp	trp;

  BN = BNc;
  BD = BDc;

  lcmp. clear ();

  for (unsigned k = 0; k < m; k ++) {
    qf_double_t pl = (*zr) [2*pole [k] + 2];
#ifdef _QF_CAUER_DEBUG
    std::cout << "Pole (" << 2 * pole [k] + 2 << ") = " << 
	      (1 / (pl * Ws)) << "\n";
#endif

   // Remove finite pole. This produces two normal components
    // one capacitor, one parallel LC
    if (pole_finite (1 / pl, Ws, &trp) && Pspec -> optc) return false;

    if (!Pspec -> dual) {
      // Shunt cap (to ground)
      lcmp. insert (new qf_cap (trp. v, true, true));
      // Serial L || C
      lcmp. insert (new qf_plc (trp. w1, trp. w2, false, false));
    }
    else {
      // First serial ind
      lcmp. insert (new qf_ind (trp. v, false, false));
      // Shunt L + C
      lcmp. insert (new qf_slc (trp. w2, trp. w1, true, true));
    }

    BN. slfodd ();
    BD. slfeven ();
  }

  // Final removal of infinite pole (= parallel cap or serial ind)

  qf_double_t last_cmp = Ws * BN. eval (1) / BD. eval (1);

  if (!Pspec -> dual) {
    //  pole_pC (cap, Ws);
    lcmp. insert (new qf_cap (last_cmp, true, true));
  }
  else {
    lcmp. insert (new qf_ind (last_cmp, false, false));
  }

  lcmp. insert (new qf_end ());
  return true;
}

bool qf_cauer::do_synth_odd (void) {

  scptX1 (F, P, BNc, BDc);

  BNc. slfodd ();
  BDc. slfeven ();

#ifdef _QF_CAUER_DEBUG
  BNc. disp ("BN");
  BDc. disp ("BD");
#endif

  return synth_odd ();
}
  
void qf_cauer::scpt_odd (void) {

  // Computes zeros of transmission
  m = (Pspec -> ord + 1) / 2;

  qf_double_t	sqrth = sqrt (Pspec -> th);
  qf_double_t	delta = 1;

  calc_an (zr, Pspec -> th, Pspec -> ord);
  
  for (unsigned u = 1; u <= m; u++)
    delta *= (*zr) [2*u - 1] * (*zr) [2*u - 1];
  delta /= sqrth;

  qf_double_t c = delta * sqrt (1 / (rho * rho) - 1);
  Ws = sqrth; // We normalize according to Wc = 1

#ifdef _QF_CAUER_DEBUG
  std::cout << "Norm. puls. (1/Ws) = " << 1 / Ws << '\n';
  std::cout << "c = " << c << endl;
#endif

  // Now forms the transmission rational fraction D = F/P

  F = qf_poly (0.0, 1.0, 0.0);	// initially F(X) = X
  P = qf_poly (0.0, 0.0, c);	// initially P(X) = c

  for (unsigned u = 1; u < m; u++) {
    qf_poly MF (1, 0, (*zr) [2*u] * (*zr) [2*u]);
    qf_poly MP ((*zr) [2*u] * (*zr) [2*u], 0, 1);

#ifdef _QF_CAUER_DEBUG
    F. disp ("F");
    P. disp ("P");
    MF. disp ("F'");
    MP. disp ("P'");
#endif

    F *= MF;
    P *= MP;
  }

#ifdef _QF_CAUER_DEBUG
  F. disp ("F");
  P. disp ("P");
#endif

}

// Indirection function that calls the right synth according to type
bool qf_cauer::do_synth (void) {

  return Pspec -> ord % 2 ? do_synth_odd (): do_synth_even ();
}

// Synthesize and optimize if asked for
bool qf_cauer::synth (void) {

  qf_double_t opt_val;
  m = (Pspec -> ord - 1) / 2;

  // If we don't optimize we just compute a standard filter

  if (! Pspec -> optc) {

    pole_idx_init (m);
    pole_idx_std (m);
    return do_synth ();
  }

  // Computes and optimizes
  bool	  ok;

  opt_best = hugedbl;
  pole_idx_init (m);
  ok = do_synth ();

  while (pole_idx_next (m - 1, m, true)) {
    if (ok) {
      // Have we have found a better solution?
      opt_val = check_optim ();
      if (opt_val == 0) return true; // There was nothing to optimize

      if (opt_val < opt_best) {
	opt_best = opt_val;
	best = pole;
      }

      // Do not recompute polynoms
      ok = Pspec -> ord % 2 ? synth_odd () : synth_even ();
    }
  }

  // We have exhausted the possible combinations
  // so we back off to the best one recorded
  pole = best;
  return Pspec -> ord % 2 ? synth_odd () : synth_even ();
}
