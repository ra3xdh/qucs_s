/***************************************************************************
                          LTL_SPICE.h  -  description
                      --------------------------------------

    spice4qucs code        : Tue. 19 May 2015
    copyright              : (C) 2015 by Mike Brinson
    email                    : mbrin72043@yahoo.co.uk

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 #ifndef LTRA_SPICE_H
#define LTRA_SPICE_H

#include "components/component.h"

class LTRA_SPICE : public Component {
public:
  LTRA_SPICE();
  ~LTRA_SPICE();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
protected:
  QString netlist();
  QString spice_netlist(bool isXyce = false);
};

#endif // LTRA_SPICE_H
