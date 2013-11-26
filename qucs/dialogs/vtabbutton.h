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

#ifndef VERTICALTABBUTTON_H
#define VERTICALTABBUTTON_H

#include <QPushButton>
#include <QStyleOptionButton>
#include "vtabglobal.h"

/**
   This class provides vertical button which can be used as ordinary button
 
   @author Gopala Krishna A <krishna.ggk@gmail.com>
*/
class VTab : public QPushButton,public VTabInterface
{
  Q_OBJECT

public:
  VTab(VTabPosition pos = TabLeft,int p_id = -1,QWidget *parent=0l,const char* name=0l);
  VTabPosition position() const { return m_position; }
  int id() const;

  virtual QSize sizeHint() const;
  virtual void setText(const QString &s);

  virtual ~VTab();

public slots:
 void setID(int p_id);
 void setPosition(VTabPosition p_pos);

signals:
 void toggled(int p_id,bool p_state);// internal signal
protected:
  void paintEvent(QPaintEvent* event);
  
private:
  QStyleOptionButton getStyleOption() const;
 VTabPosition m_position;
 int m_id;

 virtual void drawButton(QPainter *p);
 void init();
	
private slots:
  void slotToggled(bool b);
};

/*inline VTabPosition VTab::position() const
{
    return m_position;
}*/

inline int VTab::id() const
{
  return m_id;
}

#endif
