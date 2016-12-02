/*
 * bsim3v34nMOS.h - device definitions for bsim3v34nMOS module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef bsim3v34nMOS_H
#define bsim3v34nMOS_H

#include "component.h"

class bsim3v34nMOS : public Component
{
  public:
    bsim3v34nMOS();
    ~bsim3v34nMOS() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
};

#endif /* bsim3v34nMOS_H */
