/*
 * bsim4v30nMOS.h - device definitions for bsim4v30nMOS module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef bsim4v30nMOS_H
#define bsim4v30nMOS_H

#include "component.h"

class bsim4v30nMOS : public Component
{
  public:
    bsim4v30nMOS();
    ~bsim4v30nMOS() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
};

#endif /* bsim4v30nMOS_H */
