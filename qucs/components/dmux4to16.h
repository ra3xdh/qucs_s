/*
 * dmux4to16.h - device definitions for dmux4to16 module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef dmux4to16_H
#define dmux4to16_H

#include "component.h"

class dmux4to16 : public Component
{
  public:
    dmux4to16();
    ~dmux4to16() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
    QString vhdlCode(int);
    QString verilogCode(int);
};

#endif /* dmux4to16_H */
