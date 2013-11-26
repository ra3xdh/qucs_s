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

#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QDom>

#include "qf_box.h"
#include "qf_settings.h"
//Added by qt3to4:
#include <Q3TextStream>

struct tQucsSettings QucsSettings;

// #########################################################################
// Loads the settings file and stores the settings.
bool loadSettings (void)
{
  bool result = true;

  QFile file(QDir::homeDirPath()+QDir::convertSeparators ("/.qucs/filterrc"));
  if(!file.open(QIODevice::ReadOnly))
    result = false; // settings file doesn't exist
  else {
    Q3TextStream stream(&file);
    QString Line, Setting;
    while(!stream.atEnd()) {
      Line = stream.readLine();
      if (Line.left(9) == "<!DOCTYPE") {
	file.close ();
	result = loadXmlSettings ();
	break;
      }
      Setting = Line.section('=',0,0);
      Line = Line.section('=',1,1);
      if(Setting == "FilterWindow") {
	QucsSettings.x = Line.section(",",0,0).toInt();
	QucsSettings.y = Line.section(",",1,1).toInt();
	break;
      }
    }
    file.close();
  }  

  file.setName(QDir::homeDirPath()+QDir::convertSeparators ("/.qucs/qucsrc"));
  if(!file.open(QIODevice::ReadOnly))
    result = true; // qucs settings not necessary
  else {
    Q3TextStream stream(&file);
    QString Line, Setting;
    while(!stream.atEnd()) {
      Line = stream.readLine();
      Setting = Line.section('=',0,0);
      Line = Line.section('=',1,1).stripWhiteSpace();
      if(Setting == "Font")
	QucsSettings.font.fromString(Line);
      else if(Setting == "Language")
	QucsSettings.Language = Line;
    }
    file.close();
  }
  return result;
}

// #########################################################################
// Saves the settings in the settings file.
bool saveSettings(qf_box *qucs)
{
  if(qucs->x() == QucsSettings.x)
    if(qucs->y() == QucsSettings.y)
      return true;   // nothing has changed


  QFile file(QDir::homeDirPath()+QDir::convertSeparators ("/.qucs/filterrc"));
  if(!file.open(QIODevice::WriteOnly)) {
    QMessageBox::warning(0, QObject::tr("Warning"),
			QObject::tr("Cannot save settings !"));
    return false;
  }

  QString Line;
  Q3TextStream stream(&file);

  stream << "Settings file, Qucs Filter " PACKAGE_VERSION "\n"
	 << "FilterWindow=" << qucs->x() << ',' << qucs->y() << '\n';

  file.close();
  return true;
}

// #########################################################################
// Saves the application settings in the XML settings file.
bool saveXmlSettings (qf_box * qucs)
{
  QDomDocument doc ("QucsFilter");
  QDomElement el, gr, rt;

  rt = doc.createElement ("Settings");
  doc.appendChild (rt);

  el = doc.createElement ("Version");
  el.setAttribute ("value", PACKAGE_VERSION);
  rt.appendChild (el);

  el = doc.createElement ("Window");
  el.setAttribute ("x", qucs->x());
  el.setAttribute ("y", qucs->y());
  rt.appendChild (el);

  gr = doc.createElement ("LC");
  el = doc.createElement ("Type");
  el.setAttribute ("Type", qucs->FilterName->currentItem ());
  el.setAttribute ("Class", qucs->TformName->currentItem ());
  el.setAttribute ("SpecifyOrder", qucs->OrderBox->isChecked ());
  el.setAttribute ("Order", qucs->OrderCombo->currentItem ());
  el.setAttribute ("SubOrder", qucs->SubOrderCombo->currentItem ());  
  gr.appendChild (el);
  el = doc.createElement ("Cutoff");
  el.setAttribute ("Value", qucs->EnterCutoff->text ());
  el.setAttribute ("Unit", qucs->CutoffCombo->currentItem ());
  gr.appendChild (el);
  el = doc.createElement ("Bandwidth");
  el.setAttribute ("Value", qucs->EnterBandwidth->text ());
  el.setAttribute ("Unit", qucs->BandwidthCombo->currentItem ());
  gr.appendChild (el);
  el = doc.createElement ("Stopband");
  el.setAttribute ("Value", qucs->EnterStopband->text ());
  el.setAttribute ("Unit", qucs->StopbandCombo->currentItem ());
  gr.appendChild (el);
  el = doc.createElement ("Ripple");
  el.setAttribute ("Value", qucs->EnterRipple->text ());
  gr.appendChild (el);
  el = doc.createElement ("Angle");
  el.setAttribute ("Value", qucs->EnterAngle->text ());
  gr.appendChild (el);
  el = doc.createElement ("Attenuation");
  el.setAttribute ("Value", qucs->EnterAttenuation->text ());
  gr.appendChild (el);
  el = doc.createElement ("Impedance");
  el.setAttribute ("Zin", qucs->EnterZin->text ());
  el.setAttribute ("Zout", qucs->EnterZout->text ());
  gr.appendChild (el);
  rt.appendChild (gr);

  QFile file (QDir::homeDirPath()+QDir::convertSeparators ("/.qucs/filterrc"));
  if (!file.open (QIODevice::WriteOnly)) {
    QMessageBox::warning (0,
			  QObject::tr("Warning"),
			  QObject::tr("Cannot save settings file !"));
    return false;
  }

  Q3TextStream str (&file);
  str << doc.toString ();
  file.close ();
  return true;
}

// #########################################################################
// Helper function to find XML nodes and attributes
static QString getXml (QDomDocument * doc, const QString type,
		       const QString attr, const QString group = "")
{
  QDomElement docElem = doc->documentElement ();

  QDomNode n = docElem.firstChild ();
  while (!n.isNull()) {
    QDomElement e = n.toElement ();
    if (!e.isNull()) {
      if (group.isEmpty ()) {
	if (e.tagName () == type) {
	  return e.attribute (attr, "");
	}
      }
      else {
	if (e.tagName () == group) {
	  QDomNode ng = e.firstChild ();
	  while (!ng.isNull()) {
	    QDomElement eg = ng.toElement ();
	    if (!eg.isNull()) {
	      if (eg.tagName () == type) {
		return eg.attribute (attr, "");
	      }
	    }
	    ng = ng.nextSibling();
	  }
	}
      }
    }
    n = n.nextSibling();
  }
  return "";
}

static int getInteger (QDomDocument * doc, const QString type,
		       const QString attr, const QString group = "")
{
  return getXml (doc, type, attr, group).toInt ();
}

static double getDouble (QDomDocument * doc, const QString type,
			 const QString attr, const QString group = "")
{
  return getXml (doc, type, attr, group).toDouble ();
}

static QString getString (QDomDocument * doc, const QString type,
			  const QString attr, const QString group = "")
{
  return getXml (doc, type, attr, group);
}

// #########################################################################
// Loads the XML settings file and stores the settings.
bool loadXmlSettings (void)
{
  bool result = true;

  QFile file (QDir::homeDirPath()+QDir::convertSeparators ("/.qucs/filterrc"));
  if (!file.open(QIODevice::ReadOnly))
    result = false; // settings file doesn't exist
  else {
    QDomDocument doc;
    QString errmsg;
    int line, col;
    if (!doc.setContent(&file, true, &errmsg, &line, &col)) {
		
      cerr << (const char*)file.fileName().toUtf8() << ":" << line << ":" << col
	   << ": " << (const char*)errmsg.toUtf8() << endl;
      cerr << (const char*)doc.toString ().toUtf8();
      file.close();
      return false;
    }

    QucsSettings.x = getInteger (&doc, "Window", "x");
    QucsSettings.y = getInteger (&doc, "Window", "y");

    QucsSettings.type = getInteger (&doc, "Type", "Type", "LC");
    QucsSettings.form = getInteger (&doc, "Type", "Class", "LC");
    QucsSettings.specify = getInteger (&doc, "Type", "SpecifyOrder", "LC");
    QucsSettings.ord = getInteger (&doc, "Type", "Order", "LC");
    QucsSettings.subord = getInteger (&doc, "Type", "SubOrder", "LC");

    QucsSettings.cutoff = getDouble (&doc, "Cutoff", "Value", "LC");
    QucsSettings.cutoff_unit = getInteger (&doc, "Cutoff", "Unit", "LC");
    QucsSettings.zin = getDouble (&doc, "Impedance", "Zin", "LC");
    QucsSettings.zout = getDouble (&doc, "Impedance", "Zout", "LC");
    QucsSettings.bw = getDouble (&doc, "Bandwidth", "Value", "LC");
    QucsSettings.bw_unit = getInteger (&doc, "Bandwidth", "Unit", "LC");
    QucsSettings.sb = getDouble (&doc, "Stopband", "Value", "LC");
    QucsSettings.sb_unit = getInteger (&doc, "Stopband", "Unit", "LC");
    QucsSettings.ripple = getDouble (&doc, "Ripple", "Value", "LC");
    QucsSettings.angle = getDouble (&doc, "Angle", "Value", "LC");
    QucsSettings.atten = getDouble (&doc, "Attenuation", "Value", "LC");
  }
  return result;
}
