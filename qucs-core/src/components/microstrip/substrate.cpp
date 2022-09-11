/*
 * substrate.cpp - microstrip substrate class implementation
 *
 * Copyright (C) 2004, 2008 Stefan Jahn <stefan@lkcc.org>
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

#include "component.h"
#include "substrate.h"

using namespace qucs;

// Constructor creates an unnamed instance of the substrate class.
substrate::substrate () : object () {
}

/* The copy constructor creates a new instance based on the given
   substrate object. */
substrate::substrate (const substrate & c) : object (c) {
}

// Destructor deletes a substrate object.
substrate::~substrate () {
}

// properties
PROP_REQ [] = {
  { "er", PROP_REAL, { 9.8, PROP_NO_STR }, PROP_RNGII (1, 100) },
  { "h", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_POS_RANGE },
  { "t", PROP_REAL, { 35e-6, PROP_NO_STR }, PROP_POS_RANGE },
  { "tand", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_POS_RANGE },
  { "rho", PROP_REAL, { 0.022e-6, PROP_NO_STR }, PROP_POS_RANGE },
  { "D", PROP_REAL, { 0.15e-6, PROP_NO_STR }, PROP_POS_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  PROP_NO_PROP };
struct define_t substrate::miscdef =
  { "SUBST", 0, PROP_COMPONENT, PROP_SUBSTRATE, PROP_LINEAR, PROP_DEF };
