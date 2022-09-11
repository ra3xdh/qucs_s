/*
 * differentiate.cpp - the Qucs equation differentiator implementations
 *
 * Copyright (C) 2007, 2008 Stefan Jahn <stefan@lkcc.org>
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
#include <ctype.h>
#include <cmath>

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "consts.h"
#include "equation.h"
#include "differentiate.h"

using namespace qucs;
using namespace qucs::eqn;

// Short helper macros.
#define C(con) ((constant *) (con))
#define A(con) ((application *) (con))
#define R(con) ((reference *) (con))
#define D(con) (C(con)->d)

#define isConst(n) ((n)->getTag()==CONSTANT && C(n)->getType()==TAG_DOUBLE)
#define isRef(r,v) ((r)->getTag()==REFERENCE && !strcmp(R(r)->n,v))
#define isZero(n)  (isConst(n) && D(n) == 0.0)
#define isOne(n)   (isConst(n) && D(n) == 1.0)
#define isNeg(n)   (isConst(n) && D(n) == -1.0)
#define isEuler(n) ((isConst(n) && D(n) == euler) || isRef(n,"e"))
#define isval(n,v) (isConst(n) && D(n) == v)

#define isVar(v)   ((v)->getTag()==REFERENCE)
#define isApp(v)   ((v)->getTag()==APPLICATION)
#define isMul(v)   (isApp(v) && !strcmp(A(v)->n,"*"))
#define isSqr(v)   (isApp(v) && !strcmp(A(v)->n,"sqr"))

#define retCon(val) \
  constant * res = new constant (TAG_DOUBLE); res->d = val; return res;
#define defCon(def,val) \
  constant * def = new constant (TAG_DOUBLE); def->d = val;
#define defRef(def,var) \
  reference * def = new reference (); def->n = strdup (var);
#define retApp1(op,f0) \
  application * res = new application (); res->n = strdup (op); \
  res->nargs = 1; res->args = f0; res->args->setNext (NULL); return res;
#define defApp1(def,op,f0) \
  application * def = new application (); def->n = strdup (op); \
  def->nargs = 1; def->args = f0; def->args->setNext (NULL);
#define defApp2(def,op,f0,f1) \
  application * def = new application (); def->n = strdup (op); \
  def->nargs = 2; def->args = f0; def->args->append (f1);
#define retApp2(op,f0,f1) \
  application * res = new application (); res->n = strdup (op); \
  res->nargs = 2; res->args = f0; res->args->append (f1); return res;
#define retApp3(op,f0,f1,f2) \
  application * res = new application (); res->n = strdup (op); \
  res->nargs = 3; res->args = f0; res->args->append (f1); \
  res->args->append (f2); return res;
#define defApp3(def,op,f0,f1,f2) \
  application * def = new application (); def->n = strdup (op); \
  def->nargs = 3; def->args = f0; def->args->append (f1); \
  def->args->append (f2);

#define _A(idx) app->args->get(idx)
#define _A0 _A(0)
#define _A1 _A(1)
#define _A2 _A(2)
#define _D0 _A(0)->differentiate (derivative)
#define _D1 _A(1)->differentiate (derivative)
#define _D2 _A(2)->differentiate (derivative)

#define _AF0(var) node * var = _A0;
#define _AF1(var) node * var = _A1;
#define _AF2(var) node * var = _A2;
#define _AD0(var) node * var = _D0;
#define _AD1(var) node * var = _D1;
#define _AD2(var) node * var = _D2;

#define _AA(a,idx) A(a)->args->get(idx)
#define _AA0(a) _AA(a,0)
#define _AA1(a) _AA(a,1)

#define _AAF0(a,var) node * var = _AA0(a);
#define _AAF1(a,var) node * var = _AA1(a);

node * differentiate::plus_binary (application * app, char * derivative) {
  _AD0 (d0);
  _AD1 (d1);
  if (!isConst (d0) && !isConst (d1)) {
    retApp2 ("+", d0, d1);
  }
  return plus_reduce (d0, d1);
}

node * differentiate::plus_unary (application * app, char * derivative) {
  _AD0 (d0);
  return d0;
}

node * differentiate::plus_reduce (node * f0, node * f1) {
  if (isZero (f0) && isZero (f1)) {
    delete f0; delete f1;
    retCon (0);
  } else if (isZero (f0)) {
    delete f0;
    return f1;
  } else if (isZero (f1)) {
    delete f1;
    return f0;
  } else if (isConst (f0) && isConst (f1)) {
    nr_double_t t = D(f0) + D(f1);
    delete f0; delete f1;
    retCon (t);
  } else {
    retApp2 ("+", f0, f1);
  }
}

node * differentiate::minus_binary (application * app, char * derivative) {
  _AD0 (d0);
  _AD1 (d1);
  if (!isConst (d0) && !isConst (d1)) {
    retApp2 ("-", d0, d1);
  }
  return minus_reduce (d0, d1);
}

node * differentiate::minus_unary (application * app, char * derivative) {
  _AD0 (d0);
  return minus_reduce (d0);
}

node * differentiate::minus_reduce (node * f0) {
  if (isZero (f0)) {
    delete f0;
    retCon (0);
  } else if (isConst (f0)) {
    nr_double_t t = -D(f0);
    delete f0;
    retCon (t);
  }
  retApp1 ("-", f0);
}

node * differentiate::minus_reduce (node * f0, node * f1) {
  if (isZero (f0) && isZero (f1)) {
    delete f0; delete f1;
    retCon (0);
  } else if (isZero (f0)) {
    delete f0;
    return minus_reduce (f1);
  } else if (isZero (f1)) {
    delete f1;
    return f0;
  } else if (isConst (f0) && isConst (f1)) {
    nr_double_t t = D(f0) - D(f1);
    delete f0; delete f1;
    retCon (t);
  } else {
    retApp2 ("-", f0, f1);
  }
}

node * differentiate::times (application * app, char * derivative) {
  _AF0 (f0);
  _AF1 (f1);
  if (isConst (f0) && isConst (f1)) {
    retCon (0);
  }
  _AD0 (d0);
  _AD1 (d1);
  node * t1 = times_reduce (f0->recreate(), d1);
  node * t2 = times_reduce (f1->recreate(), d0);
  return plus_reduce (t1, t2);
}

node * differentiate::times_reduce (node * f0, node * f1) {
  if (isZero (f0) || isZero (f1)) {
    delete f0; delete f1;
    retCon (0);
  } else if (isOne (f0)) {
    delete f0;
    return f1;
  } else if (isNeg (f0)) {
    delete f0;
    return minus_reduce (f1);
  } else if (isOne (f1)) {
    delete f1;
    return f0;
  } else if (isNeg (f1)) {
    delete f1;
    return minus_reduce (f0);
  } else if (isConst (f0) && isConst (f1)) {
    nr_double_t t = D(f0) * D(f1);
    delete f0; delete f1;
    retCon (t);
  } else {
    retApp2 ("*", f0, f1);
  }
}

node * differentiate::over (application * app, char * derivative) {
  _AF0 (f0);
  _AF1 (f1);
  if (isConst (f0) && isConst (f1)) {
    retCon (0);
  }
  _AD0 (d0);
  _AD1 (d1);
  node * t1 = times_reduce (f0->recreate(), d1);
  node * t2 = times_reduce (f1->recreate(), d0);
  node * t3 = minus_reduce (t2, t1);
  node * t4 = sqr_reduce (f1->recreate());
  return over_reduce (t3, t4);
}

node * differentiate::over_reduce (node * f0, node * f1) {
  if (isOne (f0) && isOne (f1)) {
    delete f0; delete f1;
    retCon (1);
  } else if (isZero (f0)) {
    delete f0; delete f1;
    retCon (0);
  } else if (isConst (f0) && isConst (f1)) {
    if (isZero (f1)) {
      retApp2 ("/", f0, f1);
    }
    nr_double_t t = D(f0) / D(f1);
    delete f0; delete f1;
    retCon (t);
  } else if (isOne (f1)) {
    delete f1;
    return f0;
  } else if (isNeg (f1)) {
    delete f1;
    return minus_reduce (f0);
  } else {
    over_reduce_adv (f0, f1);
    retApp2 ("/", f0, f1);
  }
}

void differentiate::over_reduce_adv (node * &f0, node * &f1) {
  if (isVar (f0)) {
    if (isSqr (f1)) {
      _AAF0 (f1,g1);
      if (isVar (g1)) {
	if (!strcmp (R(f0)->n, R(g1)->n)) {
	  defCon (one, 1);
	  reference * var = new reference (*R(g1));
	  delete f0;
	  delete f1;
	  f0 = one;
	  f1 = var;
	}
      }
    }
  }
}

node * differentiate::power (application * app, char * derivative) {
  _AF0 (f0);
  _AF1 (f1);
  if (isConst (f0) && isConst (f1)) {
    retCon (0);
  }
  _AD0 (d0);
  _AD1 (d1);
  if (isZero (d1)) {
    defCon (one, 1);
    node * t1 = minus_reduce (f1->recreate(), one);
    node * t2 = power_reduce (f0->recreate(), t1);
    node * t3 = times_reduce (f1->recreate(), t2);
    return times_reduce (t3, d0);
  }
  else {
    node * t1 = power_reduce (f0->recreate(), f1->recreate());
    node * ln = ln_reduce (f0->recreate());
    node * t2 = times_reduce (d1, ln);
    node * t3 = times_reduce (f1->recreate(), d0);
    node * t4 = over_reduce (t3, f0->recreate());
    node * t5 = plus_reduce (t2, t4);
    return times_reduce (t1, t5);
  }
}

node * differentiate::power_reduce (node * f0, node * f1) {
  if (isOne (f0)) {
    delete f0; delete f1;
    retCon (1);
  } else if (isZero (f0)) {
    delete f0; delete f1;
    retCon (0);
  } else if (isConst (f0) && isConst (f1)) {
    if (isZero (f1)) {
      delete f0; delete f1;
      retCon (1);
    }
    nr_double_t t = std::pow (D(f0), D(f1));
    delete f0; delete f1;
    retCon (t);
  } else if (isOne (f1)) {
    delete f1;
    return f0;
  } else {
    retApp2 ("^", f0, f1);
  }
}

node * differentiate::ln (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  return over_reduce (d0, f0->recreate ());
}

node * differentiate::ln_reduce (node * f0) {
  if (isOne (f0)) {
    delete f0;
    retCon (0);
  } else if (isEuler (f0)) {
    delete f0;
    retCon (1);
  }
  retApp1 ("ln", f0);
}

node * differentiate::log10 (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  node * t1 = over_reduce (d0, f0->recreate ());
  defCon (ten, 10);
  return over_reduce (t1, ln_reduce (ten));
}

node * differentiate::log2 (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  node * t1 = over_reduce (d0, f0->recreate ());
  defCon (two, 2);
  return over_reduce (t1, ln_reduce (two));
}

node * differentiate::sqrt (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  defCon (half, 0.5);
  node * t1 = times_reduce (half, d0);
  node * t2 = sqrt_reduce (f0->recreate());
  return over_reduce (t1, t2);
}

node * differentiate::sqrt_reduce (node * f0) {
  if (isOne (f0)) {
    delete f0;
    retCon (1);
  } else if (isZero (f0)) {
    delete f0;
    retCon (0);
  }
  retApp1 ("sqrt", f0);
}

node * differentiate::app_reduce (const char * func, node * d0, node * f0) {
  if (isOne (d0)) {
    delete d0;
    retApp1 (func, f0);
  } else if (isZero (d0)) {
    delete d0; delete f0;
    retCon (0);
  }
  defApp1 (app, func, f0);
  return times_reduce (d0, app);
}

node * differentiate::exp (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  return app_reduce ("exp", d0, f0->recreate());
}

node * differentiate::limexp (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  defCon (lexp, ::exp (limitexp));
  defCon (lcon, limitexp);
  defApp2 (ask, "<", f0->recreate(), lcon);
  defApp1 (exp, "exp", f0->recreate());
  defApp3 (ite, "?:", ask, exp, lexp);
  return times_reduce (d0, ite);
}

node * differentiate::sin (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  return app_reduce ("cos", d0, f0->recreate());
}

node * differentiate::cos (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  node * t1 = minus_reduce (d0);
  return app_reduce ("sin", t1, f0->recreate());
}

node * differentiate::tan (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  defApp1 (sec, "sec", f0->recreate());
  defCon (two, 2);
  node * t1 = power_reduce (sec, two);
  return times_reduce (d0, t1);
}

node * differentiate::sec (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  defApp1 (sin, "sin", f0->recreate());
  defApp1 (cos, "cos", f0->recreate());
  defCon (two, 2);
  node * t1 = power_reduce (cos, two);
  node * t2 = over_reduce (sin, t1);
  return times_reduce (d0, t2);
}

node * differentiate::cot (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  defApp1 (cosec, "cosec", f0->recreate());
  defCon (two, 2);
  node * t1 = minus_reduce (d0);
  node * t2 = power_reduce (cosec, two);
  return times_reduce (t1, t2);
}

node * differentiate::cosec (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  defApp1 (sin, "sin", f0->recreate());
  defApp1 (cos, "cos", f0->recreate());
  defCon (two, 2);
  node * t1 = minus_reduce (d0);
  node * t2 = power_reduce (sin, two);
  node * t3 = over_reduce (cos, t2);
  return times_reduce (t1, t3);
}

node * differentiate::abs (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  return app_reduce ("sign", d0, f0->recreate());
}

node * differentiate::step (application *, char *) {
  retCon (0);
}

node * differentiate::sign (application *, char *) {
  retCon (0);
}

node * differentiate::arcsin (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  node * sqr = sqr_reduce (f0->recreate());
  defCon (one, 1);
  node * t1 = minus_reduce (one, sqr);
  node * t2 = sqrt_reduce (t1);
  return over_reduce (d0, t2);
}

node * differentiate::square (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  defCon (two, 2);
  node * t1 = times_reduce (two, d0);
  return times_reduce (t1, f0->recreate());
}

node * differentiate::sqr_reduce (node * f0) {
  if (isOne (f0)) {
    delete f0;
    retCon (1);
  } else if (isZero (f0)) {
    delete f0;
    retCon (0);
  } else if (isConst (f0)) {
    nr_double_t t = D(f0) * D(f0);
    delete f0;
    retCon (t);
  } else {
    retApp1 ("sqr", f0);
  }
}

node * differentiate::arccos (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  node * sqr = sqr_reduce (f0->recreate());
  defCon (one, 1);
  node * t1 = minus_reduce (one, sqr);
  node * t2 = sqrt_reduce (t1);
  node * t3 = minus_reduce (d0);
  return over_reduce (t3, t2);
}

node * differentiate::arctan (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  node * sqr = sqr_reduce (f0->recreate());
  defCon (one, 1);
  node * t1 = plus_reduce (one, sqr);
  return over_reduce (d0, t1);
}

node * differentiate::arccot (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  node * sqr = sqr_reduce (f0->recreate());
  defCon (one, 1);
  node * t1 = plus_reduce (one, sqr);
  node * t2 = minus_reduce (d0);
  return over_reduce (t2, t1);
}

node * differentiate::arcsec (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  node * sqr = sqr_reduce (f0->recreate());
  defCon (one, 1);
  node * t1 = minus_reduce (sqr, one);
  node * t2 = sqrt_reduce (t1);
  node * t3 = times_reduce (f0->recreate(), t2);
  return over_reduce (d0, t3);
}

node * differentiate::arccosec (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  node * sqr = sqr_reduce (f0->recreate());
  defCon (one, 1);
  node * t1 = minus_reduce (sqr, one);
  node * t2 = sqrt_reduce (t1);
  node * t3 = times_reduce (f0->recreate(), t2);
  node * t4 = minus_reduce (d0);
  return over_reduce (t4, t3);
}

node * differentiate::sinh (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  return app_reduce ("cosh", d0, f0->recreate());
}

node * differentiate::cosh (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  return app_reduce ("sinh", d0, f0->recreate());
}

node * differentiate::tanh (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  defApp1 (cosh, "cosh", f0->recreate());
  defCon (two, 2);
  node * t1 = power_reduce (cosh, two);
  return over_reduce (d0, t1);
}

node * differentiate::coth (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  defApp1 (sinh, "sinh", f0->recreate());
  defCon (two, 2);
  node * t1 = power_reduce (sinh, two);
  node * t2 = minus_reduce (d0);
  return over_reduce (t2, t1);
}

node * differentiate::artanh (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  node * sqr = sqr_reduce (f0->recreate());
  defCon (one, 1);
  node * t1 = minus_reduce (one, sqr);
  return over_reduce (d0, t1);
}

node * differentiate::arcoth (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  node * sqr = sqr_reduce (f0->recreate());
  defCon (one, 1);
  node * t1 = minus_reduce (sqr, one);
  node * t2 = minus_reduce (d0);
  return over_reduce (t2, t1);
}

node * differentiate::arcosh (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  node * sqr = sqr_reduce (f0->recreate());
  defCon (one, 1);
  node * t1 = minus_reduce (sqr, one);
  node * t2 = sqrt_reduce (t1);
  return over_reduce (d0, t2);
}

node * differentiate::arsinh (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  node * sqr = sqr_reduce (f0->recreate());
  defCon (one, 1);
  node * t1 = plus_reduce (sqr, one);
  node * t2 = sqrt_reduce (t1);
  return over_reduce (d0, t2);
}

node * differentiate::arsech (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  node * sqr = sqr_reduce (f0->recreate());
  defCon (one, 1);
  node * t1 = minus_reduce (one, sqr);
  node * t2 = sqrt_reduce (t1);
  node * t3 = times_reduce (f0->recreate(), t2);
  node * t4 = minus_reduce (d0);
  return over_reduce (t4, t3);
}

node * differentiate::arcosech (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  node * sqr = sqr_reduce (f0->recreate());
  defCon (one, 1);
  node * t1 = plus_reduce (one, sqr);
  node * t2 = sqrt_reduce (t1);
  node * t3 = times_reduce (f0->recreate(), t2);
  node * t4 = minus_reduce (d0);
  return over_reduce (t4, t3);
}

node * differentiate::ifthenelse (application * app, char * derivative) {
  _AF0 (f0);
  _AD1 (d1);
  _AD2 (d2);
  if (isConst (d1) && isConst (d2)) {
    if (D(d1) == D(d2)) {
      nr_double_t t = D(d1);
      delete d1; delete d2;
      retCon (t);
    }
  }
  retApp3 ("?:", f0->recreate(), d1, d2);
}

node * differentiate::sinc (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  defApp1 (sinc, "sinc", f0->recreate());
  defApp1 (cos, "cos", f0->recreate());
  node * t1 = minus_reduce (cos, sinc);
  node * t2 = over_reduce (t1, f0->recreate());
  return times_reduce (d0, t2);
}

node * differentiate::norm (application * app, char * derivative) {
  _AF0 (f0);
  _AD0 (d0);
  defCon (two, 2);
  node * t1 = times_reduce (d0, two);
  return times_reduce (t1, f0->recreate());
}

node * differentiate::xhypot (application * app, char * derivative) {
  _AF0 (f0);
  _AF1 (f1);
  _AD0 (d0);
  _AD1 (d1);
  node * t1 = hypot_reduce (f0->recreate(), f1->recreate());
  node * t2 = times_reduce (d0, f0->recreate());
  node * t3 = times_reduce (d1, f1->recreate());
  node * t4 = plus_reduce (t2, t3);
  return over_reduce (t4, t1);
}

node * differentiate::hypot_reduce (node * f0, node * f1) {
  if (isZero (f0) && isZero (f1)) {
    delete f0; delete f1;
    retCon (0);
  } else if (isZero (f0)) {
    delete f0;
    return sqrt_reduce (sqr_reduce (f1));
  } else if (isZero (f1)) {
    delete f1;
    return sqrt_reduce (sqr_reduce (f0));
  } else if (isConst (f0) && isConst (f1)) {
    nr_double_t t = ::xhypot (D(f0), D(f1));
    delete f0; delete f1;
    retCon (t);
  } else {
    retApp2 ("hypot", f0, f1);
  }
}

#include "constants.h"

node * differentiate::vt (application * app, char * derivative) {
  _AD0 (d0);
  defCon (con, kBoverQ);
  return times_reduce (d0, con);
}

// List of differentiators.
struct differentiation_t eqn::differentiations[] = {
  { "+", differentiate::plus_binary,  2 },
  { "+", differentiate::plus_unary,   1 },
  { "-", differentiate::minus_binary, 2 },
  { "-", differentiate::minus_unary,  1 },
  { "*", differentiate::times,        2 },
  { "/", differentiate::over,         2 },
  { "^", differentiate::power,        2 },

  { "?:", differentiate::ifthenelse, 3 },

  { "ln",       differentiate::ln,        1 },
  { "log10",    differentiate::log10,     1 },
  { "log2",     differentiate::log2,      1 },
  { "sqrt",     differentiate::sqrt,      1 },
  { "exp",      differentiate::exp,       1 },
  { "sinc",     differentiate::sinc,      1 },
  { "norm",     differentiate::norm,      1 },
  { "sin",      differentiate::sin,       1 },
  { "cos",      differentiate::cos,       1 },
  { "tan",      differentiate::tan,       1 },
  { "sec",      differentiate::sec,       1 },
  { "cot",      differentiate::cot,       1 },
  { "cosec",    differentiate::cosec,     1 },
  { "abs",      differentiate::abs,       1 },
  { "step",     differentiate::step,      1 },
  { "sign",     differentiate::sign,      1 },
  { "arcsin",   differentiate::arcsin,    1 },
  { "arccos",   differentiate::arccos,    1 },
  { "arctan",   differentiate::arctan,    1 },
  { "arccot",   differentiate::arccot,    1 },
  { "arcsec",   differentiate::arcsec,    1 },
  { "arccosec", differentiate::arccosec,  1 },
  { "sqr",      differentiate::square,    1 },
  { "sinh",     differentiate::sinh,      1 },
  { "cosh",     differentiate::cosh,      1 },
  { "tanh",     differentiate::tanh,      1 },
  { "coth",     differentiate::coth,      1 },
  { "arsinh",   differentiate::arsinh,    1 },
  { "arcosh",   differentiate::arcosh,    1 },
  { "artanh",   differentiate::artanh,    1 },
  { "arcoth",   differentiate::arcoth,    1 },
  { "arsech",   differentiate::arsech,    1 },
  { "arcosech", differentiate::arcosech,  1 },
  { "hypot",    differentiate::xhypot,    2 },
  { "limexp",   differentiate::limexp,    1 },
  { "vt",       differentiate::vt,        1 },

  { NULL, NULL, 0 /* end of list */ }
};
