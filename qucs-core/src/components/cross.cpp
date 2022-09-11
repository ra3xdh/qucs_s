/*
 * cross.cpp - cross connector class implementation
 *
 * Copyright (C) 2003, 2004, 2008 Stefan Jahn <stefan@lkcc.org>
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
#include "cross.h"

using namespace qucs;

cross::cross () : circuit (4) {
  type = CIR_CROSS;
  setVoltageSources (3);
}

void cross::initSP (void) {
  allocMatrixS ();
  setS (NODE_1, NODE_1, -1.0 / 2.0);
  setS (NODE_1, NODE_2,  1.0 / 2.0);
  setS (NODE_1, NODE_3,  1.0 / 2.0);
  setS (NODE_1, NODE_4,  1.0 / 2.0);
  setS (NODE_2, NODE_1,  1.0 / 2.0);
  setS (NODE_2, NODE_2, -1.0 / 2.0);
  setS (NODE_2, NODE_3,  1.0 / 2.0);
  setS (NODE_2, NODE_4,  1.0 / 2.0);
  setS (NODE_3, NODE_1,  1.0 / 2.0);
  setS (NODE_3, NODE_2,  1.0 / 2.0);
  setS (NODE_3, NODE_3, -1.0 / 2.0);
  setS (NODE_3, NODE_4,  1.0 / 2.0);
  setS (NODE_4, NODE_1,  1.0 / 2.0);
  setS (NODE_4, NODE_2,  1.0 / 2.0);
  setS (NODE_4, NODE_3,  1.0 / 2.0);
  setS (NODE_4, NODE_4, -1.0 / 2.0);
}

void cross::initDC (void) {
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
  voltageSource (VSRC_2, NODE_1, NODE_3);
  voltageSource (VSRC_3, NODE_1, NODE_4);
}

void cross::initAC (void) {
  initDC ();
}

void cross::initTR (void) {
  initDC ();
}
