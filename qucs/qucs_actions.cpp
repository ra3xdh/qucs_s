/***************************************************************************
                               qucs_actions.cpp
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

/*! \file qucs_actions.cpp
 *  \brief Actions implementation for the GUI menu items
 */

#include <QtCore>
#include <stdlib.h>

#include <QProcess>
#include <QRegularExpressionValidator>
#include <QLineEdit>
#include <QAction>
#include <QStatusBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QMenu>
#include <QComboBox>
#include <QDockWidget>
#include <QTreeWidgetItem>
#include <QMutableHashIterator>
#include <QListWidget>
#include <QDesktopServices>

#include "portsymbol.h"
#include "projectView.h"
#include "main.h"
#include "qucs.h"
#include "schematic.h"
#include "textdoc.h"
#include "mouseactions.h"
#include "messagedock.h"
#include "components/ground.h"
#include "components/subcirport.h"
#include "components/equation.h"
#include "spicecomponents/sp_nutmeg.h"
#include "dialogs/matchdialog.h"
#include "dialogs/changedialog.h"
#include "dialogs/searchdialog.h"
#include "dialogs/librarydialog.h"
#include "dialogs/loaddialog.h"
#include "dialogs/importdialog.h"
#include "dialogs/aboutdialog.h"
#include "module.h"

#include "extsimkernels/xyce.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// for editing component name on schematic
QRegularExpression  Expr_CompProp;
QRegularExpressionValidator Val_CompProp(Expr_CompProp, 0);

// -----------------------------------------------------------------------
// This function is called from all toggle actions.
bool QucsApp::performToggleAction(bool on, QAction *Action,
      pToggleFunc Function, pMouseFunc MouseMove, pMouseFunc2 MousePress)
{
  slotHideEdit(); // disable text edit of component property
  Schematic *Doc = (Schematic*)DocumentTab->currentWidget();

  // Perform toggle release clean up.
  if(!on) {
    MouseMoveAction = 0;
    MousePressAction = 0;
    MouseReleaseAction = 0;
    MouseDoubleClickAction = 0;
    activeAction = 0;   // no action active

    // Return to select mode.
    slotEscape();
    return false;
  }

  do {
    if(Function) if((Doc->*Function)()) {
      Action->blockSignals(true);
      Action->setChecked(false);  // release toolbar button
      Action->blockSignals(false);
      Doc->viewport()->update();
      break;
    }

    if(activeAction) {
      activeAction->blockSignals(true); // do not call toggle slot
      activeAction->setChecked(false);       // set last toolbar button off
      activeAction->blockSignals(false);
    }
    activeAction = Action;

    MouseMoveAction = MouseMove;
    MousePressAction = MousePress;
    MouseReleaseAction = 0;
    MouseDoubleClickAction = 0;

  } while(false);   // to perform "break"

  Doc->viewport()->update();
  return true;
}

// -----------------------------------------------------------------------
// Is called, when "set on grid" action is triggered.
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
  TextDoc * Doc = (TextDoc *) DocumentTab->currentWidget();
  if (isTextDocument (Doc)) {
    //TODO Doc->clearParagraphBackground (Doc->tmpPosX);
    Doc->commentSelected ();

    editActivate->blockSignals (true);
    editActivate->setChecked(false);  // release toolbar button
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
  TextDoc *Doc = (TextDoc*)DocumentTab->currentWidget();
  if(isTextDocument(Doc)) {
    Doc->viewport()->setFocus();
    //Doc->del();
    Doc->textCursor().deleteChar();

    editDelete->blockSignals(true);
    editDelete->setChecked(false);  // release toolbar button
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
// Toolbar button to update the diagram limits using the mouse - aka zooming.
void QucsApp::slotSetDiagramLimits(bool on)
{
  performToggleAction(on, setDiagramLimits, 0,
                    &MouseActions::MMoveSetLimits, &MouseActions::MPressSetLimits);
}

// -----------------------------------------------------------------------
// Context menu option to reset the diagram limits to defaults.
void QucsApp::slotResetDiagramLimits()
{
  if (view->focusElement && view->focusElement->Type == isDiagram)
  {
    Diagram* diagram = dynamic_cast<Diagram*>(view->focusElement);
    Schematic* Doc = dynamic_cast<Schematic*>(DocumentTab->currentWidget());

    diagram->xAxis.autoScale = true;
    diagram->yAxis.autoScale = true;
    diagram->zAxis.autoScale = true;

    diagram->updateGraphData();

    Doc->setChanged(true, true);
    Doc->viewport()->update();
  }

  // Return to select mode (in case SetDiagramLimits is still selected).
  slotEscape();
}

// -----------------------------------------------------------------------
// Is called, when "show grid" action is triggered.
void QucsApp::slotShowGrid()
{
  qDebug() << "slotShowGrid";
  Schematic* schematic = static_cast<Schematic*>(DocumentTab->currentWidget());
  if(!isTextDocument(schematic)) {
    schematic->setGridOn(!schematic->getGridOn());
    schematic->setChanged(true);
    schematic->viewport()->repaint();
  }
}

// -----------------------------------------------------------------------
// Is called, when "move component text" action is triggered.
void QucsApp::slotMoveText(bool on)
{
  performToggleAction(on, moveText, 0,
    &MouseActions::MMoveMoveTextB, &MouseActions::MPressMoveText);
}

// -----------------------------------------------------------------------
// Is called, when "Zoom in" action is triggered.
void QucsApp::slotZoomIn(bool on)
{
  auto *Doc = (TextDoc*)DocumentTab->currentWidget();
  if(isTextDocument(Doc)) {
    Doc->zoomBy(1.5f);
    magPlus->blockSignals(true);
    magPlus->setChecked(false);
    magPlus->blockSignals(false);
  }
  else
    performToggleAction(on, magPlus, 0,
      &MouseActions::MMoveZoomIn, &MouseActions::MPressZoomIn);
}


void QucsApp::slotEscape()
{
    select->setChecked(true);
    slotSearchClear();
}

// -----------------------------------------------------------------------
// Is called when the select toolbar button is pressed.
void QucsApp::slotSelect(bool on)
{
  QWidget *w = DocumentTab->currentWidget();
  if(isTextDocument(w)) {
    ((TextDoc*)w)->viewport()->setFocus();
      select->blockSignals(true);
      select->setChecked(true);
      select->blockSignals(false);
    return;
  }

  // goto to insertWire mode if ESC pressed during wiring
  Schematic *Doc = (Schematic*)DocumentTab->currentWidget();
  if(MouseMoveAction == &MouseActions::MMoveWire2) {
    MouseMoveAction = &MouseActions::MMoveWire1;
    MousePressAction = &MouseActions::MPressWire1;
    Doc->viewport()->update();

    select->blockSignals(true);
    select->setChecked(false);
    select->blockSignals(false);
    return;
  }

  if(performToggleAction(on, select, 0, 0, &MouseActions::MPressSelect)) {
    MouseReleaseAction = &MouseActions::MReleaseSelect;
    MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
  }
}

// --------------------------------------------------------------------
void QucsApp::slotEditCut()
{
  statusBar()->showMessage(tr("Cutting selection..."));
  slotHideEdit(); // disable text edit of component property

  QWidget *Doc = DocumentTab->currentWidget();
  if(isTextDocument (Doc)) {
    ((TextDoc *)Doc)->cut();
  } else {
    ((Schematic *)Doc)->cut();
  }

  statusBar()->showMessage(tr("Ready."));
}

// --------------------------------------------------------------------
void QucsApp::slotEditCopy()
{
  statusBar()->showMessage(tr("Copying selection to clipboard..."));

  QWidget *Doc = DocumentTab->currentWidget();
  if(isTextDocument (Doc)) {
    ((TextDoc *)Doc)->copy();
  } else {
    ((Schematic *)Doc)->copy();
  }

  statusBar()->showMessage(tr("Ready."));
}

// -----------------------------------------------------------------------
void QucsApp::slotEditPaste(bool on)
{
  // get the current document
  QWidget *Doc = DocumentTab->currentWidget();

  // if the current document is a text document paste in
  // the contents of the clipboard as text
  if(isTextDocument (Doc))
  {
    ((TextDoc*)Doc)->paste();

    editPaste->blockSignals(true);
    editPaste->setChecked(false);  // release toolbar button
    editPaste->blockSignals(false);
    return;
  }
  else {
    // if it's not a text doc, prevent the user from editing
    // while we perform the paste operation
    slotHideEdit();

    if(!on)
    {
      MouseMoveAction = 0;
      MousePressAction = 0;
      MouseReleaseAction = 0;
      MouseDoubleClickAction = 0;
      activeAction = 0;   // no action active
      return;
    }

    if(!view->pasteElements((Schematic *)Doc))
    {
      editPaste->blockSignals(true); // do not call toggle slot
      editPaste->setChecked(false);       // set toolbar button off
      editPaste->blockSignals(false);
      return;   // if clipboard empty
    }

    if(activeAction)
    {
      activeAction->blockSignals(true); // do not call toggle slot
      activeAction->setChecked(false);       // set last toolbar button off
      activeAction->blockSignals(false);
    }
    activeAction = editPaste;

    MouseMoveAction = &MouseActions::MMovePaste;
    view->movingRotated = 0;
    MousePressAction = 0;
    MouseReleaseAction = 0;
    MouseDoubleClickAction = 0;
  }
}

// -----------------------------------------------------------------------
void QucsApp::slotInsertEntity ()
{
  TextDoc * Doc = (TextDoc *) DocumentTab->currentWidget ();
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
  slotHideEdit(); // disable text edit of component property
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
    activeAction->setChecked(false);       // set last toolbar button off
    activeAction->blockSignals(false);
  }
  activeAction = insEquation;

  if(view->selElem)
    delete view->selElem;  // delete previously selected component

  if (QucsSettings.DefaultSimulator == spicecompat::simNgspice) {
      view->selElem = new NutmegEquation();
  } else {
      view->selElem = new Equation();
  }

  MouseMoveAction = &MouseActions::MMoveElement;
  MousePressAction = &MouseActions::MPressElement;
}

// -----------------------------------------------------------------------
// Is called when the mouse is clicked upon the ground toolbar button.
void QucsApp::slotInsertGround(bool on)
{
  slotHideEdit(); // disable text edit of component property
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
    activeAction->setChecked(false);       // set last toolbar button off
    activeAction->blockSignals(false);
  }
  activeAction = insGround;

  if(view->selElem)
    delete view->selElem;  // delete previously selected component

  view->selElem = new Ground();

  MouseMoveAction = &MouseActions::MMoveElement;
  MousePressAction = &MouseActions::MPressElement;
}

// -----------------------------------------------------------------------
// Is called when the mouse is clicked upon the port toolbar button.
void QucsApp::slotInsertPort(bool on)
{
  slotHideEdit(); // disable text edit of component property
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
    activeAction->setChecked(false);       // set last toolbar button off
    activeAction->blockSignals(false);
  }
  activeAction = insPort;

  if(view->selElem)
    delete view->selElem;  // delete previously selected component

  Schematic *Doc = (Schematic*)DocumentTab->currentWidget();
  if (Doc->getSymbolMode()) {
    view->selElem = new PortSymbol();
  } else {
     view->selElem = new SubCirPort();
  }

  MouseMoveAction = &MouseActions::MMoveElement;
  MousePressAction = &MouseActions::MPressElement;
}

// --------------------------------------------------------------
// Is called, when "Undo"-Button is pressed.
void QucsApp::slotEditUndo()
{
  Schematic *Doc = (Schematic*)DocumentTab->currentWidget();
  if(isTextDocument(Doc)) {
    ((TextDoc*)Doc)->viewport()->setFocus();
    ((TextDoc*)Doc)->undo();
    return;
  }

  slotHideEdit(); // disable text edit of component property

  Doc->undo();
  Doc->viewport()->update();
}

// --------------------------------------------------------------
// Is called, when "Undo"-Button is pressed.
void QucsApp::slotEditRedo()
{
  Schematic *Doc = (Schematic*)DocumentTab->currentWidget();
  if(isTextDocument(Doc)) {
    ((TextDoc*)Doc)->viewport()->setFocus();
    ((TextDoc*)Doc)->redo();
    return;
  }

  slotHideEdit(); // disable text edit of component property

  Doc->redo();
  Doc->viewport()->update();
}

// --------------------------------------------------------------
// Is called, when "Align top" action is triggered.
void QucsApp::slotAlignTop()
{
  slotHideEdit(); // disable text edit of component property

  Schematic *Doc = (Schematic*)DocumentTab->currentWidget();
  if(!Doc->aligning(0))
    QMessageBox::information(this, tr("Info"),
          tr("At least two elements must be selected !"));
  Doc->viewport()->update();
}

// --------------------------------------------------------------
// Is called, when "Align bottom" action is triggered.
void QucsApp::slotAlignBottom()
{
  slotHideEdit(); // disable text edit of component property

  Schematic *Doc = (Schematic*)DocumentTab->currentWidget();
  if(!Doc->aligning(1))
    QMessageBox::information(this, tr("Info"),
          tr("At least two elements must be selected !"));
  Doc->viewport()->update();
}

// --------------------------------------------------------------
// Is called, when "Align left" action is triggered.
void QucsApp::slotAlignLeft()
{
  slotHideEdit(); // disable text edit of component property

  Schematic *Doc = (Schematic*)DocumentTab->currentWidget();
  if(!Doc->aligning(2))
    QMessageBox::information(this, tr("Info"),
          tr("At least two elements must be selected !"));
  Doc->viewport()->update();
}

// --------------------------------------------------------------
// Is called, when "Align right" action is triggered.
void QucsApp::slotAlignRight()
{
  slotHideEdit(); // disable text edit of component property

  Schematic *Doc = (Schematic*)DocumentTab->currentWidget();
  if(!Doc->aligning(3))
    QMessageBox::information(this, tr("Info"),
          tr("At least two elements must be selected !"));
  Doc->viewport()->update();
}

// --------------------------------------------------------------
// Is called, when "Distribute horizontally" action is triggered.
void QucsApp::slotDistribHoriz()
{
  slotHideEdit(); // disable text edit of component property

  Schematic *Doc = (Schematic*)DocumentTab->currentWidget();
  Doc->distributeHorizontal();
  Doc->viewport()->update();
}

// --------------------------------------------------------------
// Is called, when "Distribute vertically" action is triggered.
void QucsApp::slotDistribVert()
{
  slotHideEdit(); // disable text edit of component property

  Schematic *Doc = (Schematic*)DocumentTab->currentWidget();
  Doc->distributeVertical();
  Doc->viewport()->update();
}

// --------------------------------------------------------------
// Is called, when "Center horizontally" action is triggered.
void QucsApp::slotCenterHorizontal()
{
  slotHideEdit(); // disable text edit of component property

  Schematic *Doc = (Schematic*)DocumentTab->currentWidget();
  if(!Doc->aligning(4))
    QMessageBox::information(this, tr("Info"),
          tr("At least two elements must be selected !"));
  Doc->viewport()->update();
}

// --------------------------------------------------------------
// Is called, when "Center vertically" action is triggered.
void QucsApp::slotCenterVertical()
{
  slotHideEdit(); // disable text edit of component property

  Schematic *Doc = (Schematic*)DocumentTab->currentWidget();
  if(!Doc->aligning(5))
    QMessageBox::information(this, tr("Info"),
          tr("At least two elements must be selected !"));
  Doc->viewport()->update();
}

// ---------------------------------------------------------------------
// Is called when the "select all" action is triggered.
void QucsApp::slotSelectAll()
{
  slotHideEdit(); // disable text edit of component property

  QWidget *Doc = DocumentTab->currentWidget();
  if(isTextDocument(Doc)) {
    ((TextDoc*)Doc)->viewport()->setFocus();
    //((TextDoc*)Doc)->selectAll(true);
    ((TextDoc*)Doc)->selectAll();
  }
  else {
    auto selectionRect = ((Schematic*)Doc)->allBoundingRect().marginsAdded(QMargins{1, 1, 1, 1});
    ((Schematic*)Doc)->selectElements(selectionRect, true, false);
    ((Schematic*)Doc)->viewport()->update();
  }
}

// ---------------------------------------------------------------------
// Is called when the "select markers" action is triggered.
void QucsApp::slotSelectMarker()
{
  slotHideEdit(); // disable text edit of component property

  Schematic *Doc = (Schematic*)DocumentTab->currentWidget();
  Doc->selectMarkers();
  Doc->viewport()->update();
}


extern QString lastDirOpenSave; // to remember last directory and file

// ------------------------------------------------------------------------
// Is called by slotShowLastMsg(), by slotShowLastNetlist() and from the
// component edit dialog.
void QucsApp::editFile(const QString& File)
{
    if ((QucsSettings.Editor.toLower() == "qucs") || QucsSettings.Editor.isEmpty())
    {
        // The Editor is 'qucs' or empty, open a net document tab
        if (File.isEmpty()) {
            QucsApp::slotTextNew();
        }
        else
        {
            slotHideEdit(); // disable text edit of component property

            statusBar()->showMessage(tr("Opening file..."));

            QFileInfo finfo(File);

            if(!finfo.exists())
                statusBar()->showMessage(tr("Opening aborted, file not found."), 2000);
            else {
                gotoPage(File);
                lastDirOpenSave = File;   // remember last directory and file
                statusBar()->showMessage(tr("Ready."));
            }
        }
    }
    else
    {
      // use an external editor
      QString prog;
      QStringList args;

      if (QucsSettings.Editor.toLower().contains("qucsedit")) {

#if defined(_WIN32) || defined(__MINGW32__)
  prog = QUCS_NAME"edit.exe";
#elif __APPLE__
  prog = "qucsedit.app/Contents/MacOS/qucsedit";
#else
  prog = "qucsedit";
#endif

        QFileInfo editor(QucsSettings.QucsatorDir + prog);
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
  editFile(QucsSettings.tempFilesDir.filePath("log.txt"));
}

// ------------------------------------------------------------------------
// Is called to show the netlist of the last simulation.
void QucsApp::slotShowLastNetlist()
{
    QStringList netlists;
    QStringList sim_lst;

    QWidget *w = DocumentTab->currentWidget();

    if (QucsSettings.DefaultSimulator == spicecompat::simXyce) {
        if (isTextDocument(w)) {
            QMessageBox::information(this, tr("Show netlist"),
                                     tr("Not a schematic tab!"));
            return;
        } else {
            Schematic *sch = (Schematic *) w;
            Xyce *xyce = new Xyce(sch,this);
            xyce->determineUsedSimulations(&sim_lst);
            delete xyce;
        }
    }

    switch (QucsSettings.DefaultSimulator) {
    case spicecompat::simQucsator :
        netlists.append(QucsSettings.tempFilesDir.filePath("netlist.txt"));
        break;
    case spicecompat::simNgspice :
    case spicecompat::simSpiceOpus :
        netlists.append(QDir::toNativeSeparators(QucsSettings.S4Qworkdir
                                                 + "/spice4qucs.cir"));
        break;
    case spicecompat::simXyce: // Xyce generates one netlist for every simulation
        for(const auto &sim : sim_lst) {
            netlists.append(QDir::toNativeSeparators(QucsSettings.S4Qworkdir
                                                     + "/spice4qucs."
                                                     + sim + ".cir"));
        }
        break;
    default: break;
    }

    if (!isTextDocument(w)) {
        Schematic *sch = (Schematic *) w;
        if (sch->isDigitalCircuit()) {
            netlists.clear();
            netlists.append(QucsSettings.tempFilesDir.filePath("netlist.txt"));
        }
    }

    for(const auto &netlist: netlists) {
        editFile(netlist);
    }
}

// ------------------------------------------------------------------------
// Is called to start the text editor.
void QucsApp::slotCallEditor()
{
  editFile(QString());
}

// ------------------------------------------------------------------------
// Is called to start the filter synthesis program.
void QucsApp::slotCallFilter()
{
  auto currentStyle = QApplication::style()->objectName();
  launchTool(QUCS_NAME "filter", "filter synthesis",(QStringList() << "-style" << currentStyle));
}

void QucsApp::slotCallActiveFilter()
{
  auto currentStyle = QApplication::style()->objectName();
  launchTool(QUCS_NAME "activefilter", "active filter synthesis",(QStringList() << "-style" << currentStyle));
}

// ------------------------------------------------------------------------
// Is called to start the transmission line calculation program.
void QucsApp::slotCallLine()
{
  auto currentStyle = QApplication::style()->objectName();
  launchTool(QUCS_NAME "trans", "line calculation",(QStringList() << "-style" << currentStyle));
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
  auto currentStyle = QApplication::style()->objectName();
  launchTool(QUCS_NAME "attenuator", "attenuator calculation",(QStringList() << "-style" << currentStyle));
}

void QucsApp::slotCallPwrComb()
{
  auto currentStyle = QApplication::style()->objectName();
  launchTool(QUCS_NAME "powercombining", "power combining calculation",(QStringList() << "-style" << currentStyle));
}

void QucsApp::slotCallSPAR_Viewer()
{
  auto currentStyle = QApplication::style()->objectName();
  launchTool(QUCS_NAME "spar-viewer", "s-parameter viewer",(QStringList() << "-style" << currentStyle));
}


/*!
 * \brief launch an external application passing arguments
 *
 * \param prog  executable program name (will be transformed depending on OS)
 * \param progDesc  program description string (used for error messages)
 * \param args  arguments to pass to the executable
 */
void QucsApp::launchTool(const QString& prog, const QString& progDesc, const QStringList &args,
                         bool qucs_tool)
{
    QString tooldir = qucs_tool ? QucsSettings.QucsatorDir : QucsSettings.BinDir;

    // Create command path based on the platform
    QString cmd;
#if defined(_WIN32) || defined(__MINGW32__)
    cmd = QDir(tooldir).absoluteFilePath(prog + ".exe");
#elif __APPLE__
    cmd = QDir(tooldir).absoluteFilePath(prog + ".app/Contents/MacOS/" + prog);
#else
    cmd = QDir(tooldir).absoluteFilePath(prog);
#endif

    // Validate if the file exists before attempting to execute
    if (!QFileInfo(cmd).exists()) {
        QMessageBox::critical(this, tr("Error"),
                            tr("Executable %1 not found! \n\n(%2)").arg(progDesc, cmd));
        return;
    }

    QProcess *tool = new QProcess();

    qDebug() << "Command :" << cmd;
    tool->setWorkingDirectory(tooldir);
    tool->start(cmd,args);

    if(!tool->waitForStarted(1000) ) {
        QMessageBox::critical(this, tr("Error"),
                            tr("Cannot start %1 program! \n\n(%2)").arg(progDesc, cmd));
        delete tool;
        return;
    }

    // to kill the application first before qucs finishes exiting
    connect(this, SIGNAL(signalKillEmAll()), tool, SLOT(kill()));
}


void QucsApp::slotCallRFLayout()
{
    QString input_file, netlist_file, odir;
    if (!isTextDocument(DocumentTab->currentWidget())) {
        Schematic *sch = (Schematic*)DocumentTab->currentWidget();
        if(sch->fileSuffix() == "dpl") {
            QMessageBox::critical(this,tr("Error"),
                                  tr("Layouting of display pages is not supported!"));
            return;
        }
        input_file = sch->getDocName();
        QFileInfo inf(sch->getDocName());
        odir = inf.absolutePath();
        netlist_file = inf.absolutePath() + QDir::separator()
                + inf.baseName() + ".net";
        QFile f(netlist_file);
        if (!f.open(QIODevice::WriteOnly)) {
            QMessageBox::critical(this, tr("Error"), tr("Cannot write netlist!"));
            return;
        }
        QTextStream stream(&f);
        QStringList Collect;
        QPlainTextEdit *ErrText = new QPlainTextEdit();  //dummy
        int pNum = sch->prepareNetlist(stream, Collect, ErrText);
        if (!sch->getIsAnalog()) {
            QMessageBox::critical(this, tr("Error"), tr("Digital schematic not supported!"));
            return;
        }
        stream << '\n';
        sch->createNetlist(stream, pNum);
        f.close();
    } else {
        QMessageBox::critical(this,tr("Error"),
                              tr("Layouting of text documents is not supported!"));
        return;
    }

    QProcess *tool = new QProcess();
    QStringList args;
    args.append("-G");
    args.append("-i");
    args.append(input_file);
    args.append("-n");
    args.append(netlist_file);
    args.append("-o");
    args.append(odir);
    tool->start(QucsSettings.RFLayoutExecutable,args);

    if(!tool->waitForStarted(1000) ) {
      QMessageBox::critical(this, tr("Error"),
                            tr("Cannot start Qucs-RFLayout: \n%1")
                            .arg(QucsSettings.RFLayoutExecutable));
      delete tool;
      return;
    }
    connect(this, SIGNAL(signalKillEmAll()), tool, SLOT(kill()));
}

// --------------------------------------------------------------
void QucsApp::slotHelpIndex()
{
  QDesktopServices::openUrl(QUrl("https://qucs-s-help.readthedocs.io/"));
}

void QucsApp::slotHelpQucsIndex()
{
    QDesktopServices::openUrl(QUrl("https://qucs-help.readthedocs.io/"));
}

// --------------------------------------------------------------
void QucsApp::slotGettingStarted()
{
  QDesktopServices::openUrl(QUrl("https://ra3xdh.github.io/pdf/qucs_s_tutorial.pdf"));
}

// ---------------------------------------------------------------------
// Is called when the find action is triggered.
void QucsApp::slotEditFind()
{
  SearchDia->initSearch(DocumentTab->currentWidget(),
      ((TextDoc *)DocumentTab->currentWidget())->textCursor().selectedText(), false);
}

// --------------------------------------------------------------
void QucsApp::slotChangeProps()
{
  QWidget *Doc = DocumentTab->currentWidget();
  if(isTextDocument(Doc)) {
    ((TextDoc*)Doc)->viewport()->setFocus();

    SearchDia->initSearch(Doc,
        ((TextDoc *)Doc)->textCursor().selectedText(), true);
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
  slotHideEdit(); // disable text edit of component property

  if(ProjName.isEmpty()) {
    QMessageBox::critical(this, tr("Error"), tr("No project open!"));
    return;
  }


  QStringList List = QFileDialog::getOpenFileNames(this, tr("Select files to copy"),
    lastDir.isEmpty() ? QStringLiteral(".") : lastDir, QucsFileFilter);

  if(List.isEmpty()) {
    statusBar()->showMessage(tr("No files copied."), 2000);
    return;
  }


  char *Buffer = (char*)malloc(0x10000);
  if(!Buffer) return;  // should never happen

  QStringList FileList = List;  // make a copy as recommended by Qt
  QStringList::Iterator it = FileList.begin();
  QFileInfo Info(*it);
  lastDir = Info.absolutePath();  // remember last directory

  // copy all files to project directory
  int Num;
  QFile origFile, destFile;
  while(it != FileList.end()) {
    Info.setFile(*it);
    origFile.setFileName(*it);
    destFile.setFileName(QucsSettings.QucsWorkDir.absolutePath() +
                     QDir::separator() + Info.fileName());

    if(!origFile.open(QIODevice::ReadOnly)) {
      QMessageBox::critical(this, tr("Error"), tr("Cannot open \"%1\" !").arg(*it));
      it++;
      continue;
    }

    if(destFile.exists())
      if(QMessageBox::information(this, tr("Overwrite"),
           tr("File \"%1\" already exists.\nOverwrite ?").arg(*it),
           QMessageBox::Yes|QMessageBox::No)
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
      Num = origFile.read(Buffer, 0x10000);
      if(Num < 0) {
        QMessageBox::critical(this, tr("Error"), tr("Cannot read \"%1\" !").arg(*it));
        break;
      }
      Num = destFile.write(Buffer, Num);
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
  slotUpdateTreeview();
  statusBar()->showMessage(tr("Ready."));
}

// -----------------------------------------------------------
void QucsApp::slotCursorLeft(bool left)
{
  if(!editText->isHidden()) return;  // for edit of component property ?

  Schematic *Doc = (Schematic*)DocumentTab->currentWidget();
  const auto selection = Doc->currentSelection();

  const auto totalCount = selection.components.size()
       + selection.wires.size()
       + selection.paintings.size()
       + selection.diagrams.size()
       + selection.labels.size()
       + selection.markers.size();

  if (totalCount == selection.markers.size()) {
      Doc->markerLeftRight(left, selection.markers);
  }

  else if (totalCount == 0) {
    left
      ? Doc->scrollLeft(Doc->horizontalScrollBar()->singleStep())
      : Doc->scrollRight(Doc->horizontalScrollBar()->singleStep());
    Doc->viewport()->update();
    return;
  }

  // random selection. move all of them
  const auto dx = left ? -Doc->getGridX() : Doc->getGridX();
  const auto mover = [dx](Element* e) { e->moveCenter(dx, 0); };
  std::ranges::for_each(selection.paintings, mover);
  std::ranges::for_each(selection.diagrams, mover);
  std::ranges::for_each(selection.labels, mover);
  std::ranges::for_each(selection.components, mover);
  std::ranges::for_each(selection.wires, mover);
  std::ranges::for_each(selection.nodes, mover);
  Doc->healAfterKeyboardMutation();
  Doc->viewport()->update();
}

// -----------------------------------------------------------
void QucsApp::slotCursorUp(bool up)
{
  if(editText->isHidden()) {  // for edit of component property ?
  }else if(up){
    if(view->MAx3 == 0) return;  // edit component namen ?
    Component *pc = (Component*)view->focusElement;
    Property *pp = pc->Props.at(view->MAx3-1);  // current property
    int Begin = pp->Description.indexOf('[');
    if(Begin < 0) return;  // no selection list ?
    int End = pp->Description.indexOf(editText->text(), Begin); // current
    if(End < 0) return;  // should never happen
    End = pp->Description.lastIndexOf(',', End);
    if(End < Begin) return;  // was first item ?
    End--;
    int Pos = pp->Description.lastIndexOf(',', End);
    if(Pos < Begin) Pos = Begin;   // is first item ?
    Pos++;
    if(pp->Description.at(Pos) == ' ') Pos++; // remove leading space
    editText->setText(pp->Description.mid(Pos, End-Pos+1));
    editText->selectAll();
    return;
  }else{ // down
    if(view->MAx3 == 0) return;  // edit component namen ?
    Component *pc = (Component*)view->focusElement;
    Property *pp = pc->Props.at(view->MAx3-1);  // current property
    int Pos = pp->Description.indexOf('[');
    if(Pos < 0) return;  // no selection list ?
    Pos = pp->Description.indexOf(editText->text(), Pos); // current list item
    if(Pos < 0) return;  // should never happen
    Pos = pp->Description.indexOf(',', Pos);
    if(Pos < 0) return;  // was last item ?
    Pos++;
    if(pp->Description.at(Pos) == ' ') Pos++; // remove leading space
    int End = pp->Description.indexOf(',', Pos);
    if(End < 0) {  // is last item ?
      End = pp->Description.indexOf(']', Pos);
      if(End < 0) return;  // should never happen
    }
    editText->setText(pp->Description.mid(Pos, End-Pos));
    editText->selectAll();
    return;
  }

  Schematic *Doc = (Schematic*)DocumentTab->currentWidget();
  const auto selection = Doc->currentSelection();

  const auto totalCount = selection.components.size()
       + selection.wires.size()
       + selection.paintings.size()
       + selection.diagrams.size()
       + selection.labels.size()
       + selection.markers.size();

  if (totalCount == selection.markers.size()) {
      Doc->markerUpDown(up, selection.markers);
  }

  else if (totalCount == 0) {
    up
      ? Doc->scrollUp(Doc->verticalScrollBar()->singleStep())
      : Doc->scrollDown(Doc->verticalScrollBar()->singleStep());
    Doc->viewport()->update();
    return;
  }

  // random selection. move all of them
  const auto dy = up ? -Doc->getGridY() : Doc->getGridY();
  const auto mover = [dy](Element* e) { e->moveCenter(0, dy); };
  std::ranges::for_each(selection.paintings, mover);
  std::ranges::for_each(selection.diagrams, mover);
  std::ranges::for_each(selection.labels, mover);
  std::ranges::for_each(selection.components, mover);
  std::ranges::for_each(selection.wires, mover);
  std::ranges::for_each(selection.nodes, mover);
  Doc->healAfterKeyboardMutation();
  Doc->viewport()->update();
}

// -----------------------------------------------------------
// Is called if user clicked on component text of if return is
// pressed in the component text QLineEdit.
// In "view->MAx3" is the number of the current property.
void QucsApp::slotApplyCompText()
{
  QFont f = QucsSettings.font;
  Schematic *Doc = (Schematic*)DocumentTab->currentWidget();
  f.setPointSizeF( Doc->getScale() * float(f.pointSize()) );
  editText->setFont(f);

  Component *const component = dynamic_cast<Component*>(view->focusElement);
  if(!component) return;  // should never happen
  view->MAx1 = component->cx + component->tx;
  view->MAy1 = component->cy + component->ty;

  // Here is a bit of *magic* and implicit coupling: the value of view->MAx3
  // comes from Component::getTextSelected, and it's equal 0 when component
  // name is clicked, N + 1 when Nth component property is clicked.
  const int component_text_index = view->MAx3;
  const bool is_name = component_text_index == 0;

  Property *const component_property = !is_name
                                     ? component->Props.at(component_text_index - 1)
                                     : nullptr;

  if (editText->isVisible()) {   // is called the first time ?
    if (is_name) {
      const auto new_name{editText->text()};

      if (!new_name.isEmpty() && component->Name != new_name) {

        bool is_unique = std::none_of(
          Doc->a_Components->begin(),
          Doc->a_Components->end(),
          [&new_name](const Component* other) { return other->Name == new_name; }
        );

        if (is_unique) {
          component->Name = new_name;
          Doc->setChanged(true, true);  // only one undo state
        }
      }

    }
    else if (component_property) {  // property was applied
      if (component_property->Value != editText->text()) {
        component_property->Value = editText->text();
        Doc->recreateComponent(component);  // because of "Num" and schematic symbol
        Doc->setChanged(true, true); // only one undo state
      }
    }
  }

  const QString s = is_name
                ? component->Name
                : component_property->Value;

  editText->setReadOnly(false);
  QPoint editTextTopLeft;
  if (component_property) {  // is it a property ?
    editTextTopLeft = Doc->modelToViewport(QPoint{component->cx, component->cy} + component_property->boundingRect().topLeft());
    editTextTopLeft.rx() += editText->fontMetrics().boundingRect(component_property->Name + "=" + '\u0020').width();

    if(component_property->Description.indexOf('[') >= 0)  // is selection list ?
      editText->setReadOnly(true);
    Expr_CompProp.setPattern("[^\"]*");
  }
  else { // it is the component name
    Expr_CompProp.setPattern("[\\w_]+");
    editTextTopLeft = Doc->modelToViewport(QPoint{component->cx + component->tx, component->cy + component->ty});
  }

  {
    auto size = editText->fontMetrics().boundingRect(s ).size();
    size.rwidth() += editText->fontMetrics().averageCharWidth();
    editText->setFixedSize(size);
  }

  view->MAx2 = editTextTopLeft.x();
  view->MAy2 = editTextTopLeft.y();

  Val_CompProp.setRegularExpression(Expr_CompProp);
  editText->setValidator(&Val_CompProp);

  editText->setText(s);
  editText->setStyleSheet("color: black; background-color: " + QucsSettings.BGColor.name());
  editText->setFocus();
  editText->selectAll();
  editText->setParent(Doc->viewport());
  editText->move(view->MAx2, view->MAy2);
  editText->show();
  //editText->reparent(Doc->viewport(), 0, QPoint(view->MAx2, view->MAy2), true);
}

// -----------------------------------------------------------
// Is called if the text of the property edit changed, to match
// the width of the edit field.
void QucsApp::slotResizePropEdit(const QString& t)
{
  editText->resize(editText->fontMetrics().boundingRect(t).width()+4,
                   editText->fontMetrics().lineSpacing());
}

// -----------------------------------------------------------
void QucsApp::slotCreateLib()
{
  slotHideEdit(); // disable text edit of component property

  if(ProjName.isEmpty()) {
    QMessageBox::critical(this, tr("Error"), tr("Please open project with subcircuits!"));
    return;
  }

  LibraryDialog *d = new LibraryDialog(this);
  d->fillSchematicList(Content->exportSchematic());
  auto r = d->exec();
  if (r != QDialog::Accepted) {
    fillLibrariesTreeView();
  }
}

// -----------------------------------------------------------
void QucsApp::slotImportData()
{
  slotHideEdit(); // disable text edit of component property

  QString import_dir = ".";
  if (!ProjName.isEmpty()) {
      import_dir = QucsSettings.QucsWorkDir.absolutePath();
  } else {
      QString dname;
      if (isTextDocument(DocumentTab->currentWidget())) {
          TextDoc *doc = (TextDoc *)DocumentTab->currentWidget();
          dname = doc->getDocName();
      } else {
          Schematic *doc = (Schematic *)DocumentTab->currentWidget();
          dname = doc->getDocName();
      }
      QFileInfo inf(dname);
      if (inf.exists()) {
          import_dir = inf.absolutePath();
      }
  }

  ImportDialog *d = new ImportDialog(this);
  d->setImportDir(import_dir);
  if(d->exec() == QDialog::Accepted)
    slotUpdateTreeview();
}

// -----------------------------------------------------------
void QucsApp::slotExportGraphAsCsv()
{
  slotHideEdit(); // disable text edit of component property

  for(;;) {
    if(view->focusElement)
      if(view->focusElement->Type == isGraph)
        break;

    QMessageBox::critical(this, tr("Error"), tr("Please select a diagram graph!"));
    return;
  }

  /*QString s = Q3FileDialog::getSaveFileName(
     lastDir.isEmpty() ? QStringLiteral(".") : lastDir,
     tr("CSV file")+" (*.csv);;" + tr("Any File")+" (*)",
     this, 0, tr("Enter an Output File Name"));
     */
  QString s = QFileDialog::getSaveFileName(this, tr("Enter an Output File Name"),
    lastDir.isEmpty() ? QStringLiteral(".") : lastDir, tr("CSV file")+" (*.csv);;" + tr("Any File")+" (*)");

  if(s.isEmpty())
    return;

  QFileInfo Info(s);
  lastDir = Info.absolutePath();  // remember last directory
  if(Info.suffix().isEmpty())
    s += ".csv";

  QFile File(s);
  if(File.exists())
    if(QMessageBox::information(this, tr("Info"),
          tr("Output file already exists!")+"\n"+tr("Overwrite it?"),
          QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
      return;

  if(!File.open(QIODevice::WriteOnly)) {
    QMessageBox::critical(this, QObject::tr("Error"),
                          QObject::tr("Cannot create output file!"));
    return;
  }

  QTextStream Stream(&File);


  DataX const *pD;
  Graph *g = (Graph*)view->focusElement;
  // First output the names of independent and dependent variables.
  for(unsigned ii=0; (pD=g->axis(ii)); ++ii)
    Stream << '\"' << pD->Var << "\";";
  Stream << "\"r " << g->Var << "\";\"i " << g->Var << "\"\n";


  int n, m;
  double *py = g->cPointsY;
  int Count = g->countY * g->axis(0)->count;
  for(n = 0; n < Count; n++) {
    m = n;
    for(unsigned ii=0; (pD=g->axis(ii)); ++ii) {
      Stream << *(pD->Points + m%pD->count) << ';';
      m /= pD->count;
    }

    Stream << *(py) << ';' << *(py+1) << '\n';
    py += 2;
  }

  File.close();
}


void QucsApp::slotOpenRecent()
{
  QAction *action = qobject_cast<QAction *>(sender());
  if (action) {
    gotoPage(action->data().toString());
    updateRecentFilesList(action->data().toString());
  }
}

void QucsApp::slotUpdateRecentFiles()
{
  QMutableStringListIterator it(QucsSettings.RecentDocs);
  while(it.hasNext()) {
    if (!QFile::exists(it.next())) {
        it.remove();
    }
  }

  for (int i = 0; i < MaxRecentFiles; ++i) {
    if (i < QucsSettings.RecentDocs.size()) {
      fileRecentAction[i]->setText(QucsSettings.RecentDocs[i]);
      fileRecentAction[i]->setData(QucsSettings.RecentDocs[i]);
      fileRecentAction[i]->setVisible(true);
    } else {
      fileRecentAction[i]->setVisible(false);
    }
  }
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
      // dialog should populate according to checkboxes
      // build vaComponents QMap

      // remove all previously registered modules
      QMutableHashIterator<QString, Module *> it( Module::Modules );
      while(it.hasNext()) {
        it.next();
        if (it.value()->category == QObject::tr("verilog-a user devices")) {
          it.remove();
        }
      }

      if (! Module::vaComponents.isEmpty()) {
        // Register whatever is in Module::vaComponents
        //Module::registerDynamicComponents();

        // update the combobox, set new category in view
        // pick up new category 'verilog-a user components' from `Module::category`
        //set new category into view
        QucsApp::fillComboBox(true);
        CompChoose->setCurrentIndex(CompChoose->count()-1);
        slotSetCompView(CompChoose->count()-1);

        // icons of dynamically registered components ready to be dragged
      }
      else {
        // remove any previously registered icons from the listview
        int foundCat = CompChoose->findText(QObject::tr("verilog-a user devices"));
        if (foundCat != -1) {
          CompChoose->setCurrentIndex(foundCat);
          CompComps->clear();
        }
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

    if (QucsSettings.DefaultSimulator == spicecompat::simNgspice) {
        buildWithOpenVAF();
        return;
    }

    messageDock->builderTabs->setTabIcon(0,QPixmap());
    messageDock->builderTabs->setTabText(0,tr("admsXml"));
    messageDock->builderTabs->setTabIcon(1,QPixmap());
    messageDock->builderTabs->setTabText(1,tr("Compiler"));
    messageDock->msgDock->setWindowTitle(tr("admsXml Dock"));


    QString make;

#if defined(_WIN32) || defined(__MINGW32__)
    make = "mingw32-make.exe";    // must be on the path!
#else
    make = "make";                // must be on the path!
#endif
    QFileInfo inf(QucsSettings.Qucsator);
    QString QucsatorPath = inf.path()+QDir::separator();

    QDir prefix = QDir(QucsatorPath+"../");

    QDir include = QDir(QucsatorPath+"../include/qucs-core");

    QString workDir = QucsSettings.QucsWorkDir.absolutePath();

    // need to cd into project to make sure output is dropped there?
    // need to cd - into previous location?
    QDir::setCurrent(workDir);

    QProcess *builder = new QProcess();
    builder->setProcessChannelMode(QProcess::MergedChannels);
    // get current va document
    QucsDoc *Doc = getDoc();
    QString vaModule = Doc->fileBase(Doc->getDocName());

    QString admsXml = QucsSettings.AdmsXmlBinDir.canonicalPath();

#if defined(_WIN32) || defined(__MINGW32__)
    admsXml = QDir::toNativeSeparators(admsXml+"/"+"admsXml.exe");
#else
    admsXml = QDir::toNativeSeparators(admsXml+"/"+"admsXml");
#endif

    // admsXml emits C++
    QStringList Arguments;
    Arguments << "-f" <<  QDir::toNativeSeparators(include.absoluteFilePath("va2cpp.makefile"))
              << QStringLiteral("ADMSXML=%1").arg(admsXml)
              << QStringLiteral("PREFIX=%1").arg(QDir::toNativeSeparators(prefix.absolutePath()))
              << QStringLiteral("MODEL=%1").arg(vaModule);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("PATH", env.value("PATH") );
    builder->setProcessEnvironment(env);

    // prepend command to log
    QString cmdString = QStringLiteral("%1 %2\n").arg(make, Arguments.join(" "));
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
              << QStringLiteral("PREFIX=\"%1\"").arg(QDir::toNativeSeparators(prefix.absolutePath()))
              << QStringLiteral("PROJDIR=\"%1\"").arg(QDir::toNativeSeparators(workDir))
              << QStringLiteral("MODEL=%1").arg(vaModule);

    // prepend command to log
    cmdString = QStringLiteral("%1 %2\n").arg(make, Arguments.join(" "));
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


void QucsApp::buildWithOpenVAF()
{
    messageDock->builderTabs->setTabIcon(0,QPixmap());
    messageDock->builderTabs->setTabText(0,tr("OpenVAF"));
    messageDock->msgDock->setWindowTitle(tr("OpenVAF Dock"));


    QString workDir = QucsSettings.QucsWorkDir.absolutePath();
    QDir::setCurrent(workDir);

    QProcess *builder = new QProcess();
    builder->setProcessChannelMode(QProcess::MergedChannels);
    // get current va document
    QucsDoc *Doc = getDoc();
    QString vaModule = Doc->getDocName();

    QString openVAF = QucsSettings.OpenVAFExecutable;

    // admsXml emits C++
    QStringList Arguments;
    Arguments<<vaModule;

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("PATH", env.value("PATH") );
    builder->setProcessEnvironment(env);

    // prepend command to log
    QString cmdString = QStringLiteral("%1 %2\n").arg(openVAF, Arguments.join(" "));
    messageDock->admsOutput->appendPlainText(cmdString);

    qDebug() << "Command :" << openVAF << Arguments.join(" ");
    builder->start(openVAF, Arguments);

    // admsXml seems to communicate all via stdout, or is it because of make?
    QString vaStatus;
    if (!builder->waitForFinished()) {
        vaStatus = builder->errorString();
        qDebug() << "OpenVAF failed:" << vaStatus;
    }
    else {
        vaStatus = builder->readAll();
        qDebug() << "OpenVAF stdout"  << vaStatus;
    }

    delete builder;

    // push make output to message dock
    messageDock->admsOutput->appendPlainText(vaStatus);

    // shot the message docks
    messageDock->msgDock->show();
}

// ----------------------------------------------------------
void QucsApp::slotHelpAbout()
{
  AboutDialog *ad = new AboutDialog(this);
  ad->exec();
}

// vim:ts=8:sw=2:noet
