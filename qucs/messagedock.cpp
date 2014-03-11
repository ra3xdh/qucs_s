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

    // add a dock for the adms messages
    QWidget *admsMessage = new QWidget();
    QVBoxLayout *admsLayout = new QVBoxLayout();

    admsOutput = new QTextEdit();

//    admsOutput->setText( vaStatus );

    admsLayout->addWidget(admsOutput);
    admsMessage->setLayout(admsLayout);

    admsDock = new QDockWidget(tr("admsXml"), this);

    admsDock->setWidget(admsMessage);
    App_->addDockWidget(Qt::BottomDockWidgetArea, admsDock);


    // add a dock for the cpp compiler messages
    QWidget *cppMessage = new QWidget();
    QVBoxLayout *cppLayout = new QVBoxLayout();

    cppOutput = new QTextEdit();

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
}
