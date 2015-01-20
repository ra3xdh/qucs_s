#include "qf_common.h"
#include "qf_poly.h"
#include "qf_comp.h"
#include "qf_filter.h"
#include "qf_capacity.h"
#include "qf_butcheb.h"

  qf_buttcheb::qf_buttcheb (qf_spec* Pspec): qf_filter (Pspec)
{}

// This function synthesizes the ladder filter out of the coefficients
// computed for each class (Butterworth or Chebychev 1)

bool  qf_buttcheb::synth (void) {

  Rlist::iterator i = gn. begin ();
  unsigned	  o = Pspec -> ord;

  // The dual form begins by a serial inductance
  if (Pspec -> dual) {
   lcmp. insert (new qf_ind (*i, false, false));
   i ++ ;
   o --;
  }

  while (o) {
   lcmp. insert (new qf_cap (*i, true, true));
   i ++;
   o --;

   if (o == 0) break;

   lcmp. insert (new qf_ind (*i, false, false));
   i ++;
   o --;
  }

  lcmp. insert (new qf_end ());
  return true;
}

// Functions specific to Butterworth

void  qf_butt::coeffs (void) {
  
  qf_double_t ang = pi / (2 * Pspec -> ord);

  for (unsigned n = 1; n <= Pspec -> ord; n ++) {
    gn. push_back (2 * sin ((2 * n - 1) * ang));
  }
}

  qf_butt::qf_butt (qf_spec* Pspec): qf_buttcheb (Pspec) {

  if (! Pspec -> ord_given) {
    qf_double_t d1 = pow (10, Pspec -> amax / 10);
    Pspec -> ord = static_cast <unsigned>
	    (ceil (0.5 * log10 (d1 - 1)) / log10 (Pspec -> th));
  }

  coeffs ();
}

// Functions specific to chebychev

void  qf_cheb::coeffs () {

  if (Pspec -> ord % 2 == 0) { // Calculate ripple
    qf_double_t rho;
    rho = fabs (Pspec -> r1 - Pspec -> r2) / (Pspec -> r1 + Pspec -> r2);
    rho = 1 - rho * rho;
    Pspec -> amin = -10 * log10 (rho);
  }
    
  qf_double_t bt = log (1 / tanh (Pspec -> amin / 17.37));
  qf_double_t gm = sinh (bt / (2 * Pspec -> ord));

  qf_double_t fa = pi / Pspec -> ord;
  qf_double_t oa;
  qf_double_t a = sin (fa / 2);
  qf_double_t sfa = sin (fa);
  qf_double_t b = gm * gm + sfa * sfa;

  qf_double_t g = 2 * a / gm;
  gn. push_back (g);

  for (qf_double_t i = 2; i <= Pspec -> ord; i ++) {
    oa = a;
    a = sin (fa * (i - 0.5));
    g = 4 * oa * a / (b * g);
    sfa = sin (i * fa);
    b = gm * gm + sfa * sfa;
    gn. push_back (g);
  }
  g = tanh (bt / 4);
}

  qf_cheb::qf_cheb (qf_spec* Pspec): qf_buttcheb (Pspec) {

  if (! Pspec -> ord_given) {
    qf_double_t ep = sqrt (pow (10, Pspec -> amin / 10) - 1);
    qf_double_t A = pow (10, Pspec -> amax / 10);
    qf_double_t ea = acosh (sqrt (A - 1) / ep);
    Pspec -> ord = static_cast <int> (ceil (ea / acosh (Pspec -> th)));
  }

  if (((Pspec -> ord % 2 == 0) && (Pspec -> r1 == Pspec -> r2)) ||
      ((Pspec -> ord % 2 == 1) && (Pspec -> r1 != Pspec -> r2)))
	  Pspec -> ord++;  // We cannot synthesize even orders that have
			   // equal termination or vice-versa

  if (Pspec -> r2 > Pspec -> r1) Pspec -> dual = true;
  coeffs ();
}

bool chebvalid (qf_spec* Pspec) {
  if (Pspec -> ord_given) 
    return ! (((Pspec -> ord % 2 == 0) && (Pspec -> r1 == Pspec -> r2)) ||
      ((Pspec -> ord % 2 == 1) && (Pspec -> r1 != Pspec -> r2)));
    
  return true;
}
