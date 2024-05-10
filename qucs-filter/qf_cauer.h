/***************************************************************************
                               qf_cauer.h
                             --------------
    begin                : Mon Jan 02 2006
    copyright            : (C) 2006 by Vincent Habchi, F5RCS
    email                : 10.50@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _QF_CAUER_H
#define _QF_CAUER_H

#include "qf_filter.h"

namespace qf {
class cauer : public filter {
private:
  // Standard parameters
  qf_float rho_; // Reflection coeff.
  qf_float th_;  // Modular angle

  // Zeros of transmission
  qf_float* a_;

public:
  cauer(qf_float, qf_float, qf_float, qf_float, qf_float, qf_float, qft,
        bool is_tee);
  virtual ~cauer(void);

  // Computes standard form
  void normalize(qf_float, qf_float, qf_float);

  void xfer(void);      // Computes xfer fctn
  void values(void);    // Computes norm values
  virtual void synth(); // Standard -> Actual form
};
} // namespace qf

#endif // _QF_CAUER_H
