/*
 * input.h - input netlist class definitions
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

#ifndef __INPUT_H__
#define __INPUT_H__

namespace qucs {

class net;
class circuit;
class analysis;
class environment;

class input : public object
{
 public:
  input ();
  input (char *);
  ~input ();
  int netlist (net *);
  FILE * getFile (void) { return fd; }
  void setFile (FILE * f) { fd = f; }
  void factory (void);
  circuit * createCircuit (char *);
  analysis * createAnalysis (char *);
  substrate * createSubstrate (char *);
  environment * getEnv (void) { return env; }
  void setEnv (environment * e) { env = e; }
  static void assignDefaultProperties (object *, struct define_t *);
  static qucs::vector * createVector (struct value_t *);

 private:
  FILE * fd;
  net * subnet;
  environment * env;
};

// externalize global variable
extern int netlist_check;

}

#endif /* __INPUT_H__ */
