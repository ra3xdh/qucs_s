/*
 * nigbt.h - device definitions for nigbt module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef nigbt_H
#define nigbt_H

#include "component.h"

class nigbt : public Component
{
  public:
    nigbt();
    ~nigbt() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
};

#endif /* nigbt_H */
