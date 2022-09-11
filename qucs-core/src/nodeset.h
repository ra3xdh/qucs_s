/*
 * nodeset.h - node set class definitions
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

#ifndef __NODESET_H__
#define __NODESET_H__

namespace qucs {

class nodeset
{
 public:
  MCREATOR (nodeset);
  nodeset (char *);
  nodeset (char *, nr_double_t);
  nodeset (const nodeset &);
  virtual ~nodeset ();
  nodeset * getNext (void) { return next; }
  void setNext (nodeset * p) { next = p; }
  void setName (char *);
  char * getName (void);
  nr_double_t getValue (void) { return value; }
  void setValue (nr_double_t val) { value = val; }
  nodeset * findNodeset (char *);

 private:
  char * name;
  nr_double_t value;
  nodeset * next;
};

} // namespace qucs

#endif /* __NODESET_H__ */
