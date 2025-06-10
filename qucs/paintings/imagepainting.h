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
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QDebug>
#include <QPainter>
#include <QObject>
#include <QComboBox>
#include <QCheckBox>
#include <QApplication>

class ImagePainting : public QObject, public qucs::Rectangle {
  Q_OBJECT
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

  void setImageFromPixmap(const QPixmap& pixmap);
  void setImageFromPath(const QString& path);
  void setImageFromClipboard();

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
  QPixmap originalImage;
  void loadImage();

  // Local pen properties
  QColor penColor;
  int penWidth;
  Qt::PenStyle penStyle;
  bool m_filled;

  // Dialog widget members
  QLineEdit* m_pathEdit;
  QLineEdit* m_widthEdit;
  QLineEdit* m_heightEdit;
  QCheckBox* m_aspectRatioCheck;
  QPushButton* m_resetButton;
  QLabel* m_statusLabel;

  // Dialog handler methods
  void onBrowseClicked();
  void onResetClicked();
  void onAspectRatioToggled(bool checked);
  void onPathChanged(const QString& newPath);
  void updateHeight();
};

#endif // IMAGEPAINTING_H
