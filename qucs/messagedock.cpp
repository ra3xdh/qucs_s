/***************************************************************************
                            messagedock.cpp
                            ---------------
    begin                : Tue Mar 11 2014
    copyright            : (C) 2014 by Guilherme Brondani Torri
    email                : guitorri AT gmail DOT com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "messagedock.h"

#include "main.h"
#include "qucsdoc.h"
#include "textdoc.h"

#include <QDockWidget>
#include <QDebug>

/*!
 * \file messagedock.cpp
 * \brief Definition of the MessageDock class.
 */

/*!
 * \brief MessageDock::MessageDock constructor
 * \param App_ is the parent class
 * It creates two docked text fields to hold the output of make called
 * over admsXml and the C++ compiler used to build the Verilog-A
 * dynamic loaded libraries.
 * \see QucsApp::slotBuildModule() for the make output assignment.
 */
MessageDock::MessageDock(QucsApp *App_): QWidget()
{

    builderTabs = new QTabWidget();
    builderTabs->setTabPosition(QTabWidget::South);

    // 1) add a dock for the adms messages
    admsOutput = new QPlainTextEdit();
    admsOutput->setReadOnly(true);

    builderTabs->insertTab(0,admsOutput,tr("admsXml"));


    // 2) add a dock for the cpp compiler messages
    cppOutput = new QPlainTextEdit();
    cppOutput->setReadOnly(true);

    builderTabs->insertTab(1,cppOutput,tr("Compiler"));

    msgDock = new QDockWidget(tr("admsXml Dock"));
    msgDock->setWidget(builderTabs);
    App_->addDockWidget(Qt::BottomDockWidgetArea, msgDock);

    // start hidden
    msgDock->hide();

    // monitor the amds output
    connect(admsOutput,SIGNAL(textChanged()), this, SLOT(slotAdmsChanged()));
    // monitor the compiler output
    connect(cppOutput,SIGNAL(textChanged()), this, SLOT(slotCppChanged()));
    // check out if cursor over 'fail' line
    connect(admsOutput, SIGNAL(cursorPositionChanged()), this, SLOT(slotCursor()));
}

/*!
 * \brief MessageDock::reset clear the text and tab icons
 */
void MessageDock::reset()
{
    admsOutput->clear();
    cppOutput->clear();

    builderTabs->setTabIcon(0,QPixmap());
    builderTabs->setTabIcon(1,QPixmap());
}

/*!
 * \brief MessageDock::slotAdmsChanged monitors the adms log, update tab icon
 */
void MessageDock::slotAdmsChanged()
{
    // look for [fatal..] output of admsXml
    // get line from either
    //  * [fatal..] ./3ph_vs.va:34:42: analog function '$abstime' is undefined
    //  * [fatal..] ./mypotentiometer.va: during lexical analysis syntax error at line 33 --
    //  * [fatal..] [./mypotentiometer.va:63:1]: at 'Rad_Angle':

    // \todo can we change the mouse cursor over the highlighted lines?
    // A Qt::PointingHandCursor would be nice.
    QString logContents = admsOutput->toPlainText();
    QStringList lines = logContents.split("\n");

    bool error = false;

    QList<QTextEdit::ExtraSelection> extraSelections;
    for (int i = 0; i < lines.size(); ++i) {
        QString line = lines[i];

        if (line.contains("[fatal..]",Qt::CaseSensitive)) {
            // get cursor for the line
            int pos = admsOutput->document()->findBlockByLineNumber(i).position();
            QTextCursor cursor = admsOutput->textCursor();
            cursor.setPosition(pos);

            // highlight 'fatal' lines on the log
            QTextEdit::ExtraSelection selection;
            QColor lineColor = QColor(Qt::yellow).lighter(160);
            selection.format.setBackground(lineColor);
            selection.format.setProperty(QTextFormat::FullWidthSelection, true);
            selection.cursor = cursor;
            extraSelections.append(selection);

            error = true;
        }
        else if (line.contains("[error..]",Qt::CaseSensitive)) {
            // Do something with error?
             error = true;
        }
        else if (line.contains("*** No rule to make target",Qt::CaseSensitive)) {
            // Do something with error?
             error = true;
        }
    }

    // highlight all the fatal warnings
    admsOutput->setExtraSelections(extraSelections);

    // Change adms tab icon
    if (error)
         builderTabs->setTabIcon(0,QPixmap(":/bitmaps/error.png"));
    else
         builderTabs->setTabIcon(0,QPixmap(":/bitmaps/tick.png"));
}

/*!
 * \brief MessageDock::slotCppChanged monitors the compiler log, update tab icon
 */
void MessageDock::slotCppChanged()
{
    QString logContents = cppOutput->toPlainText();

    bool error = false;

    if (logContents.contains("*** No rule to make target")) {
        error = true;
    }
    else if (logContents.contains("error",Qt::CaseInsensitive)) {
        error = true;
    }

    // Change compiler tab icon
    if (error)
         builderTabs->setTabIcon(1,QPixmap(":/bitmaps/error.png"));
    else
         builderTabs->setTabIcon(1,QPixmap(":/bitmaps/tick.png"));
}

/*!
 * \brief MessageDock::slotCursor
 */
void MessageDock::slotCursor()
{
    qWarning()  << admsOutput->textCursor().blockNumber();
    int gotoLine = -1;
    QString line =  admsOutput->textCursor().block().text();
    if (line.contains("[fatal..]",Qt::CaseSensitive)) {
        // \todo improve the parsing of line
        // try to find line number: ":34:"
        if (line.contains(":",Qt::CaseSensitive)) {
            int a,b;
            a = line.indexOf(":")+1;
            b = line.indexOf(":",a);
            gotoLine = line.mid(a,b-a).trimmed().toInt();
            qWarning() << "goto line " << gotoLine;
        }

        // try to find line number: "syntax error at line 33 --"
        if (line.contains("syntax error ",Qt::CaseSensitive)) {
            int a,b;
            a = line.indexOf("at line");
            b = line.indexOf("--",a);
            gotoLine = line.mid(a+7,b-a-7).trimmed().toInt();
            qWarning() << "goto line " << gotoLine;
        }
    }

  // \todo set hightlight in QucsDoc Verilog-A file?
  // move cursor? addt line number? highliht line number? set in focus

    /*
     * add slot to TextDoc
     * it takes the gotoLine
     * hightlings the line
     * QucsApp::getDoc() //current should be a TextDoc
     */
//    QucsDoc *foo =  QucsMain->getDoc();
//    qWarning() << foo->DocName;

    if (gotoLine >= 0) {

        // \todo it will mark whatever document is open. parse the model file
        // name from the fatal message and ->findDoc instead of ->getDoc?

        // grab active text document
        TextDoc * d = (TextDoc*)QucsMain->getDoc();

        QTextCursor cursor = d->textCursor();
        int pos = d->document()->findBlockByLineNumber(gotoLine-1).position();
        cursor.setPosition(pos);

        // Highligt a give line
        QList<QTextEdit::ExtraSelection> extraSelections;
        QTextEdit::ExtraSelection selection;
        QColor lineColor = QColor(Qt::yellow).lighter(160);
        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = cursor;

        // get existing
        extraSelections.append(d->extraSelections());

        // append new
        extraSelections.append(selection);

        // color the selections on the active document
        d->setExtraSelections(extraSelections);

        //move focus to VA code
        d->setFocus();
        //move cursor to highlighted line
//        d->setCursor(d->document()->);
        d->setTextCursor(cursor);
    }

    /// \todo add line numbers to TextDoc, highlight as the cursor moves
    /// problem that now the cursor paints over the failed line.
    /// can we have multiple selections?


}



