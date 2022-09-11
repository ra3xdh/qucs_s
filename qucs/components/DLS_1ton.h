/*
 * DLS_1ton.h - device definitions for DLS_1ton module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef DLS_1ton_H
#define DLS_1ton_H

#include "component.h"

class DLS_1ton : public Component
{
  public:
    DLS_1ton();
    ~DLS_1ton() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
};

#endif /* DLS_1ton_H */
