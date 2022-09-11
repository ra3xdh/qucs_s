/*
 * spdeembed.cpp - S-parameters de-embedding component class definitions
 *
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
#include "spdeembed.h"

using namespace qucs;

// Constructor creates an empty and unnamed instance of the spdeembed class.
spdeembed::spdeembed () : spfile () {
  type = CIR_SPDFILE;
  setVariableSized (true); // number of ports is not fixed
}

void spdeembed::initSP (void) {
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
  if (nPorts == getSize () - 1) { // size includes Ref port
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

void spdeembed::calcSP (nr_double_t frequency) {

  // nothing to do if the given file type had errors
  if (spara == NULL || sfreq == NULL) return;

  // set interpolated S-parameters
  matrix st = getInterpolMatrixS (frequency);
  int n = getSize() - 1; // size includes Ref port
  matrix si(n);
  if (n == 2) {
    // (not really a special case, but easier/faster to handle as such)
    // for 2-port S-parameters the de-embedding matrix is the
    //  inverse transposed and flipped along the anti-diagonal
    st = inverse(st);
    si.set(0, 0, st.get(1, 1));
    si.set(0, 1, st.get(1, 0));
    si.set(1, 0, st.get(0, 1));
    si.set(1, 1, st.get(0, 0));
  } else {
    // first reorder ports to group the connected and unconnected ones
    // as the standard schematic symbol has the odd ports on one side
    // and the even port on the other side
    // TODO: support generic port order (will need additional component dialog in GUI)
    int i, j;
    std::vector<int> v;
    // build ports permutation vector
    //  schematic symbol has odd ports on left side
    //  and even ports on right side
    //  need to group left side and right side together first (see docs)
    v.reserve(n);
    for (i = 0; i < (n/2); i++) // (0,2,4,6,...)
      v.push_back(2*i);
    for (i = 0; i < (n/2); i++) // (1,3,5,7...)
      v.push_back(2*i+1);

    for (i = 0; i < n; i++)
      for (j = 0; j < n; j++)
        si.set(i, j, st.get(v[i], v[j]));

    // use general anti-network formula
    int hn = n / 2; // FIXME: check size to be even?
    matrix Sll = si.getTopLeftCorner(hn, hn);
    matrix Slr = si.getTopRightCorner(hn, hn);
    matrix Srl = si.getBottomLeftCorner(hn, hn);
    matrix Srr = si.getBottomRightCorner(hn, hn);

    matrix Sall = inverse(Srr - Srl*inverse(Sll)*Slr);
    matrix Salr = inverse(Slr - Sll*inverse(Srl)*Srr);
    matrix Sarl = inverse(Srl - Srr*inverse(Slr)*Sll);
    matrix Sarr = inverse(Sll - Slr*inverse(Srr)*Srl);

    st.setTopLeftCorner(Sall, hn, hn);
    st.setTopRightCorner(Salr, hn, hn);
    st.setBottomLeftCorner(Sarl, hn, hn);
    st.setBottomRightCorner(Sarr, hn, hn);

    // reorder the ports of the anti-network S-parameters matrix
    // to bring them back to the original position
    for (i = 0; i < n; i++)
      for (j = 0; j < n; j++)
        si.set(v[i], v[j], st.get(i, j));
  }
  setMatrixS (expandSParaMatrix (si));
}

void spdeembed::initDC (void) {
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

void spdeembed::initAC (void) {
  setVoltageSources (0);
  allocMatrixMNA ();
  initSP ();
}

void spdeembed::calcAC (nr_double_t frequency) {
  // nothing to do if the given file type had errors
  if (spara == NULL || sfreq == NULL) return;
  // calculate interpolated S-parameters
  calcSP (frequency);
  // convert S-parameters to Y-parameters
  setMatrixY (stoy (getMatrixS ()));
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
struct define_t spdeembed::cirdef =
  { "SPDfile",
    PROP_NODES, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
