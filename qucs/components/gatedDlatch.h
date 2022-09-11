/*
 * gatedDlatch.h - device definitions for gatedDlatch module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef gatedDlatch_H
#define gatedDlatch_H

#include "component.h"

class gatedDlatch : public Component
{
  public:
    gatedDlatch();
    ~gatedDlatch() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
    QString vhdlCode(int);
    QString verilogCode(int);
};

#endif /* gatedDlatch_H */
