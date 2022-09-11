/*
 * check_netlist.h - checker definitions for the Qucs netlist
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

#ifndef __CHECK_NETLIST_H__
#define __CHECK_NETLIST_H__

#include "netdefs.h"

/* Forward declarations. */
namespace qucs {
  class environment;
}

/* Externalize variables used by the scanner and parser. */
extern int netlist_lineno;
extern FILE * netlist_in;

__BEGIN_DECLS

/* Externalize variables used by the scanner and parser. */
extern struct definition_t * definition_root;

/* Available functions of the checker. */
void netlist_status (void);
void netlist_list (void);
void netlist_destroy (void);
void netlist_destroy_env (void);
int  netlist_checker (qucs::environment *);
int  netlist_parse (void);
int  netlist_error (const char *);
int  netlist_lex (void);
int  netlist_lex_destroy (void);
int  netlist_checker_variables (qucs::environment *);

/* Some more functionality. */
struct definition_t *
netlist_unchain_definition (struct definition_t *, struct definition_t *);

__END_DECLS

#endif /* __CHECK_NETLIST_H__ */
