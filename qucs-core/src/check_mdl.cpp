/*
 * check_mdl.cpp - iterate an IC-CAP MDL file
 *
 * Copyright (C) 2006, 2007 Stefan Jahn <stefan@lkcc.org>
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

#include "logging.h"
#include "strlist.h"
#include "object.h"
#include "complex.h"
#include "vector.h"
#include "dataset.h"
#include "sweep.h"
#include "valuelist.h"
#include "constants.h"
#include "check_mdl.h"
#include "parse_mdl.hpp"

using namespace qucs;

// Global variables.
dataset * mdl_result = NULL;
struct mdl_link_t * mdl_root = NULL;
struct mdl_sync_t * mdl_sync_root = NULL;

// Creates an independent data vector.
static void mdl_create_depdataset (sweep * data, char * name) {
  vector v (name);
  for (int i = 0; i < data->getSize (); i++) v.add (data->get (i));
  mdl_result->appendDependency (new vector (v));
}

// Creates an independent data vector with a single element.
static void mdl_create_condataset (double val, char * name) {
  vector v (name);
  v.add (val);
  mdl_result->appendDependency (new vector (v));
}

// The functions creates dependent data vector(s).
static void mdl_create_vardataset (struct mdl_point_t * point,
				   struct mdl_datasize_t * dsize,
				   const char * name, const char * type,
				   strlist * deps) {
  vector * v = new vector[dsize->x * dsize->y] ();
  // adjust type
  if (!strcmp (type, "MEAS"))
    type = ".M";
  else if (!strcmp (type, "SIMU"))
    type = ".S";
  else if (!strcmp (type, "COMMON"))
    type = "";
  // create vectors
  for (struct mdl_point_t * p = point; p != NULL; p = p->next) {
    int n = (p->y - 1) * 2 + p->x - 1;
    v[n].add (nr_complex_t (p->r, p->i));
  }
  // go through indices
  for (int x = 1; x < dsize->x + 1; x++) {
    for (int y = 1; y < dsize->y + 1; y++) {
      // create vector description
      int n = (y - 1) * 2 + x - 1;
      char * txt = (char *) malloc (strlen (name) + strlen (type) + 4 + 2 * 3);
      if (dsize->x > 1 || dsize->y > 1)
	sprintf (txt, "%s%s[%d,%d]", name, type, x, y);
      else
	sprintf (txt, "%s%s", name, type);
      v[n].setName (txt);
      free (txt);
      // put vector into dataset
      if (v[n].getSize () > 1) {
	v[n].setDependencies (new strlist (*deps));
	mdl_result->appendVariable (new vector (v[n]));
      } else {
	v[n].setDependencies (new strlist ());
	mdl_result->appendDependency (new vector (v[n]));
      }
    }
  }
  delete[] v;
}

// Look through hypertable elements for a name and return value.
static char * mdl_find_helement (struct mdl_element_t * root,
				 const char * name) {
  for (; root != NULL; root = root->next) {
    if (!strcmp (root->name, name)) return root->value;
  }
  return NULL;
}

// Look through table elements for a name and return value.
static char * mdl_find_telement (struct mdl_element_t * root,
				 const char * name) {
  for (; root != NULL; root = root->next) {
    if (!strcmp (root->name, "Name") && !strcmp (root->value, name)) {
      if (root->next && !strcmp (root->next->name, "Value")) {
	return root->next->value;
      }
    }
  }
  return NULL;
}

// Convert number suffix into a multiplication factor.
static double mdl_convert_factor (char * end) {
  double f = 1.0;
  if (end) {
    switch (*end) {
    case 'K': f = 1e+03; break;
    case 'M': f = 1e+06; break;
    case 'G': f = 1e+09; break;
    case 'T': f = 1e+12; break;
    case 'm': f = 1e-03; break;
    case 'u': f = 1e-06; break;
    case 'n': f = 1e-09; break;
    case 'p': f = 1e-12; break;
    case 'f': f = 1e-15; break;
    case 'a': f = 1e-18; break;
    }
  }
  return f;
}

// Forward declaration.
static double mdl_telement_dvalue (struct mdl_link_t *, struct mdl_element_t *,
				   const char *);

// The function resolves the given variable trying upscope resolving.
static int mdl_resolve_variable (struct mdl_link_t * link, char * name,
				 double &val) {
  int done = 0;
  val = 0.0;
  struct mdl_lcontent_t * root;
  // try finding variable in current link
  for (root = link->content; !done && root != NULL; root = root->next) {
    if (root->type == t_TABLE) {
      struct mdl_element_t * eroot = root->table->data;
      if (mdl_find_telement (eroot, name)) {
	val = mdl_telement_dvalue (link, eroot, name);
	done++;
      }
    }
  }
  // resolve variable in upper scope recursively
  if (!done && link->parent) {
    done = mdl_resolve_variable (link->parent, name, val);
  }
  return done;
}

// Converts a string into a valid value.  Uses variable substitutions.
static double mdl_variable_value (struct mdl_link_t * link, char * txt) {
  double val = 0.0;
  char * end = NULL;
  if (txt != NULL) {
    // remove whitespaces
    char * t, * p = txt;
    while (*p) {
      if (isspace (*p)) {
	t = p;
	while (*t) { *t = *(t + 1); t++; }
	p--;
      }
      p++;
    }
    // extract value if possible
    val = strtod (txt, &end);
    // not a value, a variable
    if (end == txt) {
      double f = 1.0;
      if      (*txt == '-') { f = -1.0; txt++; }
      else if (*txt == '+') { f = +1.0; txt++; }
      if (!mdl_resolve_variable (link, txt, val)) {
	// special variables
	if (!strcmp (txt, "PI")) {
	  val = pi;
	}
	// no resolvable (probably equation)
	else {
	  logprint (LOG_ERROR,
		    "checker error, unable to resolve `%s' variable in '%s'\n",
		    txt, link->name);
	  val = 0.0;
	}
      }
      val = f * val;
    }
    // normal value with probably a suffix
    else {
      val *= mdl_convert_factor (end);
    }
  }
  return val;
}

// Returns a double variable value stored in a hypertable.
static double mdl_helement_dvalue (struct mdl_link_t * link,
				   struct mdl_element_t * eroot,
				   const char * name) {
  char * txt = mdl_find_helement (eroot, name);
  return mdl_variable_value (link, txt);
}

// Returns a double variable value stored in a table.
static double mdl_telement_dvalue (struct mdl_link_t * link,
				   struct mdl_element_t * eroot,
				   const char * name) {
  char * txt = mdl_find_telement (eroot, name);
  return mdl_variable_value (link, txt);
}

// Returns a integer variable value stored in a hypertable.
static int mdl_helement_ivalue (struct mdl_link_t * link,
				struct mdl_element_t  * eroot,
				const char * name) {
  return (int) mdl_helement_dvalue (link, eroot, name);
}

// Looks for dependent data vectors and creates them.
static void mdl_find_vardataset (struct mdl_dcontent_t * droot, char * name,
				 strlist * deps) {
  struct mdl_dcontent_t * root;
  // go through dataset content
  for (root = droot; root != NULL; root = root->next) {
    if (root->type == t_DATASET) {
      // create possibly both - MEAS and SIMU - data vectors
      struct mdl_dataset_t * dset = root->data;
      if (dset->data1)
	mdl_create_vardataset (dset->data1, dset->dsize, name, dset->type1,
			       deps);
      if (dset->data2)
	mdl_create_vardataset (dset->data2, dset->dsize, name, dset->type2,
			       deps);
    }
  }
}

// Looks for independent data vectors and creates them.
valuelist<int> * mdl_find_depdataset (struct mdl_link_t * link,
				      struct mdl_dcontent_t * droot,
				      char * name) {
  char * stype = NULL;
  double val, start, stop, step;
  int nof = 0, order = 0;
  valuelist<int> * deps = new valuelist<int> ();
  struct mdl_dcontent_t * root;

  // go through dataset content
  for (root = droot; root != NULL; root = root->next) {
    if (root->type == t_HYPTABLE) {
      struct mdl_hyptable_t * hyptab = root->hyptable;
      // found a sweep definition?
      if (!strcmp (hyptab->name, "Edit Sweep Def")) {
	if (!strcmp (stype, "LIN")) {
	  // linear sweep
	  order = mdl_helement_ivalue (link, hyptab->data, "Sweep Order");
	  start = mdl_helement_dvalue (link, hyptab->data, "Start");
	  stop = mdl_helement_dvalue (link, hyptab->data, "Stop");
	  nof = mdl_helement_ivalue (link, hyptab->data, "# of Points");
	  step = mdl_helement_dvalue (link, hyptab->data, "Step Size");
	  if (nof <= 0) nof = (int) fabs ((stop - start) / step) + 1;
	  deps->insert({{name,order}});
	  linsweep * sw = new linsweep ();
	  sw->create (start, stop, nof);
	  mdl_create_depdataset (sw, name);
	  delete sw;
	}
	else if (!strcmp (stype, "CON")) {
	  // constant sweep
	  val = mdl_helement_dvalue (link, hyptab->data, "Value");
	  mdl_create_condataset (val, name);
	}
	else if (!strcmp (stype, "LOG")) {
	  // logarithmic sweep
	  order = mdl_helement_ivalue (link, hyptab->data, "Sweep Order");
	  start = mdl_helement_dvalue (link, hyptab->data, "Start");
	  stop = mdl_helement_dvalue (link, hyptab->data, "Stop");
	  nof = mdl_helement_ivalue (link, hyptab->data, "Total Pts");
	  if (nof <= 0)
	    nof = mdl_helement_ivalue (link, hyptab->data, "# of Points");
	  if (start * stop == 0.0) {
	    if (start == 0.0) start = 1.0;
	    if (stop  == 0.0) stop  = 1.0;
	  }
	  deps->insert({{name,order}});
	  logsweep * sw = new logsweep ();
	  sw->create (start, stop, nof);
	  mdl_create_depdataset (sw, name);
	  delete sw;
	}
	else if (!strcmp (stype, "LIST")) {
	  // list sweep
	  order = mdl_helement_ivalue (link, hyptab->data, "Sweep Order");
	  nof = mdl_helement_ivalue (link, hyptab->data, "# of Values");
	  deps->insert({{name,order}});
	}
	else if (!strcmp (stype, "SYNC")) {
	  // sync sweep
	  struct mdl_sync_t * sync = (struct mdl_sync_t *)
	    calloc (sizeof (struct mdl_sync_t), 1);
	  sync->ratio = mdl_helement_dvalue (link, hyptab->data, "Ratio");
	  sync->offset = mdl_helement_dvalue (link, hyptab->data, "Offset");
	  sync->master = mdl_find_helement (hyptab->data, "Master Sweep");
	  sync->master = strdup (sync->master);
	  sync->name = strdup (name);
	  sync->next = mdl_sync_root;
	  mdl_sync_root = sync;
	}
      }
      // found a sweep information?
      else if (!strcmp (hyptab->name, "Edit Sweep Info")) {
	stype = mdl_find_helement (hyptab->data, "Sweep Type");
      }
      // found a list table?
      else if (!strcmp (hyptab->name, "List Table")) {
	lstsweep * sw = new lstsweep ();
	sw->create (nof);
	char txt[16];
	for (int i = 0; i < nof; i++) {
	  sprintf (txt, "Value %d", i + 1);
	  val = mdl_helement_dvalue (link, hyptab->data, txt);
	  sw->set (i, val);
	}
	mdl_create_depdataset (sw, name);
	delete sw;
      }
    }
  }
  return deps;
}

// Composes a link name.
static char * mdl_create_linkname (char * base, char * name) {
  char * txt = (char *) malloc (strlen (base) + 2 + strlen (name));
  sprintf (txt, "%s.%s", base, name);
  return txt;
}

// Collects dependency links.
static void mdl_find_deplink (struct mdl_link_t * link, char * name,
			      valuelist<int> * deps) {
  struct mdl_lcontent_t * root;
  const valuelist<int> * d;
  // go through link content
  for (root = link->content; root != NULL; root = root->next) {
    // independent data vector
    if (root->type == t_DATA) {
      d = mdl_find_depdataset (link, root->data->content, name);
      if (d != NULL) {
	valuelist<int> copy = *d;
	delete d;
	copy.insert(deps->begin(),deps->end());
	*deps=copy;
      }
    }
    // link to independent data vector
    else if (root->type == t_LINK && !strcmp (root->link->type, "SWEEP")) {
      char * txt = mdl_create_linkname (name, root->link->name);
      root->link->parent = link;
      mdl_find_deplink (root->link, txt, deps);
      free (txt);
    }
  }
}

// Collects variable links.
static void mdl_find_varlink (struct mdl_link_t * link, char * name,
			      strlist * deps) {
  struct mdl_lcontent_t * root;
  // go through link content
  for (root = link->content; root != NULL; root = root->next) {
    // dependent data vector
    if (root->type == t_DATA) {
      mdl_find_vardataset (root->data->content, name, deps);
    }
    // link to dependent data vector
    else if (root->type == t_LINK && (!strcmp (root->link->type, "OUT") ||
				      !strcmp (root->link->type, "XFORM"))) {
      char * txt = mdl_create_linkname (name, root->link->name);
      root->link->parent = link;
      mdl_find_varlink (root->link, txt, deps);
      free (txt);
    }
  }
}

// Sorts a dependency list according to their sweep order.
static strlist * mdl_sort_deps (valuelist<int> * d) {
  strlist * deps = new strlist ();
  for (int i = 0; static_cast<unsigned int>(i) < d->size(); i++) {
    for (auto &val: *d) {
      if (val.second == i + 1) {
	deps->append (val.first.c_str());
      }
    }
  }
  return deps;
}

// Iterates the MDL file recursively.
static void mdl_find_link (struct mdl_link_t * link, char * name) {
  struct mdl_lcontent_t * root;

  // collect independent data
  valuelist<int> * vdeps = new valuelist<int> ();
  mdl_find_deplink (link, name, vdeps);
  strlist * deps = mdl_sort_deps (vdeps);
  delete vdeps;

  // collect dependent data
  mdl_find_varlink (link, name, deps);
  delete deps;

  // go through link content
  for (root = link->content; root != NULL; root = root->next) {
    if (root->type == t_LINK &&
	strcmp (root->link->type, "OUT") &&
	strcmp (root->link->type, "SWEEP") &&
	strcmp (root->link->type, "XFORM")) {
      char * txt = mdl_create_linkname (name, root->link->name);
      root->link->parent = link;
      mdl_find_link (root->link, txt);
      free (txt);
    }
  }
}

// Creates an synchronized independent data vector.
static void mdl_create_syndataset (vector * v, char * name) {
  v->setName (name);
  mdl_result->appendDependency (v);
}

// Goes through list of sync sweeps and creates them if possible.
void mdl_find_syncdatasets (struct mdl_sync_t * root) {
  struct mdl_sync_t * sync;
  for (sync = root; sync != NULL; sync = sync->next) {
    // determine master sweep link
    char * link = sync->name;
    int i = strlen (link) - 1;
    while (i > 0 && link[i] != '.') i--;
    if (link[i] == '.') {
      link[i] = '\0';
      char * txt = (char *) malloc (i + 2 + strlen (sync->master));
      sprintf (txt, "%s.%s", link, sync->master);
      link[i] = '.';
      free (sync->master);
      sync->master = txt;
    }
    // create synchronize independent data vector
    vector * v = mdl_result->findDependency (sync->master);
    if (v != NULL) {
      vector * s = new vector ((*v) * sync->ratio + sync->offset);
      mdl_create_syndataset (s, sync->name);
    }
  }
}

// Destroys an element structure.
static void mdl_free_element (struct mdl_element_t * e) {
  free (e->name);
  free (e->value);
  free (e->attr);
  free (e);
}

// Destroys a datasize structure.
static void mdl_free_datasize (struct mdl_datasize_t * d) {
  free (d->type);
  free (d);
}

// Destroys a hypertable structure.
static void mdl_free_hyptable (struct mdl_hyptable_t * h) {
  free (h->name);
  struct mdl_element_t * e, * next;
  for (e = h->data; e != NULL; e = next) {
    next = e->next;
    mdl_free_element (e);
  }
  free (h);
}

// Destroys a table structure.
static void mdl_free_table (struct mdl_table_t * t) {
  free (t->name);
  struct mdl_element_t * e, * next;
  for (e = t->data; e != NULL; e = next) {
    next = e->next;
    mdl_free_element (e);
  }
  free (t);
}

// Destroys a dataset structure.
static void mdl_free_dataset (struct mdl_dataset_t * d) {
  free (d->type1);
  struct mdl_point_t * p, * next;
  for (p = d->data1; p != NULL; p = next) {
    next = p->next;
    free (p);
  }
  free (d->type2);
  for (p = d->data2; p != NULL; p = next) {
    next = p->next;
    free (p);
  }
  if (d->dsize) mdl_free_datasize (d->dsize);
}

// Destroys a data content structure.
static void mdl_free_dcontent (struct mdl_dcontent_t * c) {
  switch (c->type) {
  case t_DATASET: mdl_free_dataset (c->data); break;
  case t_HYPTABLE: mdl_free_hyptable (c->hyptable); break;
  }
}

// Destroys a data structure.
static void mdl_free_data (struct mdl_data_t * d) {
  struct mdl_dcontent_t * c, * next;
  for (c = d->content; c != NULL; c = next) {
    next = c->next;
    mdl_free_dcontent (c);
  }
}

// Forward declaration.
static void mdl_free_link (struct mdl_link_t *);

// Destroys a link content structure.
static void mdl_free_lcontent (struct mdl_lcontent_t * c) {
  switch (c->type) {
  case t_LINK: mdl_free_link (c->link); break;
  case t_DATA: mdl_free_data (c->data); break;
  case t_TABLE: mdl_free_table (c->table); break;
  }
  free (c);
}

// Destroys a link structure.
static void mdl_free_link (struct mdl_link_t * l) {
  free (l->name);
  free (l->type);
  struct mdl_lcontent_t * c, * next;
  for (c = l->content; c != NULL; c = next) {
    next = c->next;
    mdl_free_lcontent (c);
  }
}

// Destroys a sync structure.
static void mdl_free_sync (struct mdl_sync_t * s) {
  struct mdl_sync_t * next;
  for (; s != NULL; s = next) {
    next = s->next;
    free (s->name);
    free (s->master);
    free (s);
  }
}

// Computes the dependent vector length for the given dependency list.
static int mdl_get_depsize (strlist * deps) {
  char * n;
  vector * v;
  int res = 1;
  for (int i = 0; i < deps->length (); i++) {
    if ((n = deps->get (i)) != NULL)
      if ((v = mdl_result->findDependency (n)) != NULL)
	res *= v->getSize ();
  }
  return res;
}

// Checks the variable vector dependencies.  Makes them independent if
// necessary.
static void mdl_check_xform_deplen (void) {
  vector * v, * next;
  for (v = mdl_result->getVariables (); v; v = next) {
    next = (vector *) v->getNext ();
    strlist * deps = v->getDependencies ();
    if (deps->length () <= 0) {
      vector * d = new vector (*v);
      mdl_result->delVariable (v);
      mdl_result->addDependency (d);
    }
  }
}

// Checks the variable vector dependencies and reduces them if necessary.
static void mdl_check_xform_dep (void) {
  vector * v, * d;
  strlist * deps;
  for (v = mdl_result->getVariables (); v; v = (vector *) v->getNext ()) {
    deps = v->getDependencies ();
    int s = mdl_get_depsize (deps);
    // dependencies differ from actual vector length
    if (v->getSize () != s) {
      int found = 0;
      for (int i = 0; i < deps->length (); i++) {
	// find out a single dependency with the appropriate length
	char * n = deps->get (i);
	if (n != NULL) {
	  d = mdl_result->findDependency (n);
	  if (d != NULL && v->getSize () == d->getSize ()) {
	    strlist * dep = new strlist ();
	    dep->add (n);
	    v->setDependencies (dep);
	    found++;
	    break;
	  }
	}
      }
      // if not found, then no dependency vector
      if (!found) v->setDependencies (new strlist ());
    }
  }
}

// Checks the XFORM's in the model file
static void mdl_check_xforms (void) {
  mdl_check_xform_dep ();
  mdl_check_xform_deplen ();
}

/* This function is the overall MDL data checker.  It returns zero on
   success, non-zero otherwise. */
int mdl_check (void) {
  int errors = 0;
  mdl_result = new dataset ();
  struct mdl_link_t * root;
  for (root = mdl_root; root; root = root->next) {
    char * name = root->name;
    mdl_find_link (root, name);
  }
  mdl_find_syncdatasets (mdl_sync_root);
  mdl_check_xforms ();
  return errors ? -1 : 0;
}

// Destroys data used by the MDL checker.
void mdl_destroy (void) {
  if (mdl_result != NULL) {
    // delete associated dataset
    delete mdl_result;
    mdl_result = NULL;
  }
  if (mdl_root != NULL) {
    // release internal data structures
    struct mdl_link_t * root, * next;
    for (root = mdl_root; root; root = next) {
      next = root->next;
      mdl_free_link (root);
    }
    mdl_root = NULL;
  }
  if (mdl_sync_root != NULL) {
    // release internal sync structures
    mdl_free_sync (mdl_sync_root);
    mdl_sync_root = NULL;
  }
}

// Initializes the MDL checker.
void mdl_init (void) {
  mdl_root = NULL;
  mdl_result = NULL;
  mdl_sync_root = NULL;
}
