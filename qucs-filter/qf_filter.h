/***************************************************************************
                               filter.h
                             ----------------
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

#ifndef _QF_FILTER_H
#define _QF_FILTER_H

// Header for filter
#include "qf_poly.h"
#include "qf_subsection.h"
#include <QString>
#include <vector>

namespace qf {
enum filter_type { LOWPASS, HIGHPASS, BANDPASS, BANDSTOP };

typedef enum filter_type qft;

enum filter_kind { BUTT, CHEB, ICHEB, BESS, CAUER, UNDEF };

typedef enum filter_kind qfk;

enum ctype { CAP, IND, RES };

typedef enum ctype qfct;

struct comp {
  qfct comp;
  qf_float val;
};

typedef struct comp qfc;

// Generic filter class

class filter {
protected:
  const qft type_; // Lowpass, highpass...
  const qfk kind_; // Butterworth, Chebichev...
  unsigned ord_;   // Order of filter
  bool is_tee_;    // Tee or Pi

  const qf_float fc_;  // Cutoff for lp or hp / Center for bp and bs
  qf_float bw_;        // Bandwidth
  const qf_float imp_; // Terminating impedance
  std::vector<subsection> proto_subsecs_;
  std::vector<subsection> subsecs_;

  // Polynomial description

  poly E_; // H(w) = E(w)/P(w)
  poly F_; // D(w) = F(w)/P(w)
  poly P_;

  poly BN_; // B(w) susceptance of filter
  poly BD_; // B(w) susceptance of filter

  unsigned n_comp_; // Number of components

public:
  filter(qfk, qft, qf_float, qf_float, qf_float, bool);
  virtual ~filter(); // Exit

  // Common routines to perform extraction of poles and zeros
  // This one extracts a finite pole of transmission
  void extract_pole_pCsLC(qf_float, qf_float);
  int order() { return ord_; }
  virtual void synth() = 0; // Synthesize filter
  QString to_qucs();

private:
  QString num2str(qf_float);
};
} // namespace qf
#endif // _QF_FILTER_H
