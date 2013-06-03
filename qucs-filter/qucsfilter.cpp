/***************************************************************************
                               qucsfilter.cpp
                              ----------------
    begin                : Wed Mar 02 2005
    copyright            : (C) 2005 by Michael Margraf
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

#include <math.h>
#include <stdlib.h>
#include <string>

#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QValidator>
#include <QTimer>
#include <QClipboard>
#include <QApplication>
#include <QGridLayout>
#include <QPixmap>

#include "lc_filter.h"
#include "qf_poly.h"
#include "qf_filter.h"
#include "qf_cauer.h"
#include "qucsfilter.h"
#include "helpdialog.h"

QucsFilter::QucsFilter()
{
  // set application icon
  setWindowIcon(QPixmap(QucsSettings.BitmapDir + "big.qucs.xpm"));
  setWindowTitle("Qucs Filter " PACKAGE_VERSION);

  // --------  create menubar  -------------------
  QMenu *fileMenu = new QMenu(tr("&File"));

  QAction * fileQuit = new QAction(tr("E&xit"), this);
  fileQuit->setShortcut(Qt::CTRL+Qt::Key_Q);
  connect(fileQuit, SIGNAL(activated()), SLOT(slotQuit()));

  fileMenu->addAction(fileQuit);

  QMenu *helpMenu = new QMenu(tr("&Help"), this);
  QAction * helpHelp = new QAction(tr("Help..."), this);
  helpHelp->setShortcut(Qt::Key_F1);
  connect(helpHelp, SIGNAL(activated()), SLOT(slotHelpIntro()));

  QAction * helpAbout = new QAction(tr("&About QucsFilter..."), this);
  helpMenu->addAction(helpAbout);
  connect(helpAbout, SIGNAL(activated()), SLOT(slotHelpAbout()));

  QAction * helpAboutQt = new QAction(tr("About Qt..."), this);
  helpMenu->addAction(helpAboutQt);
  connect(helpAboutQt, SIGNAL(activated()), SLOT(slotHelpAboutQt()));

  helpMenu->addAction(helpHelp);
  helpMenu->addSeparator();
  helpMenu->addAction(helpAbout);
  helpMenu->addAction(helpAboutQt);

  QMenuBar *bar = new QMenuBar(this);
  bar->addMenu(fileMenu);
  bar->addSeparator();
  bar->addMenu(helpMenu);


  // -------  create main windows widgets --------
  gbox = new QGridLayout(this);
  gbox->setSpacing(3);

  QWidget *Space = new QWidget(this);   // reserve space for menubar
  Space->setFixedSize(5, bar->height());
  gbox->addWidget(Space, 0,0);

  QLabel *Label1 = new QLabel(tr("Filter type:"), this);
  gbox->addWidget(Label1, 1,0);
  ComboType = new QComboBox(this);
  ComboType->addItem("Bessel");
  ComboType->addItem("Butterworth");
  ComboType->addItem("Chebyshev");
  ComboType->addItem("Cauer");
  gbox->addWidget(ComboType, 1,1);
  connect(ComboType, SIGNAL(activated(int)), SLOT(slotTypeChanged(int)));

  QLabel *Label2 = new QLabel(tr("Filter class:"), this);
  gbox->addWidget(Label2, 2,0);
  ComboClass = new QComboBox(this);
  ComboClass->addItem(tr("Low pass"));
  ComboClass->addItem(tr("High pass"));
  ComboClass->addItem(tr("Band pass"));
  ComboClass->addItem(tr("Band stop"));
  gbox->addWidget(ComboClass, 2,1);
  connect(ComboClass, SIGNAL(activated(int)), SLOT(slotClassChanged(int)));

  IntVal = new QIntValidator(1, 200, this);
  DoubleVal = new QDoubleValidator(this);

  LabelOrder = new QLabel(tr("Order:"), this);
  gbox->addWidget(LabelOrder, 3,0);
  EditOrder = new QLineEdit("3", this);
  EditOrder->setValidator(IntVal);
  gbox->addWidget(EditOrder, 3,1);

  LabelStart = new QLabel(tr("Corner frequency:"), this);
  gbox->addWidget(LabelStart, 4,0);
  EditCorner = new QLineEdit("1", this);
  EditCorner->setValidator(DoubleVal);
  gbox->addWidget(EditCorner, 4,1);
  ComboCorner = new QComboBox(this);
  ComboCorner->addItem("Hz");
  ComboCorner->addItem("kHz");
  ComboCorner->addItem("MHz");
  ComboCorner->addItem("GHz");
  ComboCorner->setCurrentIndex(3);
  gbox->addWidget(ComboCorner, 4,2);

  LabelStop = new QLabel(tr("Stop frequency:"), this);
  gbox->addWidget(LabelStop, 5,0);
  EditStop = new QLineEdit("2", this);
  EditStop->setValidator(DoubleVal);
  gbox->addWidget(EditStop, 5,1);
  ComboStop = new QComboBox(this);
  ComboStop->addItem("Hz");
  ComboStop->addItem("kHz");
  ComboStop->addItem("MHz");
  ComboStop->addItem("GHz");
  ComboStop->setCurrentIndex(3);
  gbox->addWidget(ComboStop, 5,2);

  LabelBandStop = new QLabel(tr("Stop band frequency:"), this);
  gbox->addWidget(LabelBandStop, 6,0);
  EditBandStop = new QLineEdit("3", this);
  EditBandStop->setValidator(DoubleVal);
  gbox->addWidget(EditBandStop, 6,1);
  ComboBandStop = new QComboBox(this);
  ComboBandStop->addItem("Hz");
  ComboBandStop->addItem("kHz");
  ComboBandStop->addItem("MHz");
  ComboBandStop->addItem("GHz");
  ComboBandStop->setCurrentIndex(3);
  gbox->addWidget(ComboBandStop, 6,2);

  LabelRipple = new QLabel(tr("Pass band ripple:"), this);
  gbox->addWidget(LabelRipple, 7,0);
  EditRipple = new QLineEdit("1", this);
  EditRipple->setValidator(DoubleVal);
  gbox->addWidget(EditRipple, 7,1);
  LabelRipple_dB = new QLabel("dB", this);
  gbox->addWidget(LabelRipple_dB, 7,2);

  LabelAtten = new QLabel(tr("Stop band attenuation:"), this);
  gbox->addWidget(LabelAtten, 8,0);
  EditAtten = new QLineEdit("20", this);
  EditAtten->setValidator(DoubleVal);
  gbox->addWidget(EditAtten, 8,1);
  LabelAtten_dB = new QLabel("dB", this);
  gbox->addWidget(LabelAtten_dB, 8,2);

  QLabel *Label9 = new QLabel(tr("Impedance:"), this);
  gbox->addWidget(Label9, 9,0);
  EditImpedance = new QLineEdit("50", this);
  EditImpedance->setValidator(DoubleVal);
  gbox->addWidget(EditImpedance, 9,1);
  QLabel *Label10 = new QLabel("Ohm", this);
  gbox->addWidget(Label10, 9,2);


  QPushButton *ButtonGo = new QPushButton(tr("Calculate and put into Clipboard"), this);
  connect(ButtonGo, SIGNAL(clicked()), SLOT(slotCalculate()));
  gbox->addWidget(ButtonGo, 10,0,1,3);

  LabelResult = new QLabel(this);
  ResultState = 100;
  slotShowResult();
  LabelResult->setAlignment(Qt::AlignHCenter);
  gbox->addWidget(LabelResult, 11,0,1,3);

  // -------  finally set initial state  --------
  slotTypeChanged(0);
  slotClassChanged(0);
}

QucsFilter::~QucsFilter()
{
  delete gbox;
  delete IntVal;
  delete DoubleVal;
}

// ************************************************************
void QucsFilter::slotQuit()
{
  int tmp;
  tmp = x();		// call size and position function in order to ...
  tmp = y();		// ... set them correctly before closing the ...
  tmp = width();	// dialog !!!  Otherwise the frame of the window ...
  tmp = height();	// ... will not be recognized (a X11 problem).

  accept();
}

// ************************************************************
void QucsFilter::slotHelpAbout()
{
  QMessageBox::about(this, tr("About..."),
    "QucsFilter Version " PACKAGE_VERSION+
    tr("\nFilter synthesis program\n")+
    tr("Copyright (C) 2005, 2006 by")+
    "\nVincent Habchi, Toyoyuki Ishikawa,\n"
    "Michael Margraf, Stefan Jahn\n"
    "\nThis is free software; see the source for copying conditions."
    "\nThere is NO warranty; not even for MERCHANTABILITY or "
    "\nFITNESS FOR A PARTICULAR PURPOSE.\n\n");
}

// ************************************************************
void QucsFilter::slotHelpAboutQt()
{
  QMessageBox::aboutQt(this, tr("About Qt"));
}

// ************************************************************
void QucsFilter::slotHelpIntro()
{
  HelpDialog *d = new HelpDialog(this);
  d->show();
}

// ************************************************************
void QucsFilter::setError(const QString& Message)
{
  LabelResult->setText(tr("Result:") + "<font color=\"#FF0000\"><b>  " +
                       tr("Error") + "</b></font>");
  QMessageBox::critical(this, tr("Error"), Message);
}

// ************************************************************
QString * QucsFilter::calculateFilter(struct tFilter * Filter)
{
  QString * s = NULL;

  if (Filter->Type == TYPE_CAUER) {
    qf_cauer * F = NULL;
    double amin, amax, fc, fs, bw, r;
    fc = Filter->Frequency;
    amin = Filter->Ripple;
    fs = Filter->Frequency3;
    r = Filter->Impedance;
    amax = Filter->Attenuation;
    bw = Filter->Frequency2 - fc;

    switch (Filter->Class) {
    case CLASS_LOWPASS:
      F = new qf_cauer (amin, amax, fc, fs, r, 0, LOWPASS);
      break;
    case CLASS_HIGHPASS:
      F = new qf_cauer (amin, amax, fc, fs, r, 0, HIGHPASS);
      break;
    case CLASS_BANDPASS:
      F = new qf_cauer (amin, amax, fc + bw / 2, fs, r, bw, BANDPASS);
      break;
    case CLASS_BANDSTOP:
      F = new qf_cauer (amin, amax, fc + bw / 2, fs, r, bw, BANDSTOP);
      break;
    }
    if (F) {
      //F->dump();
      EditOrder->setText(QString::number(F->order()));
      s = new QString(F->to_qucs().c_str());
      delete F;
    }
    else {
      s = NULL;
    }
  }
  else {
    s = LC_Filter::createSchematic(Filter);
  }
  return s;
}

// ************************************************************
void QucsFilter::slotCalculate()
{
  // get numerical values from input widgets
  double CornerFreq   = EditCorner->text().toDouble();
  double StopFreq     = EditStop->text().toDouble();
  double BandStopFreq = EditBandStop->text().toDouble();

  // add exponent
  CornerFreq   *= pow(10, double(3*ComboCorner->currentIndex()));
  StopFreq     *= pow(10, double(3*ComboStop->currentIndex()));
  BandStopFreq *= pow(10, double(3*ComboBandStop->currentIndex()));

  tFilter Filter;
  Filter.Type = ComboType->currentIndex();
  Filter.Class = ComboClass->currentIndex();
  Filter.Order = EditOrder->text().toInt();
  Filter.Ripple = EditRipple->text().toDouble();
  Filter.Attenuation = EditAtten->text().toDouble();
  Filter.Impedance = EditImpedance->text().toDouble();
  Filter.Frequency = CornerFreq;
  Filter.Frequency2 = StopFreq;
  Filter.Frequency3 = BandStopFreq;

  if(EditStop->isEnabled())
    if(Filter.Frequency >= Filter.Frequency2) {
      setError(tr("Stop frequency must be greater than start frequency."));
      return;
    }

  if(EditOrder->isEnabled()) {
    if (Filter.Order < 2) {
      setError(tr("Filter order must not be less than two."));
      return;
    }
    if(Filter.Order > 19) if(Filter.Type == TYPE_BESSEL) {
      setError(tr("Bessel filter order must not be greater than 19."));
      return;
    }
  }

  QString * s = calculateFilter(&Filter);
  if(!s) return;

  // put resulting filter schematic into clipboard
  QClipboard *cb = QApplication::clipboard();
  cb->setText(*s);
  delete s;

  // show result for some time
  ResultState = 0;
  LabelResult->setText(tr("Result:") + "<font color=\"#008000\"><b>  " +
                       tr("Successful") + "</b></font>");
  QTimer::singleShot(500, this, SLOT(slotShowResult()));
}

// ************************************************************
void QucsFilter::slotShowResult()
{
  if(ResultState > 5) {
    LabelResult->setText(tr("Result: --"));
    return;
  }

  int c;
  ResultState++;
  if(ResultState & 1)  c = 0xFF;
  else c = 0x80;
  QString s = QString("<font color=\"#00%1000\"><b>  ").arg(c, 2, 16);
  LabelResult->setText(tr("Result:") + s + tr("Successful") + "</b></font>");

  c = 500;
  if(ResultState > 5)  c = 3000;
  QTimer::singleShot(c, this, SLOT(slotShowResult()));
}

// ************************************************************
void QucsFilter::slotTypeChanged(int index)
{
  switch(index) {
    case TYPE_BESSEL:
    case TYPE_BUTTERWORTH:
	LabelRipple->setEnabled(false);
	EditRipple->setEnabled(false);
	LabelRipple_dB->setEnabled(false);
	break;
    case TYPE_CHEBYSHEV:
    case TYPE_CAUER:
	LabelRipple->setEnabled(true);
	EditRipple->setEnabled(true);
	LabelRipple_dB->setEnabled(true);
	break;
  }
  if (index == TYPE_CAUER) {
    LabelOrder->setEnabled(false);
    EditOrder->setEnabled(false);
    LabelAtten->setEnabled(true);
    EditAtten->setEnabled(true);
    LabelAtten_dB->setEnabled(true);
    LabelBandStop->setEnabled(true);
    EditBandStop->setEnabled(true);
    ComboBandStop->setEnabled(true);
  }
  else {
    LabelOrder->setEnabled(true);
    EditOrder->setEnabled(true);
    LabelAtten->setEnabled(false);
    EditAtten->setEnabled(false);
    LabelAtten_dB->setEnabled(false);
    LabelBandStop->setEnabled(false);
    EditBandStop->setEnabled(false);
    ComboBandStop->setEnabled(false);
  }
}

// ************************************************************
void QucsFilter::slotClassChanged(int index)
{
  switch(index) {
    case CLASS_LOWPASS:
    case CLASS_HIGHPASS:
      LabelStop->setEnabled(false);
      EditStop->setEnabled(false);
      ComboStop->setEnabled(false);
      LabelStart->setText(tr("Corner frequency:"));
      break;
    case CLASS_BANDPASS:
    case CLASS_BANDSTOP:
      LabelStop->setEnabled(true);
      EditStop->setEnabled(true);
      ComboStop->setEnabled(true);
      LabelStart->setText(tr("Start frequency:"));
      break;
  }
  if (index == CLASS_BANDPASS) {
    LabelBandStop->setText(tr("Stop band frequency:"));
    LabelRipple->setText(tr("Pass band ripple:"));
  }
  else if (index == CLASS_BANDSTOP) {
    LabelBandStop->setText(tr("Pass band frequency:"));
    LabelRipple->setText(tr("Pass band attenuation:"));
  }
}
