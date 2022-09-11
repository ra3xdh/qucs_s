/*
 * netdefs.h - netlist definitions for arbitrary netlists
 *
 * Copyright (C) 2004, 2007, 2008 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __NETDEFS_H__
#define __NETDEFS_H__

namespace qucs {
class environment;
}

/* Representation of a node list. */
struct node_t {
  char * node;
  char * xlate;
  int xlatenr;
  struct node_t * next;
};

/* Representation of a value. */
struct value_t {
  char * ident;
  char * unit;
  char * scale;
  double value;
  int var;
  int subst;
  int hint;
  int range;
  struct value_t * next;
};

/* Representation of a key / value pair. */
struct pair_t {
  char * key;
  struct value_t * value;
  struct pair_t * next;
};

/* Representation of a definition line in the netlist. */
struct definition_t {
  char * type;
  char * instance;
  struct node_t * nodes;
  struct pair_t * pairs;
  struct definition_t * next;
  struct definition_t * sub;
  qucs::environment * env;
  void * eqns;
  int duplicate;
  int action;
  int substrate;
  int nonlinear;
  int nodeset;
  int line;
  int copy;
  int ncount;
  char * text;
  char * subcircuit;
  struct value_t * values;
  struct define_t * define;
};

// Structure defining a key value pair.
struct property_t {
  const char * key; // key name
  int type;         // type of property
  struct {
    nr_double_t d;  // default value
    const char * s; // default string
  } defaultval;
  struct {
    char il;        // interval boundary
    nr_double_t l;  // lower bound of the value
    nr_double_t h;  // upper bound of the value
    char ih;        // interval boundary
    const char * str[8]; // possible string list
  } range;
};

// Structure defining an available component type.
struct define_t {
  const char * type;            // component name
  int nodes;                    // number of nodes
  int action;                   // is that an action?
  int substrate;                // is that a substrate?
  int nonlinear;                // is the component linear?
  struct property_t * required; // required properties
  struct property_t * optional; // optional proberties
};

// Maximum number of S-parameter ports.
#define MAX_PORTS 256

#define PROP_ACTION       1
#define PROP_COMPONENT    0
#define PROP_SUBSTRATE    1
#define PROP_NO_SUBSTRATE 0
#define PROP_NONLINEAR    1
#define PROP_LINEAR       0
#define PROP_NODES        -1
#define PROP_NO_PROP      { NULL, PROP_REAL, { PROP_NO_VAL, PROP_NO_STR }, \
                            PROP_NO_RANGE }
#define PROP_NO_VAL       0.0
#define PROP_NO_STR       ((char *) -1)
#define PROP_NO_RANGE     { '.', 0, 0, '.', { NULL } }
#define PROP_VAL_MAX      std::numeric_limits<nr_double_t>::max()
#define PROP_VAL_MIN      std::numeric_limits<nr_double_t>::min()
#define PROP_POS_RANGE    { '[', 0, 0, '.', { NULL } }
#define PROP_NEG_RANGE    { '.', 0, 0, ']', { NULL } }
#define PROP_POS_RANGEX   { ']', 0, 0, '.', { NULL } }
#define PROP_NEG_RANGEX   { '.', 0, 0, '[', { NULL } }
#define PROP_MIN_VAL(k)   { '[', k, 0, '.', { NULL } }
#define PROP_MAX_VAL(k)   { '.', 0, k, ']', { NULL } }
#define PROP_MIN_VALX(k)  { ']', k, 0, '.', { NULL } }
#define PROP_MAX_VALX(k)  { '.', 0, k, '[', { NULL } }
#define PROP_RNG_X01I     { ']', 0, 1, ']', { NULL } }
#define PROP_RNGII(f,t)   { '[', f, t, ']', { NULL } }
#define PROP_RNGXI(f,t)   { ']', f, t, ']', { NULL } }
#define PROP_RNGIX(f,t)   { '[', f, t, '[', { NULL } }
#define PROP_RNGXX(f,t)   { ']', f, t, '[', { NULL } }

#define PROP_RNG_STR1(s) \
  { '.', 0, 0, '.', { s, NULL } }
#define PROP_RNG_STR2(s1,s2) \
  { '.', 0, 0, '.', { s1, s2, NULL } }
#define PROP_RNG_STR3(s1,s2,s3) \
  { '.', 0, 0, '.', { s1, s2, s3, NULL } }
#define PROP_RNG_STR4(s1,s2,s3,s4) \
  { '.', 0, 0, '.', { s1, s2, s3, s4, NULL } }
#define PROP_RNG_STR5(s1,s2,s3,s4,s5) \
  { '.', 0, 0, '.', { s1, s2, s3, s4, s5, NULL } }
#define PROP_RNG_STR6(s1,s2,s3,s4,s5,s6) \
  { '.', 0, 0, '.', { s1, s2, s3, s4, s5, s6, NULL } }
#define PROP_RNG_STR7(s1,s2,s3,s4,s5,s6,s7) \
  { '.', 0, 0, '.', { s1, s2, s3, s4, s5, s6, s7, NULL } }

#define PROP_RNG_YESNO    PROP_RNG_STR2 ("yes", "no")
#define PROP_RNG_BJT      PROP_RNG_STR2 ("npn", "pnp")
#define PROP_RNG_FET      PROP_RNG_STR2 ("nfet", "pfet")
#define PROP_RNG_MOS      PROP_RNG_STR2 ("nmos", "pmos")
#define PROP_RNG_TYP      PROP_RNG_STR4 ("lin", "log", "list", "const")
#define PROP_RNG_SOL \
  PROP_RNG_STR5 ("CroutLU", "DoolittleLU", "HouseholderQR", \
		 "HouseholderLQ", "GolubSVD")
#define PROP_RNG_DIS \
  PROP_RNG_STR7 ("Kirschning", "Kobayashi", "Yamashita", "Getsinger", \
		 "Schneider", "Pramanick", "Hammerstad")
#define PROP_RNG_MOD \
  PROP_RNG_STR3 ("Wheeler", "Schneider", "Hammerstad")

#define PROP_NONE         -1
#define PROP_INT          0
#define PROP_REAL         1
#define PROP_STR          2
#define PROP_LIST         3

#define PROP_IS_PROP(prop)   ((prop).key != NULL)
#define PROP_IS_VAL(prop)    ((prop).type != PROP_STR)
#define PROP_IS_INT(prop)    ((prop).type == PROP_INT)
#define PROP_IS_STR(prop)    (!PROP_IS_VAL (prop))
#define PROP_IS_LST(prop)    ((prop).type == PROP_LIST)
#define PROP_HAS_RANGE(prop) ((prop).range.l != 0 || (prop).range.h != 0 || \
                              (prop).range.il != '.' || (prop).range.ih != '.')
#define PROP_HAS_STR(prop)   ((prop).range.str[0] != NULL)

#define PROP_REQ static struct property_t required
#define PROP_OPT static struct property_t optional
#define PROP_DEF required, optional

#define create_definition() \
  ((struct definition_t *) calloc (sizeof (struct definition_t), 1))
#define create_value() \
  ((struct value_t *) calloc (sizeof (struct value_t), 1))
#define create_node() \
  ((struct node_t *) calloc (sizeof (struct node_t), 1))
#define create_pair() \
  ((struct pair_t *) calloc (sizeof (struct pair_t), 1))

#endif /* __NETDEFS_H__ */
