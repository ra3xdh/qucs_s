/*
 * jkff_SR.h - device definitions for jkff_SR module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef jkff_SR_H
#define jkff_SR_H

#include "component.h"

class jkff_SR : public Component
{
  public:
    jkff_SR();
    ~jkff_SR() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
    QString vhdlCode(int);
    QString verilogCode(int);
};

#endif /* jkff_SR_H */
