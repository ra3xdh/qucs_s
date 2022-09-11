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

const qf_double_t SN_ACC = 1e-5;	// Accuracy of sn(x) is SN_ACC^2
const qf_double_t K_ERR1 = 1e-8;	// Accuracy of K(k)
const qf_double_t K_ERR2 = 1e-20;	// Accuracy of K(k)
const qf_double_t K_ERR3 = 1e-6;	// Accuracy of K(k)
const qf_double_t K_ERR =  4e-16;                // Accuracy of K (k)
const qf_double_t SK_ERR = sqrt (K_ERR);         // Accuracy of sn (u, k)

class qf_cauer : public qf_filter
{
 private:

  // Standard parameters
  qf_double_t rho;			// Reflection coeff.
  qf_double_t th;			// Modular angle

  // Zeros of transmission
  qf_double_t * a;

public:
  qf_cauer (unsigned, qf_double_t, qf_double_t);
  qf_cauer (qf_double_t, qf_double_t, qf_double_t, qf_double_t, qf_double_t,
	    qf_double_t, qft);
  virtual ~qf_cauer (void);

  // Elliptic functions
  static qf_double_t K (qf_double_t);
  static qf_double_t Kp (qf_double_t);
  static qf_double_t sn (qf_double_t, qf_double_t);
  static qf_double_t ellip_RF (qf_double_t, qf_double_t, qf_double_t);
  static qf_double_t ellip_K (qf_double_t);
  static qf_double_t ellip_sncndn (qf_double_t, qf_double_t,
				   qf_double_t&, qf_double_t&, qf_double_t&);
  static qf_double_t ellip_sn (qf_double_t, qf_double_t);

  // Computes standard form
  void normalize (qf_double_t, qf_double_t, qf_double_t, qft);

  void xfer (void);		// Computes xfer fctn
  void values (void);		// Computes norm values
  virtual void synth (qft);	// Standard -> Actual form
  void dump (void);		// Dumps to cout
};

#endif // _QF_CAUER_H
