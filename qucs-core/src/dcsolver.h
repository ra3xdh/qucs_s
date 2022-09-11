/*
 * dcsolver.h - DC solver class definitions
 *
 * Copyright (C) 2003-2008 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __DCSOLVER_H__
#define __DCSOLVER_H__

#include "nasolver.h"

namespace qucs {

class dcsolver : public nasolver<nr_double_t>
{
 public:
  ACREATOR (dcsolver);
  dcsolver (char *);
  dcsolver (dcsolver &);
  ~dcsolver ();
  int  solve (void);
  static void calc (dcsolver *);
  void init (void);
  void restart (void);
  void saveOperatingPoints (void);

 private:
  int saveOPs;
};

} // namespace qucs

#endif /* __DCSOLVER_H__ */

