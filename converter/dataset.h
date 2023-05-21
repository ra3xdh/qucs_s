/*
 * dataset.h - dataset class definitions
 *
 * Copyright (C) 2003, 2004, 2006, 2007 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __DATASET_H__
#define __DATASET_H__

#include "object.h"

namespace qucs {

class vector;

class dataset : public object
{
 public:
  dataset ();
  dataset (char *);
  dataset (const dataset &);
  ~dataset ();
  void addDependency (qucs::vector *);
  void addDependencies (qucs::vector *);
  void appendDependency (qucs::vector *);
  void appendDependencies (qucs::vector *);
  void addVariable (qucs::vector *);
  void addVariables (qucs::vector *);
  void appendVariable (qucs::vector *);
  void appendVariables (qucs::vector *);
  void applyDependencies (qucs::vector * v);
  void delDependency (qucs::vector *);
  void delVariable (qucs::vector *);

  void assignDependency (const char *const, const char * const);
  char * getFile (void);
  void setFile (const char *);
  void print (void);
  void printData (qucs::vector *, FILE *);
  void printDependency (qucs::vector *, FILE *);
  void printVariable (qucs::vector *, FILE *);
  qucs::vector * findDependency (const char *);
  qucs::vector * findVariable (const std::string &);
  qucs::vector * getDependencies (void) { return dependencies; }
  qucs::vector * getVariables (void) { return variables; }
  int isDependency (qucs::vector *);
  int isVariable (qucs::vector *);
  qucs::vector * findOrigin (char *);
  static dataset * load (const char *);
  static dataset * load_touchstone (const char *);
  static dataset * load_csv (const char *);
  static dataset * load_citi (const char *);
  static dataset * load_zvr (const char *);
  static dataset * load_mdl (const char *);

  int countDependencies (void);
  int countVariables (void);

 private:
  char * file;
  qucs::vector * dependencies;
  qucs::vector * variables;
};

} // namespace qucs

#endif /* __DATASET_H__ */
