//-*- C++ -*-
/****************************************************************************
**     Qucs Attenuator Synthesis
**     qucsattenuator.h
**
**
**
**
**
**
**
*****************************************************************************/

#ifndef QUCSATTENUATOR_H
#define QUCSATTENUATOR_H

#include <QDialog>
#include <QLabel>
#include "attenuatorfunc.h"

class QComboBox;
class QLineEdit;
class QIntValidator;
class QDoubleValidator;
class QLabel;
class QPushButton;

struct tQucsSettings
{
  int x, y;      // position of main window
  QFont font;
  QString LangDir;
  QString BitmapDir;
  QString Language;
};

extern struct tQucsSettings QucsSettings;

class QucsAttenuator : public QDialog
{
 Q_OBJECT
 public:
  QucsAttenuator();
  ~QucsAttenuator();

 private slots:
  void slotHelpIntro();
  void slotHelpAbout();
  void slotHelpAboutQt();
  void slotTopologyChanged();
  void slotCalculate();
  void slotQuit();
  void slotSetText_Zin(const QString &);
  void slotSetText_Zout(const QString &);

 private:
  QComboBox *ComboTopology;
  QLabel *LabelTopology, *LabelAtten, *LabelImp1, *LabelImp2;
  QLabel *LabelR1, *LabelR2, *LabelR3, *pixTopology, *LabelResult;
  QLabel *LabelR3_Ohm;
  QLineEdit *lineEdit_Attvalue, *lineEdit_Zin, *lineEdit_Zout;
  QLineEdit *lineEdit_R1, *lineEdit_R2, *lineEdit_R3, *lineEdit_Results;
  QPushButton *Calculate;
  QIntValidator *IntVal;
  QDoubleValidator *DoubleVal;

};

#endif
