/***************************************************************************
                          qucstrans.cpp  -  description
                             -------------------
    begin                : Sun Feb 27 2005
    copyright            : (C) 2005, 2006 by Stefan Jahn
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

#include <QDate>
#include <QTime>
#include <QLayout>
#include <QPushButton>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QToolButton>
#include <QImage>
#include <QFileDialog>
#include <QMenuBar>
#include <QAction>
#include <QGroupBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QImage>
#include <QIcon>
#include <QToolTip>
#include <QRadioButton>
#include <QStatusBar>
#include <QDir>
#include <QButtonGroup>
#include <QStackedWidget>
#include <QRegExp>
#include <QValidator>
#include <QClipboard>
#include <QApplication>
#include <QPixmap>
#include <QVBoxLayout>
#include <QCloseEvent>
#include <QAction>
#include <QDebug>

#include "qucstrans.h"
#include "helpdialog.h"
#include "optionsdialog.h"
#include "transline.h"
#include "units.h"
#include "microstrip.h"
#include "coplanar.h"
#include "coax.h"
#include "rectwaveguide.h"
#include "c_microstrip.h"

// Defines maximum number of entries in each property category.
static const int TransMaxBox[MAX_TRANS_BOXES] = { 9, 1, 4, 3 };

// Helper #defines for the below transmission line types.
#define TRANS_RADIOS  { -1, -1, -1, -1 }
#define TRANS_QOBJS   NULL, NULL, NULL, NULL
#define TRANS_END     { NULL, 0, NULL, { NULL }, 0, TRANS_QOBJS }
#define TRANS_RESULT  { NULL, NULL, NULL }
#define TRANS_RESULTS { TRANS_RESULT }

// Defines the available transmission line types.
struct TransType TransLineTypes[] = {
  { ModeMicrostrip, "Microstrip", "microstrip.png", NULL,
    { { {
      { "Er",    2.94,  NULL, TRANS_NONES,   0, TRANS_QOBJS },
      { "Mur",   1,     NULL, TRANS_NONES,   0, TRANS_QOBJS },
      { "H",     10,    NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "H_t",   1e20,  NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "T",     0.1,   NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "Cond",  4.1e7, NULL, TRANS_NONES,   0, TRANS_QOBJS },
      { "Tand",  0,     NULL, TRANS_NONES,   0, TRANS_QOBJS },
      { "Rough", 0,     NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Freq",  1, NULL, TRANS_FREQS, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "W",     10,  NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "L",     100, NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Z0",    50, NULL, TRANS_OHMS,   0, TRANS_QOBJS },
      { "Ang_l", 90, NULL, TRANS_ANGLES, 0, TRANS_QOBJS },
      TRANS_END
    } } },
    4, TRANS_RESULTS, TRANS_RADIOS
  },
  { ModeCoplanar, "Coplanar", "cpw.png", NULL,
    { { {
      { "Er",    2.94,  NULL, TRANS_NONES,   0, TRANS_QOBJS },
      { "H",     10,    NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "T",     0.1,   NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "Cond",  4.1e7, NULL, TRANS_NONES,   0, TRANS_QOBJS },
      { "Tand",  0,     NULL, TRANS_NONES,   0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Freq",  1, NULL, TRANS_FREQS, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "W",     10,  NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "S",      5,  NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "L",     100, NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Z0",    50, NULL, TRANS_OHMS,   0, TRANS_QOBJS },
      { "Ang_l", 90, NULL, TRANS_ANGLES, 0, TRANS_QOBJS },
      TRANS_END
    } } },
    4, TRANS_RESULTS, { 1, 0, -1, -1 }
  },
  { ModeGroundedCoplanar, "GroundedCoplanar", "cpw_back.png", NULL,
    { { {
      { "Er",    2.94,  NULL, TRANS_NONES,   0, TRANS_QOBJS },
      { "H",     10,    NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "T",     0.1,   NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "Cond",  4.1e7, NULL, TRANS_NONES,   0, TRANS_QOBJS },
      { "Tand",  0,     NULL, TRANS_NONES,   0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Freq",  1, NULL, TRANS_FREQS, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "W",     10,  NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "S",      5,  NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "L",     100, NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Z0",    50, NULL, TRANS_OHMS,   0, TRANS_QOBJS },
      { "Ang_l", 90, NULL, TRANS_ANGLES, 0, TRANS_QOBJS },
      TRANS_END
    } } },
    4, TRANS_RESULTS, { 1, 0, -1, -1 }
  },
  { ModeRectangular, "Rectangular", "rectwaveguide.png", NULL,
    { { {
      { "Er",    1,     NULL, TRANS_NONES, 0, TRANS_QOBJS },
      { "Mur",   1,     NULL, TRANS_NONES, 0, TRANS_QOBJS },
      { "Cond",  4.1e7, NULL, TRANS_NONES, 0, TRANS_QOBJS },
      { "Tand",  0,     NULL, TRANS_NONES, 0, TRANS_QOBJS },
      { "TanM",  0,     NULL, TRANS_NONES, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Freq",  10, NULL, TRANS_FREQS, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "a",     1000, NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "b",     500,  NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "L",     4000, NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Z0",    50, NULL, TRANS_OHMS,   0, TRANS_QOBJS },
      { "Ang_l", 90, NULL, TRANS_ANGLES, 0, TRANS_QOBJS },
      TRANS_END
    } } },
    3, TRANS_RESULTS, { 0, 1, -1, -1 }
  },
  { ModeCoaxial, "Coaxial", "coax.png", NULL,
    { { {
      { "Er",    2.1,   NULL, TRANS_NONES, 0, TRANS_QOBJS },
      { "Mur",   1,     NULL, TRANS_NONES, 0, TRANS_QOBJS },
      { "Tand",  0.002, NULL, TRANS_NONES, 0, TRANS_QOBJS },
      { "Sigma", 4.1e7, NULL, TRANS_NONES, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Freq",  10, NULL, TRANS_FREQS, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "din",   40,   NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "dout",  134,  NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "L",     1000, NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Z0",    50, NULL, TRANS_OHMS,   0, TRANS_QOBJS },
      { "Ang_l", 90, NULL, TRANS_ANGLES, 0, TRANS_QOBJS },
      TRANS_END
    } } },
    2, TRANS_RESULTS, { 0, 1, -1, -1 }
  },
  { ModeCoupledMicrostrip, "CoupledMicrostrip", "c_microstrip.png", NULL,
    { { {
      { "Er",    4.3,   NULL, TRANS_NONES,   0, TRANS_QOBJS },
      { "Mur",   1,     NULL, TRANS_NONES,   0, TRANS_QOBJS },
      { "H",     8.27,  NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "H_t",   1e20,  NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "T",     0.68,  NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "Cond",  4.1e7, NULL, TRANS_NONES,   0, TRANS_QOBJS },
      { "Tand",  0,     NULL, TRANS_NONES,   0, TRANS_QOBJS },
      { "Rough", 0,     NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Freq",  10, NULL, TRANS_FREQS, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "W",     8.66, NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "S",     5.31, NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      { "L",     1000, NULL, TRANS_LENGTHS, 0, TRANS_QOBJS },
      TRANS_END
    } },
    { {
      { "Z0e",   50, NULL, TRANS_OHMS,   0, TRANS_QOBJS },
      { "Z0o",   50, NULL, TRANS_OHMS,   0, TRANS_QOBJS },
      { "Ang_l", 90, NULL, TRANS_ANGLES, 0, TRANS_QOBJS },
      TRANS_END
    } } },
    7, TRANS_RESULTS, TRANS_RADIOS
  },
  { ModeNone, NULL, NULL, NULL, { { { TRANS_END } } }, 0,
    TRANS_RESULTS, TRANS_RADIOS }
};

struct TransUnit TransUnits[] = {
  { "Frequency",  TRANS_FREQS },
  { "Length",     TRANS_LENGTHS },
  { "Resistance", TRANS_OHMS },
  { "Angle",      TRANS_ANGLES },
};

/* Constructor setups the GUI. */
QucsTranscalc::QucsTranscalc() {
    
  QWidget *centralWidget = new QWidget(this);  
  setCentralWidget(centralWidget);
  
  // set application icon
  setWindowIcon(QPixmap(QucsSettings.BitmapDir + "big.qucs.xpm"));
  setWindowTitle("Qucs Transcalc " PACKAGE_VERSION);

  // create file menu
  QMenu *fileMenu = new QMenu(tr("&File"));

  QAction *fileLoad = new QAction(tr("&Load"), this);
  fileLoad->setShortcut(Qt::CTRL+Qt::Key_L);
  fileMenu->addAction(fileLoad);
  connect(fileLoad, SIGNAL(activated()), SLOT(slotFileLoad()));

  QAction *fileSave = new QAction (tr("&Save"), this);
  fileSave->setShortcut(Qt::CTRL+Qt::Key_S);
  fileMenu->addAction(fileSave);
  connect(fileSave, SIGNAL(activated()), SLOT(slotFileSave()));

  fileMenu->addSeparator();

  QAction *fileOption = new QAction (tr("&Options"), this);
  fileOption->setShortcut(Qt::CTRL+Qt::Key_O);
  fileMenu->addAction(fileOption);
  connect(fileOption, SIGNAL(activated()), SLOT(slotOptions()));

  fileMenu->addSeparator();

  QAction *fileQuit = new QAction (tr("&Quit"), this);
  fileQuit->setShortcut(Qt::CTRL+Qt::Key_Q);
  fileMenu->addAction(fileQuit);
  connect(fileQuit, SIGNAL(activated()), SLOT(slotQuit()));

  // create execute menu
  QMenu *execMenu = new QMenu(tr("&Execute"));

  QAction *execCopy = new QAction(tr("&Copy to Clipboard"), this);
  execCopy->setShortcut(Qt::Key_F2);
  execMenu->addAction(execCopy);
  connect(execCopy, SIGNAL(activated()), SLOT(slotCopyToClipBoard()));

  QAction *execAnalyze = new QAction(tr("&Analyze"), this);
  execAnalyze->setShortcut(Qt::Key_F3);
  execMenu->addAction(execAnalyze);
  connect(execAnalyze, SIGNAL(activated()), SLOT(slotAnalyze()));

  QAction *execSynthesize = new QAction (tr("&Synthesize"), this);
  execSynthesize->setShortcut(Qt::Key_F4);
  execMenu->addAction(execSynthesize);
  connect(execSynthesize, SIGNAL(activated()), SLOT(slotSynthesize()));

  // create help menu
  QMenu *helpMenu = new QMenu(tr("&Help"));
  QAction *helpHelp = new QAction(tr("&Help"), this);
  helpHelp->setShortcut(Qt::Key_F1);
  helpMenu->addAction(helpHelp);
  connect(helpHelp, SIGNAL(activated()), SLOT(slotHelpIntro()));

  QAction *helpAbout = new QAction(tr("About"), this);
  helpMenu->addAction(helpAbout);
  connect(helpAbout, SIGNAL(activated()), SLOT(slotAbout()));

  // setup menu bar
  menuBar()->addMenu(fileMenu);
  menuBar()->addMenu(execMenu);
  menuBar()->addSeparator();
  menuBar()->addMenu(helpMenu);

  // === left
  // seletion combo and figure
  QVBoxLayout *vl = new QVBoxLayout();

  // transmission line type choice
  QGroupBox * lineGroup = new QGroupBox (tr("Transmission Line Type"));
  tranType = new QComboBox (lineGroup);
  tranType->insertItem (0, tr("Microstrip Line"));
  tranType->insertItem (1, tr("Coplanar Waveguide"));
  tranType->insertItem (2, tr("Grounded Coplanar"));
  tranType->insertItem (3, tr("Rectangular Waveguide"));
  tranType->insertItem (4, tr("Coaxial Line"));
  tranType->insertItem (5, tr("Coupled Microstrip"));
  connect(tranType, SIGNAL(activated(int)), SLOT(slotSelectType(int)));
  // setup transmission line picture
  pix = new QLabel (lineGroup);
  pix->setPixmap(QPixmap(QucsSettings.BitmapDir+"microstrip.png"));

  QVBoxLayout *vfig = new QVBoxLayout();
  vfig->addWidget(tranType);
  vfig->addWidget(pix);
  vfig->setSpacing(3);
  lineGroup->setLayout(vfig);

  vl->addWidget(lineGroup);

  // init additional translations
  setupTranslations ();

  // set current mode
  mode = ModeMicrostrip;

  // === middle
  QVBoxLayout *vm = new QVBoxLayout();
  vm->setSpacing (3);

  // substrate parameter box
  QGroupBox * substrate = new QGroupBox (tr("Substrate Parameters"));
  vm->addWidget(substrate);

  // Pass the GroupBox > create Grid layout > Add widgets > set layout
  createPropItems (substrate, TRANS_SUBSTRATE);

  // component parameter box
  QGroupBox * component = new QGroupBox (tr("Component Parameters"));
  vm->addWidget(component);
  createPropItems (component, TRANS_COMPONENT);


  // === right
  QVBoxLayout *vr = new QVBoxLayout();
  vr->setSpacing (3);

  // physical parameter box
  QGroupBox * physical = new QGroupBox (tr("Physical Parameters"));
  vr->addWidget(physical);
  createPropItems (physical, TRANS_PHYSICAL);

  // analyze/synthesize buttons
  QHBoxLayout * h2 = new QHBoxLayout();
  QPushButton * analyze = new QPushButton (tr("Analyze"));
  h2->addWidget(analyze);
  analyze->setToolTip(tr("Derive Electrical Parameters"));
  connect(analyze, SIGNAL(clicked()), SLOT(slotAnalyze()));

  QPushButton * synthesize = new QPushButton (tr("Synthesize"));
  h2->addWidget(synthesize);
  synthesize->setToolTip(tr("Compute Physical Parameters"));
  connect(synthesize, SIGNAL(clicked()), SLOT(slotSynthesize()));
  vr->addLayout(h2);

  // electrical parameter box
  QGroupBox * electrical = new QGroupBox (tr("Electrical Parameters"));
  vr->addWidget(electrical);
  createPropItems (electrical, TRANS_ELECTRICAL);

  calculated = new QGroupBox (tr("Calculated Results"));
  vr->addWidget(calculated);

  // status line
  //statBar = new QStatusBar (this);
  //QLabel * statText = new QLabel ("Ready.", statBar);
  statusBar()->showMessage (tr("Ready."));
  //statBar->setFixedHeight (statText->height ());
  //delete statText;

  QVBoxLayout *vmain = new QVBoxLayout();

  QHBoxLayout *hmain = new QHBoxLayout();
  hmain->addLayout(vl);
  hmain->addLayout(vm);
  hmain->addLayout(vr);

  vmain->addLayout(hmain);
  //vmain->addWidget(statBar);
  
  centralWidget->setLayout(vmain);
  
  // setup calculated result bix
  createResultItems (calculated);
  updateSelection ();

  // instantiate transmission lines
  TransLineTypes[0].line = new microstrip ();
  TransLineTypes[0].line->setApplication (this);
  TransLineTypes[1].line = new coplanar ();
  TransLineTypes[1].line->setApplication (this);
  TransLineTypes[2].line = new groundedCoplanar ();
  TransLineTypes[2].line->setApplication (this);
  TransLineTypes[3].line = new rectwaveguide ();
  TransLineTypes[3].line->setApplication (this);
  TransLineTypes[4].line = new coax ();
  TransLineTypes[4].line->setApplication (this);
  TransLineTypes[5].line = new c_microstrip ();
  TransLineTypes[5].line->setApplication (this);
}

/* Destructor destroys the application. */
QucsTranscalc::~QucsTranscalc()
{
  for (int i = 0; i < MAX_TRANS_TYPES; i++) {
    if (TransLineTypes[i].line)
      delete TransLineTypes[i].line;
  }
}

/* The function puts translatable text into the transmission line
   structures. */
void QucsTranscalc::setupTranslations () {
  // calculated results
  int i = 0;
  TransLineTypes[i].result[0].name = new QString(tr("ErEff"));
  TransLineTypes[i].result[1].name = new QString(tr("Conductor Losses"));
  TransLineTypes[i].result[2].name = new QString(tr("Dielectric Losses"));
  TransLineTypes[i].result[3].name = new QString(tr("Skin Depth"));

  i++;
  TransLineTypes[i].result[0].name = new QString(tr("ErEff"));
  TransLineTypes[i].result[1].name = new QString(tr("Conductor Losses"));
  TransLineTypes[i].result[2].name = new QString(tr("Dielectric Losses"));
  TransLineTypes[i].result[3].name = new QString(tr("Skin Depth"));

  i++;
  TransLineTypes[i].result[0].name = new QString(tr("ErEff"));
  TransLineTypes[i].result[1].name = new QString(tr("Conductor Losses"));
  TransLineTypes[i].result[2].name = new QString(tr("Dielectric Losses"));
  TransLineTypes[i].result[3].name = new QString(tr("Skin Depth"));

  i++;
  TransLineTypes[i].result[0].name = new QString(tr("ErEff"));
  TransLineTypes[i].result[1].name = new QString(tr("Conductor Losses"));
  TransLineTypes[i].result[2].name = new QString(tr("Dielectric Losses"));
  TransLineTypes[i].result[3].name = new QString(tr("TE-Modes"));
  TransLineTypes[i].result[4].name = new QString(tr("TM-Modes"));

  i++;
  TransLineTypes[i].result[0].name = new QString(tr("Conductor Losses"));
  TransLineTypes[i].result[1].name = new QString(tr("Dielectric Losses"));
  TransLineTypes[i].result[2].name = new QString(tr("TE-Modes"));
  TransLineTypes[i].result[3].name = new QString(tr("TM-Modes"));

  i++;
  TransLineTypes[i].result[0].name = new QString(tr("ErEff Even"));
  TransLineTypes[i].result[1].name = new QString(tr("ErEff Odd"));
  TransLineTypes[i].result[2].name = new QString(tr("Conductor Losses Even"));
  TransLineTypes[i].result[3].name = new QString(tr("Conductor Losses Odd"));
  TransLineTypes[i].result[4].name = new QString(tr("Dielectric Losses Even"));
  TransLineTypes[i].result[5].name = new QString(tr("Dielectric Losses Odd"));
  TransLineTypes[i].result[6].name = new QString(tr("Skin Depth"));

  // extra tool tips
  struct TransType * t = TransLineTypes;
  t->array[0].item[0].tip = new QString(tr("Relative Permittivity"));
  t->array[0].item[1].tip = new QString(tr("Relative Permeability"));
  t->array[0].item[2].tip = new QString(tr("Height of Substrate"));
  t->array[0].item[3].tip = new QString(tr("Height of Box Top"));
  t->array[0].item[4].tip = new QString(tr("Strip Thickness"));
  t->array[0].item[5].tip = new QString(tr("Strip Conductivity"));
  t->array[0].item[6].tip = new QString(tr("Dielectric Loss Tangent"));
  t->array[0].item[7].tip = new QString(tr("Conductor Roughness"));
  t->array[1].item[0].tip = new QString(tr("Frequency"));
  t->array[2].item[0].tip = new QString(tr("Line Width"));
  t->array[2].item[1].tip = new QString(tr("Line Length"));
  t->array[3].item[0].tip = new QString(tr("Characteristic Impedance"));
  t->array[3].item[1].tip = new QString(tr("Electrical Length"));

  t++;
  t->array[0].item[0].tip = new QString(tr("Relative Permittivity"));
  t->array[0].item[1].tip = new QString(tr("Height of Substrate"));
  t->array[0].item[2].tip = new QString(tr("Strip Thickness"));
  t->array[0].item[3].tip = new QString(tr("Strip Conductivity"));
  t->array[0].item[4].tip = new QString(tr("Dielectric Loss Tangent"));
  t->array[1].item[0].tip = new QString(tr("Frequency"));
  t->array[2].item[0].tip = new QString(tr("Line Width"));
  t->array[2].item[1].tip = new QString(tr("Gap Width"));
  t->array[2].item[2].tip = new QString(tr("Line Length"));
  t->array[3].item[0].tip = new QString(tr("Characteristic Impedance"));
  t->array[3].item[1].tip = new QString(tr("Electrical Length"));

  t++;
  t->array[0].item[0].tip = new QString(tr("Relative Permittivity"));
  t->array[0].item[1].tip = new QString(tr("Height of Substrate"));
  t->array[0].item[2].tip = new QString(tr("Strip Thickness"));
  t->array[0].item[3].tip = new QString(tr("Strip Conductivity"));
  t->array[0].item[4].tip = new QString(tr("Dielectric Loss Tangent"));
  t->array[1].item[0].tip = new QString(tr("Frequency"));
  t->array[2].item[0].tip = new QString(tr("Line Width"));
  t->array[2].item[1].tip = new QString(tr("Gap Width"));
  t->array[2].item[2].tip = new QString(tr("Line Length"));
  t->array[3].item[0].tip = new QString(tr("Characteristic Impedance"));
  t->array[3].item[1].tip = new QString(tr("Electrical Length"));

  t++;
  t->array[0].item[0].tip = new QString(tr("Relative Permittivity"));
  t->array[0].item[1].tip = new QString(tr("Relative Permeability"));
  t->array[0].item[2].tip = new QString(tr("Conductivity of Metal"));
  t->array[0].item[3].tip = new QString(tr("Dielectric Loss Tangent"));
  t->array[0].item[4].tip = new QString(tr("Magnetic Loss Tangent"));
  t->array[1].item[0].tip = new QString(tr("Frequency"));
  t->array[2].item[0].tip = new QString(tr("Width of Waveguide"));
  t->array[2].item[1].tip = new QString(tr("Height of Waveguide"));
  t->array[2].item[2].tip = new QString(tr("Waveguide Length"));
  t->array[3].item[0].tip = new QString(tr("Characteristic Impedance"));
  t->array[3].item[1].tip = new QString(tr("Electrical Length"));

  t++;
  t->array[0].item[0].tip = new QString(tr("Relative Permittivity"));
  t->array[0].item[1].tip = new QString(tr("Relative Permeability"));
  t->array[0].item[2].tip = new QString(tr("Dielectric Loss Tangent"));
  t->array[0].item[3].tip = new QString(tr("Conductivity of Metal"));
  t->array[1].item[0].tip = new QString(tr("Frequency"));
  t->array[2].item[0].tip = new QString(tr("Inner Diameter"));
  t->array[2].item[1].tip = new QString(tr("Outer Diameter"));
  t->array[2].item[2].tip = new QString(tr("Length"));
  t->array[3].item[0].tip = new QString(tr("Characteristic Impedance"));
  t->array[3].item[1].tip = new QString(tr("Electrical Length"));

  t++;
  t->array[0].item[0].tip = new QString(tr("Relative Permittivity"));
  t->array[0].item[1].tip = new QString(tr("Relative Permeability"));
  t->array[0].item[2].tip = new QString(tr("Height of Substrate"));
  t->array[0].item[3].tip = new QString(tr("Height of Box Top"));
  t->array[0].item[4].tip = new QString(tr("Strip Thickness"));
  t->array[0].item[5].tip = new QString(tr("Strip Conductivity"));
  t->array[0].item[6].tip = new QString(tr("Dielectric Loss Tangent"));
  t->array[0].item[7].tip = new QString(tr("Conductor Roughness"));
  t->array[1].item[0].tip = new QString(tr("Frequency"));
  t->array[2].item[0].tip = new QString(tr("Line Width"));
  t->array[2].item[1].tip = new QString(tr("Gap Width"));
  t->array[2].item[2].tip = new QString(tr("Length"));
  t->array[3].item[0].tip = new QString(tr("Even-Mode Impedance"));
  t->array[3].item[1].tip = new QString(tr("Odd-Mode Impedance"));
  t->array[3].item[2].tip = new QString(tr("Electrical Length"));
}

/* Creates a property item 'val' in a parameter category specified by
   its 'box'. */
void QucsTranscalc::createPropItem (QGridLayout * parentGrid, TransValue * val,
                    int box, QButtonGroup * group) {
  QRadioButton * r = NULL;
  QLabel * l;
  QLineEdit * e;
  QComboBox * c;
  QDoubleValidator * v = new QDoubleValidator (this);

  // name label
  l = new QLabel (val->name);
  parentGrid->addWidget(l, parentGrid->rowCount(), 0);

  l->setAlignment (Qt::AlignRight);
  if (val->tip) l->setToolTip(*(val->tip));
  val->label = l;

  // editable value text
  e = new QLineEdit ();
  parentGrid->addWidget(e, parentGrid->rowCount()-1, 1);
  e->setText (QString::number (val->value));
  e->setAlignment (Qt::AlignRight);
  e->setValidator (v);
  connect(e, SIGNAL(textChanged(const QString&)), SLOT(slotValueChanged()));
  if (!val->name) e->setDisabled (true);
  val->lineedit = e;

  // unit choice
  c = new QComboBox ();
  parentGrid->addWidget(c, parentGrid->rowCount()-1, 2);
  if (!val->units[0]) {
    c->addItem ("NA");
    c->setDisabled(true);
  }
  else {
    int nounit = 0;
    for (int i = 0; val->units[i]; i++) {
      c->addItem (val->units[i]);
      if (!strcmp (val->units[i], "NA")) nounit++;
    }
    c->setDisabled (nounit != 0);
    c->setCurrentIndex (0);
  }
  connect(c, SIGNAL(activated(int)), SLOT(slotValueChanged()));
  val->combobox = c;

  // special synthesize-computation choice
  if (box == TRANS_PHYSICAL) {
    r = new QRadioButton ();
    r->setDisabled (true);
    r->setHidden(true);
    val->radio = r;
    parentGrid->addWidget(r, parentGrid->rowCount()-1, 3);
  }
}

/* The function changes the content of a GUI's property entry
   depending on the given transmission line property. */
void QucsTranscalc::updatePropItem (TransValue * val) {
  // update label text
  val->label->setText (val->name);
  if (val->tip) val->label->setToolTip(*(val->tip));
  // update editable value text
  val->lineedit->setText (QString::number (val->value));
  val->lineedit->setDisabled (!val->name);

  // update unit choice
  val->combobox->clear();
  if (!val->units[0]) {
    val->combobox->addItem ("NA");
    val->combobox->setDisabled (true);
  }
  else {
    int nounit = 0;
    for (int i = 0; val->units[i]; i++) {
      val->combobox->addItem (val->units[i]);
      if (!strcmp (val->units[i], "NA")) nounit++;
    }
    val->combobox->setCurrentIndex(val->unit);
    val->combobox->setDisabled (nounit);
  }
}

/* Switches to the current type of transmission line layout. */
void QucsTranscalc::setMode (int _mode) {
  // change necessary?
  if (mode == _mode) return;
  storeValues ();
  mode = _mode;
  setUpdatesEnabled(false);
  updateMode ();

  // update selection and results
  updateSelection ();
  updateResultItems ();

  slotAnalyze();
  setUpdatesEnabled(true);
  repaint();
}

/* Updates the property items of the current mode. */
void QucsTranscalc::updateMode (void) {
  // go through each type of parameter category
  for (int box = 0; box < MAX_TRANS_BOXES; box++) {
    int last = 0, idx = getTypeIndex();
    struct TransValue * val = TransLineTypes[idx].array[box].item;
    // update each property item
    for (int i = 0; i < TransMaxBox[box]; i++) {
      // fix uninitialized memory
      if (val->name == NULL) last++;
      if (last) {
    val->name = NULL;
    val->value = 0;
    val->tip = NULL;
    val->units[0] = NULL;
      }
        updatePropItem (val);
      val++;
    }
  }
}

/* Updates the current choice of physical property selection. */
void QucsTranscalc::updateSelection () {
  int idx = getTypeIndex ();
  struct TransValue * val = TransLineTypes[idx].array[TRANS_PHYSICAL].item;
  for (int i = 0; i < TransMaxBox[TRANS_PHYSICAL]; i++) {
    if (TransLineTypes[idx].radio[i] != -1) {
      val->radio->setHidden(false);
      if (TransLineTypes[idx].radio[i] == 1) {
    val->radio->setDown (true);
    val->radio->setChecked (true);
    val->radio->setToolTip(tr("Selected for Calculation"));
      }
      else {
    val->radio->setDown (false);
    val->radio->setChecked (false);
    val->radio->setToolTip(tr("Check item for Calculation"));
      }
      val->radio->setDisabled (false);
    }
    else {
      val->radio->setHidden(true);
      val->radio->setDown (false);
      val->radio->setChecked (false);
      val->radio->setDisabled (true);
    }
    val++;
  }
}

/* The function creates the property items for the given category of
   transmission line parameters. */
void QucsTranscalc::createPropItems (QGroupBox *parent, int box) {
  struct TransValue * val, * dup;
  int last = 0, idx = getTypeIndex ();
  val = TransLineTypes[idx].array[box].item;

  QGridLayout *boxGrid = new QGridLayout();

  QButtonGroup * group = new QButtonGroup();
  connect(group, SIGNAL(buttonPressed(int)), SLOT(slotRadioChecked(int)));

  boxGrid->setSpacing(2);
  parent->setLayout(boxGrid);

  // go through each parameter category
  for (int i = 0; i < TransMaxBox[box]; i++) {
    // fix uninitialized memory
    if (val->name == NULL) last++;
    if (last) {
      val->name = NULL;
      val->value = 0;
      val->tip = NULL;
      val->units[0] = NULL;
    }

    createPropItem (boxGrid, val, box, group);

    // publish the newly created widgets to the other transmission lines
    for (int _mode = 0; _mode < MAX_TRANS_TYPES; _mode++) {
      if (idx != _mode) {
    dup = & TransLineTypes[_mode].array[box].item[i];
    dup->label = val->label;
    dup->lineedit = val->lineedit;
    dup->combobox = val->combobox;
    dup->radio = val->radio;
    //dup->value = val->value;
      }
    }
    val++;
  }
}

/* Creates a single result item. */
void QucsTranscalc::createResultItem (QGridLayout * parentGrid, TransResult * res) {
  QLabel * n = new QLabel (res->name ? *(res->name) + ":" : QString());
  parentGrid->addWidget(n, parentGrid->count(),0,1,1);
  n->setAlignment (Qt::AlignRight);
  res->label = n;

  QLabel * v = new QLabel ();
  parentGrid->addWidget(v, parentGrid->count()-1,1,1,1);
  v->setAlignment (Qt::AlignLeft);
  res->value = v;
  if (!res->name) {
    v->hide ();
    n->hide ();
  }
}

/* Updates the given result item. */
void QucsTranscalc::updateResultItem (TransResult * res) {
  if (res->name) {
    res->label->setText (*(res->name) + ":");
    res->label->show ();
    res->value->show ();
  } else {
    res->label->hide ();
    res->value->hide ();
  }
}

/* Creates all the result items. */
void QucsTranscalc::createResultItems (QGroupBox * parent) {
  struct TransResult * res, * dup;
  int idx = getTypeIndex ();
  res = & TransLineTypes[idx].result[0];

  QGridLayout *boxGrid = new QGridLayout();

  boxGrid->setSpacing(2);
  parent->setLayout(boxGrid);

  for (int i = 0; i < MAX_TRANS_RESULTS; i++) {
    createResultItem (boxGrid, res);
    for (int _mode = 0; _mode < MAX_TRANS_TYPES; _mode++) {
      if (idx != _mode) {
    dup = & TransLineTypes[_mode].result[i];
    dup->label = res->label;
    dup->value = res->value;
      }
    }
    res++;
  }
}

/* Puts the given result into its widget. */
void QucsTranscalc::setResult (int line, const char * text) {
  struct TransResult * res;
  res = & TransLineTypes[getTypeIndex ()].result[line];
  res->value->setText (text);
}

/* Updates the current result items. */
void QucsTranscalc::updateResultItems() {
  int idx = getTypeIndex ();
  struct TransResult * res = TransLineTypes[idx].result;
  for (int i = 0; i < MAX_TRANS_RESULTS; i++) {
    updateResultItem (res);
    res++;
  }
}

/* Returns the appropriate array index for the given mode. */
int QucsTranscalc::getTypeIndex () {
  struct TransType * type = TransLineTypes;
  for (int i = 0; type->type != ModeNone; i++) {
    if (type->type == mode) return i;
    type++;
  }
  return 0;
}

/* Returns the property value specified by its name. */
struct TransValue * QucsTranscalc::findProperty (QString prop) {
  struct TransValue * val = NULL;
  for (int box = 0; box < MAX_TRANS_BOXES; box++) {
    val = TransLineTypes[getTypeIndex ()].array[box].item;
    for (int i = 0; val->name; i++) {
      if (prop == val->name) return val;
      val++;
    }
  }
  return NULL;
}

/* Sets the given property to the given value. */
void QucsTranscalc::setProperty (QString prop, double value) {
  struct TransValue * val = findProperty (prop);
  if (val) {
    val->lineedit->setText (QString::number (value));
    val->value = value;
  }
}

/* Returns the given property value. */
double QucsTranscalc::getProperty (QString prop) {
  struct TransValue * val = findProperty (prop);
  if (val) {
    QString str = val->lineedit->text ();
    val->value = str.toDouble ();
    return val->value;
  }
  return 0;
}

/* Sets the given property unit. */
void QucsTranscalc::setUnit (QString prop, const char * unit) {
  struct TransValue * val = findProperty (prop);
  if (val) {
    if (!unit) {
      val->combobox->setCurrentIndex (0);
      val->unit = 0;
    }
    else for (int i = 0; val->units[i]; i++) {
      if (!strcmp (unit, val->units[i])) {
    val->combobox->setCurrentIndex (i);
    val->unit = i;
    break;
      }
    }
  }
}

/* Returns the given property unit. */
char * QucsTranscalc::getUnit (QString prop) {
  struct TransValue * val = findProperty (prop);
  if (val) {
    QString str = val->combobox->currentText ();
    for (int i = 0; val->units[i]; i++) {
      if (str == val->units[i]) {
    val->unit = i;
    return (char *) val->units[i];
      }
    }
  }
  return NULL;
}

/* Returns the given property selection. */
bool QucsTranscalc::isSelected (QString prop) {
  struct TransValue * val = findProperty (prop);
  if (val) {
    if (val->radio)
      if (val->radio->isChecked ()) {
    return true;
      }
  }
  return false;
}

void QucsTranscalc::slotAbout()
{
  QMessageBox::about(this, tr("About..."),
    "QucsTranscalc Version " PACKAGE_VERSION "\n"+
    tr("Transmission Line Calculator for Qucs\n")+
    tr("Copyright (C) 2001 by Gopal Narayanan\n")+
    tr("Copyright (C) 2002 by Claudio Girardi\n")+
    tr("Copyright (C) 2005 by Stefan Jahn\n")+
    tr("Copyright (C) 2008 by Michael Margraf\n")+
    "\nThis is free software; see the source for copying conditions."
    "\nThere is NO warranty; not even for MERCHANTABILITY or "
    "\nFITNESS FOR A PARTICULAR PURPOSE.");
}

void QucsTranscalc::slotQuit()
{
  int tmp;
  tmp = x();		// call size and position function in order to ...
  tmp = y();		// ... set them correctly before closing the ...
  tmp = width();	// dialog !!!  Otherwise the frame of the window ...
  tmp = height();	// will not be recognized (a X11 problem).

  storeValues ();
  qApp->quit();
}

void QucsTranscalc::closeEvent(QCloseEvent *Event)
{
  int tmp;
  tmp = x();		// call size and position function in order to ...
  tmp = y();		// ... set them correctly before closing the ...
  tmp = width();	// dialog !!!  Otherwise the frame of the window ...
  tmp = height();	// will not be recognized (a X11 problem).

  storeValues ();
  Event->accept();
}

void QucsTranscalc::slotSelectType (int Type)
{
  pix->setPixmap(QPixmap(QucsSettings.BitmapDir + QString(TransLineTypes[Type].bitmap)));
  setMode (Type);
  statusBar()->showMessage(tr("Ready."));
}

void QucsTranscalc::slotAnalyze()
{
  if (TransLineTypes[getTypeIndex()].line)
    TransLineTypes[getTypeIndex()].line->analyze();
  statusBar()->showMessage(tr("Values are consistent."));
}

void QucsTranscalc::slotSynthesize()
{
  if (TransLineTypes[getTypeIndex()].line)
    TransLineTypes[getTypeIndex()].line->synthesize();
  statusBar()->showMessage(tr("Values are consistent."));
}

void QucsTranscalc::slotValueChanged()
{
  statusBar()->showMessage(tr("Values are inconsistent."));
}

// Load transmission line values from the given file.
bool QucsTranscalc::loadFile(QString fname, int * _mode) {
  QFile file(QDir::convertSeparators (fname));
  if(!file.open(QIODevice::ReadOnly)) return false; // file doesn't exist

  QTextStream stream(&file);
  QString Line, Name, Unit;
  double Value;

  int oldmode = mode;
  while(!stream.atEnd()) {
    Line = stream.readLine();
    for (int i = 0; i < MAX_TRANS_TYPES; i++) {
      if (Line == "<" + QString(TransLineTypes[i].description) + ">") {
    if (_mode) {
      *_mode = TransLineTypes[i].type;
      setMode (*_mode);
      updatePixmap (mode);
    } else {
      mode = TransLineTypes[i].type;
    }
    while(!stream.atEnd()) {
      Line = stream.readLine();
      if (Line == "</" + QString(TransLineTypes[i].description) + ">")
        break;
      Line = Line.simplified();
      Name = Line.section(' ',0,0);
      Value = Line.section(' ',1,1).toDouble();
      Unit = Line.section(' ',2,2);
      setProperty (Name, Value);
      setUnit (Name, Unit.toAscii());
    }
    break;
      }
    }
  }
  if (!_mode) mode = oldmode;
  updateMode ();
  file.close();
  return true;
}

// Saves current transmission line values into the given file.
bool QucsTranscalc::saveFile(QString fname) {
  QFile file (QDir::convertSeparators (fname));
  if(!file.open (QIODevice::WriteOnly)) return false; // file not writable
  QTextStream stream (&file);

  // some lines of documentation
  stream << "# QucsTranscalc " << PACKAGE_VERSION << "  " << fname << "\n";
  stream << "#   Generated on " << QDate::currentDate().toString()
     << " at " << QTime::currentTime().toString() << ".\n";
  stream << "#   It is not suggested to edit the file, use QucsTranscalc "
     << "instead.\n\n";

  storeValues ();
  saveMode (stream);
  file.close ();
  return true;
}

/* Writes the transmission line values for the current modes into the
   given stream. */
void QucsTranscalc::saveMode(QTextStream& stream) {
  struct TransType * t = &TransLineTypes[getTypeIndex ()];
  struct TransValue * val = NULL;
  stream << "<" << t->description << ">\n";
  for (int box = 0; box < MAX_TRANS_BOXES; box++) {
    val = t->array[box].item;
    while (val->name) {
      stream << "  " << val->name << " " << val->value << " "
         << val->units[val->unit] << "\n";
      val++;
    }
  }
  stream << "</" << t->description << ">\n";
}

// Writes the transmission line values for all modes into the given stream.
void QucsTranscalc::saveModes(QTextStream& stream) {
  int oldmode = mode;
  for (int i = 0; i < MAX_TRANS_TYPES; i++) {
    mode = TransLineTypes[i].type;
    saveMode (stream);
  }
  mode = oldmode;
}

// Saves the GUI values into internal data structures.
void QucsTranscalc::storeValues (void) {
  struct TransType * t = &TransLineTypes[getTypeIndex ()];
  struct TransValue * val;
  for (int box = 0; box < MAX_TRANS_BOXES; box++) {
    val = t->array[box].item;
    int i = 0;
    while (val->name) {
      getProperty (val->name);
      getUnit (val->name);
      if (box == TRANS_PHYSICAL) {
    if (val->radio->isEnabled()) {
      if (val->radio->isChecked())
        t->radio[i] = 1;
      else
        t->radio[i] = 0;
    }
    else t->radio[i] = -1;
      }
      i++;
      val++;
    }
  }
}

void QucsTranscalc::slotFileLoad()
{
  statusBar()->showMessage(tr("Loading file..."));
  int _mode = 0;
  QString s = QFileDialog::getOpenFileName(this, tr("Enter a Filename"),
                           QucsSettings.QucsWorkDir.path(),
                           tr("Transcalc File") + " (*.trc)");
  if (!s.isEmpty())  {
    QucsSettings.QucsWorkDir.setPath(QDir::cleanPath(s));
    if (!loadFile (s, &_mode)) {
      QMessageBox::critical (this, tr("Error"),
                 tr("Cannot load file:")+" '"+s+"'!");
    }
  }
  else statusBar()->showMessage(tr("Loading aborted."), 2000);

  statusBar()->showMessage(tr("Ready."));
}

void QucsTranscalc::slotFileSave()
{
  statusBar()->showMessage(tr("Saving file..."));

  QString s = QFileDialog::getSaveFileName(this, tr("Enter a Filename"),
                           QucsSettings.QucsWorkDir.path(),
                           tr("Transcalc File") + " (*.trc)");
  if (!s.isEmpty())  {
    QucsSettings.QucsWorkDir.setPath(QDir::cleanPath(s));
    if (!saveFile (s)) {
      QMessageBox::critical (this, tr("Error"),
                 tr("Cannot save file:")+" '"+s+"'!");
    }
  }
  else statusBar()->showMessage(tr("Saving aborted."), 2000);

  statusBar()->showMessage(tr("Ready."));
}

// Returns the current textual mode.
QString QucsTranscalc::getMode (void) {
  return TransLineTypes[getTypeIndex ()].description;
}

// Sets the mode according to the given textual mode.
void QucsTranscalc::setMode (QString _mode) {
  for (int i = 0; i < MAX_TRANS_TYPES; i++) {
    if (TransLineTypes[i].description == _mode) {
      setMode (TransLineTypes[i].type);
      updatePixmap (mode);
      break;
    }
  }
}

// Updates the combobox and pixmap for the current mode.
void QucsTranscalc::updatePixmap (int _mode) {
  pix->setPixmap(QPixmap(QucsSettings.BitmapDir + QString(TransLineTypes[_mode].bitmap)));
  tranType->setCurrentIndex(_mode);
}

void QucsTranscalc::slotHelpIntro()
{
  HelpDialog *d = new HelpDialog(this);
  d->show();
}

void QucsTranscalc::slotOptions()
{
  OptionsDialog *d = new OptionsDialog(this);
  d->show();
}

// Translates a given textual unit into a array index.
int QucsTranscalc::translateUnit(const char * unit, int type) {
  for (int i = 0; TransUnits[type].units[i]; i++) {
    if (unit && !strcmp (unit, TransUnits[type].units[i]))
      return i;
  }
  return 0;
}

void QucsTranscalc::slotRadioChecked(int id)
{
  int idx = getTypeIndex ();
  for (int i = 0; i < TransMaxBox[TRANS_PHYSICAL]; i++) {
    if (TransLineTypes[idx].radio[i] != -1) {
      TransLineTypes[idx].radio[i] = 0;
      if (i == id) {
    TransLineTypes[idx].radio[i] = 1;
      }
    }
  }
  updateSelection ();
}

void QucsTranscalc::slotCopyToClipBoard()
{
  int created = 0;
  QString s = "<Qucs Schematic " PACKAGE_VERSION ">\n";

  // create microstrip schematic
  if (mode == ModeMicrostrip) {
    transline * l = TransLineTypes[ModeMicrostrip].line;
    s += "<Components>\n";
    s += "  <Pac P1 1 90 150 -74 -26 1 1 \"1\" 1 \"50 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n";
    s +="  <Pac P2 1 270 150 18 -26 0 1 \"2\" 1 \"50 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n";
    s += "  <GND * 1 90 180 0 0 0 0>\n";
    s += "  <GND * 1 270 180 0 0 0 0>\n";
    s += QString("  <SUBST SubstTC1 1 390 140 -30 24 0 0 \"%1\" 1 \"%2 mm\" 1 \"%3 um\" 1 \"%4\" 1 \"%5\" 1 \"%6\" 1>\n").
      arg(l->getProperty("Er")).
      arg(l->getProperty("H", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("T", UNIT_LENGTH, LENGTH_UM)).
      arg(l->getProperty("Tand")).
      arg(1 / l->getProperty("Cond")).
      arg(l->getProperty("Rough", UNIT_LENGTH, LENGTH_M));
    s += "  <.SP SPTC1 1 90 240 0 51 0 0 ";
    double freq = l->getProperty("Freq", UNIT_FREQ, FREQ_GHZ);
    if (freq > 0)
      s += QString("\"log\" 1 \"%1 GHz\" 1 \"%2 GHz\" 1 ").
    arg(freq / 10).arg(freq * 10);
    else
      s += "\"lin\" 1 \"0 GHz\" 1 \"10 GHz\" 1 ";
    s += "\"51\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n";
    s += QString("  <MLIN MSTC1 1 180 100 -26 15 0 0 \"SubstTC1\" 1 \"%1 mm\" 1 \"%2 mm\" 1 \"Hammerstad\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").
      arg(l->getProperty("W", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("L", UNIT_LENGTH, LENGTH_MM));
    s += "  <Eqn EqnTC1 1 240 260 -23 12 0 0 \"A=twoport(S,'S','A')\" 1 \"ZL=real(sqrt(A[1,2]/A[2,1]))\" 1 \"yes\" 0>\n";
    s += "</Components>\n";
    s += "<Wires>\n";
    s += "  <90 100 150 100 \"\" 0 0 0 \"\">\n";
    s += "  <90 100 90 120 \"\" 0 0 0 \"\">\n";
    s += "  <210 100 270 100 \"\" 0 0 0 \"\">\n";
//    s += "  <270 100 270 120 \"\" 0 0 0 \"\">\n";
    s += "</Wires>\n";
    created++;
  }

  // create coupled microstrip schematic
  else if (mode == ModeCoupledMicrostrip) {
    transline * l = TransLineTypes[ModeCoupledMicrostrip].line;
    s += "<Components>\n";
    s += "  <Pac P1 1 100 130 -74 -26 1 1 \"1\" 1 \"50 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n";
    s += "  <Pac P2 1 320 130 18 -26 0 1 \"2\" 1 \"50 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n";
    s += "  <Pac P3 1 280 220 18 -26 0 1 \"3\" 1 \"50 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n";
    s += "  <Pac P4 1 140 200 -74 -26 1 1 \"4\" 1 \"50 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n";
    s += "  <GND * 1 100 160 0 0 0 0>\n";
    s += "  <GND * 1 140 230 0 0 0 0>\n";
    s += "  <GND * 1 320 160 0 0 0 0>\n";
    s += "  <GND * 1 280 250 0 0 0 0>\n";
    s += QString("  <SUBST SubstTC1 1 410 220 -30 24 0 0 \"%1\" 1 \"%2 mm\" 1 \"%3 um\" 1 \"%4\" 1 \"%5\" 1 \"%6\" 1>\n").
      arg(l->getProperty("Er")).
      arg(l->getProperty("H", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("T", UNIT_LENGTH, LENGTH_UM)).
      arg(l->getProperty("Tand")).
      arg(1 / l->getProperty("Cond")).
      arg(l->getProperty("Rough", UNIT_LENGTH, LENGTH_M));
    s += "  <.SP SPTC1 1 100 290 0 51 0 0 ";
    double freq = l->getProperty("Freq", UNIT_FREQ, FREQ_GHZ);
    if (freq > 0)
      s += QString("\"log\" 1 \"%1 GHz\" 1 \"%2 GHz\" 1 ").
    arg(freq / 10).arg(freq * 10);
    else
      s += "\"lin\" 1 \"0 GHz\" 1 \"10 GHz\" 1 ";
    s += "\"51\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n";
    s += QString("  <MCOUPLED MSTC1 1 190 110 -26 37 0 0 \"SubstTC1\" 1 \"%1 mm\" 1 \"%2 mm\" 1 \"%3 mm\" 1 \"Kirschning\" 0 \"Kirschning\" 0 \"26.85\" 0>\n").
      arg(l->getProperty("W", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("L", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("S", UNIT_LENGTH, LENGTH_MM));
    s += "</Components>\n";
    s += "<Wires>\n";
    s += "  <100 80 160 80 \"\" 0 0 0 \"\">\n";
    s += "  <100 80 100 100 \"\" 0 0 0 \"\">\n";
    s += "  <140 140 140 170 \"\" 0 0 0 \"\">\n";
    s += "  <140 140 160 140 \"\" 0 0 0 \"\">\n";
    s += "  <320 80 320 100 \"\" 0 0 0 \"\">\n";
    s += "  <220 80 320 80 \"\" 0 0 0 \"\">\n";
    s += "  <280 140 280 190 \"\" 0 0 0 \"\">\n";
    s += "  <220 140 280 140 \"\" 0 0 0 \"\">\n";
    s += "</Wires>\n";
    created++;
  }

  // create coaxial line schematic
  else if (mode == ModeCoaxial) {
    transline * l = TransLineTypes[ModeCoaxial].line;
    s += "<Components>\n";
    s += "  <Pac P1 1 90 150 -74 -26 1 1 \"1\" 1 \"50 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n";
    s +="  <Pac P2 1 270 150 18 -26 0 1 \"2\" 1 \"50 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n";
    s += "  <GND * 1 90 180 0 0 0 0>\n";
    s += "  <GND * 1 270 180 0 0 0 0>\n";
    s += "  <.SP SPTC1 1 90 240 0 51 0 0 ";
    double freq = l->getProperty("Freq", UNIT_FREQ, FREQ_GHZ);
    if (freq > 0)
      s += QString("\"log\" 1 \"%1 GHz\" 1 \"%2 GHz\" 1 ").
    arg(freq / 10).arg(freq * 10);
    else
      s += "\"lin\" 1 \"0 GHz\" 1 \"10 GHz\" 1 ";
    s += "\"51\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n";
    s += QString("  <COAX CXTC1 1 180 100 -26 15 0 0 \"%1\" 1 \"%2\" 0 \"%3\" 0 \"%4 mm\" 1 \"%5 mm\" 1  \"%6 mm\" 1  \"%7\" 0 \"26.85\" 0>\n").
      arg(l->getProperty("Er")).
      arg(1 / l->getProperty("Sigma")).
      arg(l->getProperty("Mur")).
      arg(l->getProperty("dout", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("din", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("L", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("Tand"));
    s += "</Components>\n";
    s += "<Wires>\n";
    s += "  <90 100 150 100 \"\" 0 0 0 \"\">\n";
    s += "  <90 100 90 120 \"\" 0 0 0 \"\">\n";
    s += "  <210 100 270 100 \"\" 0 0 0 \"\">\n";
    s += "  <270 100 270 120 \"\" 0 0 0 \"\">\n";
    s += "</Wires>\n";
    created++;
  }

  // create coplanar schematic
  else if (mode == ModeCoplanar) {
    transline * l = TransLineTypes[ModeCoplanar].line;
    s += "<Components>\n";
    s += "  <Pac P1 1 90 150 -74 -26 1 1 \"1\" 1 \"50 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n";
    s +="  <Pac P2 1 270 150 18 -26 0 1 \"2\" 1 \"50 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n";
    s += "  <GND * 1 90 180 0 0 0 0>\n";
    s += "  <GND * 1 270 180 0 0 0 0>\n";
    s += QString("  <SUBST SubstTC1 1 390 140 -30 24 0 0 \"%1\" 1 \"%2 mm\" 1 \"%3 um\" 1 \"%4\" 1 \"%5\" 1 \"0\" 1>\n").
      arg(l->getProperty("Er")).
      arg(l->getProperty("H", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("T", UNIT_LENGTH, LENGTH_UM)).
      arg(l->getProperty("Tand")).
      arg(1 / l->getProperty("Cond"));
    s += "  <.SP SPTC1 1 90 240 0 51 0 0 ";
    double freq = l->getProperty("Freq", UNIT_FREQ, FREQ_GHZ);
    if (freq > 0)
      s += QString("\"log\" 1 \"%1 GHz\" 1 \"%2 GHz\" 1 ").
    arg(freq / 10).arg(freq * 10);
    else
      s += "\"lin\" 1 \"0 GHz\" 1 \"10 GHz\" 1 ";
    s += "\"51\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n";
    s += QString("  <CLIN CLTC1 1 180 100 -26 25 0 0 \"SubstTC1\" 1 \"%1 mm\" 1 \"%2 mm\" 1 \"%3 mm\" 1 \"Air\" 1 \"yes\" 0>\n").
      arg(l->getProperty("W", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("S", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("L", UNIT_LENGTH, LENGTH_MM));
    s += "  <Eqn EqnTC1 1 240 260 -23 12 0 0 \"A=twoport(S,'S','A')\" 1 \"ZL=real(sqrt(A[1,2]/A[2,1]))\" 1 \"yes\" 0>\n";
    s += "</Components>\n";
    s += "<Wires>\n";
    s += "  <90 100 150 100 \"\" 0 0 0 \"\">\n";
    s += "  <90 100 90 120 \"\" 0 0 0 \"\">\n";
    s += "  <210 100 270 100 \"\" 0 0 0 \"\">\n";
    s += "  <270 100 270 120 \"\" 0 0 0 \"\">\n";
    s += "</Wires>\n";
    created++;
  }

  // create coplanar schematic
  else if (mode == ModeGroundedCoplanar) {
    transline * l = TransLineTypes[ModeGroundedCoplanar].line;
    s += "<Components>\n";
    s += "  <Pac P1 1 90 150 -74 -26 1 1 \"1\" 1 \"50 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n";
    s +="  <Pac P2 1 270 150 18 -26 0 1 \"2\" 1 \"50 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n";
    s += "  <GND * 1 90 180 0 0 0 0>\n";
    s += "  <GND * 1 270 180 0 0 0 0>\n";
    s += QString("  <SUBST SubstTC1 1 390 140 -30 24 0 0 \"%1\" 1 \"%2 mm\" 1 \"%3 um\" 1 \"%4\" 1 \"%5\" 1 \"0\" 1>\n").
      arg(l->getProperty("Er")).
      arg(l->getProperty("H", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("T", UNIT_LENGTH, LENGTH_UM)).
      arg(l->getProperty("Tand")).
      arg(1 / l->getProperty("Cond"));
    s += "  <.SP SPTC1 1 90 240 0 51 0 0 ";
    double freq = l->getProperty("Freq", UNIT_FREQ, FREQ_GHZ);
    if (freq > 0)
      s += QString("\"log\" 1 \"%1 GHz\" 1 \"%2 GHz\" 1 ").
    arg(freq / 10).arg(freq * 10);
    else
      s += "\"lin\" 1 \"0 GHz\" 1 \"10 GHz\" 1 ";
    s += "\"51\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n";
    s += QString("  <CLIN CLTC1 1 180 100 -26 25 0 0 \"SubstTC1\" 1 \"%1 mm\" 1 \"%2 mm\" 1 \"%3 mm\" 1 \"Metal\" 1 \"yes\" 0>\n").
      arg(l->getProperty("W", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("S", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("L", UNIT_LENGTH, LENGTH_MM));
    s += "  <Eqn EqnTC1 1 240 260 -23 12 0 0 \"A=twoport(S,'S','A')\" 1 \"ZL=real(sqrt(A[1,2]/A[2,1]))\" 1 \"yes\" 0>\n";
    s += "</Components>\n";
    s += "<Wires>\n";
    s += "  <90 100 150 100 \"\" 0 0 0 \"\">\n";
    s += "  <90 100 90 120 \"\" 0 0 0 \"\">\n";
    s += "  <210 100 270 100 \"\" 0 0 0 \"\">\n";
    s += "  <270 100 270 120 \"\" 0 0 0 \"\">\n";
    s += "</Wires>\n";
    created++;
  }

  // rectangular waveguide schematic
  else if (mode == ModeRectangular) {
    transline * l = TransLineTypes[ModeRectangular].line;
    s += "<Components>\n";
    s += "  <Pac P1 1 90 150 -74 -26 1 1 \"1\" 1 \"50 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n";
    s +="  <Pac P2 1 270 150 18 -26 0 1 \"2\" 1 \"50 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0 \"26.85\" 0>\n";
    s += "  <GND * 1 90 180 0 0 0 0>\n";
    s += "  <GND * 1 270 180 0 0 0 0>\n";
    s += "  <.SP SPTC1 1 90 240 0 51 0 0 ";
    double freq = l->getProperty("Freq", UNIT_FREQ, FREQ_GHZ);
    if (freq > 0)
      s += QString("\"log\" 1 \"%1 GHz\" 1 \"%2 GHz\" 1 ").
    arg(freq / 10).arg(freq * 10);
    else
      s += "\"lin\" 1 \"0 GHz\" 1 \"10 GHz\" 1 ";
    s += "\"51\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n";
    s += QString("  <RECTLINE RLTC1 1 180 100 -26 25 0 0 \"%1 mm\" 1 \"%2 mm\" 1 \"%3 mm\" 1 \"%4\" 0 \"%5\" 0 \"%6\" 0 \"%7\" 0 \"26.85\" 0>\n").
      arg(l->getProperty("a", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("b", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("L", UNIT_LENGTH, LENGTH_MM)).
      arg(l->getProperty("Er")).
      arg(l->getProperty("Mur")).
      arg(l->getProperty("Tand")).
      arg(1 / l->getProperty("Cond"));
    s += "  <Eqn EqnTC1 1 240 260 -23 12 0 0 \"A=twoport(S,'S','A')\" 1 \"ZL=real(sqrt(A[1,2]/A[2,1]))\" 1 \"yes\" 0>\n";
    s += "</Components>\n";
    s += "<Wires>\n";
    s += "  <90 100 150 100 \"\" 0 0 0 \"\">\n";
    s += "  <90 100 90 120 \"\" 0 0 0 \"\">\n";
    s += "  <210 100 270 100 \"\" 0 0 0 \"\">\n";
    s += "  <270 100 270 120 \"\" 0 0 0 \"\">\n";
    s += "</Wires>\n";
    created++;
  }

  // put resulting transmission line schematic into clipboard
  QClipboard *cb = QApplication::clipboard();
  cb->setText(s);

  // put a message into status line
  if (created)
    statusBar()->showMessage(tr("Schematic copied into clipboard."), 2000);
  else
    statusBar()->showMessage(tr("Transmission line type not available."), 2000);
}
