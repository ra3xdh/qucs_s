/*
 * logic_1.h - device definitions for logic_1 module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef logic_1_H
#define logic_1_H

#include "component.h"

class logic_1 : public Component
{
  public:
    logic_1();
    ~logic_1() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
    QString vhdlCode(int);
    QString verilogCode(int);
};

#endif /* logic_1_H */
