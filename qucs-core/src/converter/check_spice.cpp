/*
 * check_spice.cpp - checker for a Spice netlist
 *
 * Copyright (C) 2004-2011 Stefan Jahn <stefan@lkcc.org>
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <assert.h>
#include <float.h>
#include <ctype.h>

#if defined(_WIN32) & not defined(__MINGW32__)
#define strcasecmp stricmp
#endif

#include "constants.h"
#include "check_spice.h"
#include "qucs_producer.h"
#include "hash.h"

/* Global definitions for parser and checker. */
struct definition_t * definition_root = NULL;
struct definition_t * subcircuit_root = NULL;
struct definition_t * device_root = NULL;
int spice_errors = 0;
char * spice_title = NULL;

// List of available Spice component properties.
static struct property_t spice_noprops[] = { PROP_NO_PROP };

static struct property_t req_spice_R[] = {
  { "R", PROP_REAL, { 50, PROP_NO_STR }, PROP_NO_RANGE }, PROP_NO_PROP };
static struct property_t opt_spice_R[] = {
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };

static struct property_t req_spice_L[] = {
  { "L", PROP_REAL, { 1e-9, PROP_NO_STR }, PROP_NO_RANGE }, PROP_NO_PROP };

static struct property_t req_spice_C[] = {
  { "C", PROP_REAL, { 1e-12, PROP_NO_STR }, PROP_NO_RANGE }, PROP_NO_PROP };

static struct property_t req_spice_V[] = {
  { "U", PROP_REAL, { 1, PROP_NO_STR }, PROP_NO_RANGE }, PROP_NO_PROP };

static struct property_t req_spice_I[] = {
  { "I", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_NO_RANGE }, PROP_NO_PROP };

static struct property_t req_spice_G[] = {
  { "G", PROP_REAL, { 1, PROP_NO_STR }, PROP_NO_RANGE }, PROP_NO_PROP };

static struct property_t req_spice_E[] = {
  { "G", PROP_REAL, { 1, PROP_NO_STR }, PROP_NO_RANGE }, PROP_NO_PROP };

static struct property_t req_spice_F[] = {
  { "G", PROP_REAL, { 1, PROP_NO_STR }, PROP_NO_RANGE }, PROP_NO_PROP };

static struct property_t req_spice_H[] = {
  { "G", PROP_REAL, { 1, PROP_NO_STR }, PROP_NO_RANGE }, PROP_NO_PROP };

static struct property_t req_spice_T[] = {
  { "Z0", PROP_REAL, { 50, PROP_NO_STR }, PROP_POS_RANGE }, PROP_NO_PROP };

static struct property_t req_spice_NODESET[] = {
  { "U", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE }, PROP_NO_PROP };

static struct property_t req_spice_IC[] = {
  { "U", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE }, PROP_NO_PROP };

// List of available Spice components.
struct define_t spice_definition_available[] =
{
  /* resistor */
  { "R", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR,
    req_spice_R, opt_spice_R },
  /* inductor */
  { "L", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR,
    req_spice_L, spice_noprops },
  /* capacitor */
  { "C", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR,
    req_spice_C, spice_noprops },
  /* voltage sources */
  { "V", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR,
    req_spice_V, spice_noprops },
  /* current sources */
  { "I", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR,
    req_spice_I, spice_noprops },
  /* voltage-controlled current source */
  { "G", 4, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR,
    req_spice_G, spice_noprops },
  /* voltage-controlled voltage source */
  { "E", 4, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR,
    req_spice_E, spice_noprops },
  /* current-controlled current source */
  { "F", 4, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR,
    req_spice_F, spice_noprops },
  /* current-controlled voltage source */
  { "H", 4, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR,
    req_spice_H, spice_noprops },
  /* transformer (mutual inductors) */
  { "K", 4, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR,
    spice_noprops, spice_noprops },
  /* BJT device */
  { "Q", 5, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_NONLINEAR,
    spice_noprops, spice_noprops },
  /* MOS device */
  { "M", 4, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_NONLINEAR,
    spice_noprops, spice_noprops },
  /* JFET device */
  { "J", 3, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_NONLINEAR,
    spice_noprops, spice_noprops },
  /* diodes */
  { "D", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_NONLINEAR,
    spice_noprops, spice_noprops },
  /* relais */
  { "S", 4, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR,
    spice_noprops, spice_noprops },
  /* lossless transmission line */
  { "T", 4, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR,
    req_spice_T, spice_noprops },
  /* transient analysis */
  { "TRAN", 0, PROP_ACTION, PROP_NO_SUBSTRATE, PROP_LINEAR,
    spice_noprops, spice_noprops },
  /* AC analysis */
  { "AC", 0, PROP_ACTION, PROP_NO_SUBSTRATE, PROP_LINEAR,
    spice_noprops, spice_noprops },
  /* DC analysis */
  { "DC", 0, PROP_ACTION, PROP_NO_SUBSTRATE, PROP_LINEAR,
    spice_noprops, spice_noprops },
  /* operating point analysis */
  { "OP", 0, PROP_ACTION, PROP_NO_SUBSTRATE, PROP_LINEAR,
    spice_noprops, spice_noprops },
  /* subcircuit instance */
  { "X", PROP_NODES, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR,
    spice_noprops, spice_noprops },
  /* subcircuit definition */
  { "SUBCKT", PROP_NODES, PROP_ACTION, PROP_NO_SUBSTRATE, PROP_LINEAR,
    spice_noprops, spice_noprops },
  /* nodeset functionality */
  { "NODESET", 1, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR,
    req_spice_NODESET, spice_noprops },
  /* nodeset functionality */
  { "IC", 1, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR,
    req_spice_IC, spice_noprops },

  /* end of list */
  { NULL, 0, 0, 0, 0, spice_noprops, spice_noprops },
};

// Include also the (generated) Qucs definitions.
#include "qucsdefs.h"

// Short definition for iterating a list of values.
#define foreach_value(values,val) \
  for ((val) = (values); (val) != NULL; (val) = (val)->next) \
    if (!((val)->hint & HINT_DONE))

/* The function reverses the order of the given value list and returns
   the reversed list. */
struct value_t * netlist_reverse_values (struct value_t * values) {
  struct value_t * root, * next;
  for (root = NULL; values != NULL; values = next) {
    next = values->next;
    values->next = root;
    root = values;
  }
  return root;
}

/* The function reverses the order of the given pair list and returns
   the reversed list. */
struct pair_t * netlist_reverse_pairs (struct pair_t * pairs) {
  struct pair_t * root, * next;
  for (root = NULL; pairs != NULL; pairs = next) {
    next = pairs->next;
    pairs->next = root;
    root = pairs;
  }
  return root;
}

/* The function reverses the order of the given node list and returns
   the reversed list. */
struct node_t * netlist_reverse_nodes (struct node_t * nodes) {
  struct node_t * root, * next;
  for (root = NULL; nodes != NULL; nodes = next) {
    next = nodes->next;
    nodes->next = root;
    root = nodes;
  }
  return root;
}

// The function appends lists of pairs and returns the resulting list.
struct pair_t *
netlist_append_pairs (struct pair_t * p1, struct pair_t * p2) {
  if (p1 == NULL) return p2;
  struct pair_t * pair;
  for (pair = p1; pair->next != NULL; pair = pair->next) ;
  pair->next = p2;
  return p1;
}

// The function appends lists of values and returns the resulting list.
struct value_t *
netlist_append_values (struct value_t * v1, struct value_t * v2) {
  if (v1 == NULL) return v2;
  struct value_t * val;
  for (val = v1; val->next != NULL; val = val->next) ;
  val->next = v2;
  return v1;
}

// The function appends lists of nodes and returns the resulting list.
struct node_t *
netlist_append_nodes (struct node_t * n1, struct node_t * n2) {
  if (n1 == NULL) return n2;
  struct node_t * node;
  for (node = n1; node->next != NULL; node = node->next) ;
  node->next = n2;
  return n1;
}

/* This function goes through the list of available definitions and
   checks whether the given component type can be found. */
static struct define_t * spice_find_definition (const char * n) {
  struct define_t * def;
  for (def = spice_definition_available; def->type != NULL; def++)
    if (!strcasecmp (n, def->type)) return def;
  return NULL;
}

/* The function creates a single translated spice node. */
static struct node_t * spice_translate_node (char * node) {
  struct node_t * n = create_node ();
  if (!strcmp (node, "0")) { // translate ground node
    n->node = strdup (qucs_gnd);
  }
  else {                     // translated other node than ground
    n->node = (char *) malloc (5 + strlen (node));
    strcpy (n->node, "_net");
    strcat (n->node, node);
    // strip off invalid characters
    for (unsigned int i = 0; i < strlen (n->node); i++) {
      if (!isalpha (n->node[i]) && !isdigit (n->node[i])) {
	switch (n->node[i]) {
	case '+': n->node[i] = 'P'; break;
	case '-': n->node[i] = 'N'; break;
	default : n->node[i] = '_'; break;
	}
      }
    }
  }
  return n;
}

/* The function marks the given value to be processed and free()'s
   unnecessary memory. */
static void spice_value_done (struct value_t * val) {
  if (val->ident) { free (val->ident); val->ident = NULL; }
  if (val->unit)  { free (val->unit);  val->unit = NULL;  }
  if (val->scale) { free (val->scale); val->scale = NULL; }
  val->hint |= HINT_DONE;
}

/* The function creates the nodes list for the given component. */
static struct node_t * spice_get_nodes (struct definition_t * def) {

  // has this component actually any nodes to define?
  int nodes = def->define->nodes;
  if (nodes == 0) return NULL;

  // go through value list and try identifying nodes
  struct value_t * val;
  struct node_t * root = NULL;
  int i = 0;
  foreach_value (def->values, val) {
    if (val->hint & HINT_NODE) {
      struct node_t * n = spice_translate_node (val->ident);
      n->next = root;
      root = n;
      spice_value_done (val);
      // continue until all nodes are done
      if (++i >= nodes && nodes != PROP_NODES) break;
    }
    // nodes are defined successively
    else break;
  }
  // return reversed list of nodes
  return netlist_reverse_nodes (root);
}

/* Evaluates the unit scale in a property value.  It adjusts the
   actual value and omits the scale.  The function returns NULL if
   there is no valid scale found and otherwise the corrected scale.
   The given endptr is set to the character after the last valid scale
   character. */
static const char *
spice_evaluate_scale (char * value, char ** endptr, double * factor) {
  const char * scale = NULL;
  *factor = 1.0;
  switch (*value) {
  case 'T': case 't':
    value++; scale = "T"; break;
  case 'G': case 'g':
    value++; scale = "G"; break;
  case 'M': case 'm':
    value++; scale = (char *) ((*value == 'M') ? "M" : "m");
    if ((value[0] == 'i' || value[0] == 'I') &&
	(value[1] == 'l' || value[1] == 'L')) {
      value += 2; *factor = 2.54e-5; scale = NULL;
    }
    if ((value[0] == 'e' || value[0] == 'E') &&
	(value[1] == 'g' || value[1] == 'G')) {
      value += 2; scale = "M";
    }
    break;
  case 'k': case 'K':
    value++; scale = "k"; break;
  case 'u': case 'U':
    value++; scale = "u"; break;
  case 'n': case 'N':
    value++; scale = "n"; break;
  case 'p': case 'P':
    value++; scale = "p"; break;
  case 'f': case 'F':
    value++; scale = "f"; break;
  }
  *endptr = value;
  return scale;
}

/* This little function takes the given string value, converts it into
   an appropriate real value and save optional scale and unit. */
static struct value_t * spice_create_value (const char * ident) {
  struct value_t * val;
  char * end;
  const char  * str;
  double value = 1.0;
  val = create_value ();
  val->value = strtod (ident, &end);
  if (*end) {
    str = spice_evaluate_scale (end, &end, &value);
    val->value *= value;
    val->scale = str ? strdup (str) : NULL;
    if (*end) val->unit = strdup (end);
  }
  return val;
}

/* The function identifies key/value pairs in the value list of the
   given definition and saves these into an appropriate list. */
static struct pair_t * spice_get_pairs (struct definition_t * def) {
  struct value_t * val;
  struct pair_t * p, * root = NULL;
  struct property_t * prop;
  int i = 0;

  // if there is a value given and no description (key), it is required
  foreach_value (def->values, val) {
    prop = &def->define->required[i];
    // a float given ?
    if (val->hint & HINT_NUMBER && prop->key) {
      p = create_pair ();
      p->key = strdup (prop->key);
      p->value = spice_create_value (val->ident);
      p->next = root;
      root = p;
      spice_value_done (val);
      i++;
    }
    // skip identifier if next is a float again (F and H sources)
    else if ((def->type[0] == 'F' || def->type[0] == 'H') &&
	     strcasecmp (val->ident, "POLY") &&
	     val->hint & HINT_NAME &&
	     val->next && val->next->hint & HINT_NUMBER)
      continue;
    // break it here
    else
      break;
  }
  // other key/value pairs on that line
  foreach_value (def->values, val) {
    if (val->hint & HINT_PAIR) {
      p = create_pair ();
      p->key = strdup (val->ident);
      p->value = spice_create_value (val->unit);
      p->next = root;
      root = p;
      spice_value_done (val);
    }
  }
  return netlist_reverse_pairs (root);
}

/* The function goes through the list of definitions and tries to find
   the given device model.  It returns NULL if there is no such
   model. */
static struct definition_t *
spice_find_device (struct definition_t * root, char * type) {
  for (struct definition_t * def = root; def != NULL; def = def->next) {
    if (def->action && !strcasecmp (def->type, "MODEL") &&
	!strcasecmp (def->instance, type)) {
      return def;
    }
  }
  return NULL;
}

/* Looks for the first possible .MODEL or any other device reference
   instance name in the given list.  Returns NULL if there is no such
   thing. */
static struct value_t *
spice_find_device_instance (struct definition_t * def) {
  struct value_t * val;
  foreach_value (def->values, val) {
    if (val->hint & HINT_NAME) return val;
  }
  return NULL;
}

// Little helper structure for device translations.
struct spice_device_t {
  const char * type;            // Spice type
  const char * trans_type;      // Qucs type
  const char * trans_type_prop; // value of 'Type' in Qucs
}
spice_devices[] = {
  { "NPN",     "BJT",    "npn"  },
  { "PNP",     "BJT",    "pnp"  },
  { "NJF",     "JFET",   "nfet" },
  { "PJF",     "JFET",   "pfet" },
  { "NMOS",    "MOSFET", "nfet" },
  { "PMOS",    "MOSFET", "pfet" },
  { "D",       "Diode",  NULL   },
  { "SW",      "Relais", NULL   },
  { "VSWITCH", "Relais", NULL   },
  { "RES",     "R",      NULL   },
  { "R",       "R",      NULL   },
  { "C",       "C",      NULL   },
  { NULL, NULL, NULL }
};

/* The function goes through the given value list and creates an
   appropriate list of key/value pairs stored inside .MODEL
   specifications. */
static struct pair_t * spice_generate_Model_pairs (struct value_t * values) {
  struct pair_t * root = NULL;
  struct value_t * val;
  foreach_value (values, val) {
    if (val->hint & HINT_PAIR) {
      struct pair_t * p = create_pair ();
      p->key = strdup (val->ident);
      p->value = spice_create_value (val->unit);
      p->next = root;
      root = p;
      if (val->hint & HINT_MSTOP) break;
    }
    else break;
  }
  return netlist_reverse_pairs (root);
}

/* The function goes through the property list of the given definition
   and checks whether there is the given property stored.  It returns
   NULL if not. */
static struct pair_t *
spice_find_property (struct definition_t * def, const char * prop) {
  struct pair_t * pair;
  for (pair = def->pairs; pair != NULL; pair = pair->next) {
    if (!strcmp (pair->key, prop))
      return pair;
  }
  return NULL;
}

/* This function is the case-insensitive version of the above. */
static struct pair_t *
spice_find_property_nocase (struct definition_t * def, const char * prop) {
  struct pair_t * pair;
  for (pair = def->pairs; pair != NULL; pair = pair->next) {
    if (!strcasecmp (pair->key, prop))
      return pair;
  }
  return NULL;
}

/* The function starts at a given property pair and is otherwise
   similar to the above function. */
static struct pair_t *
spice_find_property_nocase (struct pair_t * pair, const char * prop) {
  for (; pair != NULL; pair = pair->next) {
    if (!strcasecmp (pair->key, prop))
      return pair;
  }
  return NULL;
}

/* This function looks whether the given property name is stored
   within the given list of values and returns it.  If there is no
   such name, then it returns NULL. */
static struct value_t *
spice_find_property (struct value_t * values, const char * prop) {
  struct value_t * val;
  foreach_value (values, val) {
    if (!strcasecmp (prop, val->ident)) return val;
  }
  return NULL;
}

/* The function replaces or appends the given key/value pair to the
   list of properties of the given definition. */
static void
spice_set_property_string (struct definition_t * def, const char * key,
			   const char * val) {
  struct pair_t * prop = spice_find_property (def, key);
  if (prop != NULL) {
    free (prop->value->ident);
    prop->value->ident = strdup (val);
  }
  else {
    prop = create_pair ();
    prop->key = strdup (key);
    prop->value = create_value ();
    prop->value->ident = strdup (val);
    def->pairs = netlist_append_pairs (prop, def->pairs);
  }
}

/* This function evaluates the already translated value (translated
   scale value) and returns the actual value leaving it untouched. */
static double spice_evaluate_value (struct value_t * value) {
  double val = value->value, factor = 1.0;
  if (value->scale != NULL) {
    switch (*(value->scale)) {
    case 'T': factor = 1e12;  break;
    case 'G': factor = 1e9;   break;
    case 'M': factor = 1e6;   break;
    case 'k': factor = 1e3;   break;
    case 'm': factor = 1e-3;  break;
    case 'u': factor = 1e-6;  break;
    case 'n': factor = 1e-9;  break;
    case 'p': factor = 1e-12; break;
    case 'f': factor = 1e-15; break;
    }
  }
  return val * factor;
}

/* The following function free()'s the given value. */
static void netlist_free_value (struct value_t * value) {
  free (value->ident);
  if (value->unit)  free (value->unit);
  free (value->scale);
  free (value);
}

/* Deletes the given value list. */
static void netlist_free_values (struct value_t * value) {
  struct value_t * next;
  for (; value != NULL; value = next) {
    next = value->next;
    netlist_free_value (value);
  }
}

/* Deletes the given key/value pair. */
void netlist_free_pair (struct pair_t * pair) {
  if (pair->value) netlist_free_values (pair->value);
  free (pair->key);
  free (pair);
}

/* Unchains a property pair and deletes it. */
struct pair_t *
spice_del_property (struct pair_t * root, struct pair_t * pair) {
  struct pair_t * prev;
  if (pair == root) {
    root = pair->next;
    netlist_free_pair (pair);
  }
  else {
    // find previous to the candidate to be deleted
    for (prev = root; prev != NULL && prev->next != pair; prev = prev->next) ;
    if (prev != NULL) {
      prev->next = pair->next;
      netlist_free_pair (pair);
    }
  }
  return root;
}

/* This function adjusts the given device instance definition using
   the the given device definition. */
static void spice_adjust_device (struct definition_t * def,
				 struct definition_t * trandef) {

  // first find the starting point of the definition
  struct value_t * val, * start = NULL;
  foreach_value (trandef->values, val) {
    if (val->hint & HINT_MSTART) { start = val; break; }
  }
  if (start == NULL) {
    foreach_value (trandef->values, val) {
      if (val->hint & HINT_NAME) { start = val; break; }
    }
  }

  // second: look for an appropriate available translation
  struct spice_device_t * tran;
  for (tran = spice_devices; tran->type; tran++) {
    if (!strcasecmp (tran->type, start->ident)) {
      struct pair_t * p;
      // append properties
      p = spice_generate_Model_pairs (start->next);
      def->pairs = netlist_append_pairs (def->pairs, p);
      // adjust type of device
      free (def->type);
      def->type = strdup (tran->trans_type);
      // append "Type" property
      if (tran->trans_type_prop != NULL) {
        spice_set_property_string (def, "Type", tran->trans_type_prop);
      }
      break;
    }
  }
}

/* The function translates .MODEL specifications in device (MOSFET,
   BJT, etc.)  instances. */
static void spice_translate_device (struct definition_t * root,
				    struct definition_t * def) {
  struct value_t * inst = spice_find_device_instance (def);

  // return if no Model given
  if (inst == NULL) return;

  /* first look for the Model in the local definition root, then in
     the global definition root */
  struct definition_t * tran;
  if ((tran = spice_find_device (root, inst->ident)) == NULL)
    tran = spice_find_device (definition_root, inst->ident);
  // really translate the instance here
  if (tran != NULL) {
    spice_value_done (inst);
    spice_adjust_device (def, tran);
  }
  else if (!strcasecmp (def->type, "C")) {
    // can probably be skipped
  }
  else {
    fprintf (stderr, "spice error, no such .MODEL `%s' found as specified in "
	     "device instance `%s'\n", inst->ident, def->instance);
    spice_errors++;
  }
}

/* Goes through the list of available definition and returns the
   appropriate component definition. */
static struct define_t *
spice_get_qucs_definition (struct definition_t * def) {
  struct define_t * entry;
  for (entry = qucs_definition_available; entry->type != NULL; entry++)
    if (!strcmp (entry->type, def->type)) return entry;
  return NULL;
}

// Helper structure for direct type translations.
struct spice_device_table_t {
  const char * type;  // Spice type
  const char * trans; // Qucs type
}
spice_device_table[] = {
  { "Q", "BJT"    },
  { "J", "JFET",  },
  { "M", "MOSFET" },
  { "D", "Diode"  },
  { "V", "Vdc"    },
  { "I", "Idc"    },
  { "G", "VCCS"   },
  { "E", "VCVS"   },
  { "F", "CCCS"   },
  { "H", "CCVS"   },
  { "K", "Tr"     },
  { "S", "Relais" },
  { "T", "TLIN4P" },
  { NULL, NULL }
};

/* This little function translates the Spice type definitions into Qucs
   types. */
static int spice_translate_type (struct definition_t * def) {
  struct spice_device_table_t * tran;
  int found = 0;
  for (tran = spice_device_table; tran->type && def->type; tran++) {
    found = 1;
    if (!strcasecmp (tran->type, def->type)) {
      if (!spice_find_property (def->values, "POLY")) {
	free (def->type);
	def->type = strdup (tran->trans);
	found++;
	break;
      }
    }
  }
  return found;
}

// Helper structure for node translations.
struct node_translation_t {
  const char * type;    // the type of definition
  int pass;             // pass number when the translation should be performed
  int Mapping[6];       // node ordering
  int Default[6];       // default nodes
}
node_translations[] = {
  { "BJT", 0,
    { 2, 1, 3, 4, -1 },
    { 1, 2, 3, 0, -1 }
  },
  { "MOSFET", 0,
    { 2, 1, 3, 4, -1 },
    { 1, 2, 3, 3, -1 }
  },
  { "JFET", 0,
    { 2, 1, 3, -1 },
    { 1, 2, 3, -1 }
  },
  { "Diode", 0,
    { 2, 1, -1 },
    { 1, 2, -1 }
  },
  { "Iac", 0,
    { 2, 1, -1 },
    { 1, 2, -1 }
  },
  { "Idc", 0,
    { 2, 1, -1 },
    { 1, 2, -1 }
  },
  { "VCCS", 0,
    { 3, 1, 2, 4, -1 },
    { 1, 2, 3, 4, -1 }
  },
  { "VCVS", 0,
    { 3, 1, 2, 4, -1 },
    { 1, 2, 3, 4, -1 }
  },
  { "CCCS", 1,
    { 3, 1, 2, 4, -1 },
    { 1, 2, 3, 4, -1 }
  },
  { "CCVS", 1,
    { 3, 1, 2, 4, -1 },
    { 1, 2, 3, 4, -1 }
  },
  { "Tr", 1,
    { 3, 1, 2, 4, -1 },
    { 1, 2, 3, 4, -1 }
  },
  { "NodeSet", 1,
    { 1, -1 },
    { 1, -1 }
  },
  { "Relais", 0,
    { 3, 1, 2, 4, -1 },
    { 1, 2, 3, 4, -1 }
  },
  { "TLIN4P", 0,
    { 1, 3, 4, 2, -1 },
    { 1, 2, 3, 4, -1 }
  },
  { NULL, 0, { -1 }, { -1 } }
};

// Counts the number of nodes stored in the given definition.
static int spice_count_nodes (struct definition_t * def) {
  int res = 0;
  for (struct node_t * node = def->nodes; node != NULL; node = node->next)
    res++;
  return res;
}

// Deletes node list of a definition.
static void netlist_free_nodes (struct node_t * node) {
  struct node_t * n;
  for (; node != NULL; node = n) {
    n = node->next;
    free (node->node);
    free (node);
  }
}

/* Deletes pair list of a definition. */
static void netlist_free_pairs (struct pair_t * pair) {
  struct pair_t * n;
  for (; pair != NULL; pair = n) {
    n = pair->next;
    netlist_free_pair (pair);
  }
}

// The function returns the node specified at the given position.
static struct node_t * spice_get_node (struct definition_t * def, int pos) {
  struct node_t * node;
  int i;
  for (i = 1, node = def->nodes; node != NULL; node = node->next, i++)
    if (i == pos) return node;
  return NULL;
}

/* This function transforms the collected node information in the
   Spice list into the appropriate Qucs definition. */
static void spice_translate_nodes (struct definition_t * def, int pass) {
  struct node_translation_t * nodes;
  struct node_t * node;
  // find node translator
  for (nodes = node_translations; nodes->type != NULL; nodes++)
    if (!strcmp (nodes->type, def->type)) break;
  if (nodes->type == NULL || pass != nodes->pass) return;

  struct define_t * entry = spice_get_qucs_definition (def);
  // adjust the number of nodes and append default nodes
  if (entry->nodes > spice_count_nodes (def)) {
    for (int n = spice_count_nodes (def); n < entry->nodes; n++) {
      node = create_node ();
      if (nodes->Default[n] == 0) { // default is the ground node
	node->node = strdup (qucs_gnd);
      }
      else {                        // default is some other node
	struct node_t * t = spice_get_node (def, nodes->Default[n]);
	if (t != NULL)
	  node->node = strdup (t->node);
	else {
	  // no node given, occurs in device descriptions
	  char txt[16];
	  sprintf (txt, "%s%d", "_node", n);
	  node->node = strdup (txt);
	}
      }
      def->nodes = netlist_append_nodes (def->nodes, node);
    }
  }
  // remap the node definitions if necessary
  struct node_t * root = NULL;
  for (int n = entry->nodes - 1; n >= 0; n--) {
    struct node_t * t = spice_get_node (def, nodes->Mapping[n]);
    node = create_node ();
    node->node = strdup (t->node);
    node->next = root;
    root = node;
  }
  netlist_free_nodes (def->nodes);
  def->nodes = root;
}

// Helper structure for unit translations.
struct unit_translation_t {
  const char * key;
  const char * trans;
}
unit_translations[] = {
  { "OHM",  "Ohm" },
  { "OHMS", "Ohm" },
  { "MHO",  "S" },
  { "S",    "s" },
  { "H",    "H" },
  { "F",    "F" },
  { "HZ",   "Hz" },
  { "V",    "V" },
  { "VOLT", "V" },
  { "A",    "A" },
  { "M",    "m" },
  { NULL, NULL }
};

/* This function tries to figure out units given in the Spice list and
   translates these patterns into valid Qucs units. */
static void spice_translate_units (struct definition_t * def) {
  for (struct pair_t * pair = def->pairs; pair != NULL; pair = pair->next) {
    struct value_t * value = pair->value;
    if (value->unit) {
      struct unit_translation_t * unit;
      int found = 0;
      for (unit = unit_translations; unit->key != NULL; unit++) {
	if (!strcasecmp (unit->key, value->unit)) {
	  free (value->unit);
	  value->unit = strdup (unit->trans);
	  found++;
	  break;
	}
      }
      if (!found) { // no such unit found
	free (value->unit);
	value->unit = NULL;
      }
    }
  }
}

/* This function appends all the required Qucs properties not given in
   the Spice netlist. */
static void spice_adjust_default_properties (struct definition_t * def) {
  struct define_t * entry = spice_get_qucs_definition (def);
  if (entry == NULL) return;

  // handle required properties only
  for (int i = 0; PROP_IS_PROP (entry->required[i]); i++) {
    struct property_t * prop = &entry->required[i];
    if (spice_find_property (def, prop->key) == NULL) {
      struct pair_t * pair = create_pair ();
      pair->key = strdup (prop->key);
      pair->value = create_value ();
      if (PROP_IS_VAL (*prop)) {
	pair->value->value = prop->defaultval.d;
      }
      else {
	pair->value->ident = strdup (prop->defaultval.s);
      }
      def->pairs = netlist_append_pairs (def->pairs, pair);
    }
  }
}

/* This function appends the optional Qucs properties not given in the
   Spice netlist. */
static void spice_adjust_optional_properties (struct definition_t * def) {
  struct define_t * entry = spice_get_qucs_definition (def);
  if (entry == NULL) return;

  // handle optional properties if requested
  for (int i = 0; PROP_IS_PROP (entry->optional[i]); i++) {
    struct property_t * prop = &entry->optional[i];
    if (spice_find_property (def, prop->key) == NULL) {
      struct pair_t * pair = create_pair ();
      pair->key = strdup (prop->key);
      pair->value = create_value ();
      if (PROP_IS_VAL (*prop)) {
	pair->value->value = prop->defaultval.d;
      }
      else {
	pair->value->ident = strdup (prop->defaultval.s);
      }
      def->pairs = netlist_append_pairs (def->pairs, pair);
    }
  }
}

// Helper structure for property translations and aliases in devices.
struct property_translation_t {
  const char * type;
  const char * key;
  const char * trans;
}
property_translations[] = {
  /* BJT device */
  { NULL, "CCS", "Cjs" },
  { NULL, "VA",  "Vaf" },
  { NULL, "VB",  "Var" },
  { NULL, "IK",  "Ikf" },
  { NULL, "PE",  "Vje" },
  { NULL, "ME",  "Mje" },
  { NULL, "PC",  "Vjc" },
  { NULL, "MC",  "Mjc" },
  { NULL, "PS",  "Vjs" },
  { NULL, "MS",  "Mjs" },
  /* MOSFET device */
  { NULL, "VTO", "Vt0" },
  { NULL, "U0",  "Uo"  },
  /* DIODE device */
  { NULL, "CJO", "Cj0" },
  /* OTHER devices */
  { "C",  "IC",  "V"   },
  { "L",  "IC",  "I"   },
  { NULL, "Z0",  "Z"   },
  { "R",  "TC",  "Tc1" },
  /* END of list */
  { NULL, NULL,  NULL  }
};

/* The function translates property and aliases of devices in the list
   of key/value pairs of the given definition. */
void spice_adjust_alias_properties (struct definition_t * def,
				    struct pair_t * pair) {
  struct property_translation_t * prop = property_translations;
  for (; prop->key != NULL; prop++) {
    if (!prop->type || !strcmp (prop->type, def->type)) {
      if (!strcasecmp (prop->key, pair->key)) {
	free (pair->key);
	pair->key = strdup (prop->trans);
      }
    }
  }
}

/* The function goes through the pair (property) list of the given
   definition and adjusts the spelling of the property keys according
   to the Qucs definitions. */
void spice_adjust_properties (struct definition_t * def) {
  struct define_t * entry = spice_get_qucs_definition (def);
  if (entry) {
    struct pair_t * pair;
    for (pair = def->pairs; pair != NULL; pair = pair->next) {
      int i, found = 0;
      // handle required properties
      for (i = 0; PROP_IS_PROP (entry->required[i]); i++) {
	if (!strcasecmp (entry->required[i].key, pair->key)) {
	  free (pair->key);
	  pair->key = strdup (entry->required[i].key);
	  found++;
	  break;
	}
      }
      // handle optional properties
      for (i = 0; PROP_IS_PROP (entry->optional[i]); i++) {
	if (!strcasecmp (entry->optional[i].key, pair->key)) {
	  free (pair->key);
	  pair->key = strdup (entry->optional[i].key);
	  found++;
	  break;
	}
      }
      // some other direct translations
      if (!found) spice_adjust_alias_properties (def, pair);
    }
  }
}

/* This function appends the given key/value pair to the given
   definition.  If the replace flag is non-zero the pair gets
   replaced.  If the replace flag is zero and the pair already exists
   nothing is done here. */
static void spice_append_pair (struct definition_t * def, const char * prop,
			       const char * value, int replace) {
  struct pair_t * p = spice_find_property (def, prop);
  if (p != NULL) {
    if (replace) {
      netlist_free_value (p->value);
      p->value = spice_create_value (value);
      return;
    }
    else return;
  }
  p = create_pair ();
  p->key = strdup (prop);
  p->value = spice_create_value (value);
  def->pairs = netlist_append_pairs (def->pairs, p);
}

/* The function appends the given node name to the list of nodes of
   the given definition. */
static void spice_append_node (struct definition_t * def, char * node) {
  struct node_t * n = create_node ();
  n->node = strdup (node);
  def->nodes = netlist_append_nodes (def->nodes, n);
}

// Converts the given string into upper case.
static char * spice_toupper (char * str) {
  for (unsigned int i = 0; i < strlen (str); i++) {
    if (str[i] >= 'a' && str[i] <= 'z') str[i] = toupper (str[i]);
  }
  return str;
}

/* The function adjusts the instance name of the given component
   definition. */
static void spice_adjust_instance (struct definition_t * def) {
  spice_toupper (def->instance);
}

/* This function is used by the overall netlist translator in order to
   fixup the properties, nodes, instance name and property units of
   the given definition. */
static void spice_fixup_definition (struct definition_t * def) {
  spice_adjust_instance (def);
  spice_adjust_properties (def);
  spice_translate_nodes (def, 0);
  spice_translate_units (def);
  spice_adjust_default_properties (def);
}

/* The function adds the given definition to the list of valid
   definitions.  The definition is placed at the beginning of the
   list. */
static struct definition_t *
spice_add_definition (struct definition_t * root, struct definition_t * def) {
  spice_fixup_definition (def);
  def->next = root;
  return def;
}

/* Forward declaration for recursive calls. */
static void netlist_destroy_intern (struct definition_t *);

/* Deletes a definition and all its content. */
static void
netlist_free_definition (struct definition_t * def) {
  netlist_free_nodes (def->nodes);
  netlist_free_pairs (def->pairs);
  netlist_free_values (def->values);
  free (def->subcircuit);
  free (def->text);
  if (def->sub) netlist_destroy_intern (def->sub);
  free (def->type);
  free (def->instance);
  free (def);
}

/* The function deletes the given definition list. */
static void netlist_destroy_intern (struct definition_t * root) {
  struct definition_t * def, * next;
  for (def = root; def != NULL; def = next) {
    next = def->next;
    netlist_free_definition (def);
  }
}

/* Deletes all available definition lists. */
void spice_destroy (void) {
  netlist_destroy_intern (definition_root);
  netlist_destroy_intern (device_root);
  for (struct definition_t * def = subcircuit_root; def; def = def->next) {
    netlist_destroy_intern (def->sub);
  }
  netlist_destroy_intern (subcircuit_root);
  definition_root = subcircuit_root = device_root = NULL;
  netlist_free_nodes (spice_nodes);
  spice_nodes = NULL;
  free (spice_title);
  spice_title = NULL;
}

/* Unchains a definition and deletes it. */
struct definition_t *
spice_del_definition (struct definition_t * root, struct definition_t * def) {
  struct definition_t * prev;
  if (def == root) {
    root = def->next;
    netlist_free_definition (def);
  }
  else {
    // find previous to the candidate to be deleted
    for (prev = root; prev != NULL && prev->next != def; prev = prev->next) ;
    if (prev != NULL) {
      prev->next = def->next;
      netlist_free_definition (def);
    }
  }
  return root;
}

/* This function creates an internal node name used during the
   conversion process.  The returned string is static and must be
   copied by the caller.  Successive calls result in different node
   names. */
static char * spice_create_intern_node (void) {
  static int intern = 0;
  static char txt[32];
  sprintf (txt, "_cnet%d", intern++);
  return txt;
}

/* This function places an internal node in between the given
   components.  The first node of the 'dn' component becomes the first
   node of the 'up' component. */
static void spice_adjust_vsource_nodes (struct definition_t * up,
					struct definition_t * dn) {
  struct node_t * node = spice_get_node (dn, 1);
  spice_append_node (up, node->node);
  char * inode = spice_create_intern_node ();
  spice_append_node (up, inode);
  free (node->node);
  node->node = strdup (inode);
}

/* The function copies both of the nodes from the 'right' component
   into the 'left' component. */
static void spice_adjust_isource_nodes (struct definition_t * left,
					struct definition_t * right) {
  struct node_t * node;
  node = spice_get_node (right, 1);
  spice_append_node (left, node->node);
  node = spice_get_node (right, 2);
  spice_append_node (left, node->node);
}

/* The function creates a new definition based upon the given
   definition.  The type of the new component must be given as
   well. */
static struct definition_t *
spice_create_definition (struct definition_t * base, const char * type) {
  struct definition_t * res = create_definition ();
  res->type = strdup (type);
  res->action = PROP_COMPONENT;
  res->instance = strdup (base->instance);
  res->define = base->define;
  return res;
}

/* The function returns the actual property value stored in the given
   definition of the given property.  If there is no such property it
   returns 0. */
static double
spice_get_property_value (struct definition_t * def, const char * key) {
  struct pair_t * prop = spice_find_property (def, key);
  return prop ? spice_evaluate_value (prop->value) : 0;
}

/* This function replaces or appends the given key/value pair in the
   given definition. */
static void
spice_set_property_value (struct definition_t * def, const char * key,
			  double val) {
  struct pair_t * prop = spice_find_property (def, key);
  if (prop != NULL) {
    // just replace the key/value pair
    if (prop->value->scale) {
      free (prop->value->scale);
      prop->value->scale = NULL;
    }
    if (prop->value->unit) {
      free (prop->value->unit);
      prop->value->unit = NULL;
    }
    prop->value->value = val;
  }
  else {
    // create key/value pair and append it
    prop = create_pair ();
    prop->key = strdup (key);
    prop->value = create_value ();
    prop->value->value = val;
    def->pairs = netlist_append_pairs (def->pairs, prop);
  }
}

// Helper structure for property extraction routine.
struct property_field_t {
  const char * field[16];
};

/* The function extracts a list of properties from the given value
   list and assign the property names stored in the field extractor
   structure.  The extracted properties are saved into the given
   definition. */
static void spice_extract_properties (struct definition_t * def,
				      struct value_t * values,
				      struct property_field_t * field) {
  int i = 0;
  struct value_t * val;
  // go through each not yet processed value
  foreach_value (values, val) {
    if (field->field[i] == NULL) break; // stop at the end
    if (val->hint & HINT_NAME) {
      spice_set_property_string (def, field->field[i], val->ident);
    }
    else if (val->hint & (HINT_NODE | HINT_NUMBER)) {
      spice_append_pair (def, field->field[i], val->ident, 0);
    }
    else break;
    spice_value_done (val);
    if (val->hint & HINT_MSTOP) break; // stop here if necessary
    i++;
  }
}

/* This function tries to reproduce a Spice netlist line.  It is used
   if no proper translation could be found for the line and is printed
   as commentary line then. */
static char * spice_untranslated_text (struct definition_t * def) {
  struct value_t * val;
  char str[256];
  sprintf (str, "%s%s:%s ", def->action ? "." : "", def->type, def->instance);
  char * txt = (char *) malloc (strlen (str) + 1);
  strcpy (txt, str);
  for (val = def->values; val != NULL; val = val->next) {
    if (val->hint == HINT_NODE) {
      sprintf (str, "%s ", val->ident);
    }
    else if (val->hint & (HINT_NODE | HINT_NUMBER)) {
      sprintf (str, "%s%s ", val->ident, val->hint & HINT_MSTOP ? ")" : "");
    }
    else if (val->hint & HINT_PAIR) {
      sprintf (str, "%s%s=%s%s ", val->hint & HINT_MSTART ? "(" : "",
	       val->ident, val->unit, val->hint & HINT_MSTOP ? ")" : "");
    }
    else if (val->hint & HINT_NAME) {
      sprintf (str, "%s%s%s%s",
	       (val->hint & HINT_MSTART) ? " " : "",
	       val->ident,
	       ((val->hint & HINT_MSTART) && val->next &&
	        (val->next->hint & HINT_MSTOP)) ||
	       (val->hint & HINT_MSTOP) ? "" : " ",
	       (val->hint & HINT_MSTART && val->next) ? "(" :
	       (val->hint & HINT_MSTOP) ? ")" : " ");
    }
    txt = (char *) realloc (txt, strlen (txt) + strlen (str) + 1);
    strcat (txt, str);
  }
  return spice_toupper (txt);
}

#define VAL_IS_NUMBER(val) \
  ((val) != NULL && \
  (val)->hint & (HINT_NUMBER | HINT_NODE) && \
  !((val)->hint & HINT_DONE))

#define VAL_IS_DONE(val) \
  ((val) == NULL || (val)->hint & HINT_DONE)

/* The function counts values in a property list up to a stop
   value. */
static int spice_count_real_values (struct value_t * values) {
  int ret = 0;
  struct value_t * val;
  // go through each not yet processed value
  foreach_value (values, val) {
    if (val->hint & (HINT_NODE | HINT_NUMBER))
      ret++;
    if (val->hint & HINT_MSTOP) break; // stop here if necessary
  }
  return ret;
}

/* This function is the independent source translator.  If necessary
   new kinds of sources are created.  This must be done since Qucs has
   separate sources for each type of excitation and Spice summarizes
   these voltage/current excitations in a single source. */
static struct definition_t *
spice_translate_source (struct definition_t * root,
			struct definition_t * def, char type) {
  struct definition_t * ac = NULL, * dc = def, * pulse = NULL, * expo = NULL;
  struct value_t * prop;
  char * ui = (char *) ((type == 'U') ? "U" : "I");

  // adjust the DC part of the source
  if ((prop = spice_find_property (dc->values, "DC")) != NULL) {
    spice_value_done (prop);
    prop = prop->next;
    if (VAL_IS_NUMBER (prop)) {
      spice_append_pair (def, ui, prop->ident, 1);
      spice_value_done (prop);
    }
  }
  free (dc->type);
  dc->type = strdup (type == 'U' ? "Vdc" : "Idc");

  // adjust the sinusoidal part of the source
  if ((prop = spice_find_property (dc->values, "SIN")) != NULL) {
    ac = spice_create_definition (dc, type == 'U' ? "Vac" : "Iac");
    spice_value_done (prop);
    prop = prop->next;
    if (VAL_IS_NUMBER (prop)) {
      // adjust DC offset
      nr_double_t off;
      off = spice_get_property_value (dc, ui);
      spice_append_pair (dc, ui, prop->ident, 1);
      off += spice_get_property_value (dc, ui);
      spice_set_property_value (dc, ui, off);
      spice_value_done (prop);
      // extract frequency, phase and theta
      prop = prop->next;
      struct property_field_t field =
      { { ui, "f", "Phase", "Theta", NULL } };
      spice_extract_properties (ac, prop, &field);
    }
    double v, f;
    f = spice_get_property_value (ac, "f");
    v = spice_get_property_value (ac, "Phase") * f * -360.0;
    spice_set_property_value (ac, "Phase", v);
    v = spice_get_property_value (ac, "Theta") / f;
    spice_set_property_value (ac, "Theta", f > 0 ? v : 0);
  }

  // adjust the AC part of the source
  if ((prop = spice_find_property (dc->values, "AC")) != NULL) {
    char * Mag = NULL, * Phase = NULL;
    spice_value_done (prop);
    prop = prop->next;
    if (VAL_IS_NUMBER (prop)) {
      Mag = strdup (prop->ident);
      spice_value_done (prop);
      prop = prop->next;
      if (VAL_IS_NUMBER (prop)) {
	Phase = strdup (prop->ident);
	spice_value_done (prop);
      }
    }
    if (ac == NULL) {
      ac = spice_create_definition (dc, type == 'U' ? "Vac" : "Iac");
    }
    if (Mag) {
      spice_append_pair (ac, ui, Mag, 1);
      free (Mag);
    }
    else {
      spice_append_pair (ac, ui, "0", 0);
    }
    if (Phase) {
      spice_append_pair (ac, "Phase", Phase, 1);
      free (Phase);
    }
  }

  // adjust the pulse part of the source
  if ((prop = spice_find_property (dc->values, "PULSE")) != NULL) {
    pulse = spice_create_definition (dc, type == 'U' ? "Vpulse" : "Ipulse");
    spice_value_done (prop);
    prop = prop->next;

    // periodic
    if (spice_count_real_values (prop) > 6) {
      free (pulse->type);
      pulse->type = type == 'U' ? strdup ("Vrect") : strdup ("Irect");
      double add, off = 0;
      if (VAL_IS_NUMBER (prop)) {
	add = spice_get_property_value (dc, ui);
	spice_append_pair (dc, ui, prop->ident, 1);
	off = spice_get_property_value (dc, ui);
	add += off;
	spice_set_property_value (dc, ui, add);
	prop = prop->next;
      }
      if (VAL_IS_NUMBER (prop)) {
	struct property_field_t field =
	  { { ui, "Td", "Tr", "Tf", "TH", "TL", NULL } };
	spice_extract_properties (pulse, prop, &field);
      }
      double v;
      v  = spice_get_property_value (pulse, ui);   // V2
      v -= off;
      spice_set_property_value (pulse, ui, v);
      v  = spice_get_property_value (pulse, "TH"); // PW
      v += spice_get_property_value (pulse, "Tr");
      spice_set_property_value (pulse, "TH", v);
      v  = spice_get_property_value (pulse, "TL"); // PER
      v -= spice_get_property_value (pulse, "TH");
      spice_set_property_value (pulse, "TL", v);
    }
    // single pulse
    else {
      if (VAL_IS_NUMBER (prop)) {
	spice_append_pair (pulse, type == 'U' ? "U1" : "I1", prop->ident, 0);
	spice_append_pair (dc, ui, "0", 0);
	spice_value_done (prop);
	prop = prop->next;
	struct property_field_t field =
	  { { type == 'U' ? "U2" : "I2", "T1", "Tr", "Tf", "T2", NULL } };
	spice_extract_properties (pulse, prop, &field);
      }
      double v;
      v  = spice_get_property_value (pulse, "T1");
      v += spice_get_property_value (pulse, "Tr");
      v += spice_get_property_value (pulse, "Tf");
      v += spice_get_property_value (pulse, "T2");
      spice_set_property_value (pulse, "T2", v);
    }
  }

  // adjust the exponential part of the source
  if ((prop = spice_find_property (dc->values, "EXP")) != NULL) {
    expo = spice_create_definition (dc, type == 'U' ? "Vexp" : "Iexp");
    spice_value_done (prop);
    prop = prop->next;
    // extract pulse values, rise and fall constants
    struct property_field_t field =
      { { type == 'U' ? "U1" : "I1", type == 'U' ? "U2" : "I2", "T1", "Tr",
	  "T2", "Tf", NULL } };
    spice_extract_properties (expo, prop, &field);
  }

  // set DC value to zero if necessary
  if (spice_find_property (dc, ui) == NULL) {
    spice_append_pair (def, ui, "0", 1);
  }

  // finally add sources to list of definitions
  if (ac) {
    if (type == 'U')
      spice_adjust_vsource_nodes (ac, dc);
    else
      spice_adjust_isource_nodes (ac, dc);
    root = spice_add_definition (root, ac);
  }
  if (pulse) {
    if (type == 'U')
      spice_adjust_vsource_nodes (pulse, ac ? ac : dc);
    else
      spice_adjust_isource_nodes (pulse, ac ? ac : dc);
    root = spice_add_definition (root, pulse);
  }
  if (expo) {
    if (type == 'U')
      spice_adjust_vsource_nodes (expo, pulse ? pulse : ac ? ac : dc);
    else
      spice_adjust_isource_nodes (expo, pulse ? pulse : ac ? ac : dc);
    root = spice_add_definition (root, expo);
  }
  return root;
}

/* This little function returns a static string containing an instance
   name of a parameter sweep.  Successive calls produces different
   names. */
static char * spice_create_intern_para (void) {
  static int intern = 1;
  static char txt[32];
  sprintf (txt, "SW%d", intern++);
  return txt;
}

/* The function is used to translate the lin/log10/log8 sweeps into an
   appropriate sweep in Qucs.  Depending on the given arguments it
   computes the number of points in a sweep and stores the new type of
   sweep in the given type variable. */
static int spice_evaluate_points (char ** type, double start, double stop,
				  double points) {
  int ret = 1;
  if (!strcasecmp (*type, "dec")) {      // logarithmic
    ret = (int) ((log10 (stop) - log10 (start)) * points);
    free (*type);
    *type = strdup ("log");
  }
  else if (!strcasecmp (*type, "lin")) { // linear
    ret = (int) points;
    free (*type);
    *type = strdup ("lin");
  }
  else if (!strcasecmp (*type, "oct")) { // octaves
    ret = (int) (((log10 (stop) - log10 (start)) / log10 (8.0)) * points);
    free (*type);
    *type = strdup ("log");
  }
  return ret;
}

/* This function creates a new parameter sweep (for DC analysis) and
   adjust all properties except the 'Sim' property which must be
   assigned after calling this function. */
static struct definition_t * spice_create_para (struct definition_t * base) {
  struct property_field_t props =
  { { "Param", "Start", "Stop", "Points", NULL } };
  struct definition_t * para = create_definition ();

  para->type = strdup ("SW");
  para->action = PROP_ACTION;
  para->instance = strdup (spice_create_intern_para ());
  para->define = base->define;
  spice_extract_properties (para, base->values, &props);
  spice_set_property_string (para, "Type", "lin");
  double v;
  v  = spice_get_property_value (para, "Stop");
  v -= spice_get_property_value (para, "Start");
  v /= spice_get_property_value (para, "Points");
  v += 1;
  spice_set_property_value (para, "Points", v);

  return para;
}

/* This little helper returns the number of not yet processed values
   in th given value list. */
static int spice_count_values (struct value_t * values) {
  int res = 0; struct value_t * val;
  foreach_value (values, val) res++;
  return res;
}

/* The function looks through the list of definitions if there is a
   component with the given type and instance names and returns it.
   Otherwise the function returns NULL. */
static struct definition_t *
spice_find_definition (struct definition_t * root, const char * type,
		       char * inst) {
  for (struct definition_t * def = root; def != NULL; def = def->next) {
    if (!strcasecmp (def->type, type) && !strcasecmp (def->instance, inst))
      return def;
  }
  return NULL;
}

/* The function looks through the list of definitions if there is a
   component with the given type and returns it.  Otherwise the
   function returns NULL. */
static struct definition_t *
spice_find_definition (struct definition_t * root, const char * type) {
  for (struct definition_t * def = root; def != NULL; def = def->next) {
    if (!strcasecmp (def->type, type))
      return def;
  }
  return NULL;
}

/* The function appends the given key/value pair to the properties of
   any definition of the given type. */
static void spice_add_property (struct definition_t * root, const char * type,
				const char * key, char * value) {
  for (struct definition_t * def = root; def != NULL; def = def->next) {
    if (!strcmp (def->type, type))
      spice_append_pair (def, key, value, 0);
  }
}

/* The following little function removes the value list of the given
   definition entirely if all values are marked to be processed. */
static void spice_free_values (struct definition_t * def) {
  if (spice_count_values (def->values) <= 0) {
    struct value_t * val, * next;
    for (val = def->values; val != NULL; val = next) {
      next = val->next;
      netlist_free_value (val);
    }
    def->values = NULL;
  }
}

/* Spice node lists. */
struct node_t * spice_nodes = NULL;

/* Collects the nodes specified in the PLOT and PRINT statement
   marking them as potential "external" nodes. */
static void spice_collect_external_nodes (struct definition_t * def) {
  struct value_t * val;
  foreach_value (def->values, val) {
    if (val->hint & HINT_MSTART && val->next && val->ident[0] == 'V') {
      struct node_t * n = spice_translate_node (val->next->ident);
      if (!qucs_find_node (spice_nodes, n->node)) {
	n->next = spice_nodes;
	spice_nodes = n;
      }
      else netlist_free_nodes (n);
    }
  }
}

/* This function goes through the list of device descriptions and
   checks whether the given model is already in the list.  Returns
   NULL if there is no such model. */
static struct definition_t * spice_find_Model (char * instance) {
  struct definition_t * dev;
  for (dev = device_root; dev; dev = dev->next) {
    if (!strcmp (dev->instance, instance))
      return dev;
  }
  return NULL;
}

/* The function appends a new device model.  It creates the actual
   model description and adjusts the nodes and optional/required
   properties. */
static struct definition_t * spice_add_Model (struct definition_t * def) {
  struct definition_t * Model;
  if (strcasecmp (def->type, "MODEL"))
    return NULL;
  if (spice_find_Model (def->instance))
    return NULL;
  Model = create_definition ();
  Model->action = PROP_COMPONENT;
  Model->instance = strdup (def->instance);
  Model->define = def->define;
  spice_adjust_device (Model, def);
  if (spice_translate_type (Model)) {
    spice_fixup_definition (Model);
    spice_adjust_optional_properties (Model);
    Model->next = device_root;
    device_root = Model;
  }
  else {
    free (Model->instance);
    free (Model);
    Model = NULL;
  }
  return Model;
}

/* Finds a second coupled inductor definition using the same inductors
   as the original one if there is any. */
static struct definition_t *
spice_find_coupled (struct definition_t * root, struct definition_t * coupled,
		    const char * type, char * inst) {
  for (struct definition_t * def = root; def != NULL; def = def->next) {
    if (def != coupled && !strcmp (def->type, type) && !def->action) {
      if (VAL_IS_DONE (def->values) || VAL_IS_DONE (def->values->next))
	continue;
      char * linst1 = def->values->ident;
      char * linst2 = def->values->next->ident;
      if (!strcasecmp (linst1, inst) || !strcasecmp (linst2, inst))
	return def;
    }
  }
  return NULL;
}

/* Looks for a mutual inductor instance referencing the two given
   inductors and returns it. */
static struct definition_t *
spice_find_coupled (struct definition_t * root, const char * type,
		    char * inst1, char * inst2) {
  for (struct definition_t * def = root; def != NULL; def = def->next) {
    if (!strcmp (def->type, type) && !def->action) {
      if (VAL_IS_DONE (def->values) || VAL_IS_DONE (def->values->next))
	continue;
      char * linst1 = def->values->ident;
      char * linst2 = def->values->next->ident;
      if ((!strcasecmp (linst1, inst1) && !strcasecmp (linst2, inst2)) ||
	  (!strcasecmp (linst1, inst2) && !strcasecmp (linst2, inst1)))
	return def;
    }
  }
  return NULL;
}

/* Looks for the inductor definition used in a coupled inductor.
   Emits an error message if there is no such inductor. */
static struct definition_t *
spice_find_coupled_inductor (struct definition_t * root,
			     struct definition_t * def, const char * type,
			     char * inst) {
  static struct definition_t * target;
  target = spice_find_definition (root, type, inst);
  if (target == NULL) {
    fprintf (stderr, "spice error, no such inductor `%s' found as "
	     "referenced by %s `%s'\n", inst, def->type, def->instance);
    spice_errors++;
  }
  return target;
}

// Get the coefficient of coupling.
static struct value_t *
spice_get_value_coupled (struct definition_t * def) {
  struct value_t * val = NULL;
  foreach_value (def->values, val) {
    if (val->hint & HINT_NUMBER) break;
  }
  return val;
}

// Generates a coupled inductor instance description.
static char *
spice_coupled_instance (struct definition_t * k1, struct definition_t * k2,
			struct definition_t * k3) {
  char * inst = (char *) malloc (strlen (k1->instance) +
				 strlen (k2->instance) +
				 strlen (k3->instance) + 1);
  strcpy (inst, k1->instance);
  strcat (inst, k2->instance);
  strcat (inst, k3->instance);
  return inst;
}

/* Post translation function for coupled inductors. */
static struct definition_t *
spice_translate_coupled (struct definition_t * root,
			 struct definition_t * def) {
  struct definition_t * target1, * target2, * target3;
  char * linst1 = def->values->ident;       // get inductivity 1
  char * linst2 = def->values->next->ident; // get inductivity 2
  nr_double_t l1, l2, l3, k, t;
  char * n1, * n2, * n3, * n4;
  struct node_t * nn;

  // initialize local variables
  n1 = n2 = n3 = n4 = NULL;
  l1 = l2 = l3 = k = t = 0;

  // find and handle inductivity 1
  target1 = spice_find_coupled_inductor (root, def, "L", linst1);

  // find and handle inductivity 2
  target2 = spice_find_coupled_inductor (root, def, "L", linst2);

  // if both inductors found
  if (!target1 || !target2) return root;

  l1 = spice_get_property_value (target1, "L");
  l2 = spice_get_property_value (target2, "L");

  // check three inductors
  struct definition_t * k1 = def, * k2, * k3;
  char * k12, * k13, * k23;
  k2 = spice_find_coupled (root, k1, "Tr", linst1);
  k3 = spice_find_coupled (root, k1, "Tr", linst2);

  // handle three inductors
  if (k2 != NULL && k3 != NULL) {
    char * linst3, * linst4;
    if (!strcasecmp (k2->values->ident, linst1))
      linst3 = k2->values->next->ident;
    else
      linst3 = k2->values->ident;
    if (!strcasecmp (k3->values->ident, linst2))
      linst4 = k3->values->next->ident;
    else
      linst4 = k3->values->ident;
    spice_value_done (k1->values);
    spice_value_done (k1->values->next);

    if (strcasecmp (linst3, linst4)) {
      fprintf (stderr, "spice error, cannot translate coupled inductors "
	       "`%s' and `%s'\n", linst3, linst4);
      spice_errors++;
    }
    else if (k2 != k3) {
      // find and handle inductivity 3
      target3 = spice_find_coupled_inductor (root, k2, "L", linst3);
      if (target3 != NULL) {
	// construct three mutual inductors
	struct value_t * val;
	char * inst = spice_coupled_instance (k1, k2, k3);
	free (k1->type); k1->type = strdup ("MUT2");
	free (k1->instance); k1->instance = inst;
	netlist_free_pairs (k1->pairs); k1->pairs = NULL;
	spice_value_done (k2->values);
	spice_value_done (k2->values->next);
	spice_value_done (k3->values);
	spice_value_done (k3->values->next);
	spice_append_node (k1, spice_get_node(target1, 1)->node);
	spice_append_node (k1, spice_get_node(target3, 1)->node);
	spice_append_node (k1, spice_get_node(target3, 2)->node);
	spice_append_node (k1, spice_get_node(target2, 2)->node);
	spice_append_node (k1, spice_get_node(target2, 1)->node);
	spice_append_node (k1, spice_get_node(target1, 2)->node);
	l3 = spice_get_property_value (target3, "L");
	spice_set_property_value (k1, "L1", l1);
	spice_set_property_value (k1, "L2", l2);
	spice_set_property_value (k1, "L3", l3);
	if ((val = spice_get_value_coupled (k1)) != NULL) {
	  k12 = val->ident;
	  spice_append_pair (k1, "k12", k12, 0);
	  spice_value_done (val);
	}
	if ((val = spice_get_value_coupled (k2)) != NULL) {
	  k13 = val->ident;
	  spice_append_pair (k1, "k13", k13, 0);
	  spice_value_done (val);
	}
	if ((val = spice_get_value_coupled (k3)) != NULL) {
	  k23 = val->ident;
	  spice_append_pair (k1, "k23", k23, 0);
	  spice_value_done (val);
	}
	root = spice_del_definition (root, target1);
	root = spice_del_definition (root, target2);
	root = spice_del_definition (root, target3);
	root = spice_del_definition (root, k2);
	root = spice_del_definition (root, k3);
	return root;
      }
    }
  }

  spice_value_done (def->values);
  spice_value_done (def->values->next);

  // node replacer 1
  nn = spice_get_node (target1, 2);
  n4 = nn->node;
  nn->node = strdup (spice_create_intern_node ());
  n1 = strdup (nn->node);

  // node replacer 2
  nn = spice_get_node (target2, 2);
  n3 = nn->node;
  nn->node = strdup (spice_create_intern_node ());
  n2 = strdup (nn->node);

  // get the coefficient of coupling
  struct value_t * val;
  spice_append_pair (def, "T", "1", 1);
  if ((val = spice_get_value_coupled (def)) != NULL) {
    spice_append_pair (def, "T", val->ident, 1);
    spice_value_done (val);
  }

  // apply the turns ratio of the transformer and its nodes
  k = spice_get_property_value (def, "T");
  t = sqrt (l1 / l2);
  spice_set_property_value (def, "T", t);
  spice_append_node (def, n1);
  spice_append_node (def, n2);
  spice_append_node (def, n3);
  spice_append_node (def, n4);
  // adapt inductivities of original inductors
  spice_set_property_value (target1, "L", l1 - k * l1);
  spice_set_property_value (target2, "L", l2 - k * l1 / t / t);
  // insert the actual mutual inductance if necessary
  if (k > 0) {
    struct definition_t * Mind = spice_create_definition (def, "L");
    spice_append_node (Mind, n1);
    spice_append_node (Mind, n4);
    spice_set_property_value (Mind, "L", k * l1);
    root = spice_add_definition (root, Mind);
  }
  free (n1); free (n2); free (n3); free (n4);
  return root;
}

/* Finds an additional mutual inductor definition using the same
   inductors as the original one if there is any. */
static struct definition_t *
spice_find_coupled (struct definition_t * root,
		    qucs::hash<struct definition_t> * coupled,
		    const char * type, char * inst) {
  for (struct definition_t * def = root; def != NULL; def = def->next) {
    if (!strcmp (def->type, type) && !def->action) {
      if (VAL_IS_DONE (def->values) || VAL_IS_DONE (def->values->next))
	continue;
      if (!coupled->get (def->instance)) {
	char * linst1 = def->values->ident;
	char * linst2 = def->values->next->ident;
	if (!strcasecmp (linst1, inst) || !strcasecmp (linst2, inst))
	  return def;
      }
    }
  }
  return NULL;
}

/* Looks recursively for mutual inductors.  In the end the two hash
   maps contain all the inductors coupled by those mutual inductors in
   the other hash map. */
static struct definition_t *
spice_search_coupled (struct definition_t * root,
		      qucs::hash<struct definition_t> * K_hash,
		      qucs::hash<struct definition_t> * L_hash,
		      const char * type, char * inst) {
  char * linst;
  struct definition_t * l, * k;

  // find mutual inductors referencing the given inductor instance
  while ((k = spice_find_coupled (root, K_hash, type, inst)) != NULL) {
    // already had this one?
    if (K_hash->get (k->instance))
      continue;
    K_hash->put (k->instance, k);

    // first referenced inductor
    linst = k->values->ident;
    if (!L_hash->get (linst)) {
      if ((l = spice_find_coupled_inductor (root, k, "L", linst)) != NULL) {
	L_hash->put (linst, l);
	// recurse
	root = spice_search_coupled (root, K_hash, L_hash, type, linst);
      }
    }

    // second referenced inductor
    linst = k->values->next->ident;
    if (!L_hash->get (linst)) {
      if ((l = spice_find_coupled_inductor (root, k, "L", linst)) != NULL) {
	L_hash->put (linst, l);
	// recurse
	root = spice_search_coupled (root, K_hash, L_hash, type, linst);
      }
    }
  }
  return root;
}

/* Post translation function for multiple coupled inductors. */
static struct definition_t *
spice_translate_coupled_x (struct definition_t * root,
			   struct definition_t * def) {
  char * linst1, * linst2;
  qucs::hash<struct definition_t> K_hash;
  qucs::hash<struct definition_t> L_hash;
  qucs::hashiterator<struct definition_t> it;
  struct definition_t * l;

  // save first 2 referenced inductors and the mutual inductor
  linst1 = def->values->ident;
  if ((l = spice_find_coupled_inductor (root, def, "L", linst1)) != NULL)
    L_hash.put (linst1, l);
  linst2 = def->values->next->ident;
  if ((l = spice_find_coupled_inductor (root, def, "L", linst2)) != NULL)
    L_hash.put (linst2, l);
  K_hash.put (def->instance, def);

  // look for more mutual inductors involving the first 2 inductors
  root = spice_search_coupled (root, &K_hash, &L_hash, "Tr", linst1);
  root = spice_search_coupled (root, &K_hash, &L_hash, "Tr", linst2);

  // create coupling coefficient matrix
  int i, o, s = L_hash.count ();
  nr_double_t * kval = new nr_double_t[s * s];
  struct definition_t * k;
  struct value_t * val;
  qucs::hashiterator<struct definition_t> iti, ito;
  // outer loop
  for (ito = qucs::hashiterator<struct definition_t> (L_hash), o = 0;
       *ito; ++ito, ++o) {
    // inner loop
    for (iti = qucs::hashiterator<struct definition_t> (L_hash), i = 0;
	 *iti; ++iti, ++i) {
      if (i > o) {
	// cross-coupling coefficients
	nr_double_t kvalue = 0;
	k = spice_find_coupled (root, "Tr",
				iti.currentKey (), ito.currentKey ());
	if (k != NULL) {
	  if ((val = spice_get_value_coupled (k)) != NULL) {
	    kvalue = spice_evaluate_value (val);
	  }
	}
	kval[s * o + i] = kvalue;
	kval[s * i + o] = kvalue;
      }
      else if (i == o) {
	// self-coupling coefficient
	kval[s * o + i] = 1;
      }
    }
  }

  spice_value_done (def->values);
  spice_value_done (def->values->next);

  // adjust MUTX instance
  free (def->type); def->type = strdup ("MUTX");
  netlist_free_pairs (def->pairs); def->pairs = NULL;

  // create L property vector
  struct pair_t * pair = create_pair ();
  pair->key = strdup ("L");
  def->pairs = netlist_append_pairs (def->pairs, pair);
  for (it = qucs::hashiterator<struct definition_t> (L_hash); *it; ++it) {
    spice_append_node (def, spice_get_node(it.currentVal (), 1)->node);
    spice_append_node (def, spice_get_node(it.currentVal (), 2)->node);
    val = create_value ();
    val->value = spice_get_property_value (it.currentVal (), "L");
    pair->value = netlist_append_values (pair->value, val);
    root = spice_del_definition (root, it.currentVal ());
  }

  // create k property vector
  pair = create_pair ();
  pair->key = strdup ("k");
  def->pairs = netlist_append_pairs (def->pairs, pair);
  for (i = 0; i < s * s; i++) {
    val = create_value ();
    val->value = kval[i];
    pair->value = netlist_append_values (pair->value, val);
  }

  // remove remaining coupled inductors from definition list
  for (it = qucs::hashiterator<struct definition_t> (K_hash); *it; ++it) {
    if (def != it.currentVal ()) {
      root = spice_del_definition (root, it.currentVal ());
    }
  }

  delete[] kval;
  return root;
}

/* Constructs an edd equation name. */
static char *
spice_create_eqnstr (struct definition_t * def, int p, char type) {
  char * n = (char *) malloc (strlen (def->instance) + 4 + 3);
  sprintf (n, "%s.%c%d", def->instance, type, p);
  return n;
}

/* The function adds edd equation properties to the given netlist
   definition and also adds the appropriate equation instances. */
static struct definition_t *
spice_add_edd_equation (struct definition_t * root,
			struct definition_t * def, int p,
			struct definition_t ** i, struct definition_t ** q) {
  struct definition_t * ieqn, * qeqn;
  char I_[4], Q_[4];
  char * ieq = spice_create_eqnstr (def, p, 'I');
  char * qeq = spice_create_eqnstr (def, p, 'Q');
  sprintf (I_, "I%d", p);
  sprintf (Q_, "Q%d", p);
  spice_set_property_string (def, Q_, qeq);
  spice_set_property_string (def, I_, ieq);
  ieqn = spice_create_definition (def, "Eqn");
  qeqn = spice_create_definition (def, "Eqn");
  spice_set_property_string (ieqn, "Export", "no");
  spice_set_property_string (qeqn, "Export", "no");
  spice_set_property_string (ieqn, ieq, "0");
  spice_set_property_string (qeqn, qeq, "0");
  root = spice_add_definition (root, qeqn);
  root = spice_add_definition (root, ieqn);
  sprintf (ieq, "Eqn%sI%d", def->instance, p);
  sprintf (qeq, "Eqn%sQ%d", def->instance, p);
  free (ieqn->instance);
  free (qeqn->instance);
  qeqn->instance = strdup (qeq);
  ieqn->instance = strdup (ieq);
  free (ieq);
  free (qeq);
  if (i) (*i) = ieqn;
  if (q) (*q) = qeqn;
  return root;
}

/* Since there no or little documentation about the polynom orders in
   the SPICE2G6 'POLY' statements the following piece of code is a
   straight re-implementation of the Fortran code in SPICE2G6. */
static void spice2g6_nxtpwr (int * seq, int nd) {
  int i, k, ps;

  // special handling of one-dimensional polynoms
  if (nd == 1) {
    seq[0]++;
    return;
  }

  // two and more-dimensional polynoms
  k = nd;
  do {
    if (seq[k - 1] != 0) break;
  }
  while (--k != 0);

  if (k == 0) {
    seq[0]++;
  }
  else if (k != nd) {
    seq[k - 1]--;
    seq[k]++;
  }
  else {
    for (i = 0; i < k - 1; i++)
      if (seq[i] != 0) break;
    if (i == k - 1) {
      seq[0] = seq[nd - 1] + 1;
      seq[nd-1] = 0;
      return;
    }
    ps = 1;
    k = nd - 1;
    while (seq[k - 1] < 1) {
      ps += seq[k];
      seq[k] = 0;
      k--;
    }
    seq[k] += ps;
    seq[k - 1]--;
  }
}

/* The function takes the given spice value, converts it into an
   appropriate real value and save optional scale and unit and finally
   returns the actual value. */
static double spice_get_value (struct value_t * val) {
  const char * str;
  char * end;
  double v;
  val->value = strtod (val->ident, &end);
  if (*end) {
    str = spice_evaluate_scale (end, &end, &v);
    val->value *= v;
    val->scale = str ? strdup (str) : NULL;
    if (*end) val->unit = strdup (end);
  }
  return val->value;
}

/* Creates a 'nd' dimensional polynomial expression extracted from the
   coefficient list of a value. */
static char *
spice_create_poly (struct value_t * prop, int nd, int integrate) {
  struct value_t * val;

  // construct polynomial expression
  int * pn = (int *) calloc (nd, sizeof (int));
  static char expr[1024];
  char value[256];
  strcpy (expr, "");

  // go through spice values
  foreach_value (prop, val) {
    if (!VAL_IS_NUMBER (val)) break;

    double k = spice_get_value (val);
    spice_value_done (val);

    // construct single polynom
    if (k != 0.0) {

      // coefficient
      sprintf (value, "%+g", k);
      strcat (expr, value);

      // remaining polynom
      for (int i = 0; i < nd; i++) {
	int n = integrate ? i + 1 : i + 2;
	int e = integrate ? pn[i] + 2 : pn[i];
	switch (e) {
	case 0:
	  strcpy (value, "");
	  break;
	case 1:
	  sprintf (value, "*V%d", n);
	  break;
	case 2:
	  sprintf (value, "*V%d*V%d", n, n);
	  break;
	default:
	  sprintf (value, "*V%d^%d", n, e);
	  break;
	}
	strcat (expr, value);

	// coefficient correction
	if (integrate && e > 1) {
	  sprintf (value, "/%d", e);
	  strcat (expr, value);
	}
      }
    }

    // prepare next polynom
    spice2g6_nxtpwr (pn, nd);
  }
  free (pn);
  return expr;
}

/* Translates E and G poly sources. */
static struct definition_t *
spice_translate_poly (struct definition_t * root, struct definition_t * def) {
  struct value_t * prop;
  int nd, type = -1;

  // only handle appropriate sources
  if (strcasecmp (def->type, "E") && strcasecmp (def->type, "G") &&
      strcasecmp (def->type, "F") && strcasecmp (def->type, "H"))
    return root;

  // save source type information
  if (!strcasecmp (def->type, "E"))
    type = 0;
  else if (!strcasecmp (def->type, "G"))
    type = 1;
  else if (!strcasecmp (def->type, "H"))
    type = 2;
  else if (!strcasecmp (def->type, "F"))
    type = 3;

  if ((prop = spice_find_property (def->values, "POLY")) != NULL) {
    // retype poly source into EDD
    free (def->type);
    def->type = strdup ("EDD");
    spice_value_done (prop);
    // get number of polynomial terms
    prop = prop->next;
    nd = (int) spice_evaluate_value (prop);
    spice_value_done (prop);

    // construct properties, equations and nodes of the EDD
    if (type <= 1) {
      // handle E and G voltage controlled sources
      prop = prop->next;
      for (int i = nd * 2 - 1; i >= 0; i--) {
	int p = (i + 1) / 2 + 1;
	struct node_t * node = spice_translate_node (prop->ident);
	def->nodes = netlist_append_nodes (def->nodes, node);
	if (i & 1) root = spice_add_edd_equation (root, def, p, NULL, NULL);
	spice_value_done (prop);
	prop = prop->next;
      }
    }
    else {
      // handle F and H current controlled sources
      prop = prop->next;
      for (int i = nd; i > 0; i--) {
	struct definition_t * vdc, * ibuf;
	char * vn, * np, * nn;
	int p = i + 1;
	// find referenced voltage source (where current flows through)
	vn = prop->ident;
	vdc = spice_find_definition (root, "Vdc", vn);
	if (vdc) {
	  // create intermediate current controlled voltage source
	  // passing voltage to an EDD branch
	  ibuf = spice_create_definition (def, "CCVS");
	  vn = (char *) malloc (strlen (ibuf->instance) + 3);
	  sprintf (vn, "%sV%d", ibuf->instance, p);
	  free (ibuf->instance);
	  ibuf->instance = vn;
	  struct node_t * node = spice_get_node (vdc, 1);
	  np = strdup (spice_create_intern_node ());
	  nn = strdup (spice_create_intern_node ());
	  spice_append_node (ibuf, node->node);
	  free (node->node);
	  node->node = strdup (nn);
	  spice_append_node (def, np);
	  spice_append_node (def, (char *) "gnd");
	  spice_append_node (ibuf, np);
	  spice_append_node (ibuf, (char *) "gnd");
	  spice_append_node (ibuf, nn);
	  spice_set_property_string (ibuf, "G", "1");
	  root = spice_add_definition (root, ibuf);
	  free (nn);
	  free (np);
	}
	else {
	  fprintf (stderr, "spice error, no such voltage source `%s' found as "
		   "referenced by the %s `%s' instance\n", def->type,
		   def->instance, vn);
	  spice_errors++;
	}
	root = spice_add_edd_equation (root, def, p, NULL, NULL);
	spice_value_done (prop);
	prop = prop->next;
      }
    }

    // add first (really evaluating) I and Q equations
    struct definition_t * ieqn, * qeqn;
    root = spice_add_edd_equation (root, def, 1, &ieqn, &qeqn);

    // construct polynomial expression
    char * expr = spice_create_poly (prop, nd, 0);

    // replace first current branch to reflect polynom
    char * ieq = spice_create_eqnstr (def, 1, 'I');
    spice_set_property_string (ieqn, ieq, expr);
    free (ieq);

    // finally add buffering controlled source
    struct definition_t * obuf = NULL;
    struct node_t * pnode, * nnode;
    char * intp;

    if ((type & 1) == 0)
      obuf = spice_create_definition (def, "CCVS");
    else if ((type & 1) == 1)
      obuf = spice_create_definition (def, "CCCS");

    pnode = spice_get_node (def, 1);
    nnode = spice_get_node (def, 2);
    intp = strdup (spice_create_intern_node ());

    spice_append_node (obuf, intp);
    spice_append_node (obuf, nnode->node);
    spice_append_node (obuf, pnode->node);
    spice_append_node (obuf, (char *) "gnd");

    free (pnode->node);
    pnode->node = strdup (intp);
    free (nnode->node);
    nnode->node = strdup ("gnd");
    free (intp);

    spice_set_property_string (obuf, "G", "1");
    root = spice_add_definition (root, obuf);
  }
  return root;
}

/* Translates non-linear L and C poly definitions. */
static struct definition_t *
spice_translate_poly_lc (struct definition_t * root,
			 struct definition_t * def) {
  struct value_t * prop;
  int type = -1;
  double lc = 0.0;

  // save type information
  if (!strcasecmp (def->type, "C"))
    type = 0;
  else if (!strcasecmp (def->type, "L"))
    type = 1;

  if ((prop = spice_find_property (def->values, "POLY")) != NULL) {
    // retype poly LC into EDD
    free (def->type);
    def->type = strdup ("EDD");
    spice_value_done (prop);

    // get constant L or C value
    struct pair_t * p;
    if ((p = spice_find_property (def, "C")) != NULL) {
      lc = spice_get_property_value (def, "C");
      def->pairs = spice_del_property (def->pairs, p);
    }
    else if ((p = spice_find_property (def, "L")) != NULL) {
      lc = spice_get_property_value (def, "L");
      def->pairs = spice_del_property (def->pairs, p);
    }

    // add I and Q equations
    struct definition_t * ieqn, * qeqn;
    root = spice_add_edd_equation (root, def, 1, &ieqn, &qeqn);

    // construct polynomial expression
    char * expr1 = strdup (spice_create_poly (prop, 1, 1));
    char * expr2 = expr1;
    if (lc != 0.0) {
      expr2 = (char *) malloc (strlen (expr1) + 256);
      sprintf (expr2, "%+g*V1%s", lc, expr1);
      free (expr1);
    }

    // replace first charge branch to reflect polynom
    char * qeq = spice_create_eqnstr (def, 1, 'Q');
    spice_set_property_string (qeqn, qeq, expr2);
    free (expr2);
    free (qeq);

    // finally add converting gyrator if necessary
    if (type == 1) {
      struct definition_t * gyra = NULL;
      struct node_t * pnode, * nnode;
      char * intp;

      gyra = spice_create_definition (def, "Gyrator");

      pnode = spice_get_node (def, 1);
      nnode = spice_get_node (def, 2);
      intp = strdup (spice_create_intern_node ());

      spice_append_node (gyra, intp);
      spice_append_node (gyra, nnode->node);
      spice_append_node (gyra, pnode->node);
      spice_append_node (gyra, (char *) "gnd");

      free (pnode->node);
      pnode->node = strdup (intp);
      free (nnode->node);
      nnode->node = strdup ("gnd");
      free (intp);

      spice_set_property_string (gyra, "R", "1");
      root = spice_add_definition (root, gyra);
    }
  }
  return root;
}

/* This function must be called after the actual Spice netlist
   translator in order to adjust some references or whatever in the
   resulting netlist. */
static struct definition_t *
spice_post_translator (struct definition_t * root) {
  for (struct definition_t * def = root; def != NULL; def = def->next) {
    // post-process parameter sweep
    if (def->action && !strcmp (def->type, "SW")) {
      // adjust the actual 'Param' name
      struct pair_t * prop = spice_find_property (def, "Param");
      char * val = spice_toupper (prop->value->ident);
      struct definition_t * target;
      // get the target voltage or current source and adjust the property
      target = spice_find_definition (root, "Vdc", val);
      if (target) {
	prop = spice_find_property (target, "U");
	prop->value->ident = strdup (val);
      }
      else {
	target = spice_find_definition (root, "Idc", val);
	if (target) {
	  prop = spice_find_property (target, "I");
	  prop->value->ident = strdup (val);
	}
	else {
	  fprintf (stderr, "spice error, no such source `%s' found as "
		   "referenced by the .DC analysis\n", val);
	  spice_errors++;
	}
      }
    }
    // post-process current-controlled current/voltage source
    if (!def->action && (!strcmp (def->type, "CCCS") ||
			 !strcmp (def->type, "CCVS"))) {
      struct definition_t * target;
      struct value_t * val = spice_find_device_instance (def);
      if (val) {
	char * key = val->ident;
	target = spice_find_definition (root, "Vdc", key);
	if (target) {
	  // adjust the controlling nodes of the source
	  spice_adjust_vsource_nodes (def, target);
	  spice_translate_nodes (def, 1);
	}
	else {
	  fprintf (stderr, "spice error, no such voltage source `%s' found "
		   "as referenced by the %s `%s' instance\n", def->type,
		   def->instance, key);
	  spice_errors++;
	}
      }
    }
    // post-process F and H poly sources
    if (!def->action && (!strcmp (def->type, "F") ||
			 !strcmp (def->type, "H"))) {
      root = spice_translate_poly (root, def);
    }
    // post-process switches
    if (!def->action && !strcmp (def->type, "Relais")) {
      struct pair_t * pon = spice_find_property_nocase (def, "VON");
      struct pair_t * pof = spice_find_property_nocase (def, "VOFF");
      if (pon != NULL && pof != NULL) {
	nr_double_t von = spice_evaluate_value (pon->value);
	nr_double_t vof = spice_evaluate_value (pof->value);
	def->pairs = spice_del_property (def->pairs, pon);
	def->pairs = spice_del_property (def->pairs, pof);
	nr_double_t vh = (von - vof) / 2;
	nr_double_t vt = (von + vof) / 2;
	spice_set_property_value (def, "Vt", vt);
	spice_set_property_value (def, "Vh", fabs (vh));
      }
    }
    // post-process pulse and rectangular sources
    if (!def->action && (!strcmp (def->type, "Vpulse") ||
			 !strcmp (def->type, "Ipulse") ||
			 !strcmp (def->type, "Vrect") ||
			 !strcmp (def->type, "Irect"))) {
      struct definition_t * tran;
      struct pair_t * tr = spice_find_property (def, "Tr");
      struct pair_t * tf = spice_find_property (def, "Tf");
      if (tr == NULL || tf == NULL) {
	if ((tran = spice_find_definition (definition_root, "TR")) != NULL) {
	  nr_double_t start = spice_get_property_value (tran, "Start");
	  nr_double_t stop = spice_get_property_value (tran, "Stop");
	  nr_double_t points = spice_get_property_value (tran, "Points");
	  nr_double_t tstep = (stop - start) / (points - 1);
	  nr_double_t add = 0;
	  if (!tf) {
	    spice_set_property_value (def, "Tf", tstep);
	    add += tstep;
	  }
	  if (!tr) {
	    spice_set_property_value (def, "Tr", tstep);
	    add += tstep;
	  }
	  if (!strcmp (&def->type[1], "pulse")) {
	    nr_double_t t2 = spice_get_property_value (def, "T2");
	    spice_set_property_value (def, "T2", t2 + add);
	  }
	}
      }
    }
    // post-process sinusoidal sources
    if (!def->action && (!strcmp (def->type, "Vac") ||
			 !strcmp (def->type, "Iac"))) {
      struct definition_t * tran;
      struct pair_t * f = spice_find_property (def, "f");
      if (f == NULL) {
	if ((tran = spice_find_definition (definition_root, "TR")) != NULL) {
	  nr_double_t stop = spice_get_property_value (tran, "Stop");
	  spice_set_property_value (def, "f", 1 / stop);
	}
      }
    }
    // post-process resistors
    if (!def->action && !strcmp (def->type, "R")) {
      // drop the second "R" given in Model
      struct pair_t * r1 = spice_find_property_nocase (def, "R");
      struct pair_t * r2 = spice_find_property_nocase (r1->next, "R");
      if (r2 != NULL) {
	def->pairs = spice_del_property (def->pairs, r2);
      }
      // calculate R value
      struct pair_t * L = spice_find_property_nocase (def, "L");
      struct pair_t * W = spice_find_property_nocase (def, "W");
      struct pair_t * R = spice_find_property_nocase (def, "RSH");
      struct pair_t * D = spice_find_property_nocase (def, "DEFW");
      struct pair_t * N = spice_find_property_nocase (def, "NARROW");
      nr_double_t l = 0, w = 0, r = 0, d = 0, n = 0;
      if (L) {
	l = spice_evaluate_value (L->value);
	def->pairs = spice_del_property (def->pairs, L);
      }
      if (W) {
	w = spice_evaluate_value (W->value);
	def->pairs = spice_del_property (def->pairs, W);
      }
      if (R) {
	r = spice_evaluate_value (R->value);
	def->pairs = spice_del_property (def->pairs, R);
      }
      if (D) {
	d = spice_evaluate_value (D->value);
	def->pairs = spice_del_property (def->pairs, D);
      }
      if (N) {
	n = spice_evaluate_value (N->value);
	def->pairs = spice_del_property (def->pairs, N);
      }
      if (d == 0) d = 1e-6;
      if (w == 0) w = d;
      if (l != 0 && w != 0 && r != 0) {
	r = r * (l - n) / (w - n);
	spice_set_property_value (def, "R", r);
      }
      // handle Spice 2g6 syntax
      struct value_t * val;
      foreach_value (def->values, val) {
	if (!(val->hint & HINT_DONE) && (val->hint & HINT_NUMBER)) {
	  spice_append_pair (def, "Tc2", val->ident, 0);
	  spice_value_done (val);
	  break;
	}
      }
    }
    // post-process capacitor
    if (!def->action && !strcmp (def->type, "C")) {
      // calculate C value
      struct pair_t * L = spice_find_property_nocase (def, "L");
      struct pair_t * W = spice_find_property_nocase (def, "W");
      struct pair_t * C = spice_find_property_nocase (def, "CJ");
      struct pair_t * S = spice_find_property_nocase (def, "CJSW");
      struct pair_t * D = spice_find_property_nocase (def, "DEFW");
      struct pair_t * N = spice_find_property_nocase (def, "NARROW");
      nr_double_t l = 0, w = 0, c = 0, d = 0, n = 0, s = 0;
      if (L) {
	l = spice_evaluate_value (L->value);
	def->pairs = spice_del_property (def->pairs, L);
      }
      if (W) {
	w = spice_evaluate_value (W->value);
	def->pairs = spice_del_property (def->pairs, W);
      }
      if (C) {
	c = spice_evaluate_value (C->value);
	def->pairs = spice_del_property (def->pairs, C);
      }
      if (S) {
	s = spice_evaluate_value (S->value);
	def->pairs = spice_del_property (def->pairs, S);
      }
      if (D) {
	d = spice_evaluate_value (D->value);
	def->pairs = spice_del_property (def->pairs, D);
      }
      if (N) {
	n = spice_evaluate_value (N->value);
	def->pairs = spice_del_property (def->pairs, N);
      }
      if (d == 0) d = 1e-6;
      if (w == 0) w = d;
      if (l != 0 && w != 0 && c != 0) {
	c = c * (l - n) * (w - n) + 2 * s * (l + w - 2 * n);
	spice_set_property_value (def, "C", c);
      }
    }
    // post-process lossless transmission line
    if (!def->action && !strcmp (def->type, "TLIN4P")) {
      struct pair_t * pt = spice_find_property (def, "TD");
      struct pair_t * pf = spice_find_property (def, "F");
      struct pair_t * pl = spice_find_property (def, "NL");
      nr_double_t len = 1e-3;
      if (pt != NULL) {
	// delay given
	len = spice_evaluate_value (pt->value) * C0;
	def->pairs = spice_del_property (def->pairs, pt);
	spice_set_property_value (def, "L", len);
      }
      else if (pf != NULL && pl != NULL) {
	// frequency and normalized length given
	nr_double_t f = spice_evaluate_value (pf->value);
	nr_double_t l = spice_evaluate_value (pl->value);
	def->pairs = spice_del_property (def->pairs, pf);
	def->pairs = spice_del_property (def->pairs, pl);
	len = C0 / f * l;
	spice_set_property_value (def, "L", len);
      }
      else if (pf != NULL) {
	// only frequency given, default normalized length
	nr_double_t f = spice_evaluate_value (pf->value);
	def->pairs = spice_del_property (def->pairs, pf);
	len = C0 / f * 0.25;
	spice_set_property_value (def, "L", len);
      }
      else {
	fprintf (stderr, "spice error, either TD or F required in "
		 "lossless `%s' line \n", def->instance);
	spice_errors++;
      }
    }
    // post-process mutual inductors (transformer)
    if (!def->action && !strcmp (def->type, "Tr")) {
      if (1)
	root = spice_translate_coupled_x (root, def);
      else
	root = spice_translate_coupled (root, def);
    }
    // post-process general analysis options
    if (def->action && strstr (def->type, "OPT")) {
      struct value_t * val;
      if ((val = spice_find_property (def->values, "ABSTOL")) != NULL) {
	spice_add_property (root, "DC", "abstol", val->unit);
	spice_add_property (root, "TR", "abstol", val->unit);
      }
      if ((val = spice_find_property (def->values, "RELTOL")) != NULL) {
	spice_add_property (root, "DC", "reltol", val->unit);
	spice_add_property (root, "TR", "reltol", val->unit);
      }
      if ((val = spice_find_property (def->values, "VNTOL")) != NULL) {
	spice_add_property (root, "DC", "vntol", val->unit);
	spice_add_property (root, "TR", "vntol", val->unit);
      }
      if ((val = spice_find_property (def->values, "ITL1")) != NULL) {
	spice_add_property (root, "DC", "MaxIter", val->unit);
      }
      if ((val = spice_find_property (def->values, "ITL4")) != NULL) {
	spice_add_property (root, "TR", "MaxIter", val->unit);
      }
    }
    // post-process print and plot statements
    if ((def->action && !strcmp (def->type, "PRINT")) ||
	!strcmp (def->type, "PLOT")) {
      spice_collect_external_nodes (def);
    }
    // post-process untranslated netlist lines
    if (def->define == NULL) {
      def->text = spice_untranslated_text (def);
    }
    // remove values if possible
    spice_free_values (def);
  }
  return root;
}

/* The function translates special actions defined in the Spice
   netlist including the types of simulations. */
static struct definition_t *
spice_translate_action (struct definition_t * root,
			struct definition_t * def) {
  // translate transient analysis
  if (!strcasecmp (def->type, "TRAN")) {
    free (def->type);
    def->type = strdup ("TR");
    struct value_t * val;
    int i = 0;
    foreach_value (def->values, val) {
      switch (i++) {
      case 0:
	spice_append_pair (def, "Points", val->ident, 0);
	break;
      case 1:
	spice_append_pair (def, "Stop", val->ident, 0);
	break;
      case 2:
	spice_append_pair (def, "Start", val->ident, 0);
	break;
      case 3:
	break;
      }
      spice_value_done (val);
    }
    double v;
    v  = spice_get_property_value (def, "Stop");
    v -= spice_get_property_value (def, "Start");
    v /= spice_get_property_value (def, "Points");
    v += 1;
    spice_set_property_value (def, "Points", v);
  }
  // translate AC analysis
  else if (!strcasecmp (def->type, "AC")) {
    struct value_t * val;
    char * type = NULL;
    int i = 0;
    foreach_value (def->values, val) {
      switch (i++) {
      case 0:
	type = strdup (val->ident);
	break;
      case 1:
	spice_append_pair (def, "Points", val->ident, 0);
	break;
      case 2:
	spice_append_pair (def, "Start", val->ident, 0);
	break;
      case 3:
	spice_append_pair (def, "Stop", val->ident, 0);
	break;
      }
      spice_value_done (val);
    }
    double v;
    v = spice_evaluate_points (&type,
			       spice_get_property_value (def, "Start"),
			       spice_get_property_value (def, "Stop"),
			       spice_get_property_value (def, "Points"));
    spice_set_property_value (def, "Points", v + 1);
    spice_set_property_string (def, "Type", type);
    free (type);
  }
  // translate DC analysis
  else if (!strcasecmp (def->type, "DC")) {
    struct definition_t * para1 = NULL, * para2 = NULL;
    if (spice_count_values (def->values) >= 4) {
      para1 = spice_create_para (def);
      spice_set_property_string (para1, "Sim",
				 spice_toupper (def->instance));
      if (spice_count_values (def->values) >= 4) {
	para2 = spice_create_para (def);
	spice_set_property_string (para2, "Sim",
				   spice_toupper (para1->instance));
      }
    }
    if (para1) root = spice_add_definition (root, para1);
    if (para2) root = spice_add_definition (root, para2);
  }
  // translate subcircuit definition
  else if (!strcasecmp (def->type, "SUBCKT")) {
    free (def->type);
    def->type = strdup ("Def");
    def->sub = spice_checker_intern (def->sub);
  }
  // translate operating point analysis
  else if (!strcasecmp (def->type, "OP")) {
    free (def->type);
    def->type = strdup ("DC");
    spice_set_property_string (def, "saveOPs", "yes");
  }
  return root;
}

/* The following function translates the given definition 'sub' into a
   valid Qucs subcircuit instance. */
static void spice_translate_subcircuit (struct definition_t * sub) {
  struct value_t * val;
  free (sub->type);
  sub->type = strdup ("Sub");
  foreach_value (sub->values, val) {
    if (val->hint & HINT_NAME) {
      spice_set_property_string (sub, "Type", val->ident);
      break;
    }
  }
}

/* The function returns a static string containing successive instance
   names for the nodeset functionality in Qucs. */
static char * spice_create_intern_nodeset (void) {
  static int intern = 1;
  static char txt[32];
  sprintf (txt, "NS%d", intern++);
  return txt;
}

/* The following function translates the nodeset functionality defined
   by Spice into appropriate definitions for Qucs. */
static struct definition_t *
spice_translate_nodeset (struct definition_t * root,
			 struct definition_t * def) {
  struct value_t * val;
  struct definition_t * node = def;
  free (def->type);
  def->type = strdup ("NodeSet");
  free (node->instance);
  node->instance = strdup (spice_create_intern_nodeset ());
  node->action = PROP_COMPONENT;
  foreach_value (def->values->next, val) {
    if (val->hint & HINT_NODE) {
      node->nodes = spice_translate_node (val->ident);
    }
    if (val->hint & HINT_NUMBER) {
      spice_append_pair (node, "U", val->ident, 1);
    }
    if (val->hint & HINT_NAME) {
      node = spice_create_definition (node, "NodeSet");
      root = spice_add_definition (root, node);
      free (node->instance);
      node->instance = strdup (spice_create_intern_nodeset ());
    }
  }
  return root;
}

/* This is the overall Spice netlist translator.  It adjusts the list
   of definitions into usable structures. */
static struct definition_t * spice_translator (struct definition_t * root) {
  for (struct definition_t * def = root; def != NULL; def = def->next) {
    if ((def->define = spice_find_definition (def->type)) != NULL) {
      strcpy (def->type, def->define->type);
      def->nodes = spice_get_nodes (def);
      def->pairs = spice_get_pairs (def);
      if (!def->action) { // handle normal components
	// devices
	if (!strcasecmp (def->type, "Q") || !strcasecmp (def->type, "M") ||
	    !strcasecmp (def->type, "J") || !strcasecmp (def->type, "D") ||
	    !strcasecmp (def->type, "S") || !strcasecmp (def->type, "R") ||
	    !strcasecmp (def->type, "C")) {
	  spice_translate_device (root, def);
	}
	// controlled sources
	if (!strcasecmp (def->type, "E") || !strcasecmp (def->type, "G")) {
	  root = spice_translate_poly (root, def);
	}
	// controlled sources
	if (!strcasecmp (def->type, "C") || !strcasecmp (def->type, "L")) {
	  root = spice_translate_poly_lc (root, def);
	}
	// voltage sources
	if (!strcasecmp (def->type, "V")) {
	  root = spice_translate_source (root, def, 'U');
	}
	// current sources
	if (!strcasecmp (def->type, "I")) {
	  root = spice_translate_source (root, def, 'I');
	}
	// subcircuits
	if (!strcasecmp (def->type, "X")) {
	  spice_translate_subcircuit (def);
	}
	spice_translate_type (def);
      }
      else {              // handle special actions (dot '.' commands)
	// nodeset functionality
	if (!strcasecmp (def->type, "NODESET") ||
	    !strcasecmp (def->type, "IC")) {
	  root = spice_translate_nodeset (root, def);
	}
	else {
	  root = spice_translate_action (root, def);
	}
      }
      spice_fixup_definition (def);
    }
    else {
      // handle device description
      if (!strcasecmp (def->type, "MODEL")) {
	spice_add_Model (def);
      }
    }
    // remove values if possible
    spice_free_values (def);
  }
  return root;
}

/* TODO list for Spice Translator:
       - current sources
       - subcircuits
   - file includes
   - transmission lines
       - voltage dependent sources
       - current dependent sources
       - initial conditions
       - options (partly done)
       - mutual inductors (transformer)
   - mesfet if available in Qucs
   - other mosfet models if available in Qucs
       - three mutual inductors
   - current controlled switch (gyrator + voltage controlled switch)
   - single-frequency FM (using pm-modulator)
       - controlled E, G, F, and H poly sources
   - analog behavioural B, E, G, F, and H sources
   - piece-wise linear (PWL) voltage and current sources
   - temperature analysis (.TEMP)
   - temperature option (.OPTIONS TNOM=27)
       - non-linear L and C poly components
*/

#if 0
// My debugger...
static void spice_lister (struct definition_t * root) {
  struct value_t * val;
  for (struct definition_t * def = root; def != NULL; def = def->next) {
    fprintf (stderr, "%s:%s", def->type, def->instance);
    for (val = def->values; val != NULL; val = val->next) {
      if (val->ident)
	fprintf (stderr, " %s[%d]", val->ident, val->hint);
      else
	fprintf (stderr, " %g[%d]", val->value, val->hint);
    }
    fprintf (stderr, "\n");
  }
}
#endif

// Adjusts the hint value of the last entry in the value list.
void spice_add_last_hint (struct value_t * val, int hint) {
  if (val == NULL) return;
  for (; val->next != NULL; val = val->next) ;
  val->hint |= hint;
}

// Adjusts the hint value of the last entry in the value list.
void spice_set_last_hint (struct value_t * val, int hint) {
  if (val == NULL) return;
  for (; val->next != NULL; val = val->next) ;
  val->hint = hint;
}

/* This function is the overall spice checker and translator. */
struct definition_t * spice_checker_intern (struct definition_t * root) {
#if 0
  spice_lister (root);
#endif
  root = spice_translator (root);
  root = spice_post_translator (root);
  return root;
}

/* This function is the overall spice checker and translator. */
int spice_checker (void) {
  spice_errors = 0;
  definition_root = spice_checker_intern (definition_root);
  return spice_errors;
}
