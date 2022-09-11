/*
 * spfile.cpp - S-parameter file class implementation
 *
 * Copyright (C) 2004, 2005, 2006, 2008, 2009 Stefan Jahn <stefan@lkcc.org>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * $Id$
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "object.h"
#include "vector.h"
#include "matrix.h"
#include "matvec.h"
#include "dataset.h"
#include "strlist.h"
#include "poly.h"
#include "spline.h"
#include "interpolator.h"
#include "spfile.h"

using namespace qucs;

// Constructor for S-parameter file vector.
spfile_vector::spfile_vector () {
  v = f = 0;
  isreal = 1;
  inter = NULL;
  r = c = 0;
}

// Destructor for S-parameter file vector.
spfile_vector::~spfile_vector () {
  delete inter;
}

// Passes vectors and their data types to the S-parameter file vector.
void spfile_vector::prepare (qucs::vector * _v, qucs::vector * _f,
			     bool _isreal, int it, int dt) {
  v = _v;
  f = _f;
  isreal = _isreal;
  inter = new interpolator ();
  if (isreal) {
    inter->rvectors (v, f);
    inter->prepare (it, REPEAT_NO, dt | DATA_REAL);
  }
  else {
    inter->cvectors (v, f);
    inter->prepare (it, REPEAT_NO, dt | DATA_COMPLEX);
  }
}

// Returns interpolated data.
nr_complex_t spfile_vector::interpolate (nr_double_t x) {
  if (isreal)
    return inter->rinterpolate (x);
  else
    return inter->cinterpolate (x);
}

// Constructor creates an empty and unnamed instance of the spfile class.
spfile::spfile () {
  data = NULL;
  sfreq = nfreq = NULL;
  spara = FMIN = SOPT = RN = NULL;
  interpolType = dataType = 0;
}

// Destructor deletes spfile object from memory.
spfile::~spfile () {
  delete[] spara;
  delete RN;
  delete FMIN;
  delete SOPT;
#if DEBUG && 0
  if (data) {
    data->setFile ("spfile.dat");
    data->print ();
  }
#endif
  delete data;
}

/* This function returns the S-parameter matrix of the circuit for the
   given frequency.  It uses interpolation for frequency points which
   are not part of the original touchstone file. */
matrix spfile::getInterpolMatrixS (nr_double_t frequency) {

  // first interpolate the matrix values
  matrix s (nPorts);
  for (int r = 0; r < nPorts; r++) {
    for (int c = 0; c < nPorts; c++) {
      int i = r * (nPorts + 1) + c;
      s.set (r, c, spara[i].interpolate (frequency));
    }
  }

  // then convert them to S-parameters if necessary
  switch (paraType) {
  case 'Y':
    s = ytos (s);
    break;
  case 'Z':
    s = ztos (s);
    break;
  case 'H':
    s = htos (s);
    break;
  case 'G':
    s = gtos (s);
    break;
  }
  return s;
}

/* This function expands the actual S-parameter file data stored
   within the touchstone file to have an additional reference one-port
   whose S-parameter is -1 (i.e. ground). */
matrix spfile::expandSParaMatrix (matrix s) {
  assert (s.getCols () == s.getRows ());
  int r, c, ports = s.getCols () + 1;
  nr_double_t g = -1;
  nr_complex_t fr, ss, sr, sc, sa;
  matrix res (ports);

  // compute S'mm
  for (sa = 0, r = 0; r < ports - 1; r++)
    for (c = 0; c < ports - 1; c++) sa += s.get (r, c);
  ss = (2 - g - ports + sa) / (1 - ports * g - sa);
  res.set (ports - 1, ports - 1, ss);
  fr = (1.0 - g * ss) / (1.0 - g);

  // compute S'im
  for (r = 0; r < ports - 1; r++) {
    for (sc = 0, c = 0; c < ports - 1; c++) sc += s.get (r, c);
    res.set (r, ports - 1, fr * (1.0 - sc));
  }

  // compute S'mj
  for (c = 0; c < ports - 1; c++) {
    for (sr = 0, r = 0; r < ports - 1; r++) sr += s.get (r, c);
    res.set (ports - 1, c, fr * (1.0 - sr));
  }

  // compute S'ij
  for (r = 0; r < ports - 1; r++) {
    for (c = 0; c < ports - 1; c++) {
      fr = g * res (r, ports - 1) * res (ports - 1, c) / (1.0 - g * ss);
      res.set (r, c, s.get (r, c) - fr);
    }
  }

  return res;
}

/* The function is the counterpart of the above expandSParaMatrix()
   function.  It shrinks the S-parameter matrix by removing the
   reference port. */
matrix spfile::shrinkSParaMatrix (matrix s) {
  assert (s.getCols () == s.getRows () && s.getCols () > 0);
  int r, c, ports = s.getCols ();
  nr_double_t g = -1;
  matrix res (ports - 1);

  // compute S'ij
  for (r = 0; r < ports - 1; r++) {
    for (c = 0; c < ports - 1; c++) {
      res.set (r, c, s (r, c) + g * s (r, ports - 1)  *
	       s (ports - 1, c) / (1.0 - g * s (ports - 1, ports - 1)));
    }
  }
  return res;
}

void spfile::prepare (void) {

}

/* The function creates an additional data vector for the given matrix
   entry and adds it to the dataset. */
void spfile::createVector (int r, int c) {
  int i = r * (nPorts + 1) + c;
  spara[i].r = r;
  spara[i].c = c;
  qucs::vector * v = new qucs::vector (matvec::createMatrixString ("S", r, c),
			       sfreq->getSize ());
  v->setDependencies (new strlist ());
  v->getDependencies()->add (sfreq->getName ());
  data->addVariable (v);
  spara[i].v = v;
}

/* This function goes through the dataset stored within the original
   touchstone file and looks for the S-parameter matrices and
   frequency vector.  It also tries to find the noise parameter
   data. */
void spfile::createIndex (void) {
  qucs::vector * v; int s = nPorts + 1;
  char * n;
  const char *name;
  int r, c, i;

  // go through list of dependency vectors and find frequency vectors
  for (v = data->getDependencies (); v != NULL; v = (::vector *) v->getNext ()) {
    if ((name = v->getName ()) != NULL) {
      if (!strcmp (name, "frequency")) sfreq = v;
      else if (!strcmp (name, "nfreq")) nfreq = v;
    }
  }

  // create vector index
  spara = new spfile_vector[s * s] ();

  // go through list of variable vectors and find matrix entries
  for (v = data->getVariables (); v != NULL; v = (::vector *) v->getNext ()) {
    if ((n = matvec::isMatrixVector (v->getName (), r, c)) != NULL) {
      // save matrix vector indices
      i = r * s + c;
      spara[i].r = r;
      spara[i].c = c;
      spara[i].prepare (v, sfreq, false, interpolType, dataType);
      paraType = n[0];  // save type of touchstone data
      free (n);
    }
    if ((name = v->getName ()) != NULL) {
      // find noise parameter vectors
      if (!strcmp (name, "Rn")) {
	RN = new spfile_vector ();
	RN->prepare (v, nfreq, true, interpolType, dataType);
      }
      else if (!strcmp (name, "Fmin")) {
	FMIN = new spfile_vector ();
	FMIN->prepare (v, nfreq, true, interpolType, dataType);
      }
      else if (!strcmp (name, "Sopt")) {
	SOPT = new spfile_vector ();
	SOPT->prepare (v, nfreq, false, interpolType, dataType);
      }
    }
  }
}
