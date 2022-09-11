/*
 * mod_amp.h - device definitions for mod_amp module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef mod_amp_H
#define mod_amp_H

#include "component.h"

class mod_amp : public Component
{
  public:
    mod_amp();
    ~mod_amp() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
};

#endif /* mod_amp_H */
