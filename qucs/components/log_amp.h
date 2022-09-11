/*
 * log_amp.h - device definitions for log_amp module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef log_amp_H
#define log_amp_H

#include "component.h"

class log_amp : public Component
{
  public:
    log_amp();
    ~log_amp() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
};

#endif /* log_amp_H */
