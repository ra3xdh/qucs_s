/*
 * tff_SR.h - device definitions for tff_SR module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef tff_SR_H
#define tff_SR_H

#include "component.h"

class tff_SR : public Component
{
  public:
    tff_SR();
    ~tff_SR() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
    QString vhdlCode(int);
    QString verilogCode(int);
    QString spice_netlist(bool isXyce);
};

#endif /* tff_SR_H */
