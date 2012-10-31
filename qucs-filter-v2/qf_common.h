#ifndef	_QF_COMMON_H
#define	_QF_COMMON_H

//
// Some filter attributes
//

#include <cassert>
#include <complex>
#include <cmath>
#include <limits>
#include <valarray>
#include <vector>
#include <list>
#include <iostream>
#include <iomanip>
#include <string>
#include <typeinfo>
#include <q3textstream.h>

using namespace std;

typedef double			  qf_double_t;
typedef	complex <long double>	  Lcplx;
typedef	complex <qf_double_t>	  Cplx;
typedef	valarray <qf_double_t>	  Rarray;
typedef	valarray <Cplx>		  Carray;
typedef	vector  <qf_double_t>	  Rvector;
typedef	vector  <Cplx>		  Cvector;
typedef	list <qf_double_t>	  Rlist;
typedef	list <Cplx>		  Clist;

// We implement the polynom class with the following types

typedef	Clist			  qf_roots;
typedef	Rarray			  qf_coeff;

// Structure storing what is necessary to specify a given filter and
// transform

struct qf_spec {

  // What are we computing?
  unsigned	tform;
  unsigned 	filter;

  // Impedances
  qf_double_t	r1;	      // Input impedance
  qf_double_t	r2;	      // Output impedance

  // Frequencies
  qf_double_t	fc;	      // Cutoff frequency
  qf_double_t	fs;	      // Stopband corner frequency
  qf_double_t	bw;	      // Bandwidth

  qf_double_t	th;	      // Sharpness according to tform type

  // Order ?
  bool		ord_given;    // Is the order specified?
  unsigned	ord;	      // If yes
  char		subord;	      // For even order Cauer filters

  // Ripples and attenuation
  qf_double_t	amin;	      // Ripple
  qf_double_t	amax;	      // Stopband attenuation
  qf_double_t	ang;	      // Angle (for Cauer filters)

  // Optimization

  bool		optc;	      // Do we optimize Cauer filters?
  int		Copt;	      // Optimize for minimum or maximum C
  int		Lopt;	      // Optimize for minimum or maximum L

  // Various flags

  bool		dual;	      // Dual form
  bool		all_ind_equal;// All inductors are equal
  bool		use_cross;    // Use microstrip crosses rather than Ts
  bool		stop_is_pole; // The first pole location is given

  // Special

  bool		is_zigzag;    // Used by cauer to select even order only
};

typedef	  struct qf_spec	  qf_spec;
#endif
