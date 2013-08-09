/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

struct tQucsSettings {
  int x, y;         // position of main window
  double cutoff;    // cutoff frequency
  int cutoff_unit;  // cutoff frequency unit
  double zin, zout; // in/output impedances
  int type;
  int form;
  int ord;
  int specify;
  int subord;
  double bw;
  int bw_unit;
  double sb;
  int sb_unit;
  double ripple;
  double angle;
  double atten;
  QFont font;
  QString LangDir;
  QString Language;
};

extern struct tQucsSettings QucsSettings;

bool loadSettings (void);
bool saveSettings (qf_box *);
bool loadXmlSettings (void);
bool saveXmlSettings (qf_box *);

