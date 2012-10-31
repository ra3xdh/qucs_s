// Definitions for qf_zigzag.cpp

#ifndef _QF_ZIGZAG_H
#define	_QF_ZIGZAG_H

#include <vector>
//Added by qt3to4:
#include <Q3TextStream>
using namespace std;

class qf_zigzag:  public qf_tform {
  
  private:
	      qf_zigzag	    (qf_spec*);
  inline      ~qf_zigzag    (void) {}

  void	      zigzag_opt    (Rvector&, qf_double_t, qf_double_t, qf_double_t,
			     qf_double_t&);
  void	      synth_zz	    (qf_cauer*);

  // As for bandpass
  qf_double_t th	    (void) {return 
				    fabs (Tspec -> fs -
					  Tspec -> fc * Tspec -> fc /
					  Tspec -> fs) / Tspec -> bw;}
  public:
  void		  dnrm	      (qf_ind*) {};
  void		  dnrm	      (qf_cap*) {};
  void		  dnrm	      (qf_res*) {};
  void		  dnrm	      (qf_slc*) {};
  void		  dnrm	      (qf_plc*) {};

  // Dump
 
  void		    dump   (Q3TextStream&);
  friend qf_tform* zigzag  (qf_spec* Tspec) {return new qf_zigzag (Tspec);}
};

qf_tform*	    zigzag  (qf_spec*);
void		    zzdump  (qf_tform*, Q3TextStream&);

  # ifdef _QF_API

struct	qf_tform_api	    qf_zigzag_api = {

	"ZigZag",
	QF_ZIGZAG,

	NULL,
	zigzag,
	CAN_ORDER | CAN_ORDER_ONLY | CAN_ORDER_EVEN | 
	CAN_RIPPLE | CAN_BANDPASS | CAN_OPTIMIZE,
	CAN_ATTENUATION | CAN_STOPBAND,
	CAN_ANGLE,
	0
};

  # endif // _QF_API
#endif	  // _QF_ZIGZAG_H
