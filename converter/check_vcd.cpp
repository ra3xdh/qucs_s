/*
 * check_vcd.cpp - iterate a vcd file
 *
 * Copyright (C) 2005, 2006, 2008, 2010 Stefan Jahn <stefan@lkcc.org>
 * Copyright (C) 2005 Raimund Jacob <raimi@lkcc.org>
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

#include "check_vcd.h"

// Enable for debugging purposes.
#define VCD_DEBUG 0

// Some more definitions.
#define VCD_SKIP_ARRAYS 1
#define VCD_INCLUDE_RANGE 0
#define VCD_TIMEVAR "dtime"

// Global variables.
struct vcd_file * vcd = NULL;
int vcd_errors = 0;
int vcd_freehdl = 1;
int vcd_correct = 0;
struct vcd_set * vcd_sets = NULL;
struct dataset_variable * dataset_root = NULL;

/* The function looks through all variable definitions in all scopes
   to find the given reference code. */
static struct vcd_vardef *
vcd_find_code (struct vcd_scope * root, char * code) {
  struct vcd_scope * scope;
  for (scope = root; scope; scope = scope->next) {
    struct vcd_vardef * var;
    for (var = scope->vardefs; var; var = var->next) {
      if (!strcmp (var->code, code))
	return var;
    }
    // search in sub-scopes
    if ((var = vcd_find_code (scope->scopes, code)) != NULL)
      return var;
  }
  return NULL;
}

// Free's the given VCD change.
static void vcd_free_change (struct vcd_change * vc) {
  free (vc->code);
  free (vc->value);
  free (vc);
}

// Free's the given VCD changeset.
static void vcd_free_changeset (struct vcd_changeset * cs) {
  struct vcd_change * vc, * vnext;
  for (vc = cs->changes; vc; vc = vnext) {
    vnext = vc->next;
    vcd_free_change (vc);
  }
  free (cs);
}

#if VCD_FAST

/* Returns changes sets in the list's order itself. */
static struct vcd_changeset *
vcd_find_firstset_fast (struct vcd_changeset * root) {
  static struct vcd_changeset * last = root;
  struct vcd_changeset * result = NULL;
  result = last;
  if (last) last = last->next;
  return result;
}

/* Reverses the given change set and returns the pointer to the new
   root change set. */
static struct vcd_changeset *
vcd_reverse_changesets (struct vcd_changeset * root) {
  struct vcd_changeset * cs = NULL;
  while (root != NULL) {
    struct vcd_changeset * next = root->next;
    root->next = cs;
    cs = root;
    root = next;
  }
  return cs;
}

#else

/* Returns the change set with the smallest time stamp which is yet
   unprocessed.  It is based upon the fact that the list is in reverse
   order. */
static struct vcd_changeset *
vcd_find_firstset (struct vcd_changeset * root) {
  struct vcd_changeset * cs, * result = NULL;
  double Min = root ? root->t : 0;
  for (cs = root; cs; cs = cs->next) {
    if (!cs->done && cs->t <= Min) {
      Min = cs->t;
      result = cs;
    }
  }
  return result;
}

#endif

/* Looks for the a variable name in the given list of variables. */
static struct vcd_variable *
vcd_find_variable (struct vcd_variable * root, struct vcd_vardef * var) {
  for (struct vcd_variable * vv = root; vv; vv = vv->next) {
    if (!strcmp (vv->code, var->code))
      return vv;
  }
  return NULL;
}

/* The function sorts the given VCD changesets.  For this it creates a
   new list of VCD sets.  Changesets with the same timestamp are
   merged appropriately. */
static void vcd_sort_changesets (struct vcd_changeset * root) {
  struct vcd_changeset * cs;
  struct vcd_set * vs, * current = NULL;
  struct vcd_change * vc;
  struct vcd_variable * vv;

#if VCD_FAST
  // for fast lookup, need to reverse list
  root = vcd_reverse_changesets (root);
#endif

  // as long as there are still changesets unprocessed
#if VCD_FAST
  while ((cs = vcd_find_firstset_fast (root)) != NULL) {
#else
  while ((cs = vcd_find_firstset (root)) != NULL) {
#endif
    // create set if necessary
    if (current == NULL || current->t != cs->t) {
      vs = (struct vcd_set *) calloc (1, sizeof (struct vcd_set));
      vs->t = cs->t;
      // chain the list of sets
      if (!current)
	vcd_sets = vs;
      else
	current->next = vs;
      current = vs;
    }

    // go through list of value changes
    for (vc = cs->changes; vc; vc = vc->next) {
      vv = vcd_find_variable (current->variables, vc->var);
      if (vv != NULL) {
	// duplicate value change
	vv->value = vc->value;
	vv->isreal = vc->isreal;
	if (current->t > 0) { // due to a $dumpvars before
	  fprintf (stderr, "vcd notice, duplicate value change at t = %g of "
		   "variable `%s'\n", current->t, vc->var->ident);
	}
      }
      else {
	// add variable to set
	vv = (struct vcd_variable *) calloc (1, sizeof (struct vcd_variable));
	vv->ident = vc->var->ident;
	vv->type = vc->var->type;
	vv->value = vc->value;
	vv->isreal = vc->isreal;
	vv->code = vc->code;
	vv->next = current->variables;
	current->variables = vv;
      }
    }
#ifndef VCD_FAST
    // changeset processed
    cs->done = 1;
#else
    //vcd_free_changeset (cs);
#endif
  }
}

/* Predends the scope identifiers in front of a variable identfier. */
static char *
vcd_prepend_scopes (struct vcd_vardef * var, char * ident) {
  struct vcd_scope * scope = var->scope;
  while (scope && scope != vcd->scopes) {
    if (vcd->scopes) {
      if (scope == vcd->scopes->scopes && vcd->scopes->vardefs == NULL) {
	/* skip top-scope description if there are no variables */
	scope = scope->parent;
	continue;
      }
    }
    char * txt = (char *) malloc (strlen (ident) + strlen (scope->ident) + 2);
    sprintf (txt, "%s.%s", scope->ident, ident);
    free (ident);
    ident = txt;
    scope = scope->parent;
  }
  return ident;
}

/* This function initially creates a dataset variable based on the
   given VCD variable. */
static struct dataset_variable *
vcd_create_variable (struct vcd_vardef * var) {
  struct dataset_variable * ds;
  int len = strlen (var->ident) + 1;

  ds = (struct dataset_variable *)
    calloc (1, sizeof (struct dataset_variable));
  ds->output = 1;

#if VCD_INCLUDE_RANGE
  if (var->range) len += 32;
#endif
  char * id1, * id2 = (char *) malloc (len);

  // using VCD produced by FreeHDL
  if (vcd_freehdl) {
    // skip these
    if (strstr (var->ident, "implicit_wait_for"))
      ds->output = 0;
    // correct nodes
    if (vcd_correct) {
      if (strstr (var->ident, "net") == var->ident) {
	id1 = strdup (&var->ident[3]);
      } else {
	id1 = strdup (var->ident);
      }
    }
    // keep nodes
    else {
      id1 = strdup (var->ident);
    }
  }
  // other than FreeHDL
  else {
    id1 = strdup (var->ident);
  }

#if VCD_INCLUDE_RANGE
  if (!var->range) {
    // no range
    sprintf (id2, "%s", id1);
  } else if (var->range->l == -1) {
    // one bit
    sprintf (id2, "%s[%d]", id1, var->range->h);
  } else if (var->range->h == -1) {
    // arrays
    sprintf (id2, "%s[%d]", id1, var->range->l);
  } else {
    // bit range
    sprintf (id2, "%s[%d:%d]", id1, var->range->l, var->range->h);
  }
#else
  sprintf (id2, "%s", id1);
#endif

#if VCD_SKIP_ARRAYS
  if (var->range && var->range->h == -1 && var->range->l != -1) {
    ds->output = 0;
  }
#endif

  ds->ident = strdup (id2);
  if (vcd_freehdl) {
    ds->ident = vcd_prepend_scopes (var, ds->ident);
  }

  free (id1);
  free (id2);
  return ds;
}

/* Based on the value in the given VCD variable and the size (in bits)
   the function returns a nicely formatted value for the dataset. */
static char * vcd_create_value (struct vcd_variable * vv, int size) {
  int i, len = strlen (vv->value);
  char * value;

  if (vv->type == VAR_REAL) {
    // a real
    char txt[64];
    double val = strtod (vv->value, NULL);
    sprintf (txt, "%+.11e", val);
    value = strdup (txt);
  } else if (vv->type == VAR_INTEGER) {
    // an integer
    char txt[64];
    long val = 0, bit, i = strlen (vv->value) - 1;
    for (bit = 1; i >= 0; i--, bit <<= 1) {
      if (vv->value[i] == '1')
	val |= bit;
      else if (vv->value[i] == '0')
	val &= ~bit;
    }
    sprintf (txt, "%+ld", val);
    value = strdup (txt);
    vv->isreal = 1;
  } else if (size == len) {
    // already good
    value = strdup (vv->value);
  } else {
    // fill left extending values for vectors
    value = (char *) calloc (1, size + 1);
    char fill;
    fill = (vv->value[0] == '1') ? '0' : vv->value[0];
    for (i = 0; i < size - len; i++) value[i] = fill;
    strcpy (&value[i], vv->value);
  }
  return value;
}

/* The function creates a full dataset variable for the given VCD
   variable. */
static struct dataset_variable *
vcd_create_dataset (struct vcd_vardef * var) {
  struct dataset_variable * ds;
  struct dataset_value * dv, * current = NULL;
  struct vcd_set * vs;
  struct vcd_variable * vv;
  char * currentval = NULL;
  int found;

  ds = vcd_create_variable (var);

  // go through all VCD sets
  for (vs = vcd_sets; vs; vs = vs->next) {
    found = 0;
    // through all variables in the set
    for (vv = vs->variables; vv; vv = vv->next) {
      if (!strcmp (vv->code, var->code)) {
	// found the variable in the set
	dv = (struct dataset_value *)
	  calloc (1, sizeof (struct dataset_value));
	dv->value = vcd_create_value (vv, var->size);
	// get value attribute
	ds->isreal = vv->isreal;
	// chain the list of values
	if (!current)
	  ds->values = dv;
	else
	  current->next = dv;
	current = dv;
	// save current value of the variable
	currentval = dv->value;
	found++;
	break;
      }
    }
    if (!found) {
      // variable did not occur in the current VCD set
      if (!currentval) {
	// no initial value given
	fprintf (stderr, "vcd error, variable `%s' has no initial value\n",
		 ds->ident);
	vcd_errors++;
      } else {
	// else: apply previous value
	dv = (struct dataset_value *)
	  calloc (1, sizeof (struct dataset_value));
	dv->value = strdup (currentval);
	// chain the list
	if (!current)
	  ds->values = dv;
	else
	  current->next = dv;
	current = dv;
      }
    }
  }
  return ds;
}

/* The function creates the independent (timestamps) variable.  It
   passes through all VCD changesets and collects the simulation
   times. */
static struct dataset_variable * vcd_create_indep (const char * name) {
  struct dataset_variable * ds;
  struct dataset_value * dv, * current = NULL;
  struct vcd_set * vs;

  // create dataset
  ds = (struct dataset_variable *)
    calloc (1, sizeof (struct dataset_variable));
  ds->ident = strdup (name);
  ds->output = 1;

  // go through all VCD sets
  for (vs = vcd_sets; vs; vs = vs->next) {
    dv = (struct dataset_value *) calloc (1, sizeof (struct dataset_value));
    ds->size++;
    // apply timestamp transformation
    char txt[64];
    sprintf (txt, "%+.11e", vs->t * vcd->t * vcd->scale);
    dv->value = strdup (txt);
    // chain the list
    if (!current)
      ds->values = dv;
    else
      current->next = dv;
    current = dv;
  }
  return ds;
}

/* The function creates a list of dataset for each VCD variable. */
static void vcd_prepare_variable_datasets (struct vcd_scope * root) {
  struct vcd_scope * scope;
  struct dataset_variable * data;
  // through each scope
  for (scope = root; scope; scope = scope->next) {
    struct vcd_vardef * var;
    // through each variable in this scope
    for (var = scope->vardefs; var; var = var->next) {
      data = vcd_create_dataset (var);
      data->type = DATA_DEPENDENT;
      data->dependencies = strdup (VCD_TIMEVAR);
      data->next = dataset_root;
      dataset_root = data;
    }
    vcd_prepare_variable_datasets (scope->scopes);
  }
}

/* The function creates a list of dataset for each VCD variable and
   for the independent (timestamps) variable as well. */
static void vcd_prepare_datasets (void) {
  struct dataset_variable * data;
  // the dependent variables
  vcd_prepare_variable_datasets (vcd->scopes);
  // the independent variable
  data = vcd_create_indep (VCD_TIMEVAR);
  data->type = DATA_INDEPENDENT;
  data->next = dataset_root;
  dataset_root = data;
}

#if VCD_DEBUG
// Debugging: Prints the VCD sets.
static void vcd_print_sets (void) {
  struct vcd_changeset * vs;
  struct vcd_change * vv;
  for (vs = vcd->changesets; vs; vs = vs->next) {
    fprintf (stderr, "--- t => %g\n", vs->t);
    for (vv = vs->changes; vv; vv = vv->next) {
      fprintf (stderr, "%s\t => %s\n", vv->value, vv->var->ident);
    }
  }
}

// Debugging: Prints the generate data sets.
static void vcd_dataset_print (void) {
  struct dataset_variable * ds;
  struct dataset_value * dv;
  for (ds = dataset_root; ds; ds = ds->next) {
    fprintf (stderr, "\n%s%s => %s\n",
	     ds->type == DATA_INDEPENDENT ? "in" : "",
	     ds->type == DATA_UNKNOWN ? "xxx" : "dep", ds->ident);
    for (dv = ds->values; dv; dv = dv->next) {
      fprintf (stderr, "  %s\n", dv->value);
    }
  }
}
#endif /* VCD_DEBUG */

/* This function is the overall VCD data checker.  It returns zero on
   success, non-zero otherwise. */
int vcd_checker (void) {

  // de-reference each value change identfier code
  struct vcd_changeset * changeset;
  for (changeset = vcd->changesets; changeset; changeset = changeset->next) {
    struct vcd_change * change;
    for (change = changeset->changes; change; change = change->next) {
      change->var = vcd_find_code (vcd->scopes, change->code);
      if (change->var == NULL) {
	fprintf (stderr, "vcd error, no such variable reference `%s' "
		 "found\n", change->code);
	vcd_errors++;
      }
    }
  }

  if (vcd_errors) return -1;

  // sort the VCD changesets
  vcd_sort_changesets (vcd->changesets);

  // create the outgoing datasets
  vcd_prepare_datasets ();

#if VCD_DEBUG
  vcd_print_sets ();
  vcd_dataset_print ();
#endif /* VCD_DEBUG */

  return vcd_errors ? -1 : 0;
}

// Free's the given scope root.
static void vcd_free_scope (struct vcd_scope * root) {
  struct vcd_scope * vs, * snext;
  for (vs = root; vs; vs = snext) {
    snext = vs->next;
    free (vs->ident);
    struct vcd_vardef * vv, * vnext;
    for (vv = vs->vardefs; vv; vv = vnext) {
      vnext = vv->next;
      free (vv->code);
      free (vv->ident);
      free (vv->range);
      free (vv);
    }
    vcd_free_scope (vs->scopes);
    free (vs);
  }
}

// Free's the given VCD file.
static void vcd_free_file (struct vcd_file * vcd) {
  vcd_free_scope (vcd->scopes);
  struct vcd_changeset * cs, * cnext;
  for (cs = vcd->changesets; cs; cs = cnext) {
    cnext = cs->next;
    vcd_free_changeset (cs);
  }
  free (vcd);
}

// Free's the given set of VCD changes.
static void vcd_free_set (struct vcd_set * root) {
  struct vcd_set * vs, * snext;
  for (vs = root; vs; vs = snext) {
    snext = vs->next;
    struct vcd_variable * vv, * vnext;
    for (vv = vs->variables; vv; vv = vnext) {
      vnext = vv->next;
      free (vv);
    }
    free (vs);
  }
}

// Free's the given dataset list.
static void vcd_free_dataset (struct dataset_variable * root) {
  struct dataset_variable * ds, * snext;
  for (ds = root; ds; ds = snext) {
    snext = ds->next;
    free (ds->ident);
    free (ds->dependencies);
    struct dataset_value * dv, * dnext;
    for (dv = ds->values; dv; dv = dnext) {
      dnext = dv->next;
      free (dv->value);
      free (dv);
    }
    free (ds);
  }
}

// Destroys data used by the VCD checker.
void vcd_destroy (void) {
  vcd_errors = 0;
  vcd_free_file (vcd);
  vcd = NULL;
  vcd_free_set (vcd_sets);
  vcd_sets = NULL;
  vcd_free_dataset (dataset_root);
  dataset_root = NULL;
}

// Initializes the VCD checker.
void vcd_init (void) {
  vcd = (struct vcd_file *) calloc (1, sizeof (struct vcd_file));
}
