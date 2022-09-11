/***************************************************************************
                               main.cpp
                             ------------------
    begin                : Mar 2012
    copyright            : (C) 2012 by Sudhakar.M.K
    email                : sudhakar.m.kumar@gmail.com
    copyright            : (C) 2016, Qucs team (see AUTHORS file)

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "classproto.h"
#include "myresistancebox.h"
#include "mycolorbox.h"
#include "qresistor.h"
#include "mywidget.h"
#include "helpdialog.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <QApplication>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QPixmap>
#include <QString>
#include <QImage>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QMessageBox>
#include <QClipboard>
#include <QTranslator>
#include <QLocale>
#include <QSettings>
#include <QDir>

#include <vector>
#include <string>
//------------------------class member declarations for MyWidget---------------------------------//

/* setup the GUI */
MyWidget::MyWidget()
{
  setWindowTitle("Qucs Resistor Color Code " PACKAGE_VERSION);

  // icons are handled differently on OSX
#ifndef __APPLE__
  setWindowIcon(QPixmap(":/bitmaps/big.qucs.xpm"));
#endif

  // --------  create menubar  -------------------
  QAction *fileExit = new QAction(tr("&Exit"), this);
  fileExit->setShortcut(Qt::CTRL+Qt::Key_Q);
  connect(fileExit, SIGNAL(triggered(bool)), qApp, SLOT(quit()));

  QMenu *fileMenu = new QMenu(tr("&File"));
  fileMenu->addAction(fileExit);

  QAction *help = new QAction(tr("Help..."), this);
  help->setShortcut(Qt::Key_F1);
  connect(help, SIGNAL(triggered(bool)), this, SLOT(slotHelpIntro()));

  QAction *about = new QAction(tr("&About ResistorCodes..."), this);
  connect(about, SIGNAL(triggered(bool)), this, SLOT(slotHelpAbout()));

  QAction *aboutQt = new QAction(tr("&About Qt..."), this);
  connect(aboutQt, SIGNAL(triggered(bool)), this, SLOT(slotHelpAboutQt()));

  QMenu *helpMenu = new QMenu(tr("&Help"));
  helpMenu->addAction(help);
  helpMenu->addAction(about);
  helpMenu->addSeparator();
  helpMenu->addAction(aboutQt);

  menuBar()->addMenu(fileMenu);
  menuBar()->addSeparator();
  menuBar()->addMenu(helpMenu);

	res= new QResistor();
	//--------------------resistance displayin ui ---------------------------------//
	resBox = new MyResistanceBox (this);

	connect(res, SIGNAL(valueModified(QResistor*)),resBox,SLOT(update(QResistor*)));
	//-------------------color displaying ui---------------------------------------------//
	colorBox = new MyColorBox(this);

	connect(res, SIGNAL(valueModified(QResistor*)),colorBox,SLOT(update(QResistor*)));

	//-------------------paste the configuration to clipboard--------------------------------------------//
	connect(res, SIGNAL(valueModified(QResistor*)),this,SLOT(slotConfiguration()));
	//-------------------switching buttons ui--------------------------------------//
  QPushButton *calcColor = new QPushButton(QPixmap(":/bitmaps/next.png")," To Colors", this );
    calcColor->setObjectName("calcColor");
    
	connect(calcColor, SIGNAL(clicked()),this,SLOT(setResistanceValue()));

	QPushButton *calcResistance = new QPushButton(QPixmap(":/bitmaps/previous.png")," To Resistance", this);
    calcColor->setObjectName( "calcResistance" );
    
	connect(calcResistance, SIGNAL(clicked()),this,SLOT(setColorValue()));

	QPushButton *quit = new QPushButton( "Quit", this );
    calcColor->setObjectName("quit");
    
	connect( quit, SIGNAL(clicked()), qApp, SLOT(quit()) );

  QHBoxLayout *buttonBox = new QHBoxLayout;
  buttonBox->addWidget(calcColor);
  buttonBox->addWidget(calcResistance);
  buttonBox->addWidget(quit);

	//--------------------packing all of them together---------------------------------------//

  // main box
  QWidget *main = new QWidget(this);
  setCentralWidget(main);
  QGridLayout *grid = new QGridLayout();
  main->setLayout(grid);
  grid->setSpacing (10);
  grid->setMargin (10);

  grid->addWidget( resBox, 1, 0 );
  grid->addLayout( buttonBox, 2, 0 );
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

tQucsSettings QucsSettings;

// Loads the settings file and stores the settings.
bool loadSettings()
{
    QSettings settings("qucs","qucs");
    // Qucs Resistor Tool specific settings
    settings.beginGroup("QucsResCodes");
    if(settings.contains("x"))QucsSettings.x=settings.value("x").toInt();
    if(settings.contains("y"))QucsSettings.y=settings.value("y").toInt();
    settings.endGroup();

    // Qucs general settings
    if(settings.contains("font"))QucsSettings.font.fromString(settings.value("font").toString());
    if(settings.contains("Language"))QucsSettings.Language=settings.value("Language").toString();

  return true;
}

// Saves the settings in the settings file.
bool saveApplSettings(MyWidget *w)
{
    QSettings settings ("qucs","qucs");
    settings.beginGroup("QucsResistor");
    settings.setValue("x", w->x());
    settings.setValue("y", w->y());
    settings.endGroup();
  return true;

}

int main( int argc, char **argv )
{
  QApplication a(argc, argv);

  // apply default settings
  QucsSettings.x = 100;
  QucsSettings.y = 50;
  QucsSettings.font = QFont("Helvetica", 12);

  // is application relocated?
  char * var = getenv ("QUCSDIR");
  QDir QucsDir;
  if (var != NULL) {
    QucsDir = QDir(QString(var));
    QucsSettings.LangDir =     QucsDir.canonicalPath() + "/share/qucs/lang/";
  } else {
    QString QucsApplicationPath = QCoreApplication::applicationDirPath();
#ifdef __APPLE__
    QucsDir = QDir(QucsApplicationPath.section("/bin",0,0));
#else
    QucsDir = QDir(QucsApplicationPath);
    QucsDir.cdUp();
#endif
    QucsSettings.LangDir = QucsDir.canonicalPath() + "/share/qucs/lang/";
  }

  loadSettings();

  a.setFont(QucsSettings.font);

  QTranslator tor(0);
  QString lang = QucsSettings.Language;
  if(lang.isEmpty())
    lang = QString(QLocale::system().name());
  tor.load( QString("qucs_") + lang, QucsSettings.LangDir);
  a.installTranslator(&tor);

  MyWidget *w = new MyWidget();
  w->raise();
  w->move(QucsSettings.x, QucsSettings.y); // before show()
  w->show();
  
  int result = a.exec();
  saveApplSettings(w);
  return result;
}
