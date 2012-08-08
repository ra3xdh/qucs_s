/***************************************************************************
                               qucs_init.cpp
                              ---------------
    begin                : Sat May 1 2004
    copyright            : (C) 2004 by Michael Margraf
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

#include <qaction.h>
#include <qaccel.h>
#include <qmenubar.h>
#include <qstatusbar.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qimage.h>
#include <qwhatsthis.h>
#include <qmessagebox.h>

#include "main.h"
#include "qucs.h"
#include "dialogs/vtabbeddockwidget.h"
#include "octave_window.h"

// ----------------------------------------------------------
// initializes all QActions of the application
void QucsApp::initActions()
{
  // to connect more than one key to an action
  mainAccel = new QAccel(this);

  activeAction = 0;   // no active action

  // note: first argument of QAction() for backward compatibility Qt < 3.2

  fileNew = new QAction(tr("New"),
		QIconSet(QImage(QucsSettings.BitmapDir + "filenew.png")),
		tr("&New"), CTRL+Key_N, this);
  fileNew->setStatusTip(tr("Creates a new document"));
  fileNew->setWhatsThis(
	        tr("New\n\nCreates a new schematic or data display document"));
  connect(fileNew, SIGNAL(activated()), SLOT(slotFileNew()));

  textNew = new QAction(tr("New Text"),
		QIconSet(QImage(QucsSettings.BitmapDir + "textnew.png")),
		tr("New &Text"), CTRL+SHIFT+Key_V, this);
  textNew->setStatusTip(tr("Creates a new text document"));
  textNew->setWhatsThis(tr("New Text\n\nCreates a new text document"));
  connect(textNew, SIGNAL(activated()), SLOT(slotTextNew()));

  fileOpen = new QAction(tr("Open"),
		QIconSet(QImage(QucsSettings.BitmapDir + "fileopen.png")),
		tr("&Open..."), CTRL+Key_O, this);
  fileOpen->setStatusTip(tr("Opens an existing document"));
  fileOpen->setWhatsThis(tr("Open File\n\nOpens an existing document"));
  connect(fileOpen, SIGNAL(activated()), SLOT(slotFileOpen()));

  fileSave = new QAction(tr("Save"),
		QIconSet(QImage(QucsSettings.BitmapDir + "filesave.png")),
		tr("&Save"), CTRL+Key_S, this);
  fileSave->setStatusTip(tr("Saves the current document"));
  fileSave->setWhatsThis(tr("Save File\n\nSaves the current document"));
  connect(fileSave, SIGNAL(activated()), SLOT(slotFileSave()));

  fileSaveAs = new QAction("Save as...",
		tr("Save as..."), CTRL+Key_Minus, this);
  fileSaveAs->setStatusTip(
	tr("Saves the current document under a new filename"));
  fileSaveAs->setWhatsThis(
	tr("Save As\n\nSaves the current document under a new filename"));
  connect(fileSaveAs, SIGNAL(activated()), SLOT(slotFileSaveAs()));

  fileSaveAll = new QAction(tr("Save All"),
		QIconSet(QImage(QucsSettings.BitmapDir + "filesaveall.png")),
		tr("Save &All"), CTRL+Key_Plus, this);
  fileSaveAll->setStatusTip(tr("Saves all open documents"));
  fileSaveAll->setWhatsThis(tr("Save All Files\n\nSaves all open documents"));
  connect(fileSaveAll, SIGNAL(activated()), SLOT(slotFileSaveAll()));

  fileClose = new QAction(tr("Close"),
		QIconSet(QImage(QucsSettings.BitmapDir + "fileclose.png")),
		tr("&Close"), CTRL+Key_W, this);
  fileClose->setStatusTip(tr("Closes the current document"));
  fileClose->setWhatsThis(tr("Close File\n\nCloses the current document"));
  connect(fileClose, SIGNAL(activated()), SLOT(slotFileClose()));

  symEdit = new QAction("Edit Circuit Symbol",
			tr("&Edit Circuit Symbol"), Key_F9, this);
  symEdit->setStatusTip(tr("Edits the symbol for this schematic"));
  symEdit->setWhatsThis(
	tr("Edit Circuit Symbol\n\nEdits the symbol for this schematic"));
  connect(symEdit, SIGNAL(activated()), SLOT(slotSymbolEdit()));

  fileSettings = new QAction("Document Settings...",
		tr("&Document Settings..."), CTRL+Key_Period, this);
  fileSettings->setStatusTip(tr("Document Settings"));
  fileSettings->setWhatsThis(tr("Settings\n\nSets properties of the file"));
  connect(fileSettings, SIGNAL(activated()), SLOT(slotFileSettings()));

  filePrint = new QAction(tr("Print..."),
		QIconSet(QImage(QucsSettings.BitmapDir + "fileprint.png")),
		tr("&Print..."), CTRL+Key_P, this);
  filePrint->setStatusTip(tr("Prints the current document"));
  filePrint->setWhatsThis(tr("Print File\n\nPrints the current document"));
  connect(filePrint, SIGNAL(activated()), SLOT(slotFilePrint()));

  filePrintFit = new QAction("Print Fit to Page...",
		tr("Print Fit to Page..."), CTRL+SHIFT+Key_P, this);
  filePrintFit->setStatusTip(tr("Print Fit to Page"));
  filePrintFit->setWhatsThis(
	tr("Print Fit to Page\n\n"
	   "Print and fit content to the page size"));
  connect(filePrintFit, SIGNAL(activated()), SLOT(slotFilePrintFit()));

  fileQuit = new QAction("Exit", tr("E&xit"), CTRL+Key_Q, this);
  fileQuit->setStatusTip(tr("Quits the application"));
  fileQuit->setWhatsThis(tr("Exit\n\nQuits the application"));
  connect(fileQuit, SIGNAL(activated()), SLOT(slotFileQuit()));

  applSettings = new QAction("Application Settings...",
		tr("Application Settings..."), CTRL+Key_Comma, this);
  applSettings->setStatusTip(tr("Application Settings"));
  applSettings->setWhatsThis(
	tr("Qucs Settings\n\nSets properties of the application"));
  connect(applSettings, SIGNAL(activated()), SLOT(slotApplSettings()));

  alignTop = new QAction("Align top", tr("Align top"), CTRL+Key_T, this);
  alignTop->setStatusTip(tr("Align top selected elements"));
  alignTop->setWhatsThis(
	tr("Align top\n\nAlign selected elements to their upper edge"));
  connect(alignTop, SIGNAL(activated()), SLOT(slotAlignTop()));

  alignBottom = new QAction("Align bottom", tr("Align bottom"), 0, this);
  alignBottom->setStatusTip(tr("Align bottom selected elements"));
  alignBottom->setWhatsThis(
	tr("Align bottom\n\nAlign selected elements to their lower edge"));
  connect(alignBottom, SIGNAL(activated()), SLOT(slotAlignBottom()));

  alignLeft = new QAction("Align left", tr("Align left"), 0, this);
  alignLeft->setStatusTip(tr("Align left selected elements"));
  alignLeft->setWhatsThis(
	tr("Align left\n\nAlign selected elements to their left edge"));
  connect(alignLeft, SIGNAL(activated()), SLOT(slotAlignLeft()));

  alignRight = new QAction("Align right", tr("Align right"), 0, this);
  alignRight->setStatusTip(tr("Align right selected elements"));
  alignRight->setWhatsThis(
	tr("Align right\n\nAlign selected elements to their right edge"));
  connect(alignRight, SIGNAL(activated()), SLOT(slotAlignRight()));

  distrHor = new QAction("Distribute horizontally",
		tr("Distribute horizontally"), 0, this);
  distrHor->setStatusTip(tr("Distribute equally horizontally"));
  distrHor->setWhatsThis(
	tr("Distribute horizontally\n\n"
	   "Distribute horizontally selected elements"));
  connect(distrHor, SIGNAL(activated()), SLOT(slotDistribHoriz()));

  distrVert = new QAction("Distribute vertically",
		tr("Distribute vertically"), 0, this);
  distrVert->setStatusTip(tr("Distribute equally vertically"));
  distrVert->setWhatsThis(
	tr("Distribute vertically\n\n"
	   "Distribute vertically selected elements"));
  connect(distrVert, SIGNAL(activated()), SLOT(slotDistribVert()));

  centerHor = new QAction("Center horizontally",
	        tr("Center horizontally"), 0, this);
  centerHor->setStatusTip(tr("Center horizontally selected elements"));
  centerHor->setWhatsThis(
	tr("Center horizontally\n\nCenter horizontally selected elements"));
  connect(centerHor, SIGNAL(activated()), SLOT(slotCenterHorizontal()));

  centerVert = new QAction("Center vertically",
		tr("Center vertically"), 0, this);
  centerVert->setStatusTip(tr("Center vertically selected elements"));
  centerVert->setWhatsThis(
	tr("Center vertically\n\nCenter vertically selected elements"));
  connect(centerVert, SIGNAL(activated()), SLOT(slotCenterVertical()));

  onGrid = new QAction("Set on Grid", tr("Set on Grid"), CTRL+Key_U, this);
  onGrid->setStatusTip(tr("Sets selected elements on grid"));
  onGrid->setWhatsThis(
	tr("Set on Grid\n\nSets selected elements on grid"));
  onGrid->setToggleAction(true);
  connect(onGrid, SIGNAL(toggled(bool)), SLOT(slotOnGrid(bool)));

  moveText = new QAction("Move Component Text",
			 tr("Move Component Text"), CTRL+Key_K, this);
  moveText->setStatusTip(tr("Moves the property text of components"));
  moveText->setWhatsThis(
	tr("Move Component Text\n\nMoves the property text of components"));
  moveText->setToggleAction(true);
  connect(moveText, SIGNAL(toggled(bool)), SLOT(slotMoveText(bool)));

  changeProps = new QAction("Replace...", tr("Replace..."), Key_F7, this);
  changeProps->setStatusTip(tr("Replace component properties or VHDL code"));
  changeProps->setWhatsThis(
	tr("Replace\n\nChange component properties\nor\ntext in VHDL code"));
  connect(changeProps, SIGNAL(activated()), SLOT(slotChangeProps()));

  editCut = new QAction(tr("Cut"),
		QIconSet(QImage(QucsSettings.BitmapDir + "editcut.png")),
		tr("Cu&t"), CTRL+Key_X, this);
  editCut->setStatusTip(
	tr("Cuts out the selection and puts it into the clipboard"));
  editCut->setWhatsThis(
	tr("Cut\n\nCuts out the selection and puts it into the clipboard"));
  connect(editCut, SIGNAL(activated()), SLOT(slotEditCut()));

  editCopy = new QAction(tr("Copy"),
		QIconSet(QImage(QucsSettings.BitmapDir + "editcopy.png")),
		tr("&Copy"), CTRL+Key_C, this);
  editCopy->setStatusTip(
	tr("Copies the selection into the clipboard"));
  editCopy->setWhatsThis(
	tr("Copy\n\nCopies the selection into the clipboard"));
  connect(editCopy, SIGNAL(activated()), SLOT(slotEditCopy()));

  editPaste = new QAction(tr("Paste"),
		QIconSet(QImage(QucsSettings.BitmapDir + "editpaste.png")),
		tr("&Paste"), CTRL+Key_V, this);
  editPaste->setStatusTip(
	tr("Pastes the clipboard contents to the cursor position"));
  editPaste->setWhatsThis(
	tr("Paste\n\nPastes the clipboard contents to the cursor position"));
  editPaste->setToggleAction(true);
  connect(editPaste, SIGNAL(toggled(bool)), SLOT(slotEditPaste(bool)));

  editDelete = new QAction(tr("Delete"),
		QIconSet(QImage(QucsSettings.BitmapDir + "editdelete.png")),
		tr("&Delete"), Key_Delete, this);
  editDelete->setStatusTip(tr("Deletes the selected components"));
  editDelete->setWhatsThis(tr("Delete\n\nDeletes the selected components"));
  editDelete->setToggleAction(true);
  connect(editDelete, SIGNAL(toggled(bool)), SLOT(slotEditDelete(bool)));

  editFind = new QAction("Find...", tr("Find..."), CTRL+Key_F, this);
  editFind->setStatusTip(tr("Find a piece of text"));
  editFind->setWhatsThis(tr("Find\n\nSearches for a piece of text"));
  connect(editFind, SIGNAL(activated()), SLOT(slotEditFind()));

  editFindAgain = new QAction("Find Again", tr("Find Again"), Key_F3, this);
  editFindAgain->setStatusTip(tr("Find same text again"));
  editFindAgain->setWhatsThis(
                 tr("Find\n\nSearches for the same piece of text again"));
  connect(editFindAgain, SIGNAL(activated()), SLOT(slotEditFindAgain()));

  // to ease usage with notebooks, backspace can also be used to delete
  mainAccel->connectItem(mainAccel->insertItem(Key_Backspace),
                         editDelete, SLOT(toggle()) );

  // cursor left/right to move marker on a graph
  mainAccel->connectItem(mainAccel->insertItem(Key_Left),
			this, SLOT(slotCursorLeft()));
  mainAccel->connectItem(mainAccel->insertItem(Key_Right),
			this, SLOT(slotCursorRight()));
  mainAccel->connectItem(mainAccel->insertItem(Key_Up),
			this, SLOT(slotCursorUp()));
  mainAccel->connectItem(mainAccel->insertItem(Key_Down),
			this, SLOT(slotCursorDown()));
  mainAccel->connectItem(mainAccel->insertItem(Key_Tab),
			this, SLOT(slotNextTab()));

  undo = new QAction(tr("Undo"),
		QIconSet(QImage(QucsSettings.BitmapDir + "undo.png")),
		tr("&Undo"), CTRL+Key_Z, this);
  undo->setStatusTip(tr("Undoes the last command"));
  undo->setWhatsThis(tr("Undo\n\nMakes the last action undone"));
  connect(undo, SIGNAL(activated()), SLOT(slotEditUndo()));

  redo = new QAction(tr("Redo"),
		QIconSet(QImage(QucsSettings.BitmapDir + "redo.png")),
		tr("&Redo"), CTRL+Key_Y, this);
  redo->setStatusTip(tr("Redoes the last command"));
  redo->setWhatsThis(tr("Redo\n\nRepeats the last action once more"));
  connect(redo, SIGNAL(activated()), SLOT(slotEditRedo()));

  projNew = new QAction("New Project...",
			tr("&New Project..."), CTRL+SHIFT+Key_N, this);
  projNew->setStatusTip(tr("Creates a new project"));
  projNew->setWhatsThis(tr("New Project\n\nCreates a new project"));
  connect(projNew, SIGNAL(activated()), SLOT(slotProjNewButt()));

  projOpen = new QAction("Open Project...", tr("&Open Project..."),
		CTRL+SHIFT+Key_O, this);
  projOpen->setStatusTip(tr("Opens an existing project"));
  projOpen->setWhatsThis(tr("Open Project\n\nOpens an existing project"));
  connect(projOpen, SIGNAL(activated()), SLOT(slotMenuOpenProject()));

  projDel = new QAction("Delete Project...",
			tr("&Delete Project..."), CTRL+SHIFT+Key_D, this);
  projDel->setStatusTip(tr("Deletes an existing project"));
  projDel->setWhatsThis(tr("Delete Project\n\nDeletes an existing project"));
  connect(projDel, SIGNAL(activated()), SLOT(slotMenuDelProject()));

  projClose = new QAction("Close Project",
			  tr("&Close Project"), CTRL+SHIFT+Key_W, this);
  projClose->setStatusTip(tr("Closes the current project"));
  projClose->setWhatsThis(tr("Close Project\n\nCloses the current project"));
  connect(projClose, SIGNAL(activated()), SLOT(slotMenuCloseProject()));

  addToProj = new QAction("Add Files to Project...",
		 tr("&Add Files to Project..."), CTRL+SHIFT+Key_A, this);
  addToProj->setStatusTip(tr("Copies files to project directory"));
  addToProj->setWhatsThis(
	tr("Add Files to Project\n\nCopies files to project directory"));
  connect(addToProj, SIGNAL(activated()), SLOT(slotAddToProject()));

  createLib = new QAction("Create Library...",
		 tr("Create &Library..."), CTRL+SHIFT+Key_L, this);
  createLib->setStatusTip(tr("Create Library from Subcircuits"));
  createLib->setWhatsThis(
	tr("Create Library\n\nCreate Library from Subcircuits"));
  connect(createLib, SIGNAL(activated()), SLOT(slotCreateLib()));

  createPkg = new QAction("Create Package...",
		 tr("Create &Package..."), CTRL+SHIFT+Key_Z, this);
  createPkg->setStatusTip(tr("Create compressed Package from Projects"));
  createPkg->setWhatsThis(
    tr("Create Package\n\nCreate compressed Package from complete Projects"));
  connect(createPkg, SIGNAL(activated()), SLOT(slotCreatePackage()));

  extractPkg = new QAction("Extract Package...",
		 tr("E&xtract Package..."), CTRL+SHIFT+Key_X, this);
  extractPkg->setStatusTip(tr("Install Content of a Package"));
  extractPkg->setWhatsThis(
	tr("Extract Package\n\nInstall Content of a Package"));
  connect(extractPkg, SIGNAL(activated()), SLOT(slotExtractPackage()));

  importData = new QAction("Import/Export Data...",
		 tr("&Import/Export Data..."), CTRL+SHIFT+Key_I, this);
  importData->setStatusTip(tr("Convert data file"));
  importData->setWhatsThis(
	tr("Import/Export Data\n\nConvert data file to various file formats"));
  connect(importData, SIGNAL(activated()), SLOT(slotImportData()));

  graph2csv = new QAction("Export to CSV...",
		 tr("Export to &CSV..."), CTRL+SHIFT+Key_C, this);
  graph2csv->setStatusTip(tr("Convert graph data to CSV file"));
  graph2csv->setWhatsThis(
	tr("Export to CSV\n\nConvert graph data to CSV file"));
  connect(graph2csv, SIGNAL(activated()), SLOT(slotExportGraphAsCsv()));

  magAll = new QAction(tr("View All"),
                QIconSet(QImage(QucsSettings.BitmapDir + "viewmagfit.png")),
                tr("View All"), Key_0, this);
  magAll->setStatusTip(tr("Show the whole page"));
  magAll->setWhatsThis(tr("View All\n\nShows the whole page content"));
  connect(magAll, SIGNAL(activated()), SLOT(slotShowAll()));

  magOne = new QAction(tr("View 1:1"),
                QIconSet(QImage(QucsSettings.BitmapDir + "viewmag1.png")),
                tr("View 1:1"), Key_1, this);
  magOne->setStatusTip(tr("Views without magnification"));
  magOne->setWhatsThis(
	tr("View 1:1\n\nShows the page content without magnification"));
  connect(magOne, SIGNAL(activated()), SLOT(slotShowOne()));

  magPlus = new QAction(tr("Zoom in"),
		QIconSet(QImage(QucsSettings.BitmapDir + "viewmag+.png")),
		tr("Zoom in"), Key_Plus, this);
  magPlus->setStatusTip(tr("Zooms into the current view"));
  magPlus->setWhatsThis(tr("Zoom in\n\nZooms the current view"));
  magPlus->setToggleAction(true);
  connect(magPlus, SIGNAL(toggled(bool)), SLOT(slotZoomIn(bool)));

  magMinus = new QAction(tr("Zoom out"),
		QIconSet(QImage(QucsSettings.BitmapDir + "viewmag-.png")),
		tr("Zoom out"), Key_Minus, this);
  magMinus->setStatusTip(tr("Zooms out the current view"));
  magMinus->setWhatsThis(tr("Zoom out\n\nZooms out the current view"));
  connect(magMinus, SIGNAL(activated()), SLOT(slotZoomOut()));

  select = new QAction(tr("Select"),
		QIconSet(QImage(QucsSettings.BitmapDir + "pointer.png")),
		tr("Select"), Key_Escape, this);
  select->setStatusTip(tr("Activate select mode"));
  select->setWhatsThis(tr("Select\n\nActivates select mode"));
  select->setToggleAction(true);
  connect(select, SIGNAL(toggled(bool)), SLOT(slotSelect(bool)));

  selectAll = new QAction("Select All", tr("Select All"), CTRL+Key_A, this);
  selectAll->setStatusTip(tr("Selects all elements"));
  selectAll->setWhatsThis(
	tr("Select All\n\nSelects all elements of the document"));
  connect(selectAll, SIGNAL(activated()), SLOT(slotSelectAll()));

  selectMarker = new QAction("Select Markers", tr("Select Markers"), CTRL+SHIFT+Key_M, this);
  selectMarker->setStatusTip(tr("Selects all markers"));
  selectMarker->setWhatsThis(
	tr("Select Markers\n\nSelects all diagram markers of the document"));
  connect(selectMarker, SIGNAL(activated()), SLOT(slotSelectMarker()));

  editRotate = new QAction(tr("Rotate"),
		QIconSet(QImage(QucsSettings.BitmapDir + "rotate_ccw.png")),
		tr("Rotate"), CTRL+Key_R, this);
  editRotate->setStatusTip(tr("Rotates the selected component by 90°"));
  editRotate->setWhatsThis(
    tr("Rotate\n\nRotates the selected component by 90° counter-clockwise"));
  editRotate->setToggleAction(true);
  connect(editRotate, SIGNAL(toggled(bool)), SLOT(slotEditRotate(bool)));

  editMirror = new QAction(tr("Mirror about X Axis"),
		QIconSet(QImage(QucsSettings.BitmapDir + "mirror.png")),
		tr("Mirror about X Axis"), CTRL+Key_J, this);
  editMirror->setStatusTip(tr("Mirrors the selected item about X Axis"));
  editMirror->setWhatsThis(
	tr("Mirror about X Axis\n\nMirrors the selected item about X Axis"));
  editMirror->setToggleAction(true);
  connect(editMirror, SIGNAL(toggled(bool)), SLOT(slotEditMirrorX(bool)));

  editMirrorY = new QAction(tr("Mirror about Y Axis"),
		QIconSet(QImage(QucsSettings.BitmapDir + "mirrory.png")),
		tr("Mirror about Y Axis"), CTRL+Key_M, this);
  editMirrorY->setStatusTip(tr("Mirrors the selected item about Y Axis"));
  editMirrorY->setWhatsThis(
	tr("Mirror about Y Axis\n\nMirrors the selected item about Y Axis"));
  editMirrorY->setToggleAction(true);
  connect(editMirrorY, SIGNAL(toggled(bool)), SLOT(slotEditMirrorY(bool)));

  intoH = new QAction(tr("Go into Subcircuit"),
		QIconSet(QImage(QucsSettings.BitmapDir + "bottom.png")),
		tr("Go into Subcircuit"), CTRL+Key_I, this);
  intoH->setStatusTip(tr("Goes inside the selected subcircuit"));
  intoH->setWhatsThis(
	tr("Go into Subcircuit\n\nGoes inside the selected subcircuit"));
  connect(intoH, SIGNAL(activated()), SLOT(slotIntoHierarchy()));

  popH = new QAction(tr("Pop out"),
		QIconSet(QImage(QucsSettings.BitmapDir + "top.png")),
		tr("Pop out"), CTRL+Key_H, this);
  popH->setStatusTip(tr("Pop outside subcircuit"));
  popH->setWhatsThis(
	tr("Pop out\n\nGoes up one hierarchy level, i.e. leaves subcircuit"));
  connect(popH, SIGNAL(activated()), SLOT(slotPopHierarchy()));
  popH->setEnabled(false);  // only enabled if useful !!!!

  editActivate = new QAction(tr("Deactivate/Activate"),
		QIconSet(QImage(QucsSettings.BitmapDir + "deactiv.png")),
		tr("Deactivate/Activate"), CTRL+Key_D, this);
  editActivate->setStatusTip(tr("Deactivate/Activate selected components"));
  editActivate->setWhatsThis(
	tr("Deactivate/Activate\n\nDeactivate/Activate the selected components"));
  editActivate->setToggleAction(true);
  connect(editActivate, SIGNAL(toggled(bool)), SLOT(slotEditActivate(bool)));

  insEquation = new QAction(tr("Insert Equation"),
		QIconSet(QImage(QucsSettings.BitmapDir + "equation.png")),
		tr("Insert Equation"), CTRL+Key_Less, this);
  insEquation->setStatusTip(tr("Inserts an equation"));
  insEquation->setWhatsThis(
	tr("Insert Equation\n\nInserts a user defined equation"));
  insEquation->setToggleAction(true);
  connect(insEquation, SIGNAL(toggled(bool)), SLOT(slotInsertEquation(bool)));

  insGround = new QAction(tr("Insert Ground"),
		QIconSet(QImage(QucsSettings.BitmapDir + "ground.png")),
		tr("Insert Ground"), CTRL+Key_G, this);
  insGround->setStatusTip(tr("Inserts a ground symbol"));
  insGround->setWhatsThis(tr("Insert Ground\n\nInserts a ground symbol"));
  insGround->setToggleAction(true);
  connect(insGround, SIGNAL(toggled(bool)), SLOT(slotInsertGround(bool)));

  insPort = new QAction(tr("Insert Port"),
		QIconSet(QImage(QucsSettings.BitmapDir + "port.png")),
		tr("Insert Port"), 0, this);
  insPort->setStatusTip(tr("Inserts a port symbol"));
  insPort->setWhatsThis(tr("Insert Port\n\nInserts a port symbol"));
  insPort->setToggleAction(true);
  connect(insPort, SIGNAL(toggled(bool)), SLOT(slotInsertPort(bool)));

  insWire = new QAction(tr("Wire"),
		QIconSet(QImage(QucsSettings.BitmapDir + "wire.png")),
		tr("Wire"), CTRL+Key_E, this);
  insWire->setStatusTip(tr("Inserts a wire"));
  insWire->setWhatsThis(tr("Wire\n\nInserts a wire"));
  insWire->setToggleAction(true);
  connect(insWire, SIGNAL(toggled(bool)), SLOT(slotSetWire(bool)));

  insLabel = new QAction(tr("Wire Label"),
		QIconSet(QImage(QucsSettings.BitmapDir + "nodename.png")),
		tr("Wire Label"), CTRL+Key_L, this);
  insLabel->setStatusTip(tr("Inserts a wire or pin label"));
  insLabel->setWhatsThis(tr("Wire Label\n\nInserts a wire or pin label"));
  insLabel->setToggleAction(true);
  connect(insLabel, SIGNAL(toggled(bool)), SLOT(slotInsertLabel(bool)));

  insEntity = new QAction("VHDL entity",
			  tr("VHDL entity"), CTRL+Key_Space, this);
  insEntity->setStatusTip(tr("Inserts skeleton of VHDL entity"));
  insEntity->setWhatsThis(
	tr("VHDL entity\n\nInserts the skeleton of a VHDL entity"));
  connect(insEntity, SIGNAL(activated()), SLOT(slotInsertEntity()));

  callEditor = new QAction("Text Editor", tr("Text Editor"), CTRL+Key_1, this);
  callEditor->setStatusTip(tr("Starts the Qucs text editor"));
  callEditor->setWhatsThis(tr("Text editor\n\nStarts the Qucs text editor"));
  connect(callEditor, SIGNAL(activated()), SLOT(slotCallEditor()));

  callFilter = new QAction("Filter synthesis",
			   tr("Filter synthesis"), CTRL+Key_2, this);
  callFilter->setStatusTip(tr("Starts QucsFilter"));
  callFilter->setWhatsThis(tr("Filter synthesis\n\nStarts QucsFilter"));
  connect(callFilter, SIGNAL(activated()), SLOT(slotCallFilter()));

  callLine = new QAction("Line calculation",
			 tr("Line calculation"), CTRL+Key_3, this);
  callLine->setStatusTip(tr("Starts QucsTrans"));
  callLine->setWhatsThis(
		tr("Line calculation\n\nStarts transmission line calculator"));
  connect(callLine, SIGNAL(activated()), SLOT(slotCallLine()));

  callLib = new QAction("Component Library",
			tr("Component Library"), CTRL+Key_4, this);
  callLib->setStatusTip(tr("Starts QucsLib"));
  callLib->setWhatsThis(
		tr("Component Library\n\nStarts component library program"));
  connect(callLib, SIGNAL(activated()), SLOT(slotCallLibrary()));

  callMatch = new QAction("Matching Circuit",
			  tr("Matching Circuit"), CTRL+Key_5, this);
  callMatch->setStatusTip(tr("Creates Matching Circuit"));
  callMatch->setWhatsThis(
	tr("Matching Circuit\n\nDialog for Creating Matching Circuit"));
  connect(callMatch, SIGNAL(activated()), SLOT(slotCallMatch()));

  callAtt = new QAction("Attenuator synthesis",
			tr("Attenuator synthesis"), CTRL+Key_6, this);
  callAtt->setStatusTip(tr("Starts QucsAttenuator"));
  callAtt->setWhatsThis(
	tr("Attenuator synthesis\n\nStarts attenuator calculation program"));
  connect(callAtt, SIGNAL(activated()), SLOT(slotCallAtt()));

  simulate = new QAction(tr("Simulate"),
		QIconSet(QImage(QucsSettings.BitmapDir + "gear.png")),
		tr("Simulate"), Key_F2, this);
  simulate->setStatusTip(tr("Simulates the current schematic"));
  simulate->setWhatsThis(tr("Simulate\n\nSimulates the current schematic"));
  connect(simulate, SIGNAL(activated()), SLOT(slotSimulate()));

  dpl_sch = new QAction(tr("View Data Display/Schematic"),
		QIconSet(QImage(QucsSettings.BitmapDir + "rebuild.png")),
		tr("View Data Display/Schematic"), Key_F4, this);
  dpl_sch->setStatusTip(tr("Changes to data display or schematic page"));
  dpl_sch->setWhatsThis(
	tr("View Data Display/Schematic\n\n")+
	tr("Changes to data display or schematic page"));
  connect(dpl_sch, SIGNAL(activated()), SLOT(slotToPage()));

  dcbias = new QAction("Calculate DC bias",
		       tr("Calculate DC bias"), Key_F8, this);
  dcbias->setStatusTip(tr("Calculates DC bias and shows it"));
  dcbias->setWhatsThis(
	tr("Calculate DC bias\n\nCalculates DC bias and shows it"));
  connect(dcbias, SIGNAL(activated()), SLOT(slotDCbias()));

  setMarker = new QAction(tr("Set Marker on Graph"),
		QIconSet(QImage(QucsSettings.BitmapDir + "marker.png")),
		tr("Set Marker on Graph"), CTRL+Key_B, this);
  setMarker->setStatusTip(tr("Sets a marker on a diagram's graph"));
  setMarker->setWhatsThis(
	tr("Set Marker\n\nSets a marker on a diagram's graph"));
  setMarker->setToggleAction(true);
  connect(setMarker, SIGNAL(toggled(bool)), SLOT(slotSetMarker(bool)));

  showMsg = new QAction("Show Last Messages",
			tr("Show Last Messages"), Key_F5, this);
  showMsg->setStatusTip(tr("Shows last simulation messages"));
  showMsg->setWhatsThis(
        tr("Show Last Messages\n\nShows the messages of the last simulation"));
  connect(showMsg, SIGNAL(activated()), SLOT(slotShowLastMsg()));

  showNet = new QAction("Show Last Netlist",
			tr("Show Last Netlist"), Key_F6, this);
  showNet->setStatusTip(tr("Shows last simulation netlist"));
  showNet->setWhatsThis(
	tr("Show Last Netlist\n\nShows the netlist of the last simulation"));
  connect(showNet, SIGNAL(activated()), SLOT(slotShowLastNetlist()));

  viewToolBar = new QAction("Toolbar", tr("Tool&bar"), 0, this);
  viewToolBar->setToggleAction(true);
  viewToolBar->setStatusTip(tr("Enables/disables the toolbar"));
  viewToolBar->setWhatsThis(tr("Toolbar\n\nEnables/disables the toolbar"));
  connect(viewToolBar, SIGNAL(toggled(bool)), SLOT(slotViewToolBar(bool)));

  viewStatusBar = new QAction("Statusbar", tr("&Statusbar"), 0, this);
  viewStatusBar->setToggleAction(true);
  viewStatusBar->setStatusTip(tr("Enables/disables the statusbar"));
  viewStatusBar->setWhatsThis(
	tr("Statusbar\n\nEnables/disables the statusbar"));
  connect(viewStatusBar, SIGNAL(toggled(bool)), SLOT(slotViewStatusBar(bool)));

  viewBrowseDock = new QAction("Dock Window", tr("&Dock Window"), 0, this);
  viewBrowseDock->setToggleAction(true);
  viewBrowseDock->setStatusTip(tr("Enables/disables the browse dock window"));
  viewBrowseDock->setWhatsThis(
	tr("Browse Window\n\nEnables/disables the browse dock window"));
  connect(viewBrowseDock, SIGNAL(toggled(bool)), SLOT(slotViewBrowseDock(bool)));

  viewOctaveDock = new QAction(tr("&Octave Window"), 0, this, 0);
  viewOctaveDock->setToggleAction(true);
  viewOctaveDock->setStatusTip(tr("Shows/hides the Octave dock window"));
  viewOctaveDock->setWhatsThis(
      tr("Octave Window\n\nShows/hides the Octave dock window"));
  connect(viewOctaveDock, SIGNAL(toggled(bool)), SLOT(slotViewOctaveDock(bool)));

  helpIndex = new QAction("Help Index...", tr("Help Index..."), Key_F1, this);
  helpIndex->setStatusTip(tr("Index of Qucs Help"));
  helpIndex->setWhatsThis(tr("Help Index\n\nIndex of intern Qucs help"));
  connect(helpIndex, SIGNAL(activated()), SLOT(slotHelpIndex()));

  helpGetStart = new QAction("Getting Started...",
			     tr("Getting Started..."), 0, this);
  helpGetStart->setStatusTip(tr("Getting Started with Qucs"));
  helpGetStart->setWhatsThis(
	tr("Getting Started\n\nShort introduction into Qucs"));
  connect(helpGetStart, SIGNAL(activated()), SLOT(slotGettingStarted()));

  helpAboutApp = new QAction("About Qucs...", tr("&About Qucs..."), 0, this);
  helpAboutApp->setStatusTip(tr("About the application"));
  helpAboutApp->setWhatsThis(tr("About\n\nAbout the application"));
  connect(helpAboutApp, SIGNAL(activated()), SLOT(slotHelpAbout()));

  helpAboutQt = new QAction("About Qt...", tr("About Qt..."), 0, this);
  helpAboutQt->setStatusTip(tr("About Qt"));
  helpAboutQt->setWhatsThis(tr("About Qt\n\nAbout Qt by Trolltech"));
  connect(helpAboutQt, SIGNAL(activated()), SLOT(slotHelpAboutQt()));
}

// ----------------------------------------------------------
void QucsApp::initMenuBar()
{
  fileMenu = new QPopupMenu();  // menuBar entry fileMenu
  fileNew->addTo(fileMenu);
  textNew->addTo(fileMenu);
  fileOpen->addTo(fileMenu);
  fileClose->addTo(fileMenu);
  fileMenu->insertSeparator();
  fileSave->addTo(fileMenu);
  fileSaveAll->addTo(fileMenu);
  fileSaveAs->addTo(fileMenu);
  filePrint->addTo(fileMenu);
  filePrintFit->addTo(fileMenu);
  fileMenu->insertSeparator();
  fileSettings->addTo(fileMenu);
  symEdit->addTo(fileMenu);
  fileMenu->insertSeparator();
  applSettings->addTo(fileMenu);
  fileMenu->insertSeparator();
  fileQuit->addTo(fileMenu);

  editMenu = new QPopupMenu();  // menuBar entry editMenu
  undo->addTo(editMenu);
  redo->addTo(editMenu);
  editMenu->insertSeparator();
  editCut->addTo(editMenu);
  editCopy->addTo(editMenu);
  editPaste->addTo(editMenu);
  editDelete->addTo(editMenu);
  editMenu->insertSeparator();
  select->addTo(editMenu);
  selectAll->addTo(editMenu);
  selectMarker->addTo(editMenu);
  editFind->addTo(editMenu);
  editFindAgain->addTo(editMenu);
  changeProps->addTo(editMenu);
  editRotate->addTo(editMenu);
  editMirror->addTo(editMenu);
  editMirrorY->addTo(editMenu);
  editActivate->addTo(editMenu);
  editMenu->insertSeparator();
  intoH->addTo(editMenu);
  popH->addTo(editMenu);

  alignMenu = new QPopupMenu();  // menuBar entry alignMenu
  moveText->addTo(alignMenu);
  onGrid->addTo(alignMenu);
  alignMenu->insertSeparator();
  centerHor->addTo(alignMenu);
  centerVert->addTo(alignMenu);
  alignMenu->insertSeparator();
  alignTop->addTo(alignMenu);
  alignBottom->addTo(alignMenu);
  alignLeft->addTo(alignMenu);
  alignRight->addTo(alignMenu);
  alignMenu->insertSeparator();
  distrHor->addTo(alignMenu);
  distrVert->addTo(alignMenu);

  insMenu = new QPopupMenu();  // menuBar entry insMenu
  insWire->addTo(insMenu);
  insLabel->addTo(insMenu);
  insEquation->addTo(insMenu);
  insGround->addTo(insMenu);
  insPort->addTo(insMenu);
  setMarker->addTo(insMenu);
  insEntity->addTo(insMenu);

  projMenu = new QPopupMenu();  // menuBar entry projMenu
  projNew->addTo(projMenu);
  projOpen->addTo(projMenu);
  addToProj->addTo(projMenu);
  projClose->addTo(projMenu);
  projDel->addTo(projMenu);
  projMenu->insertSeparator();
  createLib->addTo(projMenu);
  createPkg->addTo(projMenu);
  extractPkg->addTo(projMenu);
  projMenu->insertSeparator();
  importData->addTo(projMenu);
  graph2csv->addTo(projMenu);

  toolMenu = new QPopupMenu();  // menuBar entry toolMenu
  callEditor->addTo(toolMenu);
  callFilter->addTo(toolMenu);
  callLine->addTo(toolMenu);
  callLib->addTo(toolMenu);
  callMatch->addTo(toolMenu);
  callAtt->addTo(toolMenu);

  simMenu = new QPopupMenu();  // menuBar entry simMenu
  simulate->addTo(simMenu);
  dpl_sch->addTo(simMenu);
  dcbias->addTo(simMenu);
  showMsg->addTo(simMenu);
  showNet->addTo(simMenu);

  viewMenu = new QPopupMenu();  // menuBar entry viewMenu
  magAll->addTo(viewMenu);
  magOne->addTo(viewMenu);
  magPlus->addTo(viewMenu);
  magMinus->addTo(viewMenu);
  viewMenu->insertSeparator();
  viewMenu->setCheckable(true);
  viewToolBar->addTo(viewMenu);
  viewStatusBar->addTo(viewMenu);
  viewBrowseDock->addTo(viewMenu);
  viewOctaveDock->addTo(viewMenu);

  helpMenu = new QPopupMenu();  // menuBar entry helpMenu
  helpIndex->addTo(helpMenu);
  helpGetStart->addTo(helpMenu);
  helpMenu->insertSeparator();
  helpAboutApp->addTo(helpMenu);
  helpAboutQt->addTo(helpMenu);

  menuBar()->insertItem(tr("&File"), fileMenu);  // MENUBAR CONFIGURATION
  menuBar()->insertItem(tr("&Edit"), editMenu);
  menuBar()->insertItem(tr("P&ositioning"), alignMenu);
  menuBar()->insertItem(tr("&Insert"), insMenu);
  menuBar()->insertItem(tr("&Project"), projMenu);
  menuBar()->insertItem(tr("&Tools"), toolMenu);
  menuBar()->insertItem(tr("&Simulation"), simMenu);
  menuBar()->insertItem(tr("&View"), viewMenu);
  menuBar()->insertSeparator();
  menuBar()->insertItem(tr("&Help"), helpMenu);

}

// ----------------------------------------------------------
void QucsApp::initToolBar()
{
  fileToolbar = new QToolBar(this);
  fileNew->addTo(fileToolbar);
  textNew->addTo(fileToolbar);
  fileOpen->addTo(fileToolbar);
  fileSave->addTo(fileToolbar);
  fileSaveAll->addTo(fileToolbar);
  fileClose->addTo(fileToolbar);
  filePrint->addTo(fileToolbar);

  editToolbar = new QToolBar(this);
  editCut->addTo(editToolbar);
  editCopy->addTo(editToolbar);
  editPaste->addTo(editToolbar);
  editDelete->addTo(editToolbar);
  undo->addTo(editToolbar);
  redo->addTo(editToolbar);

  viewToolbar = new QToolBar(this);
  magAll->addTo(viewToolbar);
  magOne->addTo(viewToolbar);
  magPlus->addTo(viewToolbar);
  magMinus->addTo(viewToolbar);

  workToolbar = new QToolBar(this);
  select->addTo(workToolbar);
  editActivate->addTo(workToolbar);
  editMirror->addTo(workToolbar);
  editMirrorY->addTo(workToolbar);
  editRotate->addTo(workToolbar);
  intoH->addTo(workToolbar);
  popH->addTo(workToolbar);
  insWire->addTo(workToolbar);
  insLabel->addTo(workToolbar);
  insEquation->addTo(workToolbar);
  insGround->addTo(workToolbar);
  insPort->addTo(workToolbar);
  simulate->addTo(workToolbar);
  dpl_sch->addTo(workToolbar);
  setMarker->addTo(workToolbar);
  workToolbar->addSeparator();    // <<<=======================
  QWhatsThis::whatsThisButton(workToolbar);

}

// ----------------------------------------------------------
void QucsApp::initStatusBar()
{
  // To reserve enough space, insert the longest text and rewrite it afterwards.
  WarningLabel = new QLabel(tr("no warnings"), statusBar());
  statusBar()->addWidget(WarningLabel, 0, true);

  PositionLabel = new QLabel("0 : 0", statusBar());
  PositionLabel->setAlignment(Qt::AlignRight);
  statusBar()->addWidget(PositionLabel, 0, true);

  statusBar()->message(tr("Ready."), 2000);
}

// ----------------------------------------------------------
void QucsApp::slotShowWarnings()
{
  static int ResultState = 0;

  if(ResultState == 0) {
    QFont f = WarningLabel->font();
    f.setWeight(QFont::DemiBold);
    WarningLabel->setFont(f);
    WarningLabel->setText(tr("Warnings in last simulation! Press F5"));
  }

  ResultState++;
  if(ResultState & 1)
    WarningLabel->setPaletteForegroundColor(Qt::red);
  else
    WarningLabel->setPaletteForegroundColor(Qt::black);

  if(ResultState < 9)
    QTimer::singleShot(500, this, SLOT(slotShowWarnings()));
  else 
    ResultState = 0;
}

// ----------------------------------------------------------
void QucsApp::slotResetWarnings()
{
  QFont f = WarningLabel->font();   // reset warning label
  f.setWeight(QFont::Normal);
  WarningLabel->setFont(f);
  WarningLabel->setPaletteForegroundColor(Qt::black);
  WarningLabel->setText(tr("no warnings"));
}

// ----------------------------------------------------------
void QucsApp::printCursorPosition(int x, int y)
{
  PositionLabel->setText(QString::number(x)+" : "+QString::number(y));
  PositionLabel->setMinimumWidth(PositionLabel->width());
}

// ----------------------------------------------------------
// turn Toolbar on or off
void QucsApp::slotViewToolBar(bool toggle)
{
  if (toggle== false) {
    fileToolbar->hide();
    editToolbar->hide();
    viewToolbar->hide();
    workToolbar->hide();
  }
  else {
    fileToolbar->show();
    editToolbar->show();
    viewToolbar->show();
    workToolbar->show();
  }
}

// ----------------------------------------------------------
// turn Statusbar on or off
void QucsApp::slotViewStatusBar(bool toggle)
{
  if (toggle == false)
    statusBar()->hide();
  else
    statusBar()->show();
}

// ----------------------------------------------------------
// turn Brwose Dock Window on or off
void QucsApp::slotViewBrowseDock(bool toggle)
{
  if (toggle == false)
    dock->hide();
  else
    dock->show();
}

// ----------------------------------------------------------
void QucsApp::slotToggleDock(bool on)
{
  viewBrowseDock->blockSignals(true);
  viewBrowseDock->setOn(on);
  viewBrowseDock->blockSignals(false);
}

// ----------------------------------------------------------
// turn Octave Dock Window on or off
void QucsApp::slotViewOctaveDock(bool toggle)
{
  if(toggle) {
    octDock->show();
    octave->startOctave();
  }
  else
    octDock->hide();
}

// ----------------------------------------------------------
void QucsApp::slotToggleOctave(bool on)
{
  viewOctaveDock->blockSignals(true);
  viewOctaveDock->setOn(on);
  viewOctaveDock->blockSignals(false);
}

// ----------------------------------------------------------
void QucsApp::slotHelpAbout()
{
  QMessageBox::about(this, tr("About..."),
    tr("Qucs Version")+" "+PACKAGE_VERSION+"\n"+
    tr("Quite Universal Circuit Simulator")+"\n"+
    tr("Copyright (C)")+" 2003-2009 "+
    tr("by Michael Margraf")+"\n"+
    "\nThis is free software; see the source for copying conditions."
    "\nThere is NO warranty; not even for MERCHANTABILITY or "
    "\nFITNESS FOR A PARTICULAR PURPOSE.\n\n"+
    tr("Simulator by Stefan Jahn")+"\n"+
    tr("VHDL simulator 'FreeHDL' by Edwin Naroska and Marius Vollmer")+"\n"+
    tr("Special thanks to Jens Flucke and Raimund Jacob")+"\n"+
    tr("Many thanks to Mike Brinson for correcting the VHDL output")+"\n"+
    tr("GUI improvements by Gopala Krishna A")+"\n"+
    tr("Verilog-AMS interface by Helene Parruitte")+"\n\n"+
    tr("Translations:")+"\n"+
    tr("German by Stefan Jahn")+"\n"+
    tr("Polish by Dariusz Pienkowski")+"\n"+
    tr("Romanian by Radu Circa")+"\n"+
    tr("French by Vincent Habchi, F5RCS")+"\n"+
    tr("Portuguese by Luciano Franca and Helio de Sousa")+"\n"+
    tr("Spanish by Jose L. Redrejo Rodriguez")+"\n"+
    tr("Japanese by Toyoyuki Ishikawa")+"\n"+
    tr("Italian by Giorgio Luparia and Claudio Girardi")+"\n"+
    tr("Hebrew by Dotan Nahum")+"\n"+
    tr("Swedish by Peter Landgren")+"\n"+
    tr("Turkish by Onur and Ozgur Cobanoglu")+"\n"+
    tr("Hungarian by Jozsef Bus")+"\n"+
    tr("Russian by Igor Gorbounov")+"\n"+
    tr("Czech by Marek Straka")+"\n"+
    tr("Catalan by Antoni Subirats")+"\n"+
    tr("Arabic by Chabane Noureddine")+"\n"+
    tr("Kazakh by Erbol Keshubaev"));
}

// ----------------------------------------------------------
void QucsApp::slotHelpAboutQt()
{
  QMessageBox::aboutQt(this, tr("About Qt"));
}
