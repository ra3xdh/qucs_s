/***************************************************************************
                              monte_carlo.cpp
                             -----------------
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

#include "monte_carlo.h"

#include <QRegularExpression>

MonteCarlo::MonteCarlo() {
  Description = QObject::tr("Monte Carlo simulation");

  initSymbol(Description);
  Model        = ".MC";
  Name         = "MC";
  SpiceModel   = "*";
  isSimulation = true;
  Simulator    = spicecompat::simNgspice;

  Props.append(new Property(
      "Sim", "", true,
      QObject::tr("simulation to run the Monte Carlo analysis on")));
  Props.append(new Property("Runs", "10", true,
                            QObject::tr("number of Monte Carlo runs")));
  Props.append(new Property("Variable", "Var", true,
                            QObject::tr("Variable name defined in .PARAM block to apply variation")));
  Props.append(new Property("Value", "10.0", true,
                            QObject::tr("Starting value")));
  Props.append(new Property("Relvar", "0.1", true,
                            QObject::tr("Variable relative variance")));
  Props.append(new Property("NSigma", "3", true,
                            QObject::tr("Sigmas number for Gauss distribution")));
  Props.append(new Property("Function", "gauss", true,
                            QObject::tr("Distribution function [unif,gauss]")));
}

MonteCarlo::~MonteCarlo() {}

Component* MonteCarlo::newOne() {
  return new MonteCarlo();
}

Element* MonteCarlo::info(QString& Name, char*& BitmapFile, bool getNewOne) {
  Name       = QObject::tr("Monte Carlo simulation");
  BitmapFile = (char*)"monte_carlo";

  if (getNewOne) {
    return new MonteCarlo();
  }
  return 0;
}

int MonteCarlo::effectiveRuns() {
  Property* runsProp = getProperty("Runs");
  if (runsProp == nullptr) {
    return 1;
  }

  bool ok        = false;
  const int runs = runsProp->Value.trimmed().toInt(&ok);
  if (!ok || runs <= 0) {
    return 1;
  }
  return runs;
}

QString MonteCarlo::counterVarName() {
  QString counter = Name.toLower();
  counter.remove(QRegularExpression("[^a-z0-9_]"));
  return QStringLiteral("mc_%1").arg(counter);
}

QString MonteCarlo::getNgspiceBeforeSim(QString sim, int lvl) {
  Q_UNUSED(sim)
  Q_UNUSED(lvl)

  if (isActive != COMP_IS_ACTIVE) {
    return QString();
  }

  auto var = getProperty("Variable")->Value;
  auto val = getProperty("Value")->Value;
  auto relvar = getProperty("Relvar")->Value;
  auto sigma = getProperty("NSigma")->Value;
  auto func = getProperty("Function")->Value;
  const QString counter = counterVarName();

  QString s =  QString("let %1 = 0\n"
                        "dowhile %1 < %2\n")
      .arg(counter)
      .arg(effectiveRuns());
  s += QString("alterparam %1 = %2(%3,%4").arg(var).arg(func).arg(val).arg(relvar);
  if (func == "gauss") {
    s += QString(",%1)\n").arg(sigma);
  } else {
    s += ")\n";
  }
  s += "reset\n";
  return s;
}

QString MonteCarlo::getNgspiceAfterSim(QString sim, int lvl) {
  Q_UNUSED(sim)
  Q_UNUSED(lvl)

  if (isActive != COMP_IS_ACTIVE) {
    return QString();
  }

  const QString counter = counterVarName();
  return QStringLiteral("set appendwrite\n"
                        "let %1 = %1 + 1\n"
                        "end\n"
                        "unset appendwrite\n")
      .arg(counter);
}

QString MonteCarlo::netlist() {
  // ngspice-only component: nothing goes into the netlist body. The Monte
  // Carlo loop is produced by getNgspiceBeforeSim() / getNgspiceAfterSim().
  return QString();
}
