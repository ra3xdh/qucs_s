/*
 * HBT_X.h - device definitions for HBT_X module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef HBT_X_H
#define HBT_X_H

#include "component.h"

class HBT_X : public Component
{
  public:
    HBT_X();
   ~HBT_X() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif /* HBT_X_H */
