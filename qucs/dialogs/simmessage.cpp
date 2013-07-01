/***************************************************************************
                              simmessage.cpp
                             ----------------
    begin                : Sat Sep 6 2003
    copyright            : (C) 2003 by Michael Margraf
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
#include <QtGui>
#include <stdlib.h>
#include <iostream>
using namespace std;
#include <QLabel>
#include <QGroupBox>
#include <QTimer>
#include <QPushButton>
#include <QTextEdit>
#include <QDateTime>
#include <QRegExp>
#include <QTextStream>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProcess>

#include "simmessage.h"
#include "main.h"
#include "qucs.h"
#include "textdoc.h"
#include "schematic.h"
#include "components/opt_sim.h"
#include "components/vhdlfile.h"

#ifdef __MINGW32__
#define executablePostfix ".exe"
#else
#define executablePostfix ""
#endif


SimMessage::SimMessage(QWidget *w, QWidget *parent)
		: QDialog(parent) //, 0, FALSE, Qt::WDestructiveClose)
{
  setWindowTitle(tr("Qucs Simulation Messages"));
  QucsDoc *Doc;
  DocWidget = w;
  if(DocWidget->inherits("QTextEdit"))
    Doc = (QucsDoc*) ((TextDoc*)DocWidget);
  else
    Doc = (QucsDoc*) ((Schematic*)DocWidget);

  DocName = Doc->DocName;
  DataDisplay = Doc->DataDisplay;
  Script = Doc->Script;
  QFileInfo Info(DocName);
  DataSet = QDir::convertSeparators(Info.dirPath()) +
    QDir::separator() + Doc->DataSet;
  showBias = Doc->showBias;     // save some settings as the document...
  SimOpenDpl = Doc->SimOpenDpl; // ...could be closed during the simulation.
  SimRunScript = Doc->SimRunScript;

  all = new QVBoxLayout(this);
  all->setSpacing(5);
  all->setMargin(5);
  QGroupBox *Group1 = new QGroupBox(tr("Progress:"));
  all->addWidget(Group1);
  QVBoxLayout *vbox1 = new QVBoxLayout();
  Group1->setLayout(vbox1);

  ProgText = new QTextEdit();
  vbox1->addWidget(ProgText);
  ProgText->setTextFormat(Qt::PlainText);
  ProgText->setReadOnly(true);
  //ProgText->setWordWrapMode(QTextOption::NoWrap);
  ProgText->setMinimumSize(400,80);
  wasLF = false;

  QGroupBox *HGroup = new QGroupBox();
  QHBoxLayout *hbox = new QHBoxLayout();
  //HGroup->setInsideMargin(5);
  //HGroup->setInsideSpacing(5);
  all->addWidget(HGroup);
  QLabel *progr = new QLabel(tr("Progress:"));
  hbox->addWidget(progr);
  SimProgress = new QProgressBar();
  hbox->addWidget(SimProgress);
//  SimProgress->setPercentageVisible(false);
  HGroup->setLayout(hbox);

  QGroupBox *Group2 = new QGroupBox(tr("Errors and Warnings:"));
  all->addWidget(Group2);
  QVBoxLayout *vbox2 = new QVBoxLayout();

  ErrText = new QTextEdit();
  vbox2->addWidget(ErrText);
  ErrText->setTextFormat(Qt::PlainText);
  ErrText->setReadOnly(true);
  ErrText->setWordWrapMode(QTextOption::NoWrap);
  ErrText->setMinimumSize(400,80);
  Group2->setLayout(vbox2);

  QHBoxLayout *Butts = new QHBoxLayout();
  all->addLayout(Butts);

  Display = new QPushButton(tr("Goto display page"));
  Butts->addWidget(Display);
  Display->setDisabled(true);
  connect(Display,SIGNAL(clicked()),SLOT(slotDisplayButton()));

  Abort = new QPushButton(tr("Abort simulation"));
  Butts->addWidget(Abort);
  connect(Abort,SIGNAL(clicked()),SLOT(reject()));
}

SimMessage::~SimMessage()
{
  if(SimProcess.state()==QProcess::Running)  SimProcess.kill();
  delete all;
}

// ------------------------------------------------------------------------
bool SimMessage::startProcess()
{
  Abort->setText(tr("Abort simulation"));
  Display->setDisabled(true);

  QString txt = tr("Starting new simulation on %1 at %2").
    arg(QDate::currentDate().toString("ddd dd. MMM yyyy")).
    arg(QTime::currentTime().toString("hh:mm:ss"));
  ProgText->insert(txt + "\n\n");
  
  SimProcess.blockSignals(false);
 /* On Qt4 it shows as running even before we .start it. FIXME*/
  if(SimProcess.state()==QProcess::Running ||SimProcess.state()==QProcess::Starting) {
    qDebug() << "running!";  
    ErrText->insert(tr("ERROR: Simulator is still running!"));
    FinishSimulation(-1);
    return false;
  }

  Collect.clear();  // clear list for NodeSets, SPICE components etc.
  ProgText->insert(tr("creating netlist... "));
  NetlistFile.setName(QucsSettings.QucsHomeDir.filePath("netlist.txt"));
   if(!NetlistFile.open(QIODevice::WriteOnly)) {
    ErrText->insert(tr("ERROR: Cannot write netlist file!"));
    FinishSimulation(-1);
    return false;
  }

  Stream.setDevice(&NetlistFile);

  if(!DocWidget->inherits("QTextEdit")) {
    SimPorts =
       ((Schematic*)DocWidget)->prepareNetlist(Stream, Collect, ErrText);
    if(SimPorts < -5) {
      NetlistFile.close();
      ErrText->insert(tr("ERROR: Cannot simulate a text file!"));
      FinishSimulation(-1);
      return false;
    }
  }
  Collect.append("*");   // mark the end


  disconnect(&SimProcess, 0, 0, 0);
  connect(&SimProcess, SIGNAL(readyReadStandardError()), SLOT(slotDisplayErr()));
  connect(&SimProcess, SIGNAL(readyReadStandardOutput()),
                       SLOT(slotReadSpiceNetlist()));
  connect(&SimProcess, SIGNAL(finished(int)),
                       SLOT(slotFinishSpiceNetlist(int)));
 
  nextSPICE();
  return true;
  // Since now, the Doc pointer may be obsolete, as the user could have
  // closed the schematic !!!
}
  
// ---------------------------------------------------
// Converts a spice netlist into Qucs format and outputs it.
void SimMessage::nextSPICE()
{ 
  QString Line;
  for(;;) {  // search for next SPICE component
    Line = *(Collect.begin());
    Collect.remove(Collect.begin());
    if(Line == "*") {  // worked on all components ?
      startSimulator(); // <<<<<================== go on ===
      return;
    }
#warning SPICE section below not being covered?
    qDebug() << "goin thru SPICE branch on simmmessage.cpp";
    if(Line.left(5) == "SPICE") {
      if(Line.at(5) != 'o') insertSim = true;
      else insertSim = false;
      break;
    }
    Collect.append(Line);
  }


  QString FileName = Line.section('"', 1,1);
  Line = Line.section('"', 2);  // port nodes
  if(Line.isEmpty())  makeSubcircuit = false;
  else  makeSubcircuit = true;

  QString prog;
  QStringList com;
  prog = QucsSettings.BinDir + "qucsconv";
  if(makeSubcircuit)
    com << "-g" << "_ref";
  com << "-if" << "spice" << "-of" << "qucs";

  QFile SpiceFile;
  if(FileName.find(QDir::separator()) < 0)  // add path ?
    SpiceFile.setName(QucsSettings.QucsWorkDir.path() + QDir::separator() + FileName);
  else
    SpiceFile.setName(FileName);
  if(!SpiceFile.open(QIODevice::ReadOnly)) {
    ErrText->insert(tr("ERROR: Cannot open SPICE file \"%1\".").arg(FileName));
    FinishSimulation(-1);
    return;
  }

  if(makeSubcircuit) {
    Stream << "\n.Def:" << properName(FileName) << " ";
  
    Line.replace(',', ' ');
    Stream << Line;
    if(!Line.isEmpty()) Stream << " _ref";
  }
  Stream << "\n";

  ProgressText = "";
  
  qDebug() << "start QucsConv" << prog << com.join(" ");
  SimProcess.start(prog, com);
  
  if(!SimProcess.Running) {
    ErrText->insert(tr("ERROR: Cannot start QucsConv!"));
    FinishSimulation(-1);
    return;
  }

  QByteArray SpiceContent = SpiceFile.readAll();
  SpiceFile.close();
  QString command(SpiceContent); //to convert byte array to string
  SimProcess.setStandardInputFile(command);  //? FIXME works?
  qDebug() << command;
  connect(&SimProcess, SIGNAL(wroteToStdin()), SLOT(slotCloseStdin()));
}

// ------------------------------------------------------------------------
void SimMessage::slotCloseStdin()
{
  //SimProcess.closeStdin(); //? channel not available in Qt4?
  disconnect(&SimProcess, SIGNAL(wroteToStdin()), 0, 0);
}

// ------------------------------------------------------------------------
void SimMessage::slotReadSpiceNetlist()
{
  int i;
  QString s;
  ProgressText += QString(SimProcess.readAllStandardOutput());

  while((i = ProgressText.find('\n')) >= 0) {

    s = ProgressText.left(i);
    ProgressText.remove(0, i+1);


    s = s.stripWhiteSpace();
    if(s.isEmpty()) continue;
    if(s.at(0) == '#') continue;
    if(s.at(0) == '.') if(s.left(5) != ".Def:") { // insert simulations later
      if(insertSim) Collect.append(s);
      continue;
    }
    Stream << "  " << s << '\n';
  }
}

// ------------------------------------------------------------------------
void SimMessage::slotFinishSpiceNetlist(int status )
{
  if(makeSubcircuit)
    Stream << ".Def:End\n\n";

  nextSPICE();
}

// ------------------------------------------------------------------------
#ifdef __MINGW32__
#include <windows.h>
static QString pathName(QString longpath) {
  const char * lpath = QDir::convertSeparators(longpath).ascii();
  char spath[2048];
  int len = GetShortPathNameA(lpath,spath,sizeof(spath)-1);
  spath[len] = '\0';
  return QString(spath);
}
#else
static QString pathName(QString longpath) {
  return longpath;
}
#endif

// ------------------------------------------------------------------------
void SimMessage::startSimulator()
{
  // Using the Doc pointer here is risky as the user may have closed
  // the schematic, but converting the SPICE netlists is (hopefully)
  // faster than the user (I have no other idea).

  QString SimTime;
  QString Program;
  QStringList Arguments;
  QString SimPath = QDir::convertSeparators (QucsSettings.QucsHomeDir.absPath());
#ifdef __MINGW32__
  QString QucsDigiLib = "qucsdigilib.bat";
  QString QucsDigi = "qucsdigi.bat";
  QString QucsVeri = "qucsveri.bat";
#else
  QString QucsDigiLib = "qucsdigilib";
  QString QucsDigi = "qucsdigi";
  QString QucsVeri = "qucsveri";
#endif
  SimOpt = NULL;
  bool isVerilog = false;

  // Simulate text window.
  if(DocWidget->inherits("QTextEdit")) {

    TextDoc * Doc = (TextDoc*)DocWidget;

    // Take VHDL file in memory as it could contain unsaved changes.
    Stream << Doc->text();
    NetlistFile.close();
    ProgText->insert(tr("done.")+"\n");  // of "creating netlist... 

    // Simulation.
    if (Doc->simulation) {
      SimTime = Doc->SimTime;
      QString libs = Doc->Libraries.lower();
#ifdef __MINGW32__
      if(libs.isEmpty()) {
	libs = "-Wl";
      } else {
	libs.replace(" ",",-l");
	libs = "-Wl,-l" + libs;
      }
#else
      if(libs.isEmpty()) {
	libs = "-c";
      } else {
	libs.replace(" ",",-l");
	libs = "-c,-l" + libs;
      }
#endif
      Program = pathName(QucsSettings.BinDir + QucsDigi);
	  Arguments  << "netlist.txt" << DataSet << SimTime << pathName(SimPath)
		  << pathName(QucsSettings.BinDir) << libs;
    }
    // Module.
    else {
      QString text = Doc->text();
      VHDL_File_Info VInfo (text);
      QString entity = VInfo.EntityName.lower();
      QString lib = Doc->Library.lower();
      if (lib.isEmpty()) lib = "work";
      QString dir = QDir::convertSeparators (QucsSettings.QucsHomeDir.path());
      QDir vhdlDir(dir);
      if(!vhdlDir.exists("vhdl"))
	if(!vhdlDir.mkdir("vhdl")) {
	  ErrText->insert(tr("ERROR: Cannot create VHDL directory \"%1\"!")
			  .arg(vhdlDir.path()+"/vhdl"));
	  return;
	}
      vhdlDir.setPath(vhdlDir.path()+"/vhdl");
      if(!vhdlDir.exists(lib))
	if(!vhdlDir.mkdir(lib)) {
	  ErrText->insert(tr("ERROR: Cannot create VHDL directory \"%1\"!")
			  .arg(vhdlDir.path()+"/"+lib));
	  return;
	}
      vhdlDir.setPath(vhdlDir.path()+"/"+lib);
      QFile destFile;
      destFile.setName(vhdlDir.filePath(entity+".vhdl"));
      if(!destFile.open(QIODevice::WriteOnly)) {
	ErrText->insert(tr("ERROR: Cannot create \"%1\"!")
			.arg(destFile.name()));
	return;
      }
      destFile.writeBlock(text.ascii(), text.length());
      destFile.close();
      Program = pathName(QucsSettings.BinDir + QucsDigiLib);
	  Arguments << "netlist.txt" << pathName(SimPath) << entity << lib;
    }
  }
  // Simulate schematic window.
  else {
    // output NodeSets, SPICE simulations etc.
    for(QStringList::Iterator it = Collect.begin();
	it != Collect.end(); ++it) {
      // don't put library includes into netlist...
      if ((*it).right(4) != ".lst" &&
	  (*it).right(5) != ".vhdl" &&
	  (*it).right(4) != ".vhd" &&
	  (*it).right(2) != ".v") {
	Stream << *it << '\n';
      }
    }
    Stream << '\n';

    isVerilog = ((Schematic*)DocWidget)->isVerilog;
    SimTime = ((Schematic*)DocWidget)->createNetlist(Stream, SimPorts);
    if(SimTime.length()>0&&SimTime.at(0) == '\xA7') {
      NetlistFile.close();
      ErrText->insert(SimTime.mid(1));
      FinishSimulation(-1);
      return;
    }
    if (isVerilog) {
      Stream << "\n"
	     << "  initial begin\n"
	     << "    $dumpfile(\"digi.vcd\");\n"
	     << "    $dumpvars();\n"
	     << "    #" << SimTime << " $finish;\n"
	     << "  end\n\n"
	     << "endmodule // TestBench\n";
    }
    NetlistFile.close();
    ProgText->insert(tr("done.")+"\n");  // of "creating netlist... 

    if(SimPorts < 0) {
      if((SimOpt = findOptimization((Schematic*)DocWidget))) {
	    ((Optimize_Sim*)SimOpt)->createASCOnetlist();
	    Program = QucsSettings.AscoDir + "asco"+ executablePostfix; 
        Arguments << "-qucs" << QucsSettings.QucsHomeDir.filePath("asco_netlist.txt")
                  << "-o" << "asco_out";
      }
      else {
	    Program = QucsSettings.BinDir + "qucsator" + executablePostfix;

        Arguments << "-b" << "-g" << "-i" 
                  << QucsSettings.QucsHomeDir.filePath("netlist.txt")
                  << "-o" << DataSet;
      }
    } else {
      if (isVerilog) {
          Program = pathName(QucsSettings.BinDir + QucsVeri);
		  Arguments << "netlist.txt" << DataSet 
                    << SimTime << pathName(SimPath)
                    << pathName(QucsSettings.BinDir) << "-c";
      } else {
#ifdef __MINGW32__
	Program = pathName(QucsSettings.BinDir + QucsDigi);
	Arguments << "netlist.txt" << DataSet << SimTime << pathName(SimPath)
		      << pathName(QucsSettings.BinDir) << "-Wl" << "-c";
#else
	Program = pathName(QucsSettings.BinDir + QucsDigi);
	Arguments << "netlist.txt" << DataSet << SimTime << pathName(SimPath)
		      << pathName(QucsSettings.BinDir) << "-Wall" << "-c";

#endif
      }
    }
  }

  disconnect(&SimProcess, 0, 0, 0);
  connect(&SimProcess, SIGNAL(readyReadStandardError()), SLOT(slotDisplayErr()));
  connect(&SimProcess, SIGNAL(readyReadStandardOutput()), SLOT(slotDisplayMsg()));
  connect(&SimProcess, SIGNAL(finished(int)), SLOT(slotSimEnded(int)));

#ifdef SPEEDUP_PROGRESSBAR
  waitForUpdate = false;
#endif
  wasLF = false;
  
  ProgressText = "";
  
#ifdef __MINGW32__  
  QString sep(";"); // path separator
#else  
  QString sep(":");
#endif
  
  // append process PATH 
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("PATH", env.value("PATH") + sep + QucsSettings.BinDir );
  SimProcess.setProcessEnvironment(env); 
  QFile file(Program);
  if ( !file.exists() ){
    ErrText->insert(tr("ERROR: Program not found: %1").arg(Program));
    FinishSimulation(-1);
    return;
  }
  else
    file.close();
  
  SimProcess.start(Program, Arguments); // launch the program
  
  if(!SimProcess.Running) {
    ErrText->insert(tr("ERROR: Cannot start simulator!"));
    FinishSimulation(-1);
    return;
  }
	
}

// ------------------------------------------------------------------------
Component * SimMessage::findOptimization(Schematic *Doc) {
  Component *pc;
  for(pc=Doc->Components->first(); pc!=0; pc=Doc->Components->next())
    if(pc->isActive)
      if(pc->Model == ".Opt")
	return pc;
  return NULL;
}


// ------------------------------------------------------------------------
// Is called when the process sends an output to stdout.
void SimMessage::slotDisplayMsg()
{
  int i;
  ProgressText += QString(SimProcess.readAllStandardOutput());
  if(wasLF) {
    i = ProgressText.findRev('\r');
#ifdef __MINGW32__
    while (i > 1 && ProgressText.at(i+1) == '\n') {
      i = ProgressText.findRev('\r', i-1);
    }
#endif /* __MINGW32__ */
    if(i > 1) {
#ifdef SPEEDUP_PROGRESSBAR
      iProgress = 10*int(ProgressText.at(i-2).latin1()-'0') +
                     int(ProgressText.at(i-1).latin1()-'0');
      if(!waitForUpdate) {
        QTimer::singleShot(20, this, SLOT(slotUpdateProgressBar()));
        waitForUpdate = true;
      }
#else
      SimProgress->setMaximum(100);
      SimProgress->setValue(
         10*int(ProgressText.at(i-2).latin1()-'0') +
            int(ProgressText.at(i-1).latin1()-'0'));
#endif
      ProgressText.remove(0, i+1);
    }

    if(ProgressText.size()>0&&ProgressText.at(0).latin1() <= '\t')
      return;
  }
  else {
    i = ProgressText.find('\t');
    if(i >= 0) {
      wasLF = true;
      ProgText->insert(ProgressText.left(i));
      ProgressText.remove(0, i+1);
      return;
    }
  }

  ProgText->insert(ProgressText);
  ProgressText = "";
  wasLF = false;
}

#ifdef SPEEDUP_PROGRESSBAR
// ------------------------------------------------------------------------
void SimMessage::slotUpdateProgressBar()
{
  SimProgress->setProgress(iProgress, 100);
  waitForUpdate = false;
}
#endif

// ------------------------------------------------------------------------
// Is called when the process sends an output to stderr.
void SimMessage::slotDisplayErr()
{
  QTextCursor cursor = ErrText->textCursor();
  cursor.movePosition(QTextCursor::End);  
  ErrText->insert(QString(SimProcess.readAllStandardError()));
}

// ------------------------------------------------------------------------
// Is called when the simulation process terminates.
void SimMessage::slotSimEnded(int status)
{ 
  FinishSimulation(status); //0 - normal  | 1 - crash
}

// ------------------------------------------------------------------------
// Is called when the simulation ended with errors before starting simulator
// process.
void SimMessage::FinishSimulation(int Status)
{
  Abort->setText(tr("Close window"));
  Display->setDisabled(false);
  SimProgress->setValue(100);  // progress bar to 100%

  QDate d = QDate::currentDate();   // get date of today
  QTime t = QTime::currentTime();   // get time

  if(Status == 0) {
    QString txt = tr("Simulation ended on %1 at %2").
      arg(d.toString("ddd dd. MMM yyyy")).
      arg(t.toString("hh:mm:ss"));
    ProgText->insert("\n" + txt + "\n" + tr("Ready.") + "\n");
  }
  else {
    QString txt = tr("Errors occurred during simulation on %1 at %2").
      arg(d.toString("ddd dd. MMM yyyy")).
      arg(t.toString("hh:mm:ss"));
    ProgText->insert("\n" + txt + "\n" + tr("Aborted.") + "\n");
  }

  QFile file(QucsSettings.QucsHomeDir.filePath("log.txt"));  // save simulator messages
  if(file.open(QIODevice::WriteOnly)) {
    int z;
    QTextStream stream(&file);
    stream << tr("Output:\n-------") << "\n\n";
    for(z=0; z<=ProgText->document()->blockCount(); z++)
      stream << ProgText->document()->findBlock(z).text() << "\n";
    stream << "\n\n\n" << tr("Errors:\n-------") << "\n\n";
    for(z=0; z<ErrText->document()->blockCount(); z++)
      stream << ErrText->document()->findBlock(z).text() << "\n";
    file.close();
  }

  if(Status == 0) {
    if(SimOpt) { // save optimization data
      QFile ifile(QucsSettings.QucsHomeDir.filePath("asco_out.dat"));
      QFile ofile(DataSet);
      if(ifile.open(QIODevice::ReadOnly)) {
	if(ofile.open(QIODevice::WriteOnly)) {
	  QByteArray data = ifile.readAll();
	  ofile.writeBlock(data);
	  ofile.close();
	}
	ifile.close();
      }
      if(((Optimize_Sim*)SimOpt)->loadASCOout())
	((Schematic*)DocWidget)->setChanged(true,true);
    }
  }

  emit SimulationEnded(Status, this);
}

// ------------------------------------------------------------------------
// To call accept(), which is protected, from the outside.
void SimMessage::slotClose()
{
  accept();
}

// ------------------------------------------------------------------------
void SimMessage::slotDisplayButton()
{
  emit displayDataPage(DocName, DataDisplay);
  accept();
}
