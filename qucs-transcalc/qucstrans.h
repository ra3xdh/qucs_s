/***************************************************************************
                          qucstrans.h  -  description
                             -------------------
    begin                : Sun Feb 27 2005
    copyright            : (C) 2005 by Stefan Jahn
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

#ifndef QUCSTRANS_H
#define QUCSTRANS_H

#include <QDialog>
#include <QFont>
#include <QString>
#include <QGridLayout>
#include <QTextStream>
#include <QVBoxLayout>
#include <QLabel>
#include <QCloseEvent>
#include <QMainWindow>
#include <QDir>

class QComboBox;
class QLineEdit;
class QLabel;
class QGroupBox;
class QVBoxLayout;
class QRadioButton;
class QGridLayout;
class QStatusBar;
class QTextStream;
class QButtonGroup;
class QStackedWidget;

class transline;

// Current limit defintions.
#define MAX_TRANS_BOXES   4
#define MAX_TRANS_TYPES   6
#define MAX_TRANS_RESULTS 7

// Types of transmission line properties.
#define TRANS_SUBSTRATE  0
#define TRANS_COMPONENT  1
#define TRANS_PHYSICAL   2
#define TRANS_ELECTRICAL 3

#define TRANS_FREQS   { "GHz", "Hz", "kHz", "MHz", NULL }
#define TRANS_OHMS    { "Ohm", "kOhm", NULL }
#define TRANS_ANGLES  { "Deg", "Rad", NULL }
#define TRANS_LENGTHS { "mil", "cm", "mm", "m", "um", "in", "ft", NULL }
#define TRANS_NONES   { "NA", NULL }

// Application settings.
struct tQucsSettings {
  int x, y, dx, dy;    // position and size of main window
  QFont font;          // font
  QString BitmapDir;   // pixmap directory
  QString LangDir;     // translation directory
  QString Language;
  int length_unit;     // default length unit
  int freq_unit;       // default frequency unit
  int res_unit;        // default resistance unit
  int ang_unit;        // default angle unit
  QString Mode;        // current mode
  QDir QucsWorkDir;
  QDir QucsHomeDir;
};

extern tQucsSettings QucsSettings;

// Transmission line value.
struct TransValue {
  const char * name;     // label
  double value;          // value
  QString * tip;         // tool tip description
  const char * units[8]; // unit choise
  int unit;              // unit index
  QLabel * label;        // Qt label widget
  QLineEdit * lineedit;  // Qt value widget
  QComboBox * combobox;  // Qt unit widget
  QRadioButton * radio;  // Qt fixed widget
};

// Array of transmission line values.
struct TransArray {
  struct TransValue item[12];
};

// Extraneous calculation results.
struct TransResult {
  QString * name; // textual description
  QLabel * label; // Qt name widget
  QLabel * value; // Qt value widget
};

// Transmission line types.
enum TransMode {
  ModeMicrostrip = 0,
  ModeCoplanar = 1,
  ModeGroundedCoplanar = 2,
  ModeRectangular = 3,
  ModeCoaxial = 4,
  ModeCoupledMicrostrip = 5,
  ModeNone
};

// A transmission line type structure.
struct TransType {
  int type;                 // type of transmission line (see TransMode)
  const char * description; // description
  const char * bitmap;      // bitmap file name
  transline * line;         // transmission line instance
  struct TransArray array[MAX_TRANS_BOXES];
  int results;              // number of extraneous results
  struct TransResult result[MAX_TRANS_RESULTS];
  int radio[4];
};

struct TransUnit {
  const char * description;
  const char * units[8];
};

/**
  *@author Stefan Jahn
  */

class QucsTranscalc : public QMainWindow  {
   Q_OBJECT
public:
  QucsTranscalc();
 ~QucsTranscalc();

  void    setProperty (QString, double);
  double  getProperty (QString);
  void    setUnit (QString, const char *);
  char *  getUnit (QString);
  void    setResult (int, const char *);
  bool    isSelected (QString);

  void    saveMode (QTextStream&);
  void    saveModes (QTextStream&);
  bool    loadFile (QString, int * _mode = 0);
  QString getMode (void);
  void    setMode (QString);
  static  int translateUnit(const char *, int);

private slots:
  void slotAbout();
  void slotQuit();
  void slotSelectType(int);
  void slotSynthesize();
  void slotAnalyze();
  void slotValueChanged();
  void slotFileLoad();
  void slotFileSave();
  void slotHelpIntro();
  void slotOptions();
  void slotRadioChecked(int);
  void slotCopyToClipBoard();

private:
  void updateSelection ();
  void createPropItem (QGridLayout *, TransValue *, int, QButtonGroup *);
  void createResultItem (QGridLayout *, TransResult *);
  void updateResultItem (TransResult *);
  void createResultItems (QGroupBox *);
  void updateResultItems ();
  void createPropItems (QGroupBox *, int);
  int getTypeIndex ();
  void updatePropItem (TransValue *);
  void setMode (int);
  struct TransValue * findProperty (QString);
  void setupTranslations ();
  bool saveFile (QString);
  void updateMode (void);
  void storeValues (void);
  void updatePixmap (int);

 private:
  void closeEvent (QCloseEvent*);

private:
  QStatusBar * statBar;
  QLabel * pix;
  QComboBox * tranType;
  QGroupBox * calculated;
  int mode;
};

#endif /* QUCSTRANS_H */
