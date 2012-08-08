/***************************************************************************
                               textdoc.cpp
                              -------------
    begin                : Sat Mar 11 2006
    copyright            : (C) 2006 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <qlabel.h>
#include <qaction.h>
#include <qpixmap.h>
#include <qprinter.h>
#include <qfileinfo.h>
#include <qtabwidget.h>
#include <qmessagebox.h>
#include <qpaintdevicemetrics.h>
#include <qfont.h>
#include <qpopupmenu.h>
#include <qsyntaxhighlighter.h>

#include "main.h"
#include "qucs.h"
#include "textdoc.h"
#include "syntax.h"
#include "components/vhdlfile.h"
#include "components/verilogfile.h"
#include "components/vafile.h"

TextDoc::TextDoc(QucsApp *App_, const QString& Name_) : QucsDoc(App_, Name_)
{
  TextFont = QFont("Courier New");
  TextFont.setPointSize(QucsSettings.font.pointSize()-1);
  TextFont.setStyleHint(QFont::Courier);
  TextFont.setFixedPitch(true);
  setFont(TextFont);
  setCurrentFont(TextFont);

  simulation = true;
  Library = "";
  Libraries = "";
  SetChanged = false;
  devtype = DEV_DEF;

  tmpPosX = tmpPosY = 1;  // set to 1 to trigger line highlighting
  Scale = (float)TextFont.pointSize();
  setUndoDepth(QucsSettings.maxUndo);
  setLanguage (Name_);
  QFileInfo Info (Name_);
  
  if(App) {
    if(Name_.isEmpty())
      App->DocumentTab->addTab(this, QPixmap(empty_xpm),
                            QObject::tr("untitled"));
    else
      App->DocumentTab->addTab(this, QPixmap(empty_xpm),
                            Info.fileName());
    // calls indirectly "becomeCurrent"
    App->DocumentTab->setCurrentPage(App->DocumentTab->indexOf(this));
    viewport()->setFocus();

    setWordWrap(QTextEdit::NoWrap);
    setPaletteBackgroundColor(QucsSettings.BGColor);
    connect(this, SIGNAL(textChanged()), SLOT(slotSetChanged()));
    connect(this, SIGNAL(cursorPositionChanged(int, int)),
            SLOT(slotCursorPosChanged(int, int)));

    syntaxHighlight = new SyntaxHighlighter(this);
    syntaxHighlight->setLanguage (language);
  }
}

TextDoc::~TextDoc()
{
  if(App) {
    delete syntaxHighlight;
    App->DocumentTab->removePage(this);  // delete tab in TabBar
  }
}

// ---------------------------------------------------
void TextDoc::setLanguage (const QString& FileName)
{
  QFileInfo Info (FileName);
  QString ext = Info.extension (false);
  if (ext == "vhd" || ext == "vhdl")
    setLanguage (LANG_VHDL);
  else if (ext == "v")
    setLanguage (LANG_VERILOG);
  else if (ext == "va")
    setLanguage (LANG_VERILOGA);
  else if (ext == "m" || ext == "oct")
    setLanguage (LANG_OCTAVE);
  else
    setLanguage (LANG_NONE);
}

// ---------------------------------------------------
void TextDoc::setLanguage (int lang)
{
  language = lang;
}

// ---------------------------------------------------
bool TextDoc::saveSettings (void)
{
  QFile file (DocName + ".cfg");
  if (!file.open (IO_WriteOnly))
    return false;

  QTextStream stream (&file);
  stream << "Textfile settings file, Qucs " PACKAGE_VERSION "\n"
	 << "Simulation=" << simulation << "\n"
	 << "Duration=" << SimTime << "\n"
	 << "Module=" << (!simulation) << "\n"
	 << "Library=" << Library << "\n"
	 << "Libraries=" << Libraries << "\n"
	 << "ShortDesc=" << ShortDesc << "\n"
	 << "LongDesc=" << LongDesc << "\n"
	 << "Icon=" << Icon << "\n"
	 << "Recreate=" << recreate << "\n"
	 << "DeviceType=" << devtype << "\n";

  file.close ();
  SetChanged = false;
  return true;
}

// ---------------------------------------------------
bool TextDoc::loadSettings (void)
{
  QFile file (DocName + ".cfg");
  if (!file.open (IO_ReadOnly))
    return false;

  QTextStream stream (&file);
  QString Line, Setting;

  bool ok;
  while (!stream.atEnd ()) {
    Line = stream.readLine ();
    Setting = Line.section ('=', 0, 0);
    Line = Line.section ('=', 1).stripWhiteSpace ();
    if (Setting == "Simulation") {
      simulation = Line.toInt (&ok);
    } else if (Setting == "Duration") {
      SimTime = Line;
    } else if (Setting == "Module") {
    } else if (Setting == "Library") {
      Library = Line;
    } else if (Setting == "Libraries") {
      Libraries = Line;
    } else if (Setting == "ShortDesc") {
      ShortDesc = Line;
    } else if (Setting == "LongDesc") {
      LongDesc = Line;
    } else if (Setting == "Icon") {
      Icon = Line;
    } else if (Setting == "Recreate") {
      recreate = Line.toInt (&ok);
    } else if (Setting == "DeviceType") {
      devtype = Line.toInt (&ok);
    }
  }

  file.close ();
  return true;
}

// ---------------------------------------------------
void TextDoc::setName (const QString& Name_)
{
  DocName = Name_;
  setLanguage (DocName);

  QFileInfo Info (DocName);
  if (App)
    App->DocumentTab->setTabLabel (this, Info.fileName ());

  DataSet = Info.baseName (true) + ".dat";
  DataDisplay = Info.baseName (true) + ".dpl";
  if(Info.extension(false) == "m" || Info.extension(false) == "oct")
    SimTime = "1";
}

// ---------------------------------------------------
void TextDoc::becomeCurrent (bool)
{
  int x, y;
  getCursorPosition (&x, &y);
  slotCursorPosChanged (x, y);
  viewport()->setFocus ();

  if (isUndoAvailable ())
    App->undo->setEnabled (true);
  else
    App->undo->setEnabled (false);
  if (isRedoAvailable ())
    App->redo->setEnabled (true);
  else
    App->redo->setEnabled (false);

  // update appropriate menu entries
  App->symEdit->setMenuText (tr("Edit Text Symbol"));
  App->symEdit->setStatusTip (tr("Edits the symbol for this text document"));
  App->symEdit->setWhatsThis (
	tr("Edit Text Symbol\n\nEdits the symbol for this text document"));

  if (language == LANG_VHDL) {
    App->insEntity->setMenuText (tr("VHDL entity"));
    App->insEntity->setStatusTip (tr("Inserts skeleton of VHDL entity"));
    App->insEntity->setWhatsThis (
	tr("VHDL entity\n\nInserts the skeleton of a VHDL entity"));
  }
  else if (language == LANG_VERILOG || language == LANG_VERILOGA) {
    App->insEntity->setMenuText (tr("Verilog module"));
    App->insEntity->setStatusTip (tr("Inserts skeleton of Verilog module"));
    App->insEntity->setWhatsThis (
	tr("Verilog module\n\nInserts the skeleton of a Verilog module"));
  }
  else if (language == LANG_OCTAVE) {
    App->insEntity->setMenuText (tr("Octave function"));
    App->insEntity->setStatusTip (tr("Inserts skeleton of Octave function"));
    App->insEntity->setWhatsThis (
	tr("Octave function\n\nInserts the skeleton of a Octave function"));
  }
  App->simulate->setEnabled (true);
  App->editActivate->setEnabled (true);
}

// ---------------------------------------------------
void TextDoc::slotCursorPosChanged(int x, int y)
{
  if(tmpPosX > x)
    clearParagraphBackground(tmpPosX);
  else
    for(int z=tmpPosX; z<x; z++)
      clearParagraphBackground(z);
  if(tmpPosX != x)
    setParagraphBackgroundColor(x, QColor(240, 240, 255));
  App->printCursorPosition(x+1, y+1);
  tmpPosX = x;
  tmpPosY = y;
}

// ---------------------------------------------------
void TextDoc::slotSetChanged()
{
  if((isModified() && !DocChanged) || SetChanged) {
    App->DocumentTab->setTabIconSet(this, QPixmap(smallsave_xpm));
    DocChanged = true;
  }
  else if((!isModified() && DocChanged)) {
    App->DocumentTab->setTabIconSet(this, QPixmap(empty_xpm));
    DocChanged = false;
  }

  App->undo->setEnabled(isUndoAvailable());
  App->redo->setEnabled(isRedoAvailable());
}

// ---------------------------------------------------
QPopupMenu *TextDoc::createPopupMenu( const QPoint &pos )
{
   QPopupMenu *popup = QTextEdit::createPopupMenu( pos );
   if (language != LANG_OCTAVE) {
     App->fileSettings->addTo(popup);
   }
   return popup;
}

// ---------------------------------------------------
bool TextDoc::load ()
{
  QFile file (DocName);
  if (!file.open (IO_ReadOnly))
    return false;
  setLanguage (DocName);

  QTextStream stream (&file);
  setText (stream.read ());
  setModified (false);
  slotSetChanged ();
  file.close ();
  lastSaved = QDateTime::currentDateTime ();

  loadSettings ();
  SimOpenDpl = simulation ? true : false;
  return true;
}

// ---------------------------------------------------
int TextDoc::save ()
{
  saveSettings ();

  QFile file (DocName);
  if (!file.open (IO_WriteOnly))
    return -1;
  setLanguage (DocName);

  QTextStream stream (&file);
  stream << text ();
  setModified (false);
  slotSetChanged ();
  file.close ();

  QFileInfo Info (DocName);
  lastSaved = Info.lastModified ();
  return 0;
}

// -----------------------------------------------------------
void TextDoc::print(QPrinter *Printer, QPainter *Painter, bool printAll, bool)
{
  Painter->setFont(TextFont);

  sync();   // formatting whole text

  QPaintDeviceMetrics smetrics(QPainter(this).device());
  QPaintDeviceMetrics pmetrics(Painter->device());
  int margin  = 54;    // margin at each side (unit is point)
  int marginX = margin * pmetrics.logicalDpiX() / smetrics.logicalDpiX();
  int marginY = margin * pmetrics.logicalDpiY() / smetrics.logicalDpiY();
  QRect printArea(
        marginX, marginY, pmetrics.width() - 2*marginX,
        pmetrics.height() - 2*marginY - Painter->fontMetrics().lineSpacing());

  int linesPerPage = printArea.height() / Painter->fontMetrics().lineSpacing();

  int PageCount, PageNo = 1;
  QString s, printText;
  if(printAll) {
    printText = text();
    PageCount = paragraphs();
  }
  else {
    int line1, col1, line2, col2;
    printText = selectedText();
    getSelection(&line1, &col1, &line2, &col2);
    if(line1 < 0) {
      QMessageBox::critical(this, tr("Error"), tr("There is no selection!"));
      return;
    }
    PageCount = line2 - line1 + 1;
  }
  PageCount = (PageCount + linesPerPage - 1) / linesPerPage;
  
  for(;;) {
    if(Printer->aborted())
      break;
    Painter->drawText(printArea, 0, printText.section('\n', 0, linesPerPage-1));
    printText = printText.section('\n', linesPerPage);

    s = tr("Page %1 of %2").arg(PageNo).arg(PageCount);
    Painter->drawText(printArea.right() - Painter->fontMetrics().width(s),
               printArea.bottom() + Painter->fontMetrics().lineSpacing(), s);
    if(printText.isEmpty())
      break;
    Printer->newPage();
    PageNo++;
  }
}

// ---------------------------------------------------
float TextDoc::zoomBy(float s)
{
  if(s < 1.0f) s = -1.0f/s;
  Scale += s;
  if(Scale > 40.0f) Scale = 40.0f;
  if(Scale <  4.0f) Scale =  4.0f;
  zoomTo((int)(Scale+0.5f));
  return Scale;
}

// ---------------------------------------------------
void TextDoc::showAll()
{
  sync();
  Scale *= float(visibleHeight()) / float(contentsHeight());
  if(Scale > 40.0f) Scale = 40.0f;
  if(Scale <  4.0f) Scale =  4.0f;
  zoomTo((int)(Scale+0.5f));
}

// ---------------------------------------------------
void TextDoc::showNoZoom()
{
  Scale = (float)TextFont.pointSize();
  zoomTo(TextFont.pointSize());
}

// ---------------------------------------------------
bool TextDoc::loadSimulationTime(QString& Time)
{
  if(!SimTime.isEmpty()) {
    Time = SimTime;
    return true;
  }

  return false;
}

// ---------------------------------------------------
void TextDoc::commentSelected ()
{
  QString s = selectedText ();
  if (s.isEmpty ())
    return;

  // use comment string indicator depending on language
  QString co;
  int cl;
  switch (language) {
  case LANG_VHDL: 
    co = "--"; cl = 2;
    break;
  case LANG_VERILOG:
  case LANG_VERILOGA:
    co = "//"; cl = 2;
    break;
  case LANG_OCTAVE:
    co = "%"; cl = 1;
    break;
  default:
    co = ""; cl = 0;
    break;
  }

  if (s.left (cl) == co)
    s.remove (0, cl);
  else
    s = co + s;

  for (int i = s.length () - cl; i >= 0; i--)
    if (s.at (i) == '\n') {
      if (s.mid (i+1, cl) == co)
        s.remove(i+1, cl);
      else
        s.insert (i+1, co);
    }
  insert (s);
}

// ---------------------------------------------------
void TextDoc::insertSkeleton ()
{
  if (language == LANG_VHDL)
    insert ("entity  is\n  port ( : in bit);\nend;\n"
	    "architecture  of  is\n  signal : bit;\nbegin\n\nend;\n\n");
  else if (language == LANG_VERILOG)
    insert ("module  ( );\ninput ;\noutput ;\nbegin\n\nend\n"
	    "endmodule\n\n");
  else if (language == LANG_OCTAVE)
    insert ("function  =  ( )\n"
	    "endfunction\n\n");
}

// ---------------------------------------------------
QString TextDoc::getModuleName (void)
{
  switch (language) {
  case LANG_VHDL:
    {
      VHDL_File_Info VInfo (text ());
      return VInfo.EntityName;
    }
  case LANG_VERILOG:
    {
      Verilog_File_Info VInfo (text ());
      return VInfo.ModuleName;
    }
  case LANG_VERILOGA:
    {
      VerilogA_File_Info VInfo (text ());
      return VInfo.ModuleName;
    }
  case LANG_OCTAVE:
    {
      QFileInfo Info (DocName);
      return Info.baseName (true);
    }
  default:
    return "";
  }
}
