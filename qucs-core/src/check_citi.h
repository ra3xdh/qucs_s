/*
 * check_city.h - checker definitions for CITIfiles
 *
 * Copyright (C) 2006 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __CHECK_CITI_H__
#define __CHECK_CITI_H__

namespace qucs {
  class dataset;
  class vector;
}

extern qucs::dataset * citi_result;
extern struct citi_package_t * citi_root;

struct citi_header_t {
  char * package;
  char * var;
  char * type;
  int i1, i2;
  int n;
  struct citi_header_t * next;
};

struct citi_package_t {
  struct citi_header_t * head;
  qucs::vector * data;
  struct citi_package_t * next;
};

/* Externalize variables used by the scanner and parser. */
extern int citi_lineno;
extern FILE * citi_in;
void citi_restart (FILE *);

__BEGIN_DECLS

/* Externalize variables used by the scanner and parser. */
extern struct citi_t citi_options;

/* Available functions of the checker. */
int citi_parse (void);
int citi_error (const char *);
int citi_lex (void);
int citi_lex_destroy (void);
int citi_check (void);
void citi_init (void);
void citi_destroy (void);

__END_DECLS

#endif /* __CHECK_CITI_H__ */
