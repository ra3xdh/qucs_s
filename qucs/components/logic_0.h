/*
 * logic_0.h - device definitions for logic_0 module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef logic_0_H
#define logic_0_H

#include "component.h"

class logic_0 : public Component
{
  public:
    logic_0();
    ~logic_0() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
    QString vhdlCode(int);
    QString verilogCode(int);
};

#endif /* logic_0_H */
