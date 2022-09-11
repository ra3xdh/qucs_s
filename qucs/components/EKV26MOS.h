/*
 * EKV26MOS.h - device definitions for EKV26MOS module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef EKV26MOS_H
#define EKV26MOS_H

#include "component.h"

class EKV26MOS : public MultiViewComponent
{
  public:
    EKV26MOS();
    ~EKV26MOS() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
    static Element* info_pmos(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
    QString netlist();
};

#endif /* EKV26MOS_H */
