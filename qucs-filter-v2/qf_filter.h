/***************************************************************************
                               qf_filter.h
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

# ifndef _QF_FILTER_H
# define _QF_FILTER_H

# ifdef	_QF_QUCSFILTER_TEST
# define PACKAGE_VERSION "Qf test"
# endif

# include <q3textstream.h>

using namespace std;

// Maximum order of any filter
const unsigned			  QF_MAX_ORD = 19;

// Structure used to represent 3 doubles

struct	qf_trp {
  qf_double_t v;
  qf_double_t w1;
  qf_double_t w2;
};

// Generic filter class

class qf_filter
{
 friend		     class  qf_zigzag;
 protected :

  qf_spec*	      Pspec;			// Filter data

  // Polynomial description

  qf_poly	      E;			// H(W) = E(W)/P(W)
  qf_poly	      F;			// D(W) = F(W)/P(W)
  qf_poly	      P;

  qf_poly	      BN;			// B(W) susceptance of filter
  qf_poly	      BD;			// B(W) susceptance of filter

  qf_poly	      BNc;			// Copies of the former
  qf_poly	      BDc;

  qf_lcmp	      lcmp;

  // Constructor and co
		      qf_filter	  (qf_spec*);
		      qf_filter	  (const qf_filter &);

  // Calculates the susceptance of the normal filter out of its
  // characteristic polynoms
  void		      scptX1	  (qf_poly&, qf_poly&, qf_poly&, qf_poly&);
  void		      scptX2	  (qf_poly&, qf_poly&, qf_poly&, qf_poly&);
  void		      scptX3	  (qf_poly&, qf_poly&, qf_poly&, qf_poly&);

  // Common routines to perform extraction of poles and zeros
  // This one extract a finite pole of transmission
  bool		      pole_finite (qf_double_t, qf_double_t, qf_trp*);
	    
  // Those extract poles at infinity
  bool		      pole_inf_c  (qf_double_t&, qf_double_t);
  bool		      pole_inf_l  (qf_double_t&, qf_double_t);

  // and poles at zero
  bool		      pole_zero_c (qf_double_t&, qf_double_t);
  bool		      pole_zero_l (qf_double_t&, qf_double_t);

  // Synthesis of all types of ladder filters

  bool		      synth_ladder (qf_double_t);

  // Optimization routines

  qf_double_t	      opt_error   (qf_double_t&, qf_double_t&,
				   qf_double_t&, qf_double_t&);

  void		      get_bounds  (qf_double_t&, qf_double_t&,
				   qf_double_t&, qf_double_t&);

 public:
  virtual 	      ~qf_filter  (void) {}
  bool		      islvoid	  (void) {return lcmp. isvoid ();}
  void		      linit	  (void) {lcmp. init ();}
  qf_cmp*	      lnext	  (void) {return lcmp. next ();}

  // Every filter must provide a synth interface
  virtual bool	      synth	  (void) = 0;
};

// Api for filters

struct	qf_filter_api {
  QString     name;
  unsigned    id;

  bool	      (* valid) (qf_spec*);   // Validator
  qf_filter*  (* cons)  (qf_spec*);   // Constructor, returns transfer function

  bool	      is_all_pole;	      // Is it an all pole filter?
  bool	      forbid_orders;	      // Are some orders forbidden?
  bool	      only_some_orders;	      // If only some orders are allowed
  const char* f_ord_list;	      // Forbidden/Allowed order list
  unsigned    gen_flags;	      // General capacity
  unsigned    if_no_order;	      // Capacity if order is not specified
  unsigned    even;		      // Capacity for even orders
  unsigned    odd;		      // Capacity for odd orders
  unsigned    forbid_tform;	      // Incompatible transforms
};

extern struct	qf_filter_api*	qf_filter_apis [];
# endif	// _QF_FILTER
