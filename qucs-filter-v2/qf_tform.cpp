# include "qf_common.h"
# include "qf_poly.h"
# include "qf_comp.h"
# include "qf_capacity.h"
# include "qf_filter.h"
# include "qf_tform.h"
//Added by qt3to4:
#include <Q3TextStream>

// Validation functions

bool  lowpassvalid   (qf_spec* Tspec) {

  return (Tspec -> fs == 0) || (Tspec -> fc < Tspec -> fs);
}

bool  highpassvalid  (qf_spec* Tspec) {

  return (Tspec -> fs == 0) || (Tspec -> fs < Tspec -> fc);
}

// Bandpass and bandstop have the same validator
bool  bandpassvalid  (qf_spec* Tspec) {
 
  if ((Tspec -> fc - Tspec -> bw / 2) <= 0) return false;
  if (fabs (Tspec -> fc - Tspec -> fs) <= (Tspec -> bw / 2)) return false;
  return true;
}  

// Constructors (they are all the same) except qf_nonetf

qf_nonetf::qf_nonetf (qf_spec * Tspec): qf_tform (Tspec) {

  Tspec -> is_zigzag = false;
  // The constructor is supposed to do all the synthesis
  qf_filter *F = qf_filter_apis [Tspec -> filter] -> cons (Tspec);
    if (! F -> islvoid ()) {
    qf_cmp* cmp;
    F -> linit ();

    while ((cmp = F -> lnext ()) != NULL) cmp -> dnrm (this);

    lcmp. insert (new qf_end ());
  }
}

qf_lowpass::qf_lowpass (qf_spec* Tspec):   qf_tform (Tspec) {

  Tspec -> th = th ();
  Tspec -> is_zigzag = false;

  qf_filter* F = qf_filter_apis [Tspec -> filter] -> cons (Tspec);

  F -> synth ();
  if (! F -> islvoid ()) {
    qf_cmp* cmp;
    F -> linit ();

    while ((cmp = F -> lnext ()) != NULL) cmp -> dnrm (this);

    lcmp. insert (new qf_end ());
  }
}

qf_highpass::qf_highpass (qf_spec* Tspec):   qf_tform (Tspec) {

  Tspec -> th = th ();
  Tspec -> is_zigzag = false;

  qf_filter* F = qf_filter_apis [Tspec -> filter] -> cons (Tspec);

  F -> synth ();
  if (! F -> islvoid ()) {
    qf_cmp* cmp;
    F -> linit ();

    while ((cmp = F -> lnext ()) != NULL) cmp -> dnrm (this);

    lcmp. insert (new qf_end ());
  }
}

qf_bandpass::qf_bandpass (qf_spec* Tspec):   qf_tform (Tspec) {

  Tspec -> th = th ();
  Tspec -> is_zigzag = false;

  qf_filter* F = qf_filter_apis [Tspec -> filter] -> cons (Tspec);

  F -> synth ();
  if (! F -> islvoid ()) {
    qf_cmp* cmp;
    F -> linit ();

    while ((cmp = F -> lnext ()) != NULL) cmp -> dnrm (this);

    lcmp. insert (new qf_end ());
  }
}

qf_bandstop::qf_bandstop (qf_spec* Tspec):   qf_tform (Tspec) {

  Tspec -> th = th ();
  Tspec -> is_zigzag = false;

  qf_filter* F = qf_filter_apis [Tspec -> filter] -> cons (Tspec);

  F -> synth ();
  if (! F -> islvoid ()) {
    qf_cmp* cmp;
    F -> linit ();

    while ((cmp = F -> lnext ()) != NULL) cmp -> dnrm (this);

    lcmp. insert (new qf_end ());
  }
}

// Dump functions

void qf_nonetf::dump (Q3TextStream& out) {

  lcmp.tx = qf_filter_apis [Tspec -> filter] -> name +
    " of order " + QString::number(Tspec -> ord) + Tspec -> subord;
  lcmp.r1 = Tspec -> r1;
  lcmp.r2 = Tspec -> r2;
  lcmp.fc = Tspec -> fc;
  lcmp.dump_all (out);
}

void qf_lowpass::dump (Q3TextStream& out) {

  lcmp.tx = qf_filter_apis [Tspec -> filter] -> name + " low-pass filter" + 
    " of order " + QString::number(Tspec -> ord) + Tspec -> subord;
  lcmp.r1 = Tspec -> r1;
  lcmp.r2 = Tspec -> r2;
  lcmp.fc = Tspec -> fc;
  lcmp.dump_all (out);
}

void qf_highpass::dump (Q3TextStream& out) {

  lcmp.tx = qf_filter_apis [Tspec -> filter] -> name + " high-pass filter" + 
    " of order " + QString::number(Tspec -> ord) + Tspec -> subord;
  lcmp.r1 = Tspec -> r1;
  lcmp.r2 = Tspec -> r2;
  lcmp.fc = Tspec -> fc;
  lcmp.dump_all (out);
}

void qf_bandpass::dump (Q3TextStream& out) {

  lcmp.tx = qf_filter_apis [Tspec -> filter] -> name + " band-pass filter" + 
    " of order " + QString::number(Tspec -> ord) + Tspec -> subord;
  lcmp.r1 = Tspec -> r1;
  lcmp.r2 = Tspec -> r2;
  lcmp.fc = Tspec -> fc;
  lcmp.dump_all (out);
}

void qf_bandstop::dump (Q3TextStream& out) {

  lcmp.tx = qf_filter_apis [Tspec -> filter] -> name + " band-stop filter" + 
    " of order " + QString::number(Tspec -> ord) + Tspec -> subord;
  lcmp.r1 = Tspec -> r1;
  lcmp.r2 = Tspec -> r2;
  lcmp.fc = Tspec -> fc;
  lcmp.dump_all (out);
}

// Denormalization functions
// Those belong to qf_cmp classes
// According to the transformation type, the denormalization function
// computes the equivalent real component for each prototype component

// Null (just inserts elements)

void  qf_ind::dnrm  (qf_nonetf* L) {

  L -> lcmp. insert (new qf_ind (val, shnt, gnd));
}

void  qf_cap::dnrm  (qf_nonetf* L) {

  L -> lcmp. insert (new qf_cap (val, shnt, gnd));
}

void  qf_res::dnrm  (qf_nonetf*  L) {

  L -> lcmp. insert (new qf_res (val, shnt, gnd));
}

void  qf_slc::dnrm  (qf_nonetf* L) {

  L -> lcmp. insert (new qf_slc (vC, vL, shnt, gnd));
}

void  qf_plc::dnrm  (qf_nonetf*  L) {

  L -> lcmp. insert (new qf_plc (vC, vL, shnt, gnd));
}


// Lowpass

void  qf_ind::dnrm  (qf_lowpass* L) {

  L -> lcmp. insert (new qf_ind (val * L -> Tspec -> r1 / L -> Tspec -> fc, 
			    shnt, gnd));
}

void  qf_cap::dnrm  (qf_lowpass* L) {

  L -> lcmp. insert (new qf_cap (val / (L -> Tspec -> r1 * L -> Tspec -> fc),
			    shnt, gnd));
}

void  qf_res::dnrm  (qf_lowpass*  L) {

  L -> lcmp. insert (new qf_res (val * L -> Tspec -> r1, shnt, gnd));
}

void  qf_slc::dnrm  (qf_lowpass* L) {

  L -> lcmp. insert (new qf_slc (vC / (L -> Tspec -> r1 * L -> Tspec -> fc), 
			    L -> Tspec -> r1 * vL / L -> Tspec -> fc, 
			    shnt, gnd));
}

void  qf_plc::dnrm  (qf_lowpass*  L) {

  L -> lcmp. insert (new qf_plc (vC / (L -> Tspec -> r1 * L -> Tspec -> fc),
			    L -> Tspec -> r1 * vL / L -> Tspec -> fc, 
			    shnt, gnd));
}

// Highpass

void  qf_ind::dnrm  (qf_highpass* H) {

  H -> lcmp. insert (
    new qf_cap (1 / (val * H -> Tspec -> r1 * H -> Tspec -> fc), shnt, gnd));
}

void  qf_cap::dnrm  (qf_highpass* H) {

  H -> lcmp. insert (
    new qf_ind (H -> Tspec -> r1 / (val * H -> Tspec -> fc), shnt, gnd));
}

void  qf_res::dnrm  (qf_highpass* H) {

  H -> lcmp. insert (new qf_res (val * H -> Tspec -> r1, shnt, gnd));
}

void  qf_slc::dnrm  (qf_highpass* H) {

  H -> lcmp. insert (
	new qf_slc (1 / (vL * H -> Tspec -> r1 * H -> Tspec -> fc), 
		    H -> Tspec -> r1 / (vC * H -> Tspec -> fc), 
		    shnt, gnd));
}

void  qf_plc::dnrm  (qf_highpass* H) {

  H -> lcmp. insert (
	new qf_plc (1 / (vL * H -> Tspec -> r1 * H -> Tspec -> fc),
		    H -> Tspec -> r1 / (vC * H -> Tspec -> fc), 
		    shnt, gnd));
}

// Bandpass (traditional geometrical type)

void  qf_ind::dnrm  (qf_bandpass* B) {

  qf_double_t a = B -> Tspec -> fc / B -> Tspec -> bw;
  qf_double_t vC = 1 / (a * val * B -> Tspec -> r1 * B -> Tspec -> fc);
  qf_double_t vL = a * val * B -> Tspec -> r1 / B -> Tspec -> fc;

  B -> lcmp. insert (new qf_slc (vC, vL, shnt, gnd));
}

void  qf_cap::dnrm  (qf_bandpass* B) {

  qf_double_t a = B -> Tspec -> fc / B -> Tspec -> bw;
  qf_double_t vC = a * val / (B -> Tspec -> r1 * B -> Tspec -> fc);
  qf_double_t vL = B -> Tspec -> r1 / (a * val * B -> Tspec -> fc);

  B -> lcmp. insert (new qf_plc (vC, vL, shnt, gnd));
}

void  qf_res::dnrm  (qf_bandpass* B) {

  B -> lcmp. insert (new qf_res (val * B -> Tspec -> r1, shnt, gnd));
}

void  qf_slc::dnrm  (qf_bandpass* B) {

  qf_double_t a = B -> Tspec -> fc / B -> Tspec -> bw;
  qf_double_t b = sqrt (4 * a * a * vC * vL + 1);

  qf_double_t nC1 = (b - 1) / (a * vL * 2 * b);
  qf_double_t nC2 = (b + 1) / (a * vL * 2 * b);

  qf_double_t nL1 = 1 / nC1;
  qf_double_t nL2 = 1 / nC2;

  B -> lcmp. insert (new qf_pslc (nC1 / (B -> Tspec -> r1 * B -> Tspec -> fc), 
				  nL1 * B -> Tspec -> r1 / B -> Tspec -> fc, 
				  nC2 / (B -> Tspec -> r1 * B -> Tspec -> fc), 
				  nL2 * B -> Tspec -> r1 / B -> Tspec -> fc, 
				  shnt, gnd));
}


void  qf_plc::dnrm  (qf_bandpass* B) {

  qf_double_t a = B -> Tspec -> fc / B -> Tspec -> bw;
  qf_double_t b = sqrt (4 * a * a * vC * vL + 1);

  qf_double_t nL1 = (b - 1) / (a * vC * 2 * b);
  qf_double_t nL2 = (b + 1) / (a * vC * 2 * b);

  qf_double_t nC1 = 1 / nL2;
  qf_double_t nC2 = 1 / nL1;

  B -> lcmp. insert (new qf_plc (nC1 / (B -> Tspec -> r1 * B -> Tspec -> fc), 
				 nL1 * B -> Tspec -> r1 / B -> Tspec -> fc, 
				 shnt, false));

  B -> lcmp. insert (new qf_plc (nC2 / (B -> Tspec -> r1 * B -> Tspec -> fc),
				 nL2 * B -> Tspec -> r1 / B -> Tspec -> fc, 
				 false, gnd));
}

// Bandstop (inverse bandpass)

void  qf_ind::dnrm  (qf_bandstop* B) {

  qf_double_t a = B -> Tspec -> fc / B -> Tspec -> bw;
  qf_double_t vC = a / (val * B -> Tspec -> r1 * B -> Tspec -> fc);
  qf_double_t vL = val * B -> Tspec -> r1 / (a * B -> Tspec -> fc);

  B -> lcmp. insert (new qf_plc (vC, vL, shnt, gnd));
}

void  qf_cap::dnrm  (qf_bandstop* B) {

  qf_double_t a = B -> Tspec -> fc / B -> Tspec -> bw;
  qf_double_t vC = val / (a * B -> Tspec -> r1 * B -> Tspec -> fc);
  qf_double_t vL = a * B -> Tspec -> r1 / (val * B -> Tspec -> fc);

  B -> lcmp. insert (new qf_slc (vC, vL, shnt, gnd));
}

void  qf_res::dnrm  (qf_bandstop* B) {

  B -> lcmp. insert (new qf_res (val * B -> Tspec -> r1, shnt, gnd));
}

void  qf_slc::dnrm  (qf_bandstop *B) {

  qf_double_t a = B -> Tspec -> fc / B -> Tspec -> bw;
  qf_double_t s = 4 * a * a / (vC * vL);
  qf_double_t b = sqrt (s + 1);

  qf_double_t nC1 = (b - 1) * vC / (a * 2 * b);
  qf_double_t nC2 = (b + 1) * vC / (a * 2 * b);

  qf_double_t nL1 = 1 / nC1;
  qf_double_t nL2 = 1 / nC2;

  B -> lcmp. insert (new qf_pslc (nC1 / (B -> Tspec -> r1 * B -> Tspec -> fc), 
				  nL1 * B -> Tspec -> r1 / B -> Tspec -> fc, 
				  nC2 / (B -> Tspec -> r1 * B -> Tspec -> fc),
				  nL2 * B -> Tspec -> r1 / B -> Tspec -> fc, 
				  shnt, gnd));
}

void  qf_plc::dnrm  (qf_bandstop *B) {

  qf_double_t a = B -> Tspec -> fc / B -> Tspec -> bw;
  qf_double_t s = 4 * a * a / (vC * vL);
  qf_double_t b = sqrt (s + 1);

  qf_double_t nL1 = (b - 1) * vL / (a * 2 * b);
  qf_double_t nL2 = (b + 1) * vL / (a * 2 * b);

  qf_double_t nC1 = 1 / nL2;
  qf_double_t nC2 = 1 / nL1;

  B -> lcmp. insert (new qf_plc (nC1 / (B -> Tspec -> r1 * B -> Tspec -> fc),
				 nL1 * B -> Tspec -> r1 / B -> Tspec -> fc,
				 shnt, false));

  B -> lcmp. insert (new qf_plc (nC2 / (B -> Tspec -> r1 * B -> Tspec -> fc),
				 nL2 * B -> Tspec -> r1 / B -> Tspec -> fc,
				 false, gnd));
}
