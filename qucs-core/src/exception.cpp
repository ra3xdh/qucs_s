/*
 * exception.cpp - exception class implementation
 *
 * Copyright (C) 2004 Stefan Jahn <stefan@lkcc.org>
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
#include <stdarg.h>

#include "exception.h"

using namespace qucs;

// Constructor creates an instance of the exception class.
exception::exception () {
  code = EXCEPTION_UNKNOWN;
  txt = NULL;
  data = 0;
}

// Constructor creates an typified instance of the exception class.
exception::exception (int type) {
  code = type;
  txt = NULL;
  data = 0;
}

/* This copy constructor creates a instance of the exception class based
   on the given exception. */
exception::exception (const exception & e) {
  txt = e.txt ? strdup (e.txt) : NULL;
  code = e.code;
  data = e.data;
}

// Destructor deletes an instance of the exception class.
exception::~exception () {
  free (txt);
}

/* This function save the given messages format and the appropriate
   arguments to the internal text property of the exception object. */
void exception::setText (const char * format, ...) {
  char * str;
  va_list args;

  free (txt);
  str = (char *) malloc (1024); // this should be enough
  va_start (args, format);
  vsprintf (str, format, args);
  va_end (args);

  // copy string to text buffer
  txt = strdup (str); free (str);
}
