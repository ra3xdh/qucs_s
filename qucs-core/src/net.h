/*
 * net.h - net class definitions
 *
 * Copyright (C) 2003, 2004, 2005, 2006, 2007 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __NET_H__
#define __NET_H__

#include <string>
#include "ptrlist.h"

namespace qucs {

class circuit;
class node;
class nodelist;
class nodeset;
class analysis;
class dataset;
class environment;


class net : public object
{
 public:
  net ();
  net (const std::string &);
  net (net &);
  ~net ();
  circuit * getRoot (void) { return root; }
  void setRoot (circuit * c) { root = c; }
  void insertCircuit (circuit *);
  void removeCircuit (circuit *, int dropping = 1);
  int  containsCircuit (circuit *);
  int  checkCircuitChain (void);
  void list (void);
  void reducedCircuit (circuit *);
  node * findConnectedNode (node *);
  node * findConnectedCircuitNode (node *);
  void insertedCircuit (circuit *);
  void insertedNode (node *);
  void insertAnalysis (analysis *);
  void removeAnalysis (analysis *);
  dataset * runAnalysis (int &);
  void getDroppedCircuits (nodelist * nodes = NULL);
  void deleteUnusedCircuits (nodelist * nodes = NULL);
  int  getPorts (void) { return nPorts; }
  int  getReduced (void) { return reduced; }
  void setReduced (int r) { reduced = r; }
  int  getVoltageSources (void) { return nSources; }
  void setVoltageSources (int n) { nSources = n; }
  analysis * findAnalysis (const std::string &) const;
  analysis * findAnalysis (int);
  analysis * findSecondOrder (void);
  analysis * getChildAnalysis (analysis *);
  const char * getChild (analysis *) const;
  void orderAnalysis (void);
  analysis * findLastOrder (analysis *);
  ptrlist<analysis> * findLastOrderChildren (analysis *);
  void sortChildAnalyses (analysis *);
  int  containsAnalysis (analysis *, int);
  environment * getEnv (void) { return env; }
  void setEnv (environment * e) { env = e; }
  int  countPorts (void);
  int  countNodes (void);
  int  isNonLinear (void);
  void addNodeset (nodeset *);
  void delNodeset (void);
  nodeset * getNodeset (void) { return nset; }
  void setSrcFactor (nr_double_t f) { srcFactor = f; }
  nr_double_t getSrcFactor (void) { return srcFactor; }
  void setActionNetAll(net *);

 private:
  nodeset * nset;
  circuit * drop;
  circuit * root;
  ptrlist<analysis> * actions;
  ptrlist<analysis> * orgacts;
  environment * env;
  int nPorts;
  int nSources;
  int nCircuits;
  int reduced;
  int inserted;
  int insertedNodes;
  nr_double_t srcFactor;
};

} // namespace qucs

#endif /* __NET_H__ */
