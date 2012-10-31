//Added by qt3to4:
#include <Q3TextStream>
# ifndef  _QF_TFORM_H
# define  _QF_TFORM_H
// Headers for standard transformations
// Those transformations are : lowpass (no transform), highpass
// standard bandpass (geometrical type) and bandstop (inverse bandpass)

extern	struct	qf_filter_api* qf_filter_apis [];
extern	struct	qf_tform_api*	qf_tform_apis [];

// Virtual holder
class qf_tform {
  protected:
  friend	      class	qf_ind;	      // To access Tspec in denorm
  friend	      class	qf_cap;	      // To access Tspec in denorm
  friend	      class	qf_res;	      // To access Tspec in denorm
  friend	      class	qf_plc;	      // To access Tspec in denorm
  friend	      class	qf_slc;	      // To access Tspec in denorm

  qf_spec*	      Tspec;
  qf_lcmp	      lcmp;
  virtual qf_double_t th	(void) = 0;

  virtual 	      ~qf_tform (void) {}

		      qf_tform	(qf_spec* Ts) : Tspec (Ts) {};
  public:

  virtual void	      dump	(Q3TextStream&) = 0;
};

// A common static function to dispatch denormalization

void		     denorm	(qf_tform*, qf_cmp*);

// API for transformation

struct	qf_tform_api {
  QString     name;
  unsigned    id;

  bool	      (* valid) (qf_spec*);		    // Validator
  qf_tform*   (* cons)	(qf_spec*);		    // Constructor

  unsigned    gen_flags;	  // General capacity
  unsigned    if_no_order;	  // Capacity if order is not specified
  unsigned    even;		  // Capacity for even orders
  unsigned    odd;		  // Capacity for odd orders
};

// Lowpass

class qf_lowpass : public qf_tform {
  protected:
		  qf_lowpass  (qf_spec* Ts);
      virtual	  ~qf_lowpass (void)	    {}
      qf_double_t th	      (void)	    {return Tspec -> fs / Tspec -> fc;}

  public:

  // Dump
  virtual void	  dump	      (Q3TextStream&);

friend qf_tform*  lowpass     (qf_spec* Tspec) {return new qf_lowpass (Tspec);}
};

qf_tform*	  lowpass     (qf_spec*);
bool		  lowpassvalid(qf_spec*);

// None transform (does nothing) but is a child of lowpass

class qf_nonetf : public qf_tform {
  private:
		  qf_nonetf   (qf_spec* Ts);
		  ~qf_nonetf  (void)	      {}
    qf_double_t	  th	      (void)	      {return 0;}

  public:

      void	  dump	      (Q3TextStream&);
friend qf_tform*  nonetf      (qf_spec* Tspec) {return new qf_nonetf (Tspec);}
};

qf_tform*	  nonetf      (qf_spec*);


// Highpass
class qf_highpass : public qf_tform {
  private:
		  qf_highpass  (qf_spec* Ts);
		  ~qf_highpass (void)	  {}
      qf_double_t th	       (void)	  {return Tspec -> fc / Tspec -> fs;}

  public:

  // Dump
  void		  dump	      (Q3TextStream&);

friend qf_tform*  highpass     (qf_spec* spec) {return new qf_highpass (spec);}
};

qf_tform*	  highpass     (qf_spec*);
bool		  highpassvalid(qf_spec*);

// Bandpass
class qf_bandpass : public qf_tform {
  private:
		  qf_bandpass  (qf_spec* s);
		  ~qf_bandpass (void)	    {}
      qf_double_t th	       (void)	    {return 
					     fabs (Tspec -> fs -
						   Tspec -> fc * Tspec -> fc /
						   Tspec -> fs) / Tspec -> bw;}
  public:

  // Dump
  void		  dump	      (Q3TextStream&);

friend qf_tform*  bandpass     (qf_spec* spec) {return new qf_bandpass (spec);}
};

qf_tform*	  bandpass     (qf_spec*);
bool		  bandpassvalid(qf_spec*);

// Bandstop
class qf_bandstop: public qf_tform {
  private:
		  qf_bandstop  (qf_spec* s);
		  ~qf_bandstop (void)	    {}
      qf_double_t th	       (void)	    {qf_double_t 
					      sh = Tspec -> fs / Tspec -> fc;
					      qf_double_t	
					       bw = Tspec -> bw / Tspec -> fc;
					       Tspec -> bw = 
						fabs (sh - 1/sh) * Tspec -> fc;
					       return 1/fabs (
						sh * bw / (sh * sh - 1));}

  public:

  // Dump
  void		  dump	      (Q3TextStream&);

friend qf_tform*  bandstop     (qf_spec* spec) {return new qf_bandstop (spec);}
};

qf_tform*	  bandstop     (qf_spec*);
bool		  bandstopvalid(qf_spec*);

 # ifdef  _QF_API

struct	qf_tform_api	  qf_nonetf_api = {

  "None",
  QF_NONETF,

  NULL,
  nonetf,

  CAN_ORDER | CAN_RIPPLE | CAN_ATTENUATION | CAN_IMPEDANCE2 |
  CAN_ANGLE | CAN_ALL_ORDERS | CAN_SUBORDER | CAN_STOPBAND | CAN_BANDPASS |
  CAN_ORDER_ONLY | CAN_BOTH_RPL_AND_I2,
  0,
  0,
  0
};


struct	qf_tform_api	  qf_lowpass_api = {

  "Lowpass",
  QF_LOWPASS,

  lowpassvalid,
  lowpass,

  CAN_ORDER | CAN_RIPPLE | CAN_ATTENUATION | CAN_IMPEDANCE2 |
  CAN_ANGLE | CAN_ALL_ORDERS | CAN_SUBORDER | CAN_STOPBAND |
  CAN_ORDER_ONLY | CAN_BOTH_RPL_AND_I2,
  0,
  0,
  0
};

struct	qf_tform_api	  qf_highpass_api = {

  "Highpass",
  QF_HIGHPASS,

  highpassvalid,
  highpass,

  CAN_ORDER | CAN_RIPPLE | CAN_ATTENUATION | CAN_IMPEDANCE2 |
  CAN_ANGLE | CAN_ALL_ORDERS | CAN_SUBORDER | CAN_STOPBAND |
  CAN_ORDER_ONLY | CAN_BOTH_RPL_AND_I2,
  0,
  0,
  0
};

struct	qf_tform_api	  qf_bandpass_api = {

  "Bandpass",
  QF_BANDPASS,

  bandpassvalid,
  bandpass,

  CAN_ORDER | CAN_RIPPLE | CAN_ATTENUATION | CAN_IMPEDANCE2 |
  CAN_ANGLE | CAN_ALL_ORDERS | CAN_SUBORDER | CAN_BANDPASS |
  CAN_STOPBAND | CAN_ORDER_ONLY | CAN_BOTH_RPL_AND_I2,
  0,
  0,
  0
};

struct	qf_tform_api	  qf_bandstop_api = {

  "Bandstop",
  QF_BANDSTOP,

  bandpassvalid,
  bandstop,

  CAN_RIPPLE | CAN_ATTENUATION | CAN_IMPEDANCE2 | CAN_ANGLE | CAN_BANDPASS |
  CAN_STOPBAND | CAN_BOTH_RPL_AND_I2,
  0,
  0,
  0
};

  # endif // _QF_API
# endif	  // _QF_TFORM_H
