/***************************************************************************
                         ecvs.h  -  description
                         -----------------------
    copyright            : (C) 2013 by Richard Crozier
    email                : richard dot crozier at yahoo dot co dot uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/** \file ecvs.h
  * \brief The externally controlled voltage source component header file.
  *
  * Contains the m-code interface class definition.
  */

/**
  * \ingroup QucsInterface
  */

#ifndef ECVS_H
#define ECVS_H

#include "component.h"


class ecvs : public Component  {
public:
  ecvs();
  ~ecvs();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);

protected:
  QString netlist();
};

#endif

