/***************************************************************************
                               component.h
                              -------------
    begin                : Sat Aug 23 2003
    copyright            : (C) 2003 by Michael Margraf
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

#ifndef COMPONENT_H
#define COMPONENT_H

#include <QtCore>
#include "extsimkernels/spicecompat.h"
#include "qt3_compat/qt_compat.h"

#include "element.h"


class Schematic;
class ViewPainter;
class QString;
class QPen;


class Component : public Element {
public:
  Component();
  virtual ~Component() {};

  virtual Component* newOne();
  virtual void recreate(Schematic*) {};
  QString getNetlist();
  QString getSpiceNetlist(bool isXyce = false);
  QString getVerilogACode();
  virtual QString getExpression(bool isXyce = false);
  virtual QString getEquations(QString sim, QStringList &dep_vars);
  virtual QStringList getExtraVariables();
  virtual QString getProbeVariable(bool isXyce = false);
  virtual QString getSpiceModel();
  virtual QString getNgspiceBeforeSim(QString sim, int lvl=0);
  virtual QString getNgspiceAfterSim(QString sim, int lvl=0);
  virtual QString getVAvariables() {return QString("");};
  virtual QString getVAExpressions() {return QString("");};
  virtual void getExtraVANodes(QStringList& ) {};
  QString get_VHDL_Code(int);
  QString get_Verilog_Code(int);
  void    paint(ViewPainter*);
  void    paintScheme(Schematic*);
  void    print(ViewPainter*, float);
  void    setCenter(int, int, bool relative=false);
  void    getCenter(int&, int&);
  int     textSize(int&, int&);
  void    Bounding(int&, int&, int&, int&);
  void    entireBounds(int&, int&, int&, int&);
  bool    getSelected(int, int);
  int     getTextSelected(int, int, float);
  void    rotate();
  void    mirrorX();  // mirror about X axis
  void    mirrorY();  // mirror about Y axis
  QString save();
  bool    load(const QString&);

  // to hold track of the component appearance for saving and copying
  bool mirroredX;   // is it mirrored about X axis or not
  int  rotated;     // rotation angle divided by 90 degrees

  int icon_dx = 0; // used to adjust icon position
  int icon_dy = 0;

  virtual QString getSubcircuitFile() { return ""; }
  // set the pointer scematic associated with the component
  virtual void setSchematic (Schematic* p) { containingSchematic = p; }
  virtual Schematic* getSchematic () {return containingSchematic; }

  QList<qucs::Line *>     Lines;
  QList<struct qucs::Arc *>      Arcs;
  QList<qucs::Rect *>     Rects;
  QList<qucs::Ellips *>     Ellipses;
  QList<Port *>     Ports;
  QList<Text *>     Texts;
  Q3PtrList<Property> Props;

  #define COMP_IS_OPEN    0
  #define COMP_IS_ACTIVE  1
  #define COMP_IS_SHORTEN 2
  int  isActive; // should it be used in simulation or not ?
  int  tx, ty;   // upper left corner of text (position)
  bool showName;
  bool isSimulation; // is it AC,DC,TR or other spice-compatible simulation?
  bool isProbe; // is it Voltage/Current spice-compatible probe?
  bool isEquation;
  int Simulator = spicecompat::simAll;;
  QString  Model, Name;
  QString  Description;
  QString  SpiceModel;
  QPen WrongSimulatorPen;

  void paintIcon(QPixmap *pixmap);

protected:
  virtual QString netlist();
  virtual QString spice_netlist(bool isXyce = false);
  virtual QString va_code();
  virtual QString vhdlCode(int);
  virtual QString verilogCode(int);
  QString form_spice_param_list(QStringList& ignore_list, QStringList& convert_list);

  int  analyseLine(const QString&, int);
  bool getIntegers(const QString&, int *i1=0, int *i2=0, int *i3=0,
                   int *i4=0, int *i5=0, int *i6=0);
  bool getPen(const QString&, QPen&, int);
  bool getBrush(const QString&, QBrush&, int);

  void copyComponent(Component*);
  Property * getProperty(const QString&);
  Schematic* containingSchematic;
};


class MultiViewComponent : public Component {
public:
  MultiViewComponent() {};
  virtual ~MultiViewComponent() {};

  void recreate(Schematic*);

protected:
  virtual void createSymbol() {};
};


class GateComponent : public MultiViewComponent {
public:
  GateComponent();
  QString netlist();
  QString spice_netlist(bool isXyce);
  QString vhdlCode(int);
  QString verilogCode(int);

protected:
  void createSymbol();
};

// prototype of independent function
Component* getComponentFromName(QString& Line, Schematic* p=NULL);

#endif
