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

#include <cmath>
#include <stdlib.h>
#include <string>

#include <QtGui>
#include <QtWidgets>

#include "lc_filter.h"
#include "tl_filter.h"
#include "eqn_filter.h"
#include "line_filter.h"
#include "cline_filter.h"
#include "stepz_filter.h"
#include "quarterwave_filter.h"

#include "qf_poly.h"
#include "qf_filter.h"
#include "qf_cauer.h"
#include "qucsfilter.h"
#include "helpdialog.h"
#include "material_props.h"

QucsFilter::QucsFilter()
{
  QWidget *centralWidget = new QWidget(this);  
  setCentralWidget(centralWidget);
  
  // set application icon
  setWindowIcon(QPixmap(":/bitmaps/big.qucs.xpm"));
  setWindowTitle("Qucs Filter " PACKAGE_VERSION);

  // --------  create menubar  -------------------
  QMenu *fileMenu = new QMenu(tr("&File"));

  QAction * fileQuit = new QAction(tr("E&xit"), this);
  fileQuit->setShortcut(Qt::CTRL+Qt::Key_Q);
  connect(fileQuit, SIGNAL(triggered(bool)), SLOT(slotQuit()));

  fileMenu->addAction(fileQuit);

  QMenu *helpMenu = new QMenu(tr("&Help"), this);
  QAction * helpHelp = new QAction(tr("Help..."), this);
  helpHelp->setShortcut(Qt::Key_F1);
  connect(helpHelp, SIGNAL(triggered(bool)), SLOT(slotHelpIntro()));

  QAction * helpAbout = new QAction(tr("&About QucsFilter..."), this);
  helpMenu->addAction(helpAbout);
  connect(helpAbout, SIGNAL(triggered(bool)), SLOT(slotHelpAbout()));

  QAction * helpAboutQt = new QAction(tr("About Qt..."), this);
  helpMenu->addAction(helpAboutQt);
  connect(helpAboutQt, SIGNAL(triggered(bool)), SLOT(slotHelpAboutQt()));

  helpMenu->addAction(helpHelp);
  helpMenu->addSeparator();
  helpMenu->addAction(helpAbout);
  helpMenu->addAction(helpAboutQt);

  menuBar()->addMenu(fileMenu);
  menuBar()->addSeparator();
  menuBar()->addMenu(helpMenu);

  // -------  create main windows widgets --------
  all  = new QGridLayout();
  all->setSpacing(3);

  // assign layout to central widget
  centralWidget->setLayout(all);

  // ...........................................................
  box1 = new QGroupBox(tr("Filter"), this);
  all->addWidget(box1,0,0);

  gbox1 = new QGridLayout();
  gbox1->setSpacing(3);

  box1->setLayout(gbox1);

  QLabel *Label0 = new QLabel(tr("Realization:"), this);
  gbox1->addWidget(Label0, 0,0);
  ComboRealize = new QComboBox(this);
  ComboRealize->addItem("LC ladder (pi type)");
  ComboRealize->addItem("LC ladder (tee type)");
  ComboRealize->addItem("C-coupled transmission lines");
  ComboRealize->addItem("Microstrip end-coupled");
  ComboRealize->addItem("Coupled transmission lines");
  ComboRealize->addItem("Coupled microstrip");
  ComboRealize->addItem("Stepped-impedance");
  ComboRealize->addItem("Stepped-impedance microstrip");
  ComboRealize->addItem("Quarter wave");
  ComboRealize->addItem("Quarter wave microstrip");
  ComboRealize->addItem("Equation-defined");

  // microstrip filters cannot be implemented with SPICE
  if (QucsSettings.DefaultSimulator != spicecompat::simQucsator) {
      QStandardItemModel *model =
            qobject_cast<QStandardItemModel *>(ComboRealize->model());
      Q_ASSERT(model != nullptr);
      for (int i = 2; i < 10; i++) {
          QStandardItem *itm = model->item(i);
          itm->setFlags(itm->flags() & ~Qt::ItemIsEnabled);
      }
  }

  gbox1->addWidget(ComboRealize, 0,1);
  connect(ComboRealize, SIGNAL(activated(int)), SLOT(slotRealizationChanged(int)));

  QLabel *Label1 = new QLabel(tr("Filter type:"), this);
  gbox1->addWidget(Label1, 1,0);
  ComboType = new QComboBox(this);
  ComboType->addItem("Bessel");
  ComboType->addItem("Butterworth");
  ComboType->addItem("Chebyshev");
  ComboType->addItem("Cauer");
  gbox1->addWidget(ComboType, 1,1);
  connect(ComboType, SIGNAL(activated(int)), SLOT(slotTypeChanged(int)));

  QLabel *Label2 = new QLabel(tr("Filter class:"), this);
  gbox1->addWidget(Label2, 2,0);
  ComboClass = new QComboBox(this);
  ComboClass->addItem(tr("Low pass"));
  ComboClass->addItem(tr("High pass"));
  ComboClass->addItem(tr("Band pass"));
  ComboClass->addItem(tr("Band stop"));
  gbox1->addWidget(ComboClass, 2,1);
  connect(ComboClass, SIGNAL(activated(int)), SLOT(slotClassChanged(int)));

  IntVal = new QIntValidator(1, 200, this);
  DoubleVal = new QDoubleValidator(this);

  LabelOrder = new QLabel(tr("Order:"), this);
  gbox1->addWidget(LabelOrder, 3,0);
  EditOrder = new QLineEdit("3", this);
  EditOrder->setValidator(IntVal);
  gbox1->addWidget(EditOrder, 3,1);

  LabelStart = new QLabel(tr("Corner frequency:"), this);
  gbox1->addWidget(LabelStart, 4,0);
  EditCorner = new QLineEdit("1", this);
  EditCorner->setValidator(DoubleVal);
  gbox1->addWidget(EditCorner, 4,1);
  ComboCorner = new QComboBox(this);
  ComboCorner->addItem("Hz");
  ComboCorner->addItem("kHz");
  ComboCorner->addItem("MHz");
  ComboCorner->addItem("GHz");
  ComboCorner->setCurrentIndex(3);
  gbox1->addWidget(ComboCorner, 4,2);

  LabelStop = new QLabel(tr("Stop frequency:"), this);
  gbox1->addWidget(LabelStop, 5,0);
  EditStop = new QLineEdit("2", this);
  EditStop->setValidator(DoubleVal);
  gbox1->addWidget(EditStop, 5,1);
  ComboStop = new QComboBox(this);
  ComboStop->addItem("Hz");
  ComboStop->addItem("kHz");
  ComboStop->addItem("MHz");
  ComboStop->addItem("GHz");
  ComboStop->setCurrentIndex(3);
  gbox1->addWidget(ComboStop, 5,2);

  LabelBandStop = new QLabel(tr("Stop band frequency:"), this);
  gbox1->addWidget(LabelBandStop, 6,0);
  EditBandStop = new QLineEdit("3", this);
  EditBandStop->setValidator(DoubleVal);
  gbox1->addWidget(EditBandStop, 6,1);
  ComboBandStop = new QComboBox(this);
  ComboBandStop->addItem("Hz");
  ComboBandStop->addItem("kHz");
  ComboBandStop->addItem("MHz");
  ComboBandStop->addItem("GHz");
  ComboBandStop->setCurrentIndex(3);
  gbox1->addWidget(ComboBandStop, 6,2);

  LabelRipple = new QLabel(tr("Pass band ripple:"), this);
  gbox1->addWidget(LabelRipple, 7,0);
  EditRipple = new QLineEdit("1", this);
  EditRipple->setValidator(DoubleVal);
  gbox1->addWidget(EditRipple, 7,1);
  LabelRipple_dB = new QLabel("dB", this);
  gbox1->addWidget(LabelRipple_dB, 7,2);

  LabelAtten = new QLabel(tr("Stop band attenuation:"), this);
  gbox1->addWidget(LabelAtten, 8,0);
  EditAtten = new QLineEdit("20", this);
  EditAtten->setValidator(DoubleVal);
  gbox1->addWidget(EditAtten, 8,1);
  LabelAtten_dB = new QLabel("dB", this);
  gbox1->addWidget(LabelAtten_dB, 8,2);

  LabelImpedance = new QLabel(tr("Impedance:"), this);
  gbox1->addWidget(LabelImpedance, 9,0);
  EditImpedance = new QLineEdit("50", this);
  EditImpedance->setValidator(DoubleVal);
  gbox1->addWidget(EditImpedance, 9,1);
  LabelOhm = new QLabel("Ohm", this);
  gbox1->addWidget(LabelOhm, 9,2);

  // ...........................................................
  box2 = new QGroupBox(tr("Microstrip Substrate"), this);
  box2->setEnabled(false);
  all->addWidget(box2,0,1);

  gbox2 = new QGridLayout();
  gbox2->setSpacing(3);

  box2->setLayout(gbox2);

  QLabel *Label3 = new QLabel(tr("Relative permittivity:"), this);
  gbox2->addWidget(Label3, 0,0);
  ComboEr = new QComboBox(this);
  ComboEr->setEditable(true);
  ComboEr->lineEdit()->setValidator(DoubleVal);
  connect(ComboEr, SIGNAL(activated(const QString&)), SLOT(slotTakeEr(const QString&)));
  gbox2->addWidget(ComboEr, 0,1);

  const char **p = List_er;
  while(*(++p))
    ComboEr->addItem(*p);  // put material properties into combobox
  ComboEr->lineEdit()->setText("9.8");

  QLabel *Label4 = new QLabel(tr("Substrate height:"), this);
  gbox2->addWidget(Label4, 1,0);
  EditHeight = new QLineEdit("1.0", this);
  EditHeight->setValidator(DoubleVal);
  gbox2->addWidget(EditHeight, 1,1);
  QLabel *Label5 = new QLabel("mm", this);
  gbox2->addWidget(Label5, 1,2);

  QLabel *Label6 = new QLabel(tr("metal thickness:"), this);
  gbox2->addWidget(Label6, 2,0);
  EditThickness = new QLineEdit("12.5", this);
  EditThickness->setValidator(DoubleVal);
  gbox2->addWidget(EditThickness, 2,1);
  QLabel *Label7 = new QLabel("um", this);
  gbox2->addWidget(Label7, 2,2);

  QLabel *Label8 = new QLabel(tr("minimum width:"), this);
  gbox2->addWidget(Label8, 3,0);
  EditMinWidth = new QLineEdit("0.4", this);
  EditMinWidth->setValidator(DoubleVal);
  gbox2->addWidget(EditMinWidth, 3,1);
  QLabel *Label9 = new QLabel("mm", this);
  gbox2->addWidget(Label9, 3,2);

  QLabel *Label10 = new QLabel(tr("maximum width:"), this);
  gbox2->addWidget(Label10, 4,0);
  EditMaxWidth = new QLineEdit("5.0", this);
  EditMaxWidth->setValidator(DoubleVal);
  gbox2->addWidget(EditMaxWidth, 4,1);
  QLabel *Label11 = new QLabel("mm", this);
  gbox2->addWidget(Label11, 4,2);

  QSpacerItem *mySpacer=new QSpacerItem(1,1, QSizePolicy::Minimum, QSizePolicy::Expanding);
  gbox2->addItem(mySpacer, 5, 0, 1, -1);

  // ...........................................................
  QPushButton *ButtonGo = new QPushButton(tr("Calculate and put into Clipboard"), this);
  connect(ButtonGo, SIGNAL(clicked()), SLOT(slotCalculate()));
  all->addWidget(ButtonGo, 1, 0, 1, -1);

  LabelResult = new QLabel(this);
  ResultState = 100;
  slotShowResult();
  LabelResult->setAlignment(Qt::AlignHCenter);
  all->addWidget(LabelResult, 2, 0, 1, -1);

  // -------  finally set initial state  --------
  slotTypeChanged(0);
  slotClassChanged(0);
}

QucsFilter::~QucsFilter()
{
  delete gbox1;
  delete gbox2;
  delete all;
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

  qApp->quit();
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

    tSubstrate Substrate;
    Substrate.er = ComboEr->currentText().toDouble();
    Substrate.height = EditHeight->text().toDouble() / 1e3;
    Substrate.thickness = EditThickness->text().toDouble() / 1e6;
    Substrate.tand = 0.0;
    Substrate.resistivity = 1e-10;
    Substrate.roughness = 0.0;
    Substrate.minWidth = EditMinWidth->text().toDouble() / 1e3;
    Substrate.maxWidth = EditMaxWidth->text().toDouble() / 1e3;

    switch(ComboRealize->currentIndex()) {
      case 2:  // C-coupled transmission line filter
        s = Line_Filter::createSchematic(Filter, &Substrate, false);
        return s;
      case 3:  // microstrip end-coupled filter
        s = Line_Filter::createSchematic(Filter, &Substrate, true);
        return s;
      case 4:  // coupled transmission line filter
        s = CoupledLine_Filter::createSchematic(Filter, &Substrate, false);
        return s;
      case 5:  // coupled microstrip line filter
        s = CoupledLine_Filter::createSchematic(Filter, &Substrate, true);
        return s;
      case 6:  // stepped-impedance transmission line filter
        s = StepImpedance_Filter::createSchematic(Filter, &Substrate, false);
        return s;
      case 7:  // stepped-impedance microstrip line filter
        s = StepImpedance_Filter::createSchematic(Filter, &Substrate, true);
        return s;
      case 8: // Quarter wave transmission line filter
        s = QuarterWave_Filter::createSchematic(Filter, &Substrate, false);
        return s;
      case 9: // Quarter wave microstrip line  filter
        s = QuarterWave_Filter::createSchematic(Filter, &Substrate, true);
        return s;
      case 10:  // equation defined filter
        s = Equation_Filter::createSchematic(Filter);
        return s;


    }

    if (Filter->Type != TYPE_CAUER) {
      if(ComboRealize->currentIndex() == 0)
        s = LC_Filter::createSchematic(Filter, true);
      else
        s = LC_Filter::createSchematic(Filter, false);
    }
    else  {
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

// ************************************************************
void QucsFilter::slotRealizationChanged(int index)
{
  if(index < 2)         // set to LC ladder type?
    ComboClass->setEnabled(true);
  else if(index < 6) {  // set to "transmission line" types?
    // set to bandpass fixed
    ComboClass->setCurrentIndex(CLASS_BANDPASS);
    slotClassChanged(CLASS_BANDPASS);
    ComboClass->setEnabled(false);
  }
  else if(index < 8) {  // set to "stepped impedance" types?
    // set to lowpass fixed
    ComboClass->setCurrentIndex(CLASS_LOWPASS);
    slotClassChanged(CLASS_LOWPASS);
    ComboClass->setEnabled(false);
  }
  else
    ComboClass->setEnabled(true);

  if((index == 3)||(index == 5)||(index == 7)||(index == 9))
    box2->setEnabled(true);
  else
    box2->setEnabled(false);

  if ((index == 8)||(index == 9))
  {
     ComboClass->setCurrentIndex(CLASS_BANDPASS);
     slotClassChanged(CLASS_BANDPASS);
  }
}

// ************************************************************
void QucsFilter::slotTakeEr(const QString& Text)
{
  // If the ComboBox text was a material property, the material name
  // is separated by two spaces. So, cut off the name.
  ComboEr->lineEdit()->setText(Text.section("  ", 0, 0));
}
