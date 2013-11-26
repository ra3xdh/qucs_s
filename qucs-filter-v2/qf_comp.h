# ifndef _QF_COMP_H
# define _QF_COMP_H

#include <vector>
#include <list>
#include <Q3TextStream>
#include <QString>

using namespace std;

class qf_lcmp;
class qf_nonetf;
class qf_lowpass;
class qf_highpass;
class qf_bandpass;
class qf_bandstop;

// Generic component value
// A generic component has two nodes
// It can be denormalized according to its own rules
// Its value can be printed using the right unit.

struct qf_cmp {

  const QString	name;	  // Human readable form
	bool	shnt;     // Is component shunt?
	bool	gnd;	  // Is component tied to ground?

  int           x, y;

		qf_cmp	(QString n, bool s = false, bool g = false): 
					name (n), shnt(s), gnd (g) {}
  virtual	~qf_cmp	(void) {};

  virtual void	dump	(unsigned, unsigned, Q3TextStream&, Q3TextStream&) = 0;

  // Denormalization according to the transformations implemented
  // By default, these do nothing

  virtual void	dnrm	(qf_nonetf *) {};
  virtual void	dnrm	(qf_lowpass *) {};
  virtual void	dnrm	(qf_highpass *) {};
  virtual void	dnrm	(qf_bandpass *) {};
  virtual void	dnrm	(qf_bandstop *) {};
};

// An object which store the topology, i.e. the graph of all components
// that make up the filter

class qf_lcmp {
  private:
  list <qf_cmp*>	    lst;
  list <qf_cmp*>::iterator  iter;

 public:
  int x, y;
  qf_double_t r1, r2, fc;
  QString tx;

  public:
  inline void	  insert    (qf_cmp* c) {lst. push_back (c);}
  void		  print	    (void);	// Dumps the graph
  void		  init	    (void);	// Access through a pseudo-iterator
  qf_cmp*	  next	    (void);
  void		  clear	    (void)  {if (lst. size () == 0) return;
				     for (iter = lst. begin ();
					   iter != lst. end ();
					   iter ++) 
				      delete *iter;
				     lst. clear (); }
  unsigned	  size	    (void)  {return lst. size ();}

  bool		  isvoid    (void) {return lst. size () == 0;}
  int		  dump_node (int&, list <qf_cmp*>::iterator&,
			     Q3TextStream&, Q3TextStream&);
  void		  dump_all  (Q3TextStream&); 

		  qf_lcmp   (void)  {};
		  ~qf_lcmp  (void)  {clear ();}

};

// A generic class of "single" component

struct qf_cmp1: public qf_cmp {

  qf_double_t	val;
  const QString	unit;

		qf_cmp1   (QString n, QString u, qf_double_t v, bool s, bool g):
				qf_cmp (n, s, g), val(v), unit (u) {}
  virtual	~qf_cmp1  (void) {};

  virtual void	dump	  (unsigned, unsigned, Q3TextStream&, Q3TextStream&);
};

// A generic class of either serial or parallel LC

struct qf_cmplc: public qf_cmp {

  qf_double_t	vC;
  qf_double_t	vL;

		qf_cmplc  (QString n, qf_double_t vc, qf_double_t vl,
			   bool s, bool g):
			   qf_cmp (n, s, g), vC(vc), vL(vl) {}
  virtual	~qf_cmplc (void) {};

  virtual void	dump	  (unsigned, unsigned, Q3TextStream&, Q3TextStream&);
};

// Fake component to signal output

struct qf_end: public qf_cmp {

		qf_end	  (void): qf_cmp ("END", false, false) {}
  inline	~qf_end   (void)  {}
  void		dump	  (unsigned, unsigned, Q3TextStream&, Q3TextStream&) {};
};

// Inductor

struct qf_ind: public qf_cmp1 {

		qf_ind	(qf_double_t v, bool s, bool g):
			 qf_cmp1 ("IND", "H", v, s, g) {}
  inline	~qf_ind	(void) {};

  // Denormalization according to the transformations implemented

  void		dnrm	(qf_nonetf *);
  void		dnrm	(qf_lowpass *);
  void		dnrm	(qf_highpass *);
  void		dnrm	(qf_bandpass *);
  void		dnrm	(qf_bandstop *);
};

// Capacitor

struct qf_cap: public qf_cmp1 {

		qf_cap  (qf_double_t v, bool s, bool g):
			 qf_cmp1 ("CAP", "F", v, s, g) {}
  inline	~qf_cap	(void) {};

  // Denormalization according to the transformations implemented

  void		dnrm	(qf_nonetf *);
  void		dnrm	(qf_lowpass *);
  void		dnrm	(qf_highpass *);
  void		dnrm	(qf_bandpass *);
  void		dnrm	(qf_bandstop *);
};

// Resistor

struct qf_res: public qf_cmp1 {

		qf_res  (qf_double_t v, bool s, bool g):
			qf_cmp1 ("RES", "Ω", v, s, g) {}
  inline	~qf_res	(void) {};

  // Denormalization according to the transformations implemented

  void		dnrm	(qf_nonetf *);
  void		dnrm	(qf_lowpass *);
  void		dnrm	(qf_highpass *);
  void		dnrm	(qf_bandpass *);
  void		dnrm	(qf_bandstop *);
};

// Serial LC
// valC is the value of the Capacitor
// valL of the inductor 

struct qf_slc: public qf_cmplc {

		qf_slc  (qf_double_t vc, qf_double_t vl, bool s, bool g):
			qf_cmplc ("SLC", vc, vl, s, g) {}
  inline	~qf_slc	(void) {};

  // Denormalization according to the transformations implemented

  void		dnrm	(qf_nonetf *);
  void		dnrm	(qf_lowpass *);
  void		dnrm	(qf_highpass *);
  void		dnrm	(qf_bandpass *);
  void		dnrm	(qf_bandstop *);
};

// Parallel LC

struct qf_plc: public qf_cmplc {
		
		qf_plc  (qf_double_t vc, qf_double_t vl, bool s, bool g):
			qf_cmplc ("PLC", vc, vl, s, g) {}
  inline	~qf_plc	(void) {};

  // Denormalization according to the transformations implemented

  void		dnrm	(qf_nonetf *);
  void		dnrm	(qf_lowpass *);
  void		dnrm	(qf_highpass *);
  void		dnrm	(qf_bandpass *);
  void		dnrm	(qf_bandstop *);
};

// Parallel SLC

struct qf_pslc : public qf_cmplc {

  qf_double_t	vC2;
  qf_double_t	vL2;

		qf_pslc (qf_double_t vc1, qf_double_t vl1, 
			 qf_double_t vc2, qf_double_t vl2,
			 bool s, bool g):
			 qf_cmplc ("PSLC", vc1, vl1, s, g), 
			 vC2(vc2), vL2 (vl2) {}
  inline	~qf_pslc(void) {}; 

  void		dump	(unsigned, unsigned, Q3TextStream&, Q3TextStream&);
};

# endif //_QF_COMP_H
