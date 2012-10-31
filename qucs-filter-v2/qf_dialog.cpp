/***************************************************************************
                              qf_dialog.h
                             -------------
    begin                : Fri May 1 2009
    copyright            : (C) 2009 by Stefan Jahn
    email                : stefan@lkcc.org
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

#include <q3popupmenu.h>
#include <qmenubar.h>
#include <qlayout.h>
#include <qtabwidget.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <q3buttongroup.h>
#include <qpushbutton.h>
#include <q3hbox.h>
#include <qmessagebox.h>

#include "qf_dialog.h"
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3VBoxLayout>

FilterDialog::FilterDialog (QWidget * parent) : QDialog (parent)
{
  // set application icon
  ///setIcon (QPixmap(QucsSettings.BitmapDir + "big.qucs.xpm"));
  setCaption("Qucs Filter " PACKAGE_VERSION);

  all = new Q3VBoxLayout(this);

  // --------  create menubar  -------------------
  Q3PopupMenu *fileMenu = new Q3PopupMenu();
  fileMenu->insertItem(tr("E&xit"), this, SLOT(slotQuit()), Qt::CTRL+Qt::Key_Q);

  Q3PopupMenu *helpMenu = new Q3PopupMenu();
  helpMenu->insertItem(
                tr("&About Qucs Filter..."), this, SLOT(slotHelpAbout()), 0);
  helpMenu->insertItem(tr("About Qt..."), this, SLOT(slotHelpAboutQt()), 0);

  QMenuBar *bar = new QMenuBar(this);
  bar->insertItem(tr("&File"), fileMenu);
  bar->insertSeparator ();
  bar->insertItem(tr("&Help"), helpMenu);
  all->addWidget(bar);

  // reserve space for menubar
  all->addSpacing (bar->height() + 2);

  QTabWidget *t = new QTabWidget(this);
  all->addWidget(t);

  // ...........................................................
  QWidget *Tab1 = new QWidget(t);
  Q3GridLayout *gp1 = new Q3GridLayout(Tab1,12,6,5,5);

  FilterName = new QComboBox(FALSE, Tab1);
  gp1->addWidget(FilterName,0,0);
  TformName = new QComboBox(FALSE, Tab1);
  gp1->addWidget(TformName,0,1);

  OrderBox = new QCheckBox(tr("Specify order"), Tab1);
  gp1->addWidget(OrderBox,1,0);
  Q3HBox *h1 = new Q3HBox(Tab1);
  h1->setSpacing (5);
  OrderCombo = new QComboBox(FALSE, h1);
  OrderCombo->setEnabled(TRUE);
  SubOrderCombo = new QComboBox(FALSE, h1);
  SubOrderCombo->setEnabled(FALSE);
  SubOrderCombo->insertItem( tr( "b" ) );
  SubOrderCombo->insertItem( tr( "c" ) );
  gp1->addWidget(h1,1,1);

  CutoffLabel = new QLabel(tr("Cutoff/Center"),Tab1);
  gp1->addWidget(CutoffLabel,2,0);
  EnterCutoff = new QLineEdit(Tab1);
  gp1->addWidget(EnterCutoff,2,1);
  CutoffCombo = new QComboBox(Tab1);
  CutoffCombo->insertItem( tr( "Hz" ) );
  CutoffCombo->insertItem( tr( "kHz" ) );
  CutoffCombo->insertItem( tr( "MHz" ) );
  CutoffCombo->insertItem( tr( "GHz" ) );
  gp1->addWidget(CutoffCombo,2,2);

  RippleLabel = new QLabel(tr("Ripple"),Tab1);
  gp1->addWidget(RippleLabel,3,0);
  EnterRipple = new QLineEdit(Tab1);
  gp1->addWidget(EnterRipple,3,1);
  RippleUnitLabel = new QLabel(tr("dB"),Tab1);
  gp1->addWidget(RippleUnitLabel,3,2);

  AngleLabel = new QLabel(tr("Angle"),Tab1);
  gp1->addWidget(AngleLabel,3,3);
  EnterAngle = new QLineEdit(Tab1);
  gp1->addWidget(EnterAngle,3,4);
  AngleUnitLabel = new QLabel(tr("°"),Tab1);
  gp1->addWidget(AngleUnitLabel,3,5);

  ZinLabel = new QLabel(tr("Zin"),Tab1);
  gp1->addWidget(ZinLabel,7,0);
  EnterZin = new QLineEdit(Tab1);
  gp1->addWidget(EnterZin,7,1);
  OhmLabel = new QLabel(tr("Ohm"),Tab1);
  gp1->addWidget(OhmLabel,7,2);

  ZoutLabel = new QLabel(tr("Zout"),Tab1);
  ZoutLabel->setEnabled(false);
  gp1->addWidget(ZoutLabel,7,3);
  EnterZout = new QLineEdit(Tab1);
  gp1->addWidget(EnterZout,7,4);
  OhmLabel_2 = new QLabel(tr("Ohm"),Tab1);
  gp1->addWidget(OhmLabel_2,7,5);

  StopbandLabel = new QLabel(tr("Stopband corner"),Tab1);
  gp1->addWidget(StopbandLabel,5,0);
  EnterStopband = new QLineEdit(Tab1);
  gp1->addWidget(EnterStopband,5,1);
  StopbandCombo = new QComboBox(FALSE, Tab1);
  StopbandCombo->insertItem( tr( "Hz" ) );
  StopbandCombo->insertItem( tr( "kHz" ) );
  StopbandCombo->insertItem( tr( "MHz" ) );
  StopbandCombo->insertItem( tr( "GHz" ) );
  gp1->addWidget(StopbandCombo,5,2);

  BandwidthLabel = new QLabel(tr("Bandwidth"),Tab1);
  BandwidthLabel->setEnabled(FALSE);
  gp1->addWidget(BandwidthLabel,4,0);
  EnterBandwidth = new QLineEdit(Tab1);
  gp1->addWidget(EnterBandwidth,4,1);
  BandwidthCombo = new QComboBox(FALSE, Tab1);
  BandwidthCombo->setEnabled(FALSE);
  BandwidthCombo->insertItem( tr( "Hz" ) );
  BandwidthCombo->insertItem( tr( "kHz" ) );
  BandwidthCombo->insertItem( tr( "MHz" ) );
  BandwidthCombo->insertItem( tr( "GHz" ) );
  gp1->addWidget(BandwidthCombo,4,2);

  AttenuationLabel = new QLabel(tr("Attenuation"),Tab1);
  gp1->addWidget(AttenuationLabel,6,0);
  EnterAttenuation = new QLineEdit(Tab1);
  gp1->addWidget(EnterAttenuation,6,1);
  dBLabel = new QLabel(tr("dB"),Tab1);
  gp1->addWidget(dBLabel,6,2);

  DualBox = new QCheckBox(tr("dual"),Tab1);
  gp1->addMultiCellWidget(DualBox,8,8,0,2);
  CauerPoleBox = new QCheckBox(tr("Stopband is first pole"),Tab1);
  CauerPoleBox->setEnabled(FALSE);
  gp1->addMultiCellWidget(CauerPoleBox,9,9,0,2);
  OptimizeCauerBox = new QCheckBox(tr("Optimize cauer"),Tab1);
  OptimizeCauerBox->setEnabled(FALSE);
  gp1->addMultiCellWidget(OptimizeCauerBox,10,10,0,2);
  EqualInductorBox = new QCheckBox(tr("Equal inductors"),Tab1);
  EqualInductorBox->setEnabled(FALSE);
  gp1->addMultiCellWidget(EqualInductorBox,8,8,3,5);
  UseCrossBox = new QCheckBox(tr("+ rather than T"),Tab1);
  UseCrossBox->setEnabled(FALSE);
  gp1->addMultiCellWidget(UseCrossBox,9,9,3,5);

  Cboxes = new Q3VButtonGroup(tr("Optimize C"),Tab1);
  Cmin = new QRadioButton(tr("Cmin"),Cboxes);
  Cmax = new QRadioButton(tr("Cmax"),Cboxes);
  NoC = new QRadioButton(tr("noC"),Cboxes);
  gp1->addMultiCellWidget(Cboxes,11,11,0,2);

  Lboxes = new Q3VButtonGroup(tr("Optimize L"),Tab1);
  Lmin = new QRadioButton(tr("Lmin"),Lboxes);
  Lmax = new QRadioButton(tr("Lmax"),Lboxes);
  NoL = new QRadioButton(tr("noL"),Lboxes);
  gp1->addMultiCellWidget(Lboxes,11,11,3,5);

  t->addTab(Tab1, tr("LC Filters"));

  // ...........................................................
  QWidget *Tab2 = new QWidget(t);
  t->addTab(Tab2, tr("Microstrip Filters"));

  // ...........................................................
  QWidget *Tab3 = new QWidget(t);
  t->addTab(Tab3, tr("Active Filters"));

  // reserve space for vertical resizing
  all->addStretch();

  // ...........................................................
  // buttons on the bottom of the dialog (independent of the TabWidget)
  Q3HBox *Butts = new Q3HBox(this);
  Butts->setSpacing(3);
  Butts->setMargin(3);
  all->addWidget(Butts);

  cancelButton = new QPushButton(tr("Exit"),Butts);
  okButton = new QPushButton(tr("Calculate"),Butts);
  okButton->setEnabled(FALSE);

  // signals and slots connections
  connect( cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( okButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
}

FilterDialog::~FilterDialog ()
{
  delete all;
}

// ************************************************************
void FilterDialog::slotHelpAbout()
{
  QMessageBox::about(this, tr("About..."),
    "Qucs Filter Version " PACKAGE_VERSION+
    tr("\nFilter synthesis program\n")+
    tr("Copyright (C) 2009 by")+
    "\nVincent Habchi, Stefan Jahn\n"
    "\nThis is free software; see the source for copying conditions."
    "\nThere is NO warranty; not even for MERCHANTABILITY or "
    "\nFITNESS FOR A PARTICULAR PURPOSE.\n\n");
}

// ************************************************************
void FilterDialog::slotHelpAboutQt()
{
  QMessageBox::aboutQt(this, tr("About Qt"));
}

// ************************************************************
void FilterDialog::slotQuit()
{
  int tmp;
  tmp = x();            // call size and position function in order to ...
  tmp = y();            // ... set them correctly before closing the ...
  tmp = width();        // dialog !!!  Otherwise the frame of the window ...
  tmp = height();       // ... will not be recognized (a X11 problem).

  reject();
}
