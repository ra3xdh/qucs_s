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


ImagePainting::ImagePainting() :
      Rectangle(false),
      penColor(Qt::black),
      penWidth(1),
      penStyle(Qt::SolidLine)
{
  Name = "ImagePainting ";
}

Painting* ImagePainting::newOne()
{
  return new ImagePainting();
}


void ImagePainting::paint(QPainter* painter) {
  loadImage();

  // Use originalImage if available, otherwise use image
  QPixmap imageToPaint = originalImage.isNull() ? image : originalImage;

  // Null checks
  if (!imageToPaint.isNull() && !boundingRect().isEmpty()) {
    painter->save();
    painter->setPen(Qt::NoPen);

    // Scale the image to fit the bounding rectangle
    QPixmap scaledImage = imageToPaint.scaled(
        boundingRect().size(),
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        );

    painter->drawPixmap(boundingRect().topLeft(), scaledImage);
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
  if (parts.size() < 6) return false;

  // Extract coordinates
  if (parts[0] != "ImagePainting") return false;

  bool ok;
  x1 = parts[1].toInt(&ok);
  if (!ok) return false;

  y1 = parts[2].toInt(&ok);
  if (!ok) return false;

  x2 = parts[3].toInt(&ok);
  if (!ok) return false;

  y2 = parts[4].toInt(&ok);
  if (!ok) return false;

  // The base64 image data is in part 5
  QString imageData = parts[5];

  // Clear previous image and path
  image = QPixmap();
  originalImage = QPixmap();
  imagePath.clear();

  // Try to load image from base64 data
  if (!imageData.isEmpty()) {
    QByteArray byteArray = QByteArray::fromBase64(imageData.toUtf8());
    if (!byteArray.isEmpty()) {
      if (image.loadFromData(byteArray)) {
        originalImage = image; // Store original for resize operations
        return true;
      } else {
        qWarning("Failed to load image from base64 data");
      }
    }
  }

  return false;
}

QString ImagePainting::save() {
  QString imageData;

  // If we have a loaded image, convert it to base64
  if (!image.isNull()) {
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);

    // Save as PNG format for best quality and transparency support
    if (image.save(&buffer, "PNG")) {
      imageData = byteArray.toBase64();
    }
  }
  // If no image loaded but we have a path, try to load and convert
  else if (!imagePath.isEmpty()) {
    QPixmap tempImage;
    if (tempImage.load(imagePath)) {
      QByteArray byteArray;
      QBuffer buffer(&byteArray);
      buffer.open(QIODevice::WriteOnly);

      if (tempImage.save(&buffer, "PNG")) {
        imageData = byteArray.toBase64();
      }
    }
  }

  // Return in format: "ImagePainting x1 y1 x2 y2 base64_data"
  return QString("ImagePainting %1 %2 %3 %4 %5")
      .arg(x1).arg(y1).arg(x2).arg(y2).arg(imageData);
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
    QWidget* parentWidget = sch ? sch->parentWidget() : nullptr;
    if (!parentWidget) {
      parentWidget = QApplication::activeWindow();
    }

    QString newPath = QFileDialog::getOpenFileName(parentWidget, QObject::tr("Select Image"));

    if (!newPath.isEmpty()) {
      imagePath = newPath;
      image = QPixmap();
      originalImage = QPixmap();
      loadImage();

      // Set dimensions to actual image size if image loaded successfully
      if (!image.isNull()) {
        x2 = x1 + image.width();
        y2 = y1 + image.height();
      } else {
        // Fallback to default square size if image fails to load
        const int squareSize = 100;
        x2 = x1 + squareSize;
        y2 = y1 + squareSize;
      }
    } else {
      // No image selected.
      return false;
    }
  }

  return true;
}

void ImagePainting::MouseResizeMoving(int x, int y, Schematic* p) {
  Rectangle::MouseResizeMoving(x, y, p);
}

bool ImagePainting::Dialog(QWidget* parent) {
  QDialog dialog(parent);
  dialog.setWindowTitle(QObject::tr("Image Properties"));
  auto* layout = new QVBoxLayout(&dialog);

  // Add image path UI
  auto* imageLayout = new QHBoxLayout;
  auto* pathLabel = new QLabel(QObject::tr("Image Path:"));
  auto* pathEdit = new QLineEdit(imagePath);
  auto* browseButton = new QPushButton(QObject::tr("Browse..."));

  // Add status label to show if image is embedded or external
  auto* statusLabel = new QLabel();
  if (!image.isNull() && imagePath.isEmpty()) {
    statusLabel->setText(QObject::tr("Image embedded in schematic"));
    statusLabel->setStyleSheet("color: green; font-style: italic;");
  } else if (!imagePath.isEmpty()) {
    statusLabel->setText(QObject::tr("External image file"));
    statusLabel->setStyleSheet("color: blue; font-style: italic;");
  } else {
    statusLabel->setText(QObject::tr("No image loaded"));
    statusLabel->setStyleSheet("color: red; font-style: italic;");
  }

  QObject::connect(browseButton, &QPushButton::clicked, [&]() {
    QString path = QFileDialog::getOpenFileName(&dialog, QObject::tr("Select Image"), QDir::homePath());
    if (!path.isEmpty()) {
      pathEdit->setText(path);
      statusLabel->setText(QObject::tr("External image file"));
      statusLabel->setStyleSheet("color: blue; font-style: italic;");
    }
  });

  imageLayout->addWidget(pathLabel);
  imageLayout->addWidget(pathEdit);
  imageLayout->addWidget(browseButton);

  // Add dimensions UI
  auto* dimensionsLayout = new QVBoxLayout;

  // Width input
  auto* widthLayout = new QHBoxLayout;
  auto* widthLabel = new QLabel(QObject::tr("Width:"));
  auto* widthEdit = new QLineEdit(QString::number(x2 - x1));
  widthEdit->setValidator(new QIntValidator(1, 10000, &dialog));
  widthLayout->addWidget(widthLabel);
  widthLayout->addWidget(widthEdit);

  // Height input
  auto* heightLayout = new QHBoxLayout;
  auto* heightLabel = new QLabel(QObject::tr("Height:"));
  auto* heightEdit = new QLineEdit(QString::number(y2 - y1));
  heightEdit->setValidator(new QIntValidator(1, 10000, &dialog));
  heightLayout->addWidget(heightLabel);
  heightLayout->addWidget(heightEdit);

  // Aspect ratio checkbox
  auto* aspectRatioCheck = new QCheckBox(QObject::tr("Keep aspect ratio"));
  aspectRatioCheck->setChecked(false);

         // Reset to original button
  auto* resetButton = new QPushButton(QObject::tr("Reset to original dimensions"));
  resetButton->setEnabled(!image.isNull()); // Enable if image is loaded

  dimensionsLayout->addLayout(widthLayout);
  dimensionsLayout->addLayout(heightLayout);
  dimensionsLayout->addWidget(aspectRatioCheck);
  dimensionsLayout->addWidget(resetButton);

  // Function to reset dimensions to original image size
  auto resetToOriginal = [&]() {
    QPixmap tempImage;
    QString currentPath = pathEdit->text();

    // First try to use already loaded image
    if (!image.isNull()) {
      tempImage = image;
    }
    // Otherwise try to load from path
    else if (!currentPath.isEmpty() && tempImage.load(currentPath)) {
      image = tempImage;
    }

    if (!tempImage.isNull()) {
      widthEdit->setText(QString::number(tempImage.width()));
      heightEdit->setText(QString::number(tempImage.height()));
      resetButton->setEnabled(true);
    }
  };

  // Function to calculate and update height based on width and aspect ratio
  auto updateHeight = [&]() {
    if (aspectRatioCheck->isChecked()) {
      QPixmap currentImage = image;

      // If no image loaded, try to load from path
      if (currentImage.isNull() && !pathEdit->text().isEmpty()) {
        currentImage.load(pathEdit->text());
      }

      if (!currentImage.isNull()) {
        int width = widthEdit->text().toInt();
        if (width > 0) {
          double aspectRatio = (double)currentImage.height() / currentImage.width();
          int height = qRound(width * aspectRatio);
          heightEdit->setText(QString::number(height));
        }
      }
    }
  };

  // Connect reset button
  QObject::connect(resetButton, &QPushButton::clicked, resetToOriginal);

  // Connect aspect ratio checkbox
  QObject::connect(aspectRatioCheck, &QCheckBox::toggled, &dialog, [heightEdit, updateHeight](bool checked) {
    heightEdit->setEnabled(!checked);
    if (checked) {
      updateHeight();
    }
  });

  // Connect width change to height calculation
  QObject::connect(aspectRatioCheck, &QCheckBox::toggled, &dialog, [heightEdit, updateHeight](bool checked) {
    heightEdit->setEnabled(!checked);
    if (checked) {
      updateHeight();
    }
  });

  // Connect path change to reload image and update aspect ratio
  QObject::connect(pathEdit, &QLineEdit::textChanged, [&](const QString& newPath) {
    if (!newPath.isEmpty()) {
      QPixmap tempImage;
      if (tempImage.load(newPath)) {
        resetButton->setEnabled(true);
        if (aspectRatioCheck->isChecked()) {
          updateHeight();
        }
      } else {
        resetButton->setEnabled(false);
      }
    } else {
      resetButton->setEnabled(!image.isNull());
    }
  });

  layout->addWidget(statusLabel);
  layout->addLayout(imageLayout);
  layout->addLayout(dimensionsLayout);

  QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  QObject::connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
  QObject::connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

  layout->addWidget(&buttons);

  if (dialog.exec() == QDialog::Rejected) return false;

  // Update image path and load new image if changed
  QString newPath = pathEdit->text();
  if (newPath != imagePath) {
    imagePath = newPath;
    if (!imagePath.isEmpty()) {
      // Load new image from file path
      image = QPixmap();
      loadImage();
    }
    // If path is cleared but we have an embedded image, keep it
    // (imagePath will be empty but image will remain loaded)
  }

  // Update dimensions
  int newWidth = widthEdit->text().toInt();
  int newHeight = heightEdit->text().toInt();

  if (newWidth > 0 && newHeight > 0) {
    x2 = x1 + newWidth;
    y2 = y1 + newHeight;
  }

  return true;
}
Element* ImagePainting::info(QString& Name, char* &BitmapFile, bool getNewOne) {
  Name = QObject::tr("Image");
  BitmapFile = (char*)"ImagePainting";
  return getNewOne ? new ImagePainting() : nullptr;
}

void ImagePainting::loadImage() {
  // Don't reload if we already have an image and no path (base64 loaded)
  if (!image.isNull() && imagePath.isEmpty()) {
    return;
  }

  // This method is now primarily used for loading from file path
  if (imagePath.isEmpty()) return;

  // Only load if not already loaded or if path changed
  if (image.isNull()) {
    if (image.load(imagePath)) {
      originalImage = image; // Store original for resize operations
    } else {
      qWarning("Failed to load image: %s", qUtf8Printable(imagePath));
    }
  }
}

// Override rotate methods to maintain functionality
bool ImagePainting::rotate() noexcept {
  bool result = qucs::Rectangle::rotate();
  if (result) {
    // Clear cached image to force reload with new orientation
    image = QPixmap();
  }
  return result;
}

bool ImagePainting::rotate(int xc, int yc) noexcept {
  bool result = qucs::Rectangle::rotate(xc, yc);
  if (result) {
    // Clear cached image to force reload with new orientation
    image = QPixmap();
  }
  return result;
}


void ImagePainting::setImageFromPixmap(const QPixmap& pixmap) {
  if (!pixmap.isNull()) {
    image = pixmap;
    originalImage = pixmap;
    imagePath.clear(); // Clear path since this is embedded image data
  }
}

void ImagePainting::setImageFromPath(const QString& path) {
  if (!path.isEmpty()) {
    imagePath = path;
    image = QPixmap(); // Clear current image
    originalImage = QPixmap(); // Clear original image
    loadImage(); // Load from the new path
  }
}

void ImagePainting::setImageFromClipboard() {
  QClipboard* clipboard = QApplication::clipboard();
  if (clipboard->mimeData()->hasImage()) {
    QImage clipboardImage = clipboard->image();
    if (!clipboardImage.isNull()) {
      QPixmap pixmap = QPixmap::fromImage(clipboardImage);
      setImageFromPixmap(pixmap);
    }
  }
}
