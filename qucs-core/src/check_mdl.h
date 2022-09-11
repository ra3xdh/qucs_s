/*
 * check_mdl.h - checker definitions for an IC-CAP MDL file
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

#ifndef __CHECK_MDL_H__
#define __CHECK_MDL_H__

namespace qucs {
  class dataset;
}

/* Externalize variables used by the scanner and parser. */
extern int mdl_lineno;
extern FILE * mdl_in;
void mdl_restart (FILE *);

__BEGIN_DECLS

/* Available functions of the checker. */
int  mdl_check (void);
int  mdl_parse (void);
int  mdl_error (const char *);
int  mdl_lex (void);
int  mdl_lex_destroy (void);
void mdl_destroy (void);
void mdl_init (void);

__END_DECLS

/* Declaration of MDL data structures. */
extern qucs::dataset * mdl_result;
extern struct mdl_link_t * mdl_root;
extern struct mdl_sync_t * mdl_sync_root;

struct mdl_point_t {
  int n;
  int x;
  int y;
  double r;
  double i;
  struct mdl_point_t * next;
};

struct mdl_datasize_t {
  char * type;
  int size;
  int x;
  int y;
};

struct mdl_element_t {
  int number;
  char * name;
  char * value;
  char * attr;
  struct mdl_element_t * next;
};

struct mdl_hyptable_t {
  char * name;
  struct mdl_element_t * data;
};

struct mdl_table_t {
  char * name;
  struct mdl_element_t * data;
};

struct mdl_dataset_t {
  char * type1;
  struct mdl_point_t * data1;
  char * type2;
  struct mdl_point_t * data2;
  struct mdl_datasize_t * dsize;
};

struct mdl_link_t {
  struct mdl_link_t * parent;
  char * name;
  char * type;
  struct mdl_lcontent_t * content;
  struct mdl_link_t * next;
};

struct mdl_lcontent_t {
  int type;
  union {
    struct mdl_link_t * link;
    struct mdl_data_t * data;
    struct mdl_table_t * table;
  };
  struct mdl_lcontent_t * next;
};

struct mdl_data_t {
  struct mdl_dcontent_t * content;
};

struct mdl_dcontent_t {
  int type;
  union {
    struct mdl_dataset_t * data;
    struct mdl_hyptable_t * hyptable;
  };
  struct mdl_dcontent_t * next;
};

struct mdl_sync_t {
  char * master;
  char * name;
  double ratio;
  double offset;
  struct mdl_sync_t * next;
};

#endif /* __CHECK_MDL_H__ */
