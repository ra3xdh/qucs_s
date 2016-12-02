/*
 * hicumL0V1p2g.h - device definitions for hicumL0V1p2g module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef hicumL0V1p2g_H
#define hicumL0V1p2g_H

#include "component.h"

class hicumL0V1p2g : public MultiViewComponent
{
  public:
    hicumL0V1p2g();
    ~hicumL0V1p2g() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
    static Element* info_pnp(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
    QString netlist();
};

#endif /* hicumL0V1p2g_H */
