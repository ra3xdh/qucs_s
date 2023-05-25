/*
 * equation.h - checker definitions for Qucs equations
 *
 * Copyright (C) 2004-2009 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __EQUATION_H__
#define __EQUATION_H__

#include "object.h"
#include "complex.h"
#include "vector.h"
#include "matrix.h"
#include "matvec.h"

struct definition_t;

namespace qucs {

class strlist;
class dataset;
class range;

namespace eqn {

class solver;
class checker;
class constant;
class reference;
class assignment;
class application;

enum NodeTag {
  UNKNOWN = -1,
  CONSTANT = 0, /* source code constant */
  REFERENCE,    /* variable reference   */
  APPLICATION,  /* call to function     */
  ASSIGNMENT    /* root of equation     */
};

/* The equation node base class defines and implements the basic
   functionality of an equation node.  Possible types of nodes are
   listed in 'NodeTag'. */
class node
{
public:
  node ();
  node (int);
  node (const node &);
  virtual ~node ();
  node * getNext (void) { return next; }
  void setNext (node * n) { next = n; }
  int count (void);
  void append (node *);
  void appendNodes (node *);
  void setDependencies (strlist *);
  strlist * getDependencies (void);
  void setDataDependencies (strlist *);
  strlist * getDataDependencies (void) { return dataDependencies; }
  void setDropDependencies (strlist * deps) { dropDependencies = deps; }
  void addDropDependencies (char *);
  strlist * getDropDependencies (void) { return dropDependencies; }
  void setPrepDependencies (strlist * deps) { prepDependencies = deps; }
  void addPrepDependencies (char *);
  void appendPrepDependencies (strlist *);
  strlist * getPrepDependencies (void) { return prepDependencies; }
  strlist * recurseDependencies (checker *, strlist *);
  node * get (int);
  constant * getResult (int);
  int getType (void) { return type; }
  int getTag (void) { return tag; }
  void setType (int tag) { type = tag; }
  constant * getResult (void) { return res; }
  nr_double_t getResultDouble (void);
  nr_complex_t getResultComplex (void);
  qucs::vector getResultVector (void);
  void setResult (constant *);
  char * getInstance (void);
  void setInstance (const char *);
  void applyInstance (void);
  constant * calculate (void);
  strlist * collectDependencies (void);
  strlist * collectDataDependencies (void);

  /* These functions should be overloaded by derivative classes. */
  virtual void print (void) { }
  virtual void addDependencies (strlist *) { }
  virtual int evalType (void) { return type; }
  virtual char * toString (void) { return txt; }
  virtual constant * evaluate (void) { return res; }
  virtual node * differentiate (char *) { return this; }
  virtual node * recreate (void) { return new node (*this); }
  virtual void replace (char *, char *) { }

public:
  int duplicate;
  int cycle;
  int evalPossible;
  int skip;
  char * txt;
  int evaluated;
  char * instance;
  int output;
  int dropdeps;
  solver * solvee;
  checker * checkee;

private:
  int type;
  int tag;
  node * next;
  strlist * dependencies;
  constant * res;
  strlist * dataDependencies;
  strlist * dropDependencies;
  strlist * prepDependencies;
};

enum ConstantTag {
  TAG_UNKNOWN =   0,
  TAG_DOUBLE  =   1, /* double constant          */
  TAG_COMPLEX =   2, /* complex value            */
  TAG_VECTOR  =   4, /* list of complex values   */
  TAG_MATRIX  =   8, /* complex matrix           */
  TAG_MATVEC  =  16, /* list of complex matrices */
  TAG_CHAR    =  32, /* single character         */
  TAG_STRING  =  64, /* character string         */
  TAG_RANGE   = 128, /* interval specification   */
  TAG_BOOLEAN = 256, /* boolean value            */
};

/* This class represents any type of constant expression. */
class constant : public node
{
public:
  constant ();
  constant (int);
  constant (const constant &);
  ~constant ();
  void print (void);
  int evalType (void);
  char * toString (void);
  constant * evaluate (void);
  node * differentiate (char *);
  node * recreate (void);

public:
  bool dataref;
  int type;
  union {
    nr_double_t d;
    nr_complex_t * c;
    qucs::vector * v;
    matrix * m;
    matvec * mv;
    char chr;
    char * s;
    range * r;
    bool b;
  };
};

/* The class represents variable references. */
class reference : public node
{
public:
  reference ();
  reference (const reference &);
  ~reference ();
  void print (void);
  void addDependencies (strlist *);
  void findVariable (void);
  int evalType (void);
  char * toString (void);
  constant * evaluate (void);
  node * differentiate (char *);
  node * recreate (void);
  void replace (char *, char *);

public:
  char * n;
  node * ref;
};

/* This class represents assignments with a left hand and right hand
   side. */
class assignment : public node
{
public:
  assignment ();
  assignment (const assignment &);
  ~assignment ();
  void print (void);
  void addDependencies (strlist *);
  int evalType (void);
  char * toString (void);
  constant * evaluate (void);
  node * differentiate (char *);
  node * recreate (void);
  void replace (char *, char *);
  void rename (char *);
  void mul (assignment *);
  void add (assignment *);
  void mulref (assignment *);

public:
  char * result;
  node * body;
};

// Type of application function.
typedef constant * (* evaluator_t) (constant *);

// Type of derivative function.
typedef node * (* differentiator_t) (application *, char *);

/* The application class represents any kind of operation (unary,
   binary and n-ary ones) containing the appropriate argument list. */
class application : public node
{
public:
  application ();
  application (const application &);
  application (const char *, int);
  ~application ();
  void print (void);
  void addDependencies (strlist *);
  int evalType (void);
  char * toString (void);
  constant * evaluate (void);
  node * differentiate (char *);
  node * recreate (void);
  void replace (char *, char *);

public:
  char * n;
  int nargs;
  node * args;
  node * ddx;
  evaluator_t eval;
  differentiator_t derive;

private:
  void evalTypeArgs (void);
  char * createKey (void);
  int evalTypeFast (void);
  int findDifferentiator (void);
};

/* This class implements the actual functionality regarding a set of
   equations stored within a netlist. */
class checker
{
public:
  checker ();
  ~checker ();
  void collectDependencies (void);
  void collectDependencies (node *);
  void setEquations (node *);
  node * getEquations (void) { return equations; }
  void list (void);
  int findUndefined (int);
  static const char * tag2key (int);
  static int isGenerated (char *);
  strlist * getVariables (void);
  int findDuplicate (void);
  static node * findEquation (node *, const char * const);
  int detectCycles (void);
  static strlist * foldDependencies (strlist *);
  node * appendEquation (node *, node *);
  void dropEquation (node *);
  void reorderEquations (void);
  static node * lastEquation (node *);
  int applyTypes (void);
  int checkExport (void);
  void constants (void);
  int check (int noundefined = 1);
  strlist * variables (void);
  node * addDouble (const char *, const char *, nr_double_t);
  node * createDouble (const char *, const char *, nr_double_t);
  node * addComplex (const char *, const char *, nr_complex_t);
  node * createComplex (const char *, const char *, nr_complex_t);
  node * addReference (const char *, const char *, char *);
  node * createReference (const char *, const char *, char *);
  void appendEquation (node *);
  void addEquation (node *);
  node * findEquation (const char *
		       const) const;
  bool containsVariable (const char * const) const;
  nr_double_t getDouble (const char *const) const;
  void setDouble (const char *const, nr_double_t);
  qucs::vector getVector (const char * const) const;
  void setDefinitions (struct definition_t * d) { defs = d; }
  struct definition_t * getDefinitions (void) { return defs; }
  node * findProperty (char *);

public:
  node * equations;

 private:
  bool consts;
  struct definition_t * defs;
};

/* The solver class is finally used to solve the list of equations. */
class solver
{
public:
  solver (checker *);
  ~solver ();
  void setEquations (node * eqn) { equations = eqn; }
  node * getEquations (void) { return equations; }
  void setData (dataset * d) { data = d; }
  dataset * getDataset (void) { return data; }
  void evaluate (void);
  node * addEquationData (qucs::vector *, bool ref = false);
  node * addEquationData (matvec *);
  node * addGeneratedEquation (qucs::vector *, const char *);
  qucs::vector * dataVector (node *);
  void checkinDataset (void);
  void checkoutDataset (void);
  static int dataSize (constant *);
  int getDependencySize (strlist *, int);
  int getDataSize (char *);
  strlist * collectDataDependencies (node *);
  int dataSize (strlist *);
  qucs::vector * getDataVector (char *);
  void findMatrixVectors (qucs::vector *);
  char * isMatrixVector (char *, int&, int&);
  int findEquationResult (node *);
  int solve (dataset *);

public:
  node * equations;

private:
  dataset * data;
  int generated;
  checker * checkee;
};

} /* namespace eqn */

} // namespace qucs

#endif /* __EQUATION_H__ */
