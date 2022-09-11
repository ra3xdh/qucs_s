/*
 * hic0_full.h - device definitions for hic0_full module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef hic0_full_H
#define hic0_full_H

#include "component.h"

class hic0_full : public MultiViewComponent
{
  public:
    hic0_full();
    ~hic0_full() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
    static Element* info_pnp(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
    QString netlist();
};

#endif /* hic0_full_H */
