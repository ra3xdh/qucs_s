/*
 * receiver.cpp - receiver transformation class implementation
 *
 * Copyright (C) 2009 Dirk Schaefer <schad@5pm.de>
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
#include <string.h>
#include <cmath>
#include <cstdint>

#include "consts.h"
#include "object.h"
#include "complex.h"
#include "vector.h"
#include "spline.h"
#include "interpolator.h"
#include "fourier.h"
#include "receiver.h"

namespace qucs {

/* The function returns a power-of-two value which is equal or larger
   than the given value.  The maximum returned value is 2^30. */
int32_t emi::nearestbin32 (int x) {
  int32_t boundary = 1 << 30;
  int32_t current = 1;
  if (x >= boundary) return boundary;
  while (current < x) current <<= 1;
  return current;
}

/* Ideal filter construction for given center frequency, bandwidth and
   the frequency for which the filter is evaluated. */
nr_double_t emi::f_ideal (nr_double_t fc, nr_double_t bw, nr_double_t f) {
  nr_double_t lo = fc - bw / 2;
  nr_double_t hi = fc + bw / 2;
  if (f >= lo && f < hi)
    return 1.0;
  return 0.0;
}

/* Construction of a bandpass filter of 2nd order for given center
   frequency, bandwidth and the frequency for which the filter is
   evaluated. */
nr_double_t emi::f_2ndorder (nr_double_t fc, nr_double_t bw, nr_double_t f) {
  nr_double_t q = fc / bw;
  nr_complex_t p = nr_complex_t (0, f / fc);
  nr_complex_t w = p / q / (1.0 + p / q + p * p);
  return norm (w);
}

/* Construction of a gaussian filter for given center frequency,
   bandwidth and the frequency for which the filter is evaluated. */
nr_double_t emi::f_gauss (nr_double_t fc, nr_double_t bw, nr_double_t f) {
  nr_double_t a = log (0.5) / bw / bw;
  nr_double_t s = f - fc;
  return exp (a * s * s);
}

/* The function computes a EMI receiver spectrum based on the given
   waveform in the time domain.  The number of points in the waveform
   is required to be a power of two.  Also the samples are supposed
   to be equidistant. */
vector * emi::receiver (nr_double_t * ida, nr_double_t duration, int ilength) {

  int i, n, points;
  nr_double_t fres;
  vector * ed = new vector ();

  points = ilength;

  /* ilength must be a power of 2 - write wrapper later on */
  fourier::_fft_1d (ida, ilength, 1); /* 1 = forward fft */

  /* start at first AC point (0 as DC point remains untouched)
     additionally only half of the FFT result required */
  for (i = 2; i < points; i++) {
    ida[i] /= points / 2;
  }

  /* calculate frequency step */
  fres = 1.0 / duration;

  /* generate data vector; inplace calculation of magnitudes */
  nr_double_t * d = ida;
  for (n = 0, i = 0; i < points / 2; i++, n += 2){
    /* abs value of complex number */
    d[i] = xhypot (ida[n], ida[n + 1]);
    /* vector contains complex values; thus every second value */
  }

  points /= 2;

  /* define EMI settings */
  struct settings settings[] = {
    {   200, 150e3,   200,   200 },
    { 150e3,  30e6,   9e3,   9e3 },
    {  30e6,   1e9, 120e3, 120e3 },
    {     0,     0,     0,      0}
  };

  /* define EMI noise floor */
  nr_double_t noise = std::pow (10.0, (-100.0 / 40.0)) * 1e-6;

  /* generate resulting data & frequency vector */
  nr_double_t fcur, dcur;
  int ei = 0;

  /* go through each EMI setting */
  for (i = 0; settings[i].bandwidth != 0; i++ ) {

    nr_double_t bw = settings[i].bandwidth;
    nr_double_t fstart = settings[i].start;
    nr_double_t fstop = settings[i].stop;
    nr_double_t fstep = settings[i].stepsize;

    /* go through frequencies */
    for (fcur = fstart; fcur <= fstop; fcur += fstep) {

      /* calculate upper and lower frequency bounds */
      nr_double_t lo = fcur - bw / 2;
      nr_double_t hi = fcur + bw / 2;
      if (hi < fres) continue;

      /* calculate indices covering current bandwidth */
      int il = std::floor (lo / fres);
      int ir = std::floor (hi / fres);

      /* right index (ri) greater 0 and left index less than points ->
	 at least part of data is within bandwidth indices */
      if (ir >= 0 && il < points - 1) {
	/* adjust indices to reside in the data array */
	if (il < 0) il = 0;
	if (ir > points - 1) ir = points - 1;

	/* sum-up the values within the bandwidth */
	dcur = 0;
	for (int j = 0; j < ir - il; j++){
	  nr_double_t f = fres * (il + j);
	  dcur += f_2ndorder (fcur, bw, f) * d[il + j];
	}

	/* add noise to the result */
	dcur += noise * sqrt (bw);

	/* save result */
	ed->add (nr_complex_t (dcur, fcur));
	ei++;
      }
    }
  }

  /* returning values of function */
  return ed;
}

/* This is a wrapper for the basic EMI rceiver functionality.  It
   takes an arbitrary waveform in the time domain and interpolates it
   such, that its length results in a power of two elements. */
vector * emi::receiver (vector * da, vector * dt, int len) {

  int i, nlen, olen =  da->getSize ();

  // don't allow less points than the actual length
  if (len < da->getSize ()) len = da->getSize ();

  // find a power-of-two length
  nlen = emi::nearestbin32 (len);

  nr_double_t tstart = real (dt->get (0));
  nr_double_t tstop = real (dt->get (olen - 1));
  nr_double_t duration = tstop - tstart;

  /* please note: interpolation is always performed in order to ensure
     equidistant samples */

  // create interpolator (use cubic splines)
  interpolator * inter = new interpolator ();
  inter->rvectors (da, dt);
  inter->prepare (INTERPOL_CUBIC, REPEAT_NO, DATA_RECTANGULAR);

  // adjust the time domain vector using interpolation
  nr_double_t * ida = new nr_double_t[2 * nlen];
  nr_double_t tstep = duration / (nlen - 1);
  for (i = 0; i < nlen; i++) {
    nr_double_t t = i * tstep + tstart;
    ida[2 * i + 0] = inter->rinterpolate (t);
    ida[2 * i + 1] = 0;
  }

  // destroy interpolator
  delete inter;

  // run actual EMI receiver computations
  vector * res = receiver (ida, duration, nlen);

  // delete intermediate data
  delete[] ida;

  return res;
}

} // namespace qucs
