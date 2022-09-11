/*
 * andor4x2.h - device definitions for andor4x2 module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef andor4x2_H
#define andor4x2_H

#include "component.h"

class andor4x2 : public Component
{
  public:
    andor4x2();
    ~andor4x2() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
    QString vhdlCode(int);
    QString verilogCode(int);
};

#endif /* andor4x2_H */
