/***************************************************************************
                          NPN_SPICE.h  -  description
                      --------------------------------------
    begin                     : Fri Mar 9 2007
    copyright                 : (C) 2007 by Gunther Kraut
    email                     : gn.kraut@t-online.de
    spice4qucs code added  Thurs. 28 May 2015
    copyright                 : (C) 2015 by Mike Brinson
    email                     : mbrin72043@yahoo.co.uk

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 #ifndef BJT_SPICE_H
#define BJT_SPICE_H

#include "components/component.h"

class BJT_SPICE : public MultiViewComponent {
public:
  BJT_SPICE();
  ~BJT_SPICE();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  static Element* infoNPN4(QString&, char* &, bool getNewOne=false);
  static Element* infoPNP4(QString&, char* &, bool getNewOne=false);
  static Element* infoNPN5(QString&, char* &, bool getNewOne=false);
  static Element* infoPNP5(QString&, char* &, bool getNewOne=false);
protected:
  void createSymbol();
  QString netlist();
  QString spice_netlist(bool isXyce = false);
};

#endif // NPN_SPICE_H
