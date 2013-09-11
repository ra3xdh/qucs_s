/***************************************************************************
                                qucsfilter.h
                             -------------------
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

#ifndef QUCSFILTER_H
#define QUCSFILTER_H

#include <QMainWindow>
#include <QLabel>

class QGridLayout;
class QComboBox;
class QLineEdit;
class QLabel;
class QIntValidator;
class QDoubleValidator;


struct tQucsSettings {
  int x, y;      // position of main window
  QFont font;
  QString LangDir;
  QString BitmapDir;
  QString Language;
};

extern struct tQucsSettings QucsSettings;

class QucsFilter : public QMainWindow
{
  Q_OBJECT
public:
  QucsFilter();
 ~QucsFilter();

private slots:
  void slotQuit();
  void slotHelpIntro();
  void slotHelpAbout();
  void slotHelpAboutQt();
  void slotCalculate();
  void slotTypeChanged(int);
  void slotClassChanged(int);
  void slotShowResult();

private:
  void setError(const QString&);
  QString * calculateFilter(struct tFilter *);

  int ResultState;

  QGridLayout *gbox;
  QComboBox *ComboType, *ComboClass, *ComboCorner, *ComboStop, *ComboBandStop;
  QLineEdit *EditOrder, *EditCorner, *EditStop, *EditRipple, *EditImpedance;
  QLineEdit *EditAtten, *EditBandStop;
  QLabel *LabelRipple, *LabelRipple_dB, *LabelStart, *LabelStop, *LabelResult;
  QLabel *LabelAtten, *LabelAtten_dB, *LabelBandStop, *LabelOrder;
  QIntValidator *IntVal;
  QDoubleValidator *DoubleVal;
};

#endif
