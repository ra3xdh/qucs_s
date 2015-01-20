// This module calculates Inverse Chebychev filters

#include "qf_common.h"
#include "qf_poly.h"
#include "qf_comp.h"
#include "qf_capacity.h"
#include "qf_filter.h"
#include "qf_icheb.h"

#undef	_QF_ICHEB_DEBUG

// Extension to qf_poly, needed only there

// Computes the nth Chebychev polynom

static qf_poly cheb (unsigned n) {

  qf_poly C0 (0.0, 0.0, 1.0);
  qf_poly C1 (0.0, 1.0, 0.0);
  qf_poly C2 (2.0, 0.0, -1.0);

  if (n == 0) return C0;
  if (n == 1) return C1;
  if (n == 2) return C2;

  for (unsigned i = 2 ; i <= n ; i ++) {
    C2 = (C1 >> 1) * 2;
    C2 -= C0;

    C0 = C1;
    C1 = C2;
  }

  return C2;
}

// Computes a Type 2 Chebychev filter out of normal spec, that is computes
// order needed to satisfy filter requirements.

qf_icheb::qf_icheb (qf_spec* Pspec): qf_filter (Pspec), zr (0) {

  qf_double_t d1 = pow (10, -Pspec -> amin / 20);
  qf_double_t d2;

  if (! Pspec -> ord_given) {
    d2 = pow (10, -Pspec -> amax / 20);
    Pspec -> ord = order (d1, d2);
  }
  else {
    qf_double_t epc = 1 / (d1 * d1) - 1;
    qf_double_t al = cosh (Pspec -> ord * acosh (Pspec -> th));
    d2 = sqrt (1 / (1 + al * al * epc));
  }

  scpt	(d2);
}

unsigned qf_icheb::order ( qf_double_t	d1,
			   qf_double_t	d2) {

  qf_double_t ep = sqrt (1 - d1 * d1) / d1;
  qf_double_t rp = sqrt (1 - d2 * d2) / (ep * d2);

  unsigned  n = (unsigned) ceil (acosh (rp) / acosh (Pspec -> th));
  if (n % 2 == 0) n ++;		// Even orders cannot be synthesized

#ifdef _QF_ICHEB_DEBUG
  std::cout << "This is a filter of order: " << n << '\n';
#endif

  return n;
}

void qf_icheb::scpt (qf_double_t d2) {

  // Computes the 'E' polynom by its roots

  qf_double_t g = (1 + sqrt (1 - d2 * d2)) / d2;
  qf_double_t gp = pow (g, 1 / static_cast <qf_double_t> (Pspec -> ord));

  for (unsigned k = Pspec -> ord + 1; k <= (3 * Pspec -> ord + 1) / 2; k ++) {
    qf_double_t an = (2 * k - 1) * pi_over_2 / Pspec -> ord;
    qf_double_t san = sin (an);

    san *= (gp * gp - 1) / (2 * gp);

    if ((Pspec -> ord % 2) && (k == (3 * Pspec -> ord + 1) / 2)) {
#ifdef _QF_ICHEB_DEBUG
      std::cout << "Root: " << Pspec -> th / san << "\n";
#endif
      qf_poly ME (0.0, 1.0, - Pspec -> th / san);
      E *= ME;
      break;
    }
    
    else {
      qf_double_t can = cos (an);
      can *= (gp * gp + 1) / (2 * gp);

      qf_double_t mod = san * san + can * can;
      qf_double_t rpl = Pspec -> th * san / mod;
      qf_double_t ipl = Pspec -> th * can / mod;
#ifdef _QF_ICHEB_DEBUG
      std::cout << "Root: " << rpl << " + " << ipl << " i\n";
#endif

      qf_poly ME (1.0, - 2 * rpl, rpl * rpl + ipl * ipl);
      E *= ME;
    }
  }

#ifdef _QF_ICHEB_DEBUG
  E. disp ("E");
#endif

  // Now computes the zeros of transmission

  for (unsigned k = 1; k <= Pspec -> ord / 2; k ++) {
    zr. push_back (Pspec -> th / cos ((2 * k - 1) * pi_over_2 / Pspec -> ord));
#ifdef _QF_ICHEB_DEBUG
      std::cout << "Zero: " << zr [k - 1] << "\n";
#endif
  }

  // Computes F = d2 / sqrt (1 - d2^2) * x^n

  if (Pspec -> ord % 2 == 0) {
    qf_double_t k = d2;
    F = qf_poly (0, 0, k);
  }

  F = F >> Pspec -> ord;

#ifdef _QF_ICHEB_DEBUG
  F. disp ("F");
#endif
  return ;
}

bool qf_icheb::synth () {

  scptX2 (E, F, BN, BD);

#ifdef _QF_ICHEB_DEBUG
  BN. disp ("BN");
  BD. disp ("BD");
#endif

  qf_trp      trp;

//  for (unsigned k = 1; k <= ord / 2; k ++) {
    for (unsigned k = Pspec -> ord / 2, l = Pspec -> ord / 2 - 1; k > 0; k --) {
#ifdef _QF_ICHEB_DEBUG
    std::cout << "Pole " << l + 1 << ": " << zr [l] << endl;
#endif
    
    pole_finite (zr [l], 1, &trp);
    if (! Pspec -> dual) {

      // Cap (shunt to ground)
      lcmp. insert (new qf_cap (trp. v, true, true));
      // Serial L || C (direct)
      lcmp. insert (new qf_plc (trp. w1, trp. w2, false, false));
    }
    else {

      // Serial ind
      lcmp. insert (new qf_ind (trp. v, false, false));
      // Serial LC (shunt to ground)
      lcmp. insert (new qf_slc (trp. w2, trp. w1, true, true));
    }

    l = Pspec -> ord / 2 - 1 - l;
    if (l > Pspec -> ord / 4)
      l --;
  }

  // Extract final parallel cap or serial ind
  qf_double_t last_cmp = BN. eval (1) / BD. eval (1);

  if (! Pspec -> dual) {
    //  pole_pC (cap, Ws);
    lcmp. insert (new qf_cap (last_cmp, true, true));
  }
  else {
    lcmp. insert (new qf_ind (last_cmp, false, false));
  }

  lcmp. insert (new qf_end ());
  return true;
}
