#include "magcoredialog.h"

#include <QtWidgets>
#include <QSvgWidget>
#include <QSvgRenderer>

#include "component.h"
#include "schematic.h"
#include "main.h"

MagCoreDialog::MagCoreDialog(Component *pc, Schematic *sch) : QDialog(sch) {

  comp = pc;
  Sch = sch;

  QGroupBox *gpbParams = new QGroupBox;
  gpbParams->setTitle("Model parameters");
  QGridLayout *vl1 = new QGridLayout;

  vl1->addWidget(new QLabel(tr("Domains wall density, A (A/m)")),0,0,1,2);
  edtA = new QLineEdit;
  edtA->setText(comp->getProperty("A")->Value);
  vl1->addWidget(edtA,1,0,1,2);
  cbShowA = new QCheckBox(tr("Show"));
  cbShowA->setChecked(comp->getProperty("A")->display);
  vl1->addWidget(cbShowA,1,2,1,1);

  vl1->addWidget(new QLabel(tr("Domains pinning, K (A/m)")),2,0,1,2);
  edtK = new QLineEdit;
  edtK->setText(comp->getProperty("K")->Value);
  vl1->addWidget(edtK,3,0,1,2);
  cbShowK = new QCheckBox(tr("Show"));
  cbShowK->setChecked(comp->getProperty("K")->display);
  vl1->addWidget(cbShowK,3,2,1,1);

  vl1->addWidget(new QLabel(tr("Magnetization reversibility, C")),4,0,1,2);
  edtC = new QLineEdit;
  edtC->setText(comp->getProperty("C")->Value);
  vl1->addWidget(edtC,5,0,1,2);
  cbShowC = new QCheckBox(tr("Show"));
  cbShowC->setChecked(comp->getProperty("C")->display);
  vl1->addWidget(cbShowC,5,2,1,1);

  vl1->addWidget(new QLabel(tr("Saturation magnetization, MS (A/m)")),6,0,1,2);
  edtMs = new QLineEdit;
  edtMs->setText(comp->getProperty("Ms")->Value);
  vl1->addWidget(edtMs,7,0,1,2);
  cbShowMs = new QCheckBox(tr("Show"));
  cbShowMs->setChecked(comp->getProperty("Ms")->display);
  vl1->addWidget(cbShowMs,7,2,1,1);

  vl1->addWidget(new QLabel(tr("Interdomain coupling, α")),8,0,1,2);
  edtAlpha = new QLineEdit;
  edtAlpha->setText(comp->getProperty("alpha")->Value);
  vl1->addWidget(edtAlpha,9,0,1,2);
  cbShowAlpha = new QCheckBox(tr("Show"));
  cbShowAlpha->setChecked(comp->getProperty("alpha")->display);
  vl1->addWidget(cbShowAlpha,9,2,1,1);

  vl1->addWidget(new QLabel(tr("Magnetic path length (m)")),10,0,1,2);
  edtPath = new QLineEdit;
  edtPath->setText(comp->getProperty("PATH")->Value);
  vl1->addWidget(edtPath,11,0,1,2);
  cbShowPath = new QCheckBox(tr("Show"));
  cbShowPath->setChecked(comp->getProperty("PATH")->display);
  vl1->addWidget(cbShowPath,11,2,1,1);

  vl1->addWidget(new QLabel(tr("Core cross section area (m^2)")),12,0,1,2);
  edtArea = new QLineEdit;
  edtArea->setText(comp->getProperty("AREA")->Value);
  vl1->addWidget(edtArea,13,0,1,2);
  cbShowArea = new QCheckBox(tr("Show"));
  cbShowArea->setChecked(comp->getProperty("AREA")->display);
  vl1->addWidget(cbShowArea,13,2,1,1);

  vl1->addWidget(new QLabel(tr("Air gap length (m)")),14,0,1,2);
  edtGap = new QLineEdit;
  edtGap->setText(comp->getProperty("GAP")->Value);
  vl1->addWidget(edtGap,15,0,1,2);
  cbShowGap = new QCheckBox(tr("Show"));
  cbShowGap->setChecked(comp->getProperty("GAP")->display);
  vl1->addWidget(cbShowGap,15,2,1,1);

  cbHBProbes = new QCheckBox(tr("H and B probe pins"));
  if (comp->getProperty("BHprobes")->Value == "true"){
    cbHBProbes->setChecked(true);
  } else {
    cbHBProbes->setChecked(false);
  }
  vl1->addWidget(cbHBProbes,16,0,1,3);
  //vl1->addStretch();
  gpbParams->setLayout(vl1);

  cbxCoreType = new QComboBox;
  QStringList core_types;
  core_types<<"generic"<<"ring"<<"E-type"<<"U-type";
  cbxCoreType->addItems(core_types);
  cbxCoreType->setCurrentText(comp->getProperty("CoreType")->Value);
  connect(cbxCoreType,SIGNAL(currentIndexChanged(int)),this,SLOT(slotSetCoreImage()));

  QGroupBox *gpbCore = new QGroupBox;
  gpbCore->setTitle(tr("Core drawing"));
  QVBoxLayout *vl2 = new QVBoxLayout;
  vl2->addWidget(new QLabel(tr("Core configuration type:")));
  vl2->addWidget(cbxCoreType);
  coreImg = new QSvgWidget;
  vl2->addWidget(coreImg);
  coreImg->load(QString(":/bitmaps/svg/ringcore.svg"));
  coreImg->renderer()->setAspectRatioMode(Qt::KeepAspectRatio);
  coreImg->setMinimumSize(300,300);
  coreImg->setStyleSheet("background-color:white;");
  vl2->addStretch();
  gpbCore->setLayout(vl2);

  QGroupBox *gpbDims = new QGroupBox;
  gpbDims->setTitle(tr("Core dimensions"));
  QVBoxLayout *vl3 = new QVBoxLayout;
  lblD1 = new QLabel(tr("Ring diameter"));
  vl3->addWidget(lblD1);
  edtD1 = new QLineEdit;
  edtD1->setText(comp->getProperty("D1")->Value);
  connect(edtD1,SIGNAL(textChanged(QString)),this,SLOT(slotCalcPathArea()));
  vl3->addWidget(edtD1);

  lblD2 = new QLabel(tr("Ring diameter"));
  vl3->addWidget(lblD2);
  edtD2 = new QLineEdit;
  edtD2->setText(comp->getProperty("D2")->Value);
  connect(edtD2,SIGNAL(textChanged(QString)),this,SLOT(slotCalcPathArea()));
  vl3->addWidget(edtD2);

  lblD3 = new QLabel(tr("Ring diameter"));
  vl3->addWidget(lblD3);
  edtD3 = new QLineEdit;
  edtD3->setText(comp->getProperty("D3")->Value);
  connect(edtD3,SIGNAL(textChanged(QString)),this,SLOT(slotCalcPathArea()));
  vl3->addWidget(edtD3);

  lblD4 = new QLabel(tr("Ring diameter"));
  vl3->addWidget(lblD4);
  edtD4 = new QLineEdit;
  edtD4->setText(comp->getProperty("D4")->Value);
  connect(edtD4,SIGNAL(textChanged(QString)),this,SLOT(slotCalcPathArea()));
  vl3->addWidget(edtD4);

  lblD5 = new QLabel(tr("Ring diameter"));
  vl3->addWidget(lblD5);
  edtD5 = new QLineEdit;
  edtD5->setText(comp->getProperty("D5")->Value);
  connect(edtD5,SIGNAL(textChanged(QString)),this,SLOT(slotCalcPathArea()));
  vl3->addWidget(edtD5);

  lblD6 = new QLabel(tr("Ring diameter"));
  vl3->addWidget(lblD6);
  edtD6 = new QLineEdit;
  edtD6->setText(comp->getProperty("D6")->Value);
  connect(edtD6,SIGNAL(textChanged(QString)),this,SLOT(slotCalcPathArea()));
  vl3->addWidget(edtD6);
  gpbDims->setLayout(vl3);

  btnOK = new QPushButton(tr("OK"));
  connect(btnOK,SIGNAL(clicked(bool)),this,SLOT(slotOK()));
  btnCancel = new QPushButton(tr("Cancel"));
  connect(btnCancel,SIGNAL(clicked(bool)),this,SLOT(slotCancel()));
  btnApply = new QPushButton(tr("Apply"));
  connect(btnApply,SIGNAL(clicked(bool)),this,SLOT(slotApply()));

  QHBoxLayout *hl1 = new QHBoxLayout;
  hl1->addWidget(btnOK);
  hl1->addWidget(btnCancel);
  hl1->addWidget(btnApply);
  hl1->addStretch();

  sBar = new QStatusBar;
  QVBoxLayout *top = new QVBoxLayout;
  QHBoxLayout *top1 = new QHBoxLayout;
  top1->addWidget(gpbParams);
  top1->addWidget(gpbCore);
  top1->addWidget(gpbDims);
  top->addLayout(top1);
  top->addWidget(sBar);
  top->addLayout(hl1);

  setLayout(top);

  slotSetCoreImage();

}

void MagCoreDialog::setDimLabelsAtoF()
{
  lblD1->setText(tr("Dimension A (mm)"));
  lblD2->setText(tr("Dimension B (mm)"));
  lblD3->setText(tr("Dimension C (mm)"));
  lblD4->setText(tr("Dimension D (mm)"));
  lblD5->setText(tr("Dimension E (mm)"));
  lblD6->setText(tr("Dimension F (mm)"));
  lblD1->setEnabled(true);
  lblD2->setEnabled(true);
  lblD3->setEnabled(true);
  lblD4->setEnabled(true);
  lblD5->setEnabled(true);
  lblD6->setEnabled(true);
  edtD1->setEnabled(true);
  edtD2->setEnabled(true);
  edtD3->setEnabled(true);
  edtD4->setEnabled(true);
  edtD5->setEnabled(true);
  edtD6->setEnabled(true);
}

void MagCoreDialog::slotSetCoreImage()
{
  resetDimLabels();
  switch (cbxCoreType->currentIndex()) {
  case 0:
    coreImg->load(QString(":/bitmaps/svg/nocore.svg"));
    sBar->showMessage("Core type not defined; Enter magnetic path length and core area manually");
    break;
  case 1:
    coreImg->load(QString(":/bitmaps/svg/ringcore.svg"));
    lblD1->setText(tr("Ring outer diameter D (mm)"));
    lblD2->setText(tr("Ring inner diameter d (mm)"));
    lblD3->setText(tr("Ring height H (mm)"));
    edtD1->setEnabled(true);
    edtD2->setEnabled(true);
    edtD3->setEnabled(true);
    lblD1->setEnabled(true);
    lblD2->setEnabled(true);
    lblD3->setEnabled(true);
    sBar->clearMessage();
    break;
  case 2:
    coreImg->load(QString(":/bitmaps/svg/ecore.svg"));
    sBar->clearMessage();
    setDimLabelsAtoF();
    break;
  case 3:
    setDimLabelsAtoF();
    lblD6->setEnabled(false);
    edtD6->setEnabled(false);
    coreImg->load(QString(":/bitmaps/svg/ucore.svg"));
    sBar->clearMessage();
    break;
  default:
    coreImg->load(QString(":/bitmaps/svg/nocore.svg"));
    break;
  }
}

void MagCoreDialog::resetDimLabels()
{
  lblD1->setText(tr("Dimension 1"));
  lblD2->setText(tr("Dimension 2"));
  lblD3->setText(tr("Dimension 3"));
  lblD4->setText(tr("Dimension 4"));
  lblD5->setText(tr("Dimension 5"));
  lblD6->setText(tr("Dimension 6"));
  lblD1->setEnabled(false);
  lblD2->setEnabled(false);
  lblD3->setEnabled(false);
  lblD4->setEnabled(false);
  lblD5->setEnabled(false);
  lblD6->setEnabled(false);
  edtD1->setEnabled(false);
  edtD2->setEnabled(false);
  edtD3->setEnabled(false);
  edtD4->setEnabled(false);
  edtD5->setEnabled(false);
  edtD6->setEnabled(false);
}

void MagCoreDialog::slotCalcPathArea()
{
  bool r = true;
  switch (cbxCoreType->currentIndex()) {
  case 0: break;
  case 1:  r = calcRingCore();
    break;
  case 2:  r = calcECore();
    break;
  case 3: r = calcUCore();
    break;
  default: break;
  }

  if (!r) {
    sBar->showMessage(tr("Core dimension calculation error! Check input data"));
  } else {
    sBar->clearMessage();
  }
}

void MagCoreDialog::slotApply()
{
  comp->getProperty("A")->Value = edtA->text().trimmed();
  comp->getProperty("C")->Value = edtC->text().trimmed();
  comp->getProperty("K")->Value = edtK->text().trimmed();
  comp->getProperty("Ms")->Value = edtMs->text().trimmed();
  comp->getProperty("alpha")->Value = edtAlpha->text().trimmed();
  comp->getProperty("PATH")->Value = edtPath->text().trimmed();
  comp->getProperty("AREA")->Value = edtArea->text().trimmed();
  comp->getProperty("GAP")->Value = edtGap->text().trimmed();
  comp->getProperty("CoreType")->Value = cbxCoreType->currentText();
  comp->getProperty("D1")->Value = edtD1->text().trimmed();
  comp->getProperty("D2")->Value = edtD2->text().trimmed();
  comp->getProperty("D3")->Value = edtD3->text().trimmed();
  comp->getProperty("D4")->Value = edtD4->text().trimmed();
  comp->getProperty("D5")->Value = edtD5->text().trimmed();
  comp->getProperty("D6")->Value = edtD6->text().trimmed();

  comp->getProperty("A")->display = cbShowA->isChecked();
  comp->getProperty("C")->display = cbShowC->isChecked();
  comp->getProperty("K")->display = cbShowK->isChecked();
  comp->getProperty("Ms")->display = cbShowMs->isChecked();
  comp->getProperty("alpha")->display = cbShowAlpha->isChecked();
  comp->getProperty("PATH")->display = cbShowPath->isChecked();
  comp->getProperty("AREA")->display = cbShowArea->isChecked();
  comp->getProperty("GAP")->display = cbShowGap->isChecked();

  if (cbHBProbes->isChecked()) {
    comp->getProperty("BHprobes")->Value = "true";
  } else {
    comp->getProperty("BHprobes")->Value = "false";
  }

  Sch->recreateComponent(comp);
  Sch->viewport()->repaint();
  Sch->setChanged(true,true);
}

void MagCoreDialog::slotOK()
{
  slotApply();
  accept();
}

void MagCoreDialog::slotCancel()
{
  reject();
}

bool MagCoreDialog::calcRingCore()
{
  double D = edtD1->text().toDouble();
  double d = edtD2->text().toDouble();
  double H = edtD3->text().toDouble();

  if (!(std::isnormal(D)&&std::isnormal(d)&&std::isnormal(H))) {
    return false;
  }

  D *=1e-3; d *= 1e-3; H *= 1e-3;

  double lp = M_PI*(D+d)/2;
  double A = H*(D-d)/2;

  if (lp < 0 || !std::isnormal(lp)) {
    return false;
  }
  if (A < 0 || !std::isnormal(A)) {
    return false;
  }

  edtPath->setText(QString::number(lp));
  edtArea->setText(QString::number(A));

  return true;
}

bool MagCoreDialog::calcECore()
{
  double A = edtD1->text().toDouble();
  double B = edtD2->text().toDouble();
  double C = edtD3->text().toDouble();
  double D = edtD4->text().toDouble();
  double E = edtD5->text().toDouble();
  double F = edtD6->text().toDouble();
  if (!(std::isnormal(A)&&std::isnormal(B)&&std::isnormal(C)&&
        std::isnormal(D)&&std::isnormal(E)&&std::isnormal(F))) {
    return false;
  }
  A *= 1e-3; B *= 1e-3; C *= 1e-3;
  D *= 1e-3; E *= 1e-3; F *= 1e-3;

  double h = B-D;
  double q = C;
  double s = F/2;
  double p = (A-E)/2;

  double l[5];

  l[0] = 2*D;
  l[2] = l[0];
  l[1] = (E-F);
  l[3] = (M_PI/4)*(p+h);
  l[4] = (M_PI/4)*(s+h);

  double Ar[5];
  Ar[0] = 2*q*p;
  Ar[1] = 2*q*h;
  Ar[2] = 2*s*q;
  Ar[3] = 0.5*(Ar[0]+Ar[1]);
  Ar[4] = 0.5*(Ar[2]+Ar[3]);

  double C1 = 0;
  double C2 = 0;
  for (size_t i = 0; i < 5; i++) {
    C1 += l[i]/Ar[i];
    C2 += l[i]/(Ar[i]*Ar[i]);
  }

  double lp = C1*C1/C2;
  double Area = C1/C2;

  if (lp < 0 || !std::isnormal(lp)) {
    return false;
  }
  if (Area < 0 || !std::isnormal(Area)) {
    return false;
  }

  edtPath->setText(QString::number(lp));
  edtArea->setText(QString::number(Area));

  return true;
}

bool MagCoreDialog::calcUCore()
{
  double A = edtD1->text().toDouble();
  double B = edtD2->text().toDouble();
  double C = edtD3->text().toDouble();
  double D = edtD4->text().toDouble();
  double E = edtD5->text().toDouble();
  double F = 0.5*(A-E);
  if (!(std::isnormal(A)&&std::isnormal(B)&&std::isnormal(C)&&
        std::isnormal(D)&&std::isnormal(E)&&std::isnormal(F))) {
    return false;
  }
  A *= 1e-3; B *= 1e-3; C *= 1e-3;
  D *= 1e-3; E *= 1e-3; F *= 1e-3;

  double h = B-D;
  double q = C;
  double y = F;
  double p = (A-E)/2;

  double l[5];
  l[0] = 2*D;
  l[2] = l[0];
  l[1] = 2*E;
  l[3] = (M_PI/4)*(p+h);
  l[4] = (M_PI/4)*(y+h);

  double Ar[5];
  Ar[0] = q*p;
  Ar[1] = q*h;
  Ar[2] = y*q;
  Ar[3] = 0.5*(Ar[0]+Ar[1]);
  Ar[4] = 0.5*(Ar[1]+Ar[2]);

  double C1 = 0;
  double C2 = 0;
  for (size_t i = 0; i < 5; i++) {
    C1 += l[i]/Ar[i];
    C2 += l[i]/(Ar[i]*Ar[i]);
  }

  double lp = C1*C1/C2;
  double Area = C1/C2;

  if (lp < 0 || !std::isnormal(lp)) {
    return false;
  }
  if (Area < 0 || !std::isnormal(Area)) {
    return false;
  }

  edtPath->setText(QString::number(lp));
  edtArea->setText(QString::number(Area));

  return true;
}
