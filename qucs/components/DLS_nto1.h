/*
 * DLS_nto1.h - device definitions for DLS_nto1 module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef DLS_nto1_H
#define DLS_nto1_H

#include "component.h"

class DLS_nto1 : public Component
{
  public:
    DLS_nto1();
    ~DLS_nto1() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
};

#endif /* DLS_nto1_H */
