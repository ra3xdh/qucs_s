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

#ifndef VERTICALTABBAR_H
#define VERTICALTABBAR_H

#include "vtabglobal.h"
#include "vtabbutton.h"
#include <qwidget.h>
#include <q3ptrlist.h>
//Added by qt3to4:
#include <Q3VBoxLayout>

class Q3VBoxLayout;
class VTab;

/**
    @author Gopala Krishna A <krishna.ggk@gmail.com>
*/
class VTabBar : public QWidget,public VTabInterface
{
  Q_OBJECT
public:
  VTabBar(VTabPosition p = TabLeft,QWidget* parent = 0l, const char* name = 0l);

  virtual ~VTabBar();
  VTab* addTab(const QString& caption,int id = -1);

  VTab* findTab(int id);
  void setTabToolTip(VTab *tab,const QString &tip);
  void setTabToolTip(int id,const QString &tip);
  void removeTab(VTab *tab);
  void removeTab(int id);
  bool isAllTabsOff();

public slots:

  void setCurrentTab(VTab *tab);
  void setCurrentTab(int id);
  void setPosition(VTabPosition p_pos);
  void switchOffAllTabs();

signals:
  void activatedTab(int p_id);
  void activatedTab(VTab *tab);
  void allTabsOff();

private slots:
  void setTabState(int p_id,bool state);

private:
 int m_index;
 bool m_tabsOff;
 VTabPosition m_position;
 Q3VBoxLayout* m_layout;
 Q3PtrList<VTab> m_tabs;

};

#endif
