/*
 * substrate.h - microstrip substrate class definitions
 *
 * Copyright (C) 2004m 2008 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __SUBSTRATE_H__
#define __SUBSTRATE_H__

#include "object.h"

namespace qucs {

class substrate : public qucs::object
{
 public:
  MCREATOR (substrate);
  substrate (const substrate &);
  ~substrate ();
};

} // namespace qucs

#endif /* __SUBSTRATE_H__ */
