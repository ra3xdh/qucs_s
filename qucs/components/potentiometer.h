/*
 * potentiometer.h - device definitions for potentiometer module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef potentiometer_H
#define potentiometer_H

#include "component.h"

class potentiometer : public Component
{
  public:
    potentiometer();
    ~potentiometer() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
    QString spice_netlist(bool isXyce);
};

#endif /* potentiometer_H */
