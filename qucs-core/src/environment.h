/*
 * environment.h - variable environment class definitions
 *
 * Copyright (C) 2004, 2006, 2007, 2009 Stefan Jahn <stefan@lkcc.org>
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

/*! \file environment.h
 * \brief The environment class definition.
 *
 * Contains the environment class definition.
 */

#ifndef __ENVIRONMENT_H__
#define __ENVIRONMENT_H__

#include <list>
#include <string>

#include "equation.h"

namespace qucs {

class variable;
class checker;
class solver;
class dataset;


/*! \class environment
 * \brief Houses the settings for netlist evaluation.
 *
 * The environment class holds information and pointers to the
 * classes and methods used to evaluate a netlist.
 *
 */
class environment
{
 public:
  environment ();
  environment (const std::string & p_name);
  environment (const environment &);
  virtual ~environment ();
  void copy (const environment &);
  void setName (char *) = delete;
  void print (const bool all = false) const;
  void setDefinitions (struct definition_t * const d) { defs = d; }
  struct definition_t * getDefinitions (void) const { return defs; }

  // variable specific functionality
  void copyVariables (variable *);
  void deleteVariables (void);
  void addVariable (variable * const, const bool pass = true);
  variable * getVariable (const char * const) const;

  // equation specific functionality
  void setChecker (eqn::checker * c) { checkee = c; }
  eqn::checker * getChecker (void) { return checkee; }
  void setSolver (eqn::solver * s) { solvee = s; }
  eqn::solver * getSolver (void) { return solvee; }
  int equationChecker (const int noundefined = 1) const;
  int equationSolver (dataset * const);
  int runSolver (void);
  void equationSolver (void);

  // subcircuit specific
  qucs::vector getVector (const char * const) const ;
  void setDoubleConstant (const char * const, const nr_double_t);
  nr_double_t getDoubleConstant (const char * const) const;
  void setDouble (const char * const , nr_double_t);
  nr_double_t getDouble (const char * const) const;
  void setDoubleReference (const char * const, char *);
  char * getDoubleReference (const char * const) const;
  void updateReferences (environment *);
  void passConstants (void);
  void fetchConstants (void);
  variable * findValue (char *);
  void setValue (char *, eqn::constant *);
  void saveResults (void);

  /*! Adds a child to the environment. */
  inline void push_front_Child (environment * child) {
    children.push_front (child);
  }

  /*! Removes a child from the environment. */
  void remove_Child (environment * child) {
    children.remove (child);
  }

  /*! set the name */
  void setName (const std::string &p_name) {
    this->name = p_name;
  }

  /*! Returns the name of the environment. */
  const std::string & getName(void) const {
    return this->name;
  }

 private:
  std::string name;
  variable * root;
  eqn::checker * checkee;
  eqn::solver * solvee;
  std::list<environment *> children;
  bool iscopy;
  struct definition_t * defs;
};

} // namespace qucs

#endif /* __ENVIRONMENT_H__ */
