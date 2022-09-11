/*
 * ha1b.h - device definitions for ha1b module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef ha1b_H
#define ha1b_H

#include "component.h"

class ha1b : public Component
{
  public:
    ha1b();
    ~ha1b() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
    QString vhdlCode(int);
    QString verilogCode(int);
};

#endif /* ha1b_H */
