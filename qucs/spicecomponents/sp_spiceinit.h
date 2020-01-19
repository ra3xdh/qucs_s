/***************************************************************************
                                 sp_spiceinit.h
                                ----------------
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SP_SPICEINIT_H
#define SP_SPICEINIT_H

#include "components/component.h"


class SpiceSpiceinit : public Component  {
public:
  SpiceSpiceinit();
  ~SpiceSpiceinit();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);

  QString getSpiceinit();
};

#endif
