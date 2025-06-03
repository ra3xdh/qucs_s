/***************************************************************************
                              imagepainting.cpp
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
#include "imagepainting.h"
#include "filldialog.h"
#include "misc.h"
#include "schematic.h"
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


namespace qucs {

ImagePainting::ImagePainting(bool filled) :
      Rectangle(filled),
      m_filled(filled),
      penColor(Qt::black),
      penWidth(1),
      penStyle(Qt::SolidLine)
{
  Name = "ImagePainting";
}

void ImagePainting::paint(QPainter* painter) {
  loadImage();

   // Null checks
  if (!image.isNull() && !boundingRect().isEmpty()) {
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->drawPixmap(boundingRect(), image);
    painter->restore();

    // Draw selection handles when selected
    if (isSelected) {
      painter->setPen(QPen(Qt::darkGray, penWidth + 5));
      painter->drawRect(boundingRect());
      painter->setPen(QPen(Qt::white, penWidth, penStyle));
      painter->drawRect(boundingRect());

             // Draw resize handles
      const auto bounds = boundingRect().marginsAdded({0, 0, 1, 1});
      misc::draw_resize_handle(painter, bounds.topLeft());
      misc::draw_resize_handle(painter, bounds.topRight());
      misc::draw_resize_handle(painter, bounds.bottomRight());
      misc::draw_resize_handle(painter, bounds.bottomLeft());
    }
  } else {
    // Ensure base class is in valid state
    if (x1 == x2) x2 = x1 + 100; // Default width
    if (y1 == y2) y2 = y1 + 100; // Default height
    Rectangle::paint(painter);
  }
}

bool ImagePainting::load(const QString& s) {
  QStringList parts = s.split(' ', Qt::SkipEmptyParts);
  if (parts.size() < 11) return false;

         // Validate base class coordinates first
  if (!Rectangle::load(parts.mid(0, 10).join(' '))) {
    // Initialize safe defaults
    x1 = y1 = 0;
    x2 = y2 = 100;
    return false;
  }

  imagePath = parts.value(10);
  // Clear cached image to force reload
  image = QPixmap();
  return true;
}

QString ImagePainting::save() {
  return Rectangle::save() + " " + imagePath;
}

QString ImagePainting::saveCpp() {
  // Customize as needed; example:
  return QString("new ImagePainting(%1, %2, %3, %4, \"%5\")")
      .arg(x1).arg(y1).arg(x2-x1).arg(y2-y1).arg(imagePath);
}

QString ImagePainting::saveJSON() {
  return QStringLiteral("{\"type\":\"ImagePainting\",\"image\":\"%1\",%2}")
      .arg(imagePath, Rectangle::saveJSON().mid(1)); // Merge with base JSON
}

// Override getSelected to handle image area
bool ImagePainting::getSelected(const QPoint& click, int tolerance) {
  // Always check if click is within bounds (whether filled or not)
  return boundingRect()
      .marginsAdded({tolerance, tolerance, tolerance, tolerance})
      .contains(click);
}

// Override resizeTouched to maintain resize functionality
bool ImagePainting::resizeTouched(const QPoint& click, int tolerance) {
  return Rectangle::resizeTouched(click, tolerance);
}

// Override mouse interaction methods
void ImagePainting::MouseMoving(const QPoint& onGrid, Schematic* sch, const QPoint& cursor) {
  // Get the cursor coordinates
  x1 = onGrid.x();
  y1 = onGrid.y();
  x2 = x1;
  y2 = y1;

  // Draw a symbol (two mountains) while hovering
  // Draw frame
  sch->PostPaintEvent(_Rect, cursor.x() + 13, cursor.y(), 105, 48, 0, 0, true);

  // Draw the sun (larger, in the top-right corner)
  sch->PostPaintEvent(_Ellipse, cursor.x() + 100, cursor.y() + 8, 12, 12, 0, 0, true); // (x, y, width, height)

  // Draw the mountain on the left
  sch->PostPaintEvent(_Line, cursor.x() + 15, cursor.y() + 44, cursor.x() + 45, cursor.y() + 12, 0, 0, true); // left base to peak
  sch->PostPaintEvent(_Line, cursor.x() + 45, cursor.y() + 12, cursor.x() + 75, cursor.y() + 44, 0, 0, true); // peak to right base

  // Draw the mountain on the right
  sch->PostPaintEvent(_Line, cursor.x() + 45, cursor.y() + 44, cursor.x() + 81, cursor.y() + 4, 0, 0, true); // left base to peak
  sch->PostPaintEvent(_Line, cursor.x() + 81, cursor.y() + 4, cursor.x() + 115, cursor.y() + 44, 0, 0, true); // peak to right base

  // Add a ground line
  sch->PostPaintEvent(_Line, cursor.x() + 15, cursor.y() + 44, cursor.x() + 115, cursor.y() + 44, 0, 0, true); // ground
}

bool ImagePainting::MousePressing(Schematic* sch) {

  if (imagePath.isEmpty()) {
    // Define square size
    const int squareSize = 100;

    // Override to make it a square
    x2 = x1 + squareSize;
    y2 = y1 + squareSize;

    QWidget* parentWidget = sch ? sch->parentWidget() : nullptr;
    if (!parentWidget) {
      parentWidget = QApplication::activeWindow();
    }

    QString newPath = QFileDialog::getOpenFileName(parentWidget, QObject::tr("Select Image"));

    if (!newPath.isEmpty()) {
      imagePath = newPath;
      image = QPixmap();
      loadImage();
    }
  }

  return true;
}

void ImagePainting::MouseResizeMoving(int x, int y, Schematic* p) {
  Rectangle::MouseResizeMoving(x, y, p);
  // Clear cached image to force reload with new dimensions
  image = QPixmap();
}

bool ImagePainting::Dialog(QWidget* parent) {
  QDialog dialog(parent);
  dialog.setWindowTitle(QObject::tr("Image Properties"));
  auto* layout = new QVBoxLayout(&dialog);

  // Reuse FillDialog for common properties
  auto* fillDialog = new FillDialog(QObject::tr("Image"), true, &dialog);
  misc::setPickerColor(fillDialog->ColorButt, penColor);
  fillDialog->LineWidth->setText(QString::number(penWidth));
  fillDialog->StyleBox->setCurrentIndex(penStyle - Qt::SolidLine);
  fillDialog->CheckFilled->setChecked(m_filled);
  fillDialog->slotCheckFilled(m_filled);

         // Add image path UI
  auto* imageLayout = new QHBoxLayout;
  auto* pathLabel = new QLabel(QObject::tr("Image Path:"));
  auto* pathEdit = new QLineEdit(imagePath);
  auto* browseButton = new QPushButton(QObject::tr("Browse..."));

  QObject::connect(browseButton, &QPushButton::clicked, [&]() {
    QString path = QFileDialog::getOpenFileName(&dialog, QObject::tr("Select Image"));
    if (!path.isEmpty()) pathEdit->setText(path);
  });

  imageLayout->addWidget(pathLabel);
  imageLayout->addWidget(pathEdit);
  imageLayout->addWidget(browseButton);

  layout->addWidget(fillDialog);
  layout->addLayout(imageLayout);

  QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  QObject::connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
  QObject::connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

  layout->addWidget(&buttons);

  if (dialog.exec() == QDialog::Rejected) return false;

  // Update properties from FillDialog
  penColor = misc::getWidgetBackgroundColor(fillDialog->ColorButt);
  penWidth = fillDialog->LineWidth->text().toInt();
  penStyle = static_cast<Qt::PenStyle>(fillDialog->StyleBox->currentIndex() + Qt::SolidLine);
  m_filled = fillDialog->CheckFilled->isChecked();

  // Update image path
  QString newPath = pathEdit->text();
  if (newPath != imagePath) {
    imagePath = newPath;
    // Clear cached image to force reload
    image = QPixmap();
    loadImage();
  }

  return true;
}

Element* ImagePainting::info(QString& Name, char* &BitmapFile, bool getNewOne) {
  Name = QObject::tr("Image");
  BitmapFile = (char*)"ImagePainting";
  return getNewOne ? new ImagePainting() : nullptr;
}

void ImagePainting::loadImage() {
  if (imagePath.isEmpty()) return;

  // Only load if not already loaded or if path changed
  if (image.isNull()) {
    if (!image.load(imagePath)) {
      qWarning("Failed to load image: %s", qUtf8Printable(imagePath));
    }
  }
}

// Override rotate methods to maintain functionality
bool ImagePainting::rotate() noexcept {
  bool result = Rectangle::rotate();
  if (result) {
    // Clear cached image to force reload with new orientation
    image = QPixmap();
  }
  return result;
}

bool ImagePainting::rotate(int xc, int yc) noexcept {
  bool result = Rectangle::rotate(xc, yc);
  if (result) {
    // Clear cached image to force reload with new orientation
    image = QPixmap();
  }
  return result;
}

}
