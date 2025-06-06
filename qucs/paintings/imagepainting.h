/***************************************************************************
                              imagepainting.h
                             ---------------
    copyright            : (C) 2025 by Andrés Martínez Mera
    email                : andresmartinezmera@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef IMAGEPAINTING_H
#define IMAGEPAINTING_H

#include "rectangle.h"
#include <QColor>
#include <QPen>
#include <QPixmap>

class ImagePainting : public qucs::Rectangle {
public:
  ImagePainting();
  Painting* newOne() override;
  void paint(QPainter* painter) override;
  bool load(const QString& s) override;
  QString save() override;
  QString saveCpp() override;
  QString saveJSON() override;
  bool Dialog(QWidget* parent = nullptr) override;
  static Element* info(QString& Name, char* &BitmapFile, bool getNewOne = false);

  // Override selection and interaction methods
  bool getSelected(const QPoint& click, int tolerance) override;
  bool resizeTouched(const QPoint& click, int tolerance) override;
  void MouseMoving(const QPoint& onGrid, Schematic* sch, const QPoint& cursor) override;
  bool MousePressing(Schematic* sch = nullptr) override;
  void MouseResizeMoving(int x, int y, Schematic* p) override;
  bool rotate() noexcept override;
  bool rotate(int xc, int yc) noexcept override;

private:
  QString imagePath;
  QPixmap image;
  void loadImage();

  // Local pen properties
  QColor penColor;
  int penWidth;
  Qt::PenStyle penStyle;
  bool m_filled;
};

#endif // IMAGEPAINTING_H
