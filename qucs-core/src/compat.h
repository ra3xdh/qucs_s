/*
 * compat.h - compatibility header file
 *
 * Copyright (C) 2008 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __COMPAT_H__
#define __COMPAT_H__

#if HAVE_IEEEFP_H
# include <ieeefp.h>
#endif

#if !HAVE_STRCHR
# define strchr  index
# define strrchr rindex
#endif

#if defined(_WIN32) & not defined(__MINGW32__)
#define strcasecmp stricmp
#endif

#endif /* __COMPAT_H__ */
