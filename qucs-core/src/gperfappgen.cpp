/*
 * gperfappgen.cpp - generates the gperf input file
 *
 * Copyright (C) 2005 Stefan Jahn <stefan@lkcc.org>
 * Copyright (C) 2015 Guilherme Brondani Torri <guitorri@gmail.com>
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

#include "evaluate.h"
#include "equation.h"

using namespace qucs::eqn;

#include "gperfappgen.h"

/* The program is used to generate the input file for a gperf run. */
int main (void) {

  struct application_t * app;
  char * key;
  int len, n = 0;

  // Print declarations.
  fprintf (stdout,
	   "%%language=C++\n"
	   "%%define class-name gperfapphash\n"
	   "%%define lookup-function-name get\n"
	   "%%define slot-name key\n"
	   "%%define word-array-name applist\n"
	   "%%struct-type\n"
	   "struct appindex { const char * key; int index; };\n"
	   "%%%%\n");

  // Print hash key list.
  for (app = applications; app->application != NULL; app++, n++) {
    key = (char *) calloc (1, strlen (app->application) + app->nargs * 3 + 5);
    strcat (key, app->application);
    for (int i = 0; i < app->nargs; i++) {
      strcat (key, "_");
      strcat (key, checker::tag2key (app->args[i]));
    }
    len = strlen (key);
    fprintf (stdout, "\"%s\", %s%s%s%d\n", key,
	     len < 12 ? "\t" : "", len < 20 ? "\t" : "",
	     len < 28 ? "" : "", n);
    free (key);
  }

  return 0;
}
