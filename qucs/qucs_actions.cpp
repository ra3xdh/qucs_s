/***************************************************************************
                               qucsactions.cpp
                              -----------------
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
#include <QtGui>
#include <QtCore>
#include <stdlib.h>
#include <limits.h>

#include <q3process.h>

//Added by qt3to4:
#include <Q3PtrList>

#include "main.h"
#include "qucs.h"
#include "schematic.h"
#include "textdoc.h"
#include "mouseactions.h"
#include "components/ground.h"
#include "components/subcirport.h"
#include "components/equation.h"
#include "dialogs/matchdialog.h"
#include "dialogs/changedialog.h"
#include "dialogs/searchdialog.h"
#include "dialogs/librarydialog.h"
#include "dialogs/importdialog.h"
#include "dialogs/packagedialog.h"

// for editing component name on schematic
QRegExp  Expr_CompProp;
QRegExpValidator Val_CompProp(Expr_CompProp, 0);


// -----------------------------------------------------------------------
// This function is called from all toggle actions.
bool QucsApp::performToggleAction(bool on, QAction *Action,
	pToggleFunc Function, pMouseFunc MouseMove, pMouseFunc2 MousePress)
{
  editText->setHidden(true); // disable text edit of component property

  if(!on) {
    MouseMoveAction = 0;
    MousePressAction = 0;
    MouseReleaseAction = 0;
    MouseDoubleClickAction = 0;
    activeAction = 0;   // no action active
    return false;
  }

  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  do {
    if(Function) if((Doc->*Function)()) {
      Action->blockSignals(true);
      Action->setOn(false);  // release toolbar button
      Action->blockSignals(false);
      Doc->viewport()->update();
      break;
    }

    if(activeAction) {
      activeAction->blockSignals(true); // do not call toggle slot
      activeAction->setOn(false);       // set last toolbar button off
      activeAction->blockSignals(false);
    }
    activeAction = Action;

    MouseMoveAction = MouseMove;
    MousePressAction = MousePress;
    MouseReleaseAction = 0;
    MouseDoubleClickAction = 0;

  } while(false);   // to perform "break"

  Doc->viewport()->update();
  view->drawn = false;
  return true;
}

// -----------------------------------------------------------------------
// Is called, when "set on grid" action is activated.
void QucsApp::slotOnGrid(bool on)
{
  performToggleAction(on, onGrid, &Schematic::elementsOnGrid,
		&MouseActions::MMoveOnGrid, &MouseActions::MPressOnGrid);
}

// -----------------------------------------------------------------------
// Is called when the rotate toolbar button is pressed.
void QucsApp::slotEditRotate(bool on)
{
  performToggleAction(on, editRotate, &Schematic::rotateElements,
		&MouseActions::MMoveRotate, &MouseActions::MPressRotate);
}

// -----------------------------------------------------------------------
// Is called when the mirror toolbar button is pressed.
void QucsApp::slotEditMirrorX(bool on)
{
  performToggleAction(on, editMirror, &Schematic::mirrorXComponents,
		&MouseActions::MMoveMirrorX, &MouseActions::MPressMirrorX);
}

// -----------------------------------------------------------------------
// Is called when the mirror toolbar button is pressed.
void QucsApp::slotEditMirrorY(bool on)
{
  performToggleAction(on, editMirrorY, &Schematic::mirrorYComponents,
		&MouseActions::MMoveMirrorY, &MouseActions::MPressMirrorY);
}

// -----------------------------------------------------------------------
// Is called when the activate/deactivate toolbar button is pressed.
void QucsApp::slotEditActivate (bool on)
{
  TextDoc * Doc = (TextDoc *) DocumentTab->currentPage ();
  if (isTextDocument (Doc)) {
    //TODO Doc->clearParagraphBackground (Doc->tmpPosX);
    Doc->commentSelected ();

    editActivate->blockSignals (true);
    editActivate->setOn (false);  // release toolbar button
    editActivate->blockSignals (false);
  }
  else
    performToggleAction (on, editActivate,
        &Schematic::activateSelectedComponents,
        &MouseActions::MMoveActivate, &MouseActions::MPressActivate);
}

// ------------------------------------------------------------------------
// Is called if "Delete"-Button is pressed.
void QucsApp::slotEditDelete(bool on)
{
  TextDoc *Doc = (TextDoc*)DocumentTab->currentPage();
  if(Doc->inherits("QTextEdit")) {
    Doc->viewport()->setFocus();
    //Doc->del();
    Doc->textCursor().deleteChar();

    editDelete->blockSignals(true);
    editDelete->setOn(false);  // release toolbar button
    editDelete->blockSignals(false);
  }
  else
    performToggleAction(on, editDelete, &Schematic::deleteElements,
          &MouseActions::MMoveDelete, &MouseActions::MPressDelete);
}

// -----------------------------------------------------------------------
// Is called if "Wire"-Button is pressed.
void QucsApp::slotSetWire(bool on)
{
  performToggleAction(on, insWire, 0,
		&MouseActions::MMoveWire1, &MouseActions::MPressWire1);
}

// -----------------------------------------------------------------------
void QucsApp::slotInsertLabel(bool on)
{
  performToggleAction(on, insLabel, 0,
		&MouseActions::MMoveLabel, &MouseActions::MPressLabel);
}

// -----------------------------------------------------------------------
void QucsApp::slotSetMarker(bool on)
{
  performToggleAction(on, setMarker, 0,
		&MouseActions::MMoveMarker, &MouseActions::MPressMarker);
}

// -----------------------------------------------------------------------
// Is called, when "move component text" action is activated.
void QucsApp::slotMoveText(bool on)
{
  performToggleAction(on, moveText, 0,
		&MouseActions::MMoveMoveTextB, &MouseActions::MPressMoveText);
}

// -----------------------------------------------------------------------
// Is called, when "Zoom in" action is activated.
void QucsApp::slotZoomIn(bool on)
{
  TextDoc *Doc = (TextDoc*)DocumentTab->currentPage();
  if(Doc->inherits("QTextEdit")) {
    Doc->zoomBy(2.0f);
    magPlus->blockSignals(true);
    magPlus->setOn(false);
    magPlus->blockSignals(false);
  }
  else
    performToggleAction(on, magPlus, 0,
		&MouseActions::MMoveZoomIn, &MouseActions::MPressZoomIn);
}


void QucsApp::slotEscape()
{
    select->setChecked(true);
}

// -----------------------------------------------------------------------
// Is called when the select toolbar button is pressed.
void QucsApp::slotSelect(bool on)
{
  QWidget *w = DocumentTab->currentPage();
  if(w->inherits("QTextEdit")) {
    ((TextDoc*)w)->viewport()->setFocus();
      select->blockSignals(true);
      select->setOn(true);
      select->blockSignals(false);
    return;
  }

  // goto to insertWire mode if ESC pressed during wiring
  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  if(MouseMoveAction == &MouseActions::MMoveWire2) {
    MouseMoveAction = &MouseActions::MMoveWire1;
    MousePressAction = &MouseActions::MPressWire1;
    Doc->viewport()->update();
    view->drawn = false;

    select->blockSignals(true);
    select->setOn(false);
    select->blockSignals(false);
    return;
  }

  if(performToggleAction(on, select, 0, 0, &MouseActions::MPressSelect)) {
    MouseReleaseAction = &MouseActions::MReleaseSelect;
    MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
  }
}

// -----------------------------------------------------------------------
void QucsApp::slotEditPaste(bool on)
{
  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  if(Doc->inherits("QTextEdit")) {
    ((TextDoc*)Doc)->viewport()->setFocus();
    ((TextDoc*)Doc)->paste();

    editPaste->blockSignals(true);
    editPaste->setOn(false);  // release toolbar button
    editPaste->blockSignals(false);
    return;
  }
  
  editText->setHidden(true); // disable text edit of component property

  if(!on) {
    MouseMoveAction = 0;
    MousePressAction = 0;
    MouseReleaseAction = 0;
    MouseDoubleClickAction = 0;
    activeAction = 0;   // no action active
    if(view->drawn) Doc->viewport()->update();
    return;
  }

  if(!view->pasteElements(Doc)) {
    editPaste->blockSignals(true); // do not call toggle slot
    editPaste->setOn(false);       // set toolbar button off
    editPaste->blockSignals(false);
    return;   // if clipboard empty
  }

  if(activeAction) {
    activeAction->blockSignals(true); // do not call toggle slot
    activeAction->setOn(false);       // set last toolbar button off
    activeAction->blockSignals(false);
  }
  activeAction = editPaste;

  view->drawn = false;
  MouseMoveAction = &MouseActions::MMovePaste;
  view->movingRotated = 0;
  MousePressAction = 0;
  MouseReleaseAction = 0;
  MouseDoubleClickAction = 0;
}

// -----------------------------------------------------------------------
void QucsApp::slotInsertEntity ()
{
  TextDoc * Doc = (TextDoc *) DocumentTab->currentPage ();
  Doc->viewport()->setFocus ();
  //TODO Doc->clearParagraphBackground (Doc->tmpPosX);
  Doc->insertSkeleton ();

  //int x, y;
  //Doc->getCursorPosition (&x, &y);
  //x = Doc->textCursor().blockNumber();
  //y = Doc->textCursor().columnNumber();
  Doc->slotCursorPosChanged();
}
  
// -----------------------------------------------------------------------
// Is called when the mouse is clicked upon the equation toolbar button.
void QucsApp::slotInsertEquation(bool on)
{
  editText->setHidden(true); // disable text edit of component property
  MouseReleaseAction = 0;
  MouseDoubleClickAction = 0;

  if(!on) {
    MouseMoveAction = 0;
    MousePressAction = 0;
    activeAction = 0;   // no action active
    return;
  }
  if(activeAction) {
    activeAction->blockSignals(true); // do not call toggle slot
    activeAction->setOn(false);       // set last toolbar button off
    activeAction->blockSignals(false);
  }
  activeAction = insEquation;

  if(view->selElem)
    delete view->selElem;  // delete previously selected component

  view->selElem = new Equation();

  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  if(view->drawn) Doc->viewport()->update();
  view->drawn = false;
  MouseMoveAction = &MouseActions::MMoveElement;
  MousePressAction = &MouseActions::MPressElement;
}

// -----------------------------------------------------------------------
// Is called when the mouse is clicked upon the ground toolbar button.
void QucsApp::slotInsertGround(bool on)
{
  editText->setHidden(true); // disable text edit of component property
  MouseReleaseAction = 0;
  MouseDoubleClickAction = 0;

  if(!on) {
    MouseMoveAction = 0;
    MousePressAction = 0;
    activeAction = 0;   // no action active
    return;
  }
  if(activeAction) {
    activeAction->blockSignals(true); // do not call toggle slot
    activeAction->setOn(false);       // set last toolbar button off
    activeAction->blockSignals(false);
  }
  activeAction = insGround;

  if(view->selElem)
    delete view->selElem;  // delete previously selected component

  view->selElem = new Ground();

  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  if(view->drawn) Doc->viewport()->update();
  view->drawn = false;
  MouseMoveAction = &MouseActions::MMoveElement;
  MousePressAction = &MouseActions::MPressElement;
}

// -----------------------------------------------------------------------
// Is called when the mouse is clicked upon the port toolbar button.
void QucsApp::slotInsertPort(bool on)
{
  editText->setHidden(true); // disable text edit of component property
  MouseReleaseAction = 0;
  MouseDoubleClickAction = 0;

  if(!on) {
    MouseMoveAction = 0;
    MousePressAction = 0;
    activeAction = 0;   // no action active
    return;
  }
  if(activeAction) {
    activeAction->blockSignals(true); // do not call toggle slot
    activeAction->setOn(false);       // set last toolbar button off
    activeAction->blockSignals(false);
  }
  activeAction = insPort;

  if(view->selElem)
    delete view->selElem;  // delete previously selected component

  view->selElem = new SubCirPort();

  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  if(view->drawn) Doc->viewport()->update();
  view->drawn = false;
  MouseMoveAction = &MouseActions::MMoveElement;
  MousePressAction = &MouseActions::MPressElement;
}

// --------------------------------------------------------------
// Is called, when "Undo"-Button is pressed.
void QucsApp::slotEditUndo()
{
  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  if(Doc->inherits("QTextEdit")) {
    ((TextDoc*)Doc)->viewport()->setFocus();
    ((TextDoc*)Doc)->undo();
    return;
  }
  
  editText->setHidden(true); // disable text edit of component property
  
  Doc->undo();
  Doc->viewport()->update();
  view->drawn = false;
}

// --------------------------------------------------------------
// Is called, when "Undo"-Button is pressed.
void QucsApp::slotEditRedo()
{
  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  if(Doc->inherits("QTextEdit")) {
    ((TextDoc*)Doc)->viewport()->setFocus();
    ((TextDoc*)Doc)->redo();
    return;
  }
  
  editText->setHidden(true); // disable text edit of component property
  
  Doc->redo();
  Doc->viewport()->update();
  view->drawn = false;
}

// --------------------------------------------------------------
// Is called, when "Align top" action is activated.
void QucsApp::slotAlignTop()
{
  editText->setHidden(true); // disable text edit of component property

  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  if(!Doc->aligning(0))
    QMessageBox::information(this, tr("Info"),
		      tr("At least two elements must be selected !"));
  Doc->viewport()->update();
  view->drawn = false;
}

// --------------------------------------------------------------
// Is called, when "Align bottom" action is activated.
void QucsApp::slotAlignBottom()
{
  editText->setHidden(true); // disable text edit of component property

  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  if(!Doc->aligning(1))
    QMessageBox::information(this, tr("Info"),
		      tr("At least two elements must be selected !"));
  Doc->viewport()->update();
  view->drawn = false;
}

// --------------------------------------------------------------
// Is called, when "Align left" action is activated.
void QucsApp::slotAlignLeft()
{
  editText->setHidden(true); // disable text edit of component property

  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  if(!Doc->aligning(2))
    QMessageBox::information(this, tr("Info"),
		      tr("At least two elements must be selected !"));
  Doc->viewport()->update();
  view->drawn = false;
}

// --------------------------------------------------------------
// Is called, when "Align right" action is activated.
void QucsApp::slotAlignRight()
{
  editText->setHidden(true); // disable text edit of component property

  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  if(!Doc->aligning(3))
    QMessageBox::information(this, tr("Info"),
		      tr("At least two elements must be selected !"));
  Doc->viewport()->update();
  view->drawn = false;
}

// --------------------------------------------------------------
// Is called, when "Distribute horizontally" action is activated.
void QucsApp::slotDistribHoriz()
{
  editText->setHidden(true); // disable text edit of component property

  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  Doc->distributeHorizontal();
  Doc->viewport()->update();
  view->drawn = false;
}

// --------------------------------------------------------------
// Is called, when "Distribute vertically" action is activated.
void QucsApp::slotDistribVert()
{
  editText->setHidden(true); // disable text edit of component property

  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  Doc->distributeVertical();
  Doc->viewport()->update();
  view->drawn = false;
}

// --------------------------------------------------------------
// Is called, when "Center horizontally" action is activated.
void QucsApp::slotCenterHorizontal()
{
  editText->setHidden(true); // disable text edit of component property

  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  if(!Doc->aligning(4))
    QMessageBox::information(this, tr("Info"),
		      tr("At least two elements must be selected !"));
  Doc->viewport()->update();
  view->drawn = false;
}

// --------------------------------------------------------------
// Is called, when "Center vertically" action is activated.
void QucsApp::slotCenterVertical()
{
  editText->setHidden(true); // disable text edit of component property

  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  if(!Doc->aligning(5))
    QMessageBox::information(this, tr("Info"),
		      tr("At least two elements must be selected !"));
  Doc->viewport()->update();
  view->drawn = false;
}

// ---------------------------------------------------------------------
// Is called when the "select all" action is activated.
void QucsApp::slotSelectAll()
{
  editText->setHidden(true); // disable text edit of component property

  QWidget *Doc = DocumentTab->currentPage();
  if(Doc->inherits("QTextEdit")) {
    ((TextDoc*)Doc)->viewport()->setFocus();
    //((TextDoc*)Doc)->selectAll(true);
    ((TextDoc*)Doc)->selectAll();
  }
  else {
    ((Schematic*)Doc)->selectElements(INT_MIN, INT_MIN, INT_MAX, INT_MAX, true);
    ((Schematic*)Doc)->viewport()->update();
    view->drawn = false;
  }
}

// ---------------------------------------------------------------------
// Is called when the "select markers" action is activated.
void QucsApp::slotSelectMarker()
{
  editText->setHidden(true); // disable text edit of component property

  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  Doc->selectMarkers();
  Doc->viewport()->update();
  view->drawn = false;
}

// ------------------------------------------------------------------------
// Is called by slotShowLastMsg(), by slotShowLastNetlist() and from the
// component edit dialog.
void QucsApp::editFile(const QString& File)
{
  QStringList com;
  com << QucsSettings.Editor;
  if (!File.isEmpty()) com << File;
  Q3Process *QucsEditor = new Q3Process(com);
  QucsEditor->setCommunication(0);
  if(!QucsEditor->start()) {
    QMessageBox::critical(this, tr("Error"), tr("Cannot start text editor!"));
    delete QucsEditor;
    return;
  }

  // to kill it before qucs ends
  connect(this, SIGNAL(signalKillEmAll()), QucsEditor, SLOT(kill()));
}

// ------------------------------------------------------------------------
// Is called to show the output messages of the last simulation.
void QucsApp::slotShowLastMsg()
{
  editFile(QucsSettings.QucsHomeDir.filePath("log.txt"));
}

// ------------------------------------------------------------------------
// Is called to show the netlist of the last simulation.
void QucsApp::slotShowLastNetlist()
{
  editFile(QucsSettings.QucsHomeDir.filePath("netlist.txt"));
}

// ------------------------------------------------------------------------
// Is called to start the text editor.
void QucsApp::slotCallEditor()
{
  editFile(QString(""));
}

// ------------------------------------------------------------------------
// Is called to start the filter synthesis program.
void QucsApp::slotCallFilter()
{
  Q3Process *QucsFilter =
    new Q3Process(QString(QucsSettings.BinDir + "qucsfilter"));
  if(!QucsFilter->start()) {
    QMessageBox::critical(this, tr("Error"),
                          tr("Cannot start filter synthesis program!"));
    delete QucsFilter;
    return;
  }

  // to kill it before qucs ends
  connect(this, SIGNAL(signalKillEmAll()), QucsFilter, SLOT(kill()));
}

// ------------------------------------------------------------------------
// Is called to start the transmission line calculation program.
void QucsApp::slotCallLine()
{
  Q3Process *QucsLine =
    new Q3Process(QString(QucsSettings.BinDir + "qucstrans"));
  if(!QucsLine->start()) {
    QMessageBox::critical(this, tr("Error"),
                          tr("Cannot start line calculation program!"));
    delete QucsLine;
    return;
  }

  // to kill it before qucs ends
  connect(this, SIGNAL(signalKillEmAll()), QucsLine, SLOT(kill()));
}

// ------------------------------------------------------------------------
// Is called to start the component library program.
void QucsApp::slotCallLibrary()
{
  Q3Process *QucsLibrary =
    new Q3Process(QString(QucsSettings.BinDir + "qucslib"));
  if(!QucsLibrary->start()) {
    QMessageBox::critical(this, tr("Error"),
                          tr("Cannot start library program!"));
    delete QucsLibrary;
    return;
  }

  // to kill it before qucs ends
  connect(this, SIGNAL(signalKillEmAll()), QucsLibrary, SLOT(kill()));
}

// --------------------------------------------------------------
// Is called to show a dialog for creating matching circuits.
void QucsApp::slotCallMatch()
{
  MatchDialog *d = new MatchDialog(this);
  d->exec();
}

// ------------------------------------------------------------------------
// Is called to start the attenuator calculation program.
void QucsApp::slotCallAtt()
{
  Q3Process *QucsAtt =
    new Q3Process(QString(QucsSettings.BinDir + "qucsattenuator"));
  if(!QucsAtt->start()) {
    QMessageBox::critical(this, tr("Error"),
                          tr("Cannot start attenuator calculation program!"));
    delete QucsAtt;
    return;
  }

  // to kill it before qucs ends
  connect(this, SIGNAL(signalKillEmAll()), QucsAtt, SLOT(kill()));
}

// --------------------------------------------------------------
void QucsApp::slotHelpIndex()
{
  showHTML("index.html");
}

// --------------------------------------------------------------
void QucsApp::slotGettingStarted()
{
  showHTML("start.html");
}

// --------------------------------------------------------------
void QucsApp::showHTML(const QString& Page)
{
  QStringList com;
  com << QucsSettings.BinDir + "qucshelp" << Page;
  Q3Process *QucsHelp = new Q3Process(com);
  QucsHelp->setCommunication(0);
  if(!QucsHelp->start()) {
    QMessageBox::critical(this, tr("Error"), tr("Cannot start qucshelp!"));
    delete QucsHelp;
    return;
  }

  // to kill it before qucs ends
  connect(this, SIGNAL(signalKillEmAll()), QucsHelp, SLOT(kill()));
}

// ---------------------------------------------------------------------
// Is called when the find action is activated.
void QucsApp::slotEditFind()
{
  SearchDia->initSearch();
}

// ---------------------------------------------------------------------
// Is called when the find-again action is activated.
void QucsApp::slotEditFindAgain()
{
  SearchDia->searchText(true, 1);
}

// --------------------------------------------------------------
void QucsApp::slotChangeProps()
{
  QWidget *Doc = DocumentTab->currentPage();
  if(Doc->inherits("QTextEdit")) {
    ((TextDoc*)Doc)->viewport()->setFocus();
    SearchDia->initSearch(true);
  }
  else {
    ChangeDialog *d = new ChangeDialog((Schematic*)Doc);
    if(d->exec() == QDialog::Accepted) {
      ((Schematic*)Doc)->setChanged(true, true);
      ((Schematic*)Doc)->viewport()->update();
    }
  }
}

// --------------------------------------------------------------
void QucsApp::slotAddToProject()
{
  editText->setHidden(true); // disable text edit of component property

  if(ProjName.isEmpty()) {
    QMessageBox::critical(this, tr("Error"), tr("No project open!"));
    return;
  }


  QStringList List = QFileDialog::getOpenFileNames(this, tr("Select files to copy"),
    lastDir.isEmpty() ? QString(".") : lastDir, QucsFileFilter);

  if(List.isEmpty()) {
    statusBar()->message(tr("No files copied."), 2000);
    return;
  }


  char *Buffer = (char*)malloc(0x10000);
  if(!Buffer) return;  // should never happen

  QStringList FileList = List;  // make a copy as recommended by Qt
  QStringList::Iterator it = FileList.begin();
  QFileInfo Info(*it);
  lastDir = Info.dirPath(true);  // remember last directory

  // copy all files to project directory
  int Num;
  QFile origFile, destFile;
  while(it != FileList.end()) {
    Info.setFile(*it);
    origFile.setName(*it);
    destFile.setName(QucsSettings.QucsWorkDir.absPath() +
                     QDir::separator() + Info.fileName());

    if(!origFile.open(QIODevice::ReadOnly)) {
      QMessageBox::critical(this, tr("Error"), tr("Cannot open \"%1\" !").arg(*it));
      it++;
      continue;
    }

    if(destFile.exists())
      if(QMessageBox::information(this, tr("Overwrite"),
           tr("File \"%1\" already exists.\nOverwrite ?").arg(*it), QMessageBox::Yes,
           QMessageBox::No|QMessageBox::Default|QMessageBox::Escape)
         != QMessageBox::Yes) {
        origFile.close();
        it++;
        continue;
      }

    if(!destFile.open(QIODevice::WriteOnly)) {
      QMessageBox::critical(this, tr("Error"), tr("Cannot create \"%1\" !").arg(*it));
      origFile.close();
      it++;
      continue;
    }

    // copy data
    do {
      Num = origFile.readBlock(Buffer, 0x10000);
      if(Num < 0) {
        QMessageBox::critical(this, tr("Error"), tr("Cannot read \"%1\" !").arg(*it));
        break;
      }
      Num = destFile.writeBlock(Buffer, Num);
      if(Num < 0) {
        QMessageBox::critical(this, tr("Error"), tr("Cannot write \"%1\" !").arg(*it));
        break;
      }
    } while(Num == 0x10000);

    origFile.close();
    destFile.close();
    it++;
  }
  
  free(Buffer);
  readProjectFiles();  // re-read the content ListView
  statusBar()->message(tr("Ready."));
}

// -----------------------------------------------------------
void QucsApp::slotCursorLeft()
{
  if(!editText->isHidden()) return;  // for edit of component property ?

  Q3PtrList<Element> movingElements;
  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  int markerCount = Doc->copySelectedElements(&movingElements);

  if((movingElements.count() - markerCount) < 1) {
    if(markerCount > 0) {  // only move marker if nothing else selected
      Doc->markerLeftRight(true, &movingElements);
      movingElements.clear();
    }
    else {
      if(Doc->scrollLeft(Doc->horizontalScrollBar()->lineStep()))
        Doc->scrollBy(-Doc->horizontalScrollBar()->lineStep(), 0);
    }

    Doc->viewport()->update();
    view->drawn = false;
    return;
  }

  view->moveElements(&movingElements, -Doc->GridX, 0);  // move "GridX" to left
  view->MAx3 = 1;  // sign for moved elements
  view->endElementMoving(Doc, &movingElements);
}

// -----------------------------------------------------------
void QucsApp::slotCursorRight()
{
  if(!editText->isHidden()) return;  // for edit of component property ?

  Q3PtrList<Element> movingElements;
  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  int markerCount = Doc->copySelectedElements(&movingElements);

  if((movingElements.count() - markerCount) < 1) {
    if(markerCount > 0) {  // only move marker if nothing else selected
      Doc->markerLeftRight(false, &movingElements);
      movingElements.clear();
    }
    else {
      if(Doc->scrollRight(-Doc->horizontalScrollBar()->lineStep()))
        Doc->scrollBy(Doc->horizontalScrollBar()->lineStep(), 0);
    }

    Doc->viewport()->update();
    view->drawn = false;
    return;
  }

  view->moveElements(&movingElements, Doc->GridX, 0);  // move "GridX" to right
  view->MAx3 = 1;  // sign for moved elements
  view->endElementMoving(Doc, &movingElements);
}

// -----------------------------------------------------------
void QucsApp::slotCursorUp()
{
  if(!editText->isHidden()) {  // for edit of component property ?
    if(view->MAx3 == 0) return;  // edit component namen ?
    Component *pc = (Component*)view->focusElement;
    Property *pp = pc->Props.at(view->MAx3-1);  // current property
    int Begin = pp->Description.find('[');
    if(Begin < 0) return;  // no selection list ?
    int End = pp->Description.find(editText->text(), Begin); // current
    if(End < 0) return;  // should never happen
    End = pp->Description.findRev(',', End);
    if(End < Begin) return;  // was first item ?
    End--;
    int Pos = pp->Description.findRev(',', End);
    if(Pos < Begin) Pos = Begin;   // is first item ?
    Pos++;
    if(pp->Description.at(Pos) == ' ') Pos++; // remove leading space
    editText->setText(pp->Description.mid(Pos, End-Pos+1));
    editText->selectAll();
    return;
  }

  Q3PtrList<Element> movingElements;
  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  int markerCount = Doc->copySelectedElements(&movingElements);

  if((movingElements.count() - markerCount) < 1) {
    if(markerCount > 0) {  // only move marker if nothing else selected
      Doc->markerUpDown(true, &movingElements);
      movingElements.clear();
    }
    else {
      if(Doc->scrollUp(Doc->verticalScrollBar()->lineStep()))
        Doc->scrollBy(0, -Doc->verticalScrollBar()->lineStep());
    }

    Doc->viewport()->update();
    view->drawn = false;
    return;
  }

  view->moveElements(&movingElements, 0, -Doc->GridY);  // move "GridY" up
  view->MAx3 = 1;  // sign for moved elements
  view->endElementMoving(Doc, &movingElements);
}

// -----------------------------------------------------------
void QucsApp::slotCursorDown()
{
  if(!editText->isHidden()) {  // for edit of component property ?
    if(view->MAx3 == 0) return;  // edit component namen ?
    Component *pc = (Component*)view->focusElement;
    Property *pp = pc->Props.at(view->MAx3-1);  // current property
    int Pos = pp->Description.find('[');
    if(Pos < 0) return;  // no selection list ?
    Pos = pp->Description.find(editText->text(), Pos); // current list item
    if(Pos < 0) return;  // should never happen
    Pos = pp->Description.find(',', Pos);
    if(Pos < 0) return;  // was last item ?
    Pos++;
    if(pp->Description.at(Pos) == ' ') Pos++; // remove leading space
    int End = pp->Description.find(',', Pos);
    if(End < 0) {  // is last item ?
      End = pp->Description.find(']', Pos);
      if(End < 0) return;  // should never happen
    }
    editText->setText(pp->Description.mid(Pos, End-Pos));
    editText->selectAll();
    return;
  }

  Q3PtrList<Element> movingElements;
  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  int markerCount = Doc->copySelectedElements(&movingElements);

  if((movingElements.count() - markerCount) < 1) {
    if(markerCount > 0) {  // only move marker if nothing else selected
      Doc->markerUpDown(false, &movingElements);
      movingElements.clear();
    }
    else {
      if(Doc->scrollDown(-Doc->verticalScrollBar()->lineStep()))
        Doc->scrollBy(0, Doc->verticalScrollBar()->lineStep());
    }

    Doc->viewport()->update();
    view->drawn = false;
    return;
  }

  view->moveElements(&movingElements, 0, Doc->GridY);  // move "GridY" down
  view->MAx3 = 1;  // sign for moved elements
  view->endElementMoving(Doc, &movingElements);
}

// -----------------------------------------------------------
// Is called if user clicked on component text of if return is
// pressed in the component text QLineEdit.
// In "view->MAx3" is the number of the current property.
void QucsApp::slotApplyCompText()
{
  QString s;
  QFont f = QucsSettings.font;
  Schematic *Doc = (Schematic*)DocumentTab->currentPage();
  f.setPointSizeFloat( Doc->Scale * float(f.pointSize()) );
  editText->setFont(f);

  Property  *pp = 0;
  Component *pc = (Component*)view->focusElement;
  if(!pc) return;  // should never happen
  view->MAx1 = pc->cx + pc->tx;
  view->MAy1 = pc->cy + pc->ty;

  int z, n=0;  // "n" is number of property on screen
  pp = pc->Props.first();
  for(z=view->MAx3; z>0; z--) {  // calculate "n"
    if(!pp) {  // should never happen
      editText->setHidden(true);
      return;
    }
    if(pp->display) n++;   // is visible ?
    pp = pc->Props.next();
  }
  
  pp = 0;
  if(view->MAx3 > 0)  pp = pc->Props.at(view->MAx3-1); // current property
  else s = pc->Name;

  if(!editText->isHidden()) {   // is called the first time ?
    // no -> apply value to current property
    if(view->MAx3 == 0) {   // component name ?
      Component *pc2;
      if(!editText->text().isEmpty())
        if(pc->Name != editText->text()) {
          for(pc2 = Doc->Components->first(); pc2!=0; pc2 = Doc->Components->next())
            if(pc2->Name == editText->text())
              break;  // found component with the same name ?
          if(!pc2) {
            pc->Name = editText->text();
            Doc->setChanged(true, true);  // only one undo state
          }
        }
    }
    else if(pp) {  // property was applied
      if(pp->Value != editText->text()) {
        pp->Value = editText->text();
        Doc->recreateComponent(pc);  // because of "Num" and schematic symbol
        Doc->setChanged(true, true); // only one undo state
      }
    }

    n++;     // next row on screen
    (view->MAx3)++;  // next property
    pp = pc->Props.at(view->MAx3-1);  // search for next property

    Doc->viewport()->update();
    view->drawn = false;

    if(!pp) {     // was already last property ?
      editText->setHidden(true);
      return;
    }


    while(!pp->display) {  // search for next visible property
      (view->MAx3)++;  // next property
      pp = pc->Props.next();
      if(!pp) {     // was already last property ?
        editText->setHidden(true);
        return;
      }
    }
  }

  // avoid seeing the property text behind the line edit
  if(pp)  // Is it first property or component name ?
    s = pp->Value;
  editText->setMinimumWidth(editText->fontMetrics().width(s)+4);


  Doc->contentsToViewport(int(Doc->Scale * float(view->MAx1 - Doc->ViewX1)),
			 int(Doc->Scale * float(view->MAy1 - Doc->ViewY1)),
			 view->MAx2, view->MAy2);
  editText->setReadOnly(false);
  if(pp) {  // is it a property ?
    s = pp->Value;
    view->MAx2 += editText->fontMetrics().width(pp->Name+"=");
    if(pp->Description.find('[') >= 0)  // is selection list ?
      editText->setReadOnly(true);
    Expr_CompProp.setPattern("[^\"]*");
    if(!pc->showName) n--;
  }
  else   // it is the component name
    Expr_CompProp.setPattern("[\\w_]+");
  Val_CompProp.setRegExp(Expr_CompProp);
  editText->setValidator(&Val_CompProp);

  z = editText->fontMetrics().lineSpacing();
  view->MAy2 += n*z;
  editText->setText(s);
  editText->setPaletteBackgroundColor(QucsSettings.BGColor);
  editText->setFocus();
  editText->selectAll();
  editText->reparent(Doc->viewport(), 0, QPoint(view->MAx2, view->MAy2), true);
}

// -----------------------------------------------------------
// Is called if the text of the property edit changed, to match
// the width of the edit field.
void QucsApp::slotResizePropEdit(const QString& t)
{
  editText->resize(editText->fontMetrics().width(t)+4,
                   editText->fontMetrics().lineSpacing());
}

// -----------------------------------------------------------
void QucsApp::slotCreateLib()
{
  editText->setHidden(true); // disable text edit of component property

  if(ProjName.isEmpty()) {
    QMessageBox::critical(this, tr("Error"), tr("Please open project with subcircuits!"));
    return;
  }

  LibraryDialog *d = new LibraryDialog(this, ConSchematics);
  d->exec();
}

// -----------------------------------------------------------
void QucsApp::slotImportData()
{
  editText->setHidden(true); // disable text edit of component property

  if(ProjName.isEmpty()) {
    QMessageBox::critical(this, tr("Error"), tr("Please open project first!"));
    return;
  }

  ImportDialog *d = new ImportDialog(this);
  if(d->exec() == QDialog::Accepted)
    readProjectFiles();  // re-read all project files
}

// -----------------------------------------------------------
void QucsApp::slotExportGraphAsCsv()
{
  editText->setHidden(true); // disable text edit of component property

  for(;;) {
    if(view->focusElement)
      if(view->focusElement->Type == isGraph)
        break;

    QMessageBox::critical(this, tr("Error"), tr("Please select a diagram graph!"));
    return;
  }

  /*QString s = Q3FileDialog::getSaveFileName(
     lastDir.isEmpty() ? QString(".") : lastDir,
     tr("CSV file")+" (*.csv);;" + tr("Any File")+" (*)",
     this, 0, tr("Enter an Output File Name"));
     */
  QString s = QFileDialog::getSaveFileName(this, tr("Enter an Output File Name"), 
    lastDir.isEmpty() ? QString(".") : lastDir, tr("CSV file")+" (*.csv);;" + tr("Any File")+" (*)");

  if(s.isEmpty())
    return;

  QFileInfo Info(s);
  lastDir = Info.dirPath(true);  // remember last directory
  if(Info.extension().isEmpty())
    s += ".csv";

  QFile File(s);
  if(File.exists())
    if(QMessageBox::information(this, tr("Info"),
          tr("Output file already exists!")+"\n"+tr("Overwrite it?"),
          tr("&Yes"), tr("&No"), 0,1,1))
      return;

  if(!File.open(QIODevice::WriteOnly)) {
    QMessageBox::critical(this, QObject::tr("Error"),
                          QObject::tr("Cannot create output file!"));
    return;
  }

  QTextStream Stream(&File);


  DataX *pD;
  Graph *g = (Graph*)view->focusElement;
  // First output the names of independent and dependent variables.
  for(pD = g->cPointsX.first(); pD!=0; pD = g->cPointsX.next())
    Stream << '\"' << pD->Var << "\";";
  Stream << "\"r " << g->Var << "\";\"i " << g->Var << "\"\n";


  int n, m;
  double *py = g->cPointsY;
  int Count = g->countY * g->cPointsX.getFirst()->count;
  for(n = 0; n < Count; n++) {
    m = n;
    for(pD = g->cPointsX.first(); pD!=0; pD = g->cPointsX.next()) {
      Stream << *(pD->Points + m%pD->count) << ';';
      m /= pD->count;
    }

    Stream << *(py) << ';' << *(py+1) << '\n';
    py += 2;
  }

  File.close();
}

// ----------------------------------------------------------
void QucsApp::slotCreatePackage()
{
  editText->setHidden(true); // disable text edit of component property

  PackageDialog *d = new PackageDialog(this, true);
  d->exec();
}

// ----------------------------------------------------------
void QucsApp::slotExtractPackage()
{
  editText->setHidden(true); // disable text edit of component property
  PackageDialog *d = new PackageDialog(this, false);
  d->show();
  d->extractPackage();
  readProjects();
}
