/*
 * imagewriter.h - declaraction of writer to image
 *
 * Copyright (C) 2014, Yodalee, lc85301@gmail.com
 *
 * This file is part of Qucs
 *
 * Qucs is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Qucs.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef IMAGEWRITER_H_
#define IMAGEWRITER_H_ value

#include <QString>

class QWidget;

class ImageWriter
{
public:
  ImageWriter (QString lastfile);
  virtual ~ImageWriter ();
  int print(QWidget *);
  void noGuiPrint(QWidget *, QString printFile, QString color);

  QString getLastSavedFile();

  void setDiagram(bool diagram) { onlyDiagram = diagram; };
private:
  bool onlyDiagram;
  QString lastExportFilename;
};

#endif
