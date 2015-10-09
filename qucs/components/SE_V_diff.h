/***************************************************************************
                         SE_V_diff.h  -  description
                   --------------------------------------
    begin                  : Fri Mar 9 2007
    copyright              : (C) 2007 by Gunther Kraut
    email                  : gn.kraut@t-online.de
    spice4qucs code added  :  Wed. 7 Oct 2015
    copyright              : (C) 2015 by Mike Brinson
    email                  : mbrin72043@yahoo.co.uk

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef SE_V_diff_H
#define SE_V_diff_H

#include "component.h"

class SE_V_diff: public Component {
public:
  SE_V_diff();
  ~SE_V_diff();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
protected:
  QString netlist();
  QString spice_netlist(bool isXyce = false);
};

#endif // SE_V_diff
