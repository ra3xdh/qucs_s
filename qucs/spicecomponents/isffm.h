/***************************************************************************
                         isffm.h  -  description                                                             *
                   --------------------------------------                                                       *
     spice4qucs code added  Sun. 23 March 2015                                                          *
    copyright              : (C) 2015 by Mike Brinson                                                *
    email                    : mbrin72043@yahoo.co.uk                                              *
****************************************************************************/

/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify       *
 *   it under the terms of the GNU General Public License as published by    *
 *   the Free Software Foundation; either version 2 of the License, or             *
 *   (at your option) any later version.                                                                            *
 ***************************************************************************/
 #ifndef ISFFM_H
#define ISFFM_H

#include "components/component.h"

class iSffm : public Component {
public:
  iSffm();
  ~iSffm();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
protected:
  QString netlist();
  QString spice_netlist(bool isXyce = false);
};

#endif // ISFFM_H
