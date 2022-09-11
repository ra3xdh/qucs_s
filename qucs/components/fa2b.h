/*
 * fa2b.h - device definitions for fa2b module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef fa2b_H
#define fa2b_H

#include "component.h"

class fa2b : public Component
{
  public:
    fa2b();
    ~fa2b() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
    QString vhdlCode(int);
    QString verilogCode(int);
};

#endif /* fa2b_H */
