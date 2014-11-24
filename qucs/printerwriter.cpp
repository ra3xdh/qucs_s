/*
 * printerwriter.cpp - implementation of writer to printer
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

#include "printerwriter.h"
#include "schematic.h"
#include "textdoc.h"

#include <QPrinter>
#include <QPainter>
#include <QPrintDialog>

PrinterWriter::PrinterWriter()
{
  //default setting
  Printer = new QPrinter(QPrinter::HighResolution);
  Printer->setOptionEnabled(QPrinter::PrintSelection, true);
  Printer->setOptionEnabled(QPrinter::PrintPageRange, false);
  Printer->setOptionEnabled(QPrinter::PrintToFile, true);
  Printer->setColorMode(QPrinter::Color);
  Printer->setFullPage(true);

  fitToPage = false;
}

PrinterWriter::~PrinterWriter()
{
  delete Printer;
}

void
PrinterWriter::print(QWidget *doc)
{
  QPrintDialog *dialog = new QPrintDialog(Printer, 0);
  dialog->setWindowTitle(QObject::tr("Print Document"));
  dialog->addEnabledOption(QAbstractPrintDialog::PrintSelection);


  if (doc->inherits("QPlainTextEdit"))
  {
    if (dialog->exec() == QDialog::Accepted) {
       static_cast<QPlainTextEdit *>(doc)->print(Printer);
    }
  }
  else {
    Printer->setOrientation(QPrinter::Landscape);

    if (dialog->exec() == QDialog::Accepted)
    {
      QPainter Painter(Printer);
      if(!Painter.device()) {     // valid device available ?
        return;
      }
      for (int z = Printer->numCopies(); z > 0; --z) {
        if (Printer->aborted()) {
          break;
        }

        static_cast<Schematic *>(doc)->print(Printer, &Painter,
                Printer->printRange() == QPrinter::AllPages, fitToPage);
        if (z > 1 && !Printer->newPage()) {
          return;
        }
      }
    }
  }
  delete dialog;
}
