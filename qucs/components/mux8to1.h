/*
 * mux8to1.h - device definitions for mux8to1 module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef mux8to1_H
#define mux8to1_H

#include "component.h"

class mux8to1 : public Component
{
  public:
    mux8to1();
    ~mux8to1() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
    QString vhdlCode(int);
    QString verilogCode(int);
};

#endif /* mux8to1_H */
