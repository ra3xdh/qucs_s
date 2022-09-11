/*
 * variable.h - generic variable class definitions
 *
 * Copyright (C) 2004, 2007 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __VARIABLE_H__
#define __VARIABLE_H__

#include <string>

#include "components/microstrip/substrate.h"
#include "analysis.h"
#include "equation.h"



//using namespace qucs::eqn;
namespace qucs {

// Enumerate possible types of variables.
enum variably_type {
  VAR_UNKNOWN = -1, // not yet defined
  VAR_CONSTANT,     // equation constant
  VAR_REFERENCE,    // equation reference
  VAR_SUBSTRATE,    // substrate definition
  VAR_VALUE,        // equation result
  VAR_ANALYSIS      // analysis
};

class substrate;
class analysis;

namespace eqn {
  class equation;
  class constant;
}


class variable
{
 public:
  variable ();
  variable (const char * const n);
  variable (const variable &);
  virtual ~variable () = default;

  //! Sets the name of the variable
  void setName (const char * const n) {
    name = n ? std::string(n) : std::string();
  };

  //! Returns the name of the variable.
  const char * getName (void) const {
    return this->name.c_str();
  };
  void setNext (variable * const v) { next = v; }
  variable * getNext (void) const { return next; }

  void setType (const int t) { type = t; }
  int getType (void) const { return type; }

  void setConstant (eqn::constant * const c) { type = VAR_CONSTANT; value.c = c; }
  eqn::constant * getConstant (void) const { return value.c; }
  void setReference (eqn::reference * const r) { type = VAR_REFERENCE; value.r = r; }
  eqn::reference * getReference (void) const { return value.r; }
  void setSubstrate (substrate * const s) { type = VAR_SUBSTRATE; value.s = s; }
  substrate * getSubstrate (void) { return value.s; }
  void setValue (eqn::constant * const v) { type = VAR_VALUE; value.v = v; }
  eqn::constant * getValue (void) { return value.v; }
  void setAnalysis (analysis * const a) { type = VAR_ANALYSIS; value.a = a; }
  analysis * getAnalysis (void) const { return this->value.a; }
  const char * toString (void);
  void setPassing (const bool p) { this->pass = p; }
  bool getPassing (void) const { return this->pass; }

 private:
  std::string name;
  bool pass;
  int type;
  union value_t {
    eqn::constant * c;  // equation constant
    eqn::reference * r; // equation reference
    substrate * s; // substrate definition
    eqn::constant * v;  // equation result
    analysis * a;  // analysis
  } value;
  variable * next;
};

} // namespace qucs

#endif /* __VARIABLE_H__ */
