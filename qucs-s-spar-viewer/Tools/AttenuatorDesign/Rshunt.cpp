/***************************************************************************
                                Rshunt.cpp
                                ----------
    copyright            :  QUCS team
    author               :  Andrés Martínez Mera
    email                : andresmmera@protonmail.com
 ***************************************************************************/

/***************************************************************************
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 ***************************************************************************/
#include "AttenuatorDesigner.h"

// Reference: Based on design equations from attenuatorfunc.cpp R_SHUNT case

void AttenuatorDesigner::RShuntAttenuator() {

  ComponentInfo TermSpar1, TermSpar2;
  ComponentInfo Res1, Ground;
  NodeInfo NIin;
  Components.clear();

         // Design equations
  double L = pow(10, -Specs.Attenuation / 10);

  double R1 = (2*sqrt(Specs.Zout*Specs.Zin*L)*Specs.Zout*Specs.Zin
               + (Specs.Zout*Specs.Zout*Specs.Zin + Specs.Zout*Specs.Zin*Specs.Zin)*L)
              / (4*Specs.Zout*Specs.Zin
                 - (Specs.Zout*Specs.Zout + 2*Specs.Zout*Specs.Zin + Specs.Zin*Specs.Zin)*L); // Shunt resistor

  double Zin = (Specs.Zout * R1) / (Specs.Zout + R1); // Input impedance
  double Zout = (Specs.Zin  * R1) / (Specs.Zin  + R1); // Output impedance

         // Power dissipation
  Pdiss.R1 = Specs.Pin * (1 - L);

         // --- Circuit Implementation ---
         // Input terminal
  TermSpar1.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 0, 0, 0);
  TermSpar1.val["Z"] = num2str(Specs.Zin, Resistance); // Effective input impedance
  Schematic.appendComponent(TermSpar1);

  // Zin label
  QString Zin_label = QString("Zin = %1 \u03A9").arg(num2str(Zin));
  QGraphicsTextItem* label1 = new QGraphicsTextItem(Zin_label);
  label1->setDefaultTextColor(Qt::red);
  label1->setFont(QFont("Arial", 6, QFont::Bold));
  label1->setPos(-20, -30);
  Schematic.appendText(label1);

         // Node before shunt
  NIin.setParams(QString("N%1").arg(++Schematic.NumberComponents[ConnectionNodes]), 50, 0);
  Schematic.appendNode(NIin);

         // Shunt resistor to ground
  Res1.setParams(QString("R%1").arg(++Schematic.NumberComponents[Resistor]), Resistor, 0, 50, 50);
  Res1.val["R"] = num2str(R1, Resistance);
  Schematic.appendComponent(Res1);

  Ground.setParams(QString("GND%1").arg(++Schematic.NumberComponents[GND]), GND, 0, 50, 100);
  Schematic.appendComponent(Ground);

         // Output terminal
  TermSpar2.setParams(QString("T%1").arg(++Schematic.NumberComponents[Term]), Term, 180, 100, 0);
  TermSpar2.val["Z"] = num2str(Specs.Zin, Resistance); // Effective output impedance
  Schematic.appendComponent(TermSpar2);

         // --- Wiring Connections ---
         // Input terminal to input node
  Schematic.appendWire(TermSpar1.ID, 0, NIin.ID, 0);

         // Node to shunt resistor (top terminal)
  Schematic.appendWire(NIin.ID, 0, Res1.ID, 1);

         // Shunt resistor to ground
  Schematic.appendWire(Res1.ID, 0, Ground.ID, 0);

         // Output node to terminal
  Schematic.appendWire(NIin.ID, 0, TermSpar2.ID, 0);

  // Zout label
  QString Zout_label = QString("Zout = %1 \u03A9").arg(num2str(Zout));
  QGraphicsTextItem* label2 = new QGraphicsTextItem(Zout_label);
  label2->setDefaultTextColor(Qt::red);
  label2->setFont(QFont("Arial", 6, QFont::Bold));
  label2->setPos(80, -30);
  Schematic.appendText(label2);
}
