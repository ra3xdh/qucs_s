/*
 * comp_2bit.h - device definitions for comp_2bit module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef comp_2bit_H
#define comp_2bit_H

#include "component.h"

class comp_2bit : public Component
{
  public:
    comp_2bit();
    ~comp_2bit() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
    QString vhdlCode(int);
    QString verilogCode(int);
};

#endif /* comp_2bit_H */
