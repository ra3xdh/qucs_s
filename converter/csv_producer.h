/*
 * csv_producer.h - the CSV producer definitions
 *
 * Copyright (C) 2006, 2007 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __CSV_PRODUCER_H__
#define __CSV_PRODUCER_H__

#include "logging.h"
#include "strlist.h"
#include "object.h"
#include "complex.h"
#include "vector.h"
#include "dataset.h"

/* Externalize variables. */
extern qucs::dataset * qucs_data;
extern FILE * csv_out;

/* Available functions of the producers. */
void csv_producer (char * variable, const char * sep = ";");

#endif /* __CSV_PRODUCER_H__ */
