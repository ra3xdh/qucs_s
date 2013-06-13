/***************************************************************************
                              spicefile.cpp
                             ---------------
    begin                : Tue Dez 28 2004
    copyright            : (C) 2004 by Michael Margraf
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

#if HAVE_CONFIG_H
# include <config.h>
#endif

#if HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <QtGui>
#include <qregexp.h>
#include <q3process.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qmessagebox.h>
#include <q3textstream.h>
#include <qfile.h>
#include <qdir.h>
#include <qfileinfo.h>

#include "spicefile.h"
#include "schematic.h"
#include "main.h"
#include "qucs.h"


SpiceFile::SpiceFile()
{
  Description = QObject::tr("SPICE netlist file");
  // Property descriptions not needed, but must not be empty !
  Props.append(new Property("File", "", true, QString("x")));
  Props.append(new Property("Ports", "", false, QString("x")));
  Props.append(new Property("Sim", "yes", false, QString("x")));
  Props.append(new Property("Preprocessor", "none", false, QString("x")));
  withSim = false;

  Model = "SPICE";
  Name  = "X";
  changed = false;

  // Do NOT call createSymbol() here. But create port to let it rotate.
  Ports.append(new Port(0, 0));
}

// -------------------------------------------------------
Component* SpiceFile::newOne()
{
  SpiceFile *p = new SpiceFile();
  p->recreate(0);   // createSymbol() is NOT called in constructor !!!
  return p;
}

// -------------------------------------------------------
Element* SpiceFile::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("SPICE netlist");
  BitmapFile = (char *) "spicefile";

  if(getNewOne) {
    SpiceFile *p = new SpiceFile();
    p->recreate(0);   // createSymbol() is NOT called in constructor !!!
    return p;
  }
  return 0;
}

// -------------------------------------------------------
void SpiceFile::createSymbol()
{
  QFontMetrics  metrics(QucsSettings.font);   // get size of text
  int fHeight = metrics.lineSpacing();

  int No = 0;
  QString tmp, PortNames = Props.at(1)->Value;
  if(!PortNames.isEmpty())  No = PortNames.count(',') + 1;

  #define HALFWIDTH  17
  int h = 30*((No-1)/2) + 15;
  Lines.append(new Line(-HALFWIDTH, -h, HALFWIDTH, -h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( HALFWIDTH, -h, HALFWIDTH,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-HALFWIDTH,  h, HALFWIDTH,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-HALFWIDTH, -h,-HALFWIDTH,  h,QPen(Qt::darkBlue,2)));

  int w, i = fHeight/2;
  if(withSim) {
    i = fHeight - 2;
    tmp = QObject::tr("sim");
    w = metrics.width(tmp);
    Texts.append(new Text(w/-2, 0, tmp, Qt::red));
  }
  tmp = QObject::tr("spice");
  w = metrics.width(tmp);
  Texts.append(new Text(w/-2, -i, tmp));


  i = 0;
  int y = 15-h;
  while(i<No) {
    Lines.append(new Line(-30,  y,-HALFWIDTH,  y,QPen(Qt::darkBlue,2)));
    Ports.append(new Port(-30,  y));
    tmp = PortNames.section(',', i, i).mid(4);
    w = metrics.width(tmp);
    Texts.append(new Text(-20-w, y-fHeight-2, tmp));
    i++;

    if(i == No) break;
    Lines.append(new Line(HALFWIDTH,  y, 30,  y,QPen(Qt::darkBlue,2)));
    Ports.append(new Port( 30,  y));
    tmp = PortNames.section(',', i, i).mid(4);
    Texts.append(new Text( 20, y-fHeight-2, tmp));
    y += 60;
    i++;
  }

  if(No > 0) {
    Lines.append(new Line( 0, h, 0,h+15,QPen(Qt::darkBlue,2)));
    Texts.append(new Text( 4, h,"Ref"));
    Ports.append(new Port( 0, h+15));    // 'Ref' port
  }

  x1 = -30; y1 = -h-2;
  x2 =  30; y2 =  h+15;

  tx = x1+4;
  ty = y1 - fHeight - 4;
  if(Props.first()->display) ty -= fHeight;
  changed = true;
}

// ---------------------------------------------------
QString SpiceFile::netlist()
{
  if(Props.at(1)->Value.isEmpty())
    return QString("");  // no ports, no subcircuit instance

  QString s = "Sub:"+Name;   // SPICE netlist is subcircuit
  for(Port *pp = Ports.first(); pp != 0; pp = Ports.next())
    s += " "+pp->Connection->Name;   // output all node names

  QString f = properFileName(Props.first()->Value);
  s += " Type=\""+properName(f)+"\"\n";
  return s;
}

// -------------------------------------------------------
QString SpiceFile::getSubcircuitFile()
{
  // construct full filename
  QString FileName = Props.getFirst()->Value;
  return properAbsFileName(FileName);
}

// -------------------------------------------------------------------------
bool SpiceFile::createSubNetlist(QTextStream *stream)
{
  // check file name
  QString FileName = Props.first()->Value;
  if(FileName.isEmpty()) {
    ErrText += QObject::tr("ERROR: No file name in SPICE component \"%1\".").
      arg(Name);
    return false;
  }

  // check input and output file
  QFile SpiceFile, ConvFile;
  FileName = getSubcircuitFile();
  SpiceFile.setName(FileName);
  if(!SpiceFile.open(QIODevice::ReadOnly)) {
    ErrText += QObject::tr("ERROR: Cannot open SPICE file \"%1\".").
      arg(FileName);
    return false;
  }
  SpiceFile.close();
  QString ConvName = SpiceFile.name() + ".lst";
  ConvFile.setName(ConvName);
  QFileInfo Info(ConvName);

  // re-create converted file if necessary
  if(changed || !ConvFile.exists() ||
     (lastLoaded.isValid() && lastLoaded < Info.lastModified())) {
    if(!ConvFile.open(QIODevice::WriteOnly)) {
      ErrText +=
	QObject::tr("ERROR: Cannot save converted SPICE file \"%1\".").
	arg(FileName + ".lst");
      return false;
    }
    outstream = stream;
    filstream = new QTextStream(&ConvFile);
    QString SpiceName = SpiceFile.name();
    bool ret = recreateSubNetlist(&SpiceName, &FileName);
    ConvFile.close();
    delete filstream;
    return ret;
  }

  // load old file and stuff into stream
  if(!ConvFile.open(QIODevice::ReadOnly)) {
    ErrText +=
      QObject::tr("ERROR: Cannot open converted SPICE file \"%1\".").
      arg(FileName + ".lst");
    return false;
  }
  QByteArray FileContent = ConvFile.readAll();
  ConvFile.close();
  //? stream->writeRawBytes(FileContent.data(), FileContent.size());
  (*stream) << FileContent.data();
  return true;
}

// -------------------------------------------------------------------------
bool SpiceFile::recreateSubNetlist(QString *SpiceFile, QString *FileName)
{
  // initialize collectors
  ErrText = "";
  NetText = "";
  SimText = "";
  NetLine = "";

  // evaluate properties
  if(Props.at(1)->Value != "")
    makeSubcircuit = true;
  else
    makeSubcircuit = false;
  if(Props.at(2)->Value == "yes")
    insertSim = true;
  else
    insertSim = false;

  // preprocessor run if necessary
  QString preprocessor = Props.at(3)->Value;
  if (preprocessor != "none") {
    bool piping = true;
    QString script;
#ifdef __MINGW32__
    QString interpreter = "tinyperl.exe";
#else
    QString interpreter = "perl";
#endif
    if (preprocessor == "ps2sp") {
      script = "ps2sp";
    } else if (preprocessor == "spicepp") {
      script = "spicepp.pl";
    } else if (preprocessor == "spiceprm") {
      script = "spiceprm";
      piping = false;
    }
    script = QucsSettings.BinDir + script;
    SpicePrep = new Q3Process(this);
    SpicePrep->addArgument(interpreter);
    SpicePrep->addArgument(script);
    SpicePrep->addArgument(*SpiceFile);

    QFile PrepFile;
    QString PrepName = *SpiceFile + ".pre";

    if (!piping) {
      SpicePrep->addArgument(PrepName);
      connect(SpicePrep, SIGNAL(readyReadStdout()), SLOT(slotSkipOut()));
      connect(SpicePrep, SIGNAL(readyReadStderr()), SLOT(slotGetPrepErr()));
    } else {
      connect(SpicePrep, SIGNAL(readyReadStdout()), SLOT(slotGetPrepOut()));
      connect(SpicePrep, SIGNAL(readyReadStderr()), SLOT(slotGetPrepErr()));
    }

    QMessageBox *MBox = new QMessageBox(QObject::tr("Info"),
	       QObject::tr("Preprocessing SPICE file \"%1\".").arg(*SpiceFile),
               QMessageBox::NoIcon, QMessageBox::Abort,
               QMessageBox::NoButton, QMessageBox::NoButton, 0, 0, true,
	       Qt::WStyle_DialogBorder |  Qt::WDestructiveClose);
    connect(SpicePrep, SIGNAL(processExited()), MBox, SLOT(close()));

    if (piping) {
      PrepFile.setName(PrepName);
      if(!PrepFile.open(QIODevice::WriteOnly)) {
	ErrText +=
	  QObject::tr("ERROR: Cannot save preprocessed SPICE file \"%1\".").
	  arg(PrepName);
	return false;
      }
      prestream = new QTextStream(&PrepFile);
    }

    if(!SpicePrep->start()) {
      ErrText += QObject::tr("ERROR: Cannot execute \"%1\".").
	arg(interpreter + " " + script + "\".");
      if (piping) {
	PrepFile.close();
	delete prestream;
      }
      return false;
    }
    SpicePrep->closeStdin();

    MBox->exec();
    delete SpicePrep;
    if (piping) {
      PrepFile.close();
      delete prestream;
    }
    *SpiceFile = PrepName;
  }

  // begin command line construction
  QStringList com;
  com << (QucsSettings.BinDir + "qucsconv");
  if(makeSubcircuit) com << "-g" << "_ref";
  com << "-if" << "spice" << "-of" << "qucs";
  com << "-i" << *SpiceFile;

  // begin netlist text creation
  if(makeSubcircuit) {
    QString f = properFileName(*FileName);
    NetText += "\n.Def:" + properName(f) + " ";
    QString PortNames = Props.at(1)->Value;
    PortNames.replace(',', ' ');
    NetText += PortNames;
    if(makeSubcircuit) NetText += " _ref";
  }
  NetText += "\n";

  // startup SPICE conversion process
  QucsConv = new Q3Process(this);
  QucsConv->setArguments(com);
  connect(QucsConv, SIGNAL(readyReadStdout()), SLOT(slotGetNetlist()));
  connect(QucsConv, SIGNAL(readyReadStderr()), SLOT(slotGetError()));
  connect(QucsConv, SIGNAL(processExited()), SLOT(slotExited()));
  if(!QucsConv->start()) {
    ErrText += QObject::tr("ERROR: Cannot start QucsConv!");
    return false;
  }
  (*outstream) << NetText;
  (*filstream) << NetText;
  QucsConv->closeStdin();

  // waiting info dialog box
  QMessageBox *MBox = new QMessageBox(QObject::tr("Info"),
	       QObject::tr("Converting SPICE file \"%1\".").arg(*SpiceFile),
               QMessageBox::NoIcon, QMessageBox::Abort,
               QMessageBox::NoButton, QMessageBox::NoButton, 0, 0, true,
	       Qt::WStyle_DialogBorder | Qt::WDestructiveClose);
  connect(QucsConv, SIGNAL(processExited()), MBox, SLOT(close()));
  MBox->exec();

  // finish
  delete QucsConv;
  lastLoaded = QDateTime::currentDateTime();
  return true;
}

// -------------------------------------------------------------------------
void SpiceFile::slotSkipErr()
{
  SpicePrep->readStderr();
}

// -------------------------------------------------------------------------
void SpiceFile::slotSkipOut()
{
  SpicePrep->readStdout();
}

// -------------------------------------------------------------------------
void SpiceFile::slotGetPrepErr()
{
  ErrText += QString(SpicePrep->readStderr());
}

// -------------------------------------------------------------------------
void SpiceFile::slotGetPrepOut()
{
  (*prestream) << QString(SpicePrep->readStdout());
}

// -------------------------------------------------------------------------
void SpiceFile::slotGetError()
{
  ErrText += QString(QucsConv->readStderr());
}

// -------------------------------------------------------------------------
void SpiceFile::slotGetNetlist()
{
  int i;
  QString s;
  NetLine += QString(QucsConv->readStdout());

  while((i = NetLine.find('\n')) >= 0) {
    s = NetLine.left(i);
    NetLine.remove(0, i+1);
    s = s.stripWhiteSpace();
    if(s.size()>0&&s.at(0) == '#') {
      continue;
    } else if(s.isEmpty()) {
      continue;
    } else if(s.size()>0&&s.at(0) == '.') {
      if(s.left(5) != ".Def:") {
	if(insertSim) SimText += s + "\n";
	continue;
      }
    }
    if(makeSubcircuit) {
      (*outstream) << "  ";
      (*filstream) << "  ";
    }
    (*outstream) << s << "\n";
    (*filstream) << s << "\n";
  }
}

// -------------------------------------------------------------------------
void SpiceFile::slotExited()
{
  if (!QucsConv->normalExit()) {
    NetText = "";
  }
  else {
    if(makeSubcircuit) {
      (*outstream) << ".Def:End\n\n";
      (*filstream) << ".Def:End\n\n";
    }
    if(!SimText.isEmpty()) {
      (*outstream) << SimText;
      (*filstream) << SimText;
    }
  }
}
