/*
 * evaluate.cpp - the Qucs equation evaluator implementations
 *
 * Copyright (C) 2004-2011 Stefan Jahn <stefan@lkcc.org>
 * Copyright (C) 2006 Gunther Kraut <gn.kraut@t-online.de>
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
#include "vector.h"
#include "matrix.h"
#include "poly.h"
#include "spline.h"
#include "fourier.h"
#include "receiver.h"
#include "constants.h"
#include "fspecial.h"
#include "circuit.h"
#include "range.h"
#include "equation.h"
#include "evaluate.h"
#include "exception.h"
#include "exceptionstack.h"
#include "strlist.h"

using namespace qucs;
using namespace qucs::eqn;
using namespace qucs::fourier;
using namespace fspecial;

// Short macros in order to obtain the correct constant value.
#define D(con)   ((constant *) (con))->d
#define C(con)   ((constant *) (con))->c
#define V(con)   ((constant *) (con))->v
#define M(con)   ((constant *) (con))->m
#define MV(con)  ((constant *) (con))->mv
#define STR(con) ((constant *) (con))->s
#define CHR(con) ((constant *) (con))->chr
#define INT(con) ((int) D (con))
#define RNG(con) ((constant *) (con))->r
#define B(con)   ((constant *) (con))->b

#define A(a) ((assignment *) (a))
#define R(r) ((reference *) (r))

// Argument macros.
#define _ARES(idx) args->getResult(idx)
#define _ARG(idx) args->get(idx)

#define _D(var,idx) nr_double_t (var) = D (_ARES (idx));
#define _BO(var,idx) bool (var) = B (_ARES (idx));
#define _CX(var,idx) nr_complex_t * (var) = C (_ARES (idx));
#define _V(var,idx) qucs::vector * (var) = V (_ARES (idx));
#define _M(var,idx) matrix * (var) = M (_ARES (idx));
#define _MV(var,idx) matvec * (var) = MV (_ARES (idx));
#define _I(var,idx) int (var) = INT (_ARES (idx));
#define _R(var,idx) range * (var) = RNG (_ARES (idx));

#define _ARR0(var) _R (var,0)
#define _ARR1(var) _R (var,1)
#define _ARR2(var) _R (var,2)
#define _ARI0(var) _I (var,0)
#define _ARI1(var) _I (var,1)
#define _ARI2(var) _I (var,2)
#define _ARD0(var) _D (var,0)
#define _ARD1(var) _D (var,1)
#define _ARD2(var) _D (var,2)
#define _ARB0(var) _BO (var,0)
#define _ARB1(var) _BO (var,1)
#define _ARB2(var) _BO (var,2)
#define _ARC0(var) _CX (var,0)
#define _ARC1(var) _CX (var,1)
#define _ARC2(var) _CX (var,2)
#define _ARM0(var) _M (var,0)
#define _ARM1(var) _M (var,1)
#define _ARM2(var) _M (var,2)
#define _ARV0(var) _V (var,0)
#define _ARV1(var) _V (var,1)
#define _ARV2(var) _V (var,2)
#define _ARMV0(var) _MV (var,0)
#define _ARMV1(var) _MV (var,1)
#define _ARMV2(var) _MV (var,2)

// Return value definition macros.
#define _DEFD() constant * res = new constant (TAG_DOUBLE);
#define _DEFB() constant * res = new constant (TAG_BOOLEAN);
#define _DEFC() constant * res = new constant (TAG_COMPLEX);
#define _DEFV() constant * res = new constant (TAG_VECTOR);
#define _DEFM() constant * res = new constant (TAG_MATRIX);
#define _DEFMV() constant * res = new constant (TAG_MATVEC);
#define _DEFR() constant * res = new constant (TAG_RANGE);

// Return value macros.
#define _RETD(var) res->d = (var); return res;
#define _RETD_QUCS(var) res->d = (qucs::var); return res;
#define _RETD_STD(var) res->d = (std::var); return res;
#define _RETD_SPECIAL(var) res->d = (fspecial::var); return res;
#define _RETB(var) res->b = (var); return res;
#define _RETC(var) res->c = new nr_complex_t (var); return res;
#define _RETV(var) res->v = new qucs::vector (var); return res;
#define _RETM(var) res->m = new matrix (var); return res;
#define _RETMV(var) res->mv = new matvec (var); return res;
#define _RETR(var) res->r = (var); return res;

// Return value macros without arguments.
#define __RETC() res->c = new nr_complex_t (); return res;
#define __RETV() res->v = new qucs::vector (); return res;
#define __RETM() res->m = new matrix (); return res;
#define __RETMV() res->mv = new matvec (); return res;

#define SOLVEE(idx) args->get(idx)->solvee

// Throws a math exception.
#define THROW_MATH_EXCEPTION(txt) do { \
  qucs::exception * e = new qucs::exception (EXCEPTION_MATH); \
  e->setText (txt); throw_exception (e); } while (0)

/* The QUCS_CONCAT macros create a new concatenated symbol for the
   compiler in a portable way.  It is essential to use these macros
   like QUCS_CONCAT (a,b) and *not* like QUCS_CONCAT (a, b) or its
   variants. */
#if defined (__STDC__) || defined (__cplusplus)
# define QUCS_CONCAT2(a, b) a##b
# define QUCS_CONCAT3(a, b, c) a##b##c
#else
# define QUCS_CONCAT2(a, b) a/* */b
# define QUCS_CONCAT3(a, b, c) a/* */b/* */c
#endif

// The following macro is meant to be used for some simple functions.
#define MAKE_FUNC_DEFINITION_0(cfunc) \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_d) (constant * args) { \
  _ARD0 (d);							  \
  _DEFD ();							  \
  _RETD (cfunc (d));						  \
}								  \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_c) (constant * args) { \
  _ARC0 (c);							  \
  _DEFC ();							  \
  _RETC (cfunc (*c));						  \
}								  \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_v) (constant * args) { \
  _ARV0 (v);							  \
  _DEFV ();							  \
  _RETV (cfunc (*v));						  \
}

// The following macro is meant to be used for some simple functions,
// which must explicitly use the double function from the std namespace
#define MAKE_FUNC_DEFINITION_0_STD(cfunc) \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_d) (constant * args) { \
  _ARD0 (d);							  \
  _DEFD ();							  \
  _RETD_STD (cfunc (d));						  \
}								  \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_c) (constant * args) { \
  _ARC0 (c);							  \
  _DEFC ();							  \
  _RETC (cfunc (*c));						  \
}								  \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_v) (constant * args) { \
  _ARV0 (v);							  \
  _DEFV ();							  \
  _RETV (cfunc (*v));						  \
}

// The following macro is meant to be used for some simple functions,
// which must explicitly use the double function from the qucs namespace
#define MAKE_FUNC_DEFINITION_0_QUCS(cfunc) \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_d) (constant * args) { \
  _ARD0 (d);							  \
  _DEFD ();							  \
  _RETD_QUCS (cfunc (d));						  \
}								  \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_c) (constant * args) { \
  _ARC0 (c);							  \
  _DEFC ();							  \
  _RETC (cfunc (*c));						  \
}								  \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_v) (constant * args) { \
  _ARV0 (v);							  \
  _DEFV ();							  \
  _RETV (cfunc (*v));						  \
}

// The following macro is meant to be used for some simple functions,
// which must explicitly use the double function from the fspecial namespace
#define MAKE_FUNC_DEFINITION_0_SPECIAL(cfunc) \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_d) (constant * args) { \
  _ARD0 (d);							  \
  _DEFD ();							  \
  _RETD_SPECIAL (cfunc (d));						  \
}								  \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_c) (constant * args) { \
  _ARC0 (c);							  \
  _DEFC ();							  \
  _RETC (cfunc (*c));						  \
}								  \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_v) (constant * args) { \
  _ARV0 (v);							  \
  _DEFV ();							  \
  _RETV (cfunc (*v));						  \
}

#define MAKE_FUNC_DEFINITION_1(cfunc) \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_d) (constant * args) { \
  _ARD0 (d);							  \
  _DEFD ();							  \
  _RETD (cfunc (d));						  \
}								  \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_c) (constant * args) { \
  _ARC0 (c);							  \
  _DEFD ();							  \
  res->d = cfunc (*c); return res;				  \
}								  \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_v) (constant * args) { \
  _ARV0 (v);							  \
  _DEFV ();							  \
  _RETV (cfunc (*v));						  \
}								  \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_m) (constant * args) { \
  _ARM0 (m);							  \
  _DEFM ();							  \
  _RETM (cfunc (*m));						  \
}								  \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_mv) (constant * args) {\
  _ARMV0 (mv);							  \
  _DEFMV ();							  \
  _RETMV (cfunc (*mv));						  \
}

MAKE_FUNC_DEFINITION_0 (exp);    // exponential function
MAKE_FUNC_DEFINITION_0 (limexp); // limited exponential function
MAKE_FUNC_DEFINITION_0 (sin);    // sine
MAKE_FUNC_DEFINITION_0 (cos);    // cosine
MAKE_FUNC_DEFINITION_0 (tan);    // tangent
MAKE_FUNC_DEFINITION_0 (sinh);   // sine hyperbolicus
MAKE_FUNC_DEFINITION_0 (cosh);   // cosine hyperbolicus
MAKE_FUNC_DEFINITION_0 (tanh);   // tangent hyperbolicus
MAKE_FUNC_DEFINITION_0_QUCS (coth);   // cotangent hyperbolicus
MAKE_FUNC_DEFINITION_0_QUCS (sech);   // secans hyperbolicus
MAKE_FUNC_DEFINITION_0_QUCS (cosech); // cosecans hyperbolicus
MAKE_FUNC_DEFINITION_0_QUCS (signum); // signum function
MAKE_FUNC_DEFINITION_0_QUCS (sign);   // sign function
MAKE_FUNC_DEFINITION_0_QUCS (sinc);   // sin(x)/x aka sinc function
MAKE_FUNC_DEFINITION_0 (sqr);    // square value

MAKE_FUNC_DEFINITION_1 (real);   // real value
MAKE_FUNC_DEFINITION_1 (imag);   // imaginary value
MAKE_FUNC_DEFINITION_1 (abs);    // absolute value

// ******************** unary plus *************************
constant * evaluate::plus_d (constant * args) {
  _ARD0 (d1); _DEFD (); _RETD (d1);
}

constant * evaluate::plus_c (constant * args) {
  _ARC0 (c1); _DEFC (); _RETC (*c1);
}

constant * evaluate::plus_v (constant * args) {
  _ARV0 (v1); _DEFV (); _RETV (*v1);
}

constant * evaluate::plus_m (constant * args) {
  _ARM0 (m1); _DEFM (); _RETM (*m1);
}

constant * evaluate::plus_mv (constant * args) {
  _ARMV0 (v1); _DEFMV (); _RETMV (*v1);
}

// ****************** addition *****************************
constant * evaluate::plus_d_d (constant * args) {
  _ARD0 (d1);
  _ARD1 (d2);
  _DEFD ();
  _RETD (d1 + d2);
}

constant * evaluate::plus_c_c (constant * args) {
  _ARC0 (c1);
  _ARC1 (c2);
  _DEFC ();
  _RETC (*c1 + *c2);
}

constant * evaluate::plus_c_d (constant * args) {
  _ARC0 (c1);
  _ARD1 (d2);
  _DEFC ();
  _RETC (*c1 + d2);
}

constant * evaluate::plus_d_c (constant * args) {
  _ARD0 (d1);
  _ARC1 (c2);
  _DEFC ();
  _RETC (d1 + *c2);
}

constant * evaluate::plus_v_d (constant * args) {
  _ARV0 (v1);
  _ARD1 (d2);
  _DEFV ();
  _RETV (*v1 + d2);
}

constant * evaluate::plus_d_v (constant * args) {
  _ARD0 (d1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (d1 + *v2);
}

constant * evaluate::plus_v_c (constant * args) {
  _ARV0 (v1);
  _ARC1 (c2);
  _DEFV ();
  _RETV (*v1 + *c2);
}

constant * evaluate::plus_c_v (constant * args) {
  _ARC0 (c1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (*v2 + *c1);
}

constant * evaluate::plus_v_v (constant * args) {
  _ARV0 (v1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (*v1 + *v2);
}

constant * evaluate::plus_m_m (constant * args) {
  _ARM0 (m1);
  _ARM1 (m2);
  _DEFM ();
  _RETM (*m1 + *m2);
}

constant * evaluate::plus_mv_mv (constant * args) {
  _ARMV0 (v1);
  _ARMV1 (v2);
  _DEFMV ();
  _RETMV (*v1 + *v2);
}

constant * evaluate::plus_mv_m (constant * args) {
  _ARMV0 (v1);
  _ARM1 (m2);
  _DEFMV ();
  _RETMV (*v1 + *m2);
}

constant * evaluate::plus_m_mv (constant * args) {
  _ARM0 (m1);
  _ARMV1 (v2);
  _DEFMV ();
  _RETMV (*m1 + *v2);
}

constant * evaluate::plus_m_d (constant * args) {
  _ARM0 (m1);
  _ARD1 (d2);
  _DEFM ();
  _RETM (*m1 + d2);
}

constant * evaluate::plus_d_m (constant * args) {
  _ARD0 (d1);
  _ARM1 (m2);
  _DEFM ();
  _RETM (d1 + *m2);
}

constant * evaluate::plus_m_c (constant * args) {
  _ARM0 (m1);
  _ARC1 (c2);
  _DEFM ();
  _RETM (*m1 + *c2);
}

constant * evaluate::plus_c_m (constant * args) {
  _ARC0 (c1);
  _ARM1 (m2);
  _DEFM ();
  _RETM (*c1 + *m2);
}

constant * evaluate::plus_mv_d (constant * args) {
  _ARMV0 (m1);
  _ARD1  (d2);
  _DEFMV ();
  _RETMV (*m1 + d2);
}

constant * evaluate::plus_d_mv (constant * args) {
  _ARD0  (d1);
  _ARMV1 (m2);
  _DEFMV ();
  _RETMV (d1 + *m2);
}

constant * evaluate::plus_mv_c (constant * args) {
  _ARMV0 (m1);
  _ARC1  (c2);
  _DEFMV ();
  _RETMV (*m1 + *c2);
}

constant * evaluate::plus_c_mv (constant * args) {
  _ARC0  (c1);
  _ARMV1 (m2);
  _DEFMV ();
  _RETMV (*c1 + *m2);
}

constant * evaluate::plus_mv_v (constant * args) {
  _ARMV0 (m1);
  _ARV1  (v2);
  _DEFMV ();
  _RETMV (*m1 + *v2);
}

constant * evaluate::plus_v_mv (constant * args) {
  _ARV0  (v1);
  _ARMV1 (m2);
  _DEFMV ();
  _RETMV (*v1 + *m2);
}

constant * evaluate::plus_s_s (constant * args) {
  char * s1 = STR (_ARES(0));
  char * s2 = STR (_ARES(1));
  constant * res = new constant (TAG_STRING);
  char * p = (char *) malloc (strlen (s1) + strlen (s2) + 1);
  strcpy (p, s1); strcat (p, s2);
  res->s = p;
  return res;
}

constant * evaluate::plus_c_s (constant * args) {
  char   c1 = CHR (_ARES(0));
  char * s2 = STR (_ARES(1));
  constant * res = new constant (TAG_STRING);
  char * p = (char *) malloc (strlen (s2) + 2);
  p[0] = c1; strcpy (&p[1], s2);
  res->s = p;
  return res;
}

constant * evaluate::plus_s_c (constant * args) {
  char * s1 = STR (_ARES(0));
  char   c2 = CHR (_ARES(1));
  constant * res = new constant (TAG_STRING);
  char * p = (char *) malloc (strlen (s1) + 2);
  strcpy (p, s1); p[strlen (s1)] = c2; p[strlen (s1) + 1] = '\0';
  res->s = p;
  return res;
}

// ******************** unary minus ***************************
constant * evaluate::minus_d (constant * args) {
  _ARD0 (d1); _DEFD (); _RETD (-d1);
}

constant * evaluate::minus_c (constant * args) {
  _ARC0 (c1); _DEFC (); _RETC (-*c1);
}

constant * evaluate::minus_v (constant * args) {
  _ARV0 (v1); _DEFV (); _RETV (-*v1);
}

constant * evaluate::minus_m (constant * args) {
  _ARM0 (m1); _DEFM (); _RETM (-*m1);
}

constant * evaluate::minus_mv (constant * args) {
  _ARMV0 (v1); _DEFMV (); _RETMV (-*v1);
}

// ****************** subtraction *****************************
constant * evaluate::minus_d_d (constant * args) {
  _ARD0 (d1);
  _ARD1 (d2);
  _DEFD ();
  _RETD (d1 - d2);
}

constant * evaluate::minus_c_c (constant * args) {
  _ARC0 (c1);
  _ARC1 (c2);
  _DEFC ();
  _RETC (*c1 - *c2);
}

constant * evaluate::minus_c_d (constant * args) {
  _ARC0 (c1);
  _ARD1 (d2);
  _DEFC ();
  _RETC (*c1 - d2);
}

constant * evaluate::minus_d_c (constant * args) {
  _ARD0 (d1);
  _ARC1 (c2);
  _DEFC ();
  _RETC (d1 - *c2);
}

constant * evaluate::minus_v_d (constant * args) {
  _ARV0 (v1);
  _ARD1 (d2);
  _DEFV ();
  _RETV (*v1 - d2);
}

constant * evaluate::minus_d_v (constant * args) {
  _ARD0 (d1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (d1 - *v2);
}

constant * evaluate::minus_v_c (constant * args) {
  _ARV0 (v1);
  _ARC1 (c2);
  _DEFV ();
  _RETV (*v1 - *c2);
}

constant * evaluate::minus_c_v (constant * args) {
  _ARC0 (c1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (*c1 - *v2);
}

constant * evaluate::minus_v_v (constant * args) {
  _ARV0 (v1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (*v1 - *v2);
}

constant * evaluate::minus_m_m (constant * args) {
  _ARM0 (m1);
  _ARM1 (m2);
  _DEFM ();
  _RETM (*m1 - *m2);
}

constant * evaluate::minus_mv_mv (constant * args) {
  _ARMV0 (v1);
  _ARMV1 (v2);
  _DEFMV ();
  _RETMV (*v1 - *v2);
}

constant * evaluate::minus_mv_m (constant * args) {
  _ARMV0 (v1);
  _ARM1 (m2);
  _DEFMV ();
  _RETMV (*v1 - *m2);
}

constant * evaluate::minus_m_mv (constant * args) {
  _ARM0 (m1);
  _ARMV1 (v2);
  _DEFMV ();
  _RETMV (*m1 - *v2);
}

constant * evaluate::minus_m_d (constant * args) {
  _ARM0 (m1);
  _ARD1 (d2);
  _DEFM ();
  _RETM (*m1 - d2);
}

constant * evaluate::minus_d_m (constant * args) {
  _ARD0 (d1);
  _ARM1 (m2);
  _DEFM ();
  _RETM (d1 - *m2);
}

constant * evaluate::minus_m_c (constant * args) {
  _ARM0 (m1);
  _ARC1 (c2);
  _DEFM ();
  _RETM (*m1 - *c2);
}

constant * evaluate::minus_c_m (constant * args) {
  _ARC0 (c1);
  _ARM1 (m2);
  _DEFM ();
  _RETM (*c1 - *m2);
}

constant * evaluate::minus_mv_d (constant * args) {
  _ARMV0 (m1);
  _ARD1  (d2);
  _DEFMV ();
  _RETMV (*m1 - d2);
}

constant * evaluate::minus_d_mv (constant * args) {
  _ARD0  (d1);
  _ARMV1 (m2);
  _DEFMV ();
  _RETMV (d1 - *m2);
}

constant * evaluate::minus_mv_c (constant * args) {
  _ARMV0 (m1);
  _ARC1  (c2);
  _DEFMV ();
  _RETMV (*m1 - *c2);
}

constant * evaluate::minus_c_mv (constant * args) {
  _ARC0  (c1);
  _ARMV1 (m2);
  _DEFMV ();
  _RETMV (*c1 - *m2);
}

constant * evaluate::minus_mv_v (constant * args) {
  _ARMV0 (m1);
  _ARV1  (v2);
  _DEFMV ();
  _RETMV (*m1 - *v2);
}

constant * evaluate::minus_v_mv (constant * args) {
  _ARV0  (v1);
  _ARMV1 (m2);
  _DEFMV ();
  _RETMV (*v1 - *m2);
}

// ****************** multiplication *************************
constant * evaluate::times_d_d (constant * args) {
  _ARD0 (d1);
  _ARD1 (d2);
  _DEFD ();
  _RETD (d1 * d2);
}

constant * evaluate::times_c_c (constant * args) {
  _ARC0 (c1);
  _ARC1 (c2);
  _DEFC ();
  _RETC ((*c1) * (*c2));
}

constant * evaluate::times_c_d (constant * args) {
  _ARC0 (c1);
  _ARD1 (d2);
  _DEFC ();
  _RETC ((*c1) * d2);
}

constant * evaluate::times_d_c (constant * args) {
  _ARD0 (d1);
  _ARC1 (c2);
  _DEFC ();
  _RETC (d1 * (*c2));
}

constant * evaluate::times_v_d (constant * args) {
  _ARV0 (v1);
  _ARD1 (d2);
  _DEFV ();
  _RETV (*v1 * d2);
  return res;
}

constant * evaluate::times_d_v (constant * args) {
  _ARD0 (d1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (d1 * *v2);
}

constant * evaluate::times_v_c (constant * args) {
  _ARV0 (v1);
  _ARC1 (c2);
  _DEFV ();
  _RETV (*v1 * *c2);
}

constant * evaluate::times_c_v (constant * args) {
  _ARC0 (c1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (*c1 * *v2);
}

constant * evaluate::times_v_v (constant * args) {
  _ARV0 (v1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (*v1 * *v2);
}

constant * evaluate::times_m_m (constant * args) {
  _ARM0 (m1);
  _ARM1 (m2);
  _DEFM ();
  if (m1->getCols () != m2->getRows ()) {
    THROW_MATH_EXCEPTION ("nonconformant arguments in matrix multiplication");
    res->m = new matrix (m1->getRows (), m2->getCols ());
  } else {
    res->m = new matrix (*m1 * *m2);
  }
  return res;
}

constant * evaluate::times_m_c (constant * args) {
  _ARM0 (m1);
  _ARC1 (c2);
  _DEFM ();
  _RETM (*m1 * *c2);
}

constant * evaluate::times_c_m (constant * args) {
  _ARC0 (c1);
  _ARM1 (m2);
  _DEFM ();
  _RETM (*c1 * *m2);
}

constant * evaluate::times_m_d (constant * args) {
  _ARM0 (m1);
  _ARD1 (d2);
  _DEFM ();
  _RETM (*m1 * d2);
}

constant * evaluate::times_d_m (constant * args) {
  _ARD0 (d1);
  _ARM1 (m2);
  _DEFM ();
  _RETM (d1 * *m2);
}

constant * evaluate::times_mv_mv (constant * args) {
  _ARMV0 (v1);
  _ARMV1 (v2);
  _DEFMV ();
  if (v1->getCols () != v2->getRows ()) {
    THROW_MATH_EXCEPTION ("nonconformant arguments in matrix multiplication");
    res->mv = new matvec (v1->getSize (), v1->getRows (), v2->getCols ());
  } else {
    res->mv = new matvec (*v1 * *v2);
  }
  return res;
}

constant * evaluate::times_mv_c (constant * args) {
  _ARMV0 (v1);
  _ARC1 (c2);
  _DEFMV ();
  _RETMV (*v1 * *c2);
}

constant * evaluate::times_c_mv (constant * args) {
  _ARC0 (c1);
  _ARMV1 (v2);
  _DEFMV ();
  _RETMV (*c1 * *v2);
}

constant * evaluate::times_mv_d (constant * args) {
  _ARMV0 (v1);
  _ARD1 (d2);
  _DEFMV ();
  _RETMV (*v1 * d2);
}

constant * evaluate::times_d_mv (constant * args) {
  _ARD0 (d1);
  _ARMV1 (v2);
  _DEFMV ();
  _RETMV (d1 * *v2);
}

constant * evaluate::times_mv_m (constant * args) {
  _ARMV0 (v1);
  _ARM1 (m2);
  _DEFMV ();
  if (v1->getCols () != m2->getRows ()) {
    THROW_MATH_EXCEPTION ("nonconformant arguments in matrix multiplication");
    res->mv = new matvec (v1->getSize (), v1->getRows (), m2->getCols ());
  } else {
    res->mv = new matvec (*v1 * *m2);
  }
  return res;
}

constant * evaluate::times_m_mv (constant * args) {
  _ARM0 (m1);
  _ARMV1 (v2);
  _DEFMV ();
  if (m1->getCols () != v2->getRows ()) {
    THROW_MATH_EXCEPTION ("nonconformant arguments in matrix multiplication");
    res->mv = new matvec (v2->getSize (), m1->getRows (), v2->getCols ());
  } else {
    res->mv = new matvec (*m1 * *v2);
  }
  return res;
}

constant * evaluate::times_mv_v (constant * args) {
  _ARMV0 (v1);
  _ARV1 (v2);
  _DEFMV ();
  _RETMV (*v1 * *v2);
}

constant * evaluate::times_v_mv (constant * args) {
  _ARV0 (v1);
  _ARMV1 (v2);
  _DEFMV ();
  _RETMV (*v1 * *v2);
}

// ****************** division *************************
constant * evaluate::over_d_d (constant * args) {
  _ARD0 (d1);
  _ARD1 (d2);
  _DEFD ();
  if (d2 == 0.0) THROW_MATH_EXCEPTION ("division by zero");
  _RETD (d1 / d2);
}

constant * evaluate::over_c_c (constant * args) {
  _ARC0 (c1);
  _ARC1 (c2);
  _DEFC ();
  if (*c2 == 0.0) THROW_MATH_EXCEPTION ("division by zero");
  _RETC (*c1 / *c2);
}

constant * evaluate::over_c_d (constant * args) {
  _ARC0 (c1);
  _ARD1 (d2);
  _DEFC ();
  if (d2 == 0.0) THROW_MATH_EXCEPTION ("division by zero");
  _RETC (*c1 / d2);
}

constant * evaluate::over_d_c (constant * args) {
  _ARD0 (d1);
  _ARC1 (c2);
  _DEFC ();
  if (*c2 == 0.0) THROW_MATH_EXCEPTION ("division by zero");
  _RETC (d1 / *c2);
}

constant * evaluate::over_v_d (constant * args) {
  _ARV0 (v1);
  _ARD1 (d2);
  _DEFV ();
  if (d2 == 0.0) THROW_MATH_EXCEPTION ("division by zero");
  _RETV (*v1 / d2);
}

constant * evaluate::over_d_v (constant * args) {
  _ARD0 (d1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (d1 / *v2);
}

constant * evaluate::over_v_c (constant * args) {
  _ARV0 (v1);
  _ARC1 (c2);
  _DEFV ();
  if (*c2 == 0.0) THROW_MATH_EXCEPTION ("division by zero");
  _RETV (*v1 / *c2);
}

constant * evaluate::over_c_v (constant * args) {
  _ARC0 (c1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (*c1 / *v2);
}

constant * evaluate::over_v_v (constant * args) {
  _ARV0 (v1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (*v1 / *v2);
}

constant * evaluate::over_m_c (constant * args) {
  _ARM0 (m1);
  _ARC1 (c2);
  _DEFM ();
  _RETM (*m1 / *c2);
}

constant * evaluate::over_m_d (constant * args) {
  _ARM0 (m1);
  _ARD1 (d2);
  _DEFM ();
  _RETM (*m1 / d2);
}

constant * evaluate::over_mv_c (constant * args) {
  _ARMV0 (v1);
  _ARC1 (c2);
  _DEFMV ();
  _RETMV (*v1 / *c2);
}

constant * evaluate::over_mv_d (constant * args) {
  _ARMV0 (v1);
  _ARD1 (d2);
  _DEFMV ();
  _RETMV (*v1 / d2);
}

constant * evaluate::over_mv_v (constant * args) {
  _ARMV0 (v1);
  _ARV1 (v2);
  _DEFMV ();
  _RETMV (*v1 / *v2);
}

// ****************** modulo *************************
constant * evaluate::modulo_d_d (constant * args) {
  _ARD0 (d1);
  _ARD1 (d2);
  _DEFD ();
  _RETD (std::fmod (d1, d2));
}

constant * evaluate::modulo_c_c (constant * args) {
  _ARC0 (c1);
  _ARC1 (c2);
  _DEFC ();
  _RETC ((*c1) % (*c2));
}

constant * evaluate::modulo_c_d (constant * args) {
  _ARC0 (c1);
  _ARD1 (d2);
  _DEFC ();
  _RETC ((*c1) % d2);
}

constant * evaluate::modulo_d_c (constant * args) {
  _ARD0 (d1);
  _ARC1 (c2);
  _DEFC ();
  _RETC (d1 % (*c2));
}

constant * evaluate::modulo_v_d (constant * args) {
  _ARV0 (v1);
  _ARD1 (d2);
  _DEFV ();
  _RETV (*v1 % d2);
}

constant * evaluate::modulo_d_v (constant * args) {
  _ARD0 (d1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (d1 % *v2);
}

constant * evaluate::modulo_v_c (constant * args) {
  _ARV0 (v1);
  _ARC1 (c2);
  _DEFV ();
  _RETV (*v1 % *c2);
}

constant * evaluate::modulo_c_v (constant * args) {
  _ARC0 (c1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (*c1 % *v2);
}

constant * evaluate::modulo_v_v (constant * args) {
  _ARV0 (v1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (*v1 % *v2);
}

// ****************** power *************************
constant * evaluate::power_d_d (constant * args) {
  _ARD0 (d1);
  _ARD1 (d2);
  _DEFD ();
  _RETD (std::pow (d1, d2));
}

constant * evaluate::power_c_c (constant * args) {
  _ARC0 (c1);
  _ARC1 (c2);
  _DEFC ();
  _RETC (std::pow (*c1, *c2));
}

constant * evaluate::power_c_d (constant * args) {
  _ARC0 (c1);
  _ARD1 (d2);
  _DEFC ();
  _RETC (pow (*c1, d2));
}

constant * evaluate::power_d_c (constant * args) {
  _ARD0 (d1);
  _ARC1 (c2);
  _DEFC ();
  _RETC (pow (d1, *c2));
}

constant * evaluate::power_v_d (constant * args) {
  _ARV0 (v1);
  _ARD1 (d2);
  _DEFV ();
  _RETV (pow (*v1, d2));
}

constant * evaluate::power_d_v (constant * args) {
  _ARD0 (d1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (pow (d1, *v2));
}

constant * evaluate::power_v_c (constant * args) {
  _ARV0 (v1);
  _ARC1 (c2);
  _DEFV ();
  _RETV (pow (*v1, *c2));
}

constant * evaluate::power_c_v (constant * args) {
  _ARC0 (c1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (pow (*c1, *v2));
}

constant * evaluate::power_v_v (constant * args) {
  _ARV0 (v1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (pow (*v1, *v2));
}

constant * evaluate::power_m_d (constant * args) {
  _ARM0 (m1);
  _ARI1 (i2);
  _DEFM ();
  _RETM (pow (*m1, i2));
}

constant * evaluate::power_m_c (constant * args) {
  _ARM0 (m1);
  _ARC1 (c2);
  _DEFM ();
  _RETM (pow (*m1, (int) real (*c2)));
}

constant * evaluate::power_mv_d (constant * args) {
  _ARMV0 (m1);
  _ARI1  (i2);
  _DEFMV ();
  _RETMV (pow (*m1, i2));
}

constant * evaluate::power_mv_c (constant * args) {
  _ARMV0 (m1);
  _ARC1  (c2);
  _DEFMV ();
  _RETMV (pow (*m1, (int) real (*c2)));
}

constant * evaluate::power_mv_v (constant * args) {
  _ARMV0 (m1);
  _ARV1  (v2);
  _DEFMV ();
  _RETMV (pow (*m1, *v2));
}

// ****************** hypotenuse *************************
constant * evaluate::xhypot_d_d (constant * args) {
  _ARD0 (d1);
  _ARD1 (d2);
  _DEFD ();
  _RETD (xhypot (d1, d2));
}

constant * evaluate::xhypot_c_c (constant * args) {
  _ARC0 (c1);
  _ARC1 (c2);
  _DEFD ();
  _RETD (xhypot (*c1, *c2));
}

constant * evaluate::xhypot_c_d (constant * args) {
  _ARC0 (c1);
  _ARD1 (d2);
  _DEFD ();
  _RETD (xhypot (*c1, d2));
}

constant * evaluate::xhypot_d_c (constant * args) {
  _ARD0 (d1);
  _ARC1 (c2);
  _DEFD ();
  _RETD (xhypot (d1, *c2));
}

constant * evaluate::xhypot_v_d (constant * args) {
  _ARV0 (v1);
  _ARD1 (d2);
  _DEFV ();
  _RETV (xhypot (*v1, d2));
}

constant * evaluate::xhypot_d_v (constant * args) {
  _ARD0 (d1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (xhypot (d1, *v2));
}

constant * evaluate::xhypot_v_c (constant * args) {
  _ARV0 (v1);
  _ARC1 (c2);
  _DEFV ();
  _RETV (xhypot (*v1, *c2));
}

constant * evaluate::xhypot_c_v (constant * args) {
  _ARC0 (c1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (xhypot (*c1, *v2));
}

constant * evaluate::xhypot_v_v (constant * args) {
  _ARV0 (v1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (xhypot (*v1, *v2));
}

// ************** conjugate complex **********************
constant * evaluate::conj_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (d1);
}

constant * evaluate::conj_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (conj (*c1));
}

constant * evaluate::conj_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (conj (*v1));
}

constant * evaluate::conj_m (constant * args) {
  _ARM0 (m);
  _DEFM ();
  _RETM (conj (*m));
}

constant * evaluate::conj_mv (constant * args) {
  _ARMV0 (mv);
  _DEFMV ();
  _RETMV (conj (*mv));
}

// ********** square of absolute value *****************
constant * evaluate::norm_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (d1 * d1);
}

constant * evaluate::norm_c (constant * args) {
  _ARC0 (c1);
  _DEFD ();
  _RETD (norm (*c1));
}

constant * evaluate::norm_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (norm (*v1));
}

// ********** phase in degrees *****************
constant * evaluate::phase_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (d1 < 0.0 ? 180.0 : 0.0);
}

constant * evaluate::phase_c (constant * args) {
  _ARC0 (c1);
  _DEFD ();
  _RETD (rad2deg (arg (*c1)));
}

constant * evaluate::phase_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (rad2deg (arg (*v1)));
}

constant * evaluate::phase_m (constant * args) {
  _ARM0 (m1);
  _DEFM ();
  _RETM (rad2deg (arg (*m1)));
}

constant * evaluate::phase_mv (constant * args) {
  _ARMV0 (v1);
  _DEFMV ();
  _RETMV (rad2deg (arg (*v1)));
}

// ********** phase in radians *****************
constant * evaluate::arg_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (d1 < 0.0 ? pi : 0.0);
}

constant * evaluate::arg_c (constant * args) {
  _ARC0 (c1);
  _DEFD ();
  _RETD (arg (*c1));
}

constant * evaluate::arg_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (arg (*v1));
}

constant * evaluate::arg_m (constant * args) {
  _ARM0 (m1);
  _DEFM ();
  _RETM (arg (*m1));
}

constant * evaluate::arg_mv (constant * args) {
  _ARMV0 (v1);
  _DEFMV ();
  _RETMV (arg (*v1));
}

// ******* unwrap phase in radians ************
constant * evaluate::unwrap_v_1 (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (unwrap (*v1));
}

constant * evaluate::unwrap_v_2 (constant * args) {
  _ARV0 (v1);
  _ARD1 (d2);
  _DEFV ();
  _RETV (unwrap (*v1, fabs (d2)));
}

constant * evaluate::unwrap_v_3 (constant * args) {
  _ARV0 (v1);
  _ARD1 (d2);
  _ARD2 (d3);
  _DEFV ();
  _RETV (unwrap (*v1, fabs (d2), fabs (d3)));
}

// ******** radian/degree conversion **********
constant * evaluate::deg2rad_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (deg2rad (d1));
}

constant * evaluate::deg2rad_c (constant * args) {
  _ARC0 (c1);
  _DEFD ();
  _RETD (deg2rad (*c1));
}

constant * evaluate::deg2rad_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (deg2rad (*v1));
}

constant * evaluate::rad2deg_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (rad2deg (d1));
}

constant * evaluate::rad2deg_c (constant * args) {
  _ARC0 (c1);
  _DEFD ();
  _RETD (rad2deg (*c1));
}

constant * evaluate::rad2deg_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (rad2deg (*v1));
}

// ********** voltage decibel *****************
constant * evaluate::dB_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (10.0 * std::log10 (std::fabs (d1)));
}

constant * evaluate::dB_c (constant * args) {
  _ARC0 (c1);
  _DEFD ();
  _RETD (dB (*c1));
}

constant * evaluate::dB_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (dB (*v1));
}

constant * evaluate::dB_m (constant * args) {
  _ARM0 (m1);
  _DEFM ();
  _RETM (dB (*m1));
}

constant * evaluate::dB_mv (constant * args) {
  _ARMV0 (v1);
  _DEFMV ();
  _RETMV (dB (*v1));
}

// ********** square root *****************
constant * evaluate::sqrt_d (constant * args) {
  _ARD0 (d1);
  _DEFC ();
  if (d1 < 0.0)
    res->c = new nr_complex_t (0.0, std::sqrt (-d1));
  else
    res->c = new nr_complex_t (std::sqrt (d1));
  return res;
}

constant * evaluate::sqrt_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (std::sqrt (*c1));
}

constant * evaluate::sqrt_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (sqrt (*v1));
}

// ********** natural logarithm *****************
constant * evaluate::ln_d (constant * args) {
  _ARD0 (d1);
  _DEFC ();
  if (d1 < 0.0)
    res->c = new nr_complex_t (std::log (-d1), pi);
  else
    res->c = new nr_complex_t (std::log (d1));
  return res;
}

constant * evaluate::ln_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (std::log (*c1));
}

constant * evaluate::ln_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (log (*v1));
}

// ********** decimal logarithm *****************
constant * evaluate::log10_d (constant * args) {
  _ARD0 (d1);
  _DEFC ();
  if (d1 < 0.0)
    res->c = new nr_complex_t (std::log10 (-d1), pi * log10e);
  else
    res->c = new nr_complex_t (std::log10 (d1));
  return res;
}

constant * evaluate::log10_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (std::log10 (*c1));
}

constant * evaluate::log10_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (log10 (*v1));
}

// ********** binary logarithm *****************
constant * evaluate::log2_d (constant * args) {
  _ARD0 (d1);
  _DEFC ();
  if (d1 < 0.0)
    res->c = new nr_complex_t (std::log (-d1) * log2e, pi * log2e);
  else
    res->c = new nr_complex_t (std::log (d1) * log2e);
  return res;
}

constant * evaluate::log2_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (qucs::log2 (*c1));
}

constant * evaluate::log2_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (log2 (*v1));
}

// ************* arcus sine *********************
constant * evaluate::arcsin_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (std::asin (d1));
}

constant * evaluate::arcsin_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (asin (*c1));
}

constant * evaluate::arcsin_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (asin (*v1));
}

// ************* arcus cosine ******************
constant * evaluate::arccos_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (std::acos (d1));
}

constant * evaluate::arccos_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (qucs::acos (*c1));
}

constant * evaluate::arccos_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (acos (*v1));
}

// ************** arcus tangent ******************
constant * evaluate::arctan_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (std::atan (d1));
}

constant * evaluate::arctan_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (qucs::atan (*c1));
}

constant * evaluate::arctan_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (atan (*v1));
}

// *************** cotangent ********************
constant * evaluate::cot_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (1.0 / std::tan (d1));
}

constant * evaluate::cot_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (qucs::cot (*c1));
}

constant * evaluate::cot_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (cot (*v1));
}

// ************ arcus cotangent *****************
constant * evaluate::arccot_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (pi_over_2 - std::atan (d1));
}

constant * evaluate::arccot_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (qucs::acot (*c1));
}

constant * evaluate::arccot_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (acot (*v1));
}

// ***************** secans *********************
constant * evaluate::sec_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (1.0 / std::cos (d1));
}

constant * evaluate::sec_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (1.0 / std::cos (*c1));
}

constant * evaluate::sec_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (1.0 / qucs::cos (*v1));
}

// *************** arcus secans *******************
constant * evaluate::arcsec_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (std::acos (1.0 / d1));
}

constant * evaluate::arcsec_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (acos (1.0 / *c1));
}

constant * evaluate::arcsec_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (acos (1.0 / *v1));
}

// ***************** cosecans *********************
constant * evaluate::cosec_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (1.0 / sin (d1));
}

constant * evaluate::cosec_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (1.0 / sin (*c1));
}

constant * evaluate::cosec_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (1.0 / sin (*v1));
}

// ************* arcus cosecans *******************
constant * evaluate::arccosec_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (std::asin (1.0 / d1));
}

constant * evaluate::arccosec_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (asin (1.0 / *c1));
}

constant * evaluate::arccosec_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (asin (1.0 / *v1));
}

// ********** area sine hyperbolicus **************
constant * evaluate::arsinh_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (log (d1 + sqrt (d1 * d1 + 1)));
}

constant * evaluate::arsinh_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (asinh (*c1));
}

constant * evaluate::arsinh_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (asinh (*v1));
}

// ********** area cosecans hyperbolicus **************
constant * evaluate::arcosech_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  d1 = 1 / d1;
  _RETD (log (d1 + sqrt (d1 * d1 + 1)));
}

constant * evaluate::arcosech_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (asinh (1.0 / *c1));
}

constant * evaluate::arcosech_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (asinh (1 / *v1));
}

// ********* area cosine hyperbolicus ************
constant * evaluate::arcosh_d (constant * args) {
  _ARD0 (d1);
  _DEFC ();
  _RETC (acosh (nr_complex_t (d1)));
}

constant * evaluate::arcosh_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (acosh (*c1));
}

constant * evaluate::arcosh_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (acosh (*v1));
}

// ********* area secans hyperbolicus ***********
constant * evaluate::arsech_d (constant * args) {
  _ARD0 (d1);
  _DEFC ();
  _RETC (asech (nr_complex_t (d1)));
}

constant * evaluate::arsech_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (asech (*c1));
}

constant * evaluate::arsech_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (asech (*v1));
}

// ******* area tangent hyperbolicus **********
constant * evaluate::artanh_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (0.5 * log ((1.0 + d1) / (1.0 - d1)));
}

constant * evaluate::artanh_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (atanh (*c1));
}

constant * evaluate::artanh_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (atanh (*v1));
}

// ******* area cotangent hyperbolicus **********
constant * evaluate::arcoth_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (0.5 * std::log ((d1 + 1.0) / (d1 - 1.0)));
}

constant * evaluate::arcoth_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (qucs::acoth (*c1));
}

constant * evaluate::arcoth_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (qucs::acoth (*v1));
}

// This is the rtoz, ztor, ytor, rtoy helper macro.
#define MAKE_FUNC_DEFINITION_2(cfunc) \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_d) (constant * args) {   \
  _ARD0 (d);							    \
  _DEFD ();							    \
  _RETD (real (cfunc (nr_complex_t (d, 0))));				    \
}								    \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_d_d) (constant * args) { \
  _ARD0 (d);							    \
  _ARD1 (z);							    \
  _DEFD ();							    \
  _RETD (real (cfunc (nr_complex_t (d, 0), z)));			    \
}								    \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_d_c) (constant * args) { \
  _ARD0 (d);							    \
  _ARC1 (z);							    \
  _DEFC ();							    \
  _RETC (cfunc (nr_complex_t (d, 0), *z));				    \
}								    \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_c) (constant * args) {   \
  _ARC0 (c);							    \
  _DEFC ();							    \
  _RETC (cfunc (*c));						    \
}								    \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_c_d) (constant * args) { \
  _ARC0 (c);							    \
  _ARD1 (z);							    \
  _DEFC ();							    \
  _RETC (cfunc (*c, z));					    \
}								    \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_c_c) (constant * args) { \
  _ARC0 (c);							    \
  _ARC1 (z);							    \
  _DEFC ();							    \
  _RETC (cfunc (*c, *z));					    \
}								    \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_v) (constant * args) {   \
  _ARV0 (v);							    \
  _DEFV ();							    \
  _RETV (cfunc (*v));						    \
}								    \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_v_d) (constant * args) { \
  _ARV0 (v);							    \
  _ARD1 (z);							    \
  _DEFV ();							    \
  _RETV (cfunc (*v, z));					    \
}								    \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_v_c) (constant * args) { \
  _ARV0 (v);							    \
  _ARC1 (z);							    \
  _DEFV ();							    \
  _RETV (cfunc (*v, *z));					    \
}								    \

MAKE_FUNC_DEFINITION_2 (ztor);
MAKE_FUNC_DEFINITION_2 (rtoz);
MAKE_FUNC_DEFINITION_2 (ytor);
MAKE_FUNC_DEFINITION_2 (rtoy);

// ** convert reflexion coefficient to standing wave ratio **
constant * evaluate::rtoswr_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD ((1 + fabs (d1)) / (1 - fabs (d1)));
}

constant * evaluate::rtoswr_c (constant * args) {
  _ARC0 (c1);
  _DEFD ();
  _RETD ((1 + abs (*c1)) / (1 - abs (*c1)));
}

constant * evaluate::rtoswr_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  res->v = new qucs::vector (v1->getSize ());
  for (int i = 0; i < v1->getSize (); i++)
    res->v->set ((1 + abs (v1->get (i))) / (1 - abs (v1->get (i))), i);
  return res;
}

// ** differentiate vector with respect to another vector **
constant * evaluate::diff_v_2 (constant * args) {
  _ARV0 (v1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (diff (*v1, *v2));
}

constant * evaluate::diff_v_3 (constant * args) {
  _ARV0 (v1);
  _ARV1 (v2);
  _ARI2 (i3);
  _DEFV ();
  _RETV (diff (*v1, *v2, i3));
}

// ***************** maximum *******************
constant * evaluate::max_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (d1);
}

constant * evaluate::max_c (constant * args) {
  _ARC0 (c1);
  _DEFD ();
  if (fabs (arg (*c1)) < pi_over_2)
    res->d = abs (*c1);
  else
    res->d = -abs (*c1);
  return res;
}

constant * evaluate::max_v (constant * args) {
  _ARV0 (v1);
  _DEFD ();
  _RETD (v1->maximum ());
}

constant * evaluate::max_d_d (constant * args) {
  _ARD0 (d1);
  _ARD1 (d2);
  _DEFD ();
  _RETD (std::max (d1, d2));
}

constant * evaluate::max_d_c (constant * args) {
  _ARD0 (d1);
  _ARC1 (c2);
  _DEFC ();
  nr_double_t a = d1;
  nr_double_t b = fabs (arg (*c2)) < pi_over_2 ? abs (*c2) : -abs (*c2);
  nr_complex_t r = a > b ? d1 : *c2;
  _RETC (r);
}

constant * evaluate::max_c_c (constant * args) {
  _ARC0 (c1);
  _ARC1 (c2);
  _DEFC ();
  nr_double_t a = fabs (arg (*c1)) < pi_over_2 ? abs (*c1) : -abs (*c1);
  nr_double_t b = fabs (arg (*c2)) < pi_over_2 ? abs (*c2) : -abs (*c2);
  nr_complex_t r = a > b ? *c1 : *c2;
  _RETC (r);
}

constant * evaluate::max_c_d (constant * args) {
  _ARC0 (c1);
  _ARD1 (d2);
  _DEFC ();
  nr_double_t a = fabs (arg (*c1)) < pi_over_2 ? abs (*c1) : -abs (*c1);
  nr_double_t b = d2;
  nr_complex_t r = a > b ? *c1 : d2;
  _RETC (r);
}

// ***************** minimum *******************
constant * evaluate::min_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (d1);
}

constant * evaluate::min_c (constant * args) {
  _ARC0 (c1);
  _DEFD ();
  if (fabs (arg (*c1)) < pi_over_2)
    res->d = abs (*c1);
  else
    res->d = -abs (*c1);
  return res;
}

constant * evaluate::min_v (constant * args) {
  _ARV0 (v1);
  _DEFD ();
  _RETD (v1->minimum ());
}

constant * evaluate::min_d_d (constant * args) {
  _ARD0 (d1);
  _ARD1 (d2);
  _DEFD ();
  _RETD (std::min (d1, d2));
}

constant * evaluate::min_d_c (constant * args) {
  _ARD0 (d1);
  _ARC1 (c2);
  _DEFC ();
  nr_double_t a = d1;
  nr_double_t b = fabs (arg (*c2)) < pi_over_2 ? abs (*c2) : -abs (*c2);
  nr_complex_t r = a < b ? d1 : *c2;
  _RETC (r);
}

constant * evaluate::min_c_c (constant * args) {
  _ARC0 (c1);
  _ARC1 (c2);
  _DEFC ();
  nr_double_t a = fabs (arg (*c1)) < pi_over_2 ? abs (*c1) : -abs (*c1);
  nr_double_t b = fabs (arg (*c2)) < pi_over_2 ? abs (*c2) : -abs (*c2);
  nr_complex_t r = a < b ? *c1 : *c2;
  _RETC (r);
}

constant * evaluate::min_c_d (constant * args) {
  _ARC0 (c1);
  _ARD1 (d2);
  _DEFC ();
  nr_double_t a = fabs (arg (*c1)) < pi_over_2 ? abs (*c1) : -abs (*c1);
  nr_double_t b = d2;
  nr_complex_t r = a < b ? *c1 : d2;
  _RETC (r);
}

// ******************** sum **********************
constant * evaluate::sum_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (d1);
}

constant * evaluate::sum_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (*c1);
}

constant * evaluate::sum_v (constant * args) {
  _ARV0 (v1);
  _DEFC ();
  _RETC (sum (*v1));
}

// ****************** product ********************
constant * evaluate::prod_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (d1);
}

constant * evaluate::prod_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (*c1);
}

constant * evaluate::prod_v (constant * args) {
  _ARV0 (v1);
  _DEFC ();
  _RETC (prod (*v1));
}

// ******************* average *********************
constant * evaluate::avg_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (d1);
}

constant * evaluate::avg_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (*c1);
}

constant * evaluate::avg_v (constant * args) {
  _ARV0 (v1);
  _DEFC ();
  _RETC (avg (*v1));
}

// ******************* lengths *********************
constant * evaluate::length_d (constant *) {
  _DEFD ();
  _RETD (1);
}

constant * evaluate::length_c (constant *) {
  _DEFD ();
  _RETD (1);
}

constant * evaluate::length_v (constant * args) {
  _ARV0 (v1);
  _DEFD ();
  _RETD (v1->getSize ());
}

constant * evaluate::length_m (constant *) {
  _DEFD ();
  _RETD (1);
}

constant * evaluate::length_mv (constant * args) {
  _ARV0 (mv);
  _DEFD ();
  _RETD (mv->getSize ());
}

// ***************** array indices *****************
constant * evaluate::index_mv_2 (constant * args) {
  _ARMV0 (mv);
  _ARI1 (r);
  _ARI2 (c);
  _DEFV ();
  if (r < 1 || r > mv->getRows () || c < 1 || c > mv->getCols ()) {
    char txt[256];
    sprintf (txt, "matvec indices [%d,%d] out of bounds [1-%d,1-%d]",
	     r, c, mv->getRows (), mv->getCols ());
    THROW_MATH_EXCEPTION (txt);
    res->v = new qucs::vector (mv->getSize ());
  } else {
    res->v = new qucs::vector (mv->get (r - 1, c - 1));
  }
  return res;
}

constant * evaluate::index_mv_1 (constant * args) {
  _ARMV0 (mv);
  _ARI1 (i);
  _DEFM ();
  if (i < 1 || i > mv->getSize ()) {
    char txt[256];
    sprintf (txt, "matvec index [%d] out of bounds [1-%d]", i, mv->getSize ());
    THROW_MATH_EXCEPTION (txt);
    res->m = new matrix (mv->getRows (), mv->getCols ());
  } else {
    res->m = new matrix (mv->get (i - 1));
  }
  return res;
}

/* Little helper macros for the following functionality. */
#define EQUATION_HAS_DEPS(v,n) \
  ((v)->getDataDependencies() != NULL && \
   (v)->getDataDependencies()->length() >= n)
#define EQUATION_DEPS(v) \
  ((v)->getDataDependencies() ? (v)->getDataDependencies()->length() : 1)

/* This following function is used to get a subset of data entries
   from a data vector with certain data dependencies. */
void evaluate::extract_vector (constant * args, int idx, int &skip, int &size,
			       constant * res) {
  _ARV0 (v);
  int i = INT (_ARES (idx));
  int type = _ARG(idx)->getType ();
  qucs::vector * vres;
  strlist * deps = _ARES(0)->getDataDependencies ();
  int didx = (deps ? deps->length () : 0) - idx;
  int dsize = SOLVEE(0)->getDependencySize (deps, idx);

  // all of the data vector
  if (type == TAG_RANGE) {
    if (dsize > 1) {
      // dependent vectors: only ':' possible
      vres = new qucs::vector (*(res->v));
      skip *= deps ? SOLVEE(0)->getDataSize (deps->get (didx - 1)) : 1;
      size *= deps ? SOLVEE(0)->getDataSize (deps->get (didx)) : 1;
    }
    else {
      // independent vectors
      range * r = RNG (_ARES (idx));
      int i, n, k;
      int len = res->v->getSize ();
      i = (int) r->lo ();
      if (i < 0 || i >= len) {
	char txt[256];
	sprintf (txt, "vector index %d out of bounds [%d,%d]", i, 0, len - 1);
	THROW_MATH_EXCEPTION (txt);
      }
      i = (int) r->hi ();
      if (i < 0 || i >= len) {
	char txt[256];
	sprintf (txt, "vector index %d out of bounds [%d,%d]", i, 0, len - 1);
	THROW_MATH_EXCEPTION (txt);
      }
      size = 0;
      for (n = 0; n < len; n++) if (r->inside (n)) size++;
      vres = new qucs::vector (size);
      for (k = 0, n = 0; n < len; n++) {
	if (r->inside (n))
	  vres->set (res->v->get (n), k++);
      }
    }
  }
  // a subset
  else {
    vres = new qucs::vector (dsize * size);
    int len = deps ? SOLVEE(0)->getDataSize (deps->get (didx)) : v->getSize ();
    if (i < 0 || i >= len) {
      char txt[256];
      sprintf (txt, "vector index %d (%d) out of bounds [%d,%d]",
	       idx, i, 0, len - 1);
      THROW_MATH_EXCEPTION (txt);
    } else {
      int k, n;
      for (n = k = 0; k < dsize * size; n += skip, k++) {
	vres->set (res->v->get (dsize * i + n), k);
      }
    }
    if (deps && didx >= 0) {
      res->addDropDependencies (deps->get (didx));
    }
  }
  delete res->v;
  res->v = vres;
}

constant * evaluate::index_v_1 (constant * args) {
  _ARV0 (v);
  _DEFV ();
  int skip = 1, size = 1;
  res->v = new qucs::vector (*v);
  extract_vector (args, 1, skip, size, res);
  return res;
}

constant * evaluate::index_v_2 (constant * args) {
  _ARV0 (v);
  _DEFV ();
  int skip = 1, size = 1;
  res->v = new qucs::vector (*v);
  if (!EQUATION_HAS_DEPS (_ARES(0), 2)) {
    char txt[256];
    sprintf (txt, "invalid number of vector indices (%d > %d)", 2,
	     EQUATION_DEPS (_ARES(0)));
    THROW_MATH_EXCEPTION (txt);
    return res;
  }
  extract_vector (args, 1, skip, size, res);
  extract_vector (args, 2, skip, size, res);
  return res;
}

constant * evaluate::index_m_2 (constant * args) {
  _ARM0 (m);
  _ARI1 (r);
  _ARI2 (c);
  _DEFC ();
  if (r < 1 || r > m->getRows () || c < 1 || c > m->getCols ()) {
    char txt[256];
    sprintf (txt, "matrix indices [%d,%d] out of bounds [1-%d,1-%d]",
	     r, c, m->getRows (), m->getCols ());
    THROW_MATH_EXCEPTION (txt);
    res->c = new nr_complex_t ();
  } else {
    res->c = new nr_complex_t (m->get (r - 1, c - 1));
  }
  return res;
}

constant * evaluate::index_s_1 (constant * args) {
  char * s = STR (_ARES(0));
  _ARI1 (i);
  constant * res = new constant (TAG_CHAR);
  res->chr = (i >= 0 && i < (int) strlen (s)) ? s[i] : ' ';
  return res;
}

// Interpolator helper macro without number of points given.
#define INTERPOL_HELPER()                     \
  constant * arg = new constant (TAG_DOUBLE); \
  arg->d = 64;                                \
  arg->solvee = args->getResult(0)->solvee;   \
  arg->evaluate ();                           \
  args->append (arg);

// ***************** interpolation *****************
constant * evaluate::interpolate_v_v (constant * args) {
  INTERPOL_HELPER();
  return interpolate_v_v_d (args);
}

constant * evaluate::interpolate_v_v_d (constant * args) {
  _ARV0 (v1);
  _ARV1 (v2);
  _ARI2 (n);
  _DEFV ();
  if (v1->getSize () < 3) {
    THROW_MATH_EXCEPTION ("interpolate: number of datapoints must be greater "
			  "than 2");
    res->v = new qucs::vector ();
    return res;
  }
  nr_double_t last  = real (v2->get (v2->getSize () - 1));
  nr_double_t first = real (v2->get (0));
  constant * arg = new constant (TAG_VECTOR);
  arg->v = new qucs::vector (qucs::linspace (first, last, n));
  arg->solvee = args->getResult(0)->solvee;
  arg->evaluate ();
  qucs::vector * val = new qucs::vector (n);
  spline spl (SPLINE_BC_NATURAL);
  spl.vectors (*v1, *v2);
  spl.construct ();
  for (int k = 0; k < arg->v->getSize (); k++) {
    val->set (spl.evaluate (real (arg->v->get (k))).f0, k);
  }
  res->v = val;
  node * gen = SOLVEE(0)->addGeneratedEquation (arg->v, "Interpolate");
  res->addPrepDependencies (A(gen)->result);
  res->dropdeps = 1;
  delete arg;
  return res;
}

// ***************** fourier transformations *****************
#define FOURIER_HELPER_1(efunc,cfunc,isign,dep) \
constant * evaluate:: QUCS_CONCAT2 (efunc,_v_v) (constant * args) { \
  _ARV0 (v);							    \
  _ARV1 (t);							    \
  _DEFV ();							    \
  qucs::vector * val = new qucs::vector (QUCS_CONCAT2 (cfunc,_1d) (*v));    \
  int k = val->getSize ();					    \
  *val = isign > 0 ? *val / k : *val * k;			    \
  res->v = val;							    \
  int n = t->getSize ();					    \
  if (k != n) {                                                     \
    THROW_MATH_EXCEPTION ("nonconformant vector lengths");          \
    return res; }                                                   \
  nr_double_t last  = real (t->get (n - 1));			    \
  nr_double_t first = real (t->get (0));			    \
  nr_double_t delta = (last - first) / (n - 1);			    \
  constant * arg = new constant (TAG_VECTOR);			    \
  arg->v = new qucs::vector (qucs::linspace (0, 1.0 / delta, n));	    \
  arg->solvee = args->getResult(0)->solvee;			    \
  arg->evaluate ();						    \
  node * gen = SOLVEE(0)->addGeneratedEquation (arg->v, dep);	    \
  res->addPrepDependencies (A(gen)->result);			    \
  res->dropdeps = 1;						    \
  args->append (arg);						    \
  return res;							    \
}

#define FOURIER_HELPER_2(cfunc) \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_v) (constant * args) { \
  _ARV0 (v);							  \
  _DEFV ();							  \
  qucs::vector * val = new qucs::vector (QUCS_CONCAT2 (cfunc,_1d) (*v));  \
  res->v = val;							  \
  res->dropdeps = 1;						  \
  return res;							  \
}

FOURIER_HELPER_1 (time2freq,dft,1,"Frequency");
FOURIER_HELPER_1 (freq2time,idft,-1,"Time");
FOURIER_HELPER_2 (fft);
FOURIER_HELPER_2 (ifft);
FOURIER_HELPER_2 (dft);
FOURIER_HELPER_2 (idft);

// Shuffles values of FFT arounds.
constant * evaluate::fftshift_v (constant * args) {
  _ARV0 (v);
  _DEFV ();
  res->v = new qucs::vector (fftshift (*v));
  return res;
}

// This is the stoz, ztos, ytos, stoy helper macro.
#define MAKE_FUNC_DEFINITION_3(cfunc) \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_m) (constant * args) {    \
  _ARM0 (m);							     \
  _DEFM ();							     \
  _RETM (cfunc (*m));						     \
}								     \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_m_d) (constant * args) {  \
  _ARM0 (m);							     \
  _ARD1 (z);							     \
  _DEFM ();							     \
  _RETM (cfunc (*m, nr_complex_t (z, 0)));				     \
}								     \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_m_c) (constant * args) {  \
  _ARM0 (m);							     \
  _ARC1 (z);							     \
  _DEFM ();							     \
  _RETM (cfunc (*m, *z));					     \
}								     \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_m_v) (constant * args) {  \
  _ARM0 (m);							     \
  _ARV1 (z);							     \
  _DEFM ();							     \
  _RETM (cfunc (*m, *z));					     \
}								     \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_mv) (constant * args) {   \
  _ARMV0 (m);							     \
  _DEFMV ();							     \
  _RETMV (cfunc (*m));						     \
}								     \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_mv_d) (constant * args) { \
  _ARMV0 (m);							     \
  _ARD1 (z);							     \
  _DEFMV ();							     \
  _RETMV (cfunc (*m, nr_complex_t (z, 0)));				     \
}								     \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_mv_c) (constant * args) { \
  _ARMV0 (m);							     \
  _ARC1 (z);							     \
  _DEFMV ();							     \
  _RETMV (cfunc (*m, *z));					     \
}								     \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_mv_v) (constant * args) { \
  _ARMV0 (m);							     \
  _ARV1 (z);							     \
  _DEFMV ();							     \
  _RETMV (cfunc (*m, *z));					     \
}								     \

MAKE_FUNC_DEFINITION_3 (stoy);
MAKE_FUNC_DEFINITION_3 (ytos);
MAKE_FUNC_DEFINITION_3 (stoz);
MAKE_FUNC_DEFINITION_3 (ztos);

// ***************** matrix conversions *****************
constant * evaluate::ytoz_m (constant * args) {
  _ARM0 (m);
  _DEFM ();
  _RETM (ytoz (*m));
}

constant * evaluate::ytoz_mv (constant * args) {
  _ARMV0 (mv);
  _DEFMV ();
  _RETMV (ytoz (*mv));
}

constant * evaluate::ztoy_m (constant * args) {
  _ARM0 (m);
  _DEFM ();
  _RETM (ztoy (*m));
}

constant * evaluate::ztoy_mv (constant * args) {
  _ARMV0 (mv);
  _DEFMV ();
  _RETMV (ztoy (*mv));
}

// These are stos() helpers.
#define _CHKM(m) /* check square matrix */              \
  if (m->getCols () != m->getRows ()) {                 \
    THROW_MATH_EXCEPTION ("stos: not a square matrix"); \
    res->m = new matrix (m->getRows (), m->getCols ()); \
    return res; }
#define _CHKMV(mv) /* check square matrix vector */                        \
  if (mv->getCols () != mv->getRows ()) {                                  \
    THROW_MATH_EXCEPTION ("stos: not a square matrix");                    \
    res->mv = new matvec (mv->getSize (), mv->getRows (), mv->getCols ()); \
    return res; }
#define _CHKMA(m,cond) \
  if (!(cond)) {                                            \
    THROW_MATH_EXCEPTION ("stos: nonconformant arguments"); \
    res->m = new matrix (m->getRows (), m->getCols ());     \
    return res; }
#define _CHKMVA(mv,cond) \
  if (!(cond)) {                                                           \
    THROW_MATH_EXCEPTION ("stos: nonconformant arguments");                \
    res->mv = new matvec (mv->getSize (), mv->getRows (), mv->getCols ()); \
    return res; }

// Function -- MATRIX stos (MATRIX, DOUBLE)
constant * evaluate::stos_m_d (constant * args) {
  _ARM0 (m); _ARD1 (zref); _DEFM ();
  _CHKM (m);
  _RETM (stos (*m, zref));
}

// Function -- MATRIX stos (MATRIX, COMPLEX)
constant * evaluate::stos_m_c (constant * args) {
  _ARM0 (m); _ARC1 (zref); _DEFM ();
  _CHKM (m);
  _RETM (stos (*m, *zref));
}

// Function -- MATRIX stos (MATRIX, DOUBLE, DOUBLE)
constant * evaluate::stos_m_d_d (constant * args) {
  _ARM0 (m); _ARD1 (zref); _ARD2 (z0); _DEFM ();
  _CHKM (m);
  _RETM (stos (*m, zref, z0));
}

// Function -- MATRIX stos (MATRIX, DOUBLE, COMPLEX)
constant * evaluate::stos_m_d_c (constant * args) {
  _ARM0 (m); _ARD1 (zref); _ARC2 (z0); _DEFM ();
  _CHKM (m);
  _RETM (stos (*m, nr_complex_t (zref, 0), *z0));
}

// Function -- MATRIX stos (MATRIX, COMPLEX, DOUBLE)
constant * evaluate::stos_m_c_d (constant * args) {
  _ARM0 (m); _ARC1 (zref); _ARD2 (z0); _DEFM ();
  _CHKM (m);
  _RETM (stos (*m, *zref, nr_complex_t (z0, 0)));
}

// Function -- MATRIX stos (MATRIX, COMPLEX, COMPLEX)
constant * evaluate::stos_m_c_c (constant * args) {
  _ARM0 (m); _ARC1 (zref); _ARC2 (z0); _DEFM ();
  _CHKM (m);
  _RETM (stos (*m, *zref, *z0));
}

// Function -- MATRIX stos (MATRIX, VECTOR)
constant * evaluate::stos_m_v (constant * args) {
  _ARM0 (m); _ARV1 (zref); _DEFM ();
  _CHKM (m); _CHKMA (m, m->getRows () == zref->getSize ());
  _RETM (stos (*m, *zref));
}

// Function -- MATRIX stos (MATRIX, VECTOR, DOUBLE)
constant * evaluate::stos_m_v_d (constant * args) {
  _ARM0 (m); _ARV1 (zref); _ARD2 (z0); _DEFM ();
  _CHKM (m); _CHKMA (m, m->getRows () == zref->getSize ());
  _RETM (stos (*m, *zref, nr_complex_t (z0, 0)));
}

// Function -- MATRIX stos (MATRIX, VECTOR, COMPLEX)
constant * evaluate::stos_m_v_c (constant * args) {
  _ARM0 (m); _ARV1 (zref); _ARC2 (z0); _DEFM ();
  _CHKM (m); _CHKMA (m, m->getRows () == zref->getSize ());
  _RETM (stos (*m, *zref, *z0));
}

// Function -- MATRIX stos (MATRIX, DOUBLE, VECTOR)
constant * evaluate::stos_m_d_v (constant * args) {
  _ARM0 (m); _ARD1 (zref); _ARV2 (z0); _DEFM ();
  _CHKM (m); _CHKMA (m, m->getRows () == z0->getSize ());
  _RETM (stos (*m, nr_complex_t (zref, 0), *z0));
}

// Function -- MATRIX stos (MATRIX, COMPLEX, VECTOR)
constant * evaluate::stos_m_c_v (constant * args) {
  _ARM0 (m); _ARC1 (zref); _ARV2 (z0); _DEFM ();
  _CHKM (m); _CHKMA (m, m->getRows () == z0->getSize ());
  _RETM (stos (*m, *zref, *z0));
}

// Function -- MATRIX stos (MATRIX, VECTOR, VECTOR)
constant * evaluate::stos_m_v_v (constant * args) {
  _ARM0 (m); _ARV1 (zref); _ARV2 (z0); _DEFM ();
  _CHKM (m); _CHKMA (m, m->getRows () == z0->getSize () &&
		     m->getRows () == zref->getSize ());
  _RETM (stos (*m, *zref, *z0));
}

// Function -- MATVEC stos (MATVEC, DOUBLE)
constant * evaluate::stos_mv_d (constant * args) {
  _ARMV0 (mv); _ARD1 (zref); _DEFMV ();
  _CHKMV (mv);
  _RETMV (stos (*mv, zref));
}

// Function -- MATVEC stos (MATVEC, COMPLEX)
constant * evaluate::stos_mv_c (constant * args) {
  _ARMV0 (mv); _ARC1 (zref); _DEFMV ();
  _CHKMV (mv);
  _RETMV (stos (*mv, *zref));
}

// Function -- MATVEC stos (MATVEC, DOUBLE, DOUBLE)
constant * evaluate::stos_mv_d_d (constant * args) {
  _ARMV0 (mv); _ARD1 (zref); _ARD2 (z0); _DEFMV ();
  _CHKMV (mv);
  _RETMV (stos (*mv, zref, z0));
}

// Function -- MATVEC stos (MATVEC, DOUBLE, COMPLEX)
constant * evaluate::stos_mv_d_c (constant * args) {
  _ARMV0 (mv); _ARD1 (zref); _ARC2 (z0); _DEFMV ();
  _CHKMV (mv);
  _RETMV (stos (*mv, nr_complex_t (zref, 0), *z0));
}

// Function -- MATVEC stos (MATVEC, COMPLEX, DOUBLE)
constant * evaluate::stos_mv_c_d (constant * args) {
  _ARMV0 (mv); _ARC1 (zref); _ARD2 (z0); _DEFMV ();
  _CHKMV (mv);
  _RETMV (stos (*mv, *zref, nr_complex_t (z0, 0)));
}

// Function -- MATVEC stos (MATVEC, COMPLEX, COMPLEX)
constant * evaluate::stos_mv_c_c (constant * args) {
  _ARMV0 (mv); _ARC1 (zref); _ARC2 (z0); _DEFMV ();
  _CHKMV (mv);
  _RETMV (stos (*mv, *zref, *z0));
}

// Function -- MATVEC stos (MATVEC, VECTOR)
constant * evaluate::stos_mv_v (constant * args) {
  _ARMV0 (mv); _ARV1 (zref); _DEFMV ();
  _CHKMV (mv); _CHKMVA (mv, mv->getRows () == zref->getSize ());
  _RETMV (stos (*mv, *zref));
}

// Function -- MATVEC stos (MATVEC, VECTOR, DOUBLE)
constant * evaluate::stos_mv_v_d (constant * args) {
  _ARMV0 (mv); _ARV1 (zref); _ARD2 (z0); _DEFMV ();
  _CHKMV (mv); _CHKMVA (mv, mv->getRows () == zref->getSize ());
  _RETMV (stos (*mv, *zref, nr_complex_t (z0, 0)));
}

// Function -- MATVEC stos (MATVEC, VECTOR, COMPLEX)
constant * evaluate::stos_mv_v_c (constant * args) {
  _ARMV0 (mv); _ARV1 (zref); _ARC2 (z0); _DEFMV ();
  _CHKMV (mv); _CHKMVA (mv, mv->getRows () == zref->getSize ());
  _RETMV (stos (*mv, *zref, *z0));
}

// Function -- MATVEC stos (MATVEC, DOUBLE, VECTOR)
constant * evaluate::stos_mv_d_v (constant * args) {
  _ARMV0 (mv); _ARD1 (zref); _ARV2 (z0); _DEFMV ();
  _CHKMV (mv); _CHKMVA (mv, mv->getRows () == z0->getSize ());
  _RETMV (stos (*mv, nr_complex_t (zref, 0), *z0));
}

// Function -- MATVEC stos (MATVEC, COMPLEX, VECTOR)
constant * evaluate::stos_mv_c_v (constant * args) {
  _ARMV0 (mv); _ARC1 (zref); _ARV2 (z0); _DEFMV ();
  _CHKMV (mv); _CHKMVA (mv, mv->getRows () == z0->getSize ());
  _RETMV (stos (*mv, *zref, *z0));
}

// Function -- MATVEC stos (MATVEC, VECTOR, VECTOR)
constant * evaluate::stos_mv_v_v (constant * args) {
  _ARMV0 (mv); _ARV1 (zref); _ARV2 (z0); _DEFMV ();
  _CHKMV (mv); _CHKMVA (mv, mv->getRows () == z0->getSize () &&
			mv->getRows () == zref->getSize ());
  _RETMV (stos (*mv, *zref, *z0));
}

constant * evaluate::twoport_m (constant * args) {
  _ARM0 (m);
  char f = CHR (_ARES(1));
  char t = CHR (_ARES(2));
  _DEFM ();
  if (m->getRows () < 2 || m->getCols () < 2) {
    THROW_MATH_EXCEPTION ("invalid matrix dimensions for twoport "
			  "transformation");
    _RETM (*m);
  }
  _RETM (twoport (*m, toupper (f), toupper (t)));
}

constant * evaluate::twoport_mv (constant * args) {
  _ARMV0 (mv);
  char f = CHR (_ARES(1));
  char t = CHR (_ARES(2));
  _DEFMV ();
  if (mv->getRows () < 2 || mv->getCols () < 2) {
    THROW_MATH_EXCEPTION ("invalid matrix dimensions for twoport "
			  "transformation");
    _RETMV (*mv);
  }
  _RETMV (twoport (*mv, toupper (f), toupper (t)));
}

constant * evaluate::inverse_m (constant * args) {
  _ARM0 (m);
  _DEFM ();
  _RETM (inverse (*m));
}

constant * evaluate::inverse_mv (constant * args) {
  _ARMV0 (mv);
  _DEFMV ();
  _RETMV (inverse (*mv));
}

constant * evaluate::transpose_m (constant * args) {
  _ARM0 (m);
  _DEFM ();
  _RETM (transpose (*m));
}

constant * evaluate::transpose_mv (constant * args) {
  _ARMV0 (mv);
  _DEFMV ();
  _RETMV (transpose (*mv));
}

constant * evaluate::det_m (constant * args) {
  _ARM0 (m);
  _DEFC ();
  _RETC (det (*m));
}

constant * evaluate::det_mv (constant * args) {
  _ARMV0 (mv);
  _DEFV ();
  _RETV (det (*mv));
}

constant * evaluate::eye_m (constant * args) {
  _ARI0 (i);
  _DEFM ();
  _RETM (eye (i));
}

constant * evaluate::adjoint_m (constant * args) {
  _ARM0 (m);
  _DEFM ();
  _RETM (adjoint (*m));
}

constant * evaluate::adjoint_mv (constant * args) {
  _ARMV0 (mv);
  _DEFMV ();
  _RETMV (adjoint (*mv));
}

// ***************** s-parameter stability factors *****************
constant * evaluate::rollet_m (constant * args) {
  _ARM0 (m);
  _DEFD ();
  _RETD (rollet (*m));
}

constant * evaluate::rollet_mv (constant * args) {
  _ARMV0 (mv);
  _DEFV ();
  _RETV (rollet (*mv));
}

constant * evaluate::mu1_m (constant * args) {
  _ARM0 (m);
  _DEFD ();
  nr_double_t k;
  k = (1 - norm (m->get (0, 0))) /
    (abs (m->get (1, 1) - conj (m->get (0, 0)) * det (*m)) +
     abs (m->get (0, 1) * m->get (1, 0)));
  _RETD (k);
}

constant * evaluate::mu1_mv (constant * args) {
  _ARMV0 (mv);
  _DEFV ();
  qucs::vector k;
  k = (1 - norm (mv->get (0, 0))) /
    (abs (mv->get (1, 1) - conj (mv->get (0, 0)) * det (*mv)) +
     abs (mv->get (0, 1) * mv->get (1, 0)));
  _RETV (k);
}

constant * evaluate::mu2_m (constant * args) {
  _ARM0 (m);
  _DEFD ();
  nr_double_t k;
  k = (1 - norm (m->get (1, 1))) /
    (abs (m->get (0, 0) - conj (m->get (1, 1)) * det (*m)) +
     abs (m->get (0, 1) * m->get (1, 0)));
  _RETD (k);
}

constant * evaluate::mu2_mv (constant * args) {
  _ARMV0 (mv);
  _DEFV ();
  qucs::vector k;
  k = (1 - norm (mv->get (1, 1))) /
    (abs (mv->get (0, 0) - conj (mv->get (1, 1)) * det (*mv)) +
     abs (mv->get (0, 1) * mv->get (1, 0)));
  _RETV (k);
}

constant * evaluate::b1_m (constant * args) {
  _ARM0 (m);
  _DEFD ();
  _RETD (b1 (*m));
}

constant * evaluate::b1_mv (constant * args) {
  _ARMV0 (mv);
  _DEFV ();
  _RETV (b1 (*mv));
}

// ***************** vector creation *****************
constant * evaluate::linspace (constant * args) {
  _ARD0 (start);
  _ARD1 (stop);
  _ARI2 (points);
  _DEFV ();
  if (points < 2) {
    THROW_MATH_EXCEPTION ("linspace: number of points must be greater than 1");
    res->v = new qucs::vector ();
    return res;
  }
  _RETV (qucs::linspace (start, stop, points));
}

constant * evaluate::logspace (constant * args) {
  _ARD0 (start);
  _ARD1 (stop);
  _ARI2 (points);
  _DEFV ();
  if (points < 2) {
    THROW_MATH_EXCEPTION ("logspace: number of points must be greater than 1");
    res->v = new qucs::vector ();
    return res;
  }
  if (start * stop <= 0) {
    THROW_MATH_EXCEPTION ("logspace: invalid start/stop values");
    res->v = new qucs::vector (points);
    return res;
  }
  _RETV (qucs::logspace (start, stop, points));
}

// Circle helper macro with a number of points given.
#define CIRCLE_HELPER_D(argi)                   \
  int n = INT (args->getResult (argi));         \
  if (n < 2) {                                  \
    THROW_MATH_EXCEPTION ("Circle: number of points must be greater than 1"); \
    _DEFV (); \
    res->v = new qucs::vector ();			\
    return res;                                 \
  }                                             \
  constant * arg = new constant (TAG_VECTOR);   \
  arg->v = new qucs::vector (qucs::linspace (0, 360, n));	\
  arg->solvee = args->getResult(0)->solvee;     \
  arg->evaluate ();                             \
  delete args->get(argi);                       \
  args->get((argi)-1)->setNext (NULL);          \
  args->append (arg);

// Circle helper macro with no additional argument given.
#define CIRCLE_HELPER_A()                        \
  constant * arg = new constant (TAG_VECTOR);    \
  arg->v = new qucs::vector (qucs::linspace (0, 360, 64));	\
  arg->solvee = args->getResult(0)->solvee;      \
  arg->evaluate ();                              \
  args->append (arg);

// ***************** s-parameter noise circles *****************
constant * evaluate::noise_circle_d_v (constant * args) {
  qucs::vector * Sopt = V (_ARES(0));
  qucs::vector * Fmin = V (_ARES(1));
  qucs::vector * Rn   = V (_ARES(2));
  nr_double_t F = D (_ARES(3));
  qucs::vector * arc  = V (_ARES(4));

  _DEFV ();
  qucs::vector N = circuit::z0 / 4 / *Rn * (F - *Fmin) * norm (1 + *Sopt);
  qucs::vector R = sqrt (N * N + N * (1 - norm (*Sopt))) / (1 + N);
  qucs::vector C = *Sopt / (1 + N);
  qucs::vector * circle = new qucs::vector (C.getSize () * arc->getSize ());
  int i, a, j; nr_complex_t v;
  for (i = 0, j = 0; i < C.getSize (); i++) {
    for (a = 0; a < arc->getSize (); a++, j++) {
      v = C.get (i) + R.get (i) * exp (nr_complex_t (0, 1) * deg2rad (arc->get (a)));
      circle->set (v, j);
    }
  }

  node * gen = SOLVEE(4)->addGeneratedEquation (arc, "Arcs");
  res->addPrepDependencies (A(gen)->result);
  res->v = circle;
  return res;
}

constant * evaluate::noise_circle_d_d (constant * args) {
  CIRCLE_HELPER_D (4);
  return noise_circle_d_v (args);
}

constant * evaluate::noise_circle_d (constant * args) {
  CIRCLE_HELPER_A ();
  return noise_circle_d_v (args);
}

constant * evaluate::noise_circle_v_v (constant * args) {
  qucs::vector * Sopt = V (_ARES(0));
  qucs::vector * Fmin = V (_ARES(1));
  qucs::vector * Rn   = V (_ARES(2));
  qucs::vector * F    = V (_ARES(3));
  :: vector * arc  = V (_ARES(4));

  _DEFV ();
  qucs::vector * circle =
      new qucs::vector (Sopt->getSize () * arc->getSize () * F->getSize ());
  int i, a, j, f; nr_complex_t v; qucs::vector N, R, C;
  for (f = 0; f < F->getSize (); f++) {
    N = circuit::z0 / 4 / *Rn * (F->get (f) - *Fmin) * norm (1 + *Sopt);
    R = sqrt (N * N + N * (1 - norm (*Sopt))) / (1 + N);
    C = *Sopt / (1 + N);
    for (i = 0; i < C.getSize (); i++) {
      for (a = 0; a < arc->getSize (); a++) {
	v = C.get (i) + R.get (i) * exp (nr_complex_t (0, 1) * deg2rad (arc->get (a)));
	j = i * F->getSize () * arc->getSize () + f * arc->getSize () + a;
	circle->set (v, j);
      }
    }
  }

  node * gen;
  gen = SOLVEE(3)->addGeneratedEquation (F, "NF");
  res->addPrepDependencies (A(gen)->result);
  gen = SOLVEE(4)->addGeneratedEquation (arc, "Arcs");
  res->addPrepDependencies (A(gen)->result);

  res->v = circle;
  return res;
}

constant * evaluate::noise_circle_v_d (constant * args) {
  CIRCLE_HELPER_D (4);
  return noise_circle_v_v (args);
}

constant * evaluate::noise_circle_v (constant * args) {
  CIRCLE_HELPER_A ();
  return noise_circle_v_v (args);
}

// ***************** s-parameter stability circles *****************
constant * evaluate::stab_circle_l_v (constant * args) {
  _ARMV0 (S);
  _ARV1 (arc);
  _DEFV ();
  qucs::vector D = norm (S->get (1, 1)) - norm (det (*S));
  qucs::vector C = (conj (S->get (1, 1)) - S->get (0, 0) * conj (det (*S))) / D;
  qucs::vector R = abs (S->get (0, 1)) * abs (S->get (1, 0)) / D;
  qucs::vector * circle = new qucs::vector (S->getSize () * arc->getSize ());
  int a, d, i; nr_complex_t v;
  for (i = 0, d = 0; i < S->getSize (); i++) {
    for (a = 0; a < arc->getSize (); a++, d++) {
      v = C.get (i) + R.get (i) * exp (nr_complex_t (0, 1) * deg2rad (arc->get (a)));
      circle->set (v, d);
    }
  }
  node * gen = SOLVEE(1)->addGeneratedEquation (arc, "Arcs");
  res->addPrepDependencies (A(gen)->result);
  res->v = circle;
  return res;
}

constant * evaluate::stab_circle_l_d (constant * args) {
  CIRCLE_HELPER_D (1);
  return stab_circle_l_v (args);
}

constant * evaluate::stab_circle_l (constant * args) {
  CIRCLE_HELPER_A ();
  return stab_circle_l_v (args);
}

constant * evaluate::stab_circle_s_v (constant * args) {
  _ARMV0 (S);
  _ARV1 (arc);
  _DEFV ();
  qucs::vector D = norm (S->get (0, 0)) - norm (det (*S));
  qucs::vector C = (conj (S->get (0, 0)) - S->get (1, 1) * conj (det (*S))) / D;
  qucs::vector R = abs (S->get (0, 1)) * abs (S->get (1, 0)) / D;
  qucs::vector * circle = new qucs::vector (S->getSize () * arc->getSize ());
  int a, d, i; nr_complex_t v;
  for (i = 0, d = 0; i < S->getSize (); i++) {
    for (a = 0; a < arc->getSize (); a++, d++) {
      v = C.get (i) + R.get (i) * exp (nr_complex_t (0, 1) * deg2rad (arc->get (a)));
      circle->set (v, d);
    }
  }
  node * gen = SOLVEE(1)->addGeneratedEquation (arc, "Arcs");
  res->addPrepDependencies (A(gen)->result);
  res->v = circle;
  return res;
}

constant * evaluate::stab_circle_s_d (constant * args) {
  CIRCLE_HELPER_D (1);
  return stab_circle_s_v (args);
}

constant * evaluate::stab_circle_s (constant * args) {
  CIRCLE_HELPER_A ();
  return stab_circle_s_v (args);
}

// ***************** s-parameter gain circles *****************
constant * evaluate::ga_circle_d_v (constant * args) {
  _ARMV0 (S);
  _ARD1 (G);
  _ARV2 (arc);
  _DEFV ();
  qucs::vector g, D, c, s, k, C, R, d;
  D = det (*S);
  c = S->get (0, 0) - conj (S->get (1, 1)) * D;
  k = rollet (*S);
  s = S->get (0, 1) * S->get (1, 0);
  g = G / norm (S->get (1, 0));
  d = 1 + g * (norm (S->get (0, 0)) - norm (D));
  C = g * conj (c) / d;
  R = sqrt (1 - 2 * k * g * abs (s) + g * g * norm (s)) / abs (d);

  qucs::vector * circle = new qucs::vector (S->getSize () * arc->getSize ());
  int i, a, j; nr_complex_t v;
  for (i = 0, j = 0; i < C.getSize (); i++) {
    for (a = 0; a < arc->getSize (); a++, j++) {
      v = C.get (i) + R.get (i) * exp (nr_complex_t (0, 1) * deg2rad (arc->get (a)));
      circle->set (v, j);
    }
  }

  node * gen = SOLVEE(2)->addGeneratedEquation (arc, "Arcs");
  res->addPrepDependencies (A(gen)->result);
  res->v = circle;
  return res;
}

constant * evaluate::ga_circle_d_d (constant * args) {
  CIRCLE_HELPER_D (2);
  return ga_circle_d_v (args);
}

constant * evaluate::ga_circle_d (constant * args) {
  CIRCLE_HELPER_A ();
  return ga_circle_d_v (args);
}

constant * evaluate::ga_circle_v_v (constant * args) {
  _ARMV0 (S);
  _ARV1 (G);
  _ARV2 (arc);
  _DEFV ();
  qucs::vector * circle =
    new qucs::vector (S->getSize () * arc->getSize () * G->getSize ());
  int i, a, j, f; nr_complex_t v; qucs::vector g, D, c, s, k, R, C, d;
  D = det (*S);
  c = S->get (0, 0) - conj (S->get (1, 1)) * D;
  k = rollet (*S);
  s = S->get (0, 1) * S->get (1, 0);
  for (f = 0; f < G->getSize (); f++) {
    g = G->get (f) / norm (S->get (1, 0));
    d = 1 + g * (norm (S->get (0, 0)) - norm (D));
    C = g * conj (c) / d;
    R = sqrt (1 - 2 * k * g * abs (s) + g * g * norm (s)) / abs (d);
    for (i = 0; i < C.getSize (); i++) {
      for (a = 0; a < arc->getSize (); a++) {
        v = C.get (i) + R.get (i) * exp (nr_complex_t (0, 1) * deg2rad (arc->get (a)));
	j = i * G->getSize () * arc->getSize () + f * arc->getSize () + a;
	circle->set (v, j);
      }
    }
  }

  node * gen;
  gen = SOLVEE(1)->addGeneratedEquation (G, "Ga");
  res->addPrepDependencies (A(gen)->result);
  gen = SOLVEE(2)->addGeneratedEquation (arc, "Arcs");
  res->addPrepDependencies (A(gen)->result);
  res->v = circle;
  return res;
}

constant * evaluate::ga_circle_v_d (constant * args) {
  CIRCLE_HELPER_D (2);
  return ga_circle_v_v (args);
}

constant * evaluate::ga_circle_v (constant * args) {
  CIRCLE_HELPER_A ();
  return ga_circle_v_v (args);
}

constant * evaluate::gp_circle_d_v (constant * args) {
  _ARMV0 (S);
  _ARD1 (G);
  _ARV2 (arc);
  _DEFV ();
  qucs::vector g, D, c, s, k, C, R, d;
  D = det (*S);
  c = S->get (1, 1) - conj (S->get (0, 0)) * D;
  k = rollet (*S);
  s = S->get (0, 1) * S->get (1, 0);
  g = G / norm (S->get (1, 0));
  d = 1 + g * (norm (S->get (1, 1)) - norm (D));
  C = g * conj (c) / d;
  R = sqrt (1 - 2 * k * g * abs (s) + g * g * norm (s)) / abs (d);

  qucs::vector * circle = new qucs::vector (S->getSize () * arc->getSize ());
  int i, a, j; nr_complex_t v;
  for (i = 0, j = 0; i < C.getSize (); i++) {
    for (a = 0; a < arc->getSize (); a++, j++) {
      v = C.get (i) + R.get (i) * exp (nr_complex_t (0, 1) * deg2rad (arc->get (a)));
      circle->set (v, j);
    }
  }

  node * gen = SOLVEE(2)->addGeneratedEquation (arc, "Arcs");
  res->addPrepDependencies (A(gen)->result);
  res->v = circle;
  return res;
}

constant * evaluate::gp_circle_d_d (constant * args) {
  CIRCLE_HELPER_D (2);
  return gp_circle_d_v (args);
}

constant * evaluate::gp_circle_d (constant * args) {
  CIRCLE_HELPER_A ();
  return gp_circle_d_v (args);
}

constant * evaluate::gp_circle_v_v (constant * args) {
  _ARMV0 (S);
  _ARV1 (G);
  _ARV2 (arc);
  _DEFV ();
  qucs::vector * circle =
      new qucs::vector (S->getSize () * arc->getSize () * G->getSize ());
  int i, a, j, f; nr_complex_t v; qucs::vector g, D, c, s, k, R, C, d;
  D = det (*S);
  c = S->get (1, 1) - conj (S->get (0, 0)) * D;
  k = rollet (*S);
  s = S->get (0, 1) * S->get (1, 0);
  for (f = 0; f < G->getSize (); f++) {
    g = G->get (f) / norm (S->get (1, 0));
    d = 1 + g * (norm (S->get (1, 1)) - norm (D));
    C = g * conj (c) / d;
    R = sqrt (1 - 2 * k * g * abs (s) + g * g * norm (s)) / abs (d);
    for (i = 0; i < C.getSize (); i++) {
      for (a = 0; a < arc->getSize (); a++) {
        v = C.get (i) + R.get (i) * exp (nr_complex_t (0, 1) * deg2rad (arc->get (a)));
	j = i * G->getSize () * arc->getSize () + f * arc->getSize () + a;
	circle->set (v, j);
      }
    }
  }

  node * gen;
  gen = SOLVEE(1)->addGeneratedEquation (G, "Gp");
  res->addPrepDependencies (A(gen)->result);
  gen = SOLVEE(2)->addGeneratedEquation (arc, "Arcs");
  res->addPrepDependencies (A(gen)->result);
  res->v = circle;
  return res;
}

constant * evaluate::gp_circle_v_d (constant * args) {
  CIRCLE_HELPER_D (2);
  return gp_circle_v_v (args);
}

constant * evaluate::gp_circle_v (constant * args) {
  CIRCLE_HELPER_A ();
  return gp_circle_v_v (args);
}

// ***************** versus vectors *****************
constant * evaluate::plot_vs_v (constant * args) {
  _ARV0 (v);
  _DEFV ();
  int i = 1;
  for (node * arg = args->getNext (); arg != NULL; arg = arg->getNext ()) {
    node * gen = arg->solvee->addGeneratedEquation (V (_ARES (i)), "Versus");
    res->addPrepDependencies (A(gen)->result);
    i++;
  }
  res->dropdeps = 1;
  _RETV (*v);
}

constant * evaluate::plot_vs_mv (constant * args) {
  _ARMV0 (mv);
  _DEFMV ();
  int i = 1;
  for (node * arg = args->getNext (); arg != NULL; arg = arg->getNext ()) {
    node * gen = arg->solvee->addGeneratedEquation (V (_ARES (i)), "Versus");
    res->addPrepDependencies (A(gen)->result);
    i++;
  }
  res->dropdeps = 1;
  _RETMV (*mv);
}

// ***************** find x-value for given y-value *****************
constant * evaluate::xvalue_d (constant * args) {
  _ARV0 (v);
  _ARD1 (d);
  _DEFC ();
  strlist * deps = _ARG(0)->collectDataDependencies ();
  if (!deps || deps->length () != 1) {
    THROW_MATH_EXCEPTION ("not an appropriate dependent data vector");
    _RETC (0.0);
  }
  qucs::vector * indep = SOLVEE(0)->getDataVector (deps->get (0));
  int idx, i;
  nr_double_t t, diff = std::numeric_limits<nr_double_t>::max();
  for (idx = i = 0; i < v->getSize (); i++) {
    t = abs (v->get (i) - d);
    if (t < diff) {
      idx = i;
      diff = t;
    }
  }
  _RETC (indep->get (idx));
}

constant * evaluate::xvalue_c (constant * args) {
  _ARV0 (v);
  _ARC1 (c);
  _DEFC ();
  strlist * deps = _ARG(0)->collectDataDependencies ();
  if (!deps || deps->length () != 1) {
    THROW_MATH_EXCEPTION ("not an appropriate dependent data vector");
    _RETC (0.0);
  }
  qucs::vector * indep = SOLVEE(0)->getDataVector (deps->get (0));
  int idx, i;
  nr_double_t t, diff = std::numeric_limits<nr_double_t>::max();
  for (idx = i = 0; i < v->getSize (); i++) {
    t = abs (v->get (i) - *c);
    if (t < diff) {
      idx = i;
      diff = t;
    }
  }
  _RETC (indep->get (idx));
}

// ***************** find y-value for given x-value *****************
constant * evaluate::yvalue_d (constant * args) {
  _ARV0 (v);
  _ARD1 (d);
  _DEFC ();
  strlist * deps = _ARG(0)->collectDataDependencies ();
  if (!deps || deps->length () != 1) {
    THROW_MATH_EXCEPTION ("not an appropriate dependent data vector");
    _RETC (0.0);
  }
  qucs::vector * indep = SOLVEE(0)->getDataVector (deps->get (0));
  int idx, i;
  nr_double_t t, diff = std::numeric_limits<nr_double_t>::max();
  for (idx = i = 0; i < indep->getSize (); i++) {
    t = abs (indep->get (i) - d);
    if (t < diff) {
      idx = i;
      diff = t;
    }
  }
  _RETC (v->get (idx));
}

constant * evaluate::yvalue_c (constant * args) {
  _ARV0 (v);
  _ARC1 (c);
  _DEFC ();
  strlist * deps = _ARG(0)->collectDataDependencies ();
  if (!deps || deps->length () != 1) {
    THROW_MATH_EXCEPTION ("not an appropriate dependent data vector");
    _RETC (0.0);
  }
  qucs::vector * indep = SOLVEE(0)->getDataVector (deps->get (0));
  int idx, i;
  nr_double_t t, diff = std::numeric_limits<nr_double_t>::max();
  for (idx = i = 0; i < indep->getSize (); i++) {
    t = abs (indep->get (i) - *c);
    if (t < diff) {
      idx = i;
      diff = t;
    }
  }
  _RETC (v->get (idx));
}

// ***************** min, max, avg using range specification *****************
constant * evaluate::max_r (constant * args) {
  _ARV0 (v);
  _ARR1 (r);
  _DEFD ();
  strlist * deps = _ARG(0)->collectDataDependencies ();
  if (!deps || deps->length () != 1) {
    THROW_MATH_EXCEPTION ("not an appropriate dependent data vector");
    _RETD (0.0);
  }
  qucs::vector * indep = SOLVEE(0)->getDataVector (deps->get (0));
  nr_complex_t c;
  nr_double_t d, M = -std::numeric_limits<nr_double_t>::max();
  for (int i = 0; i < indep->getSize (); i++) {
    if (r->inside (real (indep->get (i)))) {
      c = v->get (i);
      d = fabs (arg (c)) < pi_over_2 ? abs (c) : -abs (c);
      if (d > M) M = d;
    }
  }
  _RETD (M);
}

constant * evaluate::min_r (constant * args) {
  _ARV0 (v);
  _ARR1 (r);
  _DEFD ();
  strlist * deps = _ARG(0)->collectDataDependencies ();
  if (!deps || deps->length () != 1) {
    THROW_MATH_EXCEPTION ("not an appropriate dependent data vector");
    _RETD (0.0);
  }
  qucs::vector * indep = SOLVEE(0)->getDataVector (deps->get (0));
  nr_complex_t c;
  nr_double_t d, M = +std::numeric_limits<nr_double_t>::max();
  for (int i = 0; i < indep->getSize (); i++) {
    if (r->inside (real (indep->get (i)))) {
      c = v->get (i);
      d = fabs (arg (c)) < pi_over_2 ? abs (c) : -abs (c);
      if (d < M) M = d;
    }
  }
  _RETD (M);
}

constant * evaluate::avg_r (constant * args) {
  _ARV0 (v);
  _ARR1 (r);
  _DEFC ();
  strlist * deps = _ARG(0)->collectDataDependencies ();
  if (!deps || deps->length () != 1) {
    THROW_MATH_EXCEPTION ("not an appropriate dependent data vector");
    _RETC (0.0);
  }
  qucs::vector * indep = SOLVEE(0)->getDataVector (deps->get (0));
  nr_complex_t c = 0.0;
  int i, k;
  for (k = i = 0; i < indep->getSize (); i++) {
    if (r->inside (real (indep->get (i)))) {
      c += v->get (i);
      k++;
    }
  }
  _RETC (c / (nr_double_t) k);
}

// *************** range functionality *****************
constant * evaluate::range_d_d (constant * args) {
  _ARD0 (d1);
  _ARD1 (d2);
  _DEFR ();
  _RETR (new range ('[', d1, d2, ']'));
}

constant * evaluate::range_c_d (constant * args) {
  _ARD1 (d2);
  _DEFR ();
  _RETR (new range ('.', d2 - 1, d2, ']'));
}

constant * evaluate::range_d_c (constant * args) {
  _ARD0 (d1);
  _DEFR ();
  _RETR (new range ('[', d1, d1 + 1, '.'));
}

constant * evaluate::range_c_c (constant *) {
  _DEFR ();
  _RETR (new range ('.', 0, 0, '.'));
}

MAKE_FUNC_DEFINITION_0_STD (ceil);        // ceil double->integer conversion
MAKE_FUNC_DEFINITION_0_STD (floor);       // floor double->integer conversion
MAKE_FUNC_DEFINITION_0_QUCS (fix);        // fix double->integer conversion
MAKE_FUNC_DEFINITION_0_QUCS (step);       // step function
MAKE_FUNC_DEFINITION_0_QUCS (round);      // round function
//#ifndef HAVE_ERF
MAKE_FUNC_DEFINITION_0_SPECIAL (erf);     // error function
//#else
//MAKE_FUNC_DEFINITION_0_STD (erf);     // error function
//#endif
//#ifndef HAVE_ERFC
MAKE_FUNC_DEFINITION_0_SPECIAL (erfc);    // complementary error function
//#else
//MAKE_FUNC_DEFINITION_0_STD (erfc);    // complementary error function
//#endif
MAKE_FUNC_DEFINITION_0_SPECIAL (erfinv);  // inverse of error function
MAKE_FUNC_DEFINITION_0_SPECIAL (erfcinv); // inverse of complementary error function
MAKE_FUNC_DEFINITION_0_SPECIAL (i0);      // modified bessel zero order

// ******************* cumulative sum *********************
constant * evaluate::cumsum_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (d1);
}

constant * evaluate::cumsum_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (*c1);
}

constant * evaluate::cumsum_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (cumsum (*v1));
}

// **************** cumulative average ******************
constant * evaluate::cumavg_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (d1);
}

constant * evaluate::cumavg_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (*c1);
}

constant * evaluate::cumavg_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (cumavg (*v1));
}

// ******************* cumulative product *********************
constant * evaluate::cumprod_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (d1);
}

constant * evaluate::cumprod_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (*c1);
}

constant * evaluate::cumprod_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (cumprod (*v1));
}

// ************** smoothing ****************
constant * evaluate::smooth_d_d (constant * args) {
  _ARD0 (y);
  _ARD1 (a);
  _DEFD ();
  if (a < 0 || a > 100) {
    THROW_MATH_EXCEPTION ("smooth: aperture percentage a must be "
			  "between 0 and 100");
  }
  _RETD (y);
}

constant * evaluate::smooth_c_d (constant * args) {
  _ARC0 (y);
  _ARD1 (a);
  _DEFC ();
  if (a < 0 || a > 100) {
    THROW_MATH_EXCEPTION ("smooth: aperture percentage a must be "
			  "between 0 and 100");
  }
  _RETC (*y);
}

constant * evaluate::smooth_v_d (constant * args) {
  _ARV0 (y);
  _ARD1 (a);
  _DEFV ();
  if (a < 0 || a > 100) {
    THROW_MATH_EXCEPTION ("smooth: aperture percentage a must be "
			  "between 0 and 100");
    __RETV ();
  }
  _RETV (smooth (*y, a));
}

// Calculates the delay group between two ports specified by the user
constant * evaluate::groupdelay_mv_i_i (constant * args) {
  _ARMV0 (S);
  int index1 = D (_ARES(1))-1;//Input port
  int index2 = D (_ARES(2))-1;//Output port
  _DEFV ();
  
  strlist * deps = _ARG(0)->collectDataDependencies ();  
  if (!deps || deps->length () != 1) {//Check there's an unique dependency
    THROW_MATH_EXCEPTION ("Not an appropriate dependent data vector");
    _RETC (0.0);
  }
  qucs::vector * freq = SOLVEE(0)->getDataVector (deps->get (0));

  //Check if the length of S matvec and the freq vector are the same.
  if (freq->getSize() != S->getSize()) {
    THROW_MATH_EXCEPTION ("The S matrix and the frequency vector must have the same length");
    __RETV ();
  }
  //Check if the input matrix is square
  if (S->getRows() != S->getCols())
  {
    THROW_MATH_EXCEPTION ("The S matrix must be square");
    __RETV ();
  }

  //Check if the port numbers are valid
  if ((index1 < 0) || (index1 > S->getRows()-1))
  {
    THROW_MATH_EXCEPTION ("The 3rd argument must be a valid port number");
    __RETV ();
  }
  if ((index2 < 0) || (index2 > S->getRows()-1))
  {
    THROW_MATH_EXCEPTION ("The 4th argument must be a valid port number");
    __RETV ();
  }

  qucs::vector svec = S->get (index1, index2);
  _RETV (groupdelay(svec, *freq));
}


constant * evaluate::groupdelay_mv (constant * args) {
  _ARMV0 (S);
  _DEFV ();
  qucs::vector svec = S->get (1, 0);
  
  strlist * deps = _ARG(0)->collectDataDependencies ();  
  if (!deps || deps->length () != 1) {//Check there's an unique dependency
    THROW_MATH_EXCEPTION ("Not an appropriate dependent data vector");
    _RETC (0.0);
  }

  qucs::vector * freq = SOLVEE(0)->getDataVector (deps->get (0));
  _RETV (groupdelay(svec, *freq));
}

constant * evaluate::groupdelay_v (constant * args) {
  _ARV0 (svec);
  _DEFV ();
  
  strlist * deps = _ARG(0)->collectDataDependencies ();  
  if (!deps || deps->length () != 1) {//Check there's an unique dependency
    THROW_MATH_EXCEPTION ("Not an appropriate dependent data vector");
    _RETC (0.0);
  }
  qucs::vector * freq = SOLVEE(0)->getDataVector (deps->get (0));
  _RETV (groupdelay(*svec, *freq));
}

// ******************* rms *********************
constant * evaluate::rms_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (fabs (d1));
}

constant * evaluate::rms_c (constant * args) {
  _ARC0 (c1);
  _DEFD ();
  _RETD (abs (*c1));
}

constant * evaluate::rms_v (constant * args) {
  _ARV0 (v1);
  _DEFD ();
  _RETD (v1->rms ());
}

// ******************* variance *********************
constant * evaluate::variance_d (constant *) {
  _DEFD ();
  _RETD (0);
}

constant * evaluate::variance_c (constant *) {
  _DEFD ();
  _RETD (0);
}

constant * evaluate::variance_v (constant * args) {
  _ARV0 (v1);
  _DEFD ();
  _RETD (v1->variance ());
}

// ******************* stddev *********************
constant * evaluate::stddev_d (constant *) {
  _DEFD ();
  _RETD (0);
}

constant * evaluate::stddev_c (constant *) {
  _DEFD ();
  _RETD (0);
}

constant * evaluate::stddev_v (constant * args) {
  _ARV0 (v1);
  _DEFD ();
  _RETD (v1->stddev ());
}

// ********* jn (bessel function of 1st kind, nth order) ***********
constant * evaluate::jn_d_d (constant * args) {
  _ARI0 (n);
  _ARD1 (x);
  _DEFD ();
  _RETD (::jn (n, x));
}

constant * evaluate::jn_d_c (constant * args) {
  _ARI0 (n);
  _ARC1 (x);
  _DEFC ();
  _RETC (jn (n, *x));
}

constant * evaluate::jn_d_v (constant * args) {
  _ARI0 (n);
  _ARV1 (x);
  _DEFV ();
  _RETV (jn (n, *x));
}

// ********* yn (bessel function of 2nd kind, nth order) ***********
constant * evaluate::yn_d_d (constant * args) {
  _ARI0 (n);
  _ARD1 (x);
  _DEFD ();
  _RETD (::yn (n, x));
}

constant * evaluate::yn_d_c (constant * args) {
  _ARI0 (n);
  _ARC1 (x);
  _DEFC ();
  _RETC (yn (n, *x));
}

constant * evaluate::yn_d_v (constant * args) {
  _ARI0 (n);
  _ARV1 (x);
  _DEFV ();
  _RETV (yn (n, *x));
}

// ***************** sqr *****************
constant * evaluate::sqr_m (constant * args) {
  _ARM0 (m1);
  _DEFM ();
  _RETM (qucs::sqr (*m1));
}

constant * evaluate::sqr_mv (constant * args) {
  _ARMV0 (m1);
  _DEFMV ();
  _RETMV (qucs::sqr (*m1));
}

// ******************* polar *********************
constant * evaluate::polar_d_d (constant * args) {
  _ARD0 (a);
  _ARD1 (p);
  _DEFC ();
  _RETC (std::polar (a, deg2rad (p)));
}

constant * evaluate::polar_c_d (constant * args) {
  _ARC0 (a);
  _ARD1 (p);
  _DEFC ();
  _RETC (qucs::polar (*a, nr_complex_t (deg2rad (p), 0)));
}

constant * evaluate::polar_d_c (constant * args) {
  _ARD0 (a);
  _ARC1 (p);
  _DEFC ();
  /// FIXME is this right?
  _RETC (qucs::polar (nr_complex_t (a, 0), nr_complex_t (deg2rad (*p),0)));
}

constant * evaluate::polar_c_c (constant * args) {
  _ARC0 (a);
  _ARC1 (p);
  _DEFC ();
  /// FIXME is this right?
  _RETC (qucs::polar (*a, nr_complex_t (deg2rad (*p), 0)));
}

constant * evaluate::polar_d_v (constant * args) {
  _ARD0 (a);
  _ARV1 (v);
  _DEFV ();
  _RETV (polar (nr_complex_t (a, 0), deg2rad (*v)));
}

constant * evaluate::polar_c_v (constant * args) {
  _ARC0 (a);
  _ARV1 (v);
  _DEFV ();
  _RETV (polar (*a, deg2rad (*v)));
}

constant * evaluate::polar_v_d (constant * args) {
  _ARV0 (v);
  _ARD1 (p);
  _DEFV ();
  _RETV (polar (*v, nr_complex_t (deg2rad (p), 0)));
}

constant * evaluate::polar_v_c (constant * args) {
  _ARV0 (v);
  _ARC1 (p);
  _DEFV ();
  _RETV (polar (*v, nr_complex_t (deg2rad (*p),0)));
}

constant * evaluate::polar_v_v (constant * args) {
  _ARV0 (a);
  _ARV1 (p);
  _DEFV ();
  _RETV (polar (*a, deg2rad (*p)));
}

// ******************* arctan2 *********************
constant * evaluate::arctan2_d_d (constant * args) {
  _ARD0 (y);
  _ARD1 (x);
  _DEFD ();
  if ((x == 0) && (y == 0)) {
    THROW_MATH_EXCEPTION ("arctan2: not defined for (0,0)");
    _RETD (-pi / 2);
  }
  _RETD (std::atan2 (y, x));
}

constant * evaluate::arctan2_d_v (constant * args) {
  _ARD0 (y);
  _ARV1 (x);
  _DEFV ();
  _RETV (atan2 (y, *x));
}

constant * evaluate::arctan2_v_d (constant * args) {
  _ARV0 (y);
  _ARD1 (x);
  _DEFV ();
  _RETV (atan2 (*y, x));
}

constant * evaluate::arctan2_v_v (constant * args) {
  _ARV0 (y);
  _ARV1 (x);
  _DEFV ();
  _RETV (atan2 (*y, *x));
}

// ******************* dbm2w *********************
constant * evaluate::dbm2w_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (0.001 * std::pow (10.0, d1 / 10.0));
}

constant * evaluate::dbm2w_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (0.001 * pow (10.0, *c1 / 10.0));
}

constant * evaluate::dbm2w_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (dbm2w (*v1));
}

// ******************* w2dbm *********************
constant * evaluate::w2dbm_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (10.0 * std::log10 (d1 / 0.001));
}

constant * evaluate::w2dbm_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (10.0 * log10 (*c1 / 0.001));
}

constant * evaluate::w2dbm_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (w2dbm (*v1));
}

// ********** integrate *****************
constant * evaluate::integrate_d_d (constant * args) {
  _ARD0 (data);
  _ARD1 (incr);
  _DEFD ();
  _RETD (data * incr);
}

constant * evaluate::integrate_c_c (constant * args) {
  _ARC0 (data);
  _ARC1 (incr);
  _DEFC ();
  _RETC (*data * *incr);
}

constant * evaluate::integrate_v_d (constant * args) {
  _ARV0 (data);
  _ARD1 (incr);
  _DEFD ();
  _RETD (integrate (*data, incr));
}

constant * evaluate::integrate_v_c (constant * args) {
  _ARV0 (data);
  _ARC1 (incr);
  _DEFC ();
  _RETC (integrate (*data, *incr));
}

// ******************* dbm *********************
constant * evaluate::dbm_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (10.0 * std::log10 (norm (d1) / circuit::z0 / 0.001));
}

constant * evaluate::dbm_d_d (constant * args) {
  _ARD0 (d1);
  _ARD1 (z);
  _DEFD ();
  _RETD (10.0 * std::log10 (norm (d1) / z / 0.001));
}

constant * evaluate::dbm_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (10.0 * log10 (norm (*c1) / circuit::z0 / 0.001));
}

constant * evaluate::dbm_c_d (constant * args) {
  _ARC0 (c1);
  _ARD1 (z);
  _DEFC ();
  _RETC (10.0 * log10 (norm (*c1) / z / 0.001));
}

constant * evaluate::dbm_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (dbm (*v1));
}

constant * evaluate::dbm_v_d (constant * args) {
  _ARV0 (v1);
  _ARD1 (z);
  _DEFV ();
  _RETV (dbm (*v1, z));
}

constant * evaluate::dbm_d_c (constant * args) {
  _ARD0 (d1);
  _ARC1 (z);
  _DEFC ();
  _RETC (10.0 * log10 (norm (d1) / conj (*z) / 0.001));
}

constant * evaluate::dbm_c_c (constant * args) {
  _ARC0 (c1);
  _ARC1 (z);
  _DEFC ();
  _RETC (10.0 * log10 (norm (*c1) / conj (*z) / 0.001));
}

constant * evaluate::dbm_v_c (constant * args) {
  _ARV0 (v1);
  _ARC1 (z);
  _DEFV ();
  _RETV (dbm (*v1, *z));
}

// ************** running average ****************
constant * evaluate::runavg_d_d (constant * args) {
  _ARD0 (x);
  _ARI1 (n);
  _DEFV ();
  if (n < 1) {
    THROW_MATH_EXCEPTION ("runavg: number n to be averaged over must be "
			  "larger or equal 1");
    __RETV ();
  }
  _RETV (runavg (nr_complex_t (x, 0), n));
}

constant * evaluate::runavg_c_d (constant * args) {
  _ARC0 (x);
  _ARI1 (n);
  _DEFV ();
  if (n < 1) {
    THROW_MATH_EXCEPTION ("runavg: number n to be averaged over must be "
			  "larger or equal 1");
    __RETV ();
  }
  _RETV (runavg (*x, n));
}

constant * evaluate::runavg_v_d (constant * args) {
  _ARV0 (x);
  _ARI1 (n);
  _DEFV ();
  if (n < 1 || n > x->getSize ()) {
    THROW_MATH_EXCEPTION ("runavg: number n to be averaged over must be "
			  "larger or equal 1 and less or equal than the "
			  "number of vector elements");
    __RETV ();
  }
  _RETV (runavg (*x, n));
}

// ********************* thermal voltage **********************
constant * evaluate::vt_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (d1 * kBoverQ);
}

constant * evaluate::vt_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (*c1 * kBoverQ);
}

constant * evaluate::vt_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v1->getSize (); i++) v->add (v1->get (i) * kBoverQ);
  res->v = v;
  return res;
}

// ************** Kaiser-Bessel derived window ****************
constant * evaluate::kbd_d_d (constant * args) {
  _ARD0 (alpha);
  _ARI1 (size);
  _DEFV ();
  int i;
  nr_double_t sval = 0.0;
  if (size <= 0) {
    THROW_MATH_EXCEPTION ("kbd: vector length must be greater than zero");
    __RETV ();
  }
  qucs::vector v (size);
  for (i = 0; i < size / 2; i++) {
    sval += fspecial::i0 (pi * alpha * std::sqrt (1.0 - sqr (4.0 * i / size - 1.0)));
    v (i) = sval;
  }
  // need to add one more value to the normalization factor at size/2
  sval += fspecial::i0 (pi * alpha * std::sqrt (1.0 - sqr (4.0 * (size / 2) / size - 1.0)));
  // normalize the window and fill in the righthand side of the window
  for (i = 0; i < size / 2; i++) {
    v (i) = std::sqrt (v (i) / sval);
    v (size - 1 - i) = v (i);
  }
  _RETV (v);
}

constant * evaluate::kbd_d (constant * args) {
  constant * arg = new constant (TAG_DOUBLE);
  arg->d = 64;
  arg->solvee = args->getResult(0)->solvee;
  arg->evaluate ();
  args->append (arg);
  return kbd_d_d (args);
}

// ***************** if-then-else operation ****************
constant * evaluate::ifthenelse_d_d (constant * args) {
  _ARB0 (cond);
  _ARD1 (d1);
  _ARD2 (d2);
  _DEFD ();
  _RETD (cond ? d1 : d2);
}

constant * evaluate::ifthenelse_b_b (constant * args) {
  _ARB0 (cond);
  _ARB1 (b1);
  _ARB2 (b2);
  _DEFB ();
  _RETB (cond ? b1 : b2);
}

constant * evaluate::ifthenelse_d_b (constant * args) {
  _ARB0 (cond);
  _ARD1 (d1);
  _ARB2 (b2);
  _DEFD ();
  _RETD (cond ? d1 : (b2 ? 1.0 : 0.0));
}

constant * evaluate::ifthenelse_b_d (constant * args) {
  _ARB0 (cond);
  _ARB1 (b1);
  _ARD2 (d2);
  _DEFD ();
  _RETD (cond ? (b1 ? 1.0 : 0.0) : d2);
}

constant * evaluate::ifthenelse_c_c (constant * args) {
  _ARB0 (cond);
  int t1 = _ARG(1)->getType ();
  int t2 = _ARG(2)->getType ();
  nr_complex_t c1, c2;
  if (t1 == TAG_DOUBLE)
    c1 = D(_ARES(1));
  else if (t1 == TAG_COMPLEX)
    c1 = *C(_ARES(1));
  else
    c1 = B(_ARES(1)) ? 1.0 : 0.0;
  if (t2 == TAG_DOUBLE)
    c2 = D(_ARES(2));
  else if (t2 == TAG_COMPLEX)
    c2 = *C(_ARES(2));
  else
    c2 = B(_ARES(2)) ? 1.0 : 0.0;
  _DEFC ();
  _RETC (cond ? c1 : c2);
}

constant * evaluate::ifthenelse_m_m (constant * args) {
  _ARB0 (cond);
  int t1 = _ARG(1)->getType ();
  int t2 = _ARG(2)->getType ();
  matrix m1, m2;
  switch (t1) {
  case TAG_DOUBLE:
    m1 = matrix (1); m1 (1,1) = D(_ARES(1)); break;
  case TAG_COMPLEX:
    m1 = matrix (1); m1 (1,1) = *C(_ARES(1)); break;
  case TAG_BOOLEAN:
    m1 = matrix (1); m1 (1,1) = B(_ARES(1)) ? 1.0 : 0.0; break;
  case TAG_MATRIX:
    m1 = *M(_ARES(1)); break;
  }
  switch (t2) {
  case TAG_DOUBLE:
    m2 = matrix (1); m2 (0,0) = D(_ARES(2)); break;
  case TAG_COMPLEX:
    m2 = matrix (1); m2 (0,0) = *C(_ARES(2)); break;
  case TAG_BOOLEAN:
    m2 = matrix (1); m2 (0,0) = B(_ARES(2)) ? 1.0 : 0.0; break;
  case TAG_MATRIX:
    m2 = *M(_ARES(2)); break;
  }
  _DEFM ();
  _RETM (cond ? m1 : m2);
}

constant * evaluate::ifthenelse_v_v (constant * args) {
  _ARB0 (cond);
  int t1 = _ARG(1)->getType ();
  int t2 = _ARG(2)->getType ();
  qucs::vector v1, v2;
  switch (t1) {
  case TAG_DOUBLE:
    v1 = qucs::vector (1); v1 (0) = D(_ARES(1)); break;
  case TAG_COMPLEX:
    v1 = qucs::vector (1); v1 (0) = *C(_ARES(1)); break;
  case TAG_BOOLEAN:
    v1 = qucs::vector (1); v1 (0) = B(_ARES(1)) ? 1.0 : 0.0; break;
  case TAG_VECTOR:
    v1 = *V(_ARES(1)); break;
  }
  switch (t2) {
  case TAG_DOUBLE:
    v2 = qucs::vector (1); v2 (0) = D(_ARES(2)); break;
  case TAG_COMPLEX:
    v2 = qucs::vector (1); v2 (0) = *C(_ARES(2)); break;
  case TAG_BOOLEAN:
    v2 = qucs::vector (1); v2 (0) = B(_ARES(2)) ? 1.0 : 0.0; break;
  case TAG_VECTOR:
    v2 = *V(_ARES(2)); break;
  }
  _DEFV ();
  _RETV (cond ? v1 : v2);
}

constant * evaluate::ifthenelse_v_v_v (constant * args) {
  _ARV0 (cond);
  int t1 = _ARG(1)->getType ();
  int t2 = _ARG(2)->getType ();
  qucs::vector v1, v2;
  switch (t1) {
  case TAG_DOUBLE:
    v1 = qucs::vector (1); v1 (0) = D(_ARES(1)); break;
  case TAG_COMPLEX:
    v1 = qucs::vector (1); v1 (0) = *C(_ARES(1)); break;
  case TAG_BOOLEAN:
    v1 = qucs::vector (1); v1 (0) = B(_ARES(1)) ? 1.0 : 0.0; break;
  case TAG_VECTOR:
    v1 = *V(_ARES(1)); break;
  }
  switch (t2) {
  case TAG_DOUBLE:
    v2 = qucs::vector (1); v2 (0) = D(_ARES(2)); break;
  case TAG_COMPLEX:
    v2 = qucs::vector (1); v2 (0) = *C(_ARES(2)); break;
  case TAG_BOOLEAN:
    v2 = qucs::vector (1); v2 (0) = B(_ARES(2)) ? 1.0 : 0.0; break;
  case TAG_VECTOR:
    v2 = *V(_ARES(2)); break;
  }
  _DEFV ();
  int i, a, b;
  qucs::vector * v = new qucs::vector ();
  for (a = b = i = 0; i < cond->getSize (); i++) {
    v->add (cond->get (i) != 0.0 ? v1 (a) : v2 (b));
    a++;
    b++;
    if (a >= v1.getSize ()) a = 0;
    if (b >= v2.getSize ()) b = 0;
  }
  res->v = v;
  return res;
}

// ************************** less *************************
constant * evaluate::less_d_d (constant * args) {
  _ARD0 (d0);
  _ARD1 (d1);
  _DEFB ();
  _RETB (d0 < d1);
}

constant * evaluate::less_d_c (constant * args) {
  _ARD0 (d0);
  _ARC1 (c1);
  _DEFB ();
  _RETB (d0 < *c1);
}

constant * evaluate::less_d_v (constant * args) {
  _ARD0 (d0);
  _ARV1 (v1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v1->getSize (); i++) {
    v->add (d0 < v1->get (i) ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::less_c_d (constant * args) {
  _ARC0 (c0);
  _ARD1 (d1);
  _DEFB ();
  _RETB (*c0 < d1);
}

constant * evaluate::less_c_c (constant * args) {
  _ARC0 (c0);
  _ARC1 (c1);
  _DEFB ();
  _RETB (*c0 < *c1);
}

constant * evaluate::less_c_v (constant * args) {
  _ARC0 (c0);
  _ARV1 (v1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v1->getSize (); i++) {
    v->add (*c0 < v1->get (i) ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::less_v_d (constant * args) {
  _ARV0 (v0);
  _ARD1 (d1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v0->getSize (); i++) {
    v->add (real (v0->get (i)) < d1 ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::less_v_c (constant * args) {
  _ARV0 (v0);
  _ARC1 (c1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v0->getSize (); i++) {
    v->add (v0->get (i) < *c1 ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::less_v_v (constant * args) {
  _ARV0 (v0);
  _ARV1 (v1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v0->getSize (); i++) {
    v->add (v0->get (i) < v1->get (i) ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

// ************************* greater ***********************
constant * evaluate::greater_d_d (constant * args) {
  _ARD0 (d0);
  _ARD1 (d1);
  _DEFB ();
  _RETB (d0 > d1);
}

constant * evaluate::greater_d_c (constant * args) {
  _ARD0 (d0);
  _ARC1 (c1);
  _DEFB ();
  _RETB (d0 > *c1);
}

constant * evaluate::greater_d_v (constant * args) {
  _ARD0 (d0);
  _ARV1 (v1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v1->getSize (); i++) {
    v->add (d0 > real (v1->get (i)) ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::greater_c_d (constant * args) {
  _ARC0 (c0);
  _ARD1 (d1);
  _DEFB ();
  _RETB (*c0 > d1);
}

constant * evaluate::greater_c_c (constant * args) {
  _ARC0 (c0);
  _ARC1 (c1);
  _DEFB ();
  _RETB (*c0 > *c1);
}

constant * evaluate::greater_c_v (constant * args) {
  _ARC0 (c0);
  _ARV1 (v1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v1->getSize (); i++) {
    v->add (*c0 > v1->get (i) ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::greater_v_d (constant * args) {
  _ARV0 (v0);
  _ARD1 (d1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v0->getSize (); i++) {
    v->add (real (v0->get (i)) > d1 ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::greater_v_c (constant * args) {
  _ARV0 (v0);
  _ARC1 (c1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v0->getSize (); i++) {
    v->add (v0->get (i) > *c1 ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::greater_v_v (constant * args) {
  _ARV0 (v0);
  _ARV1 (v1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v0->getSize (); i++) {
    v->add (v0->get (i) > v1->get (i) ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

// ********************** less or equal ********************
constant * evaluate::lessorequal_d_d (constant * args) {
  _ARD0 (d0);
  _ARD1 (d1);
  _DEFB ();
  _RETB (d0 <= d1);
}

constant * evaluate::lessorequal_d_c (constant * args) {
  _ARD0 (d0);
  _ARC1 (c1);
  _DEFB ();
  _RETB (d0 <= *c1);
}

constant * evaluate::lessorequal_d_v (constant * args) {
  _ARD0 (d0);
  _ARV1 (v1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v1->getSize (); i++) {
    v->add (d0 <= real (v1->get (i)) ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::lessorequal_c_d (constant * args) {
  _ARC0 (c0);
  _ARD1 (d1);
  _DEFB ();
  _RETB (*c0 <= d1);
}

constant * evaluate::lessorequal_c_c (constant * args) {
  _ARC0 (c0);
  _ARC1 (c1);
  _DEFB ();
  _RETB (*c0 <= *c1);
}

constant * evaluate::lessorequal_c_v (constant * args) {
  _ARC0 (c0);
  _ARV1 (v1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v1->getSize (); i++) {
    v->add (*c0 <= v1->get (i) ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::lessorequal_v_d (constant * args) {
  _ARV0 (v0);
  _ARD1 (d1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v0->getSize (); i++) {
    v->add (real (v0->get (i)) <= d1 ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::lessorequal_v_c (constant * args) {
  _ARV0 (v0);
  _ARC1 (c1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v0->getSize (); i++) {
    v->add (v0->get (i) <= *c1 ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::lessorequal_v_v (constant * args) {
  _ARV0 (v0);
  _ARV1 (v1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v0->getSize (); i++) {
    v->add (v0->get (i) <= v1->get (i) ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

// ********************* greater or equal ******************
constant * evaluate::greaterorequal_d_d (constant * args) {
  _ARD0 (d0);
  _ARD1 (d1);
  _DEFB ();
  _RETB (d0 >= d1);
}

constant * evaluate::greaterorequal_d_c (constant * args) {
  _ARD0 (d0);
  _ARC1 (c1);
  _DEFB ();
  _RETB (d0 >= *c1);
}

constant * evaluate::greaterorequal_d_v (constant * args) {
  _ARD0 (d0);
  _ARV1 (v1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v1->getSize (); i++) {
    v->add (d0 >= real (v1->get (i)) ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::greaterorequal_c_d (constant * args) {
  _ARC0 (c0);
  _ARD1 (d1);
  _DEFB ();
  _RETB (*c0 >= d1);
}

constant * evaluate::greaterorequal_c_c (constant * args) {
  _ARC0 (c0);
  _ARC1 (c1);
  _DEFB ();
  _RETB (*c0 >= *c1);
}

constant * evaluate::greaterorequal_c_v (constant * args) {
  _ARC0 (c0);
  _ARV1 (v1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v1->getSize (); i++) {
    v->add (*c0 >= v1->get (i) ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::greaterorequal_v_d (constant * args) {
  _ARV0 (v0);
  _ARD1 (d1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v0->getSize (); i++) {
    v->add (real (v0->get (i)) >= d1 ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::greaterorequal_v_c (constant * args) {
  _ARV0 (v0);
  _ARC1 (c1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v0->getSize (); i++) {
    v->add (v0->get (i) >= *c1 ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::greaterorequal_v_v (constant * args) {
  _ARV0 (v0);
  _ARV1 (v1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v0->getSize (); i++) {
    v->add (v0->get (i) >= v1->get (i) ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

// ************************** equal ************************
constant * evaluate::equal_d_d (constant * args) {
  _ARD0 (d0);
  _ARD1 (d1);
  _DEFB ();
  _RETB (d0 == d1);
}

constant * evaluate::equal_d_c (constant * args) {
  _ARD0 (d0);
  _ARC1 (c1);
  _DEFB ();
  _RETB (d0 == *c1);
}

constant * evaluate::equal_d_v (constant * args) {
  _ARD0 (d0);
  _ARV1 (v1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v1->getSize (); i++) {
    v->add (d0 == real (v1->get (i)) ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::equal_c_d (constant * args) {
  _ARC0 (c0);
  _ARD1 (d1);
  _DEFB ();
  _RETB (*c0 == d1);
}

constant * evaluate::equal_c_c (constant * args) {
  _ARC0 (c0);
  _ARC1 (c1);
  _DEFB ();
  _RETB (*c0 == *c1);
}

constant * evaluate::equal_c_v (constant * args) {
  _ARC0 (c0);
  _ARV1 (v1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v1->getSize (); i++) {
    v->add (*c0 == v1->get (i) ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::equal_v_d (constant * args) {
  _ARV0 (v0);
  _ARD1 (d1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v0->getSize (); i++) {
    v->add (real (v0->get (i)) == d1 ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::equal_v_c (constant * args) {
  _ARV0 (v0);
  _ARC1 (c1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v0->getSize (); i++) {
    v->add (v0->get (i) == *c1 ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::equal_v_v (constant * args) {
  _ARV0 (v0);
  _ARV1 (v1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v0->getSize (); i++) {
    v->add (v0->get (i) == v1->get (i) ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::equal_b_b (constant * args) {
  _ARB0 (b0);
  _ARB1 (b1);
  _DEFB ();
  _RETB (b0 == b1);
}

// ************************ not equal **********************
constant * evaluate::notequal_d_d (constant * args) {
  _ARD0 (d0);
  _ARD1 (d1);
  _DEFB ();
  _RETB (d0 != d1);
}

constant * evaluate::notequal_d_c (constant * args) {
  _ARD0 (d0);
  _ARC1 (c1);
  _DEFB ();
  _RETB (d0 != *c1);
}

constant * evaluate::notequal_d_v (constant * args) {
  _ARD0 (d0);
  _ARV1 (v1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v1->getSize (); i++) {
    v->add (d0 != real (v1->get (i)) ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::notequal_c_d (constant * args) {
  _ARC0 (c0);
  _ARD1 (d1);
  _DEFB ();
  _RETB (*c0 != d1);
}

constant * evaluate::notequal_c_c (constant * args) {
  _ARC0 (c0);
  _ARC1 (c1);
  _DEFB ();
  _RETB (*c0 != *c1);
}

constant * evaluate::notequal_c_v (constant * args) {
  _ARC0 (c0);
  _ARV1 (v1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v1->getSize (); i++) {
    v->add (*c0 != v1->get (i) ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::notequal_v_d (constant * args) {
  _ARV0 (v0);
  _ARD1 (d1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v0->getSize (); i++) {
    v->add (real (v0->get (i)) != d1 ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::notequal_v_c (constant * args) {
  _ARV0 (v0);
  _ARC1 (c1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v0->getSize (); i++) {
    v->add (v0->get (i) != *c1 ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::notequal_v_v (constant * args) {
  _ARV0 (v0);
  _ARV1 (v1);
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (int i = 0; i < v0->getSize (); i++) {
    v->add (v0->get (i) != v1->get (i) ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::notequal_b_b (constant * args) {
  _ARB0 (b0);
  _ARB1 (b1);
  _DEFB ();
  _RETB (b0 != b1);
}

// *************************** not *************************
constant * evaluate::not_b (constant * args) {
  _ARB0 (b0);
  _DEFB ();
  _RETB (!b0);
}

// *************************** or **************************
constant * evaluate::or_b_b (constant * args) {
  _ARB0 (b0);
  _ARB1 (b1);
  _DEFB ();
  _RETB (b0 || b1);
}

// ************************** and **************************
constant * evaluate::and_b_b (constant * args) {
  _ARB0 (b0);
  _ARB1 (b1);
  _DEFB ();
  _RETB (b0 && b1);
}

// ******************* random numbers **********************
constant * evaluate::rand (constant *) {
  _DEFD ();
  _RETD (((nr_double_t) ::rand ()) / (nr_double_t) RAND_MAX);
}

constant * evaluate::srand_d (constant * args) {
  static int done = 0;
  _ARD0 (d0);
  _DEFD ();
  if (!done) {
    unsigned int i0 = (unsigned int) d0;
    ::srand (i0);
    done = 1;
    _RETD (1.0);
  } else {
    _RETD (0.0);
  }
}


// ******************* assert test *************************
constant * evaluate::assert_b(constant *args)
{
    _ARB0(b0);
    if(!b0) {
	THROW_MATH_EXCEPTION ("assert failed");
	abort();
     }
     _DEFB ();
    _RETB(true);
}


constant * evaluate::assert_d(constant *args)
{
    _ARD0(d0);
    if(d0 == 0.0) {
        THROW_MATH_EXCEPTION ("assert failed");
	abort();
    }
     _DEFB ();
    _RETB(true);
}


constant * evaluate::assert_v (constant * args) {
  _ARV0 (v0);
  for (int i = 0; i < v0->getSize (); i++) {
    if( v0->get(i) == 0.0 ) {
	THROW_MATH_EXCEPTION ("assert failed");
	abort();
    }
  }
  _DEFB ();
  _RETB(true);
}


constant * evaluate::bugon_b(constant *args)
{
    _ARB0(b0);
    if(b0) {
	THROW_MATH_EXCEPTION ("bugon failed");
	abort();
     }
     _DEFB ();
    _RETB(true);
}


constant * evaluate::bugon_d(constant *args)
{
    _ARD0(d0);
    if(d0 != 0.0) {
	THROW_MATH_EXCEPTION ("bugon failed");
	abort();
     }
     _DEFB ();
    _RETB(true);
}


constant * evaluate::bugon_v (constant * args) {
  _ARV0 (v0);
  for (int i = 0; i < v0->getSize (); i++) {
    if( v0->get(i) != 0.0 ) {
	THROW_MATH_EXCEPTION ("bugon failed");
	abort();
    }
  }
  _DEFB ();
  _RETB(true);
}


// ******************* immediate vectors *******************
constant * evaluate::vector_x (constant * args) {
  _DEFV ();
  qucs::vector * v = new qucs::vector ();
  for (node * arg = args; arg != NULL; arg = arg->getNext ()) {
    constant * c = arg->getResult ();
    switch (arg->getType ()) {
    case TAG_COMPLEX:
      v->add (*(c->c)); break;
    case TAG_DOUBLE:
      v->add (c->d); break;
    case TAG_BOOLEAN:
      v->add (c->b ? 1.0 : 0.0); break;
    case TAG_VECTOR:
      v->add (c->v); break;
    default:
      v->add (0.0); break;
    }
  }
  res->v = v;
  return res;
}

// ******************* immediate matrices ******************
constant * evaluate::matrix_x (constant * args) {
  _DEFM ();
  /* create temporary list of vectors */
  qucs::vector * va = NULL;
  qucs::vector * v = new qucs::vector ();
  va = v;
  for (node * arg = args; arg != NULL; arg = arg->getNext ()) {
    constant * c = arg->getResult ();
    switch (arg->getType ()) {
    case TAG_COMPLEX:
      v->add (*(c->c)); break;
    case TAG_DOUBLE:
      v->add (c->d); break;
    case TAG_BOOLEAN:
      v->add (c->b ? 1.0 : 0.0); break;
    case TAG_VECTOR:
      v->add (c->v); break;
    case TAG_CHAR:
      if (c->chr == ';') {
	/* append new vector, i.e. a new matrix row */
	qucs::vector * vn = new qucs::vector ();
	v->setNext (vn);
	v = vn;
      }
      else v->add (0.0);
      break;
    default:
      v->add (0.0); break;
    }
  }
  /* determine matrix dimensions and create it */
  int r, c;
  for (r = 0, c = 0, v = va; v != NULL; v = (::vector *) v->getNext (), r++) {
    if (c < v->getSize ()) c = v->getSize ();
  }
  matrix * m = new matrix (r, c);
  /* fill in matrix entries and delete temporary vector list */
  qucs::vector * vn = NULL;
  for (r = 0, v = va; v != NULL; v = vn, r++) {
    for (c = 0; c < v->getSize (); c++) {
      m->set (r, c, v->get (c));
    }
    vn = (::vector *) v->getNext ();
    delete v;
  }
  /* return result matrix */
  res->m = m;
  return res;
}

// ********************** EMI receiver *********************
constant * evaluate::receiver_v_v (constant * args) {
  _ARV0 (da);
  _ARV1 (dt);
  _DEFV ();

  // run receiver functionality
  qucs::vector * ed;
  if (_ARG(2)) {
    _ARI2 (len);
    ed = emi::receiver (da, dt, len);
  }
  else {
    ed = emi::receiver (da, dt);
  }

  // create two vectors for spectrum and frequency
  int rlen = ed->getSize ();
  qucs::vector * rvec = new qucs::vector (rlen);
  qucs::vector * rfeq = new qucs::vector (rlen);
  for (int i = 0; i < rlen; i++) {
    (*rvec)(i) = real (ed->get (i));
    (*rfeq)(i) = imag (ed->get (i));
  }
  delete ed;

  // put results back into equation solver
  node * gen = SOLVEE(0)->addGeneratedEquation (rfeq, "Frequency");
  res->addPrepDependencies (A(gen)->result);
  res->dropdeps = 1;
  res->v = rvec;
  return res;
}

// Include the application array.
#include "applications.h"

