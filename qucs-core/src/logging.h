/*
 * logging.h - logging facility class definitions
 *
 * Copyright (C) 2003, 2005 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __LOGGING_H__
#define __LOGGING_H__

#define LOG_ERROR  0
#define LOG_STATUS 1
#include <stdio.h>
__BEGIN_DECLS

void logprint (int, const char *, ...);
void loginit (void);
void redirect_status_to_stdout();
void logprogressbar (nr_double_t, nr_double_t, int);
void logprogressclear (int);
extern int progressbar_enable;
extern int progressbar_gui;

__END_DECLS

#endif /* __LOGGING_H__ */
