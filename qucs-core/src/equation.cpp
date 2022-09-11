/*
 * equation.cpp - checker for the Qucs equations
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

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <ctype.h>

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "vector.h"
#include "matrix.h"
#include "matvec.h"
#include "dataset.h"
#include "strlist.h"
#include "netdefs.h"
#include "equation.h"
#include "evaluate.h"
#include "differentiate.h"
#include "constants.h"
#include "range.h"
#include "exception.h"
#include "exceptionstack.h"

namespace qucs
{

using namespace eqn;

#define A(a) ((assignment *) (a))
#define N(n) ((node *) (n))
#define C(c) ((constant *) (c))
#define R(r) ((reference *) (r))

// Constructor creates an untyped instance of the constant class.
constant::constant () : node (CONSTANT)
{
    type = TAG_UNKNOWN;
    dataref = false;
    d = 0.0;
    setType (type);
}

// This constructor creates an typed instance of the constant class.
constant::constant (int tag) : node (CONSTANT)
{
    type = tag;
    dataref = false;
    d = 0.0;
    setType (type);
}

/* This copy constructor creates a instance of the constant class
   based on the given constant. */
constant::constant (const constant & o) : node (o)
{
    type = o.type;
    dataref = o.dataref;
    d = 0.0;
    setType (type);
    switch (type)
    {
    case TAG_BOOLEAN:
        b = o.b;
        break;
    case TAG_DOUBLE:
        d = o.d;
        break;
    case TAG_COMPLEX:
        c = dataref ? o.c : new nr_complex_t (*o.c);
        break;
    case TAG_VECTOR:
        v = dataref ? o.v : new qucs::vector (*o.v);
        break;
    case TAG_MATRIX:
        m = dataref ? o.m : new matrix (*o.m);
        break;
    case TAG_MATVEC:
        mv = dataref ? o.mv : new matvec (*o.mv);
        break;
    case TAG_STRING:
        s = dataref ? o.s : strdup (s);
        break;
    case TAG_CHAR:
        chr = o.chr;
        break;
    case TAG_RANGE:
        r = dataref ? o.r : new range (*o.r);
        break;
    }
}

// Re-creates the given instance.
node * constant::recreate (void)
{
    return new constant (*this);
}

// Destructor deletes an instance of the constant class.
constant::~constant ()
{
    if (!dataref)
    {
        switch (type)
        {
        case TAG_COMPLEX:
            delete c;
            break;
        case TAG_VECTOR:
            delete v;
            break;
        case TAG_MATRIX:
            delete m;
            break;
        case TAG_MATVEC:
            delete mv;
            break;
        case TAG_STRING:
            free (s);
            break;
        case TAG_RANGE:
            delete r;
            break;
        }
    }
}

/* Depending on the type of constant the function prints the textual
   representation of the object. */
void constant::print (void)
{
    logprint (LOG_STATUS, "%s", toString ());
}

// Returns the string representation of a complex value.
static char * Cplx2String (nr_complex_t c)
{
    static char str[256]; // enough for a real or complex number
    if (imag (c) == 0.0)
    {
        sprintf (str, "%g", (double) real (c));
    }
    else
    {
        sprintf (str, "(%g%cj%g)", (double ) real (c),
                 imag (c) >= 0.0 ? '+' : '-', (double) fabs (imag (c)));
    }
    return str;
}

/* This function returns a string representation depending on the type
   of constant. */
char * constant::toString (void)
{
    char str[256];
    free (txt);
    switch (type)
    {
    case TAG_BOOLEAN:
        sprintf (str, "%d", b ? 1 : 0);
        txt = strdup (str);
        break;
    case TAG_DOUBLE:
        sprintf (str, "%g", (double) d);
        txt = strdup (str);
        break;
    case TAG_COMPLEX:
        txt = strdup (Cplx2String (*c));
        break;
    case TAG_VECTOR:
    {
        int pos = 1, len = 3 + v->getSize () - 1;
        txt = (char *) malloc (len);
        strcpy (txt, "[");
        for (int i = 0; i < v->getSize (); i++)
        {
            char * s = Cplx2String (v->get (i));
            txt = (char *) realloc (txt, len += strlen (s));
            strcpy (&txt[pos], s);
            pos += strlen (s);
            if (i != v->getSize () - 1) strcpy (&txt[pos++], ";");
        }
        strcpy (&txt[pos], "]");
    }
    break;
    case TAG_MATRIX:
    {
        int len = 3 + (m->getRows () - 1) * m->getCols () + (m->getCols () - 1);
        txt = (char *) malloc (len);
        strcpy (txt, "[");
        for (int r = 0; r < m->getRows (); r++)
        {
            for (int c = 0; c < m->getCols (); c++)
            {
                char * s = Cplx2String (m->get (r, c));
                txt = (char *) realloc (txt, len += strlen (s));
                strcat (txt, s);
                if (c != m->getCols () - 1) strcat (txt, ",");
            }
            if (r != m->getRows () - 1) strcat (txt, ";");
        }
        strcat (txt, "]");
    }
    break;
    case TAG_MATVEC:
        sprintf (str, "[%dx%d](%d)",
                 mv->getRows (), mv->getCols (), mv->getSize ());
        txt = strdup (str);
        break;
    case TAG_CHAR:
        sprintf (str, "'%c'", chr);
        txt = strdup (str);
        break;
    case TAG_STRING:
        sprintf (str, "'%s'", s);
        txt = strdup (str);
        break;
    case TAG_RANGE:
        txt = strdup (r->toString ());
        break;
    default:
        txt = strdup ("(no such type)");
        break;
    }
    return txt;
}

// Returns the type of constant.
int constant::evalType (void)
{
    return getType ();
}

// Returns the result stored in the constant.
constant * constant::evaluate (void)
{
    setResult (this);
    return getResult ();
}

// Returns the derivative of a constant.
node * constant::differentiate (char *)
{
    constant * res = new constant (TAG_DOUBLE);
    res->d = 0;
    return res;
}

// Constructor creates an instance of the reference class.
reference::reference () : node (REFERENCE)
{
    n = NULL;
    ref = NULL;
}

/* This copy constructor creates a instance of the reference class
   based on the given reference. */
reference::reference (const reference & o) : node (o)
{
    n = o.n ? strdup (o.n) : NULL;
    ref = o.ref;
}

// Re-creates the given instance.
node * reference::recreate (void)
{
    return new reference (*this);
}

// Replaces reference name by the new given name.
void reference::replace (char * src, char * dst)
{
    if (!strcmp (src, n))
    {
        free (n);
        n = dst ? strdup (dst) : NULL;
    }
}

// Destructor deletes an instance of the reference class.
reference::~reference ()
{
    free (n);
}

// Prints textual representation of the reference object.
void reference::print (void)
{
    logprint (LOG_STATUS, "%s", toString ());
}

// Returns textual representation of the reference object.
char * reference::toString (void)
{
    free (txt);
    txt = strdup (n);
    return txt;
}

// Adds the name of the reference to the list of dependencies.
void reference::addDependencies (strlist * depends)
{
    depends->add (n);
    findVariable ();
}

// Find and save the actual equation reference.
void reference::findVariable (void)
{
    ref = NULL; // force reference to be updated
    if (!ref)
    {
        node * eqn;
        if (checkee != NULL)
        {
            for (eqn = checkee->getEquations (); eqn; eqn = eqn->getNext ())
            {
                if (!strcmp (n, A(eqn)->result))
                {
                    ref = eqn;
                    break;
                }
            }
        }
        if (solvee != NULL && !ref)
        {
            for (eqn = solvee->getEquations (); eqn; eqn = eqn->getNext ())
            {
                if (!strcmp (n, A(eqn)->result))
                {
                    ref = eqn;
                    break;
                }
            }
        }
    }
}

// Returns the type of reference.
int reference::evalType (void)
{
    setType (TAG_UNKNOWN);
    findVariable ();
    if (ref != NULL)
    {
        setType (A(ref)->body->evalType ());
    }
    return getType ();
}

// Returns the actual result of the reference.
constant * reference::evaluate (void)
{
    setResult (NULL);
    findVariable ();
    if (ref != NULL)
    {
        setResult (A(ref)->body->getResult ());
    }
    return getResult ();
}

// Returns the derivative of a reference.
node * reference::differentiate (char * derivative)
{
    constant * res = new constant (TAG_DOUBLE);
    if (n != NULL && !strcmp (n, derivative))
        res->d = 1;
    else
        res->d = 0;
    return res;
}

// Constructor creates an instance of the assignment class.
assignment::assignment () : node (ASSIGNMENT)
{
    body = NULL;
    result = NULL;
}

/* This copy constructor creates a instance of the assignment class
   based on the given assignment. */
assignment::assignment (const assignment & o) : node (o)
{
    body = o.body->recreate ();
    result = o.result ? strdup (o.result) : NULL;
}

// Re-creates the given instance.
node * assignment::recreate (void)
{
    return new assignment (*this);
}

// Replaces reference name by the new given name.
void assignment::replace (char * src, char * dst)
{
    body->replace (src, dst);
}

// Renames the left hand side of the assignment.
void assignment::rename (char * n)
{
    free (result);
    result = n ? strdup (n) : NULL;
}

// Destructor deletes an instance of the assignment class.
assignment::~assignment ()
{
    delete body;
    free (result);
}

// Prints textual representation of the assignment object.
void assignment::print (void)
{
    logprint (LOG_STATUS, "%s", toString ());
}

// Returns textual representation of the assignment object.
char * assignment::toString (void)
{
    free (txt);
    char * str = body->toString ();
    txt = (char *) malloc (strlen (result) + strlen (str) + 4);
    sprintf (txt, "%s = %s", result, str);
    return txt;
}

// Adds the right hand side of the assignment to the list of dependencies.
void assignment::addDependencies (strlist * depends)
{
    body->checkee = checkee;
    body->addDependencies (depends);
}

// Returns the type of assignment.
int assignment::evalType (void)
{
    setType (body->evalType ());
    return getType ();
}

// Returns the result of the assignment.
constant * assignment::evaluate (void)
{
    body->solvee = solvee;
    setResult (body->evaluate ());
    // inherit drop/prep dependencies of applications
    if (body->getResult()->dropdeps)
    {
        getResult()->dropdeps = body->getResult()->dropdeps;
        strlist * preps = body->getPrepDependencies ();
        if (preps) getResult()->setPrepDependencies (new strlist (*preps));
    }
    return getResult ();
}

// Returns the derivative of an assignment.
node * assignment::differentiate (char * derivative)
{
    char * txt = (char *) malloc (strlen (result) + strlen (derivative) + 4);
    sprintf (txt, "d%s_d%s", result, derivative);
    assignment * res = new assignment ();
    res->result = txt;
    res->body = body->differentiate (derivative);
    return res;
}

// Some small helpers.
#define D(con) (C(con)->d)
#define isConst(n) ((n)->getTag()==CONSTANT && C(n)->getType()==TAG_DOUBLE)
#define isZero(n)  (isConst(n) && D(n) == 0.0)
#define isOne(n)   (isConst(n) && D(n) == 1.0)
#define defCon(res,val) res = new constant (TAG_DOUBLE); C(res)->d = val;

/* Multiply two assignments. */
void assignment::mul (assignment * f)
{
    node * factor = f->body->recreate ();
    if (isZero (body) || isZero (factor))
    {
        delete body;
        delete factor;
        defCon (body, 0);
    }
    else if (isOne (body))
    {
        delete body;
        body = factor;
    }
    else if (isOne (factor))
    {
        delete factor;
        //body = body;
    }
    else
    {
        application * mul = new application ("*", 2);
        mul->args = body;
        mul->args->append (factor);
        body = mul;
    }
}

/* Multiply two assignments by reference. */
void assignment::mulref (assignment * f)
{
    node * factor = f->body->recreate ();
    reference * r = new reference ();
    r->n = strdup (f->result);
    if (isZero (body) || isZero (factor))
    {
        delete body;
        defCon (body, 0);
    }
    else if (isOne (body))
    {
        body = r;
    }
    else if (isOne (factor))
    {
        //body = body;
    }
    else
    {
        application * mul = new application ("*", 2);
        mul->args = body;
        mul->args->append (r);
        body = mul;
    }
}

/* Add two assignments. */
void assignment::add (assignment * f)
{
    node * factor = f->body->recreate ();
    if (isZero (body) && isZero (factor))
    {
        delete body;
        delete factor;
        defCon (body, 0);
    }
    else if (isZero (body))
    {
        delete body;
        body = factor;
    }
    else if (isZero (factor))
    {
        delete factor;
        //body = body;
    }
    else
    {
        application * add = new application ("+", 2);
        add->args = body;
        add->args->append (factor);
        body = add;
    }
}

// Constructor creates an instance of the application class.
application::application () : node (APPLICATION)
{
    n = NULL;
    nargs = 0;
    args = NULL;
    eval = NULL;
    derive = NULL;
    ddx = NULL;
}

/* Constructor creates an instance of the application class with a
   given function name and the number of arguments. */
application::application (const char * func, int a) : node (APPLICATION)
{
    n = func ? strdup (func) : NULL;
    nargs = a;
    args = NULL;
    eval = NULL;
    derive = NULL;
    ddx = NULL;
}

/* This copy constructor creates a instance of the application class
   based on the given application. */
application::application (const application & o) : node (o)
{
    n = o.n ? strdup (o.n) : NULL;
    nargs = o.nargs;
    if (o.args != NULL)
    {
        node * arg = o.args;
        args = arg->recreate ();
        for (arg = arg->getNext (); arg != NULL; arg = arg->getNext ())
        {
            args->append (arg->recreate ());
        }
    }
    else args = NULL;
    eval = o.eval;
    derive = o.derive;
    ddx = o.ddx ? o.ddx->recreate () : NULL;
}

// Re-creates the given instance.
node * application::recreate (void)
{
    return new application (*this);
}

// Replaces reference name by the new given name.
void application::replace (char * src, char * dst)
{
    for (node * arg = args; arg != NULL; arg = arg->getNext ())
    {
        arg->replace (src, dst);
    }
    if (ddx) ddx->replace (src, dst);
}

// Destructor deletes an instance of the application class.
application::~application ()
{
    node * next;
    for (node * arg = args; arg != NULL; arg = next)
    {
        next = arg->getNext ();
        delete arg;
    }
    delete getResult ();
    free (n);
    delete ddx;
}

// Prints textual representation of the application object.
void application::print (void)
{
    logprint (LOG_STATUS, "%s", toString ());
}

// Returns textual representation of the application object.
char * application::toString (void)
{
    int nparam = nargs > 0 ? (nargs - 1) : 0;
    free (txt);
    // binary operations
    if ((!strcmp (n, "+")  || !strcmp (n, "-")  || !strcmp (n, "*") ||
            !strcmp (n, "/")  || !strcmp (n, "^")  || !strcmp (n, "%") ||
            !strcmp (n, "<")  || !strcmp (n, ">")  || !strcmp (n, "<=") ||
            !strcmp (n, ">=") || !strcmp (n, "&&") || !strcmp (n, "||") ||
            !strcmp (n, "==") || !strcmp (n, "!="))
            && nargs == 2)
    {
        char * arg1 = args->toString ();
        char * arg2 = args->getNext()->toString ();
        txt = (char *) malloc (strlen (n) + strlen (arg1) + strlen (arg2) + 3);
        sprintf (txt, "(%s%s%s)", arg1, n, arg2);
    }
    // ternary ?: operator
    else if (!strcmp (n, "?:"))
    {
        char * arg1 = args->toString ();
        char * arg2 = args->getNext()->toString ();
        char * arg3 = args->getNext()->getNext()->toString ();
        txt = (char *) malloc (strlen (arg3) + strlen (arg1) + strlen (arg2) + 5);
        sprintf (txt, "(%s?%s:%s)", arg1, arg2, arg3);
    }
    // array indices
    else if (!strcmp (n, "array"))
    {
        int len = strlen (args->toString ()) + 3 + nparam;
        txt = (char *) malloc (len);
        sprintf (txt, "%s[", args->toString ());
        for (node * arg = args->getNext (); arg != NULL; arg = arg->getNext ())
        {
            char * str = arg->toString ();
            txt = (char *) realloc (txt, len += strlen (str));
            strcat (txt, str);
            if (arg->getNext ()) strcat (txt, ",");
        }
        strcat (txt, "]");
    }
    // vectors and matrices
    else if (!strcmp (n, "vector") || !strcmp (n, "matrix"))
    {
        int len = 3 + nparam;
        txt = (char *) malloc (len);
        sprintf (txt, "[");
        for (node * arg = args; arg != NULL; arg = arg->getNext ())
        {
            if (arg->getType () == TAG_CHAR)
            {
                txt = (char *) realloc (txt, len++);
                strcat (txt, ";");
            }
            else
            {
                char * str = arg->toString ();
                txt = (char *) realloc (txt, len += strlen (str));
                strcat (txt, str);
                node * next = arg->getNext ();
                if (next && next->getType () != TAG_CHAR) strcat (txt, ",");
            }
        }
        strcat (txt, "]");
    }
    // unary and n-ary operations here
    else
    {
        int len = strlen (n) + 3 + nparam;
        txt = (char *) malloc (len);
        sprintf (txt, "%s(", n);
        for (node * arg = args; arg != NULL; arg = arg->getNext ())
        {
            char * str = arg->toString ();
            txt = (char *) realloc (txt, len += strlen (str));
            strcat (txt, str);
            if (arg->getNext ()) strcat (txt, ",");
        }
        strcat (txt, ")");
    }
    return txt;
}

// Adds the arguments of the application to the list of dependencies.
void application::addDependencies (strlist * depends)
{
    for (node * arg = args; arg != NULL; arg = arg->getNext ())
    {
        arg->checkee = checkee;
        arg->addDependencies (depends);
    }
}

/* This function goes through the arguments of an application and
   evaluates their return types. */
void application::evalTypeArgs (void)
{
    for (node * arg = args; arg != NULL; arg = arg->getNext ())
    {
        // Skip evaluating generated reference variables.
        if (arg->getTag () == REFERENCE)
            if (checker::isGenerated (R (arg)->n))
                continue;
        // Evaluate the type of argument.
        arg->evalType ();
    }
}


// gperfapphash has register inside, ignore warning
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif
#include "gperfapphash.cpp"

/* The function creates a hash key for the given type of
   application. */
char * application::createKey (void)
{
    char * key = (char *) calloc (1, strlen (n) + nargs * 3 + 5);
    strcat (key, n);
    for (node * arg = args; arg != NULL; arg = arg->getNext ())
    {
        strcat (key, "_");
        strcat (key, checker::tag2key (arg->getType ()));
    }
    return key;
}

/* This function returns the return type of the application using a
   gperf-generated hash. */
int application::evalTypeFast (void)
{
    char * key = createKey ();
    struct appindex * idx = gperfapphash::get (key, strlen (key));
    free (key);
    if (idx != NULL)
    {
        application_t * app = &applications[idx->index];
        if (app->eval)
        {
            eval = app->eval;
            setType (app->retval);
        }
    }
    return getType ();
}

// Macro to identify ddx() application.
#define isDDX() (nargs == 2 && !strcmp (n, "ddx") && \
                 args->getNext()->getTag () == REFERENCE)

/* Returns the type of application and applies the appropriate
   evaluation function if any. */
int application::evalType (void)
{
    // Evaluate type of ddx().
    if (isDDX ())
    {
        args->evalType ();
        if (!ddx) ddx = args->differentiate (R(args->getNext())->n);
        setType (ddx->evalType ());
        return getType ();
    }
    setType (TAG_UNKNOWN);
    // Evaluate type of arguments.
    evalTypeArgs ();
    // Find an appropriate differentiator.
    findDifferentiator ();
    // Try the fast method.
    if (evalTypeFast () != TAG_UNKNOWN) return getType ();

    // Go through the list of available applications.
    for (int i = 0; applications[i].application != NULL; i++)
    {
        application_t * app = &applications[i];
        // The correct application?
        if (!strcmp (n, app->application))
        {
            int nr = 0;
            if (app->nargs >= 0)
            {
                // The correct number of arguments?
                if (nargs != app->nargs) continue;
                // The correct types of arguments?
                for (node * arg = args; arg != NULL; arg = arg->getNext (), nr++)
                {
                    if (arg->getTag () == REFERENCE)
                        // Skip checking generated reference variables.
                        if (checker::isGenerated (R (arg)->n))
                            continue;
                    // Evaluate and check the type of argument.
                    if (!(arg->getType () & app->args[nr]))
                    {
                        nr = -1;
                        break;
                    }
                }
                if (nr == -1) continue;
            }
            // A valid application function?
            if (app->eval == NULL) continue;
            // Everything just fine here.
            eval = app->eval;
            setType (app->retval);
            break;
        }
    }
    // Emit error message if necessary.
    if (getType () == TAG_UNKNOWN)
    {
        logprint (LOG_ERROR, "checker error, no appropriate function for `%s'"
                  " found\n", toString ());
    }
    return getType ();
}

/* This function returns zero if the applications differentiation
   function could be found and otherwise non-zero. */
int application::findDifferentiator (void)
{
    for (int i = 0; differentiations[i].application != NULL; i++)
    {
        if (!strcmp (n, differentiations[i].application) &&
                nargs == differentiations[i].nargs)
        {
            derive = differentiations[i].derive;
            return 0;
        }
    }
    return -1;
}

/* This function runs the actual evaluation function and the returns
   the result. */
constant * application::evaluate (void)
{
    // Evaluate ddx() function.
    if (isDDX ())
    {
        delete getResult ();
        setResult (C (ddx->evaluate()->recreate ()));
        return getResult ();
    }

    int errors = 0;
    strlist * apreps = new strlist ();

    // first evaluate each argument
    for (node * arg = args; arg != NULL; arg = arg->getNext ())
    {
        // FIXME: Can save evaluation of already evaluated equations?
        if (arg->evaluated == 0 || 1)
        {
            arg->solvee = solvee;
            arg->evaluate ();
            if (arg->getResult () == NULL)
            {
                if (arg->getTag () == REFERENCE)
                {
                    logprint (LOG_ERROR, "evaluate error, no such generated variable "
                              "`%s'\n", arg->toString ());
                }
                else
                {
                    logprint (LOG_ERROR, "evaluate error, unable to evaluate "
                              "`%s'\n", arg->toString ());
                }
                errors++;
            }
            else
            {
                // inherit drop/prep dependencies
                if (arg->getResult()->dropdeps)
                {
                    strlist * preps = arg->getResult()->getPrepDependencies ();
                    // recall longest prep dependencies' list of arguments
                    if (preps && (preps->length () > apreps->length ()))
                    {
                        delete apreps;
                        apreps = new strlist (*preps);
                    }
                }
                arg->evaluated++;
            }
        }
    }

    // then evaluate application itself
    if (!errors)
    {
        // delete previous result if necessary
        delete getResult ();
        // then evaluate the application
        setResult (eval (C (args)));
        // check the returned type once again
        if (getResult()->getType () != getType ())
        {
            logprint (LOG_ERROR, "evaluate error, function `%s' returned invalid "
                      "constant type\n", toString ());
        }
    }

    // inherit prep dependencies of arguments if necessary
    if (!getResult()->dropdeps && apreps->length () > 0)
    {
        getResult()->dropdeps = 1;
        getResult()->appendPrepDependencies (apreps);
    }
    delete apreps;

    return getResult ();
}

// Returns the derivative of an application.
node * application::differentiate (char * derivative)
{
    if (isDDX ())
    {
        return ddx->differentiate (derivative);
    }
    if (derive)
        return derive (this, derivative);
    return recreate ();
}

// Constructor creates an untyped instance of the equation node class.
node::node ()
{
    tag = UNKNOWN;
    setType(TAG_UNKNOWN);
    dropdeps = output = evaluated = evalPossible = cycle = duplicate = skip = 0;
    next = NULL;
    dependencies = NULL;
    dataDependencies = NULL;
    dropDependencies = NULL;
    prepDependencies = NULL;
    txt = NULL;
    res = NULL;
    instance = NULL;
    solvee = NULL;
    checkee = NULL;
}

// This constructor creates an typed instance of the equation node class.
node::node (int type)
{
    tag = type;
    setType(TAG_UNKNOWN);
    dropdeps = output = evaluated = evalPossible = cycle = duplicate = skip = 0;
    next = NULL;
    dependencies = NULL;
    dataDependencies = NULL;
    dropDependencies = NULL;
    prepDependencies = NULL;
    txt = NULL;
    res = NULL;
    instance = NULL;
    solvee = NULL;
    checkee = NULL;
}

/* This copy constructor creates a instance of the node class based on
   the given node. */
node::node (const node & o)
{
    tag = o.tag;
    type = o.type;
    dropdeps = output = evaluated = evalPossible = cycle = duplicate = skip = 0;
    next = NULL;
    dependencies = NULL;
    dataDependencies = NULL;
    dropDependencies = NULL;
    prepDependencies = NULL;
    txt = NULL;
    res = NULL;
    instance = NULL;
    solvee = o.solvee;
    checkee = o.checkee;
}

// Destructor deletes an instance of the equation node class.
node::~node ()
{
    delete dependencies;
    delete dataDependencies;
    delete dropDependencies;
    delete prepDependencies;
    free (txt);
    free (instance);
}

// Sets the instance name where the node occurred.
void node::setInstance (const char * n)
{
    free (instance);
    instance = n ? strdup (n) : NULL;
}

// Returns the instance name where the node occurred.
char * node::getInstance (void)
{
    return instance;
}

/* The function applies the instance name of the current equation node
   to any following node within the list up to the node with a valid
   instance name. */
void node::applyInstance (void)
{
    char * i = getInstance ();
    for (node * n = getNext (); n != NULL; n = n->getNext ())
    {
        if (n->getInstance () == NULL)
            n->setInstance (i);
        else
            break;
    }
}

// Counts the number of equations node attached to the node.
int node::count (void)
{
    int c = 0;
    for (node * n = this; n != NULL; n = n->getNext ()) c++;
    return c;
}

// Appends yet another node to the equation node object.
void node::append (node * last)
{
    if (!last) return;
    node * n;
    for (n = this; n->getNext () != NULL; n = n->getNext ()) ;
    last->setNext (NULL);
    n->setNext (last);
}

// Appends other nodes to the equation node object.
void node::appendNodes (node * last)
{
    if (!last) return;
    node * n;
    for (n = this; n->getNext () != NULL; n = n->getNext ()) ;
    n->setNext (last);
}

// Returns the equation node at the given argument position.
node * node::get (int pos)
{
    node * n = this;
    for (int i = 0; i < pos && n != NULL; n = n->getNext (), i++) ;
    return n;
}

// Sets the constant equation node result.
void node::setResult (constant * r)
{
    res = r;
}

// Returns the constant equation node at the given argument position.
constant * node::getResult (int pos)
{
    node * n = this;
    for (int i = 0; i < pos && n != NULL; n = n->getNext (), i++) ;
    return n ? n->getResult () : NULL;
}

/* Returns a double value depending on the type of the equation nodes
   result type. */
nr_double_t node::getResultDouble (void)
{
    constant * c = getResult ();
    if (c != NULL)
    {
        switch (getType ())
        {
        case TAG_DOUBLE:
            return c->d;
            break;
        case TAG_COMPLEX:
            return real (*(c->c));
            break;
        case TAG_BOOLEAN:
            return c->b ? 1.0 : 0.0;
            break;
        }
    }
    return 0.0;
}

/* Returns a complex value depending on the type of the equation nodes
   result type. */
nr_complex_t node::getResultComplex (void)
{
    constant * c = getResult ();
    if (c != NULL)
    {
        switch (getType ())
        {
        case TAG_DOUBLE:
            return nr_complex_t (c->d, 0.0);
            break;
        case TAG_COMPLEX:
            return *(c->c);
            break;
        case TAG_BOOLEAN:
            return c->b ? 1.0 : 0.0;
            break;
        }
    }
    return 0.0;
}

/* Returns an immediate vector depending on the type of the equation
   nodes result type. */
qucs::vector node::getResultVector (void)
{
    constant * c = getResult ();
    qucs::vector v;
    if (c != NULL)
    {
        switch (getType ())
        {
        case TAG_MATRIX:
        {
            int ro, co, n = 0;
            v = qucs::vector (c->m->getRows () * c->m->getCols ());
            for (co = 0; co < c->m->getCols (); co++)
                for (ro = 0; ro < c->m->getRows (); ro++)
                    v (n++) = c->m->get (ro, co);
        }
        break;
        case TAG_VECTOR:
            v = *(c->v);
            break;
        case TAG_DOUBLE:
            v = qucs::vector (1);
            v (0) = c->d;
            break;
        case TAG_COMPLEX:
            v = qucs::vector (1);
            v (0) = *(c->c);
            break;
        case TAG_BOOLEAN:
            v = qucs::vector (1);
            v (0) = c->b ? 1.0 : 0.0;
            break;
        }
    }
    return v;
}

// Assigns the dependency list to the equation node object.
void node::setDependencies (strlist * depends)
{
    delete dependencies;
    dependencies = depends;
}

// Returns the dependency list of the equation node object.
strlist * node::getDependencies (void)
{
    return dependencies;
}

/* This function recursively finds the variable dependencies for each
   equation initially passed to the equation checker and returns the
   list of variable dependencies regarding this equation instance.
   The caller is responsible for deleting the returned string list
   object. */
strlist * node::recurseDependencies (checker * check, strlist * deps)
{

    strlist * res, * sub = NULL;

    /* The abort condition for recursion first. */
    if (deps->contains (A(this)->result))
    {
        res = new strlist (*deps);
        cycle = 1;
        return res;
    }

    /* Go through the list of passed dependency variables. */
    for (int i = 0; i < deps->length (); i++)
    {
        char * var = deps->get (i);
        node * child = check->findEquation (check->equations, var);
        /* Check each child equation. */
        if (child != NULL)
        {
            if (child->cycle == 0)
            {
                strlist * cdeps = child->getDependencies ();
                /* And append the dependencies of the child equation. */
                if (cdeps->length () > 0)
                {
                    res = strlist::join (sub, cdeps);
                    delete sub;
                    sub = res;
                }
            }
            /* If any child is cyclic, the parent is too. */
            else
            {
                cycle = 1;
            }
        }
    }

    /* Recurse once again if the child equations revealed any more
       dependencies. */
    if (cycle && sub && sub->length () > 0)
    {
        res = recurseDependencies (check, sub);
        delete sub;
        sub = res;
    }

    /* Return the result. */
    res = strlist::join (deps, sub);
    delete (sub);
    return res;
}

/* The function adds the given data dependency to the list of
   dependencies which are going to be dropped during the data
   export. */
void node::addDropDependencies (char * dep)
{
    if (dropDependencies == NULL) dropDependencies = new strlist ();
    dropDependencies->add (dep);
}

/* The function adds the given data dependency to the list of
   dependencies which are going to be prepend. */
void node::addPrepDependencies (char * dep)
{
    if (prepDependencies == NULL) prepDependencies = new strlist ();
    prepDependencies->add (dep);
}

/* This function appends the given dependency list to the list of
   dependencies which are going to be prepend. */
void node::appendPrepDependencies (strlist * deps)
{
    if (prepDependencies == NULL) prepDependencies = new strlist ();
    prepDependencies->append (deps);
}

/* The function sets the data dependency list of the equation node. */
void node::setDataDependencies (strlist * deps)
{
    delete dataDependencies;
    dataDependencies = deps ? new strlist (*deps) : NULL;
}

/* Evaluates the equation node and applies the data dependencies. */
constant * node::calculate (void)
{
    constant * res = evaluate ();
    if (getResult ())
    {
        strlist * deps = solvee->collectDataDependencies (this);
        getResult()->setDataDependencies (deps);
        delete deps;
    }
    else
    {
        qucs::exception * e = new qucs::exception (EXCEPTION_MATH);
        e->setText ("evaluator exception");
        throw_exception (e);
    }
    return res;
}

/* Collects the equation dependencies for a specific node. */
strlist * node::collectDependencies (void)
{
    strlist * depends = new strlist ();
    addDependencies (depends);
    setDependencies (checker::foldDependencies (depends));
    return getDependencies ();
}

/* Collects the data dependencies for a specific node. */
strlist * node::collectDataDependencies (void)
{
    strlist * deps = getResult()->getDataDependencies ();
    if (deps)
    {
        // data dependencies already collected
        setDataDependencies (deps);
        return deps;
    }
    // collect equation dependencies
    if (!getDependencies ())
        collectDependencies ();
    if (solvee)
    {
        // finally collect the appropriate data dependencies
        deps = solvee->collectDataDependencies (this);
        setDataDependencies (deps);
        delete deps;
    }
    return getDataDependencies ();
}

// Constructor creates an instance of the checker class.
checker::checker ()
{
    defs = NULL;
    equations = NULL;
    consts = false;
}

// Destructor deletes an instance of the checker class.
checker::~checker ()
{
    node * next;
    for (node * eqn = equations; eqn != NULL; eqn = next)
    {
        next = eqn->getNext ();
        delete eqn;
    }
}

// Local macro definition to go through the list of equations.
#define foreach_equation(eqn)                        \
  for (assignment * (eqn) = A (equations);           \
       (eqn) != NULL; (eqn) = A ((eqn)->getNext ()))

/* The function goes through the list of equations assigned to the
   checker and applies the dependency list. */
void checker::collectDependencies (void)
{
    foreach_equation (eqn)
    {
        collectDependencies (eqn);
    }
}

// Creates dependency list of given equation node.
void checker::collectDependencies (node * eqn)
{
    strlist * depends = new strlist ();
    eqn->addDependencies (depends);
    eqn->setDependencies (depends);
}

/* The following function goes through the list of equations and
   checks whether there is any kind of 'Export="yes|no"' assignment in
   it.  Depending on the value the referred equation results are saved
   into the dataset or not. */
int checker::checkExport (void)
{
    int errors = 0;
    assignment * next;
    // go through all equations
    for (assignment * eqn = A (equations); eqn != NULL; eqn = next)
    {
        next = A (eqn->getNext ());
        // 'Export' equation found ?
        if (!strcmp (eqn->result, "Export"))
        {
            // is the type and value correct ?
            if (eqn->body->getTag () != REFERENCE ||
                    (strcmp (R (eqn->body)->n, "yes") &&
                     strcmp (R (eqn->body)->n, "no")))
            {
                logprint (LOG_ERROR, "checker error, variable `%s' alternatives "
                          "are `yes' or `no'\n", eqn->result);
                errors++;
            }
            else
            {
                int flag = !strcmp (R (eqn->body)->n, "yes") ? 1 : 0;
                char * i = eqn->getInstance ();
                int found = 0;
                // set output flag for each equation with the same instance name
                foreach_equation (res)
                {
                    if (!strcmp (res->getInstance (), i))
                        res->output = flag;
                    if (!strcmp (res->result, "Export") &&
                            !strcmp (res->getInstance (), i))
                    {
                        found++;
                    }
                }
                // check for duplicate definitions of 'Export'
                if (found > 1)
                {
                    logprint (LOG_ERROR, "checker error, variable `%s' "
                              "occurred %dx in `Eqn:%s'\n", eqn->result, found, i);
                    errors++;
                }
                // drop the 'Export' equation being useless now
                dropEquation (eqn);
                delete eqn;
            }
        }
    }
    return errors;
}

// Logs the textual representation of all equations.
void checker::list (void)
{
    for (node * eqn = equations; eqn != NULL; eqn = eqn->getNext ())
    {
        logprint (LOG_STATUS, "%s", eqn->evalPossible ? "!" : "?");
        logprint (LOG_STATUS, "%s", eqn->evalPossible ?
                  (eqn->getType () == TAG_UNKNOWN ? "U!" :
                   eqn->getType () == TAG_DOUBLE  ? "D!" :
                   eqn->getType () == TAG_BOOLEAN ? "B!" :
                   eqn->getType () == TAG_COMPLEX ? "C!" :
                   eqn->getType () == TAG_VECTOR  ? "V!" :
                   eqn->getType () == TAG_CHAR    ? "CHR!" :
                   eqn->getType () == TAG_STRING  ? "STR!" :
                   eqn->getType () == TAG_MATVEC  ? "MV!" :
                   eqn->getType () == TAG_RANGE   ? "R!" :
                   eqn->getType () == TAG_MATRIX  ? "M!" : "?!") : "");
        eqn->print ();
        logprint (LOG_STATUS, "\n");
    }
}

/* Checks whether the variable name is a generated name which is
   identified by a ".[0-9]{4}" suffix. */
int checker::isGenerated (char * var)
{
    int len = strlen (var);
    if (len > 5)
    {
        if (isdigit (var[len-1]) && isdigit (var[len-2]) &&
                isdigit (var[len-3]) && isdigit (var[len-4]) &&
                var[len-5] == '.')
        {
            return 1;
        }
    }
    return 0;
}

/* This function checks whether the variable references could be
   resolved within the equations and returns zero if so. */
int checker::findUndefined (int noundefined)
{
    int err = 0;
    strlist * idents = getVariables ();

    foreach_equation (eqn)
    {
        strlist * depends = eqn->getDependencies ();
        for (int i = 0; i < depends->length (); i++)
        {
            char * var = depends->get (i);
            if (idents->contains (var) <= 0)
            {
                // check if this is a circuit property
                if (defs)
                {
                    node * eqn = findProperty (var);
                    if (eqn)
                    {
                        idents->append (var);
                        eqn->collectDependencies ();
                        continue;
                    }
                }
                // give an error
                if (noundefined)
                {
                    if (isGenerated (var)) // skip probably generated variables
                        continue;
                    logprint (LOG_ERROR, "checker error, undefined variable `%s' in "
                              "equation `%s'\n", var, eqn->result);
                    err++;
                }
                // give a notice only
                else
                {
                    logprint (LOG_STATUS, "checker notice, variable `%s' in "
                              "equation `%s' not yet defined\n", var, eqn->result);
                }
            }
        }
    }
    delete idents;
    return err;
}

/* This function tries to find the given variable name which occurred
   in an equation dependency in the netlist.  If there is such a
   circuit property it returns a new assignment equation. */
node * checker::findProperty (char * var)
{

    node * eqn = NULL;
    int found = 0;

    // split into instance and property name
    char * ret, * inst, * prop;
    if ((ret = strchr (var, '.')) != NULL)
    {
        int len = ret - var;
        inst = (char *) calloc (1, len + 1);
        memcpy (inst, var, len);
        prop = &var[len + 1];
    }
    else return NULL;

    // go through list of circuit elements
    for (struct definition_t * def = defs; def; def = def->next)
    {
        if (!strcmp (def->instance, inst))
        {
            for (struct pair_t * pair = def->pairs; pair; pair = pair->next)
            {
                if (!strcmp (pair->key, prop))
                {
                    if (++found == 1)
                    {
                        if (pair->value->ident != NULL)
                        {
                            // reference
                            eqn = createReference ("#property", var, pair->value->ident);
                        }
                        else
                        {
                            // value
                            eqn = createDouble ("#property", var, pair->value->value);
                        }
                    }
                }
            }
        }
    }
    if (found > 1)
    {
        logprint (LOG_ERROR, "checker error, desired property variable `%s' found "
                  "%dx, is not unique'\n", var, found);
        delete eqn;
        eqn = NULL;
    }
    else if (found == 1)
        appendEquation (eqn);
    free (inst);
    return eqn;
}

/* Go through the list of equations and store the left hand side in
   a string list. */
strlist * checker::getVariables (void)
{
    strlist * idents = new strlist ();
    foreach_equation (eqn)
    {
        idents->add (eqn->result);
    }
    return idents;
}

/* Finds duplicate equation definitions in the list of equations,
   emits appropriate error messages and returns zero if everything is
   ok. */
int checker::findDuplicate (void)
{
    int err = 0;
    strlist * idents = getVariables ();
    strlist * dups = new strlist ();

    // Collect duplicate entries.
    foreach_equation (eqn)
    {
        if (!eqn->duplicate && dups->contains (eqn->result) == 0)
        {
            eqn->duplicate = idents->contains (eqn->result);
            dups->add (eqn->result);
        }
        else
        {
            eqn->duplicate = 1;
        }
    }
    // Emit appropriate error messages.
    foreach_equation (eqndups)
    {
        if (eqndups->duplicate > 1)
        {
            logprint (LOG_ERROR, "checker error, variable `%s' assigned %dx\n",
                      eqndups->result, eqndups->duplicate);
            err++;
        }
    }
    delete idents;
    delete dups;
    return err;
}

/* The function returns the equation resulting in the passed variable
   or NULL if there is no such equation.  The function looks through
   the passed equation root. */
node * checker::findEquation (node * root, const char * const n)
{
    for (node * eqn = root; eqn != NULL; eqn = eqn->getNext ())
    {
        if (!strcmp (A(eqn)->result, n))
	  return eqn;
    }
    return NULL;
}

/* The function returns the equation resulting in the passed variable
   or NULL if there is no such equation. */
node * checker::findEquation (const char * const n) const
{
    foreach_equation (eqn)
    {
        if (!strcmp (A(eqn)->result, n)) return eqn;
    }
    return NULL;
}

/* This function display the error messages due to equation cycles and
   returns zero if there are no such cycles. */
int checker::detectCycles (void)
{
    int err = 0;

    foreach_equation (eqn)
    {
        strlist * deps = eqn->recurseDependencies (this, eqn->getDependencies ());
        if (deps->contains (eqn->result) || eqn->cycle)
        {
            logprint (LOG_ERROR, "checker error, cyclic definition of variable "
                      "`%s' involves: `%s'\n", eqn->result, deps->toString ());
            err++;
            delete deps;
        }
        else
        {
            // Set folded variable dependencies.
            deps = foldDependencies (deps);
            eqn->setDependencies (deps);
        }
    }
    return err;
}

/* The function returns a variable dependency list with unique entries
   only.  The given string list gets deleted and a new one is created
   and returned. */
strlist * checker::foldDependencies (strlist * deps)
{
    strlist * res = new strlist ();
    for (int i = 0; deps && i < deps->length (); i++)
    {
        char * var = deps->get (i);
        if (!res->contains (var)) res->append (var);
    }
    delete deps;
    return res;
}

// The function appends the given last node to the given equation root.
node * checker::appendEquation (node * root, node * last)
{
    last->setNext (NULL);
    if (root != NULL)
    {
        node * eqn = lastEquation (root);
        eqn->setNext (last);
    }
    else root = last;
    return root;
}

// Returns the last node in the given equation root.
node * checker::lastEquation (node * root)
{
    node * eqn;
    for (eqn = root; eqn && eqn->getNext () != NULL; eqn = eqn->getNext ()) ;
    return eqn;
}

// Removes the given equation node from the list of known equations.
void checker::dropEquation (node * eqn)
{
    if (eqn == equations)
    {
        equations = eqn->getNext ();
    }
    else
    {
        node * prev;
        for (prev = equations; prev->getNext () != eqn; prev = prev->getNext()) ;
        prev->setNext (eqn->getNext ());
    }
}

/* This function reorders the list of equations.  The new order can be
   used to evaluate the list step by step.  Each equation being
   evaluable is properly marked, remaining equations are appended. */
void checker::reorderEquations (void)
{
    node * root = NULL, * next, * last;

    // Go through the list of equations.
    for (node * eqn = equations; eqn != NULL; eqn = next)
    {
        strlist * deps = eqn->getDependencies ();
        int i, found, gens;
        next = eqn->getNext ();
        /* Check whether the variable dependencies can be found in
           previous equations. */
        for (found = gens = i = 0; i < deps->length (); i++)
        {
            char * var = deps->get (i);
            if (findEquation (root, var) != NULL) found++;
            if (isGenerated (var)) gens++;
        }
        // Yes.
        if (found == (deps->length () - gens))
        {
            /* Remove the equation from the current list and append it to
               the new list. */
            dropEquation (eqn);
            root = appendEquation (root, eqn);
            eqn->evalPossible = 1;
            // Now start over from the beginning.
            next = equations;
        }
    }
    // Any remaining equations get appended.
    if (root != NULL)
    {
        last = lastEquation (root);
        last->setNext (equations);
        equations = root;
    }
}


/* The function passes a list of equations to the checker and also
   passes the checker instance to each equation. */
void checker::setEquations (node * eqns)
{
    equations = eqns;
    foreach_equation (eqn)
    {
        eqn->checkee = this;
    }
}

/* The function evaluates the types for each equation and recursively
   checks the availability of the appropriate function. */
int checker::applyTypes (void)
{
    int err = 0;
    foreach_equation (eqn)
    {
        if (eqn->evalPossible)
        {
            if (eqn->evalType () == TAG_UNKNOWN)
            {
                logprint (LOG_ERROR, "checker error, type of equation `%s' "
                          "undefined\n", eqn->result);
                err++;
            }
        }
        else break;
    }
    return err;
}

/* This function is the checker routine for a parsed equations.  It
   returns zero on success or non-zero if the parsed equations
   contained errors. */
int checker::check (int noundefined)
{
    int err = 0;
    err += checkExport ();
    collectDependencies ();
    err += findUndefined (noundefined);
    err += findDuplicate ();
    err += detectCycles ();
    reorderEquations ();
    err += applyTypes ();
#if DEBUG && 0
    list ();
#endif /* DEBUG */
    return err;
}

// Constructor creates an instance of the solver class.
solver::solver (checker * c)
{
    equations = NULL;
    data = NULL;
    generated = 0;
    checkee = c;
}

// Destructor deletes an instance of the solver class.
solver::~solver ()
{
    node * next;
    for (node * eqn = equations; eqn != NULL; eqn = next)
    {
        next = eqn->getNext ();
        delete eqn;
    }
}

// The function finally evaluates each equation passed to the solver.
void solver::evaluate (void)
{
    foreach_equation (eqn)
    {
        // FIXME: Can save evaluation of already evaluated equations?
        if (eqn->evalPossible && !eqn->skip /* && eqn->evaluated == 0 */)
        {
            // exception handling around evaluation
            try_running ()
            {
                eqn->solvee = this;
                eqn->calculate ();
            }
            // handle evaluation exceptions
            catch_exception ()
            {
            default:
                estack.print ("evaluation");
                break;
            }
            eqn->evaluated++;
#if DEBUG && 0
            // print equation results
            logprint (LOG_STATUS, "%s = %s\n", A(eqn)->result,
                      eqn->getResult () ? eqn->getResult()->toString () : "error");
#if TESTING_DERIVATIVE || 0
            // print equation
            logprint (LOG_STATUS, "%s\n", eqn->toString ());
            // print derivations
            logprint (LOG_STATUS, "%s\n", eqn->differentiate("x")->toString ());
#endif
#endif
        }
    }
}

/* This function adds the given dataset vector to the set of equations
   stored in the equation solver. */
node * solver::addEquationData (qucs::vector * v, bool ref)
{
    constant * con = new constant (TAG_VECTOR);
    con->v = v;
    con->dataref = ref;
    assignment * assign = new assignment ();
    assign->result = strdup (v->getName ());
    assign->body = con;
    assign->setNext (equations);
    equations = assign;
    return assign;
}

/* The function puts the given vector into the equation set.  The
   resulting data vector is going to be copied and exported - given a
   generated name based upon the second argument. */
node * solver::addGeneratedEquation (qucs::vector * v, const char * n)
{
    // create generated name
    char * str = (char *) malloc (strlen (n) + 6);
    sprintf (str, "%s.%04d", n, ++generated);
    // copy data vector
    qucs::vector * c = new qucs::vector (*v);
    c->setName (str);
    // put vector into the equation set and ensure data export as
    // independent variable
    node * res = addEquationData (c);
    res->setInstance ("#generated");
    res->setDependencies (new strlist ());
    res->evalType ();
    res->solvee = this;
    res->evaluate ();
    res->output = 1;
    free (str);
    return res;
}

/* Depending on the type of equation result the function converts the
   given equation node to one or more valid dataset vector(s). */
qucs::vector * solver::dataVector (node * eqn)
{
    qucs::vector * v = NULL;
    if (!eqn->getResult ()) return NULL;
    switch (eqn->getType ())
    {
    case TAG_VECTOR: // simple vector
        v = new qucs::vector (* (eqn->getResult()->v));
        v->setNext (NULL);
        v->setPrev (NULL);
        break;
    case TAG_DOUBLE: // double value
        v = new qucs::vector ();
        v->add (eqn->getResult()->d);
        break;
    case TAG_BOOLEAN: // boolean value
        v = new qucs::vector ();
        v->add (eqn->getResult()->b ? 1 : 0);
        break;
    case TAG_COMPLEX: // complex value
        v = new qucs::vector ();
        v->add (* (eqn->getResult()->c));
        break;
    case TAG_MATVEC: // matrix vector
    {
        // convert matrix vector to a list of vectors
        matvec * mv = eqn->getResult()->mv;
        mv->setName (A(eqn)->result);
        for (int r = 0; r < mv->getRows (); r++)
        {
            for (int c = 0; c < mv->getCols (); c++)
            {
                // name gets automatically assigned
                qucs::vector * t = new qucs::vector (mv->get (r, c));
                // chain the vectors appropriately
                t->setNext (v);
                v = t;
            }
        }
    }
    return v;
    case TAG_MATRIX: // single matrix
    {
        // convert matrix to a list of vectors
        matrix * m = eqn->getResult()->m;
        for (int r = 0; r < m->getRows (); r++)
        {
            for (int c = 0; c < m->getCols (); c++)
            {
                qucs::vector * t = new qucs::vector ();
                t->setName (matvec::createMatrixString (A(eqn)->result, r, c));
                t->add (m->get (r, c));
                // chain the vectors appropriately
                t->setNext (v);
                v = t;
            }
        }
    }
    return v;
    default:
        return NULL;
    }
    v->setName (A(eqn)->result);
    return v;
}

/* This function collects the data vectors in a dataset and appends
   these to the list of equation node inside the equation solver. */
void solver::checkinDataset (void)
{
    if (data == NULL) return;
    qucs::vector * v;
    findMatrixVectors (data->getDependencies ());
    findMatrixVectors (data->getVariables ());
    for (v = data->getDependencies (); v != NULL; v = (qucs::vector *) v->getNext ())
    {
        if (v->getRequested () != -1)
        {
            node * eqn = addEquationData (v, true);
            strlist * deps = new strlist ();
            deps->add (v->getName ());
            eqn->setDataDependencies (deps);
            delete deps;
        }
    }
    for (v = data->getVariables (); v != NULL; v = (qucs::vector *) v->getNext ())
    {
        if (v->getRequested () != -1)
        {
            node * eqn = addEquationData (v, true);
            eqn->setDataDependencies (v->getDependencies ());
        }
    }
}

/* This function searches through the dataset of the equation solver
   for possible matrix vectors.  These are detected by the vectors'
   names (e.g. S[1,1]).  The matrix vectors found in the dataset get
   converted and saved into the set of equations.  */
void solver::findMatrixVectors (qucs::vector * v)
{
    qucs::vector * vec;
    strlist * deps;
    char * p, * cand;
    int s, r, c, a, b, n = 1;

    // initialize the  'found' flag
    for (vec = v; vec != NULL; vec = (qucs::vector *) vec->getNext ())
        vec->setRequested (0);

    // loop through the dataset vector until no more matrix vector is found
    do
    {
        r = c = s = -1;
        cand = NULL;
        deps = NULL;
        // go through the dataset
        for (vec = v; vec != NULL; vec = (qucs::vector *) vec->getNext ())
        {
            // skip detected vectors
            if (vec->getRequested ()) continue;
            // is the vector a possible matrix vector element ?
            if ((p = matvec::isMatrixVector (vec->getName (), a, b)) != NULL)
            {
                if (cand != NULL)
                {
                    // does this vectors name equals the current one ?
                    if (!strcmp (p, cand) && s == vec->getSize ())
                    {
                        // save largest row and column index and set the 'found' flag
                        if (a > r) r = a;
                        if (b > c) c = b;
                        vec->setRequested (n);
                    }
                }
                else
                {
                    /* new possible matrix vector:
                       save its name, row and column index, its size (length of
                       the vector) and data dependencies; then set the 'found' flag */
                    cand = strdup (p);
                    r = a;
                    c = b;
                    s = vec->getSize ();
                    vec->setRequested (n);
                    deps = vec->getDependencies ();
                }
                free (p);
            }
        }

        // new matrix vector detected
        if (cand != NULL)
        {
            // create a new matrix vector and set the appropriate name
            matvec * mv = new matvec (s, r + 1, c + 1);
            mv->setName (cand);
            // go through the dataset vector once again
            for (vec = v; vec != NULL; vec = (qucs::vector *) vec->getNext ())
            {
                // and collect the vectors with the same 'found' flags
                if (vec->getRequested () == n)
                {
                    p = matvec::isMatrixVector (vec->getName (), a, b);
                    mv->set (*vec, a, b);
                    free (p);
                    vec->setRequested (-1);
                }
            }
            // now store this new matrix vector into the set of equations
            node * eqn = addEquationData (mv);
            eqn->solvee = this;
            eqn->evaluate ();
            if (deps == NULL)
            {
                strlist * deps = new strlist ();
                deps->add (mv->getName ());
                eqn->setDataDependencies (deps);
                delete deps;
            }
            else
            {
                eqn->setDataDependencies (deps);
            }
            free (cand);
            cand = NULL;
        }
        // increase the current 'found' flag
        n++;
    }
    while (cand != NULL);
}

/* The function creates an assignment equation from the given matrix
   vector and returns it.  The new assignment is appended to the list
   of available equations. */
node * solver::addEquationData (matvec * mv)
{
    constant * con = new constant (TAG_MATVEC);
    con->mv = mv;
    assignment * assign = new assignment ();
    assign->result = strdup (mv->getName ());
    assign->body = con;
    assign->setNext (equations);
    equations = assign;
    return assign;
}

/* The function returns the dataset entry length of the given equation
   node (a constant).  The constant must already been evaluated when
   this function is called. */
int solver::dataSize (constant * eqn)
{
    int size = 0;
    switch (eqn->getType ())
    {
    case TAG_VECTOR: // simple vector
        size = eqn->getResult()->v->getSize ();
        break;
    case TAG_MATVEC: // matrix vector
        size = eqn->getResult()->mv->getSize ();
    default:
        size = 1;
    }
    return size;
}

/* This function returns the dataset entry length of the given
   variable name.  It must be ensured that the variable actually
   exists and is already evaluated. */
int solver::getDataSize (char * var)
{
    node * eqn = checker::findEquation (equations, var);
    return dataSize (C (eqn));
}

/* Depending on the index the function returns the cumulative product
   of the dataset entries stored in the given dependency list or one
   if there are no data dependencies at all. */
int solver::getDependencySize (strlist * deps, int idx)
{
    int size = 1;
    if (deps == NULL) return 1;
    for (int i = 0; i < deps->length () - idx; i++)
    {
        size *= getDataSize (deps->get (i));
    }
    return size;
}

/* This function goes through the given string list and calculates the
   data entries within these dataset dependencies.  It returns at
   least one no matter whether the data vectors can be found or not. */
int solver::dataSize (strlist * deps)
{
    int size = 1;
    for (int i = 0; deps != NULL && i < deps->length (); i++)
    {
        char * str = deps->get (i);
        qucs::vector * dep = data->findDependency (str);
        qucs::vector * var = data->findVariable (str);
        size *= dep ? dep->getSize () : var ? var->getSize () : 1;
    }
    return size;
}

/* The function returns the data vector in the dataset according to
   the given variable name.  If there is no such variable, it returns
   NULL. */
qucs::vector * solver::getDataVector (char * str)
{
    qucs::vector * var;
    /* search for variables in dataset */
    if (data != NULL)
    {
        if ((var = data->findVariable (str)) != NULL)
            return var;
        if ((var = data->findDependency (str)) != NULL)
            return var;
    }
    /* search for variables in equation set */
    if (equations != NULL)
    {
        node * eqn = checker::findEquation (equations, str);
        constant * res = eqn->getResult ();
        if (res->getTag () == CONSTANT && res->getType () == TAG_VECTOR)
        {
            return res->v;
        }
    }
    return NULL;
}

/* The following function collects the inherited dataset dependencies
   for the given equation node and returns it as a string list.  It
   returns NULL if there are no such dependencies. */
strlist * solver::collectDataDependencies (node * eqn)
{
    strlist * sub = NULL, * datadeps = NULL;
    // should all data dependencies be dropped?
    if (!eqn->getResult()->dropdeps)
    {
        strlist * deps = eqn->getDependencies ();
        datadeps = eqn->getDataDependencies ();
        datadeps = datadeps ? new strlist (*datadeps) : NULL;
        // go through equation dependencies
        for (int i = 0; deps && i < deps->length (); i++)
        {
            char * var = deps->get (i);
            // find equation node for the dependency
            node * n = checker::findEquation (equations, var);
            // try again in the solver equations
            if (n == NULL && eqn->solvee != NULL)
                n = checker::findEquation (eqn->solvee->getEquations (), var);
            // if finally founf the equation node
            if (n != NULL)
            {
                // pass resulting data dependencies up
                strlist * resdeps;
                if ((resdeps = n->getResult()->getDataDependencies ()) != NULL)
                    n->setDataDependencies (resdeps);
                // add data dependencies
                sub = strlist::join (datadeps, n->getDataDependencies ());
                sub->del (n->getResult()->getDropDependencies ());
                sub->add (n->getResult()->getPrepDependencies ());
            }
            delete datadeps;
            datadeps = sub;
        }
    }
    // prepend dependencies if necessary
    strlist * preps = eqn->getResult()->getPrepDependencies ();
    if (datadeps)
    {
        if (preps) datadeps->add (preps);
    }
    else
    {
        datadeps = new strlist ();
        if (preps) datadeps->add (preps);
    }
    // drop duplicate entries
    datadeps = checker::foldDependencies (datadeps);
    // delete the dependencies to be dropped intentionally
    datadeps->del (eqn->getResult()->getDropDependencies ());
    // finally return the correct data dependencies
    if (datadeps->length () == 0)
    {
        delete datadeps;
        datadeps = NULL;
    }
    return datadeps;
}

// The function stores the equation solver results back into a dataset.
void solver::checkoutDataset (void)
{
    // return if nothing todo
    if (data == NULL) return;
    // go through each equation
    foreach_equation (eqn)
    {

        // skip variables which don't need to be exported
        if (!eqn->output) continue;

        // is the equation result already in the dataset ?
        if (!findEquationResult (eqn))
        {
            qucs::vector * v = dataVector (eqn);
            if (v == NULL) continue;

            // collect inherited dataset dependencies
            strlist * datadeps = collectDataDependencies (eqn);

            // check whether dataset is smaller than its dependencies
            if (v->getSize () <= 1 && dataSize (datadeps) > v->getSize ())
            {
                delete datadeps;
                datadeps = NULL;
            }

            // store variable vector(s)
            if (datadeps && datadeps->length () > 0)
            {
                v->setDependencies (datadeps);
                if (v->getNext () != NULL)
                {
                    data->applyDependencies (v);
                    data->addVariables (v);
                }
                else
                {
                    data->addVariable (v);
                }
            }
            // store independent vector(s)
            else
            {
                if (v->getNext () != NULL)
                    data->addDependencies (v);
                else
                    data->addDependency (v);
                delete datadeps;
            }
        }
    }
}

/* This function checks whether the given equation solver result is
   already within the dataset.  It returns non-zero if so, otherwise
   the function returns zero. */
int solver::findEquationResult (node * eqn)
{
    // check each vector of a given matrix vector
    if (eqn->getType () == TAG_MATVEC)
    {
        matvec * mv = eqn->getResult()->mv;
        for (int r = 0; r < mv->getRows (); r++)
        {
            for (int c = 0; c < mv->getCols (); c++)
            {
                char * str = matvec::createMatrixString (A(eqn)->result, r, c);
                if (data->findDependency (str) || data->findVariable (str))
                    return 1;
            }
        }
    }
    // check normal data vectors
    else
    {
        char * str = A(eqn)->result;
        if (data->findDependency (str) || data->findVariable (str))
            return 1;
    }
    return 0;
}

/* This function is called in order to run the equation checker and
   the solver.  The optional dataset passed to the function receives
   the results of the calculations. */
int solver::solve (dataset * data)
{
    // load additional dataset equations
    setData (data);
    checkinDataset ();
    // put these into the checker
    checkee->setEquations (equations);
    // and check
    if (checkee->check (data ? 1 : 0) != 0)
    {
        return -1;
    }
    equations = checkee->getEquations ();
    // finally evaluate equations
    evaluate ();
    // put results into the dataset
    checkoutDataset ();
    return 0;
}

/* Go through the list of equations and store the left hand side in
   a string list. */
strlist * checker::variables (void)
{
    strlist * idents = new strlist ();
    foreach_equation (eqn)
    {
        idents->add (eqn->result);
    }
    return idents;
}

// Checks if the given variable name is an equation.
bool checker::containsVariable (const char * const ident) const
{
    foreach_equation (eqn)
    {
        if (!strcmp (ident, eqn->result))
            return true;
    }
    return false;
}

// Structure defining a predefined constant.
struct pconstant
{
    const char * ident;
    nr_double_t value;
};

// List of global constant variables.
static struct pconstant pconstants[] =
{
    { "pi", pi },
    { "e",  euler  },
    { "kB", kB   },
    { "q",  Q_e    },
    { NULL, 0    }
};

/* The function should be called before parsing the netlist.  It
   appends the predefined constants to the list of equations. */
void checker::constants (void)
{

    // return if nothing to do
    if (consts) return;

    // go through constants and add these to the equations
    for (int i = 0; pconstants[i].ident != NULL; i++)
    {
        addDouble ("#predefined", pconstants[i].ident, pconstants[i].value);
    }

    // indicate that constants have been added
    consts = true;
}

/* The function adds a new equation to the equation checker consisting
   of an assignment of a reference. */
node * checker::addReference (const char * type, const char * ident,
                              char * value)
{
    node * eqn = createReference (type, ident, value);
    addEquation (eqn);
    return eqn;
}

/* The function adds a new equation to the equation checker consisting
   of an assignment of a double variable. */
node * checker::addDouble (const char * type, const char * ident,
                           nr_double_t value)
{
    node * eqn = createDouble (type, ident, value);
    addEquation (eqn);
    return eqn;
}

/* The function adds a new equation to the equation checker consisting
   of an assignment of a complex variable. */
node * checker::addComplex (const char * type, const char * ident,
                            nr_complex_t value)
{
    node * eqn = createComplex (type, ident, value);
    addEquation (eqn);
    return eqn;
}

// Adds given equation to the equation list.
void checker::addEquation (node * eqn)
{
    eqn->setNext (equations);
    equations = eqn;
}

// Appends the given equation to the equation list.
void checker::appendEquation (node * eqn)
{
    eqn->setNext (NULL);
    node * last = lastEquation (equations);
    if (last != NULL)
        last->setNext (eqn);
    else
        equations = eqn;
}

/* This function creates a equation consisting of an assignment of a
   double variable. */
node * checker::createDouble (const char * type, const char * ident,
                              nr_double_t value)
{
    // create constant double value
    constant * c = new constant (TAG_DOUBLE);
    c->checkee = this;
    c->d = value;
    // create the appropriate assignment
    assignment * a = new assignment ();
    a->checkee = this;
    a->result = strdup (ident);
    a->body = c;
    a->output = 0;
    a->setInstance (type);
    return a;
}

/* This function creates a equation consisting of an assignment of a
   complex variable. */
node * checker::createComplex (const char * type, const char * ident,
                               nr_complex_t value)
{
    // create constant double value
    constant * c = new constant (TAG_COMPLEX);
    c->checkee = this;
    c->c = new nr_complex_t (value);
    // create the appropriate assignment
    assignment * a = new assignment ();
    a->checkee = this;
    a->result = strdup (ident);
    a->body = c;
    a->output = 0;
    a->setInstance (type);
    return a;
}

/* This function creates a equation consisting of an assignment of a
   reference. */
node * checker::createReference (const char * type, const char * ident,
                                 char * value)
{
    // create reference value
    reference * r = new reference ();
    r->checkee = this;
    r->n = strdup (value);
    // create the appropriate assignment
    assignment * a = new assignment ();
    a->checkee = this;
    a->result = strdup (ident);
    a->body = r;
    a->output = 0;
    a->setInstance (type);
    return a;
}

/* The functions looks through the set of equations for a real valued
   result and returns it.  If there is no such assignment, zero is
   returned. */
nr_double_t checker::getDouble (const char * const ident) const
{
    foreach_equation (eqn)
    {
        if (!strcmp (ident, eqn->result))
        {
            return eqn->getResultDouble ();
        }
    }
    return 0.0;
}

/* The function goes through the equation set and looks for the
   specified assignment.  If found the given value is set. */
void checker::setDouble (const char * const ident, nr_double_t val)
{
    foreach_equation (eqn)
    {
        if (!strcmp (ident, eqn->result))
        {
            if (eqn->body->getTag () == CONSTANT)
            {
                constant * c = C (eqn->body);
                if (c->type == TAG_DOUBLE) c->d = val;
            }
        }
    }
}

/* The functions looks through the set of equations for a vector
   result and returns it.  If there is no such assignment, an empty
   vector is returned. */
qucs::vector checker::getVector (const char * const ident) const
{
    foreach_equation (eqn)
    {
        if (!strcmp (ident, eqn->result))
        {
            return eqn->getResultVector ();
        }
    }
    return qucs::vector ();
}

} // namespace qucs
