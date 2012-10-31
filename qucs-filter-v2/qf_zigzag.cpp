// Computes a zig-zag filter out of a CC n rho theta, where n is even
// A zig-zag filter is a BP filter with two inductors less than a normal
// BP filter. It is shown to be ideal in this sense, that it uses the minimal
// number of inductors

// Ref: Saal, Ulbrich: 'On the design of filters by synthesis'
// IRE transactions on circuit theory, Dec. 1958 pp. 300-301

#include "qf_common.h"
#include "qf_poly.h"
#include "qf_comp.h"
#include "qf_capacity.h"
#include "qf_filter.h"
#include "qf_cauer.h"
#include "qf_tform.h"
#include "qf_zigzag.h"
//Added by qt3to4:
#include <Q3TextStream>

#undef	  _QF_ZIGZAG_DEBUG

// Arc sin in degrees
inline qf_double_t ASIND (qf_double_t ang) {
  return 180 * asin (ang) / M_PI;
}

qf_zigzag::qf_zigzag (qf_spec* Tspec): qf_tform (Tspec)
{
  // Calculate prototype with R1 = 1 Ohm
  if (! Tspec -> ord_given)
    Tspec -> th = th ();

  qf_double_t tempr1 = Tspec -> r1;
  Tspec -> r1 = 1;
  Tspec -> r2 = 1;
  Tspec -> subord = 'c';
  Tspec -> is_zigzag = true;

  qf_cauer* CP = dynamic_cast <qf_cauer*> (cauer (Tspec));
  CP -> synth ();

  // Restore true impedance and perform transform
  Tspec -> r1 = tempr1;
  synth_zz (CP);
  delete CP;
}

// This routine performs the transformation from lowpass to zigzag bandpass
// It is by no means optimized, because, for sake of simplicity,
// we use the original index numbering as used in the paper cited supra.

void qf_zigzag::synth_zz (qf_cauer* F) {

  unsigned	  m = (Tspec -> ord - 2) / 2;

  Rvector C  (Tspec -> ord);
  Rvector L  (Tspec -> ord + 1);
  Rvector c  (5 * m);
  Rvector l  (4 * m);
  Rvector cp (7 * m);
  Rvector cs (7 * m);
  Rvector lp (5 * m);

  Rvector t   (m);
  Rvector tp  (m);
  Rvector ts  (m);

  // STAGE 0
  // Allocate initial values out of standard low-pass filter

  unsigned	    mu = 1;
  qf_cmp*	    cmp;
  qf_cap*	    cap;
  qf_plc*	    plc;
  qf_ind*	    ind;

#ifdef	_QF_ZIGZAG_DEBUG
  QString  * buf = new QString ();
  Q3TextStream *CD = new Q3TextStream (* buf, QIODevice::ReadWrite);
  (F -> lcmp). dump_all (* CD);
  std::cout << buf -> data (); 
  std::cout. flush ();
  delete CD;
  delete buf;
#endif

  F -> linit ();

  while ((cmp = F -> lnext ()) != NULL) {

    // Parallel cap
    assert ((cap = dynamic_cast <qf_cap*> (cmp)) != NULL);
    C[mu++] = cap -> val;

    cmp = F -> lnext ();

    // Last component? (Should be an inductor)
    if ((ind = dynamic_cast <qf_ind*> (cmp)) != NULL)
      break;

    // Serial L // C
    assert ((plc = dynamic_cast <qf_plc*> (cmp)) != NULL);

    C[mu] = plc -> vC;
    L[mu++] = plc -> vL;
  }

  // Last component should be an inductor
  L[Tspec -> ord] = ind -> val;

  // Control
  assert (mu == Tspec -> ord);

#ifdef _QF_ZIGZAG_DEBUG
  std::cout << "STAGE 0\n";
  for (mu = 1;;) {
    std::cout << "C" << mu << " = " << C[mu] << '\n';
    mu ++;
    if (mu == Tspec -> ord) break ;
    std::cout << "C" << mu << " = " << C[mu] << '\t'
	      << "L" << mu << " = " << L[mu] << '\n';
    mu++;
  }
  std::cout << "L" << mu << " = " << L[mu] << '\n';
#endif

  // STAGE 1: split filter into PI sections

  c[5 * m + 3] = C[Tspec -> ord - 1];
  c[5 * m + 2] = C[Tspec -> ord - 2];
  t[m] = 1 + c[5 * m + 3] / c[5 * m + 2];
  c[5 * m + 1] = -c[5 * m + 3] / t[m];

  for (mu = 1; mu < m; mu++) {
    unsigned mmu = m - mu;

    c[5 * mmu + 3] = C[Tspec -> ord - 1 - 2 * mu] - c[5 * mmu + 6];
    c[5 * mmu + 2] = C[Tspec -> ord - 2 - 2 * mu];
    t[mmu] = 1 + c[5 * mmu + 3] / c[5 * mmu + 2];
    c[5 * mmu + 1] = -c[5 * mmu + 3] / t[mmu];
  }

  qf_double_t ca = C[1] - c[6];
  qf_double_t lb = L[Tspec -> ord];

#ifdef _QF_ZIGZAG_DEBUG
  std::cout << "\nSTAGE 1";
  for (mu = 0; mu < m; mu++) {
    unsigned mmu = m - mu;

    std::cout << "\nCell: " << mmu <<'\n';
    std::cout << "C(" << mmu << ", 1) = " << c[5 * mmu + 1] << '\t';
    std::cout << "C(" << mmu << ", 2) = " << c[5 * mmu + 2] << '\t';
    std::cout << "C(" << mmu << ", 3) = " << c[5 * mmu + 3] << '\n';
    std::cout << "t(" << mmu << ") = " << t[mmu] << "\n";
  }
  std::cout << "\nca = " << ca << ", lb = " << lb << "\n\n";
#endif

  // STAGE 2: perform transform

  qf_double_t a = Tspec -> fc / Tspec -> bw;

#ifdef	_QF_ZIGZAG_DEBUG
  std::cout << "STAGE 2";
#endif

  for (mu = 0; mu < m; mu++) {
    unsigned mmu = m - mu;

    qf_double_t w = 1 / sqrt (C[2 * mmu] * L[2 * mmu]);
    qf_double_t w2a = w / (2 * a);
    qf_double_t ep = sqrt (w2a * w2a + 1) + w2a;
    qf_double_t em = sqrt (w2a * w2a + 1) - w2a;
    qf_double_t ep2 = ep * ep;
    qf_double_t em2 = em * em;
    qf_double_t T = (1 + t[mmu] * ep2) / (t[mmu] + ep2);
    qf_double_t k = a * T / t[mmu];
    qf_double_t ti = 1 / (1 + t[mmu] * em2);

    cp[7 * mmu + 1] = -c[5 * mmu + 3] * k * (1 - em2) / T;
    cp[7 * mmu + 2] = -c[5 * mmu + 1] * k * (ep2 - 1) / ti;
    cp[7 * mmu + 3] = c[5 * mmu + 3] * k * (1 - em2) / (T - 1);
    lp[5 * mmu + 3] = em2 / cp[7 * mmu + 3];
    cp[7 * mmu + 4] = -c[5 * mmu + 1] * k * (T - 1) * (ep2 - 1) / (ti * ti);
    lp[5 * mmu + 4] = ep2 / cp[7 * mmu + 4];
    cp[7 * mmu + 5] = c[5 * mmu + 3] * k * (1 - em2) / ti;
    cp[7 * mmu + 6] = c[5 * mmu + 1] * k * T * (ep2 - 1) / (ti * ti);

    tp[mmu] = t[mmu] * ti / T;

#ifdef	_QF_ZIGZAG_DEBUG
    std::cout << "\nCell: " << mmu <<"\n\n";
    std::cout << "w " << w << "\n";
    std::cout << "w/2a " << w2a << "\n";
    std::cout << "ep " << ep << "\n";
    std::cout << "em " << em << "\n";
    std::cout << "ep2 " << ep2 << "\n";
    std::cout << "em2 " << em2 << "\n";
    std::cout << "T " << T << "\n";
    std::cout << "k " << k << "\n";
    std::cout << "ti " << ti << "\n";

    std::cout << "c'(" << mmu << ", 1) = " << cp[7 * mmu + 1] << '\n';
    std::cout << "c'(" << mmu << ", 2) = " << cp[7 * mmu + 2] << '\n';
    std::cout << "c'(" << mmu << ", 3) = " << cp[7 * mmu + 3] << '\t';
    std::cout << "l'(" << mmu << ", 3) = " << lp[5 * mmu + 3] << '\n';
    std::cout << "c'(" << mmu << ", 4) = " << cp[7 * mmu + 4] << '\t';
    std::cout << "l'(" << mmu << ", 4) = " << lp[5 * mmu + 4] << '\n';
    std::cout << "c'(" << mmu << ", 5) = " << cp[7 * mmu + 5] << '\n';
    std::cout << "c'(" << mmu << ", 6) = " << cp[7 * mmu + 6] << '\n';
    std::cout << "t'(" << mmu << ") = " << tp[mmu] << "\n";
#endif

  }

  qf_double_t cpa = a * ca;
  qf_double_t lpa = 1 / cpa;
  qf_double_t lpb = a * lb;
  qf_double_t cpb = 1 / lpb;

#ifdef	_QF_ZIGZAG_DEBUG
  std::cout << "\nc'(a) = " << cpa << '\n';
  std::cout << "l'(a) = " << lpa << '\n';
  std::cout << "c'(b) = " << cpb << '\n';
  std::cout << "l'(b) = " << lpb << '\n';


  // STAGE 3: transform serial cap into shunt and vice-versa

  std::cout << "\nSTAGE 3";
#endif

  for (mu = 1; mu < m; mu ++) {
    unsigned mmu = m - mu;

    ts[mmu] = tp[mmu] + cp[7 * mmu + 8] / (tp[mmu] * cp[7 * mmu + 6]);

    qf_double_t tps = 1 - tp[mmu] / ts[mmu];

    cs[7 * mmu + 8] = cp[7 * mmu + 6] * tps;
    cs[7 * mmu + 6] = cp[7 * mmu + 8] / tps;

#ifdef	_QF_ZIGZAG_DEBUG
    cout << endl << "Cell: " << mmu << endl;
    cout << "c''(" << mmu + 1 << ", 1) = " << cs[7 * mmu + 8] << endl;
    cout << "c''(" << mmu << ", 6) = " << cs[7 * mmu + 6] << endl;
    cout << "t''(" << mmu << ") = " << ts[mmu] << endl;
#endif
  }

  qf_double_t lsb = lpb * tp[m] * tp[m];
  qf_double_t csb = cpb / (tp[m] * tp[m]);

#ifdef	_QF_ZIGZAG_DEBUG
  std::cout << "\nl''(b) = " << lsb << '\n';
  std::cout << "c''(b) = " << csb << '\n';
#endif

  // STAGE 4: get rid of interstage ideal transformers

  c[6] = cp[9];
  c[7] = cp[10];
  l[6] = lp[8];
  c[8] = cp[11];
  l[7] = lp[9];
  c[9] = cp[12] + cs[15];

  qf_double_t pts2 = ts[1] * ts[1];

  for (mu = m - 2; ; mu--) {
    unsigned mmu = m - mu;

    c[5 * mmu + 1] = cp[7 * mmu + 2] * cs[7 * mmu - 1] /
		  ((cp[7 * mmu + 2] + cs[7 * mmu - 1]) * pts2);
    c[5 * mmu + 2] = cp[7 * mmu + 3] / pts2;
    l[4 * mmu + 2] = lp[5 * mmu + 3] * pts2;
    c[5 * mmu + 3] = cp[7 * mmu + 4] / pts2;
    l[4 * mmu + 3] = lp[5 * mmu + 4] * pts2;

    if (mu == 0) break;

    c[5 * mmu + 4] = (cp[7 * mmu + 5] + cs[7 * mmu + 8]) / pts2;

    pts2 *= ts[mmu] * ts[mmu];
  }

  c[5 * m + 4] = cp[7 * m + 5] / pts2;

  qf_double_t la = lpa;
	      ca = cpa + cp[8];
  qf_double_t cb = (csb * cp[7 * m + 6]) / (pts2 * (csb + cp[7 * m + 6]));
	      lb = lsb * pts2;

#ifdef	_QF_ZIGZAG_DEBUG
  std::cout << "\nSTAGE 4";
  for (mu = 1; mu <= m; mu ++) {
    std::cout << "\nCell: " << mu << '\n';
    std::cout << "c(" << mu << ", 1) = " << c[5 * mu + 1] << '\n';
    std::cout << "c(" << mu << ", 2) = " << c[5 * mu + 2] << '\t';
    std::cout << "l(" << mu << ", 2) = " << l[4 * mu + 2] << '\n';
    std::cout << "c(" << mu << ", 3) = " << c[5 * mu + 3] << '\t';
    std::cout << "l(" << mu << ", 3) = " << l[4 * mu + 3] << '\n';
    std::cout << "c(" << mu << ", 4) = " << c[5 * mu + 4] << '\n';
  }

    std::cout << "\nla = " << la << '\n';
    std::cout << "ca = " << ca << '\n';
    std::cout << "lb = " << lb << '\n';
    std::cout << "cb = " << cb << '\n';
#endif

  // STAGE 5: Synthesize new structure and denormalize


  qf_double_t nrmC = 1 / (Tspec -> r1 * Tspec -> fc);
  qf_double_t nrmL = Tspec -> r1 / Tspec -> fc;

  // First shunt L//C

  lcmp. insert (new qf_plc (ca * nrmC, la * nrmL, true, true));

  for (mu = 1; mu <= m; mu ++) {
    // Serial C
    lcmp. insert (new qf_cap (c[5 * mu + 1] * nrmC, false, false));

    // Serial L//C
    lcmp. insert (new qf_plc (c[5 * mu + 2] * nrmC, l[4 * mu + 2] * nrmL,
			      false, false));

    // Shunt L + C
    lcmp. insert (new qf_slc (c[5 * mu + 3] * nrmC, l[4 * mu + 3] * nrmL, 
		  true, true));

    // Shunt C
    lcmp. insert (new qf_cap (c[5 * mu + 4] * nrmC, true, true));
  }

  // Last serial L + C
  lcmp. insert (new qf_slc (cb * nrmC, lb * nrmL, false, false));
  lcmp. insert (new qf_end ());

  // Computes output impedance
  Tspec -> r2 = Tspec -> r1 * pts2 * tp[m] * tp[m];
}

void qf_zigzag::dump (Q3TextStream& out) {

  lcmp.tx = qf_filter_apis [Tspec -> filter] -> name + " zigzag filter" + 
    " of order " + QString::number(Tspec -> ord);
  lcmp.r1 = Tspec -> r1;
  lcmp.r2 = Tspec -> r2;
  lcmp.fc = Tspec -> fc;
  lcmp.dump_all (out);
}
