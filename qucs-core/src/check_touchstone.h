/*
 * check_touchstone.h - checker definitions for Touchstone files
 *
 * Copyright (C) 2003, 2004, 2005, 2006 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __CHECK_TOUCHSTONE_H__
#define __CHECK_TOUCHSTONE_H__

/* Touchstone (R) File Format Specification Rev 1.1

   A Touchstone (R) file (also known as an SnP file) is an ASCII text file
   used for documenting the n-port network parameter data of an active
   device or passive interconnect network.  While Touchstone file have
   been accepted as a de-facto standard for the transfer of frequency
   dependent n-port network data, up till now there has been no formal
   documentation of the file format or syntax.  This document, base upon
   information from Agilent Corporation (the originator of Touchstone), is
   a formal specification of the Touchstone file format, intended for use
   with documents and specifications produced by the EIA/IBIS Open Forum. */

// forward declarations
namespace qucs {
  class dataset;
  class vector;
  class strlist;
}

extern qucs::dataset * touchstone_result;
extern qucs::vector  * touchstone_line;
extern qucs::vector  * touchstone_vector;
extern qucs::strlist * touchstone_idents;

struct touchstone_t {
  const char * unit;
  char parameter;
  const char * format;
  double resistance;
  double factor;
  int ports;
  int noise;
  int lines;
};

/* Externalize variables used by the scanner and parser. */
extern int touchstone_lineno;
extern FILE * touchstone_in;
void touchstone_restart (FILE *);

__BEGIN_DECLS

/* Externalize variables used by the scanner and parser. */
extern struct touchstone_t touchstone_options;

/* Available functions of the checker. */
int touchstone_parse (void);
int touchstone_error (const char *);
int touchstone_lex (void);
int touchstone_lex_destroy (void);
int touchstone_check (void);
void touchstone_init (void);
void touchstone_destroy (void);

__END_DECLS

#endif /* __CHECK_TOUCHSTONE_H__ */
