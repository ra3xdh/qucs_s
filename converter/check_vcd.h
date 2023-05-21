/*
 * check_vcd.h - checker definitions for a vcd file
 *
 * Copyright (C) 2005 Raimund Jacob <raimi@lkcc.org>
 * Copyright (C) 2005, 2006, 2008 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __CHECK_VCD_H__
#define __CHECK_VCD_H__

/* Externalize variables used by the scanner and parser. */
extern int vcd_lineno;
extern FILE * vcd_in;

/* Useful defines. */
#define VCD_NOSCOPE "noscope"
#define VCD_FAST 1

__BEGIN_DECLS

/* Externalize variables used by the scanner, parser, checker and producer. */
extern struct vcd_file * vcd;
extern struct dataset_variable * dataset_root;
extern int vcd_correct;

/* Available functions of the checker. */
int  vcd_checker (void);
int  vcd_parse (void);
int  vcd_error (const char *);
int  vcd_lex (void);
int  vcd_lex_destroy (void);
void vcd_destroy (void);
void vcd_init (void);

__END_DECLS

/* Declaration of VCD data structures. */

// Types of VCD variables.
enum vcd_vartypes {
  VAR_EVENT,
  VAR_INTEGER,
  VAR_PARAMETER,
  VAR_REAL,
  VAR_REG,
  VAR_SUPPLY0,
  VAR_SUPPLY1,
  VAR_TIME,
  VAR_TRI,
  VAR_TRIAND,
  VAR_TRIOR,
  VAR_TRIREG,
  VAR_TRI0,
  VAR_TRI1,
  VAR_WAND,
  VAR_WIRE,
  VAR_WOR
};

// Range definition.
struct vcd_range {
  int l;
  int h;
};

// Variable definition.
struct vcd_vardef {
  enum vcd_vartypes type;   // type
  int size;                 // size in bits
  char * code;              // identifier code, used to reference 'ident'
  char * ident;             // variable identifier
  struct vcd_range * range; // range, e.g. [0] or [3:15]
  struct vcd_scope * scope; // scope of variable
  struct vcd_vardef * next;
};

// Types of VCD scopes.
enum vcd_scopes {
  SCOPE_MODULE,
  SCOPE_TASK,
  SCOPE_FUNCTION,
  SCOPE_BEGIN,
  SCOPE_FORK
};

// Scope definition.
struct vcd_scope {
  enum vcd_scopes type;        // type
  char * ident;                // scope identifier
  struct vcd_vardef * vardefs; // variable definitions
  struct vcd_scope * scopes;   // sub-scopes
  struct vcd_scope * parent;   // parent scope
  struct vcd_scope * next;
};

// Value change definition.
struct vcd_change {
  char * value;            // the value
  char * code;             // identifier code, references 'var'
  int isreal;              // indicates type of value
  struct vcd_vardef * var; // pointer to variable
  struct vcd_change * next;
};

// A full set of VCD value changes.
struct vcd_changeset {
  double t;                    // time stamp
  struct vcd_change * changes; // list of VCD changes 
#ifndef VCD_FAST
  int done;                    // flag for the checker
#endif
  struct vcd_changeset * next;
};

// Representation of a VCD file.
struct vcd_file {
  int t;                             // time scale (1, 10 or 100)
  double scale;                      // time unit factor
  struct vcd_scope * scopes;         // scopes
  struct vcd_scope * currentscope;   // the current scope
  struct vcd_changeset * changesets; // change sets
};

/* Checker specific data structures. */

// A VCD variable.
struct vcd_variable {
  char * code;  // identifier code, references 'var'
  char * ident; // variable identifer
  char * value; // the value
  int isreal;   // indicates type of value
  int type;     // variable type
  struct vcd_variable * next;
};

// A VCD change set.
struct vcd_set {
  double t;                        // time stamp
  struct vcd_variable * variables; // list of VCD variables
  struct vcd_set * next;
};

/* Qucs dataset specific data structures. */

// Dataset value structure.
struct dataset_value {
  char * value; // the value
  struct dataset_value * next;
};

// Types of dataset variables.
enum dataset_vartypes {
  DATA_UNKNOWN,
  DATA_DEPENDENT,
  DATA_INDEPENDENT
};

// The dataset vector representation.
struct dataset_variable {
  enum dataset_vartypes type;    // type of variable
  int output;                    // should it be exported?
  int size;                      // length of the vector
  char * ident;                  // variable identifier
  char * dependencies;           // variable dependencies (if dependent)
  int isreal;                    // indicates type of values
  struct dataset_value * values; // list of values
  struct dataset_variable * next;
};

#endif /* __CHECK_VCD_H__ */
