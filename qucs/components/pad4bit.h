/*
 * pad4bit.h - device definitions for pad4bit module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef pad4bit_H
#define pad4bit_H

#include "component.h"

class pad4bit : public Component
{
  public:
    pad4bit();
    ~pad4bit() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
    QString vhdlCode(int);
    QString verilogCode(int);
};

#endif /* pad4bit_H */
