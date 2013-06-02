/****************************************************************************
**     Qucs Attenuator Synthesis
**     qucsattenuator.cpp
**
**
**
**
**
**
**
*****************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "attenuatorfunc.h"
#include "qucsattenuator.h"
#include "helpdialog.h"

#include <QGridLayout>
#include <QPixmap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QValidator>
#include <QClipboard>
#include <QApplication>
#include <QDebug>


QucsAttenuator::QucsAttenuator()
{

  QWidget *centralWidget = new QWidget(this);  
  setCentralWidget(centralWidget);

  setWindowIcon(QPixmap(QucsSettings.BitmapDir + "big.qucs.xpm"));
  setWindowTitle("Qucs Attenuator " PACKAGE_VERSION);

  QMenu *fileMenu = new QMenu(tr("&File"));

  QAction *fileQuit = new QAction(tr("&Quit"), this);
  fileQuit->setShortcut(Qt::CTRL+Qt::Key_Q);
  connect(fileQuit, SIGNAL(activated()), SLOT(slotQuit()));

  fileMenu->addAction(fileQuit);

  QMenu *helpMenu = new QMenu(tr("&Help"));
  QAction *helpHelp = new QAction(tr("&Help"), this);
  helpHelp->setShortcut(Qt::Key_F1);
  helpMenu->addAction(helpHelp);
  connect(helpHelp, SIGNAL(activated()), SLOT(slotHelpIntro()));

  helpMenu->addSeparator();

  QAction *about = new QAction(tr("About Qt..."), this);
  helpMenu->addAction(about);
  connect(about, SIGNAL(activated()), SLOT(slotHelpAboutQt()));

  menuBar()->addMenu(fileMenu);
  menuBar()->addSeparator();
  menuBar()->addMenu(helpMenu);


  //==========Left
  QVBoxLayout *vboxLeft = new QVBoxLayout();

  QGroupBox *TopoGroup = new QGroupBox(tr("Topology"));
  QGridLayout * topoGrid = new QGridLayout(TopoGroup);

  ComboTopology = new QComboBox();//=================Topology Combobox
  ComboTopology->insertItem(1, "Pi");
  ComboTopology->insertItem(2, "Tee");
  ComboTopology->insertItem(3, "Bridged Tee");
  connect(ComboTopology, SIGNAL(activated(int)), SLOT(slotTopologyChanged()));
  topoGrid->addWidget(ComboTopology, 1,0,1,2);

  pixTopology = new QLabel(TopoGroup);//====================Pixmap for Topology
  pixTopology->setPixmap(QPixmap((QucsSettings.BitmapDir + "att_pi.png")));
  topoGrid->addWidget(pixTopology,2,0,3,2);

  topoGrid->setSpacing(5);
  TopoGroup->setLayout(topoGrid);

  vboxLeft->addWidget(TopoGroup);


  //==========Right
  QVBoxLayout *vboxRight = new QVBoxLayout();

  QGroupBox * InputGroup = new QGroupBox (tr("Input"));
  QGridLayout * inGrid = new QGridLayout();
  inGrid->setSpacing(1);

  IntVal = new QIntValidator(this);
  DoubleVal = new QDoubleValidator(this);

  LabelAtten = new QLabel(tr("Attenuation:"), InputGroup);
  inGrid ->addWidget(LabelAtten, 1,0);
  lineEdit_Attvalue = new QLineEdit(tr("1"), InputGroup);
  lineEdit_Attvalue->setValidator(DoubleVal);
  inGrid->addWidget(lineEdit_Attvalue, 1,1);
  QLabel *Label1 = new QLabel(tr("dB"), InputGroup);
  inGrid->addWidget(Label1, 1,2);

  LabelImp1 = new QLabel(tr("Zin:"), InputGroup);
  inGrid->addWidget(LabelImp1, 2,0);
  lineEdit_Zin = new QLineEdit(tr("50"), InputGroup);
  lineEdit_Zin->setValidator(DoubleVal);
  connect(lineEdit_Zin, SIGNAL(textChanged(const QString&)), this,
      SLOT(slotSetText_Zin(const QString&)) );

  inGrid->addWidget(lineEdit_Zin, 2,1);
  QLabel *Label2 = new QLabel(tr("Ohm"), InputGroup);
  inGrid->addWidget(Label2, 2,2);

  LabelImp2 = new QLabel(tr("Zout:"), InputGroup);
  inGrid->addWidget(LabelImp2, 3,0);
  lineEdit_Zout = new QLineEdit(tr("50"), InputGroup);
  lineEdit_Zout->setValidator(DoubleVal);
  connect(lineEdit_Zout, SIGNAL(textChanged(const QString&)), this,
      SLOT(slotSetText_Zout(const QString&)) );
  inGrid->addWidget(lineEdit_Zout, 3,1);
  QLabel *Label3 = new QLabel(tr("Ohm"), InputGroup);
  inGrid->addWidget(Label3, 3,2);

  InputGroup->setLayout(inGrid);

  vboxRight->addWidget(InputGroup);

  Calculate = new QPushButton(tr("Calculate and put into Clipboard"));
  connect(Calculate, SIGNAL(clicked()), SLOT(slotCalculate()));

  vboxRight->addWidget(Calculate);

  QGroupBox * OutputGroup = new QGroupBox (tr("Output"));
  QGridLayout * outGrid = new QGridLayout(OutputGroup);
  outGrid->setSpacing(1);

  LabelR1 = new QLabel(tr("R1:"), OutputGroup);
  outGrid->addWidget(LabelR1, 1,0);
  lineEdit_R1 = new QLineEdit(tr("--"), OutputGroup);
  outGrid->addWidget(lineEdit_R1, 1,1);
  QLabel *Label4 = new QLabel(tr("Ohm"), OutputGroup);
  outGrid->addWidget(Label4, 1,2);

  LabelR2 = new QLabel(tr("R2:"), OutputGroup);
  outGrid->addWidget(LabelR2, 2,0);
  lineEdit_R2 = new QLineEdit(tr("--"), OutputGroup);
  outGrid->addWidget(lineEdit_R2, 2,1);
  QLabel *Label5 = new QLabel(tr("Ohm"), OutputGroup);
  outGrid->addWidget(Label5, 2,2);

  LabelR3 = new QLabel(tr("R3:"), OutputGroup);
  outGrid->addWidget(LabelR3, 3,0);
  lineEdit_R3 = new QLineEdit(tr("--"), OutputGroup);
  outGrid->addWidget(lineEdit_R3, 3,1);
  LabelR3_Ohm = new QLabel(tr("Ohm"), OutputGroup);
  outGrid->addWidget(LabelR3_Ohm, 3,2);


  vboxRight->addWidget(OutputGroup);

  // put Left and Right together
  QHBoxLayout *hbox = new QHBoxLayout();
  hbox->addLayout(vboxLeft);
  hbox->addLayout(vboxRight);

  // append the result label
  LabelResult = new QLabel(tr("Result:"));
  LabelResult->setAlignment(Qt::AlignHCenter);

  QVBoxLayout *vbox = new QVBoxLayout();
  vbox->addLayout(hbox);
  vbox->addWidget(LabelResult);

  centralWidget->setLayout(vbox);

}

QucsAttenuator::~QucsAttenuator()
{
  delete IntVal;
  delete DoubleVal;
}

void QucsAttenuator::slotHelpIntro()
{
  HelpDialog *d = new HelpDialog(this);
  d->show();
}

void QucsAttenuator::slotHelpAboutQt()
{
      QMessageBox::aboutQt(this, tr("About Qt"));
}

void QucsAttenuator::slotHelpAbout()
{
    QMessageBox::about(this, tr("About..."),
    "QucsAttenuator Version " PACKAGE_VERSION+
    tr("\nAttenuator synthesis program\n")+
    tr("Copyright (C) 2006 by")+" Toyoyuki Ishikawa"
    "\n"+
    tr("Copyright (C) 2006 by")+" Stefan Jahn"
    "\n"
    "\nThis is free software; see the source for copying conditions."
    "\nThere is NO warranty; not even for MERCHANTABILITY or "
    "\nFITNESS FOR A PARTICULAR PURPOSE.\n\n");
}

void QucsAttenuator::slotQuit()
{
  int tmp;
  tmp = x();
  tmp = y();
  tmp = width();
  tmp = height();

  qApp->quit();
}

void QucsAttenuator::slotSetText_Zin( const QString &text )
{
  if(ComboTopology->currentIndex() == BRIDGE_TYPE) {
    lineEdit_Zout->blockSignals( TRUE );
    lineEdit_Zout->setText( text );
    lineEdit_Zout->blockSignals( FALSE );
  }
}

void QucsAttenuator::slotSetText_Zout( const QString &text )
{
  if(ComboTopology->currentIndex() == BRIDGE_TYPE) {
    lineEdit_Zin->blockSignals( TRUE );
    lineEdit_Zin->setText( text );
    lineEdit_Zin->blockSignals( FALSE );
  }
}

void QucsAttenuator::slotTopologyChanged()
{
  switch(ComboTopology->currentIndex())
    {
    case PI_TYPE:
      pixTopology->setPixmap(QPixmap((QucsSettings.BitmapDir + "att_pi.png")));
      LabelR2->setText("R2:");
      LabelR3->show();
      lineEdit_R3->show();
      LabelR3_Ohm->show();
      break;
    case TEE_TYPE:
      pixTopology->setPixmap(QPixmap((QucsSettings.BitmapDir + "att_tee.png")));
      LabelR2->setText("R2:");
      LabelR3->show();
      lineEdit_R3->show();
      LabelR3_Ohm->show();
      break;
    case BRIDGE_TYPE:
      pixTopology->setPixmap(QPixmap((QucsSettings.BitmapDir + "att_bridge.png")));
      LabelR2->setText("R4:");
      LabelR3->hide();
      lineEdit_R3->hide();
      LabelR3_Ohm->hide();
      lineEdit_Zout->setText( lineEdit_Zin->text() );
      break;
    }
    adjustSize();
}

void QucsAttenuator::slotCalculate()
{
    QUCS_Att qatt;
    int result;
    QString * s = NULL;
    struct tagATT Values;


    Values.Topology = ComboTopology->currentIndex();
    Values.Attenuation = lineEdit_Attvalue->text().toDouble();
    Values.Zin = lineEdit_Zin->text().toDouble();
    Values.Zout = lineEdit_Zout->text().toDouble();
    result = qatt.Calc(&Values);

    if(result != -1)
    {
      LabelResult->setText(tr("Result:")+" "+tr("Success!"));
      lineEdit_R1->setText(QString::number(Values.R1, 'f', 1));
      lineEdit_R2->setText(QString::number(Values.R2, 'f', 1));
      lineEdit_R3->setText(QString::number(Values.R3, 'f', 1));
      
      s = qatt.createSchematic(&Values);
      if(!s) return;
      
      QClipboard *cb = QApplication::clipboard();
      cb->setText(*s);
      delete s;
    }
    else
    {
      LabelResult->setText(tr("Error: Set Attenuation more than %1 dB").arg(QString::number(Values.MinimumATT, 'f', 3)));
      lineEdit_R1->setText("--");
      lineEdit_R2->setText("--");
      lineEdit_R3->setText("--");
    }
    adjustSize();
}
