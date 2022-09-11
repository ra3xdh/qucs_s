/*
 * dmux2to4.h - device definitions for dmux2to4 module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef dmux2to4_H
#define dmux2to4_H

#include "component.h"

class dmux2to4 : public Component
{
  public:
    dmux2to4();
    ~dmux2to4() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
    QString vhdlCode(int);
    QString verilogCode(int);
};

#endif /* dmux2to4_H */
