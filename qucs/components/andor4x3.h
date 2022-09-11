/*
 * andor4x3.h - device definitions for andor4x3 module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef andor4x3_H
#define andor4x3_H

#include "component.h"

class andor4x3 : public Component
{
  public:
    andor4x3();
    ~andor4x3() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
    QString vhdlCode(int);
    QString verilogCode(int);
};

#endif /* andor4x3_H */
