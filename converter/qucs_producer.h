/*
 * qucs_producer.h - the Qucs producer definitions
 *
 * Copyright (C) 2004, 2005, 2006, 2007 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __QUCS_PRODUCER_H__
#define __QUCS_PRODUCER_H__
namespace qucs {
  class dataset;
}

/* Externalize variables. */
extern FILE * qucs_out;
extern int qucs_actions;
extern const char * qucs_gnd;

/* Available functions of the producers. */
void qucs_producer (void);
int  qucs_find_node (struct node_t *, char *);
void qucslib_producer (void);
void qucsdata_producer_vcd (void);
void qucsdata_producer (qucs::dataset *);

#endif /* __QUCS_PRODUCER_H__ */
