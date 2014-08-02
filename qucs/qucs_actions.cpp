/***************************************************************************
                               qucsactions.cpp
                              -----------------
    begin                : Sat May 1 2004
    copyright            : (C) 2004 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/* Copyright (C) 2014 Guilherme Brondani Torri <guitorri@gmail.com>        */

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

#include <QProcess>
#include <Q3PtrList>

#include "main.h"
#include "qucs.h"
#include "schematic.h"
#include "textdoc.h"
#include "mouseactions.h"
#include "messagedock.h"
#include "components/ground.h"
#include "components/subcirport.h"
#include "components/equation.h"
#include "dialogs/matchdialog.h"
#include "dialogs/changedialog.h"
#include "dialogs/searchdialog.h"
#include "dialogs/librarydialog.h"
#include "dialogs/loaddialog.h"
#include "dialogs/importdialog.h"
#include "dialogs/packagedialog.h"
#include "module.h"

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
// It also comments out the selected text on a text document
// \todo update the status or tooltip message
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
  if(Doc->inherits("QPlainTextEdit")) {
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
  if(Doc->inherits("QPlainTextEdit")) {
    Doc->zoomBy(1.5f);
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
  if(w->inherits("QPlainTextEdit")) {
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
  // get the current document
  Schematic *Doc = (Schematic*)DocumentTab->currentPage();

  // if the current document is a text document paste in
  // the contents of the clipboard as text
  if(Doc->inherits("QPlainTextEdit"))
  {
    ((TextDoc*)Doc)->viewport()->setFocus();
    ((TextDoc*)Doc)->paste();

    editPaste->blockSignals(true);
    editPaste->setOn(false);  // release toolbar button
    editPaste->blockSignals(false);
    return;
  }
  // if it's not a text doc, prevent the user from editing
  // while we perform the paste operation
  editText->setHidden(true);

  if(!on)
  {
    MouseMoveAction = 0;
    MousePressAction = 0;
    MouseReleaseAction = 0;
    MouseDoubleClickAction = 0;
    activeAction = 0;   // no action active
    if(view->drawn) Doc->viewport()->update();
    return;
  }

  if(!view->pasteElements(Doc))
  {
    editPaste->blockSignals(true); // do not call toggle slot
    editPaste->setOn(false);       // set toolbar button off
    editPaste->blockSignals(false);
    return;   // if clipboard empty
  }

  if(activeAction)
  {
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
  if(Doc->inherits("QPlainTextEdit")) {
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
  if(Doc->inherits("QPlainTextEdit")) {
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
  if(Doc->inherits("QPlainTextEdit")) {
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


extern QString lastDirOpenSave; // to remember last directory and file

// ------------------------------------------------------------------------
// Is called by slotShowLastMsg(), by slotShowLastNetlist() and from the
// component edit dialog.
void QucsApp::editFile(const QString& File)
{
    if (QucsSettings.Editor.toLower() == "qucs" | QucsSettings.Editor.isEmpty())
    {
        // The Editor is 'qucs' or empty, open a net document tab
        if (File.isEmpty()) {
            QucsApp::slotTextNew();
        }
        else
        {
            editText->setHidden(true); // disable text edit of component property

            statusBar()->message(tr("Opening file..."));

            QFileInfo finfo(File);

            if(!finfo.exists())
                statusBar()->message(tr("Opening aborted, file not found."), 2000);
            else {
                gotoPage(File);
                lastDirOpenSave = File;   // remember last directory and file
                statusBar()->message(tr("Ready."));
            }
        }
    }
    else
    {
      // use an external editor
      QString prog;
      QStringList args;

      if (QucsSettings.Editor.toLower().contains("qucsedit")) {

#ifdef __MINGW32__
  prog = "qucsedit.exe";
#elif __APPLE__
  prog = "qucsedit.app/Contents/MacOS/qucsedit";
#else
  prog = "qucsedit";
#endif

        QFileInfo editor(QucsSettings.BinDir + prog);
        prog = QDir::toNativeSeparators(editor.canonicalFilePath());
      }
      else { // user defined editor
          QFileInfo editor(QucsSettings.Editor);
          prog = QDir::toNativeSeparators(editor.canonicalFilePath());
      }

      if (!File.isEmpty()) {
          args << File;
      }

      QProcess *QucsEditor = new QProcess();
      QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
      env.insert("PATH", env.value("PATH") );
      QucsEditor->setProcessEnvironment(env);

      qDebug() << "Command: " << prog << args.join(" ");

      QucsEditor->start(prog, args);

      if( !QucsEditor->waitForStarted(1000) ) {
        QMessageBox::critical(this, tr("Error"), tr("Cannot start text editor! \n\n%1").arg(prog));
        delete QucsEditor;
        return;
      }
      qDebug() << QucsEditor->readAllStandardError();

      // to kill it before qucs ends
      connect(this, SIGNAL(signalKillEmAll()), QucsEditor, SLOT(kill()));
    }
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
  QString prog;

#ifdef __MINGW32__
  prog = "qucsfilter.exe";
#elif __APPLE__
  prog = "qucsfilter.app/Contents/MacOS/qucsfilter";
#else
  prog = "qucsfilter";
#endif

  QProcess *QucsFilter = new QProcess();

  QucsFilter->setWorkingDirectory(QucsSettings.BinDir);
  QucsFilter->start(prog);

  prog = QDir::toNativeSeparators(QucsSettings.BinDir+prog);
  qDebug() << "Command :" << prog;

  if( !QucsFilter->waitForStarted(1000) ) {
    QMessageBox::critical(this, tr("Error"),
                          tr("Cannot start filter synthesis program! \n\n%1").arg(prog));
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
  QString prog;
#ifdef __MINGW32__
  prog = "qucstrans.exe";
#elif __APPLE__
  prog = "qucstrans.app/Contents/MacOS/qucstrans";
#else
  prog = "qucstrans";
#endif
  QProcess *QucsLine = new QProcess();

  QucsLine->setWorkingDirectory(QucsSettings.BinDir);
  QucsLine->start(prog);

  prog = QDir::toNativeSeparators(QucsSettings.BinDir+prog);
  qDebug() << "Command :" << prog;

  if( !QucsLine->waitForStarted(1000) ) {
    QMessageBox::critical(this, tr("Error"),
                          tr("Cannot start line calculation program! \n\n%1").arg(prog));
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
  QString prog;
#ifdef __MINGW32__
  prog = "qucslib.exe";
#elif __APPLE__
  prog = "qucslib.app/Contents/MacOS/qucslib";
#else
  prog = "qucslib";
#endif

  QProcess *QucsLibrary = new QProcess();

  QucsLibrary->setWorkingDirectory(QucsSettings.BinDir);
  QucsLibrary->start(prog);

  prog = QDir::toNativeSeparators(QucsSettings.BinDir+prog);
  qDebug() << "Command :" << prog;

  if( !QucsLibrary->waitForStarted(1000) ) {

    QMessageBox::critical(this, tr("Error"),
                          tr("Cannot start library program! \n\n%1").arg(prog));
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
  QString prog;
#ifdef __MINGW32__
  prog = "qucsattenuator.exe";
#elif __APPLE__
  prog = "qucsattenuator.app/Contents/MacOS/qucsattenuator";
#else
  prog = "qucsattenuator";
#endif

  QProcess *QucsAtt = new QProcess();

  QucsAtt->setWorkingDirectory(QucsSettings.BinDir);
  QucsAtt->start(prog);

  prog = QDir::toNativeSeparators(QucsSettings.BinDir+prog);
  qDebug() << "Command :" << prog;

  if( !QucsAtt->waitForStarted(1000) ) {
    QMessageBox::critical(this, tr("Error"),
                          tr("Cannot start attenuator calculation program! \n\n%1").arg(prog));
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

  QString prog;
#ifdef __MINGW32__
  prog = "qucshelp.exe";
#elif __APPLE__
  prog = "qucshelp.app/Contents/MacOS/qucshelp";
#else
  prog = "qucshelp";
#endif

  QStringList com;
  com << prog << Page;

  QProcess *QucsHelp = new QProcess();

  QucsHelp->setWorkingDirectory(QucsSettings.BinDir);
  QucsHelp->start(com.join(" "));

  qDebug() << "Command :" << com.join(" ");
  if( !QucsHelp->waitForStarted(1000) ) {
    QMessageBox::critical(this, tr("Error"), tr("Cannot start qucshelp! \n\n%1").arg(com.join(" ")));
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
  if(Doc->inherits("QPlainTextEdit")) {
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

void QucsApp::slotOpenRecent(int num)
{
    //qDebug()<<QucsSettings.RecentDocs.at(num);
    gotoPage(QucsSettings.RecentDocs.at(num));
    updateRecentFilesList(QucsSettings.RecentDocs.at(num));
    slotUpdateRecentFiles();
}

void QucsApp::slotUpdateRecentFiles()
{


    QSignalMapper* mapper = new QSignalMapper(this);
    QList<QAction*> recent_docs;

    QString file;
    foreach (file,QucsSettings.RecentDocs) {
        recent_docs.prepend(new QAction(file,this));
        connect(recent_docs.first(),SIGNAL(triggered()),mapper,SLOT(map()));
    }
    recentfilesMenu->clear();
    recentfilesMenu->addActions(recent_docs);

    for (int i=0; i<recent_docs.count(); i++) {
        mapper->setMapping(recent_docs.at(i),(recent_docs.count() - 1)-i);
    }

    connect(mapper,SIGNAL(mapped(int)),this,SLOT(slotOpenRecent(int)));

    recentfilesMenu->insertSeparator();
    recentfilesMenu->addAction(tr("Clear list"),this,SLOT(slotClearRecentFiles()));

}

void QucsApp::slotClearRecentFiles()
{
    QucsSettings.RecentDocs.clear();
    slotUpdateRecentFiles();
}


/*!
 * \brief QucsApp::slotLoadModule launches the dialog to select dynamic modueles
 */
void QucsApp::slotLoadModule()
{
    qDebug() << "slotLoadModule";

    LoadDialog *ld = new LoadDialog(this);
    ld->setApp(this);

    // fech list of _symbol.json
    // \todo fetch timestamp of VA, JSON, if VA newer, need to reload.

    QDir projDir = QucsSettings.QucsWorkDir.absolutePath();

    QStringList files;
    QString fileSuffix = "*_symbol.json";

    files = projDir.entryList(QStringList(fileSuffix),
                                 QDir::Files | QDir::NoSymLinks);

    // no JSON files or no a project?
    if (!files.size()){
        QMessageBox::critical(this, tr("Error"),
                     tr("Symbol files not found in: %1\n\n"
                        "Is the project open?\n"
                        "Have you saved the Verilog-A symbols?")
                       .arg(QString(projDir.absolutePath())));
        return;
    }

    // initialize dialog

    // pass list of potential symbol files
    ld->symbolFiles << files;
    ld->projDir = projDir;
    ld->initDialog();

    // \todo check what is already loaded, offer skip, reload

    //pass stuff to ld dialog
    // run, let user do the selections

    if (ld->exec() == QDialog::Accepted) {

      Module::vaComponents = ld->selectedComponents;

      // dialog write new bitmap into JSON
      // load, unload, reload
      // inform if symbol changed
      // populate Module::vaComponents
      // vaComponents are selected with the dialog
      // dialog should populate acording to checkboxes
      // build vaComponents QMap

      // remove all modules before registering/loaded again
      // look for modules in the category,
      // \todo investigate if it is leaking objects somewhere
      QStringList remove;
      Q3DictIterator<Module> it( Module::Modules );
      for( ; it.current(); ++it ){
        if (it.current()->category == QObject::tr("verilog-a user devices"))
          remove << it.currentKey();
      }
      for (int i = 0; i < remove.size(); ++i){
        Module::Modules.remove(remove.at(i));
      }

      if (! Module::vaComponents.isEmpty()) {
        // Register whatever is in Module::vaComponents
        Module::registerDynamicComponents();

        // update the combobox, set new category in view
        // pick up new category 'verilog-a user components' from `Module::category`
        //set new category into view
        QucsApp::fillComboBox(true);
        CompChoose->setCurrentItem(CompChoose->count()-1);
        slotSetCompView(CompChoose->count()-1);

        // icons of dynamically registered components ready to be dragged
      }
    }

    delete ld;

}


/*!
 * \brief QucsApp::slotBuildModule runs admsXml, C++ compiler to build library
 *
 * Run the va2cpp
 * Run the cpp2lib
 *
 * TODO
 * - split into two actions, elaborate and compile?
 * - collect, parse and display output of make
 *
 */
void QucsApp::slotBuildModule()
{
    qDebug() << "slotBuildModule";

    // reset message dock on entry
    messageDock->reset();

    messageDock->builderTabs->setTabIcon(0,QPixmap());
    messageDock->builderTabs->setTabIcon(1,QPixmap());


    QString make;

#ifdef __MINGW32__
    make = "mingw32-make.exe";    // must be on the path!
#else
    make = "make";                // must be on the path!
#endif

    QDir prefix = QDir(QucsSettings.BinDir+"../");

    QDir include = QDir(QucsSettings.BinDir+"../include/qucs-core");

    QString workDir = QucsSettings.QucsWorkDir.absolutePath();

    // need to cd into project to make sure output is droped there?
    // need to cd - into previous location?
    QDir::setCurrent(workDir);

    QProcess *builder = new QProcess();
    builder->setProcessChannelMode(QProcess::MergedChannels);
    // get current va document
    QucsDoc *Doc = getDoc();
    QString vaModule = Doc->fileBase(Doc->DocName);

    QString admsXml = QucsSettings.AdmsXmlBinDir.canonicalPath();

#ifdef __MINGW32__
    admsXml = QDir::toNativeSeparators(admsXml+"/"+"admsXml.exe");
#else
    admsXml = QDir::toNativeSeparators(admsXml+"/"+"admsXml");
#endif

    // admsXml emmits C++
    QStringList Arguments;
    Arguments << "-f" <<  QDir::toNativeSeparators(include.absoluteFilePath("va2cpp.makefile"))
              << QString("ADMSXML=%1").arg(admsXml)
              << QString("PREFIX=%1").arg(QDir::toNativeSeparators(prefix.absolutePath()))
              << QString("MODEL=%1").arg(vaModule);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("PATH", env.value("PATH") );
    builder->setProcessEnvironment(env);

    // prepend command to log
    QString cmdString = QString("%1 %2\n").arg(make, Arguments.join(" "));
    messageDock->admsOutput->appendPlainText(cmdString);

    qDebug() << "Command :" << make << Arguments.join(" ");
    builder->start(make, Arguments);

    // admsXml seems to communicate all via stdout, or is it because of make?
    QString vaStatus;
    if (!builder->waitForFinished()) {
        vaStatus = builder->errorString();
        qDebug() << "Make failed:" << vaStatus;
    }
    else {
        vaStatus = builder->readAll();
        qDebug() << "Make stdout"  << vaStatus;
    }

    //build libs
    qDebug() << "\nbuild libs\n";

    Arguments.clear();

    Arguments << "-f" <<  QDir::toNativeSeparators(include.absoluteFilePath("cpp2lib.makefile"))
              << QString("PREFIX=\"%1\"").arg(QDir::toNativeSeparators(prefix.absolutePath()))
              << QString("PROJDIR=\"%1\"").arg(QDir::toNativeSeparators(workDir))
              << QString("MODEL=%1").arg(vaModule);

    // prepend command to log
    cmdString = QString("%1 %2\n").arg(make, Arguments.join(" "));
    messageDock->cppOutput->appendPlainText(cmdString);

    builder->start(make, Arguments);

    QString cppStatus;

    if (!builder->waitForFinished()) {
        cppStatus = builder->errorString();
        qDebug() << "Make failed:" << cppStatus;
    }
    else {
        cppStatus = builder->readAll();
        qDebug() << "Make output:" << cppStatus;
    }
    delete builder;

    // push make output to message dock
    messageDock->admsOutput->appendPlainText(vaStatus);
    messageDock->cppOutput->appendPlainText(cppStatus);

    // shot the message docks
    messageDock->msgDock->show();

}
