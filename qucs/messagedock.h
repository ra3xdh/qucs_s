/***************************************************************************
                             messagedock.h
                             -------------
    begin                : Tue Mar 11 2014
    copyright            : (C) 2014 by Guilherme Brondani Torri
    email                : guitorri AT gmail DOT com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MESSAGEDOCK_H
#define MESSAGEDOCK_H

#include "qucs.h"

#include <QWidget>

#include <QDockWidget>

/*!
 * \file messagedock.h
 * \brief The MessageDock class definiion
 */
class MessageDock : public QWidget {
  Q_OBJECT
public:
  MessageDock(QucsApp*);
 ~MessageDock() {};

public:
  /*!
   * \brief admsOutput holds the make output of running admsXml
   */
  QTextEdit *admsOutput;
  /*!
   * \brief cppOutput holds the make output of running a C++ compiler
   */
  QTextEdit *cppOutput;

  QDockWidget *admsDock;
  QDockWidget *cppDock;

};

#endif // MESSAGEDOCK_H
