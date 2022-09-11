/*
 * device.h - device class definitions
 *
 * Copyright (C) 2004, 2005, 2006 Stefan Jahn <stefan@lkcc.org>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * $Id$
 *
 */

#ifndef __DEVICE_H__
#define __DEVICE_H__

namespace qucs {

class circuit;
class node;
class net;

namespace device {

  // creates external resistor circuit
  circuit *
    splitResistor (
      circuit * base, // calling circuit (this)
      circuit * res,  // additional resistor circuit (can be NULL)
      const char * c, // name of the additional circuit
      const char * n, // name of the inserted (internal) node
      int internal);  // number of new node (the original external node)

  // removes external resistor circuit
  void
    disableResistor (
      circuit * base, // calling circuit (this)
      circuit * res,  // additional resistor circuit
      int internal);  // number of new node (the original external node)

  // creates external capacitor circuit
  circuit *
    splitCapacitor (
      circuit * base, // calling circuit (this)
      circuit * cap,  // additional capacitor circuit (can be NULL)
      const char * c, // name of the additional circuit
      node * n1,      // first node of new capacitor
      node * n2);     // second node of new capacitor

  // removes external capacitor circuit
  void
    disableCapacitor (
      circuit * base, // calling circuit (this)
      circuit * cap); // additional capacitor circuit

  // checks whether circuit is enabled
  int
    deviceEnabled (
      circuit * c); // circuit to be checked

  // computes current and its derivative for a MOS pn-junction
  void
    pnJunctionMOS (
      nr_double_t Upn, // pn-voltage
      nr_double_t Iss, // saturation current
      nr_double_t Ute, // temperature voltage
      nr_double_t& I,  // result current
      nr_double_t& g); // result derivative

  // computes current and its derivative for a bipolar pn-junction
  void
    pnJunctionBIP (
      nr_double_t Upn, // pn-voltage
      nr_double_t Iss, // saturation current
      nr_double_t Ute, // temperature voltage
      nr_double_t& I,  // result current
      nr_double_t& g); // result derivative

  // limits the forward pn-voltage
  nr_double_t
     pnVoltage (
       nr_double_t Ud,     // current pn-voltage
       nr_double_t Uold,   // previous pn-voltage
       nr_double_t Ut,     // temperature voltage
       nr_double_t Ucrit); // critical voltage

  // computes the exponential pn-junction current
  nr_double_t
    pnCurrent (
      nr_double_t Upn,  // pn-voltage
      nr_double_t Iss,  // saturation current
      nr_double_t Ute); // temperature voltage

  // computes the exponential pn-junction current's derivative
  nr_double_t
    pnConductance (
      nr_double_t Upn,  // pn-voltage
      nr_double_t Iss,  // saturation current
      nr_double_t Ute); // temperature voltage

  // computes pn-junction depletion capacitance
  nr_double_t
    pnCapacitance (
      nr_double_t Uj,  // pn-voltage
      nr_double_t Cj,  // zero-bias capacitance
      nr_double_t Vj,  // built-in potential
      nr_double_t Mj,  // grading coefficient
      nr_double_t Fc); // forward-bias coefficient

  // computes pn-junction depletion charge
  nr_double_t
    pnCharge (
      nr_double_t Uj,  // pn-voltage
      nr_double_t Cj,  // zero-bias capacitance
      nr_double_t Vj,  // built-in potential
      nr_double_t Mj,  // grading coefficient
      nr_double_t Fc); // forward-bias coefficient

  // computes pn-junction depletion capacitance
  nr_double_t
    pnCapacitance (
      nr_double_t Uj,  // pn-voltage
      nr_double_t Cj,  // zero-bias capacitance
      nr_double_t Vj,  // built-in potential
      nr_double_t Mj); // grading coefficient

  // computes pn-junction depletion charge
  nr_double_t
    pnCharge (
      nr_double_t Uj,  // pn-voltage
      nr_double_t Cj,  // zero-bias capacitance
      nr_double_t Vj,  // built-in potential
      nr_double_t Mj); // grading coefficient

  // compute critical voltage of pn-junction
  nr_double_t
    pnCriticalVoltage (
      nr_double_t Iss,  // saturation current
      nr_double_t Ute); // temperature voltage

  // limits the forward fet-voltage
  nr_double_t
    fetVoltage (
      nr_double_t Ufet, // current fet voltage
      nr_double_t Uold, // previous fet voltage
      nr_double_t Uth); // threshold voltage

  // limits the drain-source voltage
  nr_double_t
    fetVoltageDS (
      nr_double_t Ufet,  // current fet voltage
      nr_double_t Uold); // previous fet voltage

  // calculates the overlap capacitance for mosfet (meyer model)
  void
    fetCapacitanceMeyer (
      nr_double_t Ugs,   // gate-source voltage
      nr_double_t Ugd,   // gate-drain voltage
      nr_double_t Uth,   // threshold voltage
      nr_double_t Udsat, // drain-source saturation voltage
      nr_double_t Phi,   // built-in potential
      nr_double_t Cox,   // oxide capacitance
      nr_double_t& Cgs,  // resulting gate-source capacitance
      nr_double_t& Cgd,  // resulting gate-drain capacitance
      nr_double_t& Cgb); // resulting gate-bulk capacitance

  // computes temperature dependency of energy bandgap
  nr_double_t
    Egap (
      nr_double_t T,            // temperature
      nr_double_t Eg0 = Eg0Si); // bandgap at 0K

  // computes temperature dependency of intrinsic density
  nr_double_t
    intrinsicDensity (
      nr_double_t T,            // temperature
      nr_double_t Eg0 = Eg0Si); // bandgap at 0K

  // calculates temperature dependence for saturation current
  nr_double_t
    pnCurrent_T (
      nr_double_t T1,       // reference temperature
      nr_double_t T2,       // temperature
      nr_double_t Is,       // saturation current
      nr_double_t Eg,       // bandgap at 300K
      nr_double_t N = 1,    // emission coefficient
      nr_double_t Xti = 0); // temperature coefficient

  // calculates temperature dependence for junction potential
  nr_double_t
    pnPotential_T (
      nr_double_t T1,           // reference temperature
      nr_double_t T2,           // temperature
      nr_double_t Vj,           // built-in potential
      nr_double_t Eg0 = Eg0Si); // bandgap at 0K

  // calculates temperature dependence for junction capacitance
  nr_double_t
    pnCapacitance_T (
      nr_double_t T1,  // reference temperature
      nr_double_t T2,  // temperature
      nr_double_t M,   // grading coefficient
      nr_double_t VR,  // built-in potential ratio
      nr_double_t Cj); // zero-bias capacitance

  // calculates temperature dependence for junction capacitance
  nr_double_t
    pnCapacitance_F (
      nr_double_t T1,  // reference temperature
      nr_double_t T2,  // temperature
      nr_double_t M,   // grading coefficient
      nr_double_t VR); // built-in potential ratio: Vj(T2) / Vj(T1)

} // namespace device

} // namespace qucs

#endif /* __DEVICE_H__ */
