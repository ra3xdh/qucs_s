/*
 * fourier.cpp - fourier transformation class implementation
 *
 * Copyright (C) 2005, 2006, 2009 Stefan Jahn <stefan@lkcc.org>
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

#include "consts.h"
#include "object.h"
#include "complex.h"
#include "vector.h"
#include "fourier.h"

// Helper macro.
#define Swap(a,b) { nr_double_t t; t = a; a = b; b = t; }

namespace qucs {

using namespace fourier;

/* The function performs a 1-dimensional fast fourier transformation.
   Each data item is meant to be defined in equidistant steps.  The
   number of data items needs to be of binary size, e.g. 64, 128. */
void fourier::_fft_1d (nr_double_t * data, int len, int isign) {

  // bit reversal method
  int i, j, m, n;
  n = 2 * len;
  j = 0;
  for (i = 0; i < n; i += 2) {
    if (j > i) {                   // was index already swapped ?
      Swap (data[j], data[i]);     // swap real part
      Swap (data[j+1], data[i+1]); // swap imaginary part
    }
    m = len;
    while (m >= 2 && j >= m) {     // calculate next swap index
      j -= m;
      m >>= 1;
    }
    j += m;
  }

  // Danielson-Lanzcos algorithm
  int mmax, istep;
  nr_double_t wt, th, wr, wi, wpr, wpi, ti, tr;
  mmax = 2;
  while (n > mmax) {
    istep = mmax << 1;
    th = isign * (2 * pi / mmax);
    wt = sin (0.5 * th);
    wpr = -2.0 * wt * wt;
    wpi = sin (th);
    wr = 1.0;
    wi = 0.0;
    for (m = 1; m < mmax; m += 2) {
      for (i = m; i <= n; i += istep) {
	j = i + mmax;
	tr = wr * data[j] - wi * data[j-1];
	ti = wr * data[j-1] + wi * data[j];
	data[j] = data[i] - tr;
	data[j-1] = data[i-1] - ti;
	data[i] += tr;
	data[i-1] += ti;
      }
      wr = (wt = wr) * wpr - wi * wpi + wr;
      wi = wi * wpr + wt * wpi + wi;
    }
    mmax = istep;
  }
}

/* The function transforms two real vectors using a single fast
   fourier transformation step.  The routine works in place. */
void fourier::_fft_1d_2r (nr_double_t * r1, nr_double_t * r2, int len) {
  int n3, n2, j;
  nr_double_t rep, rem, aip, aim;
  n3 = 1 + (n2 = len + len);

  // put the two real vectors into one complex vector
  for (j = 1; j <= n2; j += 2) {
    r1[j] = r2[j-1];
  }

  // transform the complex vector
  _fft_1d (r1, len, 1);

  // separate the two transforms
  r2[0] = r1[1];
  r1[1] = r2[1] = 0.0;
  for (j = 2; j <= len; j += 2) {
    // use symmetries to separate the two transforms
    rep = 0.5 * (r1[j] + r1[n2-j]);
    rem = 0.5 * (r1[j] - r1[n2-j]);
    aip = 0.5 * (r1[j+1] + r1[n3-j]);
    aim = 0.5 * (r1[j+1] - r1[n3-j]);
    // ship them out in two complex vectors
    r1[j+1] = aim;
    r2[j+1] = -rem;
    r1[j] = r1[n2-j] = rep;
    r2[j] = r2[n2-j] = aip;
    r1[n3-j] = -aim;
    r2[n3-j] = rem;
  }
}

/* The following function transforms two vectors yielding real valued
   vectors using a single inverse fast fourier transformation step.
   The routine works in place as well. */
void fourier::_ifft_1d_2r (nr_double_t * r1, nr_double_t * r2, int len) {
  nr_double_t r, i;
  int j, jj, nn = len + len;

  // put the two complex vectors into one complex vector
  for (j = 0, jj = 0; j < nn; j += 2) {
    r = r1[j] - r2[j+1];
    i = r1[j+1] + r2[j];
    r1[jj++] = r;
    r1[jj++] = i;
  }

  // transform the complex vector
  _fft_1d (r1, len, -1);

  // split the transforms into two real vectors
  for (j = 0; j < nn; j += 2) {
    r2[j] = r1[j+1];
    r1[j+1] = r2[j+1] = 0.0;
  }
}

/* This function performs a 1-dimensional fast fourier transformation
   on the given vector 'var'.  If 'sign' is -1 the inverse fft is
   computed, if +1 the fft itself is computed.  It returns a vector of
   binary size (as necessary for a fft). */
vector fourier::fft_1d (vector var, int isign) {
  int i, n, len = var.getSize ();

  // compute necessary binary data array size
  int size = 2;
  while (size < len) size <<= 1;

  // put data items (temporary array) in place
  nr_double_t * data;
  data = (nr_double_t *) calloc (2 * size * sizeof (nr_double_t), 1);
  for (n = i = 0; i < len; i++, n += 2) {
    data[n] = real (var (i)); data[n+1] = imag (var (i));
  }

  // run 1-dimensional fft
  _fft_1d (data, size, isign);

  // store transformed data items in result vector
  vector res = vector (size);
  for (n = i = 0; i < size; i++, n += 2) {
    res (i) = nr_complex_t (data[n], data[n+1]);
    if (isign < 0) res (i) /= size;
  }

  // free temporary data array
  free (data);
  return res;
}

/* The function performs a 1-dimensional discrete fourier
   transformation.  Each data item is meant to be defined in
   equidistant steps. */
void fourier::_dft_1d (nr_double_t * data, int len, int isign) {
  int k, n, size = 2 * len * sizeof (nr_double_t);
  nr_double_t * res = (nr_double_t *) calloc (size, 1);
  nr_double_t th, c, s;
  for (n = 0; n < 2 * len; n += 2) {
    th = n * pi / 2 / len;
    for (k = 0; k < 2 * len; k += 2) {
      c = cos (k * th);
      s = isign * sin (k * th);
      res[n] += data[k] * c + data[k+1] * s;
      res[n+1] += data[k+1] * c - data[k] * s;
    }
  }
  memcpy (data, res, size);
  free (res);
}

/* The function performs a 1-dimensional discrete fourier
   transformation on the given vector 'var'.  If 'sign' is -1 the
   inverse dft is computed, if +1 the dft itself is computed. */
vector fourier::dft_1d (vector var, int isign) {
  int k, n, len = var.getSize ();
  vector res = vector (len);
  for (n = 0; n < len; n++) {
    nr_double_t th = - isign * 2 * pi * n / len;
    nr_complex_t val = 0;
    for (k = 0; k < len; k++)
      val += var (k) * std::polar (1.0, th * k);
    res (n) = isign < 0 ? val / (nr_double_t) len : val;
  }
  return res;
}

// Helper functions.
vector fourier::ifft_1d (vector var) {
  return fft_1d (var, -1);
}
vector fourier::idft_1d (vector var) {
  return dft_1d (var, -1);
}
void fourier::_ifft_1d (nr_double_t * data, int len) {
  _fft_1d (data, len, -1);
}
void fourier::_idft_1d (nr_double_t * data, int len) {
  _dft_1d (data, len, -1);
}

/* The function performs a n-dimensional fast fourier transformation.
   Each data item is meant to be defined in equidistant steps.  The
   number of data items needs to be of binary size, e.g. 64, 128 for
   each dimension. */
void fourier::_fft_nd (nr_double_t * data, int len[], int nd, int isign) {

 int i, i1, i2, i3, i2rev, i3rev, ip1, ip2, ip3, ifp1, ifp2;
 int ibit, k1, k2, n, np, nr, nt;

 // compute total number of complex values
 for (nt = 1, i = 0; i < nd; i++) nt *= len[i];

 // main loop over the dimensions
 for (np = 1, i = nd - 1; i >= 0; i--) {
   n = len[i];
   nr = nt / (n * np);
   ip1 = np << 1;
   ip2 = ip1 * n;
   ip3 = ip2 * nr;

   // bit-reversal method
   for (i2rev = 1, i2 = 1; i2 <= ip2; i2 += ip1) {
     if (i2 < i2rev) {
       for (i1 = i2; i1 <= i2 + ip1 - 2; i1 += 2) {
	 for (i3 = i1; i3 <= ip3; i3 += ip2) {
	   i3rev = i2rev + i3 - i2;
	   Swap (data[i3-1], data[i3rev-1]);
	   Swap (data[i3], data[i3rev]);
	 }
       }
     }
     ibit = ip2 >> 1;
     while (ibit >= ip1 && i2rev > ibit) {
       i2rev -= ibit;
       ibit >>= 1;
     }
     i2rev += ibit;
   }

   // Danielson-Lanzcos algorithm
   nr_double_t ti, tr, wt, th, wr, wi, wpi, wpr;
   ifp1 = ip1;
   while (ifp1 < ip2) {
     ifp2 = ifp1 << 1;
     th = isign * 2 * pi / (ifp2 / ip1);
     wt = sin (0.5 * th);
     wpr = -2.0 * wt * wt;
     wpi = sin (th);
     wr = 1.0;
     wi = 0.0;
     for (i3 = 1; i3 <= ifp1; i3 += ip1) {
       for (i1 = i3; i1 <= i3 + ip1 - 2; i1 += 2) {
	 for (i2 = i1; i2 <= ip3; i2 += ifp2) {
	   k1 = i2;
	   k2 = k1 + ifp1;
	   tr = wr * data[k2-1] - wi * data[k2];
	   ti = wr * data[k2] + wi * data[k2-1];
	   data[k2-1] = data[k1-1] - tr;
	   data[k2] = data[k1] - ti;
	   data[k1-1] += tr;
	   data[k1] += ti;
	 }
       }
       wr = (wt = wr) * wpr - wi * wpi + wr;
       wi = wi * wpr + wt * wpi + wi;
     }
     ifp1 = ifp2;
   }
   np *= n;
 }
}

// Helper functions.
void fourier::_ifft_nd (nr_double_t * data, int len[], int nd) {
  _fft_nd (data, len, nd, -1);
}

// Shuffles values of FFT around.
vector fourier::fftshift (vector var) {
  int i, n, len = var.getSize ();
  vector res = vector (len);
  n = len / 2;
  for (i = 0; i < len / 2; i++) {
    res (i) = var (n + i);
    res (i + n) = var (i);
  }
  return res;
}

} // namespace qucs
