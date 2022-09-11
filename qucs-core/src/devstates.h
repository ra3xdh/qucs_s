/*
 * devstates.h - device state class definitions
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

#ifndef __DEVSTATES_H__
#define __DEVSTATES_H__

namespace qucs {

class devstates
{
 public:
  // constructor and destructor set
  devstates ();
  devstates (int, int);
  ~devstates ();

  // public functions operating on states and variables
  int  deviceState (void);
  void deviceState (int);
  int  deviceStates (void);
  void deviceStates (int, int);
  nr_double_t  operator () (int) const;
  nr_double_t& operator () (int);
  nr_double_t  deviceVar (int) const;
  nr_double_t& deviceVar (int);

 private:
  int nstates;
  int nvars;
  int nstate;
  nr_double_t * states;
  nr_double_t * pstate;
};

} // namespace qucs

#endif /* __DEVSTATES_H__ */
