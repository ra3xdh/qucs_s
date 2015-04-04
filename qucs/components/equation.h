/***************************************************************************
                          equation.h  -  description
                             -------------------
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

#ifndef EQUATION_H
#define EQUATION_H

#include "component.h"


class Equation : public Component  {

public:
  Equation();
  ~Equation();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  static void splitEqn(QString &eqn, QStringList &tokens);
  QString getExpression(bool isXyce);
  QString getEquations(QString sim, QStringList &dep_vars);
  QString getNgspiceScript();

private:
  bool containNodes(QStringList &tokens);
  void convertNodeNames(QStringList &tokens, QString &sim);

protected:
  QString vhdlCode(int);
  QString verilogCode(int);
};

#endif
