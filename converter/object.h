/*
 * object.h - generic object class definitions
 *
 * Copyright (C) 2003, 2004, 2005, 2006, 2008 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: object.h 1871 2013-03-06 12:53:05Z crobarcro $
 *
 */

 /*! \file object.h
  *
  * Contains the definition of the generic object class.
  *
  */

#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <string>
#include "property.h"

#define MCREATOR(val) \
  val (); \
  static struct define_t miscdef; \
  static struct define_t * definition (void) { return &miscdef; }

namespace qucs {

class variable;
class vector;
class property;

/*! \class object
 * \brief generic object class.
 *
 * Generic object class from which many Qucs classes are
 * inheirited.
 *
 */
class object
{
 public:
  //! Constructor creates an unnamed instance of the object class.
  object () : name(), props() {} ;
  //! This constructor creates a named instance of the object class.
  object (const std::string &n) : name(n), props() {} ;
  //! Sets the name of the object.
  void setName (const std::string &n) { this->name = n; };
  //! Get the name of the object.
  const char * getName (void) const { return this->name.c_str(); };
  void addProperty (const std::string &n, const char * const val, const bool def = false);
  void addProperty (const std::string &n, const nr_double_t, const bool def = false);
  void addProperty (const std::string &n, variable * const, const bool def = false);
  void setProperty (const std::string &n, const char * const);
  void setProperty (const std::string &n, nr_double_t);
  void setScaledProperty (const std::string &n, const nr_double_t);
  void setProperty (const std::string &n, variable * const);
  vector * getPropertyVector (const std::string &n) const;
  const char * getPropertyString (const std::string &n) const;
  const char * getPropertyReference (const std::string &n) const;
  nr_double_t getPropertyDouble (const std::string &n) const;
  nr_double_t getScaledProperty (const std::string &n) const;
  int  getPropertyInteger (const std::string &n) const;
  bool hasProperty (const std::string &n) const ;
  bool isPropertyGiven (const std::string &n) const;
  int  countProperties (void) const;
  const char *
    propertyList (void) const;

 private:
  std::string name;
  properties props;
};

} // namespace qucs

#endif /* __OBJECT_H__ */
