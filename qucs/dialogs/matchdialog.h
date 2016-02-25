/***************************************************************************
                              matchdialog.h
                             ---------------
    begin                : Fri Jul 22 2005
    copyright            : (C) 2005 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MATCHDIALOG_H
#define MATCHDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>

class Element;
class QLabel;
class QLineEdit;
class QComboBox;
class QCheckBox;
class QVBoxLayout;
class QDoubleValidator;


class MatchDialog : public QDialog {
   Q_OBJECT
public:
  MatchDialog(QWidget *parent=0);
 ~MatchDialog();

  static void c2p(double&, double&);
  static void p2c(double&, double&);
  static void r2z(double&, double&, double);
  static void z2r(double&, double&, double);
  static QString calcMatching(double, double, double, double);
  static bool calcMatchingCircuit(double, double, double, double);
  static QString calcBiMatch(double, double, double, double, double, double,
                             double, double);
  static bool calc2PortMatch(double, double, double, double, double, double,
                             double, double, double);
  void setFrequency(double);

  QLineEdit *Ref1Edit, *Ref2Edit, *FrequencyEdit,
            *S11magEdit,*S11degEdit, *S21magEdit,*S21degEdit,
            *S12magEdit,*S12degEdit, *S22magEdit,*S22degEdit;
  QCheckBox *TwoCheck;

public slots:
  void slotButtCreate();
  void slotImpedanceChanged(const QString&);
  void slotReflexionChanged(const QString&);
  void slotSetTwoPort(bool);
  void slotChangeMode(int);

private:
  QVBoxLayout *all;   // the mother of all widgets
  QDoubleValidator  *DoubleVal;
  QLabel      *Port1Label, *Port2Label, *Ohm1Label, *Ohm2Label,
              *FormatLabel, *FrequencyLabel,
              *S11Label, *S11sLabel, *S11uLabel,
              *S21Label, *S21sLabel, *S21uLabel,
              *S12Label, *S12sLabel, *S12uLabel,
              *S22Label, *S22sLabel, *S22uLabel;
  QComboBox   *FormatCombo, *UnitCombo;

  double tmpS21mag, tmpS21deg;

  void set2PortWidgetsVisible(bool);
};

#endif
