#ifndef SRC_EQNDEF_H
#define SRC_EQNDEF_H

#include "components/component.h"

class Src_eqndef : public Component {
public:
  Src_eqndef();
  ~Src_eqndef();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
protected:
  QString netlist();
  QString spice_netlist(spicecompat::SpiceDialect dialect = spicecompat::SPICEDefault);
  QString va_code();
};

#endif // SRC_EQNDEF_H
