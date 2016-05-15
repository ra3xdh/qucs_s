/***************************************************************************
                         gNL.h  -  description
                   --------------------------------------
    begin                    : Tue Mar 31 2015
    copyright              : (C) by Mike Brinson (mbrin72043@yahoo.co.uk),
														Vadim Kuznetsov (ra3xdh@gmail.com)

 ***************************************************************************/

/***************************************************************************
 *                                                                                                                     *
 *   This program is free software; you can redistribute it and/or modify      *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or            *
 *   (at your option) any later version.                                                                            *
 *                                                                                                                     *
 ***************************************************************************/
 #ifndef GNL_H
#define GNL_H

#include "components/component.h"

class gNL : public Component {
public:
  gNL();
  ~gNL();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
protected:
  QString netlist();
  QString spice_netlist(bool isXyce = false);
};

#endif // GNL_H
