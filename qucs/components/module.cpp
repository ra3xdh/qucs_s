/*
 * module.cpp - module class implementation
 *
 * Copyright (C) 2008, 2009, 2010 Stefan Jahn <stefan@lkcc.org>
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

#include "netdefs.h"
#include "components.h"
#include "analyses.h"
#include "netdefs.h"
#include "module.h"

// Global module hash.
qucs::hash<module> module::modules;

// Constructor creates an instance of the module class.
module::module () {
  definition = NULL;
  circreate = NULL;
  anacreate = NULL;
}

// Destructor deletes an instance of the module class.
module::~module () {
}

// Definitions which do not fit elsewhere.
static struct property_t props1[] = {
  PROP_NO_PROP };
static struct property_t props2[] = {
  { "Type", PROP_STR, { PROP_NO_VAL, "DEF1" }, PROP_NO_RANGE },
  PROP_NO_PROP };

struct define_t miscdef1 =
  { "Def", PROP_NODES, PROP_ACTION, PROP_NO_SUBSTRATE, PROP_LINEAR,
    props1, props1 };

struct define_t miscdef2 =
  { "Sub", PROP_NODES, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR,
    props1, props2 };

// Registers an analysis object to the list of available modules.
void module::registerModule (analysis_definer_t define,
			     analysis_creator_t create) {
  module * m = new module ();
  m->definition = define ();
  m->anacreate = create;
  modules.put ((char *) define()->type, m);
}

// Registers a circuit object to the list of available modules.
void module::registerModule (circuit_definer_t define,
			     circuit_creator_t create) {
  module * m = new module ();
  m->definition = define ();
  m->circreate = create;
  registerModule (define()->type, m);
}

// Registers a miscellaneous object to the list of available modules.
void module::registerModule (misc_definer_t define) {
  module * m = new module ();
  m->definition = define ();
  registerModule (define()->type, m);
}

/* Registers a miscellaneous structure just defined by a somple
   define_t structure to the list of available modules. */
void module::registerModule (struct define_t * define) {
  module * m = new module ();
  m->definition = define;
  registerModule (define->type, m);
}

// Puts a module into the available module hash.
void module::registerModule (const char * type, module * m) {
  if (modules.get ((char *) type) != NULL) {
    logprint (LOG_ERROR, "module already registered: %s\n", type);
  }
  else {
    modules.put ((char *) type, m);
  }
}

/* Returns the definition of a module specified by its type name if
   such is existing and otherwise NULL. */
struct define_t * module::getModule (char * type) {
  module * m = modules.get (type);
  if (m != NULL) {
    return m->definition;
  }
  return NULL;
}

// Helper macros.
#define REGISTER_CIRCUIT(val) \
  registerModule (&val::definition, &val::create)
#define REGISTER_ANALYSIS(val) \
  registerModule (&val::definition, &val::create)
#define REGISTER_MISC(val) \
  registerModule (&val::definition)

// Global static module registration.
void module::registerModules (void) {

  // miscellaneous
  registerModule (&miscdef1);
  registerModule (&miscdef2);
  REGISTER_MISC (nodeset);
  REGISTER_MISC (substrate);

  // circuit components
  REGISTER_CIRCUIT (resistor);
  REGISTER_CIRCUIT (capacitor);
  REGISTER_CIRCUIT (pac);
  REGISTER_CIRCUIT (inductor);
  REGISTER_CIRCUIT (vccs);
  REGISTER_CIRCUIT (cccs);
  REGISTER_CIRCUIT (vcvs);
  REGISTER_CIRCUIT (ccvs);
  REGISTER_CIRCUIT (biastee);
  REGISTER_CIRCUIT (dcfeed);
  REGISTER_CIRCUIT (dcblock);
  REGISTER_CIRCUIT (circulator);
  REGISTER_CIRCUIT (attenuator);
  REGISTER_CIRCUIT (isolator);
  REGISTER_CIRCUIT (trafo);
  REGISTER_CIRCUIT (strafo);
  REGISTER_CIRCUIT (vdc);
  REGISTER_CIRCUIT (idc);
  REGISTER_CIRCUIT (vac);
  REGISTER_CIRCUIT (iac);
  REGISTER_CIRCUIT (iexp);
  REGISTER_CIRCUIT (vexp);
  REGISTER_CIRCUIT (ifile);
  REGISTER_CIRCUIT (vfile);
  REGISTER_CIRCUIT (vam);
  REGISTER_CIRCUIT (vpm);
  REGISTER_CIRCUIT (vpulse);
  REGISTER_CIRCUIT (ipulse);
  REGISTER_CIRCUIT (vrect);
  REGISTER_CIRCUIT (irect);
  REGISTER_CIRCUIT (gyrator);
  REGISTER_CIRCUIT (phaseshifter);
  REGISTER_CIRCUIT (tswitch);
  REGISTER_CIRCUIT (relais);
  REGISTER_CIRCUIT (tline);
  REGISTER_CIRCUIT (tline4p);
  REGISTER_CIRCUIT (ctline);
  REGISTER_CIRCUIT (coaxline);
  REGISTER_CIRCUIT (rectline);
  REGISTER_CIRCUIT (twistedpair);
  REGISTER_CIRCUIT (rlcg);
  REGISTER_CIRCUIT (coupler);
  REGISTER_CIRCUIT (hybrid);
  REGISTER_CIRCUIT (diode);
  REGISTER_CIRCUIT (eqndefined);
  REGISTER_CIRCUIT (rfedd);
  REGISTER_CIRCUIT (diac);
  REGISTER_CIRCUIT (thyristor);
  REGISTER_CIRCUIT (triac);
  REGISTER_CIRCUIT (tunneldiode);
  REGISTER_CIRCUIT (msline);
  REGISTER_CIRCUIT (mscorner);
  REGISTER_CIRCUIT (msstep);
  REGISTER_CIRCUIT (msopen);
  REGISTER_CIRCUIT (msgap);
  REGISTER_CIRCUIT (msmbend);
  REGISTER_CIRCUIT (mscoupled);
  REGISTER_CIRCUIT (mslange);  
  REGISTER_CIRCUIT (mstee);
  REGISTER_CIRCUIT (mscross);
  REGISTER_CIRCUIT (msvia);
  REGISTER_CIRCUIT (msrstub);
  REGISTER_CIRCUIT (bondwire);
  REGISTER_CIRCUIT (cpwline);
  REGISTER_CIRCUIT (cpwopen);
  REGISTER_CIRCUIT (cpwshort);
  REGISTER_CIRCUIT (cpwgap);
  REGISTER_CIRCUIT (cpwstep);
  REGISTER_CIRCUIT (iprobe);
  REGISTER_CIRCUIT (vprobe);
  REGISTER_CIRCUIT (jfet);
  REGISTER_CIRCUIT (bjt);
  REGISTER_CIRCUIT (spfile);
  REGISTER_CIRCUIT (vnoise);
  REGISTER_CIRCUIT (inoise);
  REGISTER_CIRCUIT (mosfet);
  REGISTER_CIRCUIT (amplifier);
  REGISTER_CIRCUIT (opamp);
  REGISTER_CIRCUIT (iinoise);
  REGISTER_CIRCUIT (mutual);
  REGISTER_CIRCUIT (mutual2);
  REGISTER_CIRCUIT (mutualx);
  REGISTER_CIRCUIT (ivnoise);
  REGISTER_CIRCUIT (vvnoise);
  REGISTER_CIRCUIT (inverter);
  REGISTER_CIRCUIT (logicnor);
  REGISTER_CIRCUIT (logicor);
  REGISTER_CIRCUIT (logicnand);
  REGISTER_CIRCUIT (logicand);
  REGISTER_CIRCUIT (logicxnor);
  REGISTER_CIRCUIT (logicxor);
  REGISTER_CIRCUIT (digisource);
  REGISTER_CIRCUIT (buffer);
  REGISTER_CIRCUIT (hicumL2V2p1);
  REGISTER_CIRCUIT (HBT_X);
  REGISTER_CIRCUIT (mod_amp);
  REGISTER_CIRCUIT (hic2_full);
  REGISTER_CIRCUIT (log_amp);
  REGISTER_CIRCUIT (hic0_full);
  REGISTER_CIRCUIT (potentiometer);
  REGISTER_CIRCUIT (MESFET);
  REGISTER_CIRCUIT (EKV26MOS);
  REGISTER_CIRCUIT (hicumL0V1p2);
  REGISTER_CIRCUIT (hicumL0V1p2g);
  REGISTER_CIRCUIT (hicumL0V1p3);
  REGISTER_CIRCUIT (hicumL2V2p23);
  REGISTER_CIRCUIT (hicumL2V2p24);
  REGISTER_CIRCUIT (hicumL2V2p31n);
  REGISTER_CIRCUIT (photodiode);
  REGISTER_CIRCUIT (phototransistor);
  REGISTER_CIRCUIT (nigbt);
  REGISTER_CIRCUIT (dff_SR);
  REGISTER_CIRCUIT (tff_SR);
  REGISTER_CIRCUIT (jkff_SR);
  REGISTER_CIRCUIT (gatedDlatch);
  REGISTER_CIRCUIT (logic_1);
  REGISTER_CIRCUIT (logic_0);
  REGISTER_CIRCUIT (mux2to1);
  REGISTER_CIRCUIT (mux4to1);
  REGISTER_CIRCUIT (mux8to1);
  REGISTER_CIRCUIT (DLS_nto1);
  REGISTER_CIRCUIT (DLS_1ton);
  REGISTER_CIRCUIT (andor4x2);
  REGISTER_CIRCUIT (andor4x3);
  REGISTER_CIRCUIT (andor4x4);
  REGISTER_CIRCUIT (dmux2to4);
  REGISTER_CIRCUIT (dmux3to8);
  REGISTER_CIRCUIT (dmux4to16);
  REGISTER_CIRCUIT (ha1b);
  REGISTER_CIRCUIT (fa1b);
  REGISTER_CIRCUIT (fa2b);
  REGISTER_CIRCUIT (pad2bit);
  REGISTER_CIRCUIT (pad3bit);
  REGISTER_CIRCUIT (pad4bit);
  REGISTER_CIRCUIT (binarytogrey4bit);
  REGISTER_CIRCUIT (greytobinary4bit);
  REGISTER_CIRCUIT (comp_1bit);
  REGISTER_CIRCUIT (comp_2bit);
  REGISTER_CIRCUIT (comp_4bit);
  REGISTER_CIRCUIT (hpribin4bit);

  // analyses
  REGISTER_ANALYSIS (dcsolver);
  REGISTER_ANALYSIS (acsolver);
  REGISTER_ANALYSIS (spsolver);
  REGISTER_ANALYSIS (trsolver);
  REGISTER_ANALYSIS (hbsolver);
  REGISTER_ANALYSIS (parasweep);
}

// Global module unregistration.
void module::unregisterModules (void) {
  qucs::hashiterator<module> it;
  for (it = qucs::hashiterator<module> (modules); *it; ++it) {
    delete it.currentVal ();
  }
  modules.clear ();
}

#if DEBUG
// header prefix
static const char * def_prefix =
"/*\n"
" * qucsdefs.h - netlist definitions for the Qucs netlists\n"
" *\n"
" * This is free software; you can redistribute it and/or modify\n"
" * it under the terms of the GNU General Public License as published by\n"
" * the Free Software Foundation; either version 2, or (at your option)\n"
" * any later version.\n"
" * \n"
" */\n"
"\n"
"#ifndef __QUCSDEFS_H__\n"
"#define __QUCSDEFS_H__\n";

// header suffix
static const char * def_suffix =
"\n"
"#endif /* __QUCSDEFS_H__ */\n";

// start of list of definitions
static const char * def_start =
"\n"
"// List of available components.\n"
"struct define_t qucs_definition_available[] =\n";

// end of list entry
static const char * def_stop =
"\n"
"static struct define_t def_End = {\n"
"  ((char *) 0), -1, 1, 0, 0, req_Def, opt_Def };\n";

// Returns a compilable C-code string made from the given string.
static char * printstr (const char * str) {
  static char txt[256];
  int nostr = (str == PROP_NO_STR);
  sprintf (txt, "%s%s%s",
	   (str && !nostr) ? "\"" : "",
	   str ? nostr ? "((char *) -1)" : str : "((char *) 0)",
	   (str && !nostr) ? "\"" : "");
  return txt;
}

// Prints a property list as compilable C-code.
static void printprop (const char * type, const char * prefix,
		       struct property_t * prop) {
  const char * key;
  const char ** str;
  const char * txt;
  fprintf (stdout, "static struct property_t %s_%s[] = {\n", prefix, type);
  do {
    key = prop->key;
    fprintf (stdout, "  { %s, %d, ", printstr (key), prop->type);
    fprintf (stdout, "{ %g, %s }, ", prop->defaultval.d,
	     printstr (prop->defaultval.s));
    fprintf (stdout, "{ '%c', %g, %g, '%c',\n",
	     prop->range.il, prop->range.l, prop->range.h, prop->range.ih);
    fprintf (stdout, "    {");
    str = prop->range.str;
    do {
      txt = *str;
      fprintf (stdout, " %s", printstr (txt));
      if (txt) fprintf (stdout, ",");
      str++;
    }
    while (txt != NULL);
    fprintf (stdout, " } } }");
    if (key) fprintf (stdout, ",");
    fprintf (stdout, "\n");
    prop++;
  }
  while (key != NULL);
  fprintf (stdout, "};\n");
}

/* The function emits a complete list of the registered component
   definitions as compilable C-code. */
void module::print (void) {
  fprintf (stdout, def_prefix);
  qucs::hashiterator<module> it;
  for (it = qucs::hashiterator<module> (modules); *it; ++it) {
    module * m = it.currentVal ();
    struct define_t * def = m->definition;
    fprintf (stdout, "\n");
    printprop (def->type, "req", def->required);
    fprintf (stdout, "\n");
    printprop (def->type, "opt", def->optional);
    fprintf (stdout, "\n");
    fprintf (stdout, "static struct define_t def_%s = {\n", def->type);
    fprintf (stdout, "  %s, %d, %d, %d, %d, req_%s, opt_%s };\n",
	     printstr (def->type), def->nodes, def->action, def->substrate,
	     def->nonlinear, def->type, def->type);
  }
  fprintf (stdout, def_stop);
  fprintf (stdout, def_start);
  fprintf (stdout, "{\n");
  for (it = qucs::hashiterator<module> (modules); *it; ++it) {
    module * m = it.currentVal ();
    struct define_t * def = m->definition;
    fprintf (stdout, "  def_%s,\n", def->type);
  }
  fprintf (stdout, "  def_End\n");
  fprintf (stdout, "};\n");
  fprintf (stdout, def_suffix);
}
#endif /* DEBUG */
