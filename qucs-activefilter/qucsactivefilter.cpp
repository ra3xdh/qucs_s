/***************************************************************************
                             qucsactivefilter.cpp
                              ----------------
    begin                : Wed Apr 10 2014
    copyright            : (C) 2014 by Vadim Kuznetsov
    email                : ra3xdh@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "qucsactivefilter.h"
#include "sallenkey.h"
#include "mfbfilter.h"
#include "schcauer.h"
#include "transferfuncdialog.h"
#include "helpdialog.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <QTextCodec>


QucsActiveFilter::QucsActiveFilter(QWidget *parent)
    : QMainWindow(parent)
{
    Nfil = 4;
    Fc = 1000;
    ftyp = Filter::NoFilter;

    // preset size to make room for the TextEdit output
    resize(900, 800); // need quite some space to fill everything in
    // set application icon
    setWindowIcon(QPixmap(":/images/bitmaps/big.qucs.xpm"));
    setWindowTitle("Qucs Active Filter " PACKAGE_VERSION);

    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    // --------  create menubar  -------------------
    QMenu *fileMenu = new QMenu(tr("&File"));

    QAction * fileQuit = new QAction(tr("E&xit"), this);
    fileQuit->setShortcut(Qt::CTRL+Qt::Key_Q);
    connect(fileQuit, SIGNAL(activated()), SLOT(close()));

    fileMenu->addAction(fileQuit);

    // View menu
    QMenu *viewMenu = new QMenu(tr("&View"));
    viewConsole = new QAction(tr("&Console"), this);
    viewConsole->setCheckable(true);
    viewConsole->setChecked(QucsSettings.showConsole);
    viewConsole->setStatusTip(tr("Enables/disables the filter calculation console"));
    viewConsole->setWhatsThis(tr("Console\n\nEnables/disables the filter calculation console"));
    connect(viewConsole, SIGNAL(toggled(bool)), SLOT(slotViewConsole(bool)));
    viewMenu->addAction(viewConsole);

    // Help menu
    QMenu *helpMenu = new QMenu(tr("&Help"), this);
    QAction * helpHelp = new QAction(tr("Help..."), this);
    helpHelp->setShortcut(Qt::Key_F1);
    connect(helpHelp, SIGNAL(activated()), SLOT(slotHelpIntro()));

    QAction * helpAbout = new QAction(tr("&About QucsActiveFilter..."), this);
    helpMenu->addAction(helpAbout);
    connect(helpAbout, SIGNAL(activated()), SLOT(slotHelpAbout()));

    QAction * helpAboutQt = new QAction(tr("About Qt..."), this);
    helpMenu->addAction(helpAboutQt);
    connect(helpAboutQt, SIGNAL(activated()), SLOT(slotHelpAboutQt()));

    helpMenu->addAction(helpHelp);
    helpMenu->addSeparator();
    helpMenu->addAction(helpAbout);
    helpMenu->addAction(helpAboutQt);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(viewMenu);
    menuBar()->addSeparator();
    menuBar()->addMenu(helpMenu);

    //lblInputData = new QLabel(tr("Входные данные"));
    lblA1 = new QLabel(tr("Passband attenuation, Ap (dB)"));
    lblA2 = new QLabel(tr("Stopband attenuation, As (dB)"));
    lblF1 = new QLabel(tr("Cutoff frequency, Fc (Hz)"));
    lblF2 = new QLabel(tr("Stopband frequency, Fs (Hz)"));
    lblRpl1 = new QLabel(tr("Passband ripple Rp(dB)"));
    //lblRpl2 = new QLabel(tr("Stopband ripple (dB)"));
    lblKv = new QLabel(tr("Passband gain, Kv (dB)"));

    edtA1 = new QLineEdit("3");
    QDoubleValidator *val1 = new QDoubleValidator(0,100000,3);
    edtA1->setValidator(val1);
    edtA2 = new QLineEdit("20");
    edtA2->setValidator(val1);
    edtF1 = new QLineEdit("1000");
    edtF1->setValidator(val1);
    edtF2 = new QLineEdit("1200");
    edtF2->setValidator(val1);
    edtPassbRpl = new QLineEdit("3");
    edtPassbRpl->setValidator(val1);
    //edtStopbRpl = new QLineEdit("3");
    //edtStopbRpl->setValidator(val1);
    edtKv = new QLineEdit("0");
    edtKv->setValidator(val1);
    QIntValidator *val2 = new QIntValidator(2,20);
    lblOrder = new QLabel(tr("Filter order"));
    edtOrder = new QLineEdit("5");
    edtOrder->setValidator(val2);

    lblTyp = new QLabel(tr("Approximation type:"));
    cbxFilterFunc = new QComboBox;
    QStringList lst2;
    lst2<<tr("Butterworth")
        <<tr("Chebyshev")
        <<tr("Inverse Chebyshev")
        <<tr("Cauer (Elliptic)")
        <<tr("Bessel")
        <<tr("User defined");
    cbxFilterFunc->addItems(lst2);
    connect(cbxFilterFunc,SIGNAL(currentIndexChanged(int)),this,SLOT(slotSwitchParameters()));

    btnDefineTransferFunc = new QPushButton(tr("Manually define transfer function"));
    btnDefineTransferFunc->setEnabled(false);
    connect(btnDefineTransferFunc,SIGNAL(clicked()),this,SLOT(slotDefineTransferFunc()));

    btnCalcSchematic = new QPushButton(tr("Calculate and copy to clipboard"));
    connect(btnCalcSchematic,SIGNAL(clicked()),SLOT(slotCalcSchematic()));

    txtResult = new QPlainTextEdit;
    txtResult->setReadOnly(true);
    txtResult->setWordWrapMode(QTextOption::NoWrap);

    lblSch = new QLabel(tr("Filter topology"));
    lblResp = new QLabel(tr("Filter type:"));
    cbxResponse = new QComboBox;
    QStringList lst3;
    lst3<<tr("Low Pass")
        <<tr("High Pass")
        <<tr("Band Pass")
        <<tr("Band Stop");
    cbxResponse->addItems(lst3);
    connect(cbxResponse,SIGNAL(currentIndexChanged(int)),this,SLOT(slotUpdateResponse()));
    connect(cbxResponse,SIGNAL(currentIndexChanged(int)),this,SLOT(slotUpdateSchematic()));
    connect(cbxResponse,SIGNAL(currentIndexChanged(int)),this,SLOT(slotSetLabels()));
    connect(cbxResponse,SIGNAL(currentIndexChanged(int)),this,SLOT(slotSwitchParameters()));

    cbxFilterType = new QComboBox;
    QStringList lst;
    lst<<tr("Multifeedback (MFB)")
       <<tr("Sallen-Key (S-K)")
       <<tr("Cauer section");
     //<<tr("Пассивный");
    cbxFilterType->addItems(lst);
    connect(cbxFilterType,SIGNAL(currentIndexChanged(int)),this,SLOT(slotUpdateSchematic()));
    this->slotSwitchParameters();
    cbxFilterType->setMaxCount(3);

    // first parameters group, will go top-left
    QGroupBox *gpbPar = new QGroupBox(tr("Filter parameters"));
    QGridLayout *vl3 = new QGridLayout;
    vl3->setSpacing(3);
    vl3->addWidget(lblA1,0,0);
    vl3->addWidget(edtA1,0,1);
    vl3->addWidget(lblA2,1,0);
    vl3->addWidget(edtA2,1,1);
    vl3->addWidget(lblF1,2,0);
    vl3->addWidget(edtF1,2,1);
    vl3->addWidget(lblF2,3,0);
    vl3->addWidget(edtF2,3,1);
    vl3->addWidget(lblRpl1,4,0);
    vl3->addWidget(edtPassbRpl,4,1);
    vl3->addWidget(lblKv,5,0);
    vl3->addWidget(edtKv,5,1);
    vl3->addWidget(lblOrder,6,0);
    vl3->addWidget(edtOrder,6,1);
    gpbPar->setLayout(vl3);
    // do not actually show on screen (yet)
    gpbPar->setAttribute(Qt::WA_DontShowOnScreen);
    // call show() now since we need its actual size later
    gpbPar->show(); 

    // second parameters group, below the previous one
    QGroupBox *gpbFunc = new QGroupBox(tr("Transfer function and Topology"));
    QVBoxLayout *vl4 = new QVBoxLayout;

    QHBoxLayout *l3 = new QHBoxLayout;
    l3->addWidget(lblTyp);
    l3->addWidget(cbxFilterFunc);
    vl4->addLayout(l3);

    vl4->addWidget(btnDefineTransferFunc);

    QHBoxLayout *l1 = new QHBoxLayout;
    l1->addWidget(lblResp);
    l1->addWidget(cbxResponse);
    vl4->addLayout(l1);
    QHBoxLayout *l2 = new QHBoxLayout;
    l2->addWidget(lblSch);
    l2->addWidget(cbxFilterType);
    vl4->addLayout(l2);
    vl4->addWidget(btnCalcSchematic);

    gpbFunc->setLayout(vl4);
    // do not actually show on screen (yet)
    gpbFunc->setAttribute(Qt::WA_DontShowOnScreen); 
    // call show() now since we need its actual size later
    gpbFunc->show(); 

    // filter response box, top-right
    QGroupBox *gpbAFR = new QGroupBox(tr("General filter amplitude-frequency response"));
    QVBoxLayout *vl1 = new QVBoxLayout;
    gpbAFR->setLayout(vl1);

    QSize sz;
    QString s1 = ":/images/bitmaps/AFR.svg";
    imgAFR = new QSvgWidget(s1);
    sz = gpbPar->size(); // take left box size as reference size
    sz *= 0.6;
    imgAFR->setFixedSize(sz);
    vl1->addWidget(imgAFR);
    vl1->setAlignment(imgAFR, Qt::AlignHCenter);

    // filter section schematic box, below the previous one
    QGroupBox *gpbSCH = new QGroupBox(tr("Filter topology preview"));
    QVBoxLayout *vl2 = new QVBoxLayout;
    gpbSCH->setLayout(vl2);
    s1 = ":/images/bitmaps/cauer.svg";
    sch_pic = new QSvgWidget(s1);
    sz = gpbFunc->size(); // take lefbox size as reference size
    sz *= 0.95;
    sch_pic->setFixedSize(sz);
    vl2->addWidget(sch_pic);
    vl2->setAlignment(sch_pic, Qt::AlignHCenter);

    this->slotUpdateSchematic();

    // place the boxes in a grid, so they will align nicely
    QGridLayout *layout = new QGridLayout();
    layout->setColumnStretch(1, 5); // stretch only the right part
    layout->setColumnStretch(3, 5); // stretch only the right part
    layout->addWidget(gpbPar, 0, 0);
    layout->addWidget(gpbAFR, 0, 2);
    layout->addWidget(gpbFunc, 1, 0);
    layout->addWidget(gpbSCH, 1, 2);

    top1 = new QVBoxLayout;
    top1->addLayout(layout);

    gpbCons = new QGroupBox(tr("Filter calculation console"));
    QVBoxLayout *vl5 = new QVBoxLayout;
    vl5->addWidget(txtResult);
    gpbCons->setLayout(vl5);
    slotViewConsole(QucsSettings.showConsole);

    top1->addWidget(gpbCons);
    txtResult->setMinimumHeight(180);

    // init status bar
    statusBar()->showMessage(tr("Ready."));

    zenter = new QWidget;
    this->setCentralWidget(zenter);
    zenter->setLayout(top1);
    this->resize(this->width(),1);
}

QucsActiveFilter::~QucsActiveFilter()
{
    
}

void QucsActiveFilter::slotCalcSchematic()
{
    txtResult->clear();

    FilterParam par;
    if ((cbxResponse->currentIndex()==tLowPass)||
        (cbxResponse->currentIndex()==tHiPass)) {
       par.Ap = edtA1->text().toFloat();       
       par.Fc = edtF1->text().toFloat();
       par.Fs = edtF2->text().toFloat();
    } else {
       par.Fu = edtF1->text().toFloat();
       par.Fl = edtF2->text().toFloat();
       par.TW = edtA1->text().toFloat();

       if (par.Fl>par.Fu) {
           errorMessage(tr("Upper cutoff frequency of band-pass/band-stop filter is\n"
                           "less than lower. Unable to implement such filter.\n"
                           "Change parameters and try again."));
           return;
       }
    }
    par.As = edtA2->text().toFloat();
    par.Rp = edtPassbRpl->text().toFloat();
    double  G = edtKv->text().toFloat();
    par.Kv = pow(10,G/20.0);

    QStringList lst;
    Filter::FilterFunc ffunc;

    switch (cbxFilterFunc->currentIndex()) {
            case funcButterworth : ffunc = Filter::Butterworth;
                     break;
            case funcChebyshev : ffunc = Filter::Chebyshev;
                     break;
            case funcInvChebyshev : ffunc = Filter::InvChebyshev;
                     break;
            case funcCauer : ffunc = Filter::Cauer;
                     break;
            case funcBessel : ffunc = Filter::Bessel;
                     par.order = edtOrder->text().toInt();
                     break;
            case funcUser : ffunc = Filter::User;
                     break;
            default: ffunc = Filter::NoFunc;
                     break;
        }



    switch (cbxResponse->currentIndex()) {
    case tLowPass : ftyp = Filter::LowPass;
        break;
    case tHiPass : ftyp = Filter::HighPass;
        break;
    case tBandPass : ftyp = Filter::BandPass;
        break;
    case tBandStop : ftyp = Filter::BandStop;
        break;
    default: ftyp = Filter::NoFilter;
        break;
    }

    QString s;
    bool ok = false;

    switch (cbxFilterType->currentIndex()) {
    case topoCauer : {
            if (((ffunc==Filter::InvChebyshev)||
                 (ffunc==Filter::Cauer)||
                 (ftyp==Filter::BandStop))) {
                   SchCauer cauer(ffunc,ftyp,par);
                   ok = cauer.calcFilter();
                   cauer.createPolesZerosList(lst);
                   cauer.createPartList(lst);
		   txtResult->appendHtml("<pre>" + lst.join("\n") + "</pre>");
                   if (ok) {
                       cauer.createSchematic(s);
                   } else {
                       errorMessage(tr("Unable to implement filter with such parameters and topology \n"
                                       "Change parameters and/or topology and try again!"));
                   }
                } else {
                    errorMessage(tr("Unable to use Cauer section for Chebyshev or Butterworth \n"
                                 "frequency response. Try to use another topology."));
                }
             }

             break;
    case topoMFB : {
                if (!((ffunc==Filter::InvChebyshev)||(ffunc==Filter::Cauer))) {
                    MFBfilter mfb(ffunc,ftyp,par);
                    if (ffunc==Filter::User) {
                        mfb.set_TrFunc(coeffA,coeffB);
                    }
                    ok = mfb.calcFilter();
                    mfb.createPolesZerosList(lst);
                    mfb.createPartList(lst);
                    txtResult->appendHtml("<pre>" + lst.join("\n") + "</pre>");
                    if (ok) {
                        mfb.createSchematic(s);
                    } else {
                        errorMessage(tr("Unable to implement filter with such parameters and topology \n"
                                        "Change parameters and/or topology and try again!"));
                    }
                } else {
                    errorMessage(tr("Unable to use MFB filter for Cauer or Inverse Chebyshev \n"
                                 "frequency response. Try to use another topology."));
                }
             }
             break;
    case topoSallenKey : {
               SallenKey sk(ffunc,ftyp,par);
               if (ffunc==Filter::User) {
                   sk.set_TrFunc(coeffA,coeffB);
               }
               ok = sk.calcFilter();
               sk.createPolesZerosList(lst);
               sk.createPartList(lst);
	       txtResult->appendHtml("<pre>" + lst.join("\n") + "</pre>");
               if (ok) {
                   sk.createSchematic(s);
               } else {
                   errorMessage(tr("Unable to implement filter with such parameters and topology \n"
                                   "Change parameters and/or topology and try again!"));
               }
             }
             break;
    default : errorMessage(tr("Function will be implemented in future version"));
             break;
    }

    if (ok) {
      statusBar()->showMessage(tr("Filter calculation was successful"), 2000);
        txtResult->appendHtml("<pre>\r\n" + 
			      tr("Filter calculation was successful") +
			      "</pre>");
    } else {
      statusBar()->showMessage(tr("Filter calculation terminated with error!"), 2000);
        txtResult->appendHtml("<pre>\r\n" +
			      tr("Filter calculation terminated with error") +
			      "</pre>");
    }

    QClipboard *cb = QApplication::clipboard();
    cb->setText(s);

}

void QucsActiveFilter::slotUpdateResponse()
{
    QString s = ":/images/bitmaps/AFR.svg";

    switch (cbxResponse->currentIndex()) {
        case tLowPass :
            s = ":/images/bitmaps/AFR.svg";
            ftyp = Filter::LowPass;
            break;
        case tHiPass : s = ":/images/bitmaps/high-pass.svg";
            ftyp = Filter::HighPass;
            break;
        case tBandPass : s = ":/images/bitmaps/bandpass.svg";
            ftyp = Filter::BandPass;
            break;
        case tBandStop : s = ":/images/bitmaps/bandstop.svg";
            ftyp = Filter::BandStop;
            break;
        default: ftyp = Filter::NoFilter;
            break;
        }

    imgAFR->load(s);
}

void QucsActiveFilter::slotUpdateSchematic()
{
    slotUpdateResponse();
    // provide a default for cases not defined below (transient)
    QString s = ":/images/bitmaps/mfb-lowpass.svg";
    switch (cbxFilterType->currentIndex()) {
    case topoCauer : if ((ftyp==Filter::BandStop)||
                         (ftyp==Filter::BandPass)) {
            s = ":images/bitmaps/cauer-bandpass.svg"; // Cauer section
        } else {
            s = ":images/bitmaps/cauer.svg"; // Cauer section
        }
             break;
    case topoMFB : if (ftyp==Filter::HighPass) { // Multifeedback
            s = ":/images/bitmaps/mfb-highpass.svg";
        } else if (ftyp==Filter::LowPass) {
            s = ":/images/bitmaps/mfb-lowpass.svg";
        } else if (ftyp==Filter::BandPass) {
            s = ":/images/bitmaps/mfb-bandpass.svg";
        }
             break;
    case topoSallenKey : if (ftyp==Filter::HighPass) { // Sallen-Key
            s = ":/images/bitmaps/sk-highpass.svg";
        } else if (ftyp==Filter::LowPass) {
           s = ":/images/bitmaps/sk-lowpass.svg";
        } else if (ftyp==Filter::BandPass) {
           s = ":/images/bitmaps/sk-bandpass.svg";
        }
        break;
    default : s = ":/images/bitmaps/mfb-lowpass.svg";
        break;
    }

    sch_pic->load(s);
}

void QucsActiveFilter::slotSwitchParameters()
{
    if (cbxFilterFunc->currentIndex()==funcButterworth) { // Butterworth
        edtA1->setEnabled(true);
        edtPassbRpl->setEnabled(false);
    } else {
        edtA1->setEnabled(false);
        edtPassbRpl->setEnabled(true);
    }

    if ((cbxFilterFunc->currentIndex()==funcCauer)||
        (cbxFilterFunc->currentIndex()==funcInvChebyshev)||
        (cbxResponse->currentIndex()==tBandStop)) { // Inverse Chebyshev
                                                                                  // or Cauer
        cbxFilterType->setDisabled(true);
    } else {
        cbxFilterType->setDisabled(false);
    }

    if ((cbxFilterFunc->currentIndex()==funcInvChebyshev)||  // Inv.Chebyshev
        (cbxFilterFunc->currentIndex()==funcCauer)||  // Cauer
        (cbxFilterFunc->currentIndex()==funcUser)|| // or User defined
        (cbxResponse->currentIndex()==tBandStop))
    {
        cbxFilterType->addItem(tr("Cauer section"),Qt::DisplayRole);
        cbxFilterType->setCurrentIndex(topoCauer);
    } else {
        cbxFilterType->removeItem(topoCauer);
    }

    if ((cbxFilterFunc->currentIndex())==funcBessel) { // Bessel
        edtOrder->setEnabled(true);
    } else {
        edtOrder->setEnabled(false);
    }

    if ((cbxFilterFunc->currentIndex()==funcUser)||
        (cbxFilterFunc->currentIndex()==funcBessel)) { // Bessel or User Def.
        btnDefineTransferFunc->setEnabled(true);
        edtF2->setEnabled(false);
        edtPassbRpl->setEnabled(false);
        edtA1->setEnabled(false);
        edtA2->setEnabled(false);
        edtKv->setEnabled(false);
    } else {
        btnDefineTransferFunc->setEnabled(false);
        edtF2->setEnabled(true);
        edtPassbRpl->setEnabled(true);
        edtA1->setEnabled(true);
        edtA2->setEnabled(true);
        edtKv->setEnabled(true);
    }
}

void QucsActiveFilter::slotSetLabels()
{
    if ((cbxResponse->currentIndex()==tBandPass)|| // set proper labels
        (cbxResponse->currentIndex()==tBandStop)) {
        lblF1->setText(tr("Upper cutoff frequency, Fu (Hz)"));
        lblF2->setText(tr("Lower cutoff frequency, Fl (Hz)"));
        lblA1->setText(tr("Transient bandwidth, TW (Hz)"));
        //lblA2->setEnabled(false);
        //edtA2->setEnabled(false);
    } else {
        lblF1->setText(tr("Cutoff frequency, Fc (Hz)"));
        lblF2->setText(tr("Stopband frequency, Fs (Hz)"));
        lblA1->setText(tr("Passband attenuation, Ap (dB)"));
        //lblA2->setEnabled(true);
        //edtA2->setEnabled(true);
    }
}

void QucsActiveFilter::slotDefineTransferFunc()
{

    TransferFuncDialog *trfuncdlg = new TransferFuncDialog(coeffA,coeffB,this);
    if (trfuncdlg->exec()) {
        trfuncdlg->getCoeffs(coeffA,coeffB);
    }
    delete trfuncdlg;
}

void QucsActiveFilter::errorMessage(QString str)
{
    statusBar()->showMessage(tr("Error!"));
    QMessageBox* msg =  new QMessageBox(QMessageBox::Critical,tr("Active filter design"),
                                        str,
                                        QMessageBox::Ok);
    msg->exec();
    delete msg;
}

void QucsActiveFilter::fixSize()
{
  // make main window fit widgets height
  resize(width(), 1);
  statusBar()->showMessage(tr("Ready."));
}

void QucsActiveFilter::slotViewConsole(bool toggle) {
  gpbCons->setVisible(toggle);
  QucsSettings.showConsole = toggle;

  // a QMainWindow does not automatically resize when a child widget is hidden
  //  only way seems to force a resize after all the events have been processed
  QApplication::processEvents();
  QTimer::singleShot(0, this, SLOT(fixSize()));
}

void QucsActiveFilter::slotHelpAbout()
{
  QMessageBox::about(this, tr("About..."),
    "QucsActiveFilter Version " PACKAGE_VERSION+
    tr("\nActive Filter synthesis program\n")+
    tr("Copyright (C) 2014, 2015 by")+
    "\nVadim Kuznetsov\n"
    "\nThis is free software; see the source for copying conditions."
    "\nThere is NO warranty; not even for MERCHANTABILITY or "
    "\nFITNESS FOR A PARTICULAR PURPOSE.\n\n");
}

// ************************************************************
void QucsActiveFilter::slotHelpAboutQt()
{
  QMessageBox::aboutQt(this, tr("About Qt"));
}

// ************************************************************
void QucsActiveFilter::slotHelpIntro()
{
  HelpDialog *d = new HelpDialog(this);
  d->exec();
  delete d;
}
