/*
 * object.cpp - generic object class implementation
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
 * $Id: object.cpp 1870 2013-03-06 12:52:07Z crobarcro $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <utility>

#include "logging.h"
#include "complex.h"
#include "property.h"
#include "object.h"
#include "variable.h"

namespace qucs {

/* This function adds a property consisting of a key and a string
   value to the object. */
void object::addProperty (const std::string &n, const char * const val, const bool def) {
  property p;
  p.set(val);
  p.setDefault(def);
  props.insert({{n,p}});
}

/* This function sets the specified property consisting of a key and a
   string value in the object. */
void object::setProperty (const std::string &n, const char * const val) {
    auto it = props.find(n);
    if(it != props.end())
      (*it).second.set(val);
    else
      addProperty (n, val);
}

/* This function adds a property consisting of a key and a double
   value to the object. */
void object::addProperty (const std::string &n, const nr_double_t val, const bool def) {
  property p;
  p.set(val);
  p.setDefault(def);
  props.insert({{n,p}});
}

/* This function sets the specified property consisting of a key and a
   double value in the object. */
void object::setProperty (const std::string &n, const nr_double_t val) {
  auto it = props.find(n);
  if(it != props.end())
    (*it).second.set(val);
  else
    addProperty (n, val);
}

/* Th function sets the specified property consisting of a key and a
   double value in the object.  The property is marked a scalability
   property. */
void object::setScaledProperty (const std::string &n, const nr_double_t val) {
  setProperty ("Scaled:"+n, val);
}

/* This function adds a property consisting of a key and a variable
   value to the object. */
void object::addProperty (const std::string &n, variable * const val, const bool def) {
  property p;
  p.set(val);
  p.setDefault(def);
  props.insert({{n,p}});
}

/* Returns the requested property value which has been previously
   added as its vector representation.  If there is no such property
   the function returns NULL. */
qucs::vector * object::getPropertyVector (const std::string &n) const {
  const auto &it = props.find(n);
  if(it != props.end())
    return (*it).second.getVector();
  else
    return NULL;
}

/* Returns the requested property value which has been previously
   added as its text representation.  If there is no such property the
   function returns NULL. */
const char * object::getPropertyString (const std::string &n) const {
  const auto &it = props.find(n);
  if(it != props.end())
    return (*it).second.getString();
  else
    return NULL;
}

/* Returns the requested property reference variable name.  If there
   is no such property the function returns NULL. */
const char * object::getPropertyReference (const std::string &n) const {
  const auto &it = props.find(n);
  if(it != props.end())
    return (*it).second.getReference();
  else
    return NULL;
}

/* Returns the requested property value which has been previously
   added as its double representation.  If there is no such property
   the function returns zero. */
nr_double_t object::getPropertyDouble (const std::string &n) const {
  const auto &it = props.find(n);
  if(it != props.end())
    return (*it).second.getDouble();
  else
    return 0.0;
}

/* The functions returns the requested (scalability) property value
   which has been previously added.  If there is no such scaled
   property the function returns the standard property or zero. */
nr_double_t object::getScaledProperty (const std::string &n) const{
  std::string txt = "Scaled:"+n;
  const auto &it = props.find(txt);
  if(it != props.end())
    return (*it).second.getDouble();
  else
    return this->getPropertyDouble(n);
}

/* Returns the requested property value which has been previously
   added as its integer representation.  If there is no such property
   the function returns zero. */
int object::getPropertyInteger (const std::string &n) const {
  const auto &it = props.find(n);
  if(it != props.end())
    return (*it).second.getInteger();
  else
    return 0;
}

/* The function checks whether the object has got a certain property
   value.  If so it returns non-zero, otherwise it returns zero. */
bool object::hasProperty (const std::string &n) const {
  return props.find(n) !=  props.end();
}

/* The function checks whether the object has got a certain property
   value and if this has its default value.  If so it returns  non-zero,
   otherwise it returns zero. */
bool object::isPropertyGiven (const std::string &n) const {
  const auto &it = props.find(n);
  if(it != props.end())
    return !(*it).second.isDefault();
  else
    return false;
}

// The function returns the number of properties in the object.
int object::countProperties (void) const {
  return props.size();
}

// This function returns a text representation of the objects properties.
const char * object::propertyList (void) const {
  std::string ptxt;
  for(auto it = props.cbegin(); it != props.cend(); ++it) {      
    std::string n = it->first;    
    std::string val = it->second.toString ();
    std::string text = n+"=\""+val+"\"";
    ptxt += text;
  }
  return ptxt.c_str();
}

} // namespace qucs
