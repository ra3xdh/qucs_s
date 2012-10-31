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

#include "vtabbutton.h"
#include "vtabbar.h"
#include "vtabwidget.h"

#include <qlayout.h>
#include <q3widgetstack.h>
//Added by qt3to4:
#include <Q3HBoxLayout>

VTabWidget::VTabWidget(VTabPosition p,QWidget* parent, const char* name): QWidget(parent, name)
{
  m_position = p;
  Q3HBoxLayout *layout = new Q3HBoxLayout(this);
  m_bar = new VTabBar(p,this);
  m_wStack = new Q3WidgetStack(this);
  layout->addWidget(m_bar,0);
  layout->addWidget(m_wStack,5);
  m_autoIndexer = 0;
  m_wStack->hide();
  connect(m_bar,SIGNAL(allTabsOff()),m_wStack,SLOT(hide()));
  connect(m_bar, SIGNAL(activatedTab(int)), this, SLOT(setCurrentPage(int)));
        
  connect(m_bar,SIGNAL(allTabsOff()),this,SIGNAL(widgetStackHidden()));
  connect(m_bar,SIGNAL(activatedTab(int)),this,SIGNAL(widgetStackShown()));
}

VTabWidget::~VTabWidget()
{}

void VTabWidget::addPage(QWidget *w,const QString& tabCap)
{
  m_bar->addTab(tabCap,m_autoIndexer);
  m_wStack->addWidget(w,m_autoIndexer);
  m_autoIndexer++;
}

QWidget* VTabWidget::page(int id)
{
  return m_wStack->widget(id);
}
	
QWidget* VTabWidget::currentPage()
{
  if(m_bar->isAllTabsOff())
    return 0l;
  return m_wStack->visibleWidget();
}
	
void VTabWidget::setTabToolTip(int id,const QString &tip)
{
  m_bar->setTabToolTip(id,tip);
}
	
void VTabWidget::removePage(QWidget *w)
{
  int _id = m_wStack->id(w);
  if(_id == -1)
    return;
  m_bar->removeTab(_id);
  m_wStack->removeWidget(w);
}

void VTabWidget::removePage(int id)
{
  QWidget *w = m_wStack->widget(id);
  if(!w)
    return;
  removePage(w);
}

int VTabWidget::id(QWidget *w)
{
  return m_wStack->id(w);
}

QSize VTabWidget::sizeHint() const
{
  if(m_wStack->isHidden())
    return m_bar->sizeHint();
  int width = m_bar->sizeHint().width();
  int height = m_bar->sizeHint().height();
  width += m_wStack->sizeHint().width();
  height = QMAX(height,m_wStack->sizeHint().height());
  return QSize(width,height);
}

void VTabWidget::setCurrentPage(int id)
{
  if(!m_wStack->isShown())
    {
      m_wStack->setShown(true);
      emit widgetStackShown();
    }
  m_wStack->raiseWidget(id);
  QWidget *w = m_wStack->widget(id);
  if(w)
    emit activatedPage(w);
  VTab *const t = m_bar->findTab(id);
  if(t == 0l)
    {
      qWarning("BUG: Tab id and widget id mismatch");
      return;
    }
  if(t->isOn())
    return;
  //else programatically called
  m_bar->blockSignals(true);
  m_bar->setCurrentTab(id);
  m_bar->blockSignals(false);
}

void VTabWidget::setCurrentPage(QWidget *w)
{
  int _id = m_wStack->id(w);
  if(_id != -1)
    setCurrentPage(_id);
}


void VTabWidget::setPosition(VTabPosition p)
{
  if(m_position == p)
    return;
  m_position = p;
  Q3HBoxLayout *l = (Q3HBoxLayout*)layout();
  if(!l)
    {
      qWarning( "Error layout is null");
      return;
    }
  l->remove(m_wStack);
  l->remove(m_bar);

  m_bar->setPosition(p);
  if(p == TabLeft)
    {
      l->addWidget(m_bar,0);
      l->addWidget(m_wStack,5);
    }
  else
    {
      l->addWidget(m_wStack,5);
      l->addWidget(m_bar,0);
    }
}
