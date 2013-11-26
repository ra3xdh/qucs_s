/***************************************************************************
 * Copyright (C) 2006 by Gopala Krishna A <krishna.ggk@gmail.com>          *
 *                                                                         *
 * This is free software; you can redistribute it and/or modify            *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2, or (at your option)     *
 * any later version.                                                      *
 *                                                                         *
 * This software is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this package; see the file COPYING.  If not, write to        *
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,   *
 * Boston, MA 02110-1301, USA.                                             *
 ***************************************************************************/

#ifndef VTABBEDDOCKWIDGET_H
#define VTABBEDDOCKWIDGET_H

#include "vtabglobal.h"
#include <Q3DockWindow>

class VTabWidget;

/**
    @author Gopala Krishna A <krishna.ggk@gmail.com>
*/
class VTabbedDockWidget : public Q3DockWindow,public VTabInterface
{
  Q_OBJECT
public:
  VTabbedDockWidget(Place p=InDock, QWidget* parent=0l, const char* name=0l);

  virtual ~VTabbedDockWidget();

  virtual void setWidget(QWidget *w);
private:
  VTabWidget *m_tabWidget;

private slots:
  void slotStackShown();
  void slotStackHidden();
  void updatePosition(Q3DockWindow::Place p);
};

#endif
