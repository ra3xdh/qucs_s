/*
 * MESFET.h - device definitions for MESFET module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef MESFET_H
#define MESFET_H

#include "component.h"

class MESFET : public Component
{
  public:
    MESFET();
    ~MESFET() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
};

#endif /* MESFET_H */
