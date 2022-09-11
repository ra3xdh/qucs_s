/*
 * hicumL2V2p1.h - device definitions for hicumL2V2p1 module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef hicumL2V2p1_H
#define hicumL2V2p1_H

#include "component.h"

class hicumL2V2p1 : public Component
{
  public:
    hicumL2V2p1();
   ~hicumL2V2p1() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
};

#endif /* hicumL2V2p1_H */
