// Legendre filters (or Papoulis)
// This class of filters is montonic, but possess the sharpest fall off
// while being polynomial (no transmission zeros)

// This function returns successive Legendre's polynoms in increasing
// order

#include "qf_common.h"
#include "qf_poly.h"
#include "qf_comp.h"
#include "qf_filter.h"
#include "qf_capacity.h"
#include "qf_legendre.h"

#define	_QF_LGNDR_DEBUG

// Computes the characteristic polynomial, which is derived from
// Legendre polynomials
qf_poly	qf_lgndr::legendre (void) {

  qf_poly     L0 (0.0, 0.0, 1.0), L1 (0.0, 1.0, 0.0), L2 (1.5, 0.0, -0.5);
  qf_poly     W (2.0, 0.0, -1.0);
  qf_poly     S (0.0, 0.0, 0.0);
  unsigned    k;
  qf_double_t c;

  if (Pspec -> ord % 2) { // Odd case
    
    k = (Pspec -> ord - 1) / 2;
    c = one_over_sqrt2 / (k + 1);
    S = L0 * c;

    for (unsigned i = 1; i < k + 1; i ++) {
      if (i == 1) {S += L1 * (3 * c); continue;}
      if (i == 2) {S += L2 * (5 * c); continue;}

      // Recurrence to compute L(i)
  
      L0 = L1;
      L1 = L2;
      L2 = (L2 >> 1) * (2 * i - 1);
      L2 -= L0 * (i - 1);
      L2 *= (1.0 / i);

      // Add to Sum polynom
      S += L2 * ((2 * i + 1) * c);
    }
  }

  else { // Even case
    k = Pspec -> ord / 2 - 1;
    c = 1 / sqrt ((k + 1) * (k + 2));
    if (k % 2) {
	
      S = L1 * (3 * c);
      for (unsigned i = 3; i < k + 1; i ++) {

	// Recurrence to compute L(i)
  
	L0 = L1;
	L1 = L2;
	L2 = (L2 >> 1) * (2 * i - 1);
	L2 -= L0 * (i - 1);
	L2 *= (1.0 / i);

	if (i % 2)  S += L2 * ((2 * i + 1) * c);
      }
    }
      
    else {

      S = L0 * c;
      if (k >= 2) S += L2 * (5 * c);

      for (unsigned i = 3; i < k + 1; i ++) {
	// Recurrence to compute L(i)
  
	L0 = L1;
	L1 = L2;
	L2 = (L2 >> 1) * (2 * i - 1);
	L2 -= L0 * (i - 1);
	L2 *= (1.0 / i);

	if ((i % 2) == 0)  S += L2 * ((2 * i + 1) * c);
      }
    }
  }

  qf_poly S2 (S * S);

  // If n is even, multiply S^2 by (x + 1)
  if ((Pspec -> ord % 2) == 0) S2 *= qf_poly (0.0, 1.0, 1.0);

  // Integrate S2
  qf_poly IS2 (S2. intgrt ());

  // Form attenuation function
  qf_poly L (IS2. rond (W));
  L -= IS2. eval (-1.0);

  // Some form of normalization
  if (Pspec -> ord % 2) L. nrm (2); 
  else { // x^2 factor is null 
    qf_poly Lr (L << 2);
    Lr -= Lr.eval (0);
    L = Lr >> 2;
    L. nrm (4);
  }

  return L;
}

// Constructor

  qf_lgndr::qf_lgndr (qf_spec* Pspec): qf_filter (Pspec) {

  qf_poly     L (legendre ());

#ifdef _QF_LGNDR_DEBUG
  L. disp ("L0");
#endif

  // Computes the normalization frequency ws

  L -= 1.0;
  L.rroot (ws);
  L += 1.0;

  qf_poly MP2 (-1.0, 0.0, 0.0);
  // F(p)F(-p)
  F = (L. sqr ()). rond (MP2);

#ifdef _QF_LGNDR_DEBUG
  L.disp ("L");
  cout << "Normalization ws: " << ws << endl;
  F.disp ("F");
#endif

  // Since P(p) = 1, E(p)E(-p) = F(p)F(-p) + 1
  E = F;
  E += 1.0;

  E. hurw ();
  F. hurw ();
}

bool  qf_lgndr::synth (void) {

  scptX2 (E, F, BN, BD);

#ifdef	_QF_LGNDR_DEBUG
  BN.disp ("BN");
  BD.disp ("BD");
#endif

  return synth_ladder (1.0 / ws);
}
