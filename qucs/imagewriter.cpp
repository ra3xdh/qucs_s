/*
 * imagewriter.cpp - implementation of writer to image
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

#include "schematic.h"
#include "imagewriter.h"
#include "dialogs/exportdialog.h"

#include <QMargins>
#include <QtSvg>


ImageWriter::ImageWriter(QString lastfile)
{
  onlyDiagram = false;
  lastExportFilename = lastfile;
}

ImageWriter::~ImageWriter()
{
}

void
ImageWriter::noGuiPrint(QWidget *doc, const QString& printFile, const QString& color)
{
  Schematic *sch = dynamic_cast<Schematic*>(doc);
  const QMargins bourder{30, 30, 30, 30};
  QRect schematic_bounding_rect = sch->allBoundingRect();

  if (printFile.endsWith(".svg") || printFile.endsWith(".eps")) {
    QSvgGenerator* svg1 = new QSvgGenerator();

    QString tempfile = printFile + ".tmp.svg";
    if (!printFile.endsWith(".svg")) {
        svg1->setFileName(tempfile);
    } else {
        svg1->setFileName(printFile);
    }

    svg1->setSize(schematic_bounding_rect.size());
    QPainter *p = new QPainter(svg1);
    sch->print(nullptr, p, true, true, bourder);

    delete p;
    delete svg1;

    if (!printFile.endsWith(".svg")) {
        QString cmd = "inkscape";
        QStringList args;
        args<<"-z"<<"-D";
        QString tmpf = "--file=" + tempfile;
        args<<tmpf;

        if (printFile.endsWith(".eps")) {
          QString pf = "--export-eps=" + printFile;
          args<<pf;
        }

        int result = QProcess::execute(cmd,args);

        if (result!=0) {
            QMessageBox* msg =  new QMessageBox(QMessageBox::Critical,"Export to image", "Inkscape start error!", QMessageBox::Ok);
            msg->exec();
            delete msg;
        }
        QFile::remove(tempfile);
    }

  } else if (printFile.endsWith(".png")) {
    QImage* img;
    if (color == "BW") {
      img = new QImage(schematic_bounding_rect.size(), QImage::Format_Mono);
    } else {
      img = new QImage(schematic_bounding_rect.size(), QImage::Format_RGB888);
    }

    QPainter* p = new QPainter(img);
    sch->print(nullptr, p, true, true, bourder);

    img->save(printFile);

    delete p;
    delete img;
  } else {
    fprintf(stderr, "Unsupported format of output file. \n"
        "Use PNG, SVG or PDF format!\n");
    return;
  }
}

QString ImageWriter::getLastSavedFile()
{
    return lastExportFilename;
}

// FIXME: should check if filename exists and not silently overwrite
int ImageWriter::print(QWidget *doc)
{
  Schematic *sch = dynamic_cast<Schematic*>(doc);
  const int border = 30;

  int status = -1;

  QRect all = sch->allBoundingRect().marginsAdded(QMargins{30, 30, 30, 30});
  int w = all.width();
  int h = all.height();


  QRect selected = sch->sizeOfSelection();
  int wsel = selected.width();
  int hsel = selected.height();

  ExportDialog *dlg = new ExportDialog(
      w, h, wsel, hsel, lastExportFilename, selected.isNull(), 0);

  if (onlyDiagram) {
    dlg->setDiagram();
  }

  if (dlg->exec()) {
    QString filename = dlg->FileToSave();
    if (QFile::exists(filename)) {
        int r = QMessageBox::question(0, QObject::tr("Overwrite"),
                                         QObject::tr("File \"%1\" already exists.\nOverwrite ?")
                                         .arg(filename),
                                         QMessageBox::Yes|QMessageBox::No);
        if (r == QMessageBox::No) {
            delete dlg;
            return -1;
        }
    }
    lastExportFilename = filename;

    bool exportAll = !dlg->isExportSelected();
    if (!exportAll) {
      w = wsel;
      h = hsel;
    }

    if (!dlg->isOriginalSize()) {
      auto size_coef = static_cast<double>(dlg->Xpixels()) / static_cast<double>(w);
      w = static_cast<int>(std::round(w * size_coef));
      h = static_cast<int>(std::round(h * size_coef));
    }

    if (dlg->isValidFilename()) {
      if (!dlg->isSvg()) {
        QImage* img;

        switch (dlg->getImgFormat()) {
          case ExportDialog::Coloured : 
            img = new QImage(w,h,QImage::Format_RGB888);
            break;
          case ExportDialog::Grayscale :
            img = new QImage(w,h,QImage::Format_Grayscale8);
            break;
          case ExportDialog::Monochrome : 
            img = new QImage(w,h,QImage::Format_Mono);
            break;
          default : 
            break;
        }

        QPainter* p = new QPainter(img);
        p->fillRect(0, 0, w, h, Qt::white);
        sch->print(nullptr, p, exportAll, true, QMargins{border, border, border, border});
        img->save(filename);

        delete p;
        delete img;
      } 
      else {
        QSvgGenerator* svgwriter = new QSvgGenerator();

        if (dlg->needsInkscape()) {
          svgwriter->setFileName(filename+".tmp.svg");
        } else {
          svgwriter->setFileName(filename);
        }

        //svgwriter->setSize(QSize(1.12*w,1.1*h));
        svgwriter->setSize(QSize(w,h));
        QPainter *p = new QPainter(svgwriter);
        p->fillRect(0, 0, svgwriter->size().width(), svgwriter->size().height(), Qt::white);
        sch->print(nullptr, p, exportAll, true, QMargins{border, border, border, border});

        delete p;
        delete svgwriter;

        if (dlg->needsInkscape()) {
            QString cmd = "inkscape";
            QStringList args;
            args<<"-z"<<"-D";
            QString stmp = "--file=" + filename+".tmp.svg";
            args<<stmp;

            if (dlg->isPdf_Tex()) {
                QString tmp = filename;
                tmp.chop(4);
                stmp = "--export-pdf="+ tmp + " --export-latex";
                args<<stmp;
            }

            if (dlg->isPdf()) {
                stmp = "--export-pdf=" + filename;
                args<<stmp;
            }

            if (dlg->isEps()) {
                stmp = "--export-eps=" + filename;
                args<<stmp;
            }

            int result = QProcess::execute(cmd,args);

            if (result!=0) {
                QMessageBox::critical(0, QObject::tr("Export to image"), 
                    QObject::tr("Inkscape start error!"), QMessageBox::Ok);
            }
            QFile::remove(filename+".tmp.svg");
        }
      }

      if (QFile::exists(filename)) {
        //QMessageBox::information(0, QObject::tr("Export to image"),
        //    QObject::tr("Successfully exported"), QMessageBox::Ok);
	status = 0;
      } 
      else {
        QMessageBox::information(0, QObject::tr("Export to image"),
            QObject::tr("Disk write error!"), QMessageBox::Ok);
	status = -1;
      }
    } else {
        QMessageBox::critical(0, QObject::tr("Export to image"), 
            QObject::tr("Unsupported format of graphics file. \n"
            "Use PNG, JPEG or SVG graphics!"), QMessageBox::Ok);
	status = -1;
    }
  }
  delete dlg;
  return status;
}
