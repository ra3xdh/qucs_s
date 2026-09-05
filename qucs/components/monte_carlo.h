/***************************************************************************
                               monte_carlo.h
                              ---------------
    begin                : September 2026
    copyright            : (C) 2026 Qucs-S team
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MONTE_CARLO_H
#define MONTE_CARLO_H

#include "simulation.h"

/*!
 * \brief ngspice Monte Carlo simulation component (.MC).
 *
 * A .MC component wraps one existing .AC or .SP analysis in a Nutmeg
 * "dowhile" loop that re-runs it \c Runs times. Each iteration issues a
 * \c reset before the analysis so that native ngspice random expressions
 * (unif(), gauss(), ...) written directly into device values are
 * re-evaluated for every run. The loop is emitted through the
 * getNgspiceBeforeSim() / getNgspiceAfterSim() hooks; the component
 * contributes nothing to the netlist body itself.
 */
class MonteCarlo : public qucs::component::SimulationComponent {
public:
  MonteCarlo();
  ~MonteCarlo();
  Component* newOne();
  static Element* info(QString&, char*&, bool getNewOne = false);

  QString getNgspiceBeforeSim(QString sim, int lvl = 0) override;
  QString getNgspiceAfterSim(QString sim, int lvl = 0) override;

protected:
  QString netlist() override;

private:
  //! Parsed \c Runs value; non-numeric, empty or <= 0 is clamped to 1.
  int effectiveRuns();
  //! Nutmeg-safe loop counter name, unique per component instance.
  QString counterVarName();
};

#endif
