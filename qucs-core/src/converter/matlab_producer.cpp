/*
 * matlab_producer.cpp - the Matlab data file producer
 *
 * Copyright (C) 2009 Stefan Jahn <stefan@lkcc.org>
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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

#include "matlab_producer.h"
#include "matrix.h"
#include "matvec.h"
#include "constants.h"

#include <cstdint>

using namespace qucs;

/* Global variables. */
FILE * matlab_out = NULL; // output file stream
int matlab_symbols = 1;   // convert data names to have valid Matlab identifier
int nr_bigendian = 0;     // endianness

// Test endianness.
// http://www.geeksforgeeks.org/little-and-big-endian-mystery/
static void initendian (void) {
  unsigned int i = 1;
  char *c = (char*)&i;
  nr_bigendian = (*c == 1) ? 0 : 1;
}

// Writes a Matlab v4 header.
static void matlab_header (int32_t rows, int32_t cols, const char * name) {

  // MOPT
  char mopt[4];
  mopt[0] = nr_bigendian ? 1 : 0; // endianness
  mopt[1] = 0; // always zero
  switch (sizeof (nr_double_t) * 8) {
    case 32: mopt[2] = 1; break;
    case 64: mopt[2] = 0; break;
  }
  mopt[3] = 0; // numeric full matrix
  fwrite (mopt, sizeof (mopt), 1, matlab_out);

  // dimension
  fwrite (&rows, sizeof (int32_t), 1, matlab_out);
  fwrite (&cols, sizeof (int32_t), 1, matlab_out);

  // imaginary flag
  int32_t imag = 1;
  fwrite (&imag, sizeof (int32_t), 1, matlab_out);

  // data name length
  int32_t len = strlen (name) + 1;
  fwrite (&len, sizeof (int32_t), 1, matlab_out);

  char * ns = strdup(name);
  char * p = ns;
  // data name
  if (matlab_symbols) {
    // convert to valid Matlab identifiers
    for (unsigned int i = 0; i < strlen (ns); i++, p++) {
      if (!isalnum (*p) && *p != '_')
	*p = '_';
    }
  }
  fwrite (ns, 1, len, matlab_out);
  free(ns);
}

// Writes a Matlab v4 vector.
static void matlab_vector (::vector * v) {
  int n;

  // real part
  for (n = 0; n < v->getSize (); n++) {
    nr_double_t r = real (v->get (n));
    fwrite (&r, sizeof (nr_double_t), 1, matlab_out);
  }
  // imaginary part
  for (n = 0; n < v->getSize (); n++) {
    nr_double_t i = imag (v->get (n));
    fwrite (&i, sizeof (nr_double_t), 1, matlab_out);
  }
}

// Writes a Matlab v4 matrix.
static void matlab_matrix (matrix * m) {
  int r, c;

  // real part
  for (c = 0; c < m->getCols (); c++) {
    for (r = 0; r < m->getRows (); r++) {
      nr_double_t re = real (m->get (r, c));
      fwrite (&re, sizeof (nr_double_t), 1, matlab_out);
    }
  }
  // imaginary part
  for (c = 0; c < m->getCols (); c++) {
    for (r = 0; r < m->getRows (); r++) {
      nr_double_t im = imag (m->get (r, c));
      fwrite (&im, sizeof (nr_double_t), 1, matlab_out);
    }
  }
}

// Saves a dataset vector into a Matlab file.
static void matlab_save (::vector * v) {
  int r, c;
  char * n, * sn;
  const char * vn = v->getName ();
  matvec * mv = NULL;
  matrix m;

  // is vector matrix entry
  if ((n = matvec::isMatrixVector (vn, r, c)) != NULL) {
    // valid matrix vector and simple matrix
    if ((mv = matvec::getMatrixVector (v, n)) != NULL && mv->getSize () == 1) {
      // only save at first matrix entry [1,1]
      if (r == 0 && c == 0) {
	// save matrix
	matlab_header (mv->getRows (), mv->getCols (), n);
	m = mv->get (0);
	matlab_matrix (&m);
      }
    }
    else {
      // save vector
      sn = (char *) malloc (strlen (n) + 8);
      if (matlab_symbols) {
	// convert indices to valid Matlab identifiers
	sprintf (sn, "%s_%d_%d", n, r + 1, c + 1);
      } else {
	sprintf (sn, "%s", vn);
      }
      matlab_header (v->getSize (), 1, sn);
      matlab_vector (v);
      free (sn);
    }
    free (n);
    delete mv;
  }
  else {
    // save vector
    matlab_header (v->getSize (), 1, vn);
    matlab_vector (v);
  }
}

/* This is the overall Matlab producer. */
void matlab_producer (void) {

  dataset * data = qucs_data;
  qucs::vector * v;

  // initialize endianness
  initendian ();

  // independent vectors and matrices
  for (v = data->getDependencies (); v != NULL; v = (::vector *) v->getNext ()) {
    matlab_save (v);
  }

  // dependent vectors and matrices
  for (v = data->getVariables (); v != NULL; v = (::vector *) v->getNext ()) {
    matlab_save (v);
  }
}
