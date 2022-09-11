/*
 * circuit.h - circuit class definitions
 *
 * Copyright (C) 2003-2008 Stefan Jahn <stefan@lkcc.org>
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

/*! \file circuit.h
 * \brief The circuit class header file.
 *
 * Contains the circuit class definition.
 */

#ifndef __CIRCUIT_H__
#define __CIRCUIT_H__

#include "characteristic.h"
#include "operatingpoint.h"

#define NODE_1 0
#define NODE_2 1
#define NODE_3 2
#define NODE_4 3
#define NODE_5 4
#define NODE_6 5
#define VSRC_1 0
#define VSRC_2 1
#define VSRC_3 2
#define VSRC_4 3
#define VSRC_5 4

#define MODFLAG(val,bit) if (val) flag |= (bit); else flag &= ~(bit);
#define RETFLAG(bit)     ((flag & (bit)) != 0)

#define CREATOR(val) \
  val (); \
  static qucs::circuit * create (void) { return new val (); } \
  static struct define_t cirdef; \
  static struct define_t * definition (void) { return &cirdef; }

#include <map>
#include <string>

#include "integrator.h"
#include "valuelist.h"

namespace qucs {

enum circuit_flag {
  CIRCUIT_ENABLED     = 1,
  CIRCUIT_LINEAR      = 2,
  CIRCUIT_ORIGINAL    = 4,
  CIRCUIT_VSOURCE     = 8,
  CIRCUIT_ISOURCE     = 16,
  CIRCUIT_INTVSOURCE  = 32,
  CIRCUIT_VARSIZE     = 64,
  CIRCUIT_PROBE       = 128,
  CIRCUIT_HISTORY     = 256,
};

class node;
class property;
class substrate;
class matrix;
class net;
class environment;
class history;

/*! \class circuit
 * \brief base class for qucs circuit elements.
 *
 * This is the base class for all circuit elements and provides the
 * the functionality required for all simulation types. It has a number
 * of virtual functions intended to be overridden by the inheiriting
 * class
 *
 */
class circuit : public object, public integrator
{
 public:
  circuit * getNext (void) const { return this->next; }
  void setNext (circuit * const o) { this->next = o; }
  circuit * getPrev (void) const { return prev; }
  void setPrev (circuit * const o) { this->prev = o; }
 private:
  circuit * next;
  circuit * prev;
  
 public:
  // constructor and destructor set
  circuit ();
  circuit (int);
  circuit (const circuit &);
  virtual ~circuit ();

  // functionality to be overloaded by real, derived circuit element
  // implementations
  /*! \fn initSP
   * \brief placeholder for S-Parameter initialisation function
   *
   * Virtual function intended to be overridden by the
   * inheiriting circuit element's S-Parameter initialisation
   * function. initSP is called before commencing the simulation
   * to set up the S-Parameter matrix.
   */
  virtual void initSP (void) { allocMatrixS (); }
  virtual void calcSP (nr_double_t) { }
  virtual void initDC (void) { allocMatrixMNA (); }
  virtual void calcDC (void) { }
  virtual void restartDC (void) { }
  virtual void initNoiseSP (void) { allocMatrixN (); }
  virtual void calcNoiseSP (nr_double_t) { }
  virtual void initNoiseAC (void) { allocMatrixN (vsources); }
  virtual void calcNoiseAC (nr_double_t) { }
  virtual void initAC (void) { allocMatrixMNA (); }
  virtual void calcAC (nr_double_t) { }
  virtual void initTR (void) { allocMatrixMNA (); }
  virtual void calcTR (nr_double_t) { }
  virtual void initHB (void) { allocMatrixMNA (); }
  virtual void calcHB (nr_double_t) { }
  virtual void initHB (int) { allocMatrixMNA (); }
  virtual void calcHB (int) { }
  virtual void calcOperatingPoints (void) { }
  virtual void saveOperatingPoints (void) { }
  virtual void calcCharacteristics (nr_double_t) { }
  virtual void saveCharacteristics (nr_double_t) { }
  virtual void saveCharacteristics (nr_complex_t) { }

  // basic circuit element functionality
  void   setNode (int, const std::string&, int intern = 0);
  node * getNode (int);
  void   setType (int t) { type = t; }
  int    getType (void) { return type; }
  /*! \fn getSize
   * \brief Get the number of ports the circuit element has.
   *
   * Gets the number of ports the circuit element has
   */
  int    getSize (void) { return size; }
  /*! \fn setSize
   * \brief Set the number of ports the circuit element has.
   * \param s integer representing the number of ports
   *
   * Sets/changes the number of ports the circuit element has.
   * On setting this value, previously stored node and matrix
   * information is completely lost unless the new size equals
   * the original size
   */
  void   setSize (int);
  /*! \fn isEnabled
   * \brief Reports if circuit element is enabled.
   *
   * Returns true if the circuit element is enabled or false
   * otherwise.
   */
  bool   isEnabled (void) { return RETFLAG (CIRCUIT_ENABLED); }
  /*! \fn setEnabled
   * \brief Set a circuit element to be enabled or disabled.
   * \param e boolean indicating whether to enable or disable
   *
   * Sets the circuit element to be enabled or disabled.
   */
  void   setEnabled (bool e) { MODFLAG (e, CIRCUIT_ENABLED); }
  bool   isVariableSized (void) { return RETFLAG (CIRCUIT_VARSIZE); }
  void   setVariableSized (bool v) { MODFLAG (v, CIRCUIT_VARSIZE); }
  bool   isProbe (void) { return RETFLAG (CIRCUIT_PROBE); }
  void   setProbe (bool p) { MODFLAG (p, CIRCUIT_PROBE); }
  void   setNet (net * n) { subnet = n; }
  net *  getNet (void) { return subnet; }

  // subcircuitry
  std::string getSubcircuit (void) { return subcircuit; }
  void   setSubcircuit (const std::string &);

  // environment specific
  environment * getEnv (void) { return env; }
  void setEnv (environment * e) { env = e; }

  // nodal analyses helpers
  void setInternalVoltageSource (bool i) { MODFLAG (i, CIRCUIT_INTVSOURCE); }
  bool isInternalVoltageSource (void) { return RETFLAG (CIRCUIT_INTVSOURCE); }
  void setVoltageSource (int s) { vsource = s; }
  int  getVoltageSource (void) { return vsource; }
  int  getVoltageSources (void);
  void setVoltageSources (int);
  void voltageSource (int, int, int, nr_double_t value = 0.0);
  bool isVSource (void) { return RETFLAG (CIRCUIT_VSOURCE); }
  void setVSource (bool v) { MODFLAG (v, CIRCUIT_VSOURCE); }
  bool isISource (void) { return RETFLAG (CIRCUIT_ISOURCE); }
  void setISource (bool i) { MODFLAG (i, CIRCUIT_ISOURCE); }
  int  getNoiseSources (void);
  void setNoiseSources (int);

  // transient analyses helpers
  void transientCapacitance (int, int, int, nr_double_t, nr_double_t,
			     nr_double_t);
  void transientCapacitance (int, int, nr_double_t, nr_double_t, nr_double_t);
  void transientCapacitanceQ (int, int, int, nr_double_t);
  void transientCapacitanceQ (int, int, nr_double_t);
  void transientCapacitanceC (int, int, int, int, nr_double_t, nr_double_t);
  void transientCapacitanceC (int, int, nr_double_t, nr_double_t);
  void transientCapacitanceC2V (int, int, int, nr_double_t, nr_double_t);
  void transientCapacitanceC2Q (int, int, int, nr_double_t, nr_double_t);
  void setDelta (nr_double_t * d) { deltas = d; }
  nr_double_t * getDelta (void) { return deltas; }

  // history specific functionality
  bool hasHistory (void) { return RETFLAG (CIRCUIT_HISTORY); }
  void setHistory (bool h) { MODFLAG (h, CIRCUIT_HISTORY); }
  void initHistory (nr_double_t);
  void deleteHistory (void);
  void truncateHistory (nr_double_t);
  void appendHistory (int, nr_double_t);
  void applyHistory (history *);
  nr_double_t getV (int, nr_double_t);
  nr_double_t getV (int, int);
  nr_double_t getJ (int, nr_double_t);
  nr_double_t getHistoryAge (void);
  void setHistoryAge (nr_double_t);
  int getHistorySize (void);
  nr_double_t getHistoryTFromIndex (int);

  // s-parameter helpers
  int  getPort (void) { return pacport; }
  void setPort (int p) { pacport = p; }
  int  getInserted (void) { return inserted; }
  void setInserted (int i) { inserted = i; }
  bool isOriginal (void) { return RETFLAG (CIRCUIT_ORIGINAL); }
  void setOriginal (bool o) { MODFLAG (o, CIRCUIT_ORIGINAL); }

  // microstrip helpers
  substrate * getSubstrate (void);
  void setSubstrate (substrate *);

  // matrix entry modificators
  nr_complex_t getS (int, int);
  nr_complex_t getN (int, int);
  nr_complex_t getY (int, int);
  nr_complex_t getB (int, int);
  nr_complex_t getC (int, int);
  nr_complex_t getD (int, int);
  nr_complex_t getQV (int, int);
  nr_complex_t getGV (int);
  nr_complex_t getCV (int);
  nr_complex_t getE (int);
  nr_complex_t getI (int);
  nr_complex_t getJ (int);
  nr_complex_t getV (int);
  nr_complex_t getQ (int);
  nr_double_t getG (int, int);
  void setS (int, int, nr_complex_t);
  void setN (int, int, nr_complex_t);
  void setY (int, int, nr_complex_t);
  void setB (int, int, nr_complex_t);
  void setC (int, int, nr_complex_t);
  void setD (int, int, nr_complex_t);
  void setQV (int, int, nr_complex_t);
  void setGV (int, nr_complex_t);
  void setCV (int, nr_complex_t);
  void setE (int, nr_complex_t);
  void setI (int, nr_complex_t);
  void setJ (int, nr_complex_t);
  void setV (int, nr_complex_t);
  void setQ (int, nr_complex_t);
  void setG (int, int, nr_double_t);
  void clearB (void);
  void clearC (void);
  void clearD (void);
  void clearE (void);
  void clearI (void);
  void clearJ (void);
  void clearV (void);
  void clearY (void);
  void addY (int, int, nr_complex_t);
  void addY (int, int, nr_double_t);
  void addI (int, nr_complex_t);
  void addI (int, nr_double_t);

  // operating point functionality
  void        addOperatingPoint (const std::string &name, nr_double_t);
  nr_double_t getOperatingPoint (const std::string &name);
  void        setOperatingPoint (const std::string &name, nr_double_t);
  int         hasOperatingPoint (const std::string &name);
  valuelist<operatingpoint> & getOperatingPoints (void) { return oper; }

  // characteristics functionality
  void        addCharacteristic (const std::string &name, nr_double_t);
  nr_double_t getCharacteristic (const std::string &name);
  void        setCharacteristic (const std::string &name, nr_double_t);
  int         hasCharacteristic (const std::string &name);
  valuelist<characteristic> & getCharacteristics (void) { return charac; }

  // differentiate between linear and non-linear circuits
  void setNonLinear (bool l) { MODFLAG (!l, CIRCUIT_LINEAR); }
  bool isNonLinear (void) { return !RETFLAG (CIRCUIT_LINEAR); }

  // miscellaneous functionality
  void print (void);
  static std::string createInternal (const std::string &, const std::string &);
  void setInternalNode (int, const std::string &);

  // matrix operations
  void   allocMatrixS (void);
  void   allocMatrixN (int sources = 0);
  void   allocMatrixMNA (void);
  void   freeMatrixMNA (void);
  void   allocMatrixHB (void);
  void   freeMatrixHB (void);
  void   setMatrixS (matrix);
  matrix getMatrixS (void);
  void   setMatrixN (matrix);
  matrix getMatrixN (void);
  void   setMatrixY (matrix);
  matrix getMatrixY (void);

  static const nr_double_t z0;

 protected:
  int type;
  int pol;

 private:
  int size;
  int pacport;
  int vsource;
  int vsources;
  int nsources;
  int inserted;
  int flag;
  nr_complex_t * MatrixS;
  nr_complex_t * MatrixN;
  nr_complex_t * MatrixY;
  nr_complex_t * MatrixB;
  nr_complex_t * MatrixC;
  nr_complex_t * MatrixD;
  nr_complex_t * VectorE;
  nr_complex_t * VectorI;
  nr_complex_t * VectorV;
  nr_complex_t * VectorJ;
  nr_complex_t * VectorQ;
  nr_complex_t * MatrixQV;
  nr_complex_t * VectorGV;
  nr_complex_t * VectorCV;
  std::string subcircuit;
  node * nodes;
  substrate * subst;
  valuelist<operatingpoint> oper;
  valuelist<characteristic> charac;
  net * subnet;
  environment * env;
  nr_double_t * deltas;
  int nHistories;
  history * histories;
};

} // namespace qucs

// typedef to make it easier to set up our factory
typedef qucs::circuit *maker_t();
// function typdefs to make it easier to set up our factories
typedef qucs::circuit *creator_t();
typedef struct define_t *defs_t();

// our global factories defined in module.cpp
extern "C" {
 extern std::map<std::string, creator_t *, std::less<std::string> > factorycreate;
 extern std::map<std::string, defs_t *, std::less<std::string> > factorydef;

}

#endif /* __CIRCUIT_H__ */
