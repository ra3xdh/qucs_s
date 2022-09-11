/*
 * states.cpp - save-state variable template class implementation
 *
 * Copyright (C) 2004 Stefan Jahn <stefan@lkcc.org>
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "states.h"

// Some definitions for the save-state variables.
#define STATE_SHIFT 3
#define STATE_NUM   8
#define STATE_MASK  7

namespace qucs {

// Constructor creates an unnamed instance of the states class.
template <class state_type_t>
states<state_type_t>::states ()
{
    nstates = 0;
    currentstate = 0;
    stateval = NULL;
}

/* The copy constructor creates a new instance based on the given
   states object. */
template <class state_type_t>
states<state_type_t>::states (const states & c)
{
    nstates = c.nstates;
    currentstate = c.currentstate;

    // copy state variables if necessary
    if (nstates && c.stateval)
    {
        int size = nstates * sizeof (state_type_t) * STATE_NUM;
        stateval = (state_type_t *) malloc (size);
        memcpy (stateval, c.stateval, size);
    }
    else stateval = NULL;
}

// Destructor deletes a states object.
template <class state_type_t>
states<state_type_t>::~states ()
{
    free (stateval);
}

/* The function allocates and initializes memory for the save-state
   variables. */
template <class state_type_t>
void states<state_type_t>::initStates (void)
{
    free (stateval);
    if (nstates)
    {
        stateval = (state_type_t *)
                   calloc (nstates, sizeof (state_type_t) * STATE_NUM);
    }
    currentstate = 0;
}

// Clears the save-state variables.
template <class state_type_t>
void states<state_type_t>::clearStates (void)
{
    if (nstates && stateval)
        memset (stateval, 0, nstates * sizeof (state_type_t) * STATE_NUM);
    currentstate = 0;
}

/* The function returns a save-state variable at the given position.
   Higher positions mean earlier states.  By default the function
   returns the current state of the save-state variable. */
template <class state_type_t>
state_type_t states<state_type_t>::getState (int state, int n)
{
    int i = (n + currentstate) & STATE_MASK;
    return stateval[(state << STATE_SHIFT) + i];
}

/* This function applies the given value to a save-state variable.
   Higher positions mean earlier states.  By default the function sets
   the current state of the save-state variable. */
template <class state_type_t>
void states<state_type_t>::setState (int state, state_type_t val, int n)
{
    int i = (n + currentstate) & STATE_MASK;
    stateval[(state << STATE_SHIFT) + i] = val;
}

// Shifts one state forward.
template <class state_type_t>
void states<state_type_t>::nextState (void)
{
    if (--currentstate < 0) currentstate = STATE_NUM - 1;
}

// Shifts one state backward.
template <class state_type_t>
void states<state_type_t>::prevState (void)
{
    currentstate = (currentstate + 1) & STATE_MASK;
}

/* This function applies the given value to a save-state variable through
   all history values. */
template <class state_type_t>
void states<state_type_t>::fillState (int state, state_type_t val)
{
    // get a pointer to the start of the state array
    state_type_t * p = &stateval[state << STATE_SHIFT];
    // fill each array member with the supplied value
    for (int i = 0; i < STATE_NUM; i++)
    {
        *p++ = val;
    }
}

/* This function stores the values of the given state into the given
   pointer location. */
template <class state_type_t>
void states<state_type_t>::saveState (int state, state_type_t * values)
{
    // loop through the list of states copying them into the
    // supplied input array
    for (int i = 0; i < STATE_NUM; i++)
    {
        values[i] = getState (state, i);
    }
}

/* This function stores the values in the given pointer location
   into the state. */
template <class state_type_t>
void states<state_type_t>::inputState (int state, state_type_t * values)
{
    // loop through the list of states copying them into the
    // supplied input array
    for (int i = 0; i < STATE_NUM; i++)
    {
        setState (state, values[i], i);
    }
}

} // namespace qucs
