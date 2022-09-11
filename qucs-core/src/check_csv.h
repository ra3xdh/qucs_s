/*
 * check_csv.h - checker definitions for CSV files
 *
 * Copyright (C) 2007 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __CHECK_CSV_H__
#define __CHECK_CSV_H__

namespace qucs {
  class dataset;
  class vector;
  class strlist;
}

extern qucs::dataset * csv_result;
extern qucs::vector  * csv_vector;
extern qucs::strlist * csv_header;

/* Externalize variables used by the scanner and parser. */
extern int csv_lineno;
extern FILE * csv_in;
void csv_restart (FILE *);

__BEGIN_DECLS

/* Available functions of the checker. */
int csv_parse (void);
int csv_error (const char *);
int csv_lex (void);
int csv_lex_destroy (void);
int csv_check (void);
void csv_init (void);
void csv_destroy (void);

__END_DECLS

#endif /* __CHECK_CSV_H__ */
