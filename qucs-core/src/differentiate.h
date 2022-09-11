/*
 * differentiate.h - definitions for Qucs equation derivatives
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

#ifndef __DIFFERENTIATE_H__
#define __DIFFERENTIATE_H__

namespace qucs {

namespace eqn {

class constant;

/* This class is merely a container for the derivatives which can be
   applied to equations. */
class differentiate
{
 public:
  static node * plus_binary  (application *, char *);
  static node * plus_unary   (application *, char *);
  static node * minus_binary (application *, char *);
  static node * minus_unary  (application *, char *);
  static node * times	     (application *, char *);
  static node * over	     (application *, char *);
  static node * power	     (application *, char *);
  static node * ln	     (application *, char *);
  static node * log10	     (application *, char *);
  static node * log2	     (application *, char *);
  static node * sqrt	     (application *, char *);
  static node * exp	     (application *, char *);
  static node * sinc	     (application *, char *);
  static node * norm	     (application *, char *);
  static node * sin	     (application *, char *);
  static node * cos	     (application *, char *);
  static node * tan	     (application *, char *);
  static node * sec	     (application *, char *);
  static node * cot	     (application *, char *);
  static node * cosec	     (application *, char *);
  static node * arcsin	     (application *, char *);
  static node * square	     (application *, char *);
  static node * arccos	     (application *, char *);
  static node * arctan	     (application *, char *);
  static node * arccot	     (application *, char *);
  static node * arcsec	     (application *, char *);
  static node * arccosec     (application *, char *);
  static node * abs	     (application *, char *);
  static node * step	     (application *, char *);
  static node * sign	     (application *, char *);
  static node * sinh	     (application *, char *);
  static node * cosh	     (application *, char *);
  static node * tanh	     (application *, char *);
  static node * coth	     (application *, char *);
  static node * arsinh	     (application *, char *);
  static node * arcosh	     (application *, char *);
  static node * artanh	     (application *, char *);
  static node * arcoth	     (application *, char *);
  static node * arsech	     (application *, char *);
  static node * arcosech     (application *, char *);
  static node * ifthenelse   (application *, char *);
  static node * xhypot       (application *, char *);
  static node * limexp       (application *, char *);
  static node * vt           (application *, char *);

 private:
  static node * plus_reduce  (node *, node *);
  static node * minus_reduce (node *, node *);
  static node * minus_reduce (node *);
  static node * times_reduce (node *, node *);
  static node * over_reduce  (node *, node *);
  static node * power_reduce (node *, node *);
  static node * ln_reduce    (node *);
  static node * sqrt_reduce  (node *);
  static node * sqr_reduce   (node *);
  static node * app_reduce   (const char *, node *, node *);
  static node * hypot_reduce (node *, node *);

 private:
  static void   over_reduce_adv (node * &, node * &);
};

// Type of derivative function.
typedef node * (* differentiator_t) (application *, char *);

// Structure defining an differentiation.
struct differentiation_t
{
  const char * application; /* the name of the application         */
  differentiator_t derive;  /* the actual differentiation function */
  int nargs;                /* number of arguments                 */
};

extern struct differentiation_t differentiations[];

} /* namespace eqn */

} // namespace qucs

#endif /* __DIFFERENTIATE_H__ */
