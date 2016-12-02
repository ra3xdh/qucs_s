/*
 * hic2_full.h - device definitions for hic2_full module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef hic2_full_H
#define hic2_full_H

#include "component.h"

class hic2_full : public Component
{
  public:
    hic2_full();
    ~hic2_full() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
};

#endif /* hic2_full_H */
