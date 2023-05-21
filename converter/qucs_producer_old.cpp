/*
 * qucs_producer.cpp - the Qucs netlist producer
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

#include <complex>
#include "qucs_typedefs.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#include "object.h"
#include "complex.h"
#include "vector.h"
#include "dataset.h"
#include "netdefs.h"
#include "check_spice.h"
#include "check_vcd.h"
#include "hash.h"

/* Global variables. */
FILE * qucs_out = NULL;
int    qucs_actions = 1;
const char * qucs_gnd = "gnd";

qucs::hash<struct node_t> qucs_nodes;

/* Looks through the given list if there is such a node already in the
   list and returns non-zero if so, otherwise the function returns
   zero. */
int qucs_find_node (struct node_t * root, char * node) {
  for (struct node_t * n = root; n; n = n->next) {
    if (!strcmp (node, n->node)) return 1;
  }
  return 0;
}

/* Adds the given node list to the overall node list without
   duplicating the nodes. */
void qucs_add_nodes (struct node_t * node) {
  while (node) {
    if (!qucs_nodes.get (node->node))
      qucs_nodes.put (node->node, node);
    node = node->next;
  }
}

/* Deletes the given node list. */
void qucs_free_nodes (struct node_t * node) {
  struct node_t * n;
  for ( ; node; node = n) {
    n = node->next;
    free (node->node);
    free (node);
  }
}

/* The function deletes the collected qucs nodes. */
static void qucs_delete_nodes (void) {
  qucs_nodes.clear ();
}

/* Collects all nodes within the given definition root. */
static void qucs_collect_nodes (struct definition_t * root) {
  while (root) {
    qucs_add_nodes (root->nodes);
    root = root->next;
  }
}

/* Prints value representation. */
static void netlist_list_value (struct value_t * value) {
  if (value == NULL)
    fprintf (qucs_out, "[]");
  else if (value->ident)
    fprintf (qucs_out, "%s", value->ident);
  else if (value->next) {
    fprintf (qucs_out, "[");
    for (; value != NULL; value = value->next)
      fprintf (qucs_out, "%g%s", value->value, value->next ? ";" : "");
    fprintf (qucs_out, "]");
  } else {
    fprintf (qucs_out, "%g", value->value);
    if (value->scale)
      fprintf (qucs_out, "%s", value->scale);
    if (value->unit)
      fprintf (qucs_out, "%s", value->unit);
  }
}

/* Returns a static string for an instance identifier usable in
   netlists based upon the given identifier. */
static char * netlist_instance (const char * instance) {
  static char ret[256];
  sprintf (ret, "%s%s", isdigit (instance[0]) ? "X" : "", instance);
  return ret;
}

/* Prints a single definition on a single line. */
static void netlist_list_def (struct definition_t * def, const char * prefix) {
  struct node_t * node;
  struct pair_t * pair;
  if (def->define == NULL) {
    if (def->text != NULL)
      fprintf (qucs_out, "%s# %s\n", prefix, def->text);
  }
  else {
    if (!qucs_actions) {
      // skip specific actions if required
      if (def->action && strcmp (def->type, "Def")) return;
    }
    fprintf (qucs_out, "%s%s%s:%s", prefix, def->action ? "." : "",
	     def->type, netlist_instance (def->instance));
    for (node = def->nodes; node != NULL; node = node->next)
      fprintf (qucs_out, " %s", node->node);
    for (pair = def->pairs; pair != NULL; pair = pair->next) {
      fprintf (qucs_out, " %s=\"", pair->key);
      netlist_list_value (pair->value);
      fprintf (qucs_out, "\"");
    }
    fprintf (qucs_out, "\n");
  }
}

/* Prints definition list representation. */
static void netlist_lister (struct definition_t * root, const char * prefix) {
  struct definition_t * def;
  for (def = root; def != NULL; def = def->next) {
    if (def->sub != NULL) {
      netlist_list_def (def, prefix);
      netlist_lister (def->sub, "  ");
      fprintf (qucs_out, ".Def:End\n");
    }
    else {
      netlist_list_def (def, prefix);
    }
  }
}

/* Goes recursively through the netlist and applies additional
   reference node modifications to subcircuits and their instances. */
static void netlist_fix_reference (struct definition_t * root) {
  struct definition_t * def;
  struct node_t * n;

  // go through definitions
  for (def = root; def != NULL; def = def->next) {
    if (!strcmp (def->type, "Sub")) { // subcircuit instances
      n = create_node ();
      n->node = strdup (qucs_gnd);
      n->next = def->nodes;
      def->nodes = n;
    }
    else if (def->sub != NULL) {     // subcircuit definitions
      n = create_node ();
      n->node = strdup (qucs_gnd);
      n->next = def->nodes;
      def->nodes = n;
      netlist_fix_reference (def->sub);
    }
  }
}

/* Applies reference node modifications if necessary. */
static void netlist_reference_ground (void) {
  struct definition_t * def;

  // return if nothing todo
  if (qucs_gnd && !strcmp (qucs_gnd, "gnd")) return;

  netlist_fix_reference (definition_root);
  // go through subcircuits
  for (def = subcircuit_root; def != NULL; def = def->next) {
    netlist_fix_reference (def->sub);
  }
}

/* Look for a single subcircuit definition in a netlist. */
static struct definition_t *
netlist_get_single_subcircuit (struct definition_t * root) {
  int count = 0;
  struct definition_t * ret = NULL;
  for (struct definition_t * def = root; def != NULL; def = def->next) {
    if (!def->action) {
      count++;
    } else if (def->sub) {
      count++;
      ret = def;
    }
  }
  return (count == 1) ? ret : NULL;
}

/* Prints the overall netlist representation. */
static void netlist_list (void) {
  struct node_t * n;
  struct definition_t * def;
  time_t t = time (NULL);
  fprintf (qucs_out, "# converted Qucs netlist processed at %s\n", ctime (&t));
  if (spice_title != NULL) {
    fprintf (qucs_out, "#\n# %s#\n\n", spice_title);
  }
  netlist_lister (definition_root, "");
  for (def = subcircuit_root; def != NULL; def = def->next) {
    fprintf (qucs_out, ".Def:%s\n", netlist_instance (def->instance));
    netlist_lister (def->sub, "  ");
    fprintf (qucs_out, ".Def:End\n");
  }
  /* Instantiate subcircuit if there is just a single definition. */
  if (definition_root != NULL && subcircuit_root == NULL &&
      (def = netlist_get_single_subcircuit (definition_root)) != NULL) {
    fprintf (qucs_out, "\n# no instance of subcircuit \"%s\" found, "
	     "creating it\n", netlist_instance (def->instance));
    fprintf (qucs_out, "Sub:X1");
    for (n = def->nodes; n; n = n->next) fprintf (qucs_out, " %s", n->node);
    fprintf (qucs_out, " Type=\"%s\"\n", netlist_instance (def->instance));
  }
  /* Print overall (toplevel only) node list. */
  qucs_collect_nodes (definition_root);
  fprintf (qucs_out, "\n### TOPLEVEL NODELIST BEGIN\n");
  for (qucs::hashiterator<struct node_t> it (qucs_nodes); *it; ++it) {
    n = it.currentVal ();
    fprintf (qucs_out, "# %s\n", n->node);
  }
  qucs_delete_nodes ();
  fprintf (qucs_out, "### TOPLEVEL NODELIST END\n");
  /* Print potential external node list. */
  fprintf (qucs_out, "\n### SPICE OUTPUT NODELIST BEGIN\n");
  for (n = spice_nodes; n; n = n->next) {
    fprintf (qucs_out, "# %s\n", n->node);
  }
  fprintf (qucs_out, "### SPICE OUTPUT NODELIST END\n");
}

/* This function is the overall Qucs netlist producer. */
void qucs_producer (void) {
  if (qucs_out != NULL) {
    netlist_reference_ground ();
    netlist_list ();
  }
}

// Structure defining Qucs schematic entries of device models.
struct device_t {
  const char * ntype;      // netlist type
  const char * ltype;      // schematic type
  const char * stype;      // spice type
  int nodes;               // number of nodes
  const char * props[130]; // list of properties in schematic order
  const char * symbol;     // symbol text
  const char * coords;     // coordinates and other text
  const char * ports;      // subcircuit ports
}
qucs_devices[] = {
  /* diode */
  { "Diode", "Diode", "D", 2,
    {"Is", "N", "Cj0", "M", "Vj", "Fc", "Cp", "Isr", "Nr", "Rs", "Tt", "Ikf",
     "Kf", "Af", "Ffe", "Bv", "Ibv",  "Temp", "Xti", "Eg", "Tbv", "Trs",
     "Ttt1", "Ttt2", "Tm1", "Tm2", "Tnom", "Area", NULL },
    NULL,
    "1 0 0 -26 13 0 0",
    NULL
  },
  /* bipolar transistor */
  { "BJT", "_BJT", "Q", 4,
    { "Type", "Is", "Nf", "Nr", "Ikf", "Ikr", "Vaf", "Var", "Ise", "Ne", "Isc",
      "Nc", "Bf", "Br", "Rbm", "Irb", "Rc", "Re", "Rb", "Cje", "Vje", "Mje",
      "Cjc", "Vjc", "Mjc", "Xcjc", "Cjs", "Vjs", "Mjs", "Fc", "Tf", "Xtf",
      "Vtf", "Itf", "Tr", "Temp", "Kf", "Af", "Ffe", "Kb", "Ab", "Fb", "Ptf",
      "Xtb", "Xti", "Eg", "Tnom", "Area", NULL },
    NULL,
    "1 0 0 8 -26 0 0",
    NULL
  },
  /* junction FET */
  { "JFET", "JFET", "J", 3,
    { "Type", "Vt0", "Beta", "Lambda", "Rd", "Rs", "Is", "N", "Isr", "Nr",
      "Cgs", "Cgd", "Pb", "Fc", "M", "Kf", "Af", "Ffe", "Temp", "Xti",
      "Vt0tc", "Betatce", "Tnom", "Area", NULL },
    NULL,
    "1 190 140 30 -49 0 0",
    NULL
  },
  /* MOSFET */
  { "MOSFET", "_MOSFET", "M", 4,
    { "Type", "Vt0", "Kp", "Gamma", "Phi", "Lambda", "Rd", "Rs", "Rg", "Is",
      "N", "W", "L", "Ld", "Tox", "Cgso", "Cgdo", "Cgbo", "Cbd", "Cbs", "Pb",
      "Mj", "Fc", "Cjsw", "Mjsw", "Tt", "Nsub", "Nss", "Tpg", "Uo", "Rsh",
      "Nrd", "Nrs", "Cj", "Js", "Ad", "As", "Pd", "Ps", "Kf", "Af", "Ffe",
      "Temp", "Tnom", NULL },
    NULL,
    "1 0 0 8 -26 0 0",
    NULL
  },
  { NULL, NULL, NULL, 0, { NULL }, NULL, NULL, NULL }
};

/* Looks through the list of available Qucs devices.  Returns NULL if
   there is no such device. */
static struct device_t * qucslib_find_device (char * type) {
  struct device_t * dev;
  for (dev = qucs_devices; dev->ntype; dev++) {
    if (!strcmp (dev->ntype, type))
      return dev;
  }
  return NULL;
}

/* This function tries to find the given property in the key/value
   pairs of the given netlist entry and returns NULL if there is no
   such property. */
static struct pair_t *
qucslib_find_prop (struct definition_t * def, const char * key) {
  struct pair_t * pair;
  for (pair = def->pairs; pair; pair = pair->next) {
    if (!strcmp (pair->key, key))
      return pair;
  }
  return NULL;
}

/* The function outputs a device library entry in the Qucs schematic
   data format. */
static void qucslib_list_device (struct definition_t * def) {
  struct device_t * dev;
  if (!(dev = qucslib_find_device (def->type))) return;
  struct pair_t * pair;
  char txt[1024];

  sprintf (txt, "\n<Component %s>\n", def->instance[0] == dev->stype[0] ?
	   &def->instance[1] : def->instance);
  fprintf (qucs_out, "%s", txt);
  fprintf (qucs_out, "  <Description>\n");
  fprintf (qucs_out, "  </Description>\n");
  fprintf (qucs_out, "  <Model>\n");
  fprintf (qucs_out, "    <%s %s_ %s", dev->ltype,
	   netlist_instance (def->instance), dev->coords);
  for (int i = 0; dev->props[i]; i++) {
    if ((pair = qucslib_find_prop (def, dev->props[i])) != NULL) {
      fprintf (qucs_out, " \"");
      netlist_list_value (pair->value);
      fprintf (qucs_out, "\" 0");
    }
  }
  fprintf (qucs_out, ">\n");
  fprintf (qucs_out, "  </Model>\n");
  fprintf (qucs_out, "</Component>\n");
}

/* This function is the overall Qucs library producer. */
void qucslib_producer (void) {
  struct definition_t * def;
  fprintf (qucs_out, "<Qucs Library " PACKAGE_VERSION " \"Generic\">\n");
  for (def = device_root; def; def = def->next) {
    qucslib_list_device (def);
  }
}

/* This function is the Qucs dataset producer for VCD files. */
void qucsdata_producer_vcd (void) {
  struct dataset_variable * ds;
  struct dataset_value * dv;
  fprintf (qucs_out, "<Qucs Dataset " PACKAGE_VERSION ">\n");
  for (ds = dataset_root; ds; ds = ds->next) {
    if (!ds->output || ds->type == DATA_UNKNOWN)
      continue;
    if (ds->type == DATA_INDEPENDENT)
      fprintf (qucs_out, "<indep %s %d>\n", ds->ident, ds->size);
    else if (ds->type == DATA_DEPENDENT)
      fprintf (qucs_out, "<dep %s.%s %s>\n", ds->ident, ds->isreal ? "R" : "X",
	       ds->dependencies);
    for (dv = ds->values; dv; dv = dv->next) {
      fprintf (qucs_out, "  %s\n", dv->value);
    }
    if (ds->type == DATA_INDEPENDENT)
      fprintf (qucs_out, "</indep>\n");
    else if (ds->type == DATA_DEPENDENT)
      fprintf (qucs_out, "</dep>\n");
  }
}

/* This function is the Qucs dataset producer. */
void qucsdata_producer (dataset * data) {
  data->print ();
}
