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

#ifndef VERTICALTABWIDGET_H
#define VERTICALTABWIDGET_H

#include "vtabglobal.h"
#include <qwidget.h>

class Q3WidgetStack;
class VTabBar;

/**
    @author Gopala Krishna A <krishna.ggk@gmail.com>
*/
class VTabWidget : public QWidget,public VTabInterface
{
  Q_OBJECT
public:
  VTabWidget(VTabPosition p = TabLeft,QWidget* parent=0l, const char* name=0l);

  virtual ~VTabWidget();
  virtual QSize sizeHint() const;
  void addPage(QWidget *w,const QString& tabCap);
	
  QWidget* page(int id);
  QWidget* currentPage();
  void setTabToolTip(int id,const QString &tip);
  void removePage(QWidget *w);
  void removePage(int id);
  int id(QWidget *w);

public slots:
  void setPosition(VTabPosition p);
  void setCurrentPage(int id);
  void setCurrentPage(QWidget *w);
	
signals:
  void widgetStackShown();
  void widgetStackHidden();
  void activatedPage(QWidget *w);

private:
  VTabPosition m_position;
  VTabBar* m_bar;
  Q3WidgetStack* m_wStack;
  int m_autoIndexer;

};

#endif
