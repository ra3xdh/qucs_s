/*
 * spembed.cpp - S-parameters embedding component class definitions
 *
 * Copyright (C) 2004, 2005, 2006, 2008, 2009 Stefan Jahn <stefan@lkcc.org>
 * Copyright (C) 2017 Qucs Team
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
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "component.h"
#include "matvec.h"
#include "dataset.h"
#include "strlist.h"
#include "poly.h"
#include "spline.h"
#include "interpolator.h"
#include "spembed.h"

using namespace qucs;

// Constructor creates an empty and unnamed instance of the spembed class.
spembed::spembed () : spfile () {
  type = CIR_SPFILE;
  setVariableSized (true); // number of ports is not fixed
}

void spembed::initSP (void) {
  // allocate S-parameter matrix
  allocMatrixS ();
  
  // initialize data
  // check type of data
  const char * const dtype = getPropertyString ("Data");
  if (!strcmp (dtype, "rectangular")) {
    // rectangular data
    dataType = DATA_RECTANGULAR;
  }
  else if (!strcmp (dtype, "polar")) {
    // polar data
    dataType = DATA_POLAR;
  }

  // check type of interpolator
  const char * const itype = getPropertyString ("Interpolator");
  if (!strcmp (itype, "linear")) {
    interpolType = INTERPOL_LINEAR;
  }
  else if (!strcmp (itype, "cubic")) {
    interpolType = INTERPOL_CUBIC;
  }

  //prepare (interpolType, dataType);

  // load S-parameter file
  const char * file = getPropertyString ("File");
  if (data == NULL) data = dataset::load_touchstone (file);
  if (data != NULL) {
    // determine the number of ports defined by that file
    nPorts = (int) std::sqrt ((double) data->countVariables ());
  }
  if (nPorts == getSize () - 1) {
    if (spara == NULL) {
      // find matrix vector entries in touchstone dataset
      createIndex ();
    }
    if (sfreq == NULL) {
      logprint (LOG_ERROR, "ERROR: file `%s' contains no `frequency' "
                "vector\n", file);
    }
  }
  else {
    logprint (LOG_ERROR, "ERROR: file `%s' specifies a %d-port, `%s' "
              "requires a %d-port\n", file, nPorts, getName (),
              getSize () - 1);
  }
}

void spembed::calcSP (nr_double_t frequency) {

  // nothing to do if the given file type had errors
  if (spara == NULL || sfreq == NULL) return;

  // set interpolated S-parameters
  setMatrixS (expandSParaMatrix (getInterpolMatrixS (frequency)));
}

void spembed::calcNoiseSP (nr_double_t frequency) {
  // nothing to do if the given file type had errors
  if (spara == NULL || nfreq == NULL) return;
  setMatrixN (calcMatrixCs (frequency));
}

matrix spembed::calcMatrixCs (nr_double_t frequency) {
  // set interpolated noise correlation matrix
  nr_double_t r = real (RN->interpolate (frequency));
  nr_double_t f = real (FMIN->interpolate (frequency));
  nr_complex_t g = SOPT->interpolate (frequency);
  matrix s = getInterpolMatrixS (frequency);
  matrix n = correlationMatrix (f, g, r, s);
  matrix c = expandNoiseMatrix (n, expandSParaMatrix (s));
  return c;
}

/* This function expands the actual noise correlation matrix to have an
   additional reference one-port whose S-parameter is -1
   (i.e. ground).  The given S-parameter matrix is required to perform
   this transformation and is obtained using the expandSParaMatrix()
   function. */
matrix spembed::expandNoiseMatrix (matrix n, matrix s) {
  assert (s.getCols () == s.getRows () && n.getCols () == n.getRows () &&
	  n.getCols () == s.getCols () - 1);
  nr_double_t T = getPropertyDouble ("Temp");
  int r, c, ports = n.getCols () + 1;
  nr_double_t g = -1;

  // create K matrix
  matrix k (ports, ports - 1);
  for (r = 0; r < ports - 1; r++) {
    for (c = 0; c < ports - 1; c++) {
      if (r == c)
	k.set (r, c, 1.0 + g * (s.get (r, ports - 1) - 1.0));
      else
	k.set (r, c, g * s.get (r, ports - 1));
    }
  }
  for (c = 0; c < ports - 1; c++)
    k.set (ports - 1, c, g * s.get (ports - 1, ports - 1) - 1.0);

  // create D vector
  matrix d (ports, 1);
  for (r = 0; r < ports - 1; r++) d.set (r, 0, s.get (r, ports - 1));
  d.set (ports - 1, 0, s.get (ports - 1, ports - 1) - 1.0);

  // expand noise correlation matrix
  matrix res (ports);
  res = (k * n * adjoint (k) - celsius2kelvin (T) / T0 * fabs (1 - norm (g)) *
	 d * adjoint (d)) * norm (1 / (1 - g));
  return res;
}

/* The function is the counterpart of the above expandNoiseMatrix()
   function.  It shrinks the noise correlation matrix by removing the
   reference port.  The given S-parameter matrix is required to perform
   this transformation and is obtained using the expandSParaMatrix()
   function. */
matrix spembed::shrinkNoiseMatrix (matrix n, matrix s) {
  assert (s.getCols () == s.getRows () && n.getCols () == n.getRows () &&
	  n.getCols () == s.getCols () && n.getCols () > 0);
  int r, ports = n.getCols ();
  nr_double_t g = -1;
  nr_double_t T = getPropertyDouble ("Temp");

  // create K' matrix
  matrix k (ports - 1, ports);
  for (r = 0; r < ports - 1; r++) k.set (r, r, 1);
  for (r = 0; r < ports - 1; r++)
    k.set (r, ports - 1, g * s.get (r, ports - 1) /
	   (1.0 - g * s.get (ports - 1, ports - 1)));

  // create D' vector
  matrix d (ports - 1, 1);
  for (r = 0; r < ports - 1; r++) d.set (r, 0, s.get (r, ports - 1));

  // shrink noise correlation matrix
  matrix res (ports - 1);
  res = k * n * adjoint (k) + celsius2kelvin (T) / T0 * fabs (1.0 - norm (g)) /
    norm (1.0 - g * s.get (ports - 1, ports - 1)) * d * adjoint (d);
  return res;
}

/* This function computes the noise correlation matrix of a twoport
   based upon the noise parameters and the given S-parameter
   matrix. */
matrix spembed::correlationMatrix (nr_double_t Fmin, nr_complex_t Sopt,
				  nr_double_t Rn, matrix s) {
  assert (s.getCols () == s.getRows () && s.getCols () == 2);
  matrix c (2);
  nr_complex_t Kx = 4 * Rn / z0 / norm (1.0 + Sopt);
  c.set (0, 0, (Fmin - 1) * (norm (s.get (0, 0)) - 1) +
	 Kx * norm (1.0 - s.get (0, 0) * Sopt));
  c.set (1, 1, norm (s.get (1, 0)) * ((Fmin - 1) + Kx * norm (Sopt)));
  c.set (0, 1, s.get (0, 0) / s.get (1, 0) * c.get (1, 1) -
	 conj (s.get (1, 0)) * conj (Sopt) * Kx);
  c.set (1, 0, conj (c.get (0, 1)));
  return c;
}

/* The function computes the noise figure and noise parameters for the
   given S-parameter and noise correlation matrices of a twoport. */
nr_double_t spembed::noiseFigure (matrix s, matrix c, nr_double_t& Fmin,
				 nr_complex_t& Sopt, nr_double_t& Rn) {
  assert (s.getCols () == s.getRows () && c.getCols () == c.getRows () &&
	  s.getCols () == 2 && c.getCols () == 2);
  nr_complex_t n1, n2;
  n1 = c.get (0, 0) * norm (s.get (1, 0)) -
    2 * real (c.get (0, 1) * s.get (1, 0) * conj (s.get (0, 0))) +
    c.get (1, 1) * norm (s.get (0, 0));
  n2 = 2.0 * (c.get (1, 1) * s.get (0, 0) -
	      c.get (0, 1) * s.get (1, 0)) / (c.get (1, 1) + n1);

  // optimal source reflection coefficient
  Sopt = 1 - norm (n2);
  if (real (Sopt) < 0.0)
    Sopt = (1.0 + std::sqrt (Sopt)) / n2;  // avoid a negative radicant
  else
    Sopt = (1.0 - std::sqrt (Sopt)) / n2;

  // minimum noise figure
  Fmin = real (1.0 + (c.get (1, 1) - n1 * norm (Sopt)) /
	       norm (s.get (1, 0)) / (1.0 + norm (Sopt)));

  // equivalent noise resistance
  Rn = real ((c (0, 0) - 2.0 *
	      real (c (0, 1) * conj ((1.0 + s (0, 0)) / s (1, 0))) +
	      c (1, 1) * norm ((1.0 + s (0, 0)) / s (1, 0))) / 4.0);
  Rn = Rn * z0;

  // noise figure itself
  return real (1.0 + c.get (1, 1) / norm (s.get (1, 0)));
}

void spembed::calcNoiseAC (nr_double_t frequency) {
  // nothing to do if the given file type had errors
  if (spara == NULL || nfreq == NULL) return;
  setMatrixN (cstocy (calcMatrixCs (frequency), getMatrixY () * z0) / z0);
}

void spembed::initDC (void) {
  // get appropriate property value
  const char * const dc = getPropertyString ("duringDC");

  // a short during DC including the reference node
  if (!strcmp (dc, "shortall")) {
    int v, n, lastnode = getSize () - 1;
    setVoltageSources (lastnode);
    allocMatrixMNA ();
    // place zero voltage sources
    for (v = VSRC_1, n = NODE_1; n < lastnode; n++, v++) {
      voltageSource (v, n, lastnode);
    }
    return;
  }
  // a short during DC excluding the reference node
  if (!strcmp (dc, "short")) {
    int v, n, lastnode = getSize () - 2;
    setVoltageSources (lastnode);
    allocMatrixMNA ();
    // place zero voltage sources
    for (v = VSRC_1, n = NODE_1; n < lastnode; n++, v++) {
      voltageSource (v, n, lastnode);
    }
    return;
  }
  // an open during DC
  else if (!strcmp (dc, "open")) {
    setVoltageSources (0);
    allocMatrixMNA ();
    return;
  }
  // none specified, DC value of IDFT ?
  else {
    setVoltageSources (0);
    allocMatrixMNA ();
  }
}

void spembed::initAC (void) {
  setVoltageSources (0);
  allocMatrixMNA ();
  initSP ();
}

void spembed::calcAC (nr_double_t frequency) {
  // nothing to do if the given file type had errors
  if (spara == NULL || sfreq == NULL) return;
  // calculate interpolated S-parameters
  calcSP (frequency);
  // convert S-parameters to Y-parameters
  setMatrixY (stoy (getMatrixS ()));
}

void spembed::initTR (void) {
  initDC ();
}

// properties
PROP_REQ [] = {
  { "File", PROP_STR, { PROP_NO_VAL, "spfile.snp" }, PROP_NO_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Data", PROP_STR, { PROP_NO_VAL, "polar" },
    PROP_RNG_STR2 ("rectangular", "polar") },
  { "Interpolator", PROP_STR, { PROP_NO_VAL, "linear" },
    PROP_RNG_STR2 ("linear", "cubic") },
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  { "duringDC", PROP_STR, { PROP_NO_VAL, "open" },
    PROP_RNG_STR4 ("open", "short", "shortall", "unspecified") },
  PROP_NO_PROP };
struct define_t spembed::cirdef =
  { "SPfile",
    PROP_NODES, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
