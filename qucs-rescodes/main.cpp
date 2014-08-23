/***************************************************************************
                               main.cpp
                             ------------------
    begin                : Mar 2012
    copyright            : (C) 2012 by Sudhakar.M.K
    email                : sudhakar.m.kumar@gmail.com
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
#include <config.h>
#endif
#ifndef QUCS_RESCODES_MAIN_CPP
#define QUCS_RESCODES_MAIN_CPP
#include <qapplication.h>
#include <qhbox.h>
#include <qpushbutton.h>
#include <qfont.h>
#include <qlayout.h>
#include <qgrid.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qimage.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qclipboard.h>
#include <vector>
#include <string>

#include "classproto.h"
#include "myresistancebox.h"
#include "mycolorbox.h"
#include "qresistor.h"
#include "mywidget.h"
#include "helpdialog.h"
//------------------------class member declarations for MyWidget---------------------------------//

MyWidget::MyWidget( QWidget *parent, const char *name )
: QWidget( parent, name )
{
	setCaption("Color Codes");
	setIcon(QPixmap(QString(BITMAPDIR)+"big.qucs.xpm"));
	 // --------  create menubar  -------------------
	QPopupMenu *fileMenu = new QPopupMenu();
	fileMenu->insertItem(tr("E&xit"), qApp, SLOT(quit()), CTRL+Key_Q);

	QPopupMenu *helpMenu = new QPopupMenu();
	helpMenu->insertItem(tr("Help..."), this, SLOT(slotHelpIntro()), Key_F1);
	helpMenu->insertSeparator();
	helpMenu->insertItem(
			tr("&About ResistorCodes..."), this, SLOT(slotHelpAbout()), 0);
	helpMenu->insertItem(tr("About Qt..."), this, SLOT(slotHelpAboutQt()), 0);

	QMenuBar *bar = new QMenuBar(this);
	bar->insertItem(tr("&File"), fileMenu);
	bar->insertSeparator ();
	bar->insertItem(tr("&Help"), helpMenu);

	res= new QResistor();
	//--------------------resistance displayin ui ---------------------------------//
	resBox = new MyResistanceBox (this, "resBox");

	connect(res, SIGNAL(valueModified(QResistor*)),resBox,SLOT(update(QResistor*)));
	//-------------------color displaying ui---------------------------------------------//
	colorBox = new MyColorBox(this, "colorBox");

	connect(res, SIGNAL(valueModified(QResistor*)),colorBox,SLOT(update(QResistor*)));
	
	//-------------------paste the configuration to clipboard--------------------------------------------//
	connect(res, SIGNAL(valueModified(QResistor*)),this,SLOT(slotConfiguration()));
	//-------------------switching buttons ui--------------------------------------//
	QHBox *buttonBox = new QHBox(this,"buttonBox");
	
	QPushButton *calcColor = new QPushButton(QPixmap(QImage(QString(BITMAPDIR)+"next.png"))," To Colors", buttonBox, "calcColor" );
	connect(calcColor, SIGNAL(clicked()),this,SLOT(setResistanceValue()));

	QPushButton *calcResistance = new QPushButton(QPixmap(QImage(QString(BITMAPDIR)+"previous.png"))," To Resistance", buttonBox, "calcResistance" );
	connect(calcResistance, SIGNAL(clicked()),this,SLOT(setColorValue()));

	QPushButton *quit = new QPushButton( "Quit", buttonBox, "quit" );
//	quit->setFont( QFont( "Times", 18, QFont::Bold ) );
	connect( quit, SIGNAL(clicked()), qApp, SLOT(quit()) );


	//--------------------packing all of them together---------------------------------------//
	QGridLayout *grid = new QGridLayout( this, 4, 1, 10 );
	//3x1, 10 pixel border

	QWidget *Space = new QWidget(this);   // reserve space for menubar
	Space->setFixedSize(1, bar->height());
	grid->addWidget(Space, 0,0);

	grid->addWidget( resBox, 1, 0 );
	grid->addWidget( buttonBox, 2, 0 );
	grid->addWidget( colorBox, 3, 0 );

}
void MyWidget :: setResistanceValue()
{
	res->QResistorModify(resBox->enteredValue(),resBox->enteredTolerance());
}
void MyWidget :: setColorValue()
{
	res->QResistorModify(colorBox->enteredValue(),colorBox->enteredTolerance());
}
// ************************************************************
void MyWidget::slotHelpAbout()
{
	QMessageBox::about(this, tr("About..."),
			tr("\nResistor color code computation program\n")+
			tr("Copyright (c) 2012 by")+
			"\nArun I, Sudhakar M K \n"
			"\nThis is free software; see the source for copying conditions."
			"\nThere is NO warranty; not even for MERCHANTABILITY or "
			"\nFITNESS FOR A PARTICULAR PURPOSE.\n\n");
}

// ************************************************************
void MyWidget::slotHelpAboutQt()
{
	QMessageBox::aboutQt(this, tr("About Qt"));
}

// ************************************************************
void MyWidget::slotHelpIntro()
{
	HelpDialog *d = new HelpDialog(this);
	d->show();
}

// ************************************************************
void MyWidget::slotConfiguration()
{
	// create the Qucs schematic
	QString *s = new QString("<Qucs Schematic ");
	*s += PACKAGE_VERSION;
	*s += ">\n";
	*s += "<Components>\n";
	*s += QString("<R R1 1 370 120 -26 15 0 0 \"%1 %2Ohm\" 1 \"26.85\" 0 \"0.0\" 0 \"0.0\" 0 \"26.85\" 0 \"US\" 0>\n").arg(QString::number(res->getResistance()),QString(QChar(res->getModifier())));
	*s += "</components>\n";
	*s += "<Wires>\n";
	*s += "</Wires>\n";
	*s += "<Diagrams>\n";
	*s += "</Diagrams>\n";
	*s += "<Paintings>\n";
	*s += "</Paintings>\n";
	
	QClipboard* cb = QApplication::clipboard();
	cb->setText(*s);
	delete(s);

}

int main( int argc, char **argv )
{
	QApplication a( argc, argv );

	MyWidget w;
//	w.setGeometry( 100, 100, 500, 355 );
//	w.setFixedSize(500, 355 );
	a.setMainWidget( &w );
	w.show();
	return a.exec();
}
#endif
