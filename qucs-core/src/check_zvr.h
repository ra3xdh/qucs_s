/*
 * check_zvr.h - checker definitions for a zvr file
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

#ifndef __CHECK_ZVR_H__
#define __CHECK_ZVR_H__

// forward declarations
namespace qucs {
  class dataset;
  class vector;
}

/* Externalize variables used by the scanner and parser. */
extern int zvr_lineno;
extern FILE * zvr_in;
void zvr_restart (FILE *);

__BEGIN_DECLS

/* Available functions of the checker. */
int  zvr_check (void);
int  zvr_parse (void);
int  zvr_error (const char *);
int  zvr_lex (void);
int  zvr_lex_destroy (void);
void zvr_destroy (void);
void zvr_init (void);

__END_DECLS

/* Declaration of ZVR data structures. */
extern qucs::dataset * zvr_result;
extern struct zvr_data_t * zvr_root;

struct zvr_header_t {
  double zref;
  double start;
  double stop;
  char * funit;
  int points;
  char * d_TYP;
  char * d_UNT;
  char * d_FMT;
};

struct zvr_vector_t {
  char * n1;
  char * n2;
  char * nf;
  qucs::vector * vi;
  qucs::vector * vd;
};

struct zvr_line_t {
  double d;
  double r, i;
  struct zvr_line_t * next;
};

struct zvr_data_t {
  struct zvr_header_t * h;
  struct zvr_vector_t * v;
  struct zvr_line_t * d;
  struct zvr_data_t * next;
};


#endif /* __CHECK_ZVR_H__ */
