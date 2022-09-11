/*
 * transient.cpp - transient helper class implementation
 *
 * Copyright (C) 2004, 2006 Stefan Jahn <stefan@lkcc.org>
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

#include "object.h"
#include "complex.h"
#include "circuit.h"
#include "net.h"
#include "tvector.h"
#include "tmatrix.h"
#include "eqnsys.h"
#include "transient.h"

#define COEFFDEBUG 0
#define FIXEDCOEFF 0

// Defines where the equivalent admittance coefficient is going to be stored.
#define COEFF_G 0

namespace qucs {

using namespace transient;

/* The function calculates the integration coefficient for numerical
   integration methods.  Supported methods are: Gear (order 1-6),
   Trapezoidal, backward Euler and Adams-Moulton (order 1-6). */
void transient::calcCorrectorCoeff (int Method, int order,
				    nr_double_t * coefficients,
				    nr_double_t * delta) {

  tmatrix<nr_double_t> A (order + 1);
  tvector<nr_double_t> x (order + 1);
  tvector<nr_double_t> b (order + 1);
  eqnsys<nr_double_t> e;
  e.setAlgo (ALGO_LU_DECOMPOSITION);

  switch (Method) {
  case INTEGRATOR_GEAR: // GEAR order 1 to 6
    {
#if FIXEDCOEFF
      int i, r, c;
      // right hand side vector
      for (i = 0; i < order + 1; i++) b.set (i, 1);
      for (i = 1; i < order + 1; i++) {
	A.set (i, 0, i); // first column
	A.set (0, i, 1); // first row
      }
      for (c = 1; c <= order - 1; c++) {
	nr_double_t entry = -c;
	for (r = 1; r <= order; r++) {
	  A.set (r, c + 1, entry);
	  entry *= -c;
	}
      }
      e.passEquationSys (&A, &x, &b);
      e.solve ();

      // vector x consists of b_{-1}, a_{0}, a_{1} ... a_{k-1} right here
#if COEFFDEBUG
      logprint (LOG_STATUS, "DEBUG: Gear order %d:", order);
      for (i = 0; i < x.getRows (); i++) {
	logprint (LOG_STATUS, " %g", x.get (i));
      }
      logprint (LOG_STATUS, "\n");
#endif
      nr_double_t k = x.get (0);
      coefficients[COEFF_G] = 1 / delta[0] / k;
      for (i = 1; i <= order; i++) {
	coefficients[i] = - 1 / delta[0] / k * x.get (i);
      }
#else /* !FIXEDCOEFF */
      int c, r;
      // right hand side vector
      b.set (1, -1 / delta[0]);
      // first row
      for (c = 0; c < order + 1; c++) A.set (0, c, 1);
      nr_double_t f, a;
      for (f = 0, c = 0; c < order; c++) {
	f += delta[c];
	for (a = 1, r = 0; r < order; r++) {
	  a *= f / delta[0];
	  A.set (r + 1, c + 1, a);
	}
      }
      e.passEquationSys (&A, &x, &b);
      e.solve ();
      for (r = 0; r <= order; r++) coefficients[r] = x.get (r);
#endif /* !FIXEDCOEFF */
    }
    break;
  case INTEGRATOR_EULER: // BACKWARD EULER
    coefficients[COEFF_G] = 1 / delta[0];
    coefficients[1] = - 1 / delta[0];
    break;
  case INTEGRATOR_TRAPEZOIDAL: // TRAPEZOIDAL (bilinear)
    coefficients[COEFF_G] = 2 / delta[0];
    coefficients[1] = - 2 / delta[0];
    break;
  case INTEGRATOR_ADAMSMOULTON: // ADAMS-MOULTON order 1 to 6
    {
      int i, r, c;
      // right hand side vector
      for (i = 0; i < order + 1; i++) b.set (i, 1);
      for (i = 1; i < order + 1; i++) {
	A.set (i, 1, i); // second column
	A.set (1, i, 1); // second row
      }
      A.set (0, 0, 1);
      for (c = 1; c <= order - 2; c++) {
	nr_double_t entry = -c;
	for (r = 2; r <= order; r++) {
	  A.set (r, c + 2, r * entry);
	  entry *= -c;
	}
      }
      e.passEquationSys (&A, &x, &b);
      e.solve ();

      // vector x consists of a_{0}, b_{-1}, b_{0} ... b_{k-2} right here
#if COEFFDEBUG
      logprint (LOG_STATUS, "DEBUG: Moulton order %d:", order);
      for (i = 0; i < x.getRows (); i++) {
	logprint (LOG_STATUS, " %g", x.get (i));
      }
      logprint (LOG_STATUS, "\n");
#endif
      nr_double_t k = x.get (1);
      coefficients[COEFF_G] = 1 / delta[0] / k;
      coefficients[1] = -x.get (0) / delta[0] / k;
      for (i = 2; i <= order; i++) {
	coefficients[i] = -x.get (i) / k;
      }
    }
    break;
  }
}

/* The function calculates the integration coefficient for numerical
   integration methods.  Supported methods are: Adams-Bashford (order
   1-6), forward Euler and explicit Gear (order 1-6). */
void transient::calcPredictorCoeff (int Method, int order,
				    nr_double_t * coefficients,
				    nr_double_t * delta) {

  tmatrix<nr_double_t> A (order + 1);
  tvector<nr_double_t> x (order + 1);
  tvector<nr_double_t> b (order + 1);
  eqnsys<nr_double_t> e;
  e.setAlgo (ALGO_LU_DECOMPOSITION);

  switch (Method) {
  case INTEGRATOR_GEAR: // explicit GEAR order 1 to 6
    {
      int c, r;
      // right hand side vector
      b.set (0, 1);
      // first row
      for (c = 0; c < order + 1; c++) A.set (0, c, 1);
      nr_double_t f, a;
      for (f = 0, c = 0; c < order + 1; c++) {
	f += delta[c];
	for (a = 1, r = 0; r < order; r++) {
	  a *= f / delta[0];
	  A.set (r + 1, c, a);
	}
      }
      e.passEquationSys (&A, &x, &b);
      e.solve ();
      for (r = 0; r <= order; r++) coefficients[r] = x.get (r);
    }
    break;
  case INTEGRATOR_ADAMSBASHFORD: // ADAMS-BASHFORD order 1 to 6
    {
      int i, r, c;
      // right hand side vector
      for (i = 0; i < order + 1; i++) b.set (i, 1);
      for (i = 1; i < order + 1; i++) A.set (1, i, 1); // second row
      A.set (0, 0, 1);
      for (c = 1; c <= order - 1; c++) {
	nr_double_t entry = -c;
	for (r = 2; r <= order; r++) {
	  A.set (r, c + 1, r * entry);
	  entry *= -c;
	}
      }
      e.passEquationSys (&A, &x, &b);
      e.solve ();

      // vector x consists of a_{0}, b_{0}, b_{1} ... b_{k-1} right here
#if COEFFDEBUG
      logprint (LOG_STATUS, "DEBUG: Bashford order %d:", order);
      for (i = 0; i < x.getRows (); i++) {
	logprint (LOG_STATUS, " %g", x.get (i));
      }
      logprint (LOG_STATUS, "\n");
#endif
      coefficients[COEFF_G] = x.get (0);
      for (i = 1; i <= order; i++) {
	coefficients[i] = x.get (i) * delta[0];
      }
#if !FIXEDCOEFF
      if (order == 2) {
	nr_double_t f = - delta[0] / (2 * delta[1]);
	coefficients[0] = 1;
	coefficients[1] = (1 - f) * delta[0];
	coefficients[2] = f * delta[0];
      }
#endif
    }
    break;
  case INTEGRATOR_EULER: // FORWARD EULER
    coefficients[COEFF_G] = 1;
    coefficients[1] = delta[0];
    break;
  }
}

// Loads the equivalent conductance.
void transient::getConductance (integrator * c, nr_double_t cap,
				nr_double_t& geq) {
  nr_double_t * coeff = c->getCoefficients ();
  geq = cap * coeff[COEFF_G];
}

// This is the implicit Euler integrator.
void transient::integrateEuler (integrator * c, int qstate, nr_double_t cap,
				nr_double_t& geq, nr_double_t& ceq) {
  nr_double_t * coeff = c->getCoefficients ();
  int cstate = qstate + 1;
  nr_double_t cur;
  geq = cap * coeff[COEFF_G];
  ceq = c->getState (qstate, 1) * coeff[1];
  cur = c->getState (qstate) * coeff[COEFF_G] + ceq;
  c->setState (cstate, cur);
}

// Trapezoidal integrator.
void transient::integrateBilinear (integrator * c, int qstate, nr_double_t cap,
				   nr_double_t& geq, nr_double_t& ceq) {
  nr_double_t * coeff = c->getCoefficients ();
  int cstate = qstate + 1;
  nr_double_t cur;
  geq = cap * coeff[COEFF_G];
  ceq = c->getState (qstate, 1) * coeff[1] - c->getState (cstate, 1);
  cur = c->getState (qstate) * coeff[COEFF_G] + ceq;
  c->setState (cstate, cur);
}

// Integrator using the Gear coefficients.
void transient::integrateGear (integrator * c, int qstate, nr_double_t cap,
			       nr_double_t& geq, nr_double_t& ceq) {
  nr_double_t * coeff = c->getCoefficients ();
  int i, cstate = qstate + 1;
  nr_double_t cur;
  geq = cap * coeff[COEFF_G];
  for (ceq = 0, i = 1; i <= c->getOrder (); i++) {
    ceq += c->getState (qstate, i) * coeff[i];
  }
  cur = c->getState (qstate) * coeff[COEFF_G] + ceq;
  c->setState (cstate, cur);
}

// Integrator using the Adams-Moulton coefficients.
void transient::integrateMoulton (integrator * c, int qstate, nr_double_t cap,
				  nr_double_t& geq, nr_double_t& ceq) {
  nr_double_t * coeff = c->getCoefficients ();
  int i, cstate = qstate + 1;
  nr_double_t cur;
  geq = cap * coeff[COEFF_G];
  ceq = c->getState (qstate, 1) * coeff[1];
  for (i = 2; i <= c->getOrder (); i++) {
    ceq += c->getState (cstate, i - 1) * coeff[i];
  }
  cur = c->getState (qstate) * coeff[COEFF_G] + ceq;
  c->setState (cstate, cur);
}

/* The function applies the appropriate integration function to the
   given circuit object. */
void transient::setIntegrationMethod (circuit * c, int Method) {
  switch (Method) {
  case INTEGRATOR_GEAR:
    c->setIntegration (integrateGear);
    break;
  case INTEGRATOR_TRAPEZOIDAL:
    c->setIntegration (integrateBilinear);
    break;
  case INTEGRATOR_EULER:
    c->setIntegration (integrateEuler);
    break;
  case INTEGRATOR_ADAMSMOULTON:
    c->setIntegration (integrateMoulton);
    break;
  default:
    c->setIntegration (NULL);
    break;
  }
  c->setConductance (getConductance);
}

/* Returns an appropriate integrator type identifier and the maximum
   order depending on the given string argument. */
int transient::correctorType (const char * const Method, int& MaxOrder) {
  if (!strcmp (Method, "Gear")) {
    if (MaxOrder > 6) MaxOrder = 6;
    if (MaxOrder < 1) MaxOrder = 1;
    return INTEGRATOR_GEAR;
  }
  else if (!strcmp (Method, "Trapezoidal")) {
    MaxOrder = 2;
    return INTEGRATOR_TRAPEZOIDAL;
  }
  else if (!strcmp (Method, "Euler")) {
    MaxOrder = 1;
    return INTEGRATOR_EULER;
  }
  else if (!strcmp (Method, "AdamsMoulton")) {
    if (MaxOrder > 6) MaxOrder = 6;
    if (MaxOrder < 1) MaxOrder = 1;
    return INTEGRATOR_ADAMSMOULTON;
  }
  else if (!strcmp (Method, "AdamsBashford")) {
    if (MaxOrder > 6) MaxOrder = 6;
    if (MaxOrder < 1) MaxOrder = 1;
    return INTEGRATOR_ADAMSBASHFORD;
  }
  return INTEGRATOR_UNKNOWN;
}

/* The function returns the appropriate predictor integration method
   for the given corrector method and adjusts the order of the
   predictor as well based on the given corrector method. */
int transient::predictorType (int corrMethod, int corrOrder, int& predOrder) {
  int predMethod = INTEGRATOR_UNKNOWN;
  switch (corrMethod) {
  case INTEGRATOR_GEAR:
    predMethod = INTEGRATOR_GEAR;
    break;
  case INTEGRATOR_ADAMSMOULTON:
    predMethod = INTEGRATOR_ADAMSBASHFORD;
    break;
  case INTEGRATOR_TRAPEZOIDAL:
    predMethod = INTEGRATOR_ADAMSBASHFORD;
    break;
  case INTEGRATOR_EULER:
    predMethod = INTEGRATOR_EULER;
    break;
  }
  predOrder = corrOrder;
  return predMethod;
}

// Structure defining integration algorithm for each possible order.
struct integration_types_t {
  int Method;
  int integratorType[6];
  nr_double_t corrErrorConstant[6];
  nr_double_t predErrorConstant[6];
};

static struct integration_types_t integration_types[] = {
  { INTEGRATOR_EULER,
    { INTEGRATOR_EULER },
    { -1.0/2 },
    { +1.0/2 }
  },
  { INTEGRATOR_TRAPEZOIDAL,
    { INTEGRATOR_EULER, INTEGRATOR_TRAPEZOIDAL },
    { -1.0/2, -1.0/12 },
    { +1.0/2, +5.0/12 }
  },
  { INTEGRATOR_GEAR,
    { INTEGRATOR_GEAR, INTEGRATOR_GEAR, INTEGRATOR_GEAR,
      INTEGRATOR_GEAR, INTEGRATOR_GEAR, INTEGRATOR_GEAR },
    { -1.0/2, -2.0/9, -3.0/22, -12.0/125, -10.0/137, -20.0/343 },
    { +1.0, +1.0, +1.0, +1.0, +1.0, +1.0 }
  },
  { INTEGRATOR_ADAMSMOULTON,
    { INTEGRATOR_ADAMSMOULTON, INTEGRATOR_ADAMSMOULTON,
      INTEGRATOR_ADAMSMOULTON, INTEGRATOR_ADAMSMOULTON,
      INTEGRATOR_ADAMSMOULTON, INTEGRATOR_ADAMSMOULTON },
    { -1.0/2, -1.0/12, -1.0/24, -19.0/720, -3.0/160, -863.0/60480 },
    { +1.0/2, +1.0/12, +1.0/24, +19.0/720, +3.0/160, +863.0/60480 }
  },
  { INTEGRATOR_ADAMSBASHFORD,
    { INTEGRATOR_ADAMSBASHFORD, INTEGRATOR_ADAMSBASHFORD,
      INTEGRATOR_ADAMSBASHFORD, INTEGRATOR_ADAMSBASHFORD,
      INTEGRATOR_ADAMSBASHFORD, INTEGRATOR_ADAMSBASHFORD },
    { -1.0/2, -5.0/12, -3.0/8, -251.0/720, -95.0/288, -19087.0/60480 },
    { +1.0/2, +5.0/12, +3.0/8, +251.0/720, +95.0/288, +19087.0/60480 }
  }
};

/* The function returns the appropriate integration type for the given
   corrector integration type and order. */
int transient::correctorType (int Method, int order) {
  return integration_types[Method].integratorType[order - 1];
}

// Returns the error constant for the given corrector.
nr_double_t transient::getCorrectorError (int Method, int order) {
  return integration_types[Method].corrErrorConstant[order - 1];
}

// Returns the error constant for the given predictor.
nr_double_t transient::getPredictorError (int Method, int order) {
  return integration_types[Method].predErrorConstant[order - 1];
}

}
