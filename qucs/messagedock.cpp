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

#include <QVBoxLayout>
#include <QTextEdit>

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

    // 1) add a dock for the adms messages
    QWidget *admsMessage = new QWidget();
    QVBoxLayout *admsLayout = new QVBoxLayout();

    admsOutput = new QPlainTextEdit();
    admsOutput->setReadOnly(true);

//    admsOutput->setText( vaStatus );

    admsLayout->addWidget(admsOutput);
    admsMessage->setLayout(admsLayout);

    admsDock = new QDockWidget(tr("admsXml"), this);

    admsDock->setWidget(admsMessage);
    App_->addDockWidget(Qt::BottomDockWidgetArea, admsDock);


    // 2) add a dock for the cpp compiler messages
    QWidget *cppMessage = new QWidget();
    QVBoxLayout *cppLayout = new QVBoxLayout();

    cppOutput = new QPlainTextEdit();
    cppOutput->setReadOnly(true);

//    cppOutput->setText( cppStatus );

    cppLayout->addWidget(cppOutput);
    cppMessage->setLayout(cppLayout);

    cppDock = new QDockWidget(tr("Compiler"), this);

    cppDock->setWidget(cppMessage);
    App_->addDockWidget(Qt::BottomDockWidgetArea, cppDock);

    App_->tabifyDockWidget(admsDock, cppDock);

    //start hidden
    admsDock->hide();
    cppDock->hide();

    // monitor the amds output
    connect(admsOutput,SIGNAL(textChanged()), this, SLOT(slotAdmsChanged()));
    // check out if curso over 'fail' line
    connect(admsOutput, SIGNAL(cursorPositionChanged()), this, SLOT(slotCursor()));
}

/*!
 * \brief MessageDock::slotAdmsChanged
 */
void MessageDock::slotAdmsChanged()
{
    qWarning() << "slotAdmsChanged";
//                  admsOutput->toPlainText().count("make");

    // look for [fatal..] output of admsXml
    // get line from either
    //  * [fatal..] ./3ph_vs.va:34:42: analog function '$abstime' is undefined
    //  * [fatal..] ./mypotentiometer.va: during lexical analysis syntax error at line 33 --
    //  * [fatal..] [./mypotentiometer.va:63:1]: at 'Rad_Angle':

    // \todo can we change the mouse cursor over the highlighted lines?
    // A Qt::PointingHandCursor would be nice.
    QString plainTextEditContents = admsOutput->toPlainText();
    QStringList lines = plainTextEditContents.split("\n");

    QList<QTextEdit::ExtraSelection> extraSelections;
    for (int i = 0; i < lines.size(); ++i) {

        QString line = lines[i];

        if (line.contains("[fatal..]",Qt::CaseSensitive)) {
//            qWarning() << "fatal at line" << i;
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
        }
    }
    // highlight all the fatal warnings
    admsOutput->setExtraSelections(extraSelections);
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
            gotoLine = line.mid(a,b-a).stripWhiteSpace().toInt();
            qWarning() << "goto line " << gotoLine;
        }

        // try to find line number: "syntax error at line 33 --"
        if (line.contains("syntax error ",Qt::CaseSensitive)) {
            int a,b;
            a = line.indexOf("at line");
            b = line.indexOf("--",a);
            gotoLine = line.mid(a+7,b-a-7).stripWhiteSpace().toInt();
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


}



