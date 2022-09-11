/*
 * check_netlist.cpp - checker for the Qucs netlist
 *
 * Copyright (C) 2003-2009 Stefan Jahn <stefan@lkcc.org>
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

#include "logging.h"
#include "strlist.h"
#include "netdefs.h"
#include "equation.h"
#include "check_netlist.h"
#include "constants.h"
#include "environment.h"
#include "variable.h"
#include "module.h"

using namespace qucs;
using namespace qucs::eqn;

/* Global definitions for parser and checker. */
struct definition_t * definition_root = NULL;
struct definition_t * subcircuit_root = NULL;
environment * env_root = NULL;

/* The function counts the nodes in a definition line. */
static int checker_count_nodes (struct definition_t * def)
{
    int count = 0;
    for (struct node_t * node = def->nodes; node != NULL; node = node->next)
        count++;
    return count;
}

/* The function returns an available definition line for the given
   type.  If there is no such definition type the function returns
   NULL. */
static struct define_t * checker_find_definition (char * type, int action)
{
    struct define_t * def = module::getModule (type);
    if (def != NULL && action == def->action) return def;
    return NULL;
}

/* The function returns the number of properties in a definition line
   specified by the given key. */
static int checker_find_property (const char * key, struct pair_t * pp)
{
    int count = 0;
    while (pp != NULL)
    {
        if (!strcmp (pp->key, key))
            count++;
        pp = pp->next;
    }
    return count;
}

/* Checks if the given property key is either optional or required for
   the given definition type and returns the type of the property. */
static int checker_is_property (struct define_t * available,
                                const char * key)
{
    int i;
    for (i = 0; PROP_IS_PROP (available->required[i]); i++)
    {
        if (!strcmp (available->required[i].key, key))
            return available->required[i].type;
    }
    for (i = 0; PROP_IS_PROP (available->optional[i]); i++)
    {
        if (!strcmp (available->optional[i].key, key))
            return available->optional[i].type;
    }
    return PROP_NONE;
}

/* Counts the number of definitions given by the specified type and
   instance name in the definition list. */
static int checker_count_definition (struct definition_t * root,
                                     const char * type, char * instance)
{
    int count = 0;
    for (struct definition_t * def = root; def != NULL; def = def->next)
    {
        if (!strcmp (def->type, type) && !strcmp (def->instance, instance))
        {
            if (++count > 1)
                def->duplicate = 1;
        }
    }
    return count;
}

/* Returns the value for a given definition type, key and variable
   identifier if it is in the list of definitions.  Otherwise the
   function returns NULL. */
static struct value_t * checker_find_variable (struct definition_t * root,
        const char * type,
        const char * key,
        char * ident)
{
    struct pair_t * pair;
    for (struct definition_t * def = root; def != NULL; def = def->next)
    {
        if (!strcmp (def->type, type))
        {
            for (pair = def->pairs; pair != NULL; pair = pair->next)
            {
                if (!strcmp (pair->key, key))
                    if (pair->value->ident != NULL && ident != NULL &&
                            strcmp (pair->value->ident, ident) == 0)
                        return pair->value;
            }
        }
    }
    return NULL;
}

/* The function returns the appropriate value for a given key within
   the given netlist definition if the value is a reference (a
   string).  If there is no such key value pair the function returns
   NULL. */
static struct value_t * checker_find_reference (struct definition_t * def,
        const char * key)
{
    struct pair_t * pair;
    for (pair = def->pairs; pair != NULL; pair = pair->next)
    {
        if (!strcmp (pair->key, key))
            if (pair->value->ident != NULL)
                return pair->value;
    }
    return NULL;
}

/* The function looks for the given property key within the properties
   of the given definition line and returns its value if the property
   is not an identifier.  Otherwise the function returns NULL. */
static struct value_t * checker_find_prop_value (struct definition_t * def,
        const char * key)
{
    struct pair_t * pair;
    for (pair = def->pairs; pair != NULL; pair = pair->next)
    {
        if (!strcmp (pair->key, key))
            if (pair->value->ident == NULL)
                return pair->value;
    }
    return NULL;
}

/* The function returns the number of properties in a definition line
   specified by the given key. */
static int checker_find_property (struct definition_t * def,
                                  const char * key)
{
    return checker_find_property (key, def->pairs);
}

/* This function looks for the specified property 'key' in the given
   definition and return its value (a string reference) if it is a
   reference.  Otherwise the function returns NULL and emits an
   appropriate error message. */
static struct value_t * checker_validate_reference (struct definition_t * def,
        const char * key)
{
    struct value_t * val;
    if ((val = checker_find_reference (def, key)) == NULL)
    {
        logprint (LOG_ERROR, "line %d: checker error, not a valid `%s' property "
                  "found in `%s:%s'\n", def->line, key, def->type, def->instance);
    }
    return val;
}

/* This function checks whether the given definition is a known
   microstrip component with a substrate definition.  If the given
   identifier equals this substrate definition then the function
   returns the appropriate value.  Otherwise it returns NULL. */
static struct value_t * checker_find_substrate (struct definition_t * def,
        char * ident)
{
    struct value_t * val;
    if (checker_is_property (def->define, "Subst") == PROP_STR)
    {
        if ((val = checker_find_reference (def, "Subst")) != NULL)
        {
            if (ident != NULL && !strcmp (val->ident, ident))
                return val;
        }
    }
    return NULL;
}

/* Puts the given double value variable into an environment. */
static variable * checker_add_variable (environment * env,
                                        char * var, int type, bool pass)
{
    variable * v = new variable (var);
    eqn::constant * c = new eqn::constant (type);
    switch (type)
    {
    case TAG_DOUBLE:
        c->d = 0.0; // initialize the variable
        break;
    case TAG_VECTOR:
	  c->v = new qucs::vector ();
        break;
    }
    v->setConstant (c);
    env->addVariable (v, pass);
    return v;
}

/* Resolves the variable of a property value.  Returns non-zero on
   success, otherwise zero. */
static int checker_resolve_variable (struct definition_t * root,
                                     struct definition_t * def,
                                     struct pair_t * pair, int type)
{
    struct value_t * val;
    struct value_t * value = pair->value;
    if (value->ident != NULL)
    {
        int found = 0;
        /* 1. find variable in parameter sweeps */
        if ((val = checker_find_variable (root, "SW", "Param", value->ident)))
        {
            /* add parameter sweep variable to environment */
            if (!strcmp (def->type, "SW") && !strcmp (pair->key, "Param"))
            {
                checker_add_variable (root->env, value->ident, TAG_DOUBLE, true);
            }
            /* mark both the variable identifier and the parameter sweep
            variable to be actually variables */
            val->var = TAG_DOUBLE;
            value->var = TAG_DOUBLE;
            found++;
        }
        /* 2. find analysis in parameter sweeps */
        if ((val = checker_find_variable (root, "SW", "Sim", value->ident)))
        {
            found++;
        }
        /* 3. find substrate in microstrip components */
        if ((val = checker_find_substrate (def, value->ident)))
        {
            value->subst = 1;
            found++;
        }
        /* 4. find subcircuit definition in subcircuit components */
        if ((val = checker_find_variable (root, "Sub", "Type", value->ident)))
        {
            found++;
        }
        /* 5. find special identifiers in certain properties */
        if (value->range)
        {
            found++;
        }
        /* 6. find file reference in S-parameter file components */
        if ((val = checker_find_variable (root, "SPfile", "File", value->ident)))
        {
            found++;
        }
        /* 6a. find file reference in S-parameter de-embedding file components */
        if ((val = checker_find_variable (root, "SPDfile", "File", value->ident)))
        {
            found++;
        }
        /* 7. find variable in equation */
        if (root->env)
        {
            if (root->env->getChecker()->containsVariable (value->ident))
            {
                variable * v;
                value->var = (type == PROP_LIST) ? TAG_VECTOR : TAG_DOUBLE;
                if ((v = root->env->getVariable (value->ident)) == NULL)
                {
                    // put variable into the environment
                    checker_add_variable (root->env, value->ident, value->var, false);
                }
                found++;
            }
        }
        /* 8. find file reference in file based sources */
        if ((val = checker_find_variable (root, "Vfile", "File", value->ident)))
        {
            found++;
        }
        if ((val = checker_find_variable (root, "Ifile", "File", value->ident)))
        {
            found++;
        }
        /* 9. find property reference in the instance */
        if (!found &&
                checker_is_property (def->define, value->ident) != PROP_NONE)
        {
            if (root->env)
            {

                // create reference variable names
                char * txt = (char *)
                             malloc (strlen (def->instance) + 1 + strlen (value->ident) + 1);
                sprintf (txt, "%s.%s", def->instance, value->ident);
                char * ref = (char *)
                             malloc (strlen (def->instance) + 5 + strlen (value->ident) + 1);
                sprintf (ref, "%s.%s.ref", def->instance, value->ident);

                // replace property string
                free (value->ident);
                value->ident = strdup (ref);
                value->var = TAG_DOUBLE;

                // already done previously?
                variable * v;
                if ((v = root->env->getVariable (ref)) == NULL)
                {
                    // put variable into the environment
                    checker_add_variable (root->env, ref, TAG_DOUBLE, false);
                    // also add reference equation into environment
                    root->env->getChecker()->addReference ("#propref", ref, txt);
                }

                // done
                free (txt);
                free (ref);
                found++;
            }
        }
        /* not found */
        if (!found)
        {
            logprint (LOG_ERROR, "line %d: checker error, no such variable `%s' "
                      "used in a `%s:%s' property\n", def->line, value->ident,
                      def->type, def->instance);
            return 0;
        }
    }
    return 1;
}

/* Evaluates the unit scale in a property value.  It adjusts the
   actual value and omits the scale.  The function returns non-zero
   on success and zero otherwise. */
static int checker_evaluate_scale (struct value_t * value)
{
    double val = value->value, factor = 1.0;
    char * scale;
    if (value->scale != NULL)
    {
        scale = value->scale;
        switch (*scale)
        {
        case 'E':
            scale++;
            factor = 1e18;
            break;
        case 'P':
            scale++;
            factor = 1e15;
            break;
        case 'T':
            scale++;
            factor = 1e12;
            break;
        case 'G':
            scale++;
            factor = 1e9;
            break;
        case 'M':
            scale++;
            factor = 1e6;
            break;
        case 'k':
            scale++;
            factor = 1e3;
            break;
        case 'm':
            scale++;
            if (*scale == 'i')
            {
                scale++;
                if (*scale == 'l')
                {
                    scale++;
                    factor = 2.54e-5;
                }
            }
            else
                factor = 1e-3;
            break;
        case 'u':
            scale++;
            factor = 1e-6;
            break;
        case 'n':
            scale++;
            factor = 1e-9;
            break;
        case 'p':
            scale++;
            factor = 1e-12;
            break;
        case 'f':
            scale++;
            if (*scale == 't')
            {
                scale++;
                factor = 0.3048;
            }
            else
                factor = 1e-15;
            break;
        case 'a':
            scale++;
            factor = 1e-18;
            break;
        case 'd':
            scale++;
            if (*scale == 'B')
            {
                scale++;
                val = std::pow (10.0, val / 10.0);
                if (*scale == 'm')
                {
                    scale++;
                    factor = 1e-3;
                }
                else if (*scale == 'u')
                {
                    scale++;
                    factor = 1e-6;
                }
            }
            break;
        case 'i':
            scale++;
            if (*scale == 'n')
            {
                scale++;
                factor = 2.54e-2;
            }
            break;
        case 'y':
            scale++;
            if (*scale == 'd')
            {
                scale++;
                factor = 0.9144;
            }
            break;
        }
        if (*scale != '\0')
        {
            value->unit = strdup (scale);
        }
        free (value->scale);
        value->scale = NULL;
    }
    value->value = val * factor;
    return 1;
}

/* The function returns the number of instances of the given type within
   the list of definitions. */
static int checker_count_definitions (struct definition_t * root,
                                      const char * type, int action)
{
    int count = 0;
    for (struct definition_t * def = root; def != NULL; def = def->next)
    {
        if (def->action == action)
        {
            if (type == NULL)
                count++;
            else if (!strcmp (def->type, type))
                count++;
        }
    }
    return count;
}

/* This function looks for the specified subcircuit type in the list
   of available subcircuits and returns its definition.  If there is
   no such subcircuit the function returns NULL: */
static struct definition_t * checker_find_subcircuit (char * n)
{
    struct definition_t * def;
    for (def = subcircuit_root; def != NULL; def = def->next)
        if (n != NULL && !strcmp (def->instance, n)) return def;
    return NULL;
}

/* The function returns the subcircuit definition for the given
   subcircuit instance. */
static struct definition_t *
checker_get_subcircuit (struct definition_t * def)
{
    struct value_t * val;
    struct definition_t * sub = NULL;
    if ((val = checker_find_reference (def, "Type")) != NULL)
        sub = checker_find_subcircuit (val->ident);
    return sub;
}

// Global variable indicating cycles in subcircuit definitions.
static int checker_sub_cycles = 0;

/* The following function returns the number of circuit instances
   requiring a DC analysis (being nonlinear) in the list of definitions. */
static int checker_count_nonlinearities (struct definition_t * root)
{
    int count = 0;
    struct definition_t * sub;
    for (struct definition_t * def = root; def != NULL; def = def->next)
    {
        if (def->nonlinear != 0) count++;
        // also recurse into subcircuits if possible
        if (checker_sub_cycles <= 0)
        {
            if (!strcmp (def->type, "Sub"))
            {
                if ((sub = checker_get_subcircuit (def)) != NULL)
                {
                    count += checker_count_nonlinearities (sub->sub);
                }
            }
        }
    }
    return count;
}

/* This function returns the number of action definitions with the
   given instance name. */
static int checker_count_action (struct definition_t * root, char * instance)
{
    int count = 0;
    for (struct definition_t * def = root; def != NULL; def = def->next)
    {
        if (def->action == 1 && !strcmp (def->instance, instance))
            count++;
    }
    return count;
}

/* This (recursive) function detects any kind of cyclic definitions of
   parameter sweeps for the given instance name.  The string list
   argument is used to pass the dependencies.  The function returns
   zero if the parameter sweep in non-cyclic. */
static int checker_validate_para_cycles (struct definition_t * root,
        char * instance, strlist * deps)
{
    int errors = 0;
    struct value_t * val;
    for (struct definition_t * def = root; def != NULL; def = def->next)
    {
        /* find the appropriate definition for the given instance */
        if (def->action == 1 && !strcmp (def->instance, instance))
        {
            /* emit error message if the instance is already in the dependencies */
            if (deps->contains (instance))
            {
                logprint (LOG_ERROR, "checker error, cyclic definition of `%s' "
                          "detected, involves: %s\n", instance, deps->toString ());
                return ++errors;
            }
            deps->append (instance);
            /* recurse into parameter sweeps */
            if (!strcmp (def->type, "SW"))
            {
                if ((val = checker_find_reference (def, "Sim")) != NULL)
                {
                    return checker_validate_para_cycles (root, val->ident, deps);
                }
            }
        }
    }
    return errors;
}

/* This function validates each parameter sweep within the list of
   definitions and return non-zero on errors.  Emits appropriate error
   messages. */
static int checker_validate_para (struct definition_t * root)
{
    int errors = 0;
    struct value_t * val;
    for (struct definition_t * def = root; def != NULL; def = def->next)
    {
        /* find parameter sweep */
        if (def->action == 1 && !strcmp (def->type, "SW"))
        {
            /* the 'Sim' property must be an identifier */
            if ((val = checker_validate_reference (def, "Sim")) == NULL)
            {
                errors++;
            }
            else
            {
                /* check for self-referring sweeps */
                if (!strcmp (def->instance, val->ident))
                {
                    logprint (LOG_ERROR, "line %d: checker error, definition `%s:%s' "
                              "refers to itself\n", def->line, def->type, def->instance);
                    errors++;
                }
                /* look for the referred analysis action definition */
                if (checker_count_action (root, val->ident) != 1)
                {
                    logprint (LOG_ERROR, "line %d: checker error, no such action `%s' "
                              "found as referred in `%s:%s'\n", def->line, val->ident,
                              def->type, def->instance);
                    errors++;
                }
                /* finally detect cyclic definitions */
                strlist * deps = new strlist ();
                errors += checker_validate_para_cycles (root, val->ident, deps);
                delete deps;
            }
        }
    }
    return errors;
}

/* This function returns the next port definition in the given list of
   definitions or NULL if there is no such definition. */
static struct definition_t * checker_find_port (struct definition_t * root)
{
    for (struct definition_t * def = root; def != NULL; def = def->next)
    {
        if (def->action == PROP_COMPONENT && !strcmp (def->type, "Pac"))
        {
            return def;
        }
    }
    return NULL;
}

/* This function checks whether the port numbers for the S-parameter
   analysis are unique or not.  It returns zero on success and
   non-zero if it detected duplicate entries. */
static int checker_validate_ports (struct definition_t * root)
{
    int p, errors = 0;
    struct value_t * val;
    struct definition_t * port, * def = root;
    const char * prop = "Num";
    while ((def = checker_find_port (def)) != NULL)
    {
        if ((val = checker_find_prop_value (def, prop)) != NULL)
        {
            p = (int) val->value;
            port = root;
            while ((port = checker_find_port (port)) != NULL)
            {
                if (port != def)
                {
                    if ((val = checker_find_prop_value (port, prop)) != NULL)
                    {
                        if (p == (int) val->value)
                        {
                            logprint (LOG_ERROR, "line %d: checker error, `%s' definitions "
                                      "with duplicate `%s=%d' property found: `%s:%s' and "
                                      "`%s:%s'\n", def->line, def->type, prop, p, def->type,
                                      def->instance, port->type, port->instance);
                            errors++;
                        }
                    }
                }
                port = port->next;
            }
        }
        def = def->next;
    }
    return errors;
}

/* The following function checks whether the parametric sweeps in the
   netlist are valid or not.  It returns zero on success and non-zero
   otherwise. */
static int checker_validate_lists (struct definition_t * root)
{
    int errors = 0;
    // go through each definition
    for (struct definition_t * def = root; def != NULL; def = def->next)
    {
        /* sweeps possible in parameter sweep, ac-analysis and
           s-parameter analysis */
        if (def->action == 1 && (!strcmp (def->type, "SW") ||
                                 !strcmp (def->type, "AC") ||
                                 !strcmp (def->type, "SP")))
        {
            struct value_t * val;
            if((val = checker_find_reference (def, "Type")) == NULL) {
                logprint (LOG_ERROR, "line %d: checker error, required property "
                          "`%s' is invalid in `%s:%s'\n", def->line, "Type",
                          def->type, def->instance);
                errors++;
                continue;
            }
            char * type = val->ident;
            // list of constant values and constant values
            if (type && (!strcmp (type, "const") || !strcmp (type, "list")))
            {
                // property 'Values' is required
                if ((val = checker_find_prop_value (def, "Values")) == NULL)
                {
                    if (!strcmp (type, "const"))
                    {
                        if ((val = checker_validate_reference (def, "Values")) == NULL)
                        {
                            errors++;
                        }
                    }
                    else
                    {
                        logprint (LOG_ERROR, "line %d: checker error, required property "
                                  "`%s' not found in `%s:%s'\n", def->line, "Values",
                                  def->type, def->instance);
                        errors++;
                    }
                }
                else
                {
                    if (!strcmp (type, "const"))
                    {
                        // in constant sweeps only one value allowed
                        if (val->next != NULL)
                        {
                            logprint (LOG_ERROR, "line %d: checker error, value of `%s' "
                                      "needs to be a single constant value in `%s:%s', no "
                                      "lists possible\n", def->line, "Values",
                                      def->type, def->instance);
                            errors++;
                        }
                        val->var = TAG_UNKNOWN;
                    }
                    if (!strcmp (type, "list"))
                    {
                        val->var = TAG_VECTOR;
                    }
                    // check and evaluate the unit scale in a value list
                    for (; val != NULL; val = val->next)
                    {
                        if (!checker_evaluate_scale (val))
                            errors++;
                    }
                }
                // property 'Start' is invalid
                if (checker_find_property (def, "Start") > 0)
                {
                    logprint (LOG_ERROR, "line %d: checker error, extraneous property "
                              "`%s' is invalid in `%s:%s'\n", def->line, "Start",
                              def->type, def->instance);
                    errors++;
                }
                // property 'Stop' is invalid
                if (checker_find_property (def, "Stop") > 0)
                {
                    logprint (LOG_ERROR, "line %d: checker error, extraneous property "
                              "`%s' is invalid in `%s:%s'\n", def->line, "Stop",
                              def->type, def->instance);
                    errors++;
                }
                // property 'Points' is also invalid
                if (checker_find_property (def, "Points") > 0)
                {
                    logprint (LOG_ERROR, "line %d: checker error, extraneous property "
                              "`%s' is invalid in `%s:%s'\n", def->line, "Points",
                              def->type, def->instance);
                    errors++;
                }
            }
            // linearly and logarithmically stepped sweeps
            else if (type && (!strcmp (type, "lin") || !strcmp (type, "log")))
            {
                // property 'Start' required
                if (checker_find_property (def, "Start") <= 0)
                {
                    logprint (LOG_ERROR, "line %d: checker error, required property "
                              "`%s' not found in `%s:%s'\n", def->line, "Start",
                              def->type, def->instance);
                    errors++;
                }
                // property 'Stop' required
                if (checker_find_property (def, "Stop") <= 0)
                {
                    logprint (LOG_ERROR, "line %d: checker error, required property "
                              "`%s' not found in `%s:%s'\n", def->line, "Stop",
                              def->type, def->instance);
                    errors++;
                }
                // property 'Points' is also required
                if (checker_find_property (def, "Points") <= 0)
                {
                    logprint (LOG_ERROR, "line %d: checker error, required property "
                              "`%s' not found in `%s:%s'\n", def->line, "Points",
                              def->type, def->instance);
                    errors++;
                }
                // property 'Values' is invalid
                if (checker_find_property (def, "Values") > 0)
                {
                    logprint (LOG_ERROR, "line %d: checker error, extraneous property "
                              "`%s' is invalid in `%s:%s'\n", def->line, "Values",
                              def->type, def->instance);
                    errors++;
                }
            }
        }
    }
    return errors;
}

/* This function checks the actions to be taken in the netlist.  It
   returns zero on success, non-zero otherwise. */
static int checker_validate_actions (struct definition_t * root)
{
    int a, c, n, errors = 0;
    if ((n = checker_count_definitions (root, NULL, 1)) < 1)
    {
        logprint (LOG_ERROR, "checker error, no actions defined: nothing to do\n");
        errors++;
    }
    else
    {
        // check requirements for s-parameter analysis
        if ((a = checker_count_definitions (root, "SP", 1)) >= 1)
        {
            if ((n = checker_count_definitions (root, "Pac", 0)) < 1)
            {
                logprint (LOG_ERROR, "checker error, %d `Pac' definitions found, at "
                          "least 1 required\n", n);
                errors++;
            }
        }
        // count analyses requiring a DC solution
        a += checker_count_definitions (root, "AC", 1);
        // check dc-analysis requirements
        c = checker_count_nonlinearities (root);
        n = checker_count_definitions (root, "DC", 1);
        if (n > 1)
        {
            logprint (LOG_ERROR, "checker error, the .DC action is defined %dx, "
                      "single or none required\n", n);
            errors++;
        }
        if (a >= 1 && c >= 1 && n < 1)
        {
            logprint (LOG_ERROR, "checker error, a .DC action is required for this "
                      "circuit definition (accounted %d non-linearities)\n", c);
            errors++;
        }
    }
    errors += checker_validate_para (root);
    errors += checker_validate_ports (root);
    errors += checker_validate_lists (root);
    return errors;
}

/* This function checks the validity of each microstrip component and
   its substrate and model references.  It returns zero on success,
   emit error messages if necessary and returns non-zero on errors. */
static int checker_validate_strips (struct definition_t * root)
{
    int errors = 0;
    struct value_t * val;
    for (struct definition_t * def = root; def != NULL; def = def->next)
    {
        if (!def->action)
        {
            /* find components with substrate property */
            if (def->define && (checker_is_property (def->define, "Subst") == PROP_STR))
            {
                /* check validity of 'Subst' property */
                if ((val = checker_validate_reference (def, "Subst")) == NULL)
                {
                    errors++;
                }
                else
                {
                    if (checker_count_definition (root, "SUBST", val->ident) != 1)
                    {
                        logprint (LOG_ERROR, "line %d: checker error, no such substrate "
                                  "`%s' found as specified in `%s:%s'\n", def->line,
                                  val->ident, def->type, def->instance);
                        errors++;
                    }
                }
                /* check validity of 'Model' property */
#if DISABLE_FOR_NOW /* ThinkME!!! */
                if ((val = checker_validate_reference (def, "Model")) == NULL)
                {
                    errors++;
                }
#endif
            }
        }
    }
    return errors;
}

/* This function counts the number of occurrences of the given node
   name in the given netlist definition root. */
static int checker_count_nodes (struct definition_t * root, char * n)
{
    int count = 0;
    struct node_t * node;
    for (struct definition_t * def = root; def != NULL; def = def->next)
    {
        if (!def->action && !def->nodeset)
        {
            for (node = def->nodes; node != NULL; node = node->next)
                if (!strcmp (node->node, n)) count++;
        }
    }
    return count;
}

/* The function identifies duplicate nodesets for the same node which
   is not allowed.  It returns the number of duplications. */
static int checker_count_nodesets (struct definition_t * root, char * n)
{
    int count = 0;
    for (struct definition_t * def = root; def != NULL; def = def->next)
    {
        if (def->nodeset && !def->duplicate && def->nodes)
        {
            char * node = def->nodes->node;
            if (!strcmp (node, n))
            {
                if (++count > 1) def->duplicate = 1;
            }
        }
    }
    return count;
}

/* The following function checks whether the nodes specified by the
   nodeset functionality is valid in its current scope.  It does not
   check across subcircuit boundaries. */
static int checker_validate_nodesets (struct definition_t * root)
{
    int errors = 0;
    for (struct definition_t * def = root; def != NULL; def = def->next)
    {
        if (def->nodeset && checker_count_nodes (def) == 1)
        {
            char * node = def->nodes->node;
            if (checker_count_nodes (root, node) <= 0)
            {
                logprint (LOG_ERROR, "line %d: checker error, no such node `%s' found "
                          "as referenced by `%s:%s'\n", def->line, node, def->type,
                          def->instance);
                errors++;
            }
            if (checker_count_nodesets (root, node) > 1)
            {
                logprint (LOG_ERROR, "line %d: checker error, the node `%s' is not "
                          "uniquely defined by `%s:%s'\n", def->line, node, def->type,
                          def->instance);
                errors++;
            }
        }
    }
    return errors;
}

/* This function should be called after the netlist and the equation
   list have been checked.  It verifies that parameter sweep
   definitions and equation variable identifiers are unique.  The
   function returns zero on success and non-zero otherwise. */
static int netlist_checker_variables_intern (struct definition_t * root,
        environment * env)
{
    int errors = 0, pos;
    struct value_t * para, * ref;
    strlist * eqnvars = env->getChecker()->variables ();
    strlist * instances = new strlist ();
    strlist * vars = new strlist ();
    strlist * refs = new strlist ();
    // go through list of netlist definitions
    for (struct definition_t * def = root; def != NULL; def = def->next)
    {
        // find parameters sweeps
        if (def->action == 1 && !strcmp (def->type, "SW"))
        {
            para = checker_find_reference (def, "Param");
            ref = checker_find_reference (def, "Sim");
            if (para != NULL && ref != NULL)
            {
                // check whether sweep variable collides with equations
                if (eqnvars && eqnvars->contains (para->ident))
                {
                    logprint (LOG_ERROR, "checker error, equation variable `%s' "
                              "already defined by `%s:%s'\n", para->ident,
                              def->type, def->instance);
                    errors++;
                }
                // check for duplicate parameter names in parameter sweeps, but
                // allow them in same order sweeps
                if ((pos = vars->index (para->ident)) != -1)
                {
                    if (strcmp (ref->ident, refs->get (pos)))
                    {
                        logprint (LOG_ERROR, "checker error, variable `%s' in `%s:%s' "
                                  "already defined by `%s:%s'\n", para->ident, def->type,
                                  def->instance, def->type, instances->get (pos));
                        errors++;
                    }
                }
                // check for duplicate simulations in parameter sweeps (same order
                // sweep) and allow same parameter name only
                if ((pos = refs->index (ref->ident)) != -1)
                {
                    if (strcmp (para->ident, vars->get (pos)))
                    {
                        logprint (LOG_ERROR, "checker error, conflicting variables `%s' "
                                  "in `%s:%s' and `%s' in `%s:%s' for `%s'\n",
                                  para->ident, def->type, def->instance,
                                  vars->get (pos), def->type, instances->get (pos),
                                  ref->ident);
                        errors++;
                    }
                }
                // collect parameter sweep variables for the above two checks
                instances->add (def->instance);
                vars->add (para->ident);
                refs->add (ref->ident);
            }
        }
    }
    delete eqnvars;
    delete refs;
    delete vars;
    delete instances;
    return errors;
}

/* This is the overall variable checker for the parsed netlist.  See
   the above function for details. */
int netlist_checker_variables (environment * env)
{
    return netlist_checker_variables_intern (definition_root, env);
}

/* The function checks whether the given key-value combination is
   inside the allowed range defined by the given property definition
   and returns the number of error or zero otherwise. */
static int checker_value_in_prop_range (char * instance, struct define_t * def,
                                        struct pair_t * pp,
                                        struct property_t * prop)
{
    int errors = 0;
    // check values
    if (PROP_IS_VAL (*prop))
    {
        if (!PROP_IS_LST (*prop))
        {
            // lists of values possible?
            if (pp->value->next != NULL)
            {
                logprint (LOG_ERROR,
                          "checker error, value of `%s' needs to be "
                          "a single value in `%s:%s', no lists possible\n",
                          pp->key, def->type, instance);
                errors++;
            }
        }
        else
        {
            // a value list
            struct value_t * val = pp->value;
            val->var = TAG_VECTOR;
            // check and evaluate the unit scale in a value list
            for (; val != NULL; val = val->next)
            {
                if (!checker_evaluate_scale (val))
                    errors++;
            }
        }
        // check range of all values
        if (PROP_HAS_RANGE (*prop))
        {
            struct value_t * val = pp->value;
            if (val->ident)
            {
                /* no range checking on variable identifier */
                logprint (LOG_STATUS,
                          "checker notice, value of `%s' (variable `%s') could be "
                          "out of range `%c%g,%g%c' in `%s:%s'\n",
                          pp->key, val->ident, prop->range.il, prop->range.l,
                          prop->range.h, prop->range.ih, def->type, instance);
                val = NULL;
            }
            for (; val != NULL; val = val->next)
            {
                int rerror = 0;
                if (prop->range.il == '[' &&  (val->value < prop->range.l))
                    rerror++;
                if (prop->range.il == ']' && !(val->value > prop->range.l))
                    rerror++;
                if (prop->range.ih == '[' && !(val->value < prop->range.h))
                    rerror++;
                if (prop->range.ih == ']' &&  (val->value > prop->range.h))
                    rerror++;
                if (rerror)
                {
                    logprint (LOG_ERROR,
                              "checker error, value of `%s' (%g) is out of "
                              "range `%c%g,%g%c' in `%s:%s'\n",
                              pp->key, val->value, prop->range.il, prop->range.l,
                              prop->range.h, prop->range.ih, def->type, instance);
                    errors++;
                }
            }
        }
        // check fraction of integers
        if (PROP_IS_INT (*prop))
        {
            double integral;
            if (modf (pp->value->value, &integral) != 0)
            {
                logprint (LOG_ERROR,
                          "checker error, value of `%s' (%g) needs to be "
                          "an integer in `%s:%s'\n",
                          pp->key, pp->value->value, def->type, instance);
                errors++;
            }
        }
    }
    // check identifiers
    else
    {
        // no identifier given
        if (pp->value->ident == NULL)
        {
            logprint (LOG_ERROR,
                      "checker error, value of `%s' (%g) needs to be "
                      "an identifier in `%s:%s'\n",
                      pp->key, pp->value->value, def->type, instance);
            errors++;
        }
        // check identifier range
        else
        {
            if (PROP_HAS_STR (*prop))
            {
                int i, found = 0;
                char range[256];
                sprintf (range, "[");
                for (i = 0; prop->range.str[i]; i++)
                {
                    strcat (range, prop->range.str[i]);
                    strcat (range, ",");
                    if (!strcmp (prop->range.str[i], pp->value->ident)) found++;
                }
                if (!found)
                {
                    range[strlen (range) - 1] = ']';
                    logprint (LOG_ERROR,
                              "checker error, value of `%s' (%s) needs to be "
                              "in %s in `%s:%s'\n",
                              pp->key, pp->value->ident, range, def->type, instance);
                    errors++;
                }
                else pp->value->range = 1;
            }
        }
    }
    return errors;
}

/* The function checks whether the given key-value combination being
   part of the available definition is inside the allowed range and
   returns zero if not.  Otherwise the function returns non-zero. */
static int checker_value_in_range (char * instance, struct define_t * def,
                                   struct pair_t * pp)
{
    int i, errors = 0;
    // go through required properties
    for (i = 0; PROP_IS_PROP (def->required[i]); i++)
    {
        if (!strcmp (def->required[i].key, pp->key))
        {
            errors += checker_value_in_prop_range (instance, def, pp,
                                                   &def->required[i]);
        }
    }
    // go through optional properties
    for (i = 0; PROP_IS_PROP (def->optional[i]); i++)
    {
        if (!strcmp (def->optional[i].key, pp->key))
        {
            errors += checker_value_in_prop_range (instance, def, pp,
                                                   &def->optional[i]);
        }
    }
    return errors ? 0 : 1;
}

/* The function determines the subcircuit definitions defined in the
   original netlist and builds an appropriate subcircuit definition
   list.  It returns the given definition list with the subcircuits
   removed. */
static struct definition_t *
checker_build_subcircuits (struct definition_t * root)
{
    struct definition_t * def, * next, * prev;
    for (prev = NULL, def = root; def != NULL; def = next)
    {
        next = def->next;
        if (!strcmp (def->type, "Def"))
        {
            if (prev)
            {
                prev->next = next;
            }
            else
            {
                root = next;
            }
            def->sub = checker_build_subcircuits (def->sub);
            def->next = subcircuit_root;
            subcircuit_root = def;
        }
        else prev = def;
    }
    return root;
}

/* The function produces a copy of the given circuit definition and
   marks it as a copy.  The node definition are not included within
   the copy. */
static struct definition_t *
checker_copy_subcircuit (struct definition_t * sub)
{
    struct definition_t * copy;
    copy = (struct definition_t *) calloc (sizeof (struct definition_t), 1);
    copy->action = sub->action;
    copy->nonlinear = sub->nonlinear;
    copy->substrate = sub->substrate;
    copy->nodeset = sub->nodeset;
    copy->define = sub->define;
    copy->pairs = sub->pairs;
    copy->ncount = sub->ncount;
    copy->type = strdup (sub->type);
    copy->copy = 1;
    return copy;
}

/* This function translates the node definitions of the given
   subcircuit element 'sub'.  The translation is based upon the node
   definitions of the subcircuit 'type' and the instance 'inst' of
   such a subcircuit.  The translated nodes are saved in an extra
   'xlate' field of each node of the subcircuit element 'sub'. */
static void checker_xlat_subcircuit_nodes (struct definition_t * type,
        struct definition_t * inst,
        struct definition_t * sub)
{
    struct node_t * n, * ninst, * ntype;
    int i;
    // go through nodes of the subcircuit 'type' and 'inst'
    for (i = 1, ntype = type->nodes, ninst = inst->nodes; ntype != NULL;
            ntype = ntype->next, ninst = ninst->next, i++)
    {
        for (n = sub->nodes; n != NULL; n = n->next)
        {
            /* check whether a node in the subcircuit element 'sub' corresponds
            with the 'type', then assign the 'inst's node name */
            if (!strcmp (n->node, ntype->node))
            {
                n->xlate = strdup (ninst->node);
                n->xlatenr = i;
            }
        }
    }
}

/* The function creates a subcircuit node name consisting of the given
   arguments.  If the given 'instances' is NULL it is left out.  The
   caller is responsible to free() the returned string. */
static char * checker_subcircuit_node (char * type, char * instances,
                                       char * instance, char * node)
{
    char * txt = (char *)
                 calloc (1, strlen (type) + strlen (instance) + strlen (node) +
                         (instances ? strlen (instances) : 0) + 4);
    if (instances)
        sprintf (txt, "%s.%s.%s.%s", type, instances, instance, node);
    else
        sprintf (txt, "%s.%s.%s", type, instance, node);
    return txt;
}

/* The function reverses the order of the given node list and returns
   the reversed list. */
struct node_t * netlist_reverse_nodes (struct node_t * nodes)
{
    struct node_t * root, * next;
    for (root = NULL; nodes != NULL; nodes = next)
    {
        next = nodes->next;
        nodes->next = root;
        root = nodes;
    }
    return root;
}

/* This function assigns new node names to the subcircuit element
   'copy' based upon the previous node translation between the
   subcircuit 'type' and the instance 'inst' of this type.  The global
   'gnd' node is not touched. */
static void
checker_copy_subcircuit_nodes (struct definition_t * type,
                               struct definition_t * inst,
                               struct definition_t * sub,
                               struct definition_t * copy,
                               char * instances)
{
    struct node_t * n, * ncopy, * root = NULL;

    // go through the list of the subcircuit element's 'sub' nodes
    for (n = sub->nodes; n != NULL; n = n->next)
    {

        // create new node based upon the node translation
        ncopy = (struct node_t *) calloc (sizeof (struct node_t), 1);
        ncopy->xlatenr = n->xlatenr;
        if (n->xlate)   // translated node
        {
            if (instances == NULL)
                ncopy->node = strdup (n->xlate);
            else
                ncopy->node = NULL; // leave it blank yet, indicates translation
        }
        else if (!strcmp (n->node, "gnd"))   // ground node
        {
            ncopy->node = strdup (n->node);
        }
        else if (n->node[strlen (n->node) - 1] == '!')   // global node
        {
            ncopy->node = strdup (n->node);
        }
        else   // internal subcircuit element node
        {
            ncopy->node = checker_subcircuit_node (type->instance, instances,
                                                   inst->instance, n->node);
        }
        // chain the new node list
        ncopy->next = root;
        root = ncopy;
    }

    /* and finally reverse the created node list and assign it to the
       subcircuit element's 'copy' */
    copy->nodes = netlist_reverse_nodes (root);
}

// Returns the node at the given position.
static struct node_t * checker_get_circuit_node (struct node_t * root, int n)
{
    for (int i = 1; i < n; i++)
    {
        root = root->next;
        assert (root != NULL);
    }
    return root;
}

// The function cleans the translated nodes of a subcircuit template.
static void checker_cleanup_xlat_nodes (struct definition_t * sub)
{
    for (struct node_t * n = sub->nodes; n != NULL; n = n->next)
    {
        free (n->xlate);
        n->xlate = NULL;
        n->xlatenr = 0;
    }
}

/* The function is used to assign the nodes of the 'copy' subcircuit
   element which were left blank intentionally by the element copy in
   order to indicate that it is an external node.  Again, if the
   current node translation indicates an external node it is blanked
   again, otherwise the node gets a unique internal name.  If the
   'instances' list is NULL, then this indicates the root circuit list
   and node translations are done though they are 'external'. */
static void
checker_copy_circuit_nodes (struct definition_t * type,
                            struct definition_t * inst,
                            struct definition_t * sub,
                            struct definition_t * copy,
                            char * instances)
{
    struct node_t * n, * ncopy;

    // go through the list of the subcircuit element's 'copy' nodes
    for (ncopy = copy->nodes; ncopy != NULL; ncopy = ncopy->next)
    {
        // these NULL nodes have intentionally been blanked
        if (ncopy->node == NULL)
        {
            assert (ncopy->xlatenr != 0);
            // get translated node
            n = checker_get_circuit_node (sub->nodes, ncopy->xlatenr);
            ncopy->xlatenr = n->xlatenr;
            if (n->xlate)   // translated node
            {
                if (instances == NULL)
                    // external node indicated by no instances given
                    ncopy->node = strdup (n->xlate);
                else
                    ncopy->node = NULL; // keep blank
            }
            else if (!strcmp (n->node, "gnd"))   // global ground node
            {
                ncopy->node = strdup (n->node);
            }
            else if (n->node[strlen (n->node) - 1] == '!')   // other global node
            {
                ncopy->node = strdup (n->node);
            }
            else   // internal subcircuit element node
            {
                ncopy->node = checker_subcircuit_node (type->instance, instances,
                                                       inst->instance, n->node);
            }
        }
    }
}

/* This function returns the last entry of the given list of
   definitions or NULL if there is no such element. */
static struct definition_t *
checker_find_last_definition (struct definition_t * root)
{
    for (struct definition_t * def = root; def != NULL; def = def->next)
        if (def->next == NULL) return def;
    return NULL;
}

/* Based upon the the given subcircuit instance identifier list the
   function returns a "." - concatenated string or NULL. */
static char * checker_subcircuit_instance_list (strlist * instances)
{
    if (instances && instances->length () > 0)
        return instances->toString (".");
    return NULL;
}

/* The function creates a subcircuit instance name consisting of the
   given arguments.  If the given 'instances' is NULL it is left out.
   The caller is responsible to free() the returned string. */
static char * checker_subcircuit_instance (char * type, char * instances,
        char * instance, char * base)
{
    char * txt = (char *)
                 calloc (1, strlen (type) + strlen (instance) + strlen (base) +
                         (instances ? strlen (instances) : 0) + 4);
    if (instances)
        sprintf (txt, "%s.%s.%s.%s", type, instances, instance, base);
    else
        sprintf (txt, "%s.%s.%s", type, instance, base);
    return txt;
}

/* This function produces a copy of the given subcircuit 'type'
   containing the subcircuit elements.  Based upon the instance 'inst'
   definitions (node names and instance name) it assign new element
   instances and node names.  The function returns a NULL terminated
   circuit element list in reverse order. */
static struct definition_t *
checker_copy_subcircuits (struct definition_t * type,
                          struct definition_t * inst, strlist * * instances,
                          environment * parent)
{
    struct definition_t * def, * copy;
    struct definition_t * root = NULL;
    strlist * instcopy;
    char * list;

    // create environment for subcircuit instance
    environment * child = new environment (*(type->env));
    parent->push_front_Child (child);

    // put instance properties into subcircuit environment
    for (struct pair_t * pair = inst->pairs; pair != NULL; pair = pair->next)
    {
        // anything else than the 'Type'
        if (strcmp (pair->key, "Type"))
        {
            if (pair->value->ident == NULL)
            {
                // simple value
                child->setDoubleConstant (pair->key, pair->value->value);
                child->setDouble (pair->key, pair->value->value);
            }
            else
            {
                // reference to variable in upper environment
                child->setDoubleReference (pair->key, pair->value->ident);
            }
        }
    }

    // go through element list of subcircuit
    for (def = type->sub; def != NULL; def = def->next)
    {

        // translate the node list
        checker_xlat_subcircuit_nodes (type, inst, def);

        // allow recursive subcircuits
        if (!strcmp (def->type, "Sub"))
        {
            // get subcircuit template definition
            struct definition_t * sub = checker_get_subcircuit (def);
            // create a copy of the current subcircuit instance list
            if ((*instances) == NULL) (*instances) = new strlist ();
            instcopy = new strlist (*(*instances));
            // append instance name to recursive instance list
            (*instances)->append (inst->instance);
            copy = checker_copy_subcircuits (sub, def, instances, child);
            // put the expanded definitions into the sublist
            if (copy)
            {
                list = checker_subcircuit_instance_list (instcopy);
                // assign blanked node names to each subcircuit
                for (struct definition_t * c = copy; c != NULL; c = c->next)
                {
                    checker_copy_circuit_nodes (type, inst, def, c, list);
                }
                // append the copies to the subcircuit list
                struct definition_t * last = checker_find_last_definition (copy);
                last->next = root;
                root = copy;
            }
            // restore original instance list
            delete *instances;
            *instances = instcopy;
        }
        else
        {
            // element copy
            copy = checker_copy_subcircuit (def);
            // assign new instance name to the element
            list = checker_subcircuit_instance_list (*instances);
            copy->instance =
                checker_subcircuit_instance (type->instance, list,
                                             inst->instance, def->instance);
            copy->subcircuit = strdup (type->instance);
            // assign node list
            checker_copy_subcircuit_nodes (type, inst, def, copy, list);
            // apply environment
            copy->env = child;
            // chain definition (circuit) list
            copy->next = root;
            root = copy;
        }

        // cleanup translated nodes
        checker_cleanup_xlat_nodes (def);
    }

    // try giving child environment a unique name
    strlist * icopy = new strlist ();
    icopy->append (type->instance);
    icopy->append (*(instances));
    icopy->append (inst->instance);
    child->setName (std::string(icopy->toString (".")));
    delete icopy;

    return root;
}

/* The function checks whether the subcircuit 'instance' with the
   subcircuit 'type' is defined in cycles.  It recursively goes
   through the definitions and emits an appropriate error message if
   necessary.  The function returns zero if there are no cycles
   detected and non-zero with cycles found. */
static int checker_validate_sub_cycles (struct definition_t * root,
                                        char * type, char * instance,
                                        strlist * * deps)
{
    int errors = 0, error;
    struct value_t * val;

    /* emit an appropriate error message if the subcircuit type is already
       in the dependencies */
    if ((*deps)->contains (type))
    {
        logprint (LOG_ERROR, "checker error, cyclic definition of `%s:%s' "
                  "detected, involves: %s\n",
                  type, instance, (*deps)->toString ());
        return ++errors;
    }
    (*deps)->append (type);

    // create temporary list of subcircuit types already tested
    strlist * checked = new strlist ();
    // go through the list of circuit elements of the subcircuit
    for (struct definition_t * def = root->sub; def != NULL; def = def->next)
    {
        // find the appropriate definitions
        if (!strcmp (def->type, "Sub"))
        {
            // recurse into subcircuit instances
            if ((val = checker_find_reference (def, "Type")) != NULL)
            {
                if (!checked->contains (val->ident))   // only if not already checked
                {
                    struct definition_t * sub;
                    checked->append (val->ident);
                    // copy current dependencies
                    strlist * copy = new strlist (*(*deps));
                    // validate subcircuit
                    sub = checker_find_subcircuit (val->ident);
                    if (sub != NULL) // if possible
                        error = checker_validate_sub_cycles (sub, sub->instance,
                                                             instance, deps);
                    else
                        error = 1;
                    // on errors: go on
                    if (error)
                    {
                        errors += error;
                        delete copy;
                    }
                    // no errors: restore original dependencies
                    else
                    {
                        delete *deps;
                        *deps = copy;
                    }
                }
            }
        }
    }
    delete checked;
    return errors;
}

/* This function dynamically creates a circuit definition based on a
   given subcircuit definition including type, number of nodes and the
   properties. */
static struct define_t * netlist_create_define (struct definition_t * def)
{
    int o, r;
    struct pair_t * p;
    struct define_t * d =
        (struct define_t *) calloc (sizeof (struct define_t), 1);
    d->type = strdup (def->instance);
    d->nodes = checker_count_nodes (def);
    d->action = PROP_COMPONENT;

    // determine number of required and optional parameters
    for (o = r = 0, p = def->pairs; p != NULL; p = p->next)
    {
        if (p->value == NULL)
            r++;
        else
            o++;
    }
    d->required =
        (struct property_t *) calloc (sizeof (struct property_t), r + 2);
    d->optional =
        (struct property_t *) calloc (sizeof (struct property_t), o + 1);

    // fill in parameters
    for (o = r = 0, p = def->pairs; p != NULL; p = p->next)
    {
        if (p->value == NULL)
        {
            // required
            d->required[r].key = strdup (p->key);
            d->required[r].type = PROP_REAL;
            d->required[r].defaultval.d = PROP_NO_VAL;
            d->required[r].defaultval.s = PROP_NO_STR;
            d->required[r].range.il = '.';
            d->required[r].range.l = 0;
            d->required[r].range.h = 0;
            d->required[r].range.ih = '.';
            r++;
        }
        else
        {
            // optional
            d->optional[o].key = strdup (p->key);
            d->optional[o].type = PROP_REAL;
            d->optional[o].defaultval.d = p->value->value;
            d->optional[o].defaultval.s = PROP_NO_STR;
            d->optional[o].range.il = '.';
            d->optional[o].range.l = 0;
            d->optional[o].range.h = 0;
            d->optional[o].range.ih = '.';
            o++;
        }
    }

    // extra required
    d->required[r].key = strdup ("Type");
    d->required[r].type = PROP_STR;
    d->required[r].defaultval.d = PROP_NO_VAL;
    d->required[r].defaultval.s = PROP_NO_STR;
    d->required[r].range.il = '.';
    d->required[r].range.l = 0;
    d->required[r].range.h = 0;
    d->required[r].range.ih = '.';
    return d;
}

/* The function destroys the given circuit definition which must have
   been dynamically created before. */
static void netlist_free_define (struct define_t * d)
{
    int i;
    struct property_t * p;
    free ((char *) d->type);
    // free required properties
    for (i = 0, p = d->required; p[i].key != NULL; i++)
    {
        free ((char *) p[i].key);
    }
    free (d->required);
    // free optional properties
    for (i = 0, p = d->optional; p[i].key != NULL; i++)
    {
        free ((char *) p[i].key);
    }
    free (d->optional);
    free (d);
}

/* The function checks the presence of required and optional
   properties as well as their content in the given definition.  It
   returns zero on success and non-zero otherwise. */
static int checker_validate_properties (struct definition_t * root,
                                        struct definition_t * def,
                                        struct define_t * available)
{
    struct pair_t * pair;
    int i, n, errors = 0;

    /* check whether the required properties are given */
    for (i = 0; PROP_IS_PROP (available->required[i]); i++)
    {
        n = checker_find_property (available->required[i].key, def->pairs);
        if (n != 1)
        {
            logprint (LOG_ERROR, "line %d: checker error, required property "
                      "`%s' occurred %dx in `%s:%s'\n", def->line,
                      available->required[i].key, n, def->type, def->instance);
            errors++;
        }
    }
    /* check whether the optional properties are given zero/once */
    for (i = 0; PROP_IS_PROP (available->optional[i]); i++)
    {
        n = checker_find_property (available->optional[i].key, def->pairs);
        if (n >= 2)
        {
            logprint (LOG_ERROR, "line %d: checker error, optional property "
                      "`%s' occurred %dx in `%s:%s'\n", def->line,
                      available->optional[i].key, n, def->type, def->instance);
            errors++;
        }
    }

    /* check the property content */
    for (pair = def->pairs; pair != NULL; pair = pair->next)
    {
        /* check whether properties are either required or optional */
        int type = checker_is_property (available, pair->key);
        if (type == PROP_NONE)
        {
            if (strcmp (def->type, "Def"))
            {
                logprint (LOG_ERROR,
                          "line %d: checker error, extraneous property `%s' is "
                          "invalid in `%s:%s'\n", def->line,
                          pair->key, def->type, def->instance);
                errors++;
            }
        }
        // do not check zero-length lists
        if (pair->value != NULL)
        {
            /* check and evaluate the unit scale in a property */
            if (!checker_evaluate_scale (pair->value))
                errors++;
            /* check whether properties are in range */
            if (!checker_value_in_range (def->instance, available, pair))
            {
                errors++;
            }
            /* check variables in properties */
            if (!checker_resolve_variable (root, def, pair, type))
                errors++;
        }
    }
    return errors;
}

/* This function is used by the netlist checker to validate the
   subcircuits.  It returns zero with no errors and non-zero on
   errors. */
static int checker_validate_subcircuits (struct definition_t * root)
{
    int errors = 0;
    // go through list of definitions
    for (struct definition_t * def = root; def != NULL; def = def->next)
    {
        // check subcircuit instances
        if (!strcmp (def->type, "Sub"))
        {
            struct value_t * val;
            // validate the 'Type' reference
            if ((val = checker_validate_reference (def, "Type")) == NULL)
            {
                errors++;
            }
            else
            {
                // find an appropriate subcircuit type
                struct definition_t * sub = checker_find_subcircuit (val->ident);
                if (sub == NULL)
                {
                    logprint (LOG_ERROR, "line %d: checker error, no such subcircuit "
                              "`%s' found as referred in `%s:%s'\n", def->line,
                              val->ident, def->type, def->instance);
                    errors++;
                }
                else
                {
                    // check the number of nodes of the instance and the type
                    int n1 = checker_count_nodes (def);
                    int n2 = checker_count_nodes (sub);
                    if (n1 != n2)
                    {
                        logprint (LOG_ERROR, "line %d: checker error, subcircuit type "
                                  "`%s' requires %d nodes in `%s:%s', found %d\n",
                                  def->line, sub->instance, n2,
                                  def->type, def->instance, n1);
                        errors++;
                    }
                    // check the subcircuit instance properties
                    struct define_t * available = netlist_create_define (sub);
                    errors += checker_validate_properties (root, def, available);
                    netlist_free_define (available);
                    // and finally check for cyclic definitions
                    strlist * deps = new strlist ();
                    int err = checker_validate_sub_cycles (sub, sub->instance,
                                                           def->instance, &deps);
                    errors += err;
                    checker_sub_cycles = err;
                    delete deps;
                }
            }
        }
    }
    return errors;
}

/* Deletes node list of a definition. */
static void netlist_free_nodes (struct node_t * node)
{
    struct node_t * n;
    for (; node != NULL; node = n)
    {
        n = node->next;
        free (node->node);
        free (node);
    }
}

/* The following function free()'s the given value. */
static void netlist_free_value (struct value_t * value)
{
    free (value->ident);
    if (value->unit)  free (value->unit);
    free (value->scale);
    free (value);
}

/* Deletes pair list of a definition. */
static void netlist_free_pairs (struct pair_t * pp)
{
    struct pair_t * np;
    for (; pp != NULL; pp = np)
    {
        np = pp->next;
        struct value_t * nv, * value;
        for (value = pp->value; value != NULL; value = nv)
        {
            nv = value->next;
            netlist_free_value (value);
        }
        free (pp->key);
        free (pp);
    }
}

/* Deletes the given definition. */
static void netlist_free_definition (struct definition_t * def)
{
    netlist_free_nodes (def->nodes);
    if (!def->copy) netlist_free_pairs (def->pairs);
    free (def->subcircuit);
    free (def->type);
    free (def->instance);
    free (def);
}

/* The function removes the given definition 'cand' from the
   definition root.  It returns the new definition root. */
struct definition_t *
netlist_unchain_definition (struct definition_t * root,
                            struct definition_t * cand)
{
    struct definition_t * prev;
    if (cand == root)
    {
        root = cand->next;
        netlist_free_definition (cand);
    }
    else
    {
        // find previous to the candidate to be deleted
        for (prev = root; prev != NULL && prev->next != cand; prev = prev->next) ;
        if (prev != NULL)
        {
            prev->next = cand->next;
            netlist_free_definition (cand);
        }
    }
    return root;
}

/* The function expands the subcircuits within the given definition
   list and returns the expanded list with the subcircuit definitions
   removed. */
static struct definition_t *
checker_expand_subcircuits (struct definition_t * root, environment * parent)
{
    struct definition_t * def, * sub, * copy, * next, * prev;
    strlist * instances = NULL;

    // go through the list of definitions
    for (prev = NULL, def = root; def != NULL; def = next)
    {
        next = def->next;
        // is this a subcircuit instance definition ?
        if (!strcmp (def->type, "Sub"))
        {
            // get the subcircuit type definition
            sub = checker_get_subcircuit (def);
            // and make a copy of it
            copy = checker_copy_subcircuits (sub, def, &instances, parent);
            if (instances)
            {
                delete instances;
                instances = NULL;
            }
            // remove the subcircuit instance from the original list
            if (prev)
            {
                prev->next = next;
            }
            else
            {
                root = next;
            }
            netlist_free_definition (def);
            // put the expanded definitions into the netlist
            if (copy)
            {
                struct definition_t * last = checker_find_last_definition (copy);
                last->next = root;
                if (!prev) prev = last;
                root = copy;
            }
        }
        // component in the root environment
        else
        {
            prev = def;
            def->env = parent;
        }
    }
    return root;
}

/* This function is the checker routine for a parsed netlist.  It
   returns zero on success or non-zero if the parsed netlist contained
   errors. */
static int netlist_checker_intern (struct definition_t * root)
{
    struct definition_t * def;
    struct define_t * available;
    int n, errors = 0;

    /* go through all definitions */
    for (def = root; def != NULL; def = def->next)
    {

        /* check whether the definition type is known */
        available = checker_find_definition (def->type, def->action);
        if (available == NULL)
        {
            logprint (LOG_ERROR, "line %d: checker error, invalid definition type "
                      "`%s'\n", def->line, def->type);
            errors++;
        }
        else
        {
            /* mark nodeset definitions */
            def->nodeset = !strcmp (def->type, "NodeSet") ? 1 : 0;
            /* mark nonlinear circuit definitions */
            def->nonlinear = available->nonlinear;
            /* mark substrate definitions */
            def->substrate = available->substrate;
            /* save available definition */
            def->define = available;
            /* check whether the number of nodes is correct and save the
               number of given nodes */
            n = def->ncount = checker_count_nodes (def);
            if (available->nodes == PROP_NODES)
            {
                if (n < 1)
                {
                    logprint (LOG_ERROR,
                              "line %d: checker error, at least 1 node required in "
                              "`%s:%s', found %d\n", def->line, def->type,
                              def->instance, n);
                    errors++;
                }
            }
            else if (available->nodes != n)
            {
                logprint (LOG_ERROR,
                          "line %d: checker error, %d node(s) required in `%s:%s', "
                          "found %d\n", def->line,
                          available->nodes, def->type, def->instance, n);
                errors++;
            }
            /* check the properties except for subcircuits */
            if (strcmp (def->type, "Sub"))
            {
                errors += checker_validate_properties (root, def, available);
            }
        }
        /* check the number of definitions */
        n = checker_count_definition (root, def->type, def->instance);
        if (n != 1 && def->duplicate == 0)
        {
            logprint (LOG_ERROR, "checker error, found %d definitions of `%s:%s'\n",
                      n, def->type, def->instance);
            errors++;
        }
    }
    /* check microstrip definitions */
    errors += checker_validate_strips (root);
    /* check subcircuit definitions */
    errors += checker_validate_subcircuits (root);
    /* check nodeset definitions */
    errors += checker_validate_nodesets (root);
    return errors;
}

#if DEBUG
/* Debug function: Prints value representation. */
static void netlist_list_value (struct value_t * value)
{
    if (value == NULL)
        logprint (LOG_STATUS, "[]");
    else if (value->ident)
        logprint (LOG_STATUS, "%s", value->ident);
    else if (value->next)
    {
        logprint (LOG_STATUS, "[");
        for (; value != NULL; value = value->next)
            logprint (LOG_STATUS, "%g%s", value->value, value->next ? ";" : "");
        logprint (LOG_STATUS, "]");
    }
    else
    {
        logprint (LOG_STATUS, "%g", value->value);
        if (value->scale)
            logprint (LOG_STATUS, "%s", value->scale);
        if (value->unit)
            logprint (LOG_STATUS, "%s", value->unit);
    }
}

/* Debug function: Prints definition list representation. */
static void netlist_lister (struct definition_t * root, const char * prefix)
{
    struct definition_t * def;
    struct node_t * node;
    struct pair_t * pair;
    for (def = root; def != NULL; def = def->next)
    {
        logprint (LOG_STATUS, "%s%s:%s", prefix, def->type, def->instance);
        for (node = def->nodes; node != NULL; node = node->next)
        {
            logprint (LOG_STATUS, " %s", node->node);
        }
        for (pair = def->pairs; pair != NULL; pair = pair->next)
        {
            logprint (LOG_STATUS, " %s=\"", pair->key);
            netlist_list_value (pair->value);
            logprint (LOG_STATUS, "\"");
        }
        logprint (LOG_STATUS, "\n");
    }
}

/* Debug function: Prints the overall netlist representation. */
void netlist_list (void)
{
    struct definition_t * def;
    logprint (LOG_STATUS, "subcircuit %s\n", "root");
    netlist_lister (definition_root, "  ");
    for (def = subcircuit_root; def != NULL; def = def->next)
    {
        logprint (LOG_STATUS, "subcircuit %s\n", def->instance);
        netlist_lister (def->sub, "  ");
    }
}
#endif /* DEBUG */

/* The function logs the content of the current netlist by telling how
   many instances of which kind of components are used in the netlist. */
void netlist_status (void)
{
    struct define_t * def;
    struct definition_t * cir;
    int count;
    logprint (LOG_STATUS, "netlist content\n");
    hashiterator<module> it;
    for (it = hashiterator<module> (module::modules); *it; ++it)
    {
        def = it.currentVal()->definition;
        for (count = 0, cir = definition_root; cir != NULL; cir = cir->next)
        {
            if (!strcmp (def->type, cir->type)) count++;
        }
        if (count > 0)
        {
            logprint (LOG_STATUS, "  %5d %s instances\n", count, def->type);
        }
    }
}

/* The function builds the equation list for a given list of
   definition and removes the definition containing the equations from
   the list. */
static struct definition_t *
checker_build_equations (struct definition_t * root, eqn::node ** eroot)
{
    struct definition_t * def, * next, * prev;
    eqn::node * eqns, * last;
    *eroot = NULL;
    // go through list of definitions
    for (prev = NULL, def = root; def != NULL; def = next)
    {
        next = def->next;
        if (!strcmp (def->type, "Eqn"))
        {
            // rechain definition list
            if (prev)
            {
                prev->next = next;
            }
            else
            {
                root = next;
            }
            // append equations
            eqns = (eqn::node *) def->eqns;
            last = eqn::checker::lastEquation (eqns);
            last->setNext (*eroot);
            *eroot = eqns;
            // free this definition
            netlist_free_definition (def);
        }
        else prev = def;
    }
    return root;
}

/* The function creates an environment for the given definition root
   including equation checker and solver. */
static void checker_setup_env (struct definition_t * root,
                               environment * env, eqn::node * eqns)
{
    // create equation checker
    eqn::checker * checkee = new eqn::checker ();
    // pass equations to the checker
    checkee->setEquations (eqns);
    // add constants to the list of equations
    checkee->constants ();
    // pass checker
    env->setChecker (checkee);
    // create equation solver
    eqn::solver * solvee = new eqn::solver (checkee);
    // pass solver
    env->setSolver (solvee);
    // apply environment to the netlist root
    if (root) root->env = env;
}

/* Adds the arguments of a subcircuit into the equation checker of the
   given environment. */
static void checker_subcircuit_args (struct definition_t * def,
                                     environment * env)
{
    for (struct pair_t * pair = def->pairs; pair != NULL; pair = pair->next)
    {
        // anything else than the 'Type'
        if (strcmp (pair->key, "Type"))
        {
            // put it into the equation checker
            env->getChecker()->addDouble ("#subcircuit",
                                          pair->key, pair->value->value);
            // also put it into the environment
            variable * v = checker_add_variable (env, pair->key, TAG_DOUBLE, true);
            v->getConstant()->d = pair->value->value;
        }
    }
}

/* This is the global netlist checker.  It returns zero on success and
   non-zero on errors. */
int netlist_checker (environment * env)
{
    int errors = 0;
    eqn::node * eqns;
    struct definition_t * def;

    // create top-level environment
    env_root = new environment (env->getName ());
    // create the subcircuit list
    definition_root = checker_build_subcircuits (definition_root);
    // get equation list
    definition_root = checker_build_equations (definition_root, &eqns);
    // setup the root environment
    checker_setup_env (definition_root, env_root, eqns);
    // check list of subcircuits
    errors += netlist_checker_intern (subcircuit_root);
    // check global netlist
    errors += netlist_checker_intern (definition_root);
    // check equations in root
    env_root->setDefinitions (definition_root);
    errors += env_root->equationChecker (0);
    env_root->setDefinitions (NULL);

    // then check each subcircuit list
    for (def = subcircuit_root; def != NULL; def = def->next)
    {
        // get equation list
        def->sub = checker_build_equations (def->sub, &eqns);
        // setup the subcircuit environment
        environment * subenv = new environment (def->instance);
        env_root->push_front_Child (subenv);
        checker_setup_env (def, subenv, eqns);
        if (def->sub) def->sub->env = subenv;
        // add subcircuit parameters to equations
        checker_subcircuit_args (def, subenv);
        // check subcircuit netlist
        errors += netlist_checker_intern (def->sub);
        // check equations in subcircuit
        subenv->setDefinitions (def->sub);
        errors += subenv->equationChecker (0);
        subenv->setDefinitions (NULL);
    }

    // check actions
    errors += checker_validate_actions (definition_root);

    if (!errors)
    {
        // create actual root environment
        env->copy (*env_root);
        // and finally expand the subcircuits into the global netlist
        definition_root = checker_expand_subcircuits (definition_root, env);
    }

    return errors ? -1 : 0;
}

/* The function deletes the given definition list. */
static void netlist_destroy_intern (struct definition_t * root)
{
    struct definition_t * def, * next;
    for (def = root; def != NULL; def = next)
    {
        next = def->next;
        netlist_free_definition (def);
    }
}

/* Deletes all available definition lists. */
void netlist_destroy (void)
{
    netlist_destroy_intern (definition_root);
    for (struct definition_t * def = subcircuit_root; def; def = def->next)
    {
        netlist_destroy_intern (def->sub);
    }
    netlist_destroy_intern (subcircuit_root);
    definition_root = subcircuit_root = NULL;
    netlist_lex_destroy ();
}

/* Delete root environment(s) if necessary. */
void netlist_destroy_env (void)
{
    if (env_root != NULL)
    {
        delete env_root;
        env_root = NULL;
    }
}

