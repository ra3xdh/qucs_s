/*
 * phototransistor.h - device definitions for phototransistor module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef phototransistor_H
#define phototransistor_H

#include "component.h"

class phototransistor : public Component
{
  public:
    phototransistor();
    ~phototransistor() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
};

#endif /* phototransistor_H */
