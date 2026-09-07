/*
 * substratelib.h - minimal reader for Qucs substrate library files
 *
 * Copyright (C) 2026 Qucs-S team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef SUBSTRATELIB_H
#define SUBSTRATELIB_H

#include <QList>
#include <QString>

/* One substrate material as read from a Qucs library file.  The six values
   come from the positional <SUBST ...> statement in the order used by the
   Qucs "SUBST" component: er, h, t, tand, rho, D. */
struct SubstrateMaterial {
  QString name;      // component display name (may contain spaces and '/')
  double er = 0.0;   // relative permittivity
  double h  = 0.0;   // substrate height, in hUnit
  QString hUnit;     // length unit of h (one of: mil cm mm m um in ft)
  double t = 0.0;    // metalization thickness, in tUnit
  QString tUnit;     // length unit of t (one of: mil cm mm m um in ft)
  double tand = 0.0; // dielectric loss tangent
  double rho  = 0.0; // specific resistance of metal [Ohm*m] (may be 0)
  double D    = 0.0; // rms substrate roughness [m], bare SI value
};

/* Splits a "<value> <unit>" string (e.g. "0.8 mm") into a numeric value and
   a textual unit.  Returns false when the value is not numeric or the unit
   is not one of the Transcalc-supported length units. */
bool parseLengthWithUnit(const QString& text, double& value, QString& unit);

/* Reads all usable <Component>/<SUBST> entries from a Qucs library file.
   Returns an empty list when the file cannot be opened.  Malformed entries
   are skipped silently. */
QList<SubstrateMaterial> readSubstrateLibrary(const QString& filePath);

#endif /* SUBSTRATELIB_H */
