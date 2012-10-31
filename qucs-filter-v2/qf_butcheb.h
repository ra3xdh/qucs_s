//Added by qt3to4:
#include <Q3TextStream>
// Header for butterworth and chebychev I type filters
// These filters are the easiest to compute, since the values of the
// standard lowpass prototype are given by simple formulae. There is, thus,
// no need to rely on the complex polynomial machinery used elsewhere.
#ifndef _QF_BUTCHEB_H
#define	_QF_BUTCHEB_H

// A general class common to both filters

class qf_buttcheb:  public qf_filter {
  protected:
  Rlist		gn;				// Coeff list

  virtual void	coeffs	      (void) = 0;	// Computes coeffs
  bool		synth	      (void);		// Synthezise out of coeffs

		qf_buttcheb   (qf_spec*);
  virtual	~qf_buttcheb  (void) {}
  friend void	btchdump      (qf_filter*, Q3TextStream&);
};

class qf_butt:	public qf_buttcheb {

  private:
  virtual void	    coeffs    (void);	// Implements above

		    qf_butt   (qf_spec*);
		    ~qf_butt  (void) {}

  friend qf_filter* butt      (qf_spec* Pspec) {return new qf_butt (Pspec);}
};

class qf_cheb:	public qf_buttcheb {

  private:
  qf_double_t	    ripple;

  virtual void	    coeffs   (void);	// Implements above
		    qf_cheb  (qf_spec*);
		    ~qf_cheb (void) {}

  friend qf_filter* cheb     (qf_spec* spec) {return new qf_cheb (spec);}
};

qf_filter*	    butt      (qf_spec*);
qf_filter*	    cheb      (qf_spec*);
void		    btchdump  (qf_filter*, Q3TextStream&);
bool		    chebvalid (qf_spec *);

  # ifdef _QF_API

struct qf_filter_api    qf_butt_api = {

	"Butterworth",
	QF_BUTT,

	NULL,
	butt,

	true,
	false,
	false,
	NULL,
	CAN_ORDER |
	CAN_ALL_ORDERS | CAN_BANDPASS,
	CAN_STOPBAND | CAN_ATTENUATION,
	0,
	0,
	QF_NONETF | QF_ZIGZAG	
};

struct qf_filter_api	qf_cheb_api = {

	"Cheby 1",
	QF_CHEB,

	chebvalid,
	cheb,

	true,
	false,
	false,
	NULL,
	CAN_ORDER |
	CAN_ALL_ORDERS | CAN_BANDPASS,
	CAN_STOPBAND | CAN_RIPPLE | CAN_ATTENUATION,
	CAN_IMPEDANCE2,
	CAN_RIPPLE,
	QF_NONETF | QF_ZIGZAG
};

  # endif //_QF_API
# endif	  //_QF_BUTCHEB_H
