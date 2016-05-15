/***************************************************************************
                         eNL.h  -  description
                   --------------------------------------
    begin                    : Fri Mar 27 2015
    copyright              : (C) by Mike Brinson (mbrin72043@yahoo.co.uk),
														Vadim Kuznetsov (ra3xdh@gmail.com)

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 #ifndef ENL_H
#define ENL_H

#include "components/component.h"

class eNL : public Component {
public:
  eNL();
  ~eNL();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
protected:
  QString netlist();
  QString spice_netlist(bool isXyce = false);
};

#endif // ENL_H
