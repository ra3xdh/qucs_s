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

#include "vtabbeddockwidget.h"
#include "vtabwidget.h"
#include <qapplication.h>
#include <q3mainwindow.h>

VTabbedDockWidget::VTabbedDockWidget(Place p, QWidget* parent, const char* name): Q3DockWindow(p, parent, name)
{
  setOrientation(Qt::Vertical);
  m_tabWidget = 0l;
  setCloseMode(Q3DockWindow::Always);
}

VTabbedDockWidget::~VTabbedDockWidget()
{}

void VTabbedDockWidget::setWidget(QWidget *w)
{
  Q3DockWindow::setWidget(w);
  if(!(w->inherits("VTabWidget")))
    return;
  m_tabWidget = (VTabWidget*)w;
  setResizeEnabled(false);
  setHorizontallyStretchable(false);
  connect(m_tabWidget,SIGNAL(widgetStackHidden()),this,SLOT(slotStackHidden()));
  connect(m_tabWidget,SIGNAL(widgetStackShown()),this,SLOT(slotStackShown()));
  connect(this,SIGNAL(placeChanged( Q3DockWindow::Place )),SLOT(updatePosition(Q3DockWindow::Place)));
}

/*!
    \fn VTabbedDockWidget::slotStackShown()
 */
void VTabbedDockWidget::slotStackShown()
{
  setFixedExtentWidth(m_tabWidget->sizeHint().width());
  setHorizontallyStretchable(true);
  setResizeEnabled(true);
}

/*!
    \fn VTabbedDockWidget::slotStackHidden()
 */
void VTabbedDockWidget::slotStackHidden()
{
  setFixedExtentWidth(m_tabWidget->sizeHint().width());
  setHorizontallyStretchable(false);
  setResizeEnabled(false);
}

/*!
    \fn VTabbedDockWidget::updatePosition()
 */
void VTabbedDockWidget::updatePosition(Q3DockWindow::Place p)
{
  if(p==OutsideDock)
    return;
  if(!(qApp->mainWidget()->inherits("QMainWindow")))
    return;
  Q3MainWindow *mainWin = (Q3MainWindow*)(qApp->mainWidget());
  Qt::Dock dock;
  int ind,eo;//Not needed really
  bool nl;//Not needed really

  bool res = mainWin->getLocation ( this, dock, ind, nl, eo );
  if(res == false)
    return;
  if(dock == Qt::DockLeft)
    m_tabWidget->setPosition(TabLeft);
  else if(dock == Qt::DockRight)
    m_tabWidget->setPosition(TabRight);
  mainWin->lineUpDockWindows();
}
