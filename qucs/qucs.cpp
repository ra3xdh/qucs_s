/***************************************************************************
                                 qucs.cpp
                                ----------
    begin                : Thu Aug 28 2003
    copyright            : (C) 2003, 2004, 2005, 2006 by Michael Margraf
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <QModelIndex>
#include <QAction>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QDockWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QComboBox>
#include <QMenu>
#include <QMessageBox>
#include <QFileDialog>
#include <QStatusBar>
#include <QShortcut>
#include <QApplication>
#include <QClipboard>
#include <QInputDialog>
#include <QDesktopServices>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <QUrl>
#include <QSettings>
#include <QVariant>
#include <QDebug>

#include "main.h"
#include "qucs.h"
#include "qucsdoc.h"
#include "textdoc.h"
#include "schematic.h"
#include "mouseactions.h"
#include "messagedock.h"
#include "settings.h"
#include "wire.h"
#include "module.h"
#include "projectView.h"
#include "components/component.h"
#include "components/vacomponent.h"
#include "components/vhdlfile.h"
#include "components/verilogfile.h"
#include "components/subcircuit.h"
#include "dialogs/savedialog.h"
#include "dialogs/newprojdialog.h"
#include "dialogs/settingsdialog.h"
#include "dialogs/digisettingsdialog.h"
#include "dialogs/vasettingsdialog.h"
#include "dialogs/qucssettingsdialog.h"
#include "dialogs/searchdialog.h"
#include "dialogs/sweepdialog.h"
#include "dialogs/labeldialog.h"
#include "dialogs/matchdialog.h"
#include "dialogs/simmessage.h"
#include "dialogs/exportdialog.h"
#include "dialogs/displaydialog.h"
//#include "dialogs/vtabwidget.h"
//#include "dialogs/vtabbeddockwidget.h"
#include "extsimkernels/externsimdialog.h"
#include "dialogs/tuner.h"
#include "octave_window.h"
#include "printerwriter.h"
#include "imagewriter.h"
#include "qucslib_common.h"
#include "misc.h"
#include "extsimkernels/verilogawriter.h"
#include "extsimkernels/CdlNetlistWriter.h"
#include "extsimkernels/simsettingsdialog.h"
//#include "extsimkernels/codemodelgen.h"
#include "symbolwidget.h"

QucsApp::QucsApp(bool netlist2Console) :
  a_netlist2Console(netlist2Console)
{
  windowTitle = misc::getWindowTitle();
  setWindowTitle(windowTitle);

  QucsSettings.hasDarkTheme = misc::isDarkTheme();

  // Instantiate settings singleton and restore window geometry.
  const auto geometry = _settings::Get().item<QByteArray>("MainWindowGeometry");

  if (!geometry.isEmpty()) {
    qDebug() << "Saved geometry is: " << geometry;
    restoreGeometry(geometry);
  }
  // Set a default size and position if geometry does not yet exist.
  else {
    QSize size = QGuiApplication::primaryScreen()->size();
    int w = size.width();
    int h = size.height();
    setGeometry (w * 0.25, h * 0.25, w * 0.5, h * 0.5);
  }

  QucsFileFilter =
    tr("Schematic") + " (*.sch);;" +
    tr("Symbol only") + " (*.sym);;" +
    tr("Data Display") + " (*.dpl);;" +
    tr("Qucs Documents") + " (*.sch *.dpl);;" +
    tr("VHDL Sources") + " (*.vhdl *.vhd);;" +
    tr("Verilog Sources") + " (*.v);;" +
    tr("Verilog-A Sources") + " (*.va);;" +
    tr("Octave Scripts") + " (*.m *.oct);;" +
    tr("Spice Files") + QStringLiteral(" (") + QucsSettings.spiceExtensions.join(" ") + QStringLiteral(");;") +
    tr("Any File")+" (*)";

  //updateSchNameHash();
  //updateSpiceNameHash();

  MouseMoveAction = 0;
  MousePressAction = 0;
  MouseReleaseAction = 0;
  MouseDoubleClickAction = 0;

  initView();
  initActions();
  initMenuBar();
  fillSimulatorsComboBox();
  initToolBar();
  initStatusBar();
  viewBrowseDock->setChecked(true);
  slotViewOctaveDock(false);
  slotUpdateRecentFiles();
  initCursorMenu();
  //Module::registerModules ();

  // instance of small text search dialog
  SearchDia = new SearchDialog(this);
  TuningMode = false;

  // creates a document called "untitled"
  Schematic *d = new Schematic(this, "");
  int i = addDocumentTab(d);
  DocumentTab->setCurrentIndex(i);

  select->setChecked(true);  // switch on the 'select' action
  switchSchematicDoc(true);  // "untitled" document is schematic
  QDir homeDir       = QDir::homePath();
  lastExportFilename = homeDir.absoluteFilePath("export.png");

  // load documents given as command line arguments
  for(int z=1; z<qApp->arguments().size(); z++) {
    QString arg = qApp->arguments()[z];
    QByteArray ba = arg.toLatin1();
    const char *c_arg = ba.data();
    if(*(c_arg) != '-') {
      QFileInfo Info(arg);
      QucsSettings.QucsWorkDir.setPath(Info.absoluteDir().absolutePath());
      arg = QucsSettings.QucsWorkDir.filePath(Info.fileName());
      gotoPage(arg);
    }
  }

  QDir QucsBinDir(QucsSettings.BinDir);
  if (QucsSettings.firstRun) { // try to find Ngspice
      QString ngspice_exe_name = "ngspice";
#ifdef Q_OS_WIN
      ngspice_exe_name+="_con";
#endif
      /* search own path */
#ifdef Q_OS_MACOS
      QString ngspice_exe1 = QStandardPaths::findExecutable(ngspice_exe_name,{"/opt/homebrew/bin"});
#else
      QString ngspice_exe1 = QStandardPaths::findExecutable(ngspice_exe_name,{QucsBinDir.absolutePath()});
#endif

      /* search system path */
      QString ngspice_exe2 = QStandardPaths::findExecutable(ngspice_exe_name);

      /* search own path */
      QString qucsator_exe1 = QStandardPaths::findExecutable("qucsator_rf",{QucsBinDir.absolutePath()});
      /* search system path */
      QString qucsator_exe2 = QStandardPaths::findExecutable("qucsator_rf");

      QString ngspice_exe;
      bool ngspice_found = false;
      if(!ngspice_exe1.isEmpty()){
          ngspice_found = true;
          ngspice_exe = ngspice_exe1;
      }else if(!ngspice_exe2.isEmpty()){
          ngspice_found = true;
          ngspice_exe = ngspice_exe2;
      }

      QString qucsator_exe;
      bool qucsator_found = false;
      if(!qucsator_exe1.isEmpty()){
          qucsator_found = true;
          qucsator_exe = qucsator_exe1;
      }else if(!qucsator_exe2.isEmpty()){
          qucsator_found = true;
          qucsator_exe = qucsator_exe2;
      }

      QString info_string;
      if (ngspice_found) {
          QucsSettings.DefaultSimulator  = spicecompat::simNgspice;
          QucsSettings.NgspiceExecutable = ngspice_exe;
          info_string += tr("Ngspice found at: ") + ngspice_exe + "\n";
      }

      if(qucsator_found){
          QucsSettings.Qucsator = qucsator_exe;
          info_string += tr("QucsatorRF found at: ") + qucsator_exe + "\n";
      }

      info_string += tr("\nYou can specify another location later"
                        " using Simulation->Simulators Setings\n");
      if (!ngspice_found && qucsator_found) {
          QucsSettings.DefaultSimulator = spicecompat::simQucsator;
          info_string += tr("\nNOTE: Only QucsatorRF found. This simulator is not"
                            " recommended for general purpose schematics. "
                            " Please install Ngspice.");
      }
      if (ngspice_found || qucsator_found) {
          QMessageBox::information(nullptr,tr("Set simulator"), info_string);
          fillSimulatorsComboBox();
      } else {
#ifdef Q_OS_WIN
          QucsSettings.NgspiceExecutable = "ngspice_con.exe";
          QucsSettings.Qucsator = "qucsator_rf.exe";
#else
          QucsSettings.NgspiceExecutable = "ngspice";
          QucsSettings.Qucsator = "qucsator_rf";
#endif
          QMessageBox::information(this,tr("Qucs"),tr("No simulators found automatically. Please specify simulators"
                                                      " in the next dialog window."));
          slotSimSettings();
      }
      QucsSettings.firstRun = false;
  } else {
      if (!QucsSettings.Qucsator.contains("qucsator_rf")) {
          QucsSettings.Qucsator = QStandardPaths::findExecutable("qucsator_rf",{QucsBinDir.absolutePath()});

          if(!QucsSettings.Qucsator.isEmpty()){
            QMessageBox::information(this, "Qucs",
                    tr("QucsatorRF found at: ") + QucsSettings.Qucsator + "\n");
          }
      }
  }

//  fillLibrariesTreeView();

}

QucsApp::~QucsApp()
{
  Module::unregisterModules ();
}


void QucsApp::readXML(QFile & library_file) {
  if (!library_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "Error opening file:" << library_file.errorString();
    return;
  }

  QXmlStreamReader xmlReader(&library_file);
  QString LibraryName, ComponentName;
  QMap<QString, ComponentInfo> Component;
  QDir LibraryDir(":/Libraries"); // Path to the Qucs-S XML library
  QString QucsSLibraryPath = LibraryDir.path();

  while (!xmlReader.atEnd() && !xmlReader.hasError()) {
    if (xmlReader.readNextStartElement()) {
      if (xmlReader.name() == QString("Library")) {
        QString libraryName = xmlReader.attributes().value("name").toString();
        LibraryName = libraryName;

        while (xmlReader.readNextStartElement()) {
          if (xmlReader.name() == QString("Component")) {
            QString componentName = xmlReader.attributes().value("name").toString();
            ComponentName = componentName;
            Component[ComponentName].name = ComponentName;
            Component[ComponentName].Category = libraryName;

            QString Model = xmlReader.attributes().value("schematic_id").toString();
            Component[ComponentName].Model = Model;

            QString ShowName = xmlReader.attributes().value("show_name").toString();
            if (ShowName.isEmpty()) {
              ShowName = "true";
            }
            bool ShowNameinSchematic = (ShowName.toLower() == "true");
            Component[ComponentName].ShowNameinSchematic = ShowNameinSchematic;

            while (xmlReader.readNextStartElement()) {
              if (xmlReader.name() == QString("Description")) {
                QString Description = xmlReader.readElementText().trimmed();
                Component[ComponentName].description = Description;
              } else if (xmlReader.name() == QString("Netlists")) {
                while (xmlReader.readNextStartElement()) {
                  if (xmlReader.name() == QString("QucsatorNetlist")) {
                    QString QucsatorNetlist = xmlReader.attributes().value("value").toString();
                    Component[ComponentName].Netlists["Qucsator"] = QucsatorNetlist;
                    xmlReader.skipCurrentElement();
                  } else if (xmlReader.name() == QString("NgspiceNetlist")) {
                    QString NgspiceNetlist = xmlReader.attributes().value("value").toString();
                    Component[ComponentName].Netlists["Ngspice"] = NgspiceNetlist;

                           // Initialize optional properties with "None". These properties are required for PDK where circuit
                           // are defined using subcircuits and they point to an external library

                           // Process Include and OSDI elements
                    while (xmlReader.readNextStartElement()) {
                      if (xmlReader.name() == QString("Include")) {
                        QString includeValue = xmlReader.attributes().value("value").toString();
                        // Replace {PDK_ROOT} with the actual path
                        includeValue.replace("{PDK_ROOT}", QucsSettings.PDK_ROOT.absolutePath());
                        Component[ComponentName].Netlists["Ngspice_LibraryInclude"].append(includeValue);
                        xmlReader.skipCurrentElement();
                      } else if (xmlReader.name() == QString("OSDI")) {
                        QString osdiValue = xmlReader.attributes().value("value").toString();
                        // Replace {PDK_ROOT} with the actual path
                        osdiValue.replace("{PDK_ROOT}", QucsSettings.PDK_ROOT.absolutePath());
                        Component[ComponentName].OSDIfiles.append(osdiValue);
                        xmlReader.skipCurrentElement();
                      } else {
                        xmlReader.skipCurrentElement();
                      }
                    }
                  } else if (xmlReader.name() == QString("ProbeVariables")) {
                    while (xmlReader.readNextStartElement()) {
                      if (xmlReader.name() == QString("Ngspice")) {
                        QString NgspiceProbeVariable = xmlReader.attributes().value("value").toString();
                        Component[ComponentName].ProbeVariables["Ngspice"] = NgspiceProbeVariable;
                        xmlReader.skipCurrentElement();
                      } else if (xmlReader.name() == QString("Xyce")) {
                        QString XyceProbeVariable = xmlReader.attributes().value("value").toString();
                        Component[ComponentName].ProbeVariables["Xyce"] = XyceProbeVariable;
                        xmlReader.skipCurrentElement();
                      } else {
                        xmlReader.skipCurrentElement();
                      }
                    }
                  }
                  else {
                    xmlReader.skipCurrentElement();
                  }
                }
              } else if (xmlReader.name() == QString("Symbols")) {
                while (xmlReader.readNextStartElement()) {
                  if (xmlReader.name() == QString("Symbol")) {
                    QString symbolName = xmlReader.attributes().value("id").toString();
                    QString symbolType = xmlReader.attributes().value("type").toString();

                    if (symbolType.isEmpty()) {//If the XML file does not contain this field, set it to "true" automatically
                      symbolType = "explicit";
                    }

                    if (symbolType == "explicit") {
                      QVector<int> boundingBox;
                      SymbolDescription symbolData = parseSymbol(xmlReader, boundingBox);
                      Component[ComponentName].symbol[symbolName] = symbolData;
                      Component[ComponentName].SymbolBoundingBox[symbolName] = boundingBox;
                    } else if (symbolType == "external") {
                      xmlReader.readNextStartElement();
                      if (xmlReader.name() == QString("File")) {
                        QString PathSymbol = xmlReader.readElementText().trimmed();
                        PathSymbol.replace("{QUCS_S_COMPONENTS_LIBRARY}", QucsSLibraryPath);

                        // Read the external file and parse the symbol
                        QFile externalFile(PathSymbol);
                        if (externalFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                          QXmlStreamReader externalXmlReader(&externalFile);
                          QVector<int> boundingBox;

                          externalXmlReader.readNextStartElement();
                          qDebug() << externalXmlReader.name();

                          SymbolDescription symbolData = parseSymbol(externalXmlReader, boundingBox);
                          Component[ComponentName].symbol[symbolName] = symbolData;
                          Component[ComponentName].SymbolBoundingBox[symbolName] = boundingBox;
                          externalFile.close();
                        } else {
                          qDebug() << "Error opening external symbol file:" << PathSymbol;
                        }
                      }
                      xmlReader.skipCurrentElement();
                    } else {
                      xmlReader.skipCurrentElement();
                    }
                  } else {
                    xmlReader.skipCurrentElement();
                  }
                }
              } else if (xmlReader.name() == QString("Parameters")) {
                while (xmlReader.readNextStartElement()) {
                  if (xmlReader.name() == QString("Parameter")) {
                    QString Name = xmlReader.attributes().value("name").toString();
                    QString Unit = xmlReader.attributes().value("unit").toString();
                    QString DefaultValue = xmlReader.attributes().value("default_value").toString();

                    QString ShowParam = xmlReader.attributes().value("show").toString();
                    if (ShowParam.isEmpty()) {
                      ShowParam = "false";
                    }
                    bool Show = (ShowParam.toLower() == "true");

                    QString Description;
                    xmlReader.readNextStartElement();
                    if (xmlReader.name() == QString("Description")) {
                      Description = xmlReader.readElementText().trimmed();
                    }

                    ParameterInfo parameter;
                    parameter.DefaultValue = DefaultValue.toDouble();
                    parameter.Unit = Unit;
                    parameter.Description = Description;
                    parameter.Show = Show;
                    Component[ComponentName].parameters[Name] = parameter;

                    xmlReader.skipCurrentElement();
                  } else {
                    xmlReader.skipCurrentElement();
                  }
                }
              } else {
                xmlReader.skipCurrentElement();
              }
            }
            LibraryComponents[libraryName] = Component;
          } else {
            xmlReader.skipCurrentElement();
          }
        }
      } else {
        xmlReader.skipCurrentElement();
      }
    }
  }

  if (xmlReader.hasError()) {
    qDebug() << "XML error:" << xmlReader.errorString();
  }

  library_file.close();
}


// This function is used to parsing the XML symbol description (which could be embedded into the library file or in an external file)
SymbolDescription QucsApp::parseSymbol(QXmlStreamReader &xmlReader, QVector<int> &boundingBox) {
  SymbolDescription SymbolData;
  int minX = 1e3, maxX = -1e3, minY = 1e3, maxY = -1e3;

  while (xmlReader.readNextStartElement()) {
    if (xmlReader.name() == QString("PortSym")) {
      PortInfo Port;
      Port.x = xmlReader.attributes().value("x").toDouble();
      Port.y = xmlReader.attributes().value("y").toDouble();
      SymbolData.Ports.append(Port);
      xmlReader.skipCurrentElement();

      // Update bounding box
      updateBoundingBox(minX, maxX, minY, maxY, Port.x, Port.y);

    } else if (xmlReader.name() == QString("Line")) {
      LineInfo Line;
      Line.x1 = xmlReader.attributes().value("x1").toDouble();
      Line.y1 = xmlReader.attributes().value("y1").toDouble();
      Line.x2 = xmlReader.attributes().value("x2").toDouble();
      Line.y2 = xmlReader.attributes().value("y2").toDouble();
      QColor color(xmlReader.attributes().value("color").toString());
      int penWidth = xmlReader.attributes().value("width").toInt();
      int style = xmlReader.attributes().value("style").toDouble();

      Qt::PenStyle penStyle = static_cast<Qt::PenStyle>(style);
      QPen pen(color, penWidth, penStyle);
      Line.Pen = pen;
      SymbolData.Lines.append(Line);
      xmlReader.skipCurrentElement();

      // Update bounding box
      updateBoundingBox(minX, maxX, minY, maxY, Line.x1, Line.y1);
      updateBoundingBox(minX, maxX, minY, maxY, Line.x2, Line.y2);

      if (Line.y1 < minY) minY = Line.y1;
                        if (Line.y1 > maxY) maxY = Line.y1;
                        if (Line.y2 < minY) minY = Line.y2;
                        if (Line.y2 > maxY) maxY = Line.y2;

    } else if (xmlReader.name() == QString("Arc")) {
      ArcInfo Arc;
      Arc.x = xmlReader.attributes().value("x").toDouble();
      Arc.y = xmlReader.attributes().value("y").toDouble();
      Arc.width = xmlReader.attributes().value("arc_width").toDouble();
      Arc.height = xmlReader.attributes().value("arc_height").toDouble();
      Arc.angle = xmlReader.attributes().value("angle").toDouble();
      Arc.arclen = xmlReader.attributes().value("arclen").toDouble();

      QColor color(xmlReader.attributes().value("color").toString());
      int penWidth = xmlReader.attributes().value("pen_width").toDouble();

      QPen pen(color, penWidth, Qt::SolidLine);
      Arc.Pen = pen;

      SymbolData.Arcs.append(Arc);
      xmlReader.skipCurrentElement();

             // Update bounding box
      if (Arc.x - Arc.width/2 < minX) minX = Arc.x - Arc.width/2;
                        if (Arc.x + Arc.width/2 > maxX) maxX = Arc.x + Arc.width/2;
                        if (Arc.y - Arc.height/2 < minY) minY = Arc.y - Arc.height/2;
                        if (Arc.y + Arc.height/2 > maxY) maxY = Arc.y + Arc.height/2;

    } else if (xmlReader.name() == QString("Polyline")) {
      PolylineInfo Polyline;

             // Read pen attributes
      QColor color(xmlReader.attributes().value("color").toString());
      int penWidth = xmlReader.attributes().value("width").toDouble();
      int style = xmlReader.attributes().value("style").toInt();
      int capStyle = xmlReader.attributes().value("capStyle").toInt();
      int joinStyle = xmlReader.attributes().value("joinStyle").toInt();

      Qt::PenStyle penStyle = static_cast<Qt::PenStyle>(style);
      Qt::PenCapStyle penCapStyle = static_cast<Qt::PenCapStyle>(capStyle);
      Qt::PenJoinStyle penJoinStyle = static_cast<Qt::PenJoinStyle>(joinStyle);

      QPen pen(color, penWidth, penStyle, penCapStyle, penJoinStyle);
      Polyline.Pen = pen;

             // Read brush style
      int brushStyle = xmlReader.attributes().value("brushStyle").toInt();
      Polyline.Brush.setStyle(static_cast<Qt::BrushStyle>(brushStyle));

             // Read points
      while (xmlReader.readNextStartElement()) {
        if (xmlReader.name() == QString("point")) {
          double x = xmlReader.attributes().value("x").toDouble();
          double y = xmlReader.attributes().value("y").toDouble();
          Polyline.Points.append(QPointF(x, y));
          xmlReader.skipCurrentElement();

          // Update bounding box
          updateBoundingBox(minX, maxX, minY, maxY, x, y);

        } else {
          xmlReader.skipCurrentElement();
        }
      }
      SymbolData.Polylines.append(Polyline);

    } else if (xmlReader.name() == QString("Ellipse")) {
      EllipseInfo Ellips;

             // Read ellipse attributes
      Ellips.x = xmlReader.attributes().value("x").toDouble();
      Ellips.y = xmlReader.attributes().value("y").toDouble();
      Ellips.width = xmlReader.attributes().value("width").toDouble();
      Ellips.height = xmlReader.attributes().value("height").toDouble();

             // Read pen attributes
      QColor penColor(xmlReader.attributes().value("penColor").toString());
      int penWidth = xmlReader.attributes().value("penWidth").toDouble();
      int penStyle = xmlReader.attributes().value("penStyle").toInt();

      Ellips.Pen = QPen(penColor, penWidth, static_cast<Qt::PenStyle>(penStyle));

             // Read brush attributes
      QColor brushColor(xmlReader.attributes().value("brushColor").toString());
      int brushStyle = xmlReader.attributes().value("brushStyle").toInt();

      Ellips.Brush = QBrush(brushColor, static_cast<Qt::BrushStyle>(brushStyle));

      SymbolData.Ellipses.append(Ellips);
      xmlReader.skipCurrentElement();

             // Update bounding box
      if (Ellips.x < minX) minX = Ellips.x;
      if (Ellips.x > maxX) maxX = Ellips.x;
      if (Ellips.y < minY) minY = Ellips.y;
      if (Ellips.y > maxY) maxY = Ellips.y;

    } else if (xmlReader.name() == QString("Rect")) {
      RectInfo Rect;

             // Read rectangle attributes
      Rect.x = xmlReader.attributes().value("x").toDouble();
      Rect.y = xmlReader.attributes().value("y").toDouble();
      Rect.width = xmlReader.attributes().value("width").toDouble();
      Rect.height = xmlReader.attributes().value("height").toDouble();

             // Read pen attributes
      QColor penColor(xmlReader.attributes().value("penColor").toString());
      int penWidth = xmlReader.attributes().value("penWidth").toDouble();
      int penStyle = xmlReader.attributes().value("penStyle").toInt();

      Rect.Pen = QPen(penColor, penWidth, static_cast<Qt::PenStyle>(penStyle));

             // Read brush attributes
      QColor brushColor(xmlReader.attributes().value("brushColor").toString());
      int brushStyle = xmlReader.attributes().value("brushStyle").toInt();

      Rect.Brush = QBrush(brushColor, static_cast<Qt::BrushStyle>(brushStyle));

      SymbolData.Rects.append(Rect);
      xmlReader.skipCurrentElement();

             // Update bounding box
      if (Rect.x - Rect.width/2 < minX) minX = Rect.x - Rect.width/2;
                        if (Rect.x + Rect.width/2 > maxX) maxX = Rect.x + Rect.width/2;
                        if (Rect.y - Rect.height/2 < minY) minY = Rect.y - Rect.height/2;
                        if (Rect.y + Rect.height/2 > maxY) maxY = Rect.y + Rect.height/2;


    } else if (xmlReader.name() == QString("Text")) {
      TextInfo Text;

             // Read text attributes
      Text.x = xmlReader.attributes().value("x").toDouble();
      Text.y = xmlReader.attributes().value("y").toDouble();
      Text.s = xmlReader.attributes().value("text").toString();
      Text.Color = QColor(xmlReader.attributes().value("color").toString());
      Text.Size = xmlReader.attributes().value("size").toDouble();
      Text.mCos = xmlReader.attributes().value("mCos").toDouble();
      Text.mSin = xmlReader.attributes().value("mSin").toDouble();
      Text.over = xmlReader.attributes().value("over").toInt() != 0;
      Text.under = xmlReader.attributes().value("under").toInt() != 0;

      SymbolData.Texts.append(Text);
      xmlReader.skipCurrentElement();

             // Update bounding box
      updateBoundingBox(minX, maxX, minY, maxY, Text.x, Text.y);

    } else {
      xmlReader.skipCurrentElement();
    }
  }

  boundingBox = {minX, maxX, minY, maxY};
  return SymbolData;
}

// This function is used to update the component's bounding box. The readXML function loads the symbol geometry description and this function
// is called when parsing that information to look for new maximum and minimum x and y coordinates
void QucsApp::updateBoundingBox(int &minX, int &maxX, int &minY, int &maxY, int x, int y) {
  minX = std::min(minX, x);
  maxX = std::max(maxX, x);
  minY = std::min(minY, y);
  maxY = std::max(maxY, y);
}


// #######################################################################
// ##########                                                   ##########
// ##########     Creates the working area (QTabWidget etc.)    ##########
// ##########                                                   ##########
// #######################################################################
/**
 * @brief QucsApp::initView Setup the layour of all widgets
 */
void QucsApp::initView()
{
  // set application icon
  // APPLE sets the QApplication icon with Info.plist
#ifndef __APPLE__
  setWindowIcon (QPixmap(QStringLiteral(":/bitmaps/hicolor/scalable/apps/qucs.svg")));
#else
  // setUnifiedTitleAndToolBarOnMac(true);
  setStyleSheet("QToolButton { padding: 0px; }");
#endif

  DocumentTab = new QTabWidget(this);
#if __APPLE__
  DocumentTab->setDocumentMode(true);
#endif
  setCentralWidget(DocumentTab);

  connect(DocumentTab,
          SIGNAL(currentChanged(int)), SLOT(slotChangeView()));

  // Give every tab a close button, and connect the button's signal to
  // slotFileClose
  DocumentTab->setTabsClosable(true);
  connect(DocumentTab,
          SIGNAL(tabCloseRequested(int)), SLOT(slotFileClose(int)));
#ifdef HAVE_QTABWIDGET_SETMOVABLE
  // make tabs draggable if supported
  DocumentTab->setMovable (true);
#endif

  dock = new QDockWidget(tr("Main Dock"),this);
  TabView = new QTabWidget(dock);
  TabView->setTabPosition(QTabWidget::West);
#if __APPLE__
  TabView->setDocumentMode(true);
#endif

  connect(dock, SIGNAL(visibilityChanged(bool)), SLOT(slotToggleDock(bool)));

  view = new MouseActions(this);

  editText = new QLineEdit(this);  // for editing component properties
  editText->setFrame(false);
  editText->setHidden(true);

  QPalette p = palette();
  p.setColor(backgroundRole(), QucsSettings.BGColor);
  editText->setPalette(p);

  connect(editText, SIGNAL(returnPressed()), SLOT(slotApplyCompText()));
  connect(editText, SIGNAL(textChanged(const QString&)),
          SLOT(slotResizePropEdit(const QString&)));
  connect(editText, SIGNAL(editingFinished()), SLOT(slotHideEdit()));

  // ----------------------------------------------------------
  // "Project Tab" of the left QTabWidget
  QWidget *ProjGroup = new QWidget();
  QVBoxLayout *ProjGroupLayout = new QVBoxLayout();
  QWidget *ProjButts = new QWidget();
  QPushButton *ProjNew = new QPushButton(tr("New"));
  connect(ProjNew, SIGNAL(clicked()), SLOT(slotButtonProjNew()));
  QPushButton *ProjOpen = new QPushButton(tr("Open"));
  connect(ProjOpen, SIGNAL(clicked()), SLOT(slotButtonProjOpen()));
  QPushButton *ProjDel = new QPushButton(tr("Delete"));
  connect(ProjDel, SIGNAL(clicked()), SLOT(slotButtonProjDel()));

  QHBoxLayout *ProjButtsLayout = new QHBoxLayout();
  ProjButtsLayout->addWidget(ProjNew);
  ProjButtsLayout->addWidget(ProjOpen);
  ProjButtsLayout->addWidget(ProjDel);
  ProjButts->setLayout(ProjButtsLayout);

#if __APPLE__
  ProjGroupLayout->setContentsMargins(0, 0, 0, 0);
  ProjGroupLayout->setSpacing(0);
  ProjButtsLayout->setContentsMargins(5, 0, 5, 0);
  ProjButtsLayout->setSpacing(5);
#endif

  ProjGroupLayout->addWidget(ProjButts);

  Projects = new QListView();

  ProjGroupLayout->addWidget(Projects);
  ProjGroup->setLayout(ProjGroupLayout);

  TabView->addTab(ProjGroup, tr("Projects"));
  TabView->setTabToolTip(TabView->indexOf(ProjGroup), tr("content of project directory"));

  connect(Projects, SIGNAL(doubleClicked(const QModelIndex &)),
          this, SLOT(slotListProjOpen(const QModelIndex &)));

  // ----------------------------------------------------------
  // "Content" Tab of the left QTabWidget
  Content = new ProjectView(this);
  Content->setContextMenuPolicy(Qt::CustomContextMenu);

  TabView->addTab(Content, tr("Content"));
  TabView->setTabToolTip(TabView->indexOf(Content), tr("content of current project"));

  connect(Content, SIGNAL(clicked(const QModelIndex &)),
          SLOT(slotSelectSubcircuit(const QModelIndex &)));

  connect(Content, SIGNAL(doubleClicked(const QModelIndex &)),
          SLOT(slotOpenContent(const QModelIndex &)));

  // ----------------------------------------------------------
  // "Component" Tab of the left QTabWidget
  QWidget *CompGroup  = new QWidget();
  QVBoxLayout *CompGroupLayout = new QVBoxLayout();
  QHBoxLayout *CompSearchLayout = new QHBoxLayout();

  simulatorsCombobox = new QComboBox(this);
  simulatorsCombobox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  connect(simulatorsCombobox, SIGNAL(activated(int)), SLOT(slotChangeSimulator(int)));

  CompChoose = new QComboBox(this);
  CompComps = new QListWidget(this);
  CompComps->setViewMode(QListView::IconMode);
  CompComps->setGridSize(QSize(110,90));
  CompComps->setResizeMode(QListView::Adjust);
  CompComps->setIconSize(QSize(64,64));
  CompComps->setAcceptDrops(false);

  const QString itemStyle = R"(
      QListWidget {
          background: white;
          color: black;
          outline: none;
          padding: 0px;
          margin: 0px;
      }

      QListWidget::item {
          position: relative;
          margin-top: 0px;
          margin-left: 3px;
          margin-right: 3px;
          margin-bottom: 0px;
          border: none;
          border-radius: 2px;
          outline: none;
          background-color: transparent;
      }

      QListWidget::item:hover {
          background-color: rgba(220, 242, 255, 0.75);
      }

      QListWidget::item:selected {
          position: relative;
          background-color: rgba(181, 227, 255, 0.75);
          border: none;
          color: black;
          margin: 0px;
      }

      QListWidget::item:selected:focus {
          position: relative;
          background-color: rgba(181, 227, 255, 0.75);
          border: none;
          color: black;
          margin: 0px;
      }

      QListWidget::item:selected:!focus {
          position: relative;
          background-color: rgba(222, 222, 223, 0.75);
          border: none;
          color: black;
          margin: 0px;
      }

      QListWidget::item:selected:hover {
          position: relative;
          border: 1px solid black; /* Black border when selected and hovered */
          padding-top:-1px;
      }

      QListWidget::item:focus {
          outline: none; /* Remove the default focus outline */
      }
  )";

  CompComps->setStyleSheet(itemStyle);

  // Setup component search box and button.
  CompSearch = new QLineEdit(this);
  CompSearch->setPlaceholderText(tr("Search Components"));
  CompSearchClear = new QPushButton(tr("Clear"));
  CompSearchLayout->addWidget(CompSearch);
  CompSearchLayout->addWidget(CompSearchClear);

  // Setup the component chooser widgets.
  CompGroupLayout->setSpacing(5);
  CompGroupLayout->addLayout(CompSearchLayout);
  CompGroupLayout->addWidget(CompChoose);
  CompGroupLayout->addWidget(CompComps);
  CompGroup->setLayout(CompGroupLayout);

#if __APPLE__
  CompGroupLayout->setContentsMargins(0, 0, 0, 0);
  CompGroupLayout->setSpacing(0);
  CompSearchLayout->setContentsMargins(5, 0, 5, 0);
  CompSearchLayout->setSpacing(5);
#endif

  TabView->addTab(CompGroup,tr("Components"));
  TabView->setTabToolTip(TabView->indexOf(CompGroup), tr("components and diagrams"));

  connect(CompChoose, SIGNAL(activated(int)), SLOT(slotSetCompView(int)));
  connect(CompComps, SIGNAL(itemActivated(QListWidgetItem*)), SLOT(slotSelectComponent(QListWidgetItem*)));
  connect(CompComps, SIGNAL(itemPressed(QListWidgetItem*)), SLOT(slotSelectComponent(QListWidgetItem*)));
  connect(CompSearch, SIGNAL(textEdited(const QString &)), SLOT(slotSearchComponent(const QString &)));
  connect(CompSearchClear, SIGNAL(clicked()), SLOT(slotSearchClear()));

  // ----------------------------------------------------------
  // "Libraries" Tab of the left QTabWidget
  QWidget *LibGroup = new QWidget ();
  QVBoxLayout *LibGroupLayout = new QVBoxLayout ();

  LibCompSearch = new QLineEdit(this);
  LibCompSearch->setPlaceholderText(tr("Search Lib Components"));
  QPushButton *LibCompSearchClear = new QPushButton(tr("Clear"));
  connect(LibCompSearch, SIGNAL(textEdited(const QString &)), SLOT(slotSearchLibComponent(const QString &)));
  connect(LibCompSearchClear, SIGNAL(clicked()), SLOT(slotSearchLibClear()));

  QHBoxLayout *LibCompSearchLayout = new QHBoxLayout;
  LibCompSearchLayout->addWidget(LibCompSearch);
  LibCompSearchLayout->addWidget(LibCompSearchClear);
  LibGroupLayout->addLayout(LibCompSearchLayout);

#if __APPLE__
  LibGroupLayout->setContentsMargins(0, 0, 0, 0);
  LibGroupLayout->setSpacing(0);
  LibCompSearchLayout->setContentsMargins(5, 0, 5, 0);
  LibCompSearchLayout->setSpacing(5);
#endif

  // Load component libraries
  // Open each file in the directory where the system library is stored
  // Read the XML and register each component
  // The symbol is temporary loaded from a file as the code is not true XML.

  QDir LibraryDir(":/Libraries");
  QStringList files = LibraryDir.entryList(QStringList() << "*.xml", QDir::Files);

  for (const QString &filename : files) {
    QFile library_file(LibraryDir.filePath(filename));
    readXML(library_file);
  }

  // Load user PDKs
  QDir pdk_dir(QucsSettings.PDK_ROOT);
  QStringList pdk_folders = pdk_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
  QStringList xml_files; // List of all PDKs to load
  for (const QString &folder : pdk_folders) {
    QDir tech_dir(pdk_dir.filePath(folder + "/libs.tech/qucs/"));
    QStringList folder_xml_files = tech_dir.entryList(QStringList() << "*.xml", QDir::Files);

    for (const QString &file : folder_xml_files) {
      xml_files << tech_dir.filePath(file);
    }
  }

  // Now xml_files contains paths to all PDK (XML) files
  for (const QString &libfile : xml_files) {
    // Process each XML file here
    QFile library_file(libfile);
    readXML(library_file);
  }

  libTreeWidget = new QTreeWidget (this);
  libTreeWidget->setColumnCount (1);
  QStringList headers;
  headers.clear ();
  headers << tr ("Libraries");
  libTreeWidget->setHeaderLabels (headers);

  LibGroupLayout->addWidget (libTreeWidget,6);

  btnShowModel = new QPushButton(tr("Show model"));
  connect(btnShowModel,SIGNAL(clicked(bool)),this,SLOT(slotShowModel()));
  LibGroupLayout->addWidget(btnShowModel);

  CompDescr = new QTextEdit;
  CompDescr->setReadOnly(true);
  LibGroupLayout->addWidget(CompDescr,2);

  Symbol = new SymbolWidget;
  Symbol->disableDragNDrop();
  LibGroupLayout->addWidget(Symbol);

  LibGroup->setLayout (LibGroupLayout);

  //fillLibrariesTreeView ();
  fillComboBox(true);
  slotSetCompView(0);

  TabView->addTab (LibGroup, tr("Libraries"));
  TabView->setTabToolTip (TabView->indexOf (CompGroup), tr ("system and user component libraries"));

  connect(libTreeWidget, SIGNAL(itemPressed (QTreeWidgetItem*, int)),
           SLOT(slotSelectLibComponent (QTreeWidgetItem*)));

  // ----------------------------------------------------------
  // put the tab widget in the dock
  dock->setWidget(TabView);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea);
  this->addDockWidget(Qt::LeftDockWidgetArea, dock);
  TabView->setCurrentIndex(2);

#if __APPLE__
  QWidgetList widgets = TabView->findChildren<QWidget*>();
  foreach(QWidget* widget, widgets) {
    widget->setAttribute(Qt::WA_MacShowFocusRect, false);
  }
#endif

  // ----------------------------------------------------------
  // Octave docking window
  octDock = new QDockWidget(tr("Octave Dock"));

  connect(octDock, SIGNAL(visibilityChanged(bool)), SLOT(slotToggleOctave(bool)));
  octave = new OctaveWindow(octDock);
  this->addDockWidget(Qt::BottomDockWidgetArea, octDock);
  this->setCorner(Qt::BottomLeftCorner  , Qt::LeftDockWidgetArea);

  // ............................................

  messageDock = new MessageDock(this);

    // initial projects directory model
    a_homeDirModel = new QucsFileSystemModel(this);
    a_proxyModel = new QucsSortFilterProxyModel();
    //a_proxyModel->setDynamicSortFilter(true);
    // show all directories (project and non-project)
    a_homeDirModel->setFilter(QDir::NoDot | QDir::AllDirs);

    // ............................................
    QString path = QucsSettings.qucsWorkspaceDir.absolutePath();
    QDir ProjDir(path);
    // initial projects directory is the Qucs home directory
    QucsSettings.projsDir.setPath(path);

    // create home dir if not exist
    if(!ProjDir.exists()) {
        if(!ProjDir.mkdir(path)) {
            QMessageBox::warning(this, tr("Warning"),
                                 tr("Cannot create work directory !"));
            return;
        }
    }

  // ............................................
  readProjects(); // reads all projects and inserts them into the ListBox
}

// Put all available libraries into ComboBox.
void QucsApp::fillLibrariesTreeView ()
{
    QList<QTreeWidgetItem *> topitems;

    libTreeWidget->clear();

    // make the system libraries section header
    QTreeWidgetItem* newitem = new QTreeWidgetItem((QTreeWidget*)0, QStringList("System Libraries"));
    newitem->setChildIndicatorPolicy (QTreeWidgetItem::DontShowIndicator);
    QFont sectionFont = newitem->font(0);
    sectionFont.setItalic (true);
    sectionFont.setBold (true);
    newitem->setFont (0, sectionFont);
//    newitem->setBackground
    topitems.append (newitem);

    populateLibTreeFromDir(QucsSettings.LibDir, topitems, true);

    // make the user libraries section header
    newitem = new QTreeWidgetItem((QTreeWidget*)0, QStringList("User Libraries"));
    newitem->setChildIndicatorPolicy (QTreeWidgetItem::DontShowIndicator);
    newitem->setFont (0, sectionFont);
    topitems.append (newitem);

    QString UserLibDirPath = QucsSettings.qucsWorkspaceDir.canonicalPath () + "/user_lib/";
    populateLibTreeFromDir(UserLibDirPath, topitems);

    // make the user libraries section header
    newitem = new QTreeWidgetItem((QTreeWidget*)0, QStringList("Project Libraries"));
    newitem->setChildIndicatorPolicy (QTreeWidgetItem::DontShowIndicator);
    newitem->setFont (0, sectionFont);
    topitems.append (newitem);
    if (!ProjName.isEmpty()) {
        populateLibTreeFromDir(QucsSettings.QucsWorkDir.absolutePath(), topitems, true);
    }

    libTreeWidget->insertTopLevelItems(0, topitems);
}


bool QucsApp::populateLibTreeFromDir(const QString &LibDirPath, QList<QTreeWidgetItem *> &topitems, bool relpath)
{
    QDir LibDir(LibDirPath);
    QStringList LibFiles = LibDir.entryList(QStringList("*.lib"), QDir::Files, QDir::Name);
    QStringList blacklist = getBlacklistedLibraries(QucsSettings.LibDir);
    for (const QString& ss: blacklist) { // exclude blacklisted files
        LibFiles.removeAll(ss);
    }
    // create top level library items, base on the library names
    for(auto it = LibFiles.begin(); it != LibFiles.end(); it++)
    {
        QString libPath(LibDir.absoluteFilePath(*it));
        libPath.chop(4); // remove extension

        ComponentLibrary parsedlibrary;

        int result = parseComponentLibrary (libPath , parsedlibrary, QUCS_COMP_LIB_FULL, relpath);
        QStringList nameAndFileName;
        nameAndFileName.append (parsedlibrary.name);
        nameAndFileName.append (LibDirPath + *it);

        QTreeWidgetItem* newlibitem = new QTreeWidgetItem((QTreeWidget*)nullptr, nameAndFileName);

        switch (result)
        {
            case QUCS_COMP_LIB_IO_ERROR:
            {
                QString filename = getLibAbsPath(libPath);
                QMessageBox::critical(nullptr, tr ("Error"), tr("Cannot open \"%1\".").arg (filename));
                return false;
            }
            case QUCS_COMP_LIB_CORRUPT:
                QMessageBox::critical(nullptr, tr("Error"), tr("Library is corrupt."));
                return false;
            default:
                break;
        }

        for (int i = 0; i < parsedlibrary.components.count (); i++)
        {
            QStringList compNameAndDefinition;

            compNameAndDefinition.append (parsedlibrary.components[i].name);

            QString s = "<Qucs Schematic " PACKAGE_VERSION ">\n";

            s +=  "<Components>\n  " +
                  parsedlibrary.components[i].modelString + "\n" +
                  "</Components>\n";

            compNameAndDefinition.append (s);
            compNameAndDefinition.append(parsedlibrary.components[i].definition);
            compNameAndDefinition.append(libPath);

            QTreeWidgetItem* newcompitem = new QTreeWidgetItem(newlibitem, compNameAndDefinition);

            // Silence warning from the compiler about unused variable newcompitem
            // we pass the pointer to the parent item in the constructor
            Q_UNUSED( newcompitem )
        }

        topitems.append (newlibitem);
    }
    return true;
}

// ---------------------------------------------------------------
// Returns a pointer to the QucsDoc object whose number is "No".
// If No < 0 then a pointer to the current document is returned.
QucsDoc* QucsApp::getDoc(int No)
{
  QWidget *w;
  if(No < 0)
    w = DocumentTab->currentWidget();
  else
    w = DocumentTab->widget(No);

  if(w) {
    if(isTextDocument (w))
      return (QucsDoc*) ((TextDoc*)w);
    else
      return (QucsDoc*) ((Schematic*)w);
  }

  return 0;
}

// ---------------------------------------------------------------
// Returns a pointer to the QucsDoc object whose file name is "Name".
QucsDoc * QucsApp::findDoc (QString File, int * Pos)
{
  QucsDoc * d;
  int No = 0;
  File = QDir::toNativeSeparators (File);
  while ((d = getDoc (No++)) != 0)
    if (QDir::toNativeSeparators (d->getDocName()) == File) {
      if (Pos) *Pos = No - 1;
      return d;
    }
  return 0;
}

// ---------------------------------------------------------------
// Put the component groups into the ComboBox. It is possible to
// only put the paintings in it, because of "symbol painting mode".

// if setAll, add all categories to combobox
// if not, set just paintings (symbol painting mode)
// return stored index
int QucsApp::fillComboBox(bool setAll) {
    fillLibrariesTreeView(); // заполняем библиотеки
    //CompChoose->setMaxVisibleItems (13); // Increase this if you add items below.
    auto currentText = CompChoose->currentText();

    CompChoose->clear();
    CompSearch->clear(); // clear the search box, in case search was active...

    Module::unregisterModules();
    Module::registerModules();
    Module::registerDynamicComponents();
    int idx = 0;
    if (!setAll) {
        CompChoose->insertItem(CompChoose->count(), QObject::tr("paintings"));
    } else {
        QStringList cats = Category::getCategories();
        for (const QString &it: cats) {
            CompChoose->insertItem(CompChoose->count(), it);
        }
        idx = CompChoose->findText(currentText);
        //CompChoose->setCurrentIndex(idx == -1 ? 0 : idx);

        // Add XML-based devices
        QList<QString> StaticLibraries = LibraryComponents.keys();
        for (const QString &it: StaticLibraries) {
          CompChoose->insertItem(CompChoose->count(), it);
        }
    }
    return idx == -1 ? 0 : idx;
}

void QucsApp::fillSimulatorsComboBox() {

    simulatorsCombobox->clear();
    //simulatorsCombobox->addItem(spicecompat::getDefaultSimulatorName(spicecompat::simNotSpecified), 0);

    if (misc::simulatorExists(QucsSettings.NgspiceExecutable)) {
        QucsSettings.NgspiceExecutable = misc::unwrapExePath(QucsSettings.NgspiceExecutable);
        simulatorsCombobox->addItem(spicecompat::getDefaultSimulatorName(spicecompat::simNgspice), 1);
    }
    if (misc::simulatorExists(QucsSettings.XyceExecutable)) {
        QucsSettings.XyceExecutable = misc::unwrapExePath(QucsSettings.XyceExecutable);
        simulatorsCombobox->addItem(spicecompat::getDefaultSimulatorName(spicecompat::simXyce), 2);
    }
    if (misc::simulatorExists(QucsSettings.SpiceOpusExecutable)) {
        QucsSettings.SpiceOpusExecutable = misc::unwrapExePath(QucsSettings.SpiceOpusExecutable);
        simulatorsCombobox->addItem(spicecompat::getDefaultSimulatorName(spicecompat::simSpiceOpus), 4);
    }
    if (misc::simulatorExists(QucsSettings.Qucsator)) {
        QucsSettings.Qucsator = misc::unwrapExePath(QucsSettings.Qucsator);
        simulatorsCombobox->addItem(spicecompat::getDefaultSimulatorName(spicecompat::simQucsator), 8);
    }

    bool anySimulatorsFound = simulatorsCombobox->count() > 0;

    if (anySimulatorsFound) {
        QString current = spicecompat::getDefaultSimulatorName(QucsSettings.DefaultSimulator);
        int idx = simulatorsCombobox->findText(current);
        idx = idx < 0 ? 0 : idx;
        simulatorsCombobox->setCurrentIndex(idx);
        QucsSettings.DefaultSimulator = simulatorsCombobox->itemData(idx).toInt();
    } else {
        QucsSettings.DefaultSimulator = spicecompat::simNotSpecified;
    }

    simulate->setEnabled(anySimulatorsFound);
    simulatorsCombobox->setEnabled(anySimulatorsFound);
}


void QucsApp::slotChangeSimulator(int index) {
    int simu = spicecompat::simNotSpecified;
    int idx = simulatorsCombobox->itemData(index).toInt();
    switch (idx) {
        case 1:
            simu = spicecompat::simNgspice;
            break;
        case 2:
            simu = spicecompat::simXyce;
            break;
        case 4:
            simu = spicecompat::simSpiceOpus;
            break;
        case 8:
            simu = spicecompat::simQucsator;
            break;
        default:
            break;
    }

    if (QucsSettings.DefaultSimulator == simu) {
      return;
    }

    QucsSettings.DefaultSimulator = simu;
    saveApplSettings();

    int idx1 = fillComboBox(true);
    slotSetCompView(idx1);

    // Call update() to update subcircuit symbols in current Schematic document.
    // TextDoc has no viewport, it needs no update.
    QString tabType = DocumentTab->currentWidget()->metaObject()->className();

    if (tabType == "Schematic") {
        ((Q3ScrollView*)DocumentTab->currentWidget())->viewport()->update();
    }
    //SimulatorLabel->setText(spicecompat::getDefaultSimulatorName(QucsSettings.DefaultSimulator));
}

// ----------------------------------------------------------
// Whenever the Component Library ComboBox is changed, this slot fills the
// Component IconView with the appropriate components.
void QucsApp::slotSetCompView (int index)
{
  //qDebug() << "QucsApp::slotSetCompView(" << index << ")";

  editText->setHidden (true); // disable text edit of component property

  QList<Module *> Comps;
  if (CompChoose->count () <= 0) return;

  // was in "search mode" ?
  if (CompChoose->itemText(0) == tr("Search results")) {
    if (index == 0) // user selected "Search results" item
      return;
    CompChoose->removeItem(0);
    CompSearch->clear(); // clear the search box
    --index; // adjust requested index since item 0 was removed
  }

  while (CompComps->count() > 0) {
    QListWidgetItem * item = CompComps->takeItem(0);
    if (item != nullptr) {
      delete item;
    }
  }
  CompComps->clear ();   // clear the IconView

  // make sure the right index is selected
  //  (might have been called by a cleared search and not by user action)
  CompChoose->setCurrentIndex(index);
  int compIdx;
  QString item = CompChoose->itemText (index);
  int catIdx = Category::getModulesNr(item);

  Comps = Category::getModules(item);
  QString Name;
  pInfoFunc Infos = 0;

  // if something was registered dynamically, get and draw icons into dock
  if (item == QObject::tr("verilog-a user devices")) {

    compIdx = 0;
    QMapIterator<QString, QString> i(Module::vaComponents);
    while (i.hasNext()) {
      i.next();

      // default icon initially matches the module name
      //Name = i.key();

      // Just need path to bitmap, do not create an object
      QString vaBitmap;
      Component * c = (Component *)
              vacomponent::info (Name, vaBitmap, false, i.value());
      if (c) delete c;

      // check if icon exists, fall back to default
      QString iconPath = QucsSettings.QucsWorkDir.filePath(vaBitmap+".png");

      QFile iconFile(iconPath);
      QPixmap vaIcon;

      if(iconFile.exists())
      {
        // load bitmap defined on the JSON symbol file
        vaIcon = QPixmap(iconPath);
      }
      else
      {
        QMessageBox::information(this, tr("Info"),
                     tr("Default icon not found:\n %1.png").arg(vaBitmap));
        // default icon
        vaIcon = QPixmap(":/bitmaps/editdelete.png");
      }
      QListWidgetItem *icon = new QListWidgetItem(vaIcon, Name);
      icon->setToolTip(Name);
      icon->setData(Qt::UserRole + 1, catIdx);
      icon->setData(Qt::UserRole + 2, compIdx);
      CompComps->addItem(icon);
      compIdx++;
    }
  } else {
    // static components
    char * File;
    // Populate list of component bitmaps
    compIdx = 0;


     // New loading system
     // Given the library name, fill the list with the components
    QString Category = item; // e.g. "Lumped Components"
    QMap<QString, ComponentInfo> Components = LibraryComponents[Category]; // This is a map containing the information of the components of the category.

    QMapIterator<QString, ComponentInfo> map_it(Components);
    while (map_it.hasNext()) {
      map_it.next();
      QString ComponentName = map_it.key();
      qDebug() << "Component:" << ComponentName;

      ComponentInfo componentInfo = map_it.value();


             // Need to create the png file to display in the components palette.
             // This is done by loading the symbol descrption into a Component object
             // and then calling the paintIcon method to paint into a QPixMap.

             // The symbol description is written in a file whose path comes in the component info.

      Component C;
      QList<qucs::Line *>  Lines;
      QList<Port *>     Ports;
      QList<struct qucs::Arc *> Arcs;
      QList<qucs::Polyline *>  Polylines;
      // By default, take the first symbol
      QMap<QString, SymbolDescription>::const_iterator symbol_it = componentInfo.symbol.constBegin();
      SymbolDescription SymbolInfo = symbol_it.value();
      Component::loadSymbol(SymbolInfo, Ports, Lines, Arcs, Polylines);
      C.Ports = Ports;
      C.Lines = Lines;
      C.Arcs = Arcs;
      C.Polylines = Polylines;

      QPixmap *image = new QPixmap(128, 128);
      C.paintIcon(image);
      QIcon Icon(*image);

      QListWidgetItem *icon = new QListWidgetItem(ComponentName);

      icon->setIcon(Icon);
      icon->setToolTip(ComponentName);
      icon->setData(Qt::UserRole + 1, catIdx);
      icon->setData(Qt::UserRole + 2, compIdx);
      CompComps->addItem(icon);
    }


    // Old code below -> THIS LOADS SIMULATION BLOCKS
    QList<Module *>::const_iterator it;
    for (it = Comps.constBegin(); it != Comps.constEnd(); it++) {
      Infos = (*it)->info;
      if (Infos) {
        /// \todo warning: expression result unused, can we rewrite this?
        //(void) *((*it)->info) (Name, File, false);
        Component* c = (Component*)Infos(Name, File, true);
        if (c) delete c;
        QString icon_path = misc::getIconPath(QString (File));
        QListWidgetItem *icon = new QListWidgetItem(Name);
        if (QFileInfo::exists(icon_path)) {
            icon->setIcon(QPixmap(icon_path));
        } else {
            icon->setIcon(*(*it)->icon);
        }
        icon->setToolTip(Name);
        icon->setData(Qt::UserRole + 1, catIdx);
        icon->setData(Qt::UserRole + 2, compIdx);
        CompComps->addItem(icon);
      }
      compIdx++;
    }
  }
}


// ------------------------------------------------------------------
// When CompSearch is being edited, create a temp page show the
// search result
void QucsApp::slotSearchComponent(const QString &searchText)
{
  // qDebug() << "User search: " << searchText;
  CompComps->clear ();   // clear the IconView

  // not already in "search mode"
  if (CompChoose->itemText(0) != tr("Search results")) {
    ccCurIdx = CompChoose->currentIndex(); // remember current panel
    // insert "Search results" at the beginning, so that it is visible
    CompChoose->insertItem(-1, tr("Search results"));
    CompChoose->setCurrentIndex(0);
  }

  if (searchText.isEmpty()) {
    slotSetCompView(CompChoose->currentIndex());
  } else {
    CompChoose->setCurrentIndex(0); // make sure the "Search results" category is selected
    editText->setHidden (true); // disable text edit of component property

    //traverse all component and match searchText with name
    QString Name;
    char * File;
    QList<Module *> Comps;

    QStringList cats = Category::getCategories ();
    int catIdx = 0;
    for (const QString& it : cats) {
      // this will go also over the "verilog-a user devices" category, if present
      //   but since modules there have no 'info' function it won't handle them
      Comps = Category::getModules(it);
      QList<Module *>::const_iterator modit;
      int compIdx = 0;
      for (modit = Comps.constBegin(); modit != Comps.constEnd(); modit++) {
        if ((*modit)->info) {
          /// \todo warning: expression result unused, can we rewrite this?
          (void) *((*modit)->info) (Name, File, false);

          if((Name.indexOf(searchText, 0, Qt::CaseInsensitive)) != -1) {
            //match
            QString icon_path = misc::getIconPath(QString (File));
            QListWidgetItem *icon = new QListWidgetItem(Name);
            if (QFileInfo::exists(icon_path)) {
                icon->setIcon(QPixmap(icon_path));
            } else {
                icon->setIcon(*(*modit)->icon);
            }
            icon->setToolTip(it + ": " + Name);
            // add component category and module indexes to the icon
            icon->setData(Qt::UserRole + 1, catIdx);
            icon->setData(Qt::UserRole + 2, compIdx);
            CompComps->addItem(icon);
          }
        }
        compIdx++;
      }
      catIdx++;
    }
    // the "verilog-a user devices" is the last category, if present
    QMapIterator<QString, QString> i(Module::vaComponents);
    int compIdx = 0;
    while (i.hasNext()) {
      i.next();
      // Just need path to bitmap, do not create an object
      QString vaName, vaBitmap;
      vacomponent::info (vaName, vaBitmap, false, i.value());

      if((vaName.indexOf(searchText, 0, Qt::CaseInsensitive)) != -1) {
        //match

        // check if icon exists, fall back to default
        QString iconPath = QucsSettings.QucsWorkDir.filePath(vaBitmap+".png");

        QFile iconFile(iconPath);
        QPixmap vaIcon;

        if(iconFile.exists())
        {
          // load bitmap defined on the JSON symbol file
          vaIcon = QPixmap(iconPath);
        }
        else
        {
          // default icon
          vaIcon = QPixmap(":/bitmaps/editdelete.png");
        }

        // Add icon an name tag to dock
        QListWidgetItem *icon = new QListWidgetItem(vaIcon, vaName);
        icon->setToolTip(tr("verilog-a user devices") + ": " + vaName);
        // Verilog-A is the last category
        icon->setData(Qt::UserRole + 1, catIdx-1);
        icon->setData(Qt::UserRole + 2, compIdx);
        CompComps->addItem(icon);
      }
      compIdx++;
    }
  }
}

// ------------------------------------------------------------------
void QucsApp::slotSearchClear()
{
  // was in "search mode" ?
  if (CompChoose->itemText(0) == tr("Search results")) {
    CompChoose->removeItem(0); // remove the added "Search results" item
    CompSearch->clear();
    // go back to the panel selected before search started
    slotSetCompView(ccCurIdx);
    // the added "Search results" panel text will be removed by slotSetCompView()
  }
}

// ------------------------------------------------------------------
// Is called when the mouse is clicked within the Component QIconView.
void QucsApp::slotSelectComponent(QListWidgetItem *item)
{
  slotHideEdit(); // disable text edit of component property

  // delete previously selected elements
  if(view->selElem != 0)  delete view->selElem;
  view->selElem  = 0;   // no component/diagram/painting selected

  if(item == 0) {   // mouse button pressed not over an item ?
    CompComps->clearSelection();  // deselect component in ViewList
    return;
  }

  //if(view->drawn)
  //  ((Q3ScrollView*)DocumentTab->currentWidget())->viewport()->update();
  //view->drawn = false;

  // toggle last toolbar button off
  if(activeAction) {
    activeAction->blockSignals(true); // do not call toggle slot
    activeAction->setChecked(false);       // set last toolbar button off
    activeAction->blockSignals(false);
  }
  activeAction = 0;

  MouseMoveAction = &MouseActions::MMoveElement;
  MousePressAction = &MouseActions::MPressElement;
  MouseReleaseAction = 0;
  MouseDoubleClickAction = 0;

  pInfoFunc Infos = 0;
  pInfoVAFunc InfosVA = 0;

  int i = CompComps->row(item);
  QList<Module *> Comps;

  // if symbol mode, only paintings are enabled.
  Comps = Category::getModules(CompChoose->currentText());

  QString name = CompComps->item(i)->text();
  QString CompName;
  QString CompFile_qstr;
  char *CompFile_cptr;

  qDebug() << "pressed CompComps id" << i << name;
  int catIdx =  CompComps->item(i)->data(Qt::UserRole + 1).toInt();
  int compIdx =  CompComps->item(i)->data(Qt::UserRole + 2).toInt();
  qDebug() << "slotSelectComponent()" << catIdx << compIdx;

    if (catIdx > -1){
      Category* cat = Category::Categories.at(catIdx);
      Module *mod = cat->Content.at(compIdx);
      qDebug() << "mod->info" << mod->info;
      qDebug() << "mod->infoVA" << mod->infoVA;
      Infos = mod->info;
      if (Infos) {
        // static component (LEGACY)
        view->selElem = (*mod->info) (CompName, CompFile_cptr, true);

      } else {
        // Verilog-A component
        InfosVA = mod->infoVA;
        // get JSON file out of item name on widgetitem
        QString filename = Module::vaComponents[name];
        if (InfosVA) {
          view->selElem = (*InfosVA) (CompName, CompFile_qstr, true, filename);
        }
      }
    } else {
      Component *C = new Component();
      // Load component data from LibraryComponents
      QString CategoryName = CompChoose->currentText();
      QString ComponentName = CompComps->item(i)->text();
      QMap<QString, ComponentInfo> Components = LibraryComponents[CategoryName];
      ComponentInfo CI = Components[ComponentName];

      C->loadfromComponentInfo(CI);
      view->selElem = C;
    }

  // in "search mode" ?
  if (CompChoose->itemText(0) == tr("Search results")) {
    if (Infos || InfosVA) {
      // change currently selected category, so the user will
      //   see where the component comes from
      CompChoose->setCurrentIndex(catIdx+1); // +1 due to the added "Search Results" item
      ccCurIdx = catIdx; // remember the category to select when exiting search
      //!! comment out the above two lines if you would like that the search
      //!!   returns back to the last selected category instead
    }
  }
}

// ####################################################################
// #####  Functions for the menu that appears when right-clicking #####
// #####  on a file in the "Content" ListView.                    #####
// ####################################################################

void QucsApp::initCursorMenu()
{
  ContentMenu = new QMenu(this);
#define APPEND_MENU(action, slot, text) \
  { \
  action = new QAction(tr(text), ContentMenu); \
  connect(action, SIGNAL(triggered()), SLOT(slot())); \
  ContentMenu->addAction(action); \
  }

  APPEND_MENU(ActionCMenuOpen, slotCMenuOpen, "Open")
  APPEND_MENU(ActionCMenuCopy, slotCMenuCopy, "Duplicate")
  APPEND_MENU(ActionCMenuRename, slotCMenuRename, "Rename")
  APPEND_MENU(ActionCMenuDelete, slotCMenuDelete, "Delete")
  APPEND_MENU(ActionCMenuInsert, slotCMenuInsert, "Insert")

#undef APPEND_MENU
  connect(Content, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(slotShowContentMenu(const QPoint&)));
}

// ----------------------------------------------------------
// Shows the menu.
void QucsApp::slotShowContentMenu(const QPoint& pos)
{
  QModelIndex idx = Content->indexAt(pos);
  if (idx.isValid() && idx.parent().isValid()) {
    ActionCMenuInsert->setVisible(
        idx.sibling(idx.row(), 1).data().toString().contains(tr("-port"))
    );
    ContentMenu->popup(Content->mapToGlobal(pos));
  }
}

// ----------------------------------------------------------
QString QucsApp::fileType (const QString& Ext)
{
  QString Type = tr("unknown");
  if (Ext == "v")
    Type = tr("Verilog source");
  else if (Ext == "va")
    Type = tr("Verilog-A source");
  else if (Ext == "vhd" || Ext == "vhdl")
    Type = tr("VHDL source");
  else if (Ext == "dat")
    Type = tr("data file");
  else if (Ext == "dpl")
    Type = tr("data display");
  else if (Ext == "sch")
    Type = tr("schematic");
  else if (Ext == "sym")
    Type = tr("symbol");
  else if (Ext == "vhdl.cfg" || Ext == "vhd.cfg")
    Type = tr("VHDL configuration");
  else if (Ext == "cfg")
    Type = tr("configuration");
  return Type;
}

void QucsApp::slotCMenuOpen()
{
  slotOpenContent(Content->currentIndex());
}

void QucsApp::slotCMenuCopy()
{
  QModelIndex idx = Content->currentIndex();

  //test the item is valid
  if (!idx.isValid() || !idx.parent().isValid()) { return; }

  QString filename = idx.sibling(idx.row(), 0).data().toString();
  QDir dir(QucsSettings.QucsWorkDir);
  QString file(dir.filePath(filename));
  QFileInfo fileinfo(file);

  //check changed file save
  int z = 0; //search if the doc is loaded
  QucsDoc *d = findDoc(file, &z);
  if (d != NULL && d->getDocChanged()) {
    DocumentTab->setCurrentIndex(z);
    int ret = QMessageBox::question(this, tr("Copying Qucs document"),
        tr("The document contains unsaved changes!\n") +
        tr("Do you want to save the changes before copying?"),
        QMessageBox::Ignore|QMessageBox::Save);
    if (ret == QMessageBox::Save) {
      d->save();
    }
  }

  QString suffix = fileinfo.suffix();
  QString base = fileinfo.completeBaseName();
  if(base.isEmpty()) {
    base = filename;
  }

  bool exists = true;   //generate unique name
  int i = 0;
  QString defaultName;
  while (exists) {
    ++i;
    defaultName = base + "_copy" + QString::number(i) + "." + suffix;
    exists = QFile::exists(dir.filePath(defaultName));
  }

  bool ok;
  QString s = QInputDialog::getText(this, tr("Copy file"), tr("Enter new name:"), QLineEdit::Normal, defaultName, &ok);

  if(ok && !s.isEmpty()) {
    if (!s.endsWith(suffix)) {
      s += QStringLiteral(".") + suffix;
    }

    if (QFile::exists(dir.filePath(s))) {  //check New Name exists
      QMessageBox::critical(this, tr("error"), tr("Cannot copy file to identical name: %1").arg(filename));
      return;
    }

    if (!QFile::copy(dir.filePath(filename), dir.filePath(s))) {
      QMessageBox::critical(this, tr("Error"), tr("Cannot copy schematic: %1").arg(filename));
      return;
    }
    //TODO: maybe require disable edit here

    // refresh the schematic file path
    //this->updateSchNameHash();
    //this->updateSpiceNameHash();

    slotUpdateTreeview();
  }
}

void QucsApp::slotCMenuRename()
{
  QModelIndex idx = Content->currentIndex();

  //test the item is valid
  if (!idx.isValid() || !idx.parent().isValid()) { return; }

  QString filename = idx.sibling(idx.row(), 0).data().toString();
  QString file(QucsSettings.QucsWorkDir.filePath(filename));
  QFileInfo fileinfo(file);

  if (findDoc(file)) {
    QMessageBox::critical(this, tr("Error"),
        tr("Cannot rename an open file!"));
    return;
  }

  QString suffix = fileinfo.suffix();
  QString base = fileinfo.completeBaseName();
  if(base.isEmpty()) {
    base = filename;
  }

  bool ok;
  QString s = QInputDialog::getText(this, tr("Rename file"), tr("Enter new filename:"), QLineEdit::Normal, base, &ok);

  if(ok && !s.isEmpty()) {
    if (!s.endsWith(suffix)) {
      s += QStringLiteral(".") + suffix;
    }
    QDir dir(QucsSettings.QucsWorkDir.path());
    if(!dir.rename(filename, s)) {
      QMessageBox::critical(this, tr("Error"), tr("Cannot rename file: %1").arg(filename));
      return;
    }

    slotUpdateTreeview();
  }
}

void QucsApp::slotCMenuDelete()
{
  QModelIndex idx = Content->currentIndex();

  //test the item is valid
  if (!idx.isValid() || !idx.parent().isValid()) { return; }

  QString filename = idx.sibling(idx.row(), 0).data().toString();
  QString file(QucsSettings.QucsWorkDir.filePath(filename));

  if (findDoc (file)) {
    QMessageBox::critical(this, tr("Error"), tr("Cannot delete an open file!"));
    return;
  }

  int No;
  No = QMessageBox::warning(this, tr("Warning"),
      tr("This will delete the file permanently! Continue ?"),
      QMessageBox::No | QMessageBox::Yes);
  if(No == QMessageBox::Yes) {
    if(!QFile::remove(file)) {
      QMessageBox::critical(this, tr("Error"),
      tr("Cannot delete file: %1").arg(filename));
      return;
    }
  }

  slotUpdateTreeview();
}

void QucsApp::slotCMenuInsert()
{
  slotSelectSubcircuit(Content->currentIndex());
}

// ################################################################
// #####    Functions that handle the project operations.     #####
// ################################################################

// Checks for qucs directory and reads all existing Qucs projects.
void QucsApp::readProjects()
{
    QString path = QucsSettings.projsDir.absolutePath();
    QString homepath = QucsSettings.qucsWorkspaceDir.absolutePath();

    if (path == homepath) {
        // in Qucs Home, disallow further up in the dirs tree
        a_homeDirModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    } else {
        a_homeDirModel->setFilter(QDir::NoDot | QDir::AllDirs);
    }

    // set the root path
    QModelIndex rootModelIndex = a_homeDirModel->setRootPath(path);
    // assign the model to the proxy and the proxy to the view
    a_proxyModel->setSourceModel(a_homeDirModel);
    a_proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    // sort by first column (file name, only column show in the QListView)
    a_proxyModel->sort(0);
    Projects->setModel(a_proxyModel);
    // fix the listview on the root path of the model
    Projects->setRootIndex(a_proxyModel->mapFromSource(rootModelIndex));
}

// ----------------------------------------------------------
// Is called, when "Create New Project" button is pressed.
void QucsApp::slotButtonProjNew()
{
  slotHideEdit(); // disable text edit of component property

  NewProjDialog *d = new NewProjDialog(this);
  if(d->exec() != QDialog::Accepted) return;

  QDir projDir(QucsSettings.projsDir.path());
  QString name = d->ProjName->text();
  bool open = d->OpenProj->isChecked();

  if (!name.endsWith("_prj")) {
    name += "_prj";
  }

  if(!projDir.mkdir(name)) {
    QMessageBox::information(this, tr("Info"),
        tr("Cannot create project directory !"));
  }
  if(open) {
    openProject(QucsSettings.projsDir.filePath(name));
  }
}

// ----------------------------------------------------------
// Opens an existing project.
void QucsApp::openProject(const QString& Path)
{
  slotHideEdit(); // disable text edit of component property

  QDir ProjDir(QDir::cleanPath(Path)); // the full path
  QString openProjName = ProjDir.dirName(); // only the project directory name

  if(!ProjDir.exists() || !ProjDir.isReadable()) { // check project directory
    QMessageBox::critical(this, tr("Error"),
                          tr("Cannot access project directory: %1").arg(Path));
    return;
  }

  if (!openProjName.endsWith("_prj")) { // should not happen
    QMessageBox::critical(this, tr("Error"),
                          tr("Project directory name does not end in '_prj'(%1)").arg(openProjName));
    return;
  }

  if(!closeAllFiles()) return;   // close files and ask for saving them
  Schematic *d = new Schematic(this, "");
  int i = addDocumentTab(d);
  DocumentTab->setCurrentIndex(i);

  slotResetWarnings();

  QucsSettings.QucsWorkDir.setPath(ProjDir.path());
  octave->adjustDirectory();

  Content->setProjPath(QucsSettings.QucsWorkDir.absolutePath());

  TabView->setCurrentIndex(1);   // switch to "Content"-Tab

  openProjName.chop(4); // remove "_prj" from name
  ProjName = openProjName;   // remember the name of project
  QDir parentDir = QucsSettings.QucsWorkDir;
  parentDir.cdUp();
    // show name in title of main window
  setWindowTitle( tr("Project: ") + ProjName + " (" +  parentDir.absolutePath() + ") - " + windowTitle);
  fillLibrariesTreeView();
}

// ----------------------------------------------------------
// Is called when the open project menu is called.
void QucsApp::slotMenuProjOpen()
{
  QString d = QFileDialog::getExistingDirectory(
      this, tr("Choose Project Directory for Opening"),
      QucsSettings.qucsWorkspaceDir.path(),
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
  if(d.isEmpty()) return;

  openProject(d);
}

// ----------------------------------------------------------
// Is called, when "Open Project" button is pressed.
void QucsApp::slotButtonProjOpen()
{
  slotHideEdit();

  QModelIndex idx = Projects->currentIndex();
  if (!idx.isValid()) {
    QMessageBox::information(this, tr("Info"),
      tr("No project is selected !"));
  } else {
    slotListProjOpen(idx);
  }
}

// ----------------------------------------------------------
// Is called when project is double-clicked to open it.
void QucsApp::slotListProjOpen(const QModelIndex &idx)
{
    QString dName = idx.data().toString();
    if (dName.endsWith("_prj")) { // it's a Qucs project
        openProject(QucsSettings.projsDir.filePath(dName));
    } else { // it's a normal directory
        // change projects directory to the selected one
        QucsSettings.projsDir.setPath(QucsSettings.projsDir.filePath(dName));
        readProjects();
        //repaint();
    }
}

// ----------------------------------------------------------
// Is called when the close project menu is called.
void QucsApp::slotMenuProjClose()
{
  slotHideEdit(); // disable text edit of component property

  if(!closeAllFiles()) return;   // close files and ask for saving them
  Schematic *d = new Schematic(this, "");
  int i = addDocumentTab(d);
  DocumentTab->setCurrentIndex(i);

  slotResetWarnings();
  setWindowTitle(windowTitle);
  QucsSettings.QucsWorkDir.setPath(QucsSettings.qucsWorkspaceDir.absolutePath());
  octave->adjustDirectory();

  Content->setProjPath("");

  TabView->setCurrentIndex(0);   // switch to "Projects"-Tab
  ProjName = "";
  fillLibrariesTreeView();
}

// remove a directory recursively
bool QucsApp::recurRemove(const QString &Path)
{
  QDir projDir = QDir(Path);
  return projDir.removeRecursively();
}

// ----------------------------------------------------------
bool QucsApp::deleteProject(const QString& Path)
{
  slotHideEdit();

  if(Path.isEmpty()) return false;

  QString delProjName = QDir(Path).dirName(); // only project directory name

  if (!delProjName.endsWith("_prj")) { // should not happen
    QMessageBox::critical(this, tr("Error"),
                          tr("Project directory name does not end in '_prj' (%1)").arg(delProjName));
    return false;
  }

  delProjName.chop(4); // remove "_prj" from name

  if(delProjName == ProjName) {
    QMessageBox::information(this, tr("Info"),
        tr("Cannot delete an open project !"));
    return false;
  }

  // first ask, if really delete project ?
  if(QMessageBox::warning(this, tr("Warning"),
      tr("This will destroy all the project files permanently ! Continue ?"),
      QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)  return false;

  if (!recurRemove(Path)) {
    QMessageBox::information(this, tr("Info"),
        tr("Cannot remove project directory!"));
    return false;
  }
  return true;
}

// ----------------------------------------------------------
// Is called, when "Delete Project" menu is activated.
void QucsApp::slotMenuProjDel()
{
  QString d = QFileDialog::getExistingDirectory(
      this, tr("Choose Project Directory for Deleting"),
      QucsSettings.qucsWorkspaceDir.path(),
      QFileDialog::ShowDirsOnly
      | QFileDialog::DontResolveSymlinks);

  deleteProject(d);
}

// ----------------------------------------------------------
// Is called, when "Delete Project" button is pressed.
void QucsApp::slotButtonProjDel()
{
  QModelIndex idx = Projects->currentIndex();
  if(!idx.isValid()) {
    QMessageBox::information(this, tr("Info"),
        tr("No project is selected!"));
    return;
  }

  deleteProject(QucsSettings.projsDir.filePath(idx.data().toString()));
}


// ################################################################
// #####  Functions that handle the file operations for the   #####
// #####  documents.                                          #####
// ################################################################

void QucsApp::slotFileNew()
{
  statusBar()->showMessage(tr("Creating new schematic..."));
  slotHideEdit(); // disable text edit of component property

  Schematic *d = new Schematic(this, "");
  int i = addDocumentTab(d);
  DocumentTab->setCurrentIndex(i);

  statusBar()->showMessage(tr("Ready."));
}

void QucsApp::slotSymbolNew()
{
  statusBar()->showMessage(tr("Creating new schematic..."));
  slotHideEdit(); // disable text edit of component property

  Schematic *d = new Schematic(this, "");
  int i = addDocumentTab(d);
  DocumentTab->setCurrentIndex(i);
  slotSymbolEdit();
  d->setIsSymbolOnly(true);
  statusBar()->showMessage(tr("Ready."));
}

// --------------------------------------------------------------
void QucsApp::slotTextNew()
{
  statusBar()->showMessage(tr("Creating new text editor..."));
  slotHideEdit(); // disable text edit of component property
  TextDoc *d = new TextDoc(this, "");
  int i = addDocumentTab(d);
  DocumentTab->setCurrentIndex(i);

  statusBar()->showMessage(tr("Ready."));
}

// --------------------------------------------------------------
// Changes to the document "Name". If already open then it goes to it
// directly, otherwise it loads it.
bool QucsApp::gotoPage(const QString& Name)
{
  int No = DocumentTab->currentIndex();

  int i = 0;
  QucsDoc * d = findDoc (Name, &i);  // search, if page is already loaded

  if(d) {   // open page found ?
    d->becomeCurrent(true);
    DocumentTab->setCurrentIndex(i);  // make new document the current
    return true;
  }

  QFileInfo Info(Name);
  bool is_sch = false;
  if(Info.suffix() == "sch" || Info.suffix() == "dpl" ||
     Info.suffix() == "sym") {
    d = new Schematic(this, Name);
    i = addDocumentTab((Schematic *)d, Info.fileName());
    is_sch = true;
  }
  else {
    d = new TextDoc(this, Name);
    i = addDocumentTab((TextDoc *)d, Info.fileName());
  }
  DocumentTab->setCurrentIndex(i);

  if (!Info.isWritable()) {
      QMessageBox::warning(this,tr("Open file"),
                           tr("Document opened in read-only mode! "
                           "Simulation will not work. Please copy the document "
                           "to the directory where you have write permission!"));
      statusBar()->showMessage("Read only");
  }

  if(!d->load()) {    // load document if possible
    delete d;
    DocumentTab->setCurrentIndex(No);
    return false;
  }
  slotChangeView();
  if (Info.suffix() == "sym") {
    // We dealing with a file containing *only* a symbol definition.
    // Because of that we want to switch straight to symbol editing mode
    // and skip any actions performed with a usual schematic.
    Schematic *sch = (Schematic *)d;
    slotSymbolEdit();
    sch->setIsSymbolOnly(true);
  } else if (is_sch) {
      Schematic *sch = (Schematic *)d;
      if (sch->checkDplAndDatNames()) sch->setChanged(true,true);
  }

  // if only an untitled document was open -> close it
  if(getDoc(0)->getDocName().isEmpty())
    if(!getDoc(0)->getDocChanged())
      delete DocumentTab->widget(0);

  return true;
}

QString lastDirOpenSave; // to remember last directory and file

// --------------------------------------------------------------
void QucsApp::slotFileOpen()
{
  slotHideEdit(); // disable text edit of component property

  statusBar()->showMessage(tr("Opening file..."));

  QString s = QFileDialog::getOpenFileName(this, tr("Enter a Schematic Name"),
    lastDirOpenSave.isEmpty() ? QString(QDir::homePath()) : lastDirOpenSave, QucsFileFilter);

  if(s.isEmpty())
    statusBar()->showMessage(tr("Opening aborted"), 2000);
  else {
    updateRecentFilesList(s);

    gotoPage(s);
    lastDirOpenSave = s;   // remember last directory and file

    statusBar()->showMessage(tr("Ready."));
  }
}

// --------------------------------------------------------------
bool QucsApp::saveFile(QucsDoc *Doc)
{
  if(!Doc)
    Doc = getDoc();

  if(Doc->getDocName().isEmpty())
    return saveAs();

  int Result = Doc->save();
  if(Result < 0)  return false;

  // It's assumed that *.sym files contain *only* a symbol
  // definition. We don't want these files to be subject
  // of any activities or "optimizations" which may change
  // the symbol.
  if (!Doc->getDocName().endsWith(".sym")) {
    updatePortNumber(Doc, Result);
  }
  slotUpdateTreeview();
  return true;
}

// --------------------------------------------------------------
void QucsApp::slotFileSave()
{
  statusBar()->showMessage(tr("Saving file..."));
  DocumentTab->blockSignals(true);   // no user interaction during that time
  slotHideEdit(); // disable text edit of component property

  if(!saveFile()) {
    DocumentTab->blockSignals(false);
    statusBar()->showMessage(tr("Saving aborted"), 2000);
    statusBar()->showMessage(tr("Ready."));
    return;
  }

  DocumentTab->blockSignals(false);
  statusBar()->showMessage(tr("Ready."));

  if(!ProjName.isEmpty())
    slotUpdateTreeview();
}

// --------------------------------------------------------------
bool QucsApp::saveAs()
{
  QWidget *w = DocumentTab->currentWidget();
  QucsDoc *Doc = getDoc();

  int n = -1;
  QString s, Filter, selfilter;
  QStringList Filters;
  QFileInfo Info;
  while(true) {
    QString file_ext;
    s = Doc->getDocName();
    Info.setFile(s);
    if(s.isEmpty()) {   // which is default directory ?
      if(ProjName.isEmpty()) {
        if(lastDirOpenSave.isEmpty())  s = QDir::homePath();
        else  s = lastDirOpenSave;
      }
      else s = QucsSettings.QucsWorkDir.path();
    } else {
      file_ext = QString("*.") + Info.suffix();
    }

    // list of known file extensions
    QString ext = "vhdl;vhd;v;va;sch;dpl;m;oct;net;qnet;ckt;cir;sp;txt;sym";
    QStringList extlist = ext.split (';');

    if(isTextDocument (w)) {
      Filters << tr("VHDL Sources")+" (*.vhdl *.vhd);;"
              << tr("Verilog Sources")+" (*.v);;"
              << tr("Verilog-A Sources")+" (*.va);;"
              << tr("Octave Scripts")+" (*.m *.oct);;"
              << tr("Qucs Netlist")+" (*.net *.qnet);;"
              << tr("SPICE Netlist")+" (*.ckt *.cir *.sp);;"
              << tr("Plain Text")+" (*.txt);;"
              << tr("Any File")+" (*)";
      Filter = Filters.join("");
      bool found = false;
      for (const auto &ss: Filters) {
        if (ss.contains(file_ext)) {
          found = true;
          selfilter = ss;
          selfilter.chop(2);
          break;
        }
      }
      if (!found) {
        selfilter = Filters.first();
      }
    } else {
      Schematic *sch = (Schematic *) Doc;
      if (sch->getIsSymbolOnly()) {
        Filter = tr("Subcircuit symbol") + "(*.sym)";
        selfilter = tr("Subcircuit symbol") + "(*.sym)";
      } else {
        Filter = QucsFileFilter;
        selfilter = tr("Schematic") + " (*.sch)";
      }
    }

    s = QFileDialog::getSaveFileName(this, tr("Enter a Document Name"),
                                     s, Filter, &selfilter);
    if(s.isEmpty())  return false;
    Info.setFile(s);               // try to guess the best extension ...
    ext = Info.suffix();

    if(ext.isEmpty() || !extlist.contains(ext))
    {
      // if no extension was specified or is unknown
      if (!isTextDocument (w))
      {
        // assume it is a schematic
        s += ".sch";
      }
    }

    Info.setFile(s);
    if(QFile::exists(s)) {
      n = QMessageBox::warning(this, tr("Warning"),
      tr("The file '")+Info.fileName()+tr("' already exists!\n")+
      tr("Saving will overwrite the old one! Continue?"),
        QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,QMessageBox::Cancel);
      if(n == QMessageBox::Cancel) return false;    // cancel
      if(n == QMessageBox::No) continue;
    }

    // search, if document is open
    QucsDoc * d = findDoc (s);
    if(d) {
      QMessageBox::information(this, tr("Info"),
      tr("Cannot overwrite an open document"));
      return false;
    }

    break;
  }
  Doc->setName(s);
  DocumentTab->setTabText(DocumentTab->indexOf(w), misc::properFileName(s));
  lastDirOpenSave = Info.absolutePath();  // remember last directory and file

  n = Doc->save();   // SAVE
  if(n < 0)  return false;

  // It's assumed that *.sym files contain *only* a symbol
  // definition. We don't want these files to be subject
  // of any activities or "optimizations" which may change
  // the symbol.
  if (!Doc->getDocName().endsWith(".sym")) {
    updatePortNumber(Doc, n);
  }

  slotUpdateTreeview();
  updateRecentFilesList(s);
  return true;
}

// --------------------------------------------------------------
void QucsApp::slotFileSaveAs()
{
  statusBar()->showMessage(tr("Saving file under new filename..."));
  DocumentTab->blockSignals(true);   // no user interaction during the time
  slotHideEdit(); // disable text edit of component property

  if(!saveAs()) {
    DocumentTab->blockSignals(false);
    statusBar()->showMessage(tr("Saving aborted"), 3000);
    statusBar()->showMessage(tr("Ready."));
    return;
  }

  DocumentTab->blockSignals(false);
  statusBar()->showMessage(tr("Ready."));

  // refresh the schematic file path
  slotRefreshSchPath();

  if(!ProjName.isEmpty())
    slotUpdateTreeview();
}


// --------------------------------------------------------------
void QucsApp::slotFileSaveAll()
{
  statusBar()->showMessage(tr("Saving all files..."));
  slotHideEdit(); // disable text edit of component property
  DocumentTab->blockSignals(true);   // no user interaction during the time

  int No=0;
  QucsDoc *Doc;  // search, if page is already loaded
  while((Doc=getDoc(No++)) != 0) {
    if(Doc->getDocName().isEmpty())  // make document the current ?
      DocumentTab->setCurrentIndex(No-1);
    if (saveFile(Doc)) { // Hack! TODO: Maybe it's better to let slotFileChanged()
      setDocumentTabChanged(No-1, false); // know about Tab number?
    }
  }

  DocumentTab->blockSignals(false);
  // Call update() to update subcircuit symbols in current Schematic document.
  // TextDoc has no viewport, it needs no update.
  QString tabType = DocumentTab->currentWidget()->metaObject()->className();

  if (tabType == "Schematic") {
    ((Q3ScrollView*)DocumentTab->currentWidget())->viewport()->update();
  }
  statusBar()->showMessage(tr("Ready."));

  // refresh the schematic file path
  slotRefreshSchPath();
  slotUpdateTreeview();
}

// --------------------------------------------------------------
// Close the currently active file tab
void QucsApp::slotFileClose()
{
    // Using file index -1 closes the currently active file window
    closeFile(-1);
}

// --------------------------------------------------------------
// Close the file tab specified by its index
void QucsApp::slotFileClose(int index)
{
    // Call closeFile with a specific tab index
    closeFile(index);
    // Reset Tunerdialog
    if (TuningMode) tunerDia->slotResetTunerDialog();
}

// --------------------------------------------------------------
// Common function to close a file tab specified by its index
// checking for changes in the file before doing so. If called
// index == -1, the active document will be closed
void QucsApp::closeFile(int index)
{
    statusBar()->showMessage(tr("Closing file..."));

    slotHideEdit(); // disable text edit of component property

    QucsDoc *Doc = getDoc(index);
    if(Doc->getDocChanged()) {
      switch(QMessageBox::warning(this,tr("Closing Qucs document"),
        tr("The document contains unsaved changes!\n")+
        tr("Do you want to save the changes before closing?"),
        QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel)) {
        case QMessageBox::Save : slotFileSave();
                 break;
        case QMessageBox::Cancel : return;
        default: break;
      }
    }

    DocumentTab->removeTab(index);
    delete Doc;

    if(DocumentTab->count() < 1) { // if no document left, create an untitled
      Schematic *d = new Schematic(this, "");
      addDocumentTab(d);
      DocumentTab->setCurrentIndex(0);
    }

    statusBar()->showMessage(tr("Ready."));
}


// --------------------------------------------------------------
bool QucsApp::closeAllFiles()
{
  SaveDialog *sd = new SaveDialog(this);
  sd->setApp(this);
  for(int i=0; i < DocumentTab->count(); ++i) {
    QucsDoc *doc = getDoc(i);
    if(doc->getDocChanged())
      sd->addUnsavedDoc(doc);
  }
  int Result = SaveDialog::DontSave;
  if(!sd->isEmpty())
     Result = sd->exec();
  delete sd;
  if(Result == SaveDialog::AbortClosing)
    return false;
  QucsDoc *doc = 0;
  while((doc = getDoc()) != 0)
  delete doc;


  //switchEditMode(true);   // set schematic edit mode
  return true;
}

void QucsApp::slotFileExamples() {
  statusBar()->showMessage(tr("Open example…"));

  auto exampleFile =
      QFileDialog::getOpenFileName(this, tr("Select example schematic"),
                                   QucsSettings.ExamplesDir, QucsFileFilter);

  if (exampleFile.isEmpty()) {
      statusBar()->showMessage(tr("Open example canceled"), 2000);
      return;
  }

  gotoPage(exampleFile);
  statusBar()->showMessage(tr("Ready."));
}

void QucsApp::slotHelpTutorial()
{
  // pass the QUrl representation of a local file
  QUrl url = QUrl::fromLocalFile(QDir::cleanPath(QucsSettings.DocDir + "/tutorial/" + QObject::sender()->objectName()));
  QDesktopServices::openUrl(url);
}

void QucsApp::slotHelpTechnical()
{
  // pass the QUrl representation of a local file
  QUrl url = QUrl::fromLocalFile(QDir::cleanPath(QucsSettings.DocDir + "/technical/" + QObject::sender()->objectName()));
  QDesktopServices::openUrl(url);
}

void QucsApp::slotHelpReport()
{
  // pass the QUrl representation of a local file
  QUrl url = QUrl::fromLocalFile(QDir::cleanPath(QucsSettings.DocDir + "/report/" + QObject::sender()->objectName()));
  QDesktopServices::openUrl(url);
}



// --------------------------------------------------------------
// Is called when another document is selected via the TabBar.
void QucsApp::slotChangeView()
{
  QWidget *w = DocumentTab->currentWidget();
  editText->setHidden (true); // disable text edit of component property
  QucsDoc * Doc;
  if(w==NULL)return;
  // for text documents
  if (isTextDocument (w)) {
    TextDoc *d = (TextDoc*)w;
    Doc = (QucsDoc*)d;
    // update menu entries, etc. if necessary
    magAll->setDisabled(true);
    magSel->setDisabled(true);
    if(cursorLeft->isEnabled())
      switchSchematicDoc (false);
  }
  // for schematic documents
  else {
    Schematic *d = (Schematic*)w;
    Doc = (QucsDoc*)d;
    magAll->setDisabled(false);
    magSel->setDisabled(false);
    // already in schematic?
    if(cursorLeft->isEnabled()) {
      // which mode: schematic or symbol editor ?
      if((CompChoose->count() > 1) == d->getSymbolMode())
        changeSchematicSymbolMode (d);
    }
    else {
      switchSchematicDoc(true);
      changeSchematicSymbolMode(d);
    }

    showGrid->setChecked(d->getGridOn());
  }

  Doc->becomeCurrent(true);

//  TODO proper window title
//  QFileInfo Info (Doc-> getDocName());
//
//  if (!ProjName.isEmpty()) {
//    QDir parentDir = QucsSettings.QucsWorkDir;
//    parentDir.cdUp();
//    setWindowTitle(tr("Project: ") + ProjName + " (" + parentDir.absolutePath() + ") "
//                   + windowTitle);
//  } else {
//    setWindowTitle(Info.fileName() + " (" + Info.filePath() +") - " + windowTitle);
//  }

  HierarchyHistory.clear();
  popH->setEnabled(false);
}

// --------------------------------------------------------------
void QucsApp::slotFileSettings ()
{
  editText->setHidden (true); // disable text edit of component property

  QWidget * w = DocumentTab->currentWidget ();
  if (isTextDocument (w)) {
    QucsDoc * Doc = (QucsDoc *) ((TextDoc *) w);
    QString ext = Doc->fileSuffix ();
    // Octave properties
    if (ext == "m" || ext == "oct") {
    }
    // Verilog-A properties
    else if (ext == "va") {
      VASettingsDialog * d = new VASettingsDialog ((TextDoc *) w);
      d->exec ();
    }
    // VHDL and Verilog-HDL properties
    else {
      DigiSettingsDialog * d = new DigiSettingsDialog ((TextDoc *) w);
      d->exec ();
    }
  }
  // schematic properties
  else {
    SettingsDialog * d = new SettingsDialog ((Schematic *) w);
    d->exec ();

    // TODO: It would be better to emit a signal to notify all subscribers
    // that the diagram settings have changed.
    showGrid->setChecked(static_cast<Schematic*>(w)->getGridOn());
  }
}

// --------------------------------------------------------------
void QucsApp::slotApplSettings()
{
  slotHideEdit(); // disable text edit of component property

  QucsSettingsDialog *d = new QucsSettingsDialog(this);
  d->exec();
}

// --------------------------------------------------------------
void QucsApp::slotRefreshSchPath()
{
  //this->updateSchNameHash();
  //this->updateSpiceNameHash();

  statusBar()->showMessage(tr("The schematic search path has been refreshed."), 2000);
}

// --------------------------------------------------------------
void QucsApp::updatePortNumber(QucsDoc *currDoc, int No)
{
  if(No<0) return;

  QString pathName = currDoc->getDocName();
  QString ext = currDoc->fileSuffix ();
  QFileInfo Info (pathName);
  QString Model, File, Name = Info.fileName();

  if (ext == "sch") {
    Model = "Sub";
  }
  else if (ext == "vhdl" || ext == "vhd") {
    Model = "VHDL";
  }
  else if (ext == "v") {
    Model = "Verilog";
  }

  // update all occurencies of subcircuit in all open documents
  No = 0;
  QWidget *w;
  Component *pc_tmp;
  while((w=DocumentTab->widget(No++)) != 0) {
    if(isTextDocument (w))  continue;

    // start from the last to omit re-appended components
    Schematic *Doc = (Schematic*)w;
    for(Component *pc=Doc->a_Components->last(); pc!=0; ) {
      if(pc->Model == Model) {
        File = pc->Props.front()->Value;
        if((File == pathName) || (File == Name)) {
          pc_tmp = Doc->a_Components->prev();
          Doc->recreateComponent(pc);  // delete and re-append component
          if(!pc_tmp)  break;
          Doc->a_Components->findRef(pc_tmp);
          pc = Doc->a_Components->current();
          continue;
        }
      }
      pc = Doc->a_Components->prev();
    }
  }
}

// --------------------------------------------------------------
int QucsApp::addDocumentTab(QFrame* widget, const QString& title)
{
  int index = DocumentTab->addTab(widget, title.isEmpty() ? tr("untitled") : title);
#if __APPLE__
  widget->setFrameStyle(QFrame::NoFrame);
  QTabBar* tabBar = DocumentTab->tabBar();
  QLabel* modifiedLabel = new QLabel(" ", tabBar);
  modifiedLabel->setFixedWidth(10);
  tabBar->setTabButton(index, QTabBar::RightSide, modifiedLabel);
#endif
  return index;
}

// --------------------------------------------------------------
void QucsApp::setDocumentTabChanged(int index, bool changed)
{
#ifdef __APPLE__
  ((QLabel *)DocumentTab->tabBar()->tabButton(index, QTabBar::RightSide))->setText(changed ? "\u26AB" : " ");
#else
    if (changed) {
        DocumentTab->setTabIcon(index,QIcon(":bitmaps/svg/filesave.svg"));
    } else {
        DocumentTab->setTabIcon(index,QIcon());
    }
#endif
}

// --------------------------------------------------------------
// printCurrentDocument: call printerwriter to print document
void QucsApp::printCurrentDocument(bool fitToPage)
{
  statusBar()->showMessage(tr("Printing..."));
  slotHideEdit(); // disable text edit of component property

  PrinterWriter *writer = new PrinterWriter();
  writer->setFitToPage(fitToPage);
  writer->print(DocumentTab->currentWidget());
  delete writer;

  statusBar()->showMessage(tr("Ready."));
  return;
}

// --------------------------------------------------------------
void QucsApp::slotFilePrint()
{
  printCurrentDocument(false);
}

// --------------------------------------------------------------
// Fit printed content to page size.
void QucsApp::slotFilePrintFit()
{
  printCurrentDocument(true);
}

// --------------------------------------------------------------------
// Exits the application.
void QucsApp::slotFileQuit()
{
  statusBar()->showMessage(tr("Exiting application..."));
  slotHideEdit(); // disable text edit of component property

  saveSettings();
  if(closeAllFiles()) {
    emit signalKillEmAll();   // kill all subprocesses
    qApp->quit();
  }

  statusBar()->showMessage(tr("Ready."));
}

//-----------------------------------------------------------------
// To get all close events.
void QucsApp::closeEvent(QCloseEvent* Event)
{
   saveSettings();
   if(closeAllFiles()) {
      emit signalKillEmAll();   // kill all subprocesses
      Event->accept();
      qApp->quit();
   }
   else
      Event->ignore();
}

//-----------------------------------------------------------------
// Saves settings
void QucsApp::saveSettings()
{
  saveApplSettings();
}

// --------------------------------------------------------------------
// Is called when the toolbar button is pressed to go into a subcircuit.
void QucsApp::slotIntoHierarchy()
{
  slotHideEdit(); // disable text edit of component property

  Schematic *Doc = (Schematic*)DocumentTab->currentWidget();
  Component *pc = Doc->searchSelSubcircuit();
  if(pc == 0) { return; }

  QString s = pc->getSubcircuitFile();
  if(!gotoPage(s)) { return; }

  HierarchyHistory.push(Doc->getDocName()); //remember for the way back
  popH->setEnabled(true);
}

// --------------------------------------------------------------------
// Is called when the toolbar button is pressed to leave a subcircuit.
void QucsApp::slotPopHierarchy()
{
  slotHideEdit(); // disable text edit of component property

  if(HierarchyHistory.size() == 0) return;

  QString Doc = HierarchyHistory.pop();

  if(!gotoPage(Doc)) {
    HierarchyHistory.push(Doc);
    return;
  }

  if(HierarchyHistory.size() == 0) {
    popH->setEnabled(false);
  }
}

// --------------------------------------------------------------
void QucsApp::slotShowAll()
{
  slotHideEdit(); // disable text edit of component property
  getDoc()->showAll();
}

// --------------------------------------------------------------
void QucsApp::slotZoomToSelection()
{
    slotHideEdit(); // disable text edit of component property
    getDoc()->zoomToSelection();
}

// -----------------------------------------------------------
// Sets the scale factor to 1.
void QucsApp::slotShowOne()
{
  slotHideEdit(); // disable text edit of component property
  getDoc()->showNoZoom();
}

// -----------------------------------------------------------
void QucsApp::slotZoomOut()
{
  slotHideEdit(); // disable text edit of component property
  getDoc()->zoomBy(0.5f);
}

/*!
 * \brief QucsApp::slotTune
 *  is called when the tune toolbar button is pressed.
 */
void QucsApp::slotTune(bool checked)
{
    if (checked)
    {
        QWidget *w = DocumentTab->currentWidget(); // remember from which Tab the tuner was started
        if (isTextDocument(w))
        {
            //Probably digital Simulation
            QMessageBox::warning(this, "Not implemented",
                                 "Currently tuning is not supported for this document type", QMessageBox::Ok);
            tune->blockSignals(true);
            tune->setChecked(false);
            tune->blockSignals(false);
            return;
        }

        Schematic* d(dynamic_cast<Schematic*>(w));
        assert(d);

        bool found = false;
        bool digi_found = false;
        bool exit = false;
        for(Component *pc = d->a_DocComps.first(); pc != 0; pc = d->a_DocComps.next()) {
            if (pc->isSimulation) {
                found = true;
            }
            if (pc->Type == isDigitalComponent) {
                digi_found = true;
                exit = true;
            }
        }
        if (!found) {
            QMessageBox::warning(this,tr("Error"),tr("No simulations found. Tuning not possible."
                                                     " Please add at least one simulation."));
            exit = true;
        }
        if (digi_found) {
            QMessageBox::warning(this,tr("Error"),tr("Tuning not possible for digital simulation. "
                                                     "Only analog simulation supported."));
        }
        if (d->a_Diagrams->isEmpty() && !d->getSimOpenDpl()) {
            QMessageBox::warning(this,tr("Error"),tr("Tuning has no effect without diagrams. "
                                                     "Add at least one diagram on schematic."));
            exit = true;
        }

        if (exit) {
            tune->blockSignals(true);
            tune->setChecked(false);
            tune->blockSignals(false);
            return;
        }


        // instance of tuner
        TuningMode = true;
        tunerDia = new TunerDialog(w, this);//The object can be instantiated here since when checked == false the memory will be freed
        // inform the Tuner Dialog when a component is deleted
        connect(d, SIGNAL(signalComponentDeleted(Component *)),
                tunerDia, SLOT(slotComponentDeleted(Component *)));

        slotHideEdit(); // disable text edit of component property
        simulateToolbar->setEnabled(false); // disable workToolbar to preserve TuneMouseAction

        MousePressAction = &MouseActions::MPressTune;
        MouseReleaseAction = 0; //While Tune is active release is not needed. This puts Press Action back to normal select

        tunerDia->show();
    }
    else
    {
        this->simulateToolbar->setEnabled(true);

        // MouseActions are reset in closing of tunerDialog class
        tunerDia->close();//According to QWidget documentation (http://doc.qt.io/qt-4.8/qwidget.html#close),
                          //the object is removed since it has the Qt::WA_DeleteOnClose flag
        TuningMode = false;
    }
}


QWidget *QucsApp::getSchematicWidget(QucsDoc *Doc)
{
    QWidget *w = nullptr;
    QFileInfo Info(QucsSettings.QucsWorkDir.filePath(Doc->getDataDisplay()));
    int z = 0;
    QFileInfo sch_inf(Doc->getDocName());
    QString sch_name = sch_inf.absolutePath() + QDir::separator() + Doc->getDataDisplay();
    QucsDoc *d = findDoc(sch_name, &z);  // check if schematic is already open in a Tab

    if (d)
    {
        // schematic already loaded
        // this should be the simulation schematic of this data display
        w = DocumentTab->widget(z);
    }
    else
    {
        // schematic not yet loaded
        int i = 0;
        int No = DocumentTab->currentIndex(); // remember current Tab
        if(Info.suffix() == "sch" || Info.suffix() == "dpl" ||
           Info.suffix() == "sym") {
          d = new Schematic(this, Info.absoluteFilePath());
          i = DocumentTab->addTab((Schematic *)d, QPixmap(":/bitmaps/empty.xpm"), Info.fileName());
        } else {
          d = new TextDoc(this, Info.absoluteFilePath());
          i = DocumentTab->addTab((TextDoc *)d, QPixmap(":/bitmaps/empty.xpm"), Info.fileName());
        }
        DocumentTab->setCurrentIndex(i); // temporarily switch to the newly created Tab

        if(d->load()) {
          // document loaded successfully
          w = DocumentTab->widget(i);
        } else {
          // failed loading document
          // load() above has already shown a QMessageBox about not being able to load the file
          delete d;
          DocumentTab->setCurrentIndex(No);
        }
        DocumentTab->setCurrentIndex(No);
    }
    return w;
}

/*!
 * \brief QucsApp::slotSimulate
 *  is called when the simulate toolbar button is pressed.
*/
void QucsApp::slotSimulate(QWidget *w)
{

  if (w == nullptr)
      w = DocumentTab->currentWidget();

  //Check is schematic digital
  bool isDigital = false;
  if (!isTextDocument(w)) {
      Schematic* schematicPtr = (Schematic*)w;
      isDigital = schematicPtr->isDigitalCircuit();

      if (isDigital && schematicPtr->getShowBias() == 0) {
          QMessageBox::warning(this,tr("Simulate schematic"),
                                    tr("DC bias simulation mode is not supported "
                                       "for digital schematic!"));
          return;
      }
  }

  if (QucsSettings.DefaultSimulator!=spicecompat::simQucsator && !isDigital) {
      slotSimulateWithSpice();
      return;
  }

  slotHideEdit(); // disable text edit of component property

  QucsDoc *Doc;

  if(isTextDocument (w)) {
    Doc = (QucsDoc*)((TextDoc*)w);
    if(Doc->getSimTime().isEmpty() && ((TextDoc*)Doc)->simulation) {
      DigiSettingsDialog *d = new DigiSettingsDialog((TextDoc*)Doc);
      if(d->exec() == QDialog::Rejected)
        return;
    }
  }
  else
    Doc = (QucsDoc*)((Schematic*)w);

  if(Doc->getDocName().isEmpty()) // if document 'untitled' ...
    if(!saveAs()) return;    // ... save schematic before

  // Perhaps the document was modified from another program ?
  QFileInfo Info(Doc->getDocName());
  QString ext = Info.suffix();
  if(Doc->getLastSaved().isValid()) {
    if(Doc->getLastSaved() < Info.lastModified()) {
      int No = QMessageBox::warning(this, tr("Warning"),
               tr("The document was modified by another program !") + '\n' +
               tr("Do you want to reload or keep this version ?"),
               QMessageBox::Yes|QMessageBox::No);
      if(No == QMessageBox::Yes)
        Doc->load();
    }
  }

  slotResetWarnings();

  if(Info.suffix() == "m" || Info.suffix() == "oct") {
    // It is an Octave script.
    if(Doc->getDocChanged())
      Doc->save();
    slotViewOctaveDock(true);
    octave->runOctaveScript(Doc->getDocName());
    return;
  }

  if (ext == "dpl") {
      // simulation started from Data Display: open referenced schematic
      w = getSchematicWidget(Doc);
  }

  SimMessage *sim = new SimMessage(w, this);
  sim->setDocWidget(w);
  // disconnect is automatically performed, if one of the involved objects
  // is destroyed !
  connect(sim, SIGNAL(SimulationEnded(int, SimMessage*)), this,
    SLOT(slotAfterSimulation(int, SimMessage*)));
  connect(sim, SIGNAL(displayDataPage(QString&, QString&)),
    this, SLOT(slotChangePage(QString&, QString&)));

  if (TuningMode == true) {
      connect(sim, SIGNAL(progressBarChanged(int)), tunerDia, SLOT(slotUpdateProgressBar(int)));
  } else { //It doesn't make sense to connect the slot outside the tuning mode
      sim->show();
  }

  if(!sim->startProcess()) {
      if (TuningMode == true) sim->show();//The message window is hidden when the tuning mode is active, but in case of error such window pops up
      return;
  }

  // to kill it before qucs ends
  connect(this, SIGNAL(signalKillEmAll()), sim, SLOT(slotClose()));
}

// ------------------------------------------------------------------------
// Is called after the simulation process terminates.
void QucsApp::slotAfterSimulation(int Status, SimMessage *sim)
{

  if(Status != 0) { // errors ocurred ?
      if (TuningMode) {
          sim->show();
          tunerDia->SimulationEnded();
      }
      return;
  }

  if(sim->ErrText->document()->lineCount() > 1)   // were there warnings ?
    slotShowWarnings();

  int i=0;
  QWidget *w;  // search, if page is still open
  while((w=DocumentTab->widget(i++)) != 0)
    if(w == sim->DocWidget)
      break;

  if(sim->showBias == 0) {  // paint dc bias into schematic ?
    sim->slotClose();   // close and delete simulation window
    if(w) {  // schematic still open ?
      SweepDialog *Dia = new SweepDialog((Schematic*)sim->DocWidget);

      // silence warning about unused variable.
      Q_UNUSED(Dia)
    }
  }
  else {
    if(sim->SimRunScript) {
      // run script
      octave->startOctave();
      octave->runOctaveScript(sim->Script);
    }
    if(sim->SimOpenDpl) {
      // switch to data display
      if(sim->DataDisplay.right(2) == ".m" ||
        sim->DataDisplay.right(4) == ".oct") {  // Is it an Octave script?
        octave->startOctave();
        octave->runOctaveScript(sim->DataDisplay);
      }
      else
        slotChangePage(sim->DocName, sim->DataDisplay);
      //sim->slotClose();   // close and delete simulation window
    }
    else {
        if(w) {
            if(!isTextDocument (sim->DocWidget)) {
                int idx = Category::getModulesNr (QObject::tr("diagrams"));
                CompChoose->setCurrentIndex(idx);   // switch to diagrams
                slotSetCompView (idx);
                // load recent simulation data (if document is still open)
                ((Schematic*)sim->DocWidget)->reloadGraphs();
            }
        }
    }
  }

  if(!isTextDocument (sim->DocWidget)) {
    //((Schematic*)sim->DocWidget)->viewport()->update();
    ((Schematic*)DocumentTab->currentWidget())->viewport()->update();
  }

  // Kill the simulation process, otherwise we have 200+++ sims in the background
  if(TuningMode) {
    sim->slotClose();
    tunerDia->SimulationEnded();
  }

}

// ------------------------------------------------------------------------
void QucsApp::slotDCbias()
{
  getDoc()->setShowBias(0);
  slotSimulate();
}

// ------------------------------------------------------------------------
// Changes to the corresponding data display page or vice versa.
void QucsApp::slotChangePage(const QString& DocName, const QString& DataDisplay)
{
  if(DataDisplay.isEmpty())  return;

  QFileInfo Info(DocName);
  QString Name = Info.path() + QDir::separator() + DataDisplay;

  QWidget  *w = DocumentTab->currentWidget();

  int z = 0;  // search, if page is already loaded
  QucsDoc * d = findDoc (Name, &z);

  if(d)
    DocumentTab->setCurrentIndex(z);
  else {   // no open page found ?
    QString ext = QucsDoc::fileSuffix (DataDisplay);

    int i = 0;
    if (ext != "vhd" && ext != "vhdl" && ext != "v" && ext != "va" &&
      ext != "oct" && ext != "m") {
      d = new Schematic(this, Name);
      i = addDocumentTab((Schematic *)d, DataDisplay);
    }
    else {
      d = new TextDoc(this, Name);
      i = addDocumentTab((TextDoc *)d, DataDisplay);
    }
    DocumentTab->setCurrentIndex(i);

    QFile file(Name);
    if(file.open(QIODevice::ReadOnly)) {      // try to load document
      file.close();
      if(!d->load()) {
        delete d;
        return;
      }
    }
    else {
      if(file.open(QIODevice::ReadWrite)) {  // if document doesn't exist, create
        d->setDataDisplay(Info.fileName());
        slotUpdateTreeview();
      }
      else {
        QMessageBox::critical(this, tr("Error"), tr("Cannot create ")+Name);
        return;
      }
      file.close();
    }

    d->becomeCurrent(true);
  }


  if(DocumentTab->currentWidget() == w)      // if page not ...
    if(!isTextDocument (w))
      ((Schematic*)w)->reloadGraphs();  // ... changes, reload here !

  TabView->setCurrentIndex(2);   // switch to "Component"-Tab
  if (Name.right(4) == ".dpl") {
    int i = Category::getModulesNr (QObject::tr("diagrams"));
    CompChoose->setCurrentIndex(i);   // switch to diagrams
    slotSetCompView (i);
  }
}

// -------------------------------------------------------------------
// Changes to the data display of current page.
void QucsApp::slotToPage()
{
  QucsDoc *d = getDoc();
  if(d->getDataDisplay().isEmpty()) {
    QMessageBox::critical(this, tr("Error"), tr("No page set !"));
    return;
  }

  if(d->getDocName().right(2) == ".m" ||
     d->getDocName().right(4) == ".oct")
    slotViewOctaveDock(true);
  else
    slotChangePage(d->getDocName(), d->getDataDisplay());
}

// -------------------------------------------------------------------
// Called when file name in Project View is double-clicked
//   or open is selected in the context menu
void QucsApp::slotOpenContent(const QModelIndex &idx)
{
  editText->setHidden(true); // disable text edit of component property

  //test the item is valid
  if (!idx.isValid()) { return; }
  if (!idx.parent().isValid()) { return; }

  QString filename = idx.sibling(idx.row(), 0).data().toString();
  QString note = idx.sibling(idx.row(), 1).data().toString();
  QFileInfo Info(QucsSettings.QucsWorkDir.filePath(filename));
  QString extName = Info.suffix();

  if (extName == "sch" || extName == "dpl" || extName == "vhdl" ||
      extName == "vhd" || extName == "v" || extName == "va" ||
      extName == "m" || extName == "oct") {
    gotoPage(Info.absoluteFilePath());
    updateRecentFilesList(Info.absoluteFilePath());
    slotUpdateRecentFiles();

    if(note.isEmpty())     // is subcircuit ?
      if(extName == "sch") return;

    select->blockSignals(true);  // switch on the 'select' action ...
    select->setChecked(true);
    select->blockSignals(false);

    activeAction = select;
    MouseMoveAction = 0;
    MousePressAction = &MouseActions::MPressSelect;
    MouseReleaseAction = &MouseActions::MReleaseSelect;
    MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
    return;
  }

  if(extName == "dat") {
    editFile(Info.absoluteFilePath());  // open datasets with text editor
    return;
  }

  // File is no Qucs file, so go through list and search a user
  // defined program to open it.
  QStringList com;
  QStringList::const_iterator it = QucsSettings.FileTypes.constBegin();
  while(it != QucsSettings.FileTypes.constEnd()) {
    if(extName == (*it).section('/',0,0)) {
      QString progName = (*it).section('/',1,1);
      com = progName.split(" ");
      com << Info.absoluteFilePath();

      QProcess *Program = new QProcess();
      //Program->setCommunication(0);
      QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
      env.insert("PATH", env.value("PATH") );
      Program->setProcessEnvironment(env);
      QString cmd = com.at(0);
      QStringList com_args = com;
      com_args.removeAt(0);
      Program->start(cmd, com_args);
      if(Program->state()!=QProcess::Running&&
              Program->state()!=QProcess::Starting) {
        QMessageBox::critical(this, tr("Error"),
               tr("Cannot start \"%1\"!").arg(Info.absoluteFilePath()));
        delete Program;
      }
      return;
    }
    it++;
  }

  // If no appropriate program was found, open as text file.
  editFile(Info.absoluteFilePath());  // open datasets with text editor
}

// ---------------------------------------------------------
// Is called when the mouse is clicked within the Content QListView.
void QucsApp::slotSelectSubcircuit(const QModelIndex &idx)
{
  editText->setHidden(true); // disable text edit of component property

  if(!idx.isValid()) {   // mouse button pressed not over an item ?
    Content->clearSelection();  // deselect component in ListView
    return;
  }

  bool isVHDL = false;
  bool isVerilog = false;
  QModelIndex parentIdx = idx.parent();
  if(!parentIdx.isValid()) { return; }

  QString category = parentIdx.data().toString();

  if(category == tr("Schematics")) {
    if(idx.sibling(idx.row(), 1).data().toString().isEmpty())
      return;   // return, if not a subcircuit
  }
  else if(category == tr("VHDL"))
    isVHDL = true;
  else if(category == tr("Verilog"))
    isVerilog = true;
  else
    return;

  QString filename = idx.sibling(idx.row(), 0).data().toString();
  QString note = idx.sibling(idx.row(), 1).data().toString();
  int idx_pag = DocumentTab->currentIndex();
  QString tab_titl = "";
  if (idx_pag>=0) tab_titl = DocumentTab->tabText(idx_pag);
  if (filename == tab_titl ) return; // Forbid to paste subcircuit into itself.

  // delete previously selected elements
  if(view->selElem != 0)  delete view->selElem;
  view->selElem = 0;

  // toggle last toolbar button off
  if(activeAction) {
    activeAction->blockSignals(true); // do not call toggle slot
    activeAction->setChecked(false);       // set last toolbar button off
    activeAction->blockSignals(false);
  }
  activeAction = 0;

  Component *Comp;
  if(isVHDL)
    Comp = new VHDL_File();
  else if(isVerilog)
    Comp = new Verilog_File();
  else
    Comp = new Subcircuit();
  Comp->Props.first()->Value = idx.sibling(idx.row(), 0).data().toString();
  Comp->recreate(0);
  view->selElem = Comp;

  MouseMoveAction = &MouseActions::MMoveElement;
  MousePressAction = &MouseActions::MPressElement;
  MouseReleaseAction = 0;
  MouseDoubleClickAction = 0;
}

// ---------------------------------------------------------
// Is called when the mouse is clicked within the Content QListView.
void QucsApp::slotSelectLibComponent(QTreeWidgetItem *item)
{
    // get the current document
    Schematic *Doc = (Schematic*)DocumentTab->currentWidget();

    // if the current document is a schematic activate the paste
    if(!isTextDocument(Doc))
    {
        // if there's not a higher level item, this is a top level item,
        // not a component item so return
        if(item->parent() == 0) return;
        if(item->text(1).isEmpty()) return;   // return, if not a subcircuit

        // copy the subcircuit schematic to the clipboard
        QClipboard *cb = QApplication::clipboard();
        QString model = item->text(1);
        QString name = item->text(0);
        QString list = item->text(2);
        QString lib = item->text(3);
        cb->setText(item->text(1));
        qDebug()<<name<<lib;

        QString content;
        CompDescr->clear();

        CompDescr->setText("Name: " + name);
        CompDescr->append("Library: " + lib);
        CompDescr->append("----------------------------");
        if(!getSection("Description", list, content)) {
            QMessageBox::critical(this, tr("Error"), tr("Library is corrupt."));
            return;
        }
        CompDescr->append(content);
        CompDescr->moveCursor(QTextCursor::Start);

        if(getSection("Model", list, content)) {
            Symbol->ModelString = content;
        }

        if(getSection("Spice", list, content)) {
            Symbol->SpiceString = content;
        }

        if(getSection("VHDLModel", list, content)) {
            Symbol->VHDLModelString = content;
        }

        if(getSection("VerilogModel", list, content)) {
            Symbol->VerilogModelString = content;
        }

        if(!getSection("Symbol", list, content)) {
            QMessageBox::critical(this, tr("Error"), tr("Library is corrupt."));
            return;
        }
        Symbol->setSymbol(content,lib,name);

        // activate the paste command
        slotEditPaste (true);
    }

}


// ---------------------------------------------------------
// This function is called if the document type changes, i.e.
// from schematic to text document or vice versa.
void QucsApp::switchSchematicDoc (bool SchematicMode)
{
  // switch our scroll key actions on/off according to SchematicMode
  cursorLeft->setEnabled(SchematicMode);
  cursorRight->setEnabled(SchematicMode);
  cursorUp->setEnabled(SchematicMode);
  cursorDown->setEnabled(SchematicMode);

  // text document
  if (!SchematicMode) {
    if (activeAction) {
      activeAction->blockSignals (true); // do not call toggle slot
      activeAction->setChecked(false);       // set last toolbar button off
      activeAction->blockSignals (false);
    }
    activeAction = select;
    select->blockSignals (true);
    select->setChecked(true);
    select->blockSignals (false);
  }
  // schematic document
  else {
    MouseMoveAction = 0;
    MousePressAction = &MouseActions::MPressSelect;
    MouseReleaseAction = &MouseActions::MReleaseSelect;
    MouseDoubleClickAction = &MouseActions::MDoubleClickSelect;
  }

  selectMarker->setEnabled (SchematicMode);
  alignTop->setEnabled (SchematicMode);
  alignBottom->setEnabled (SchematicMode);
  alignLeft->setEnabled (SchematicMode);
  alignRight->setEnabled (SchematicMode);
  centerHor->setEnabled (SchematicMode);
  centerVert->setEnabled (SchematicMode);
  distrHor->setEnabled (SchematicMode);
  distrVert->setEnabled (SchematicMode);
  onGrid->setEnabled (SchematicMode);
  moveText->setEnabled (SchematicMode);
  filePrintFit->setEnabled (SchematicMode);
  editRotate->setEnabled (SchematicMode);
  editMirror->setEnabled (SchematicMode);
  editMirrorY->setEnabled (SchematicMode);
  intoH->setEnabled (SchematicMode);
  popH->setEnabled (SchematicMode);
  dcbias->setEnabled (SchematicMode);
  insWire->setEnabled (SchematicMode);
  insLabel->setEnabled (SchematicMode);
  insPort->setEnabled (SchematicMode);
  insGround->setEnabled (SchematicMode);
  insEquation->setEnabled (SchematicMode);
  setMarker->setEnabled (SchematicMode);
  setDiagramLimits->setEnabled (SchematicMode);

  exportAsImage->setEnabled (SchematicMode); // only export schematic, no text

  editFind->setEnabled (!SchematicMode);
  insEntity->setEnabled (!SchematicMode);

  buildModule->setEnabled(!SchematicMode); // only build if VA document
}

// ---------------------------------------------------------
void QucsApp::switchEditMode(bool SchematicMode)
{
  fillComboBox(SchematicMode);
  slotSetCompView(0);

  intoH->setEnabled(SchematicMode);
  popH->setEnabled(SchematicMode);
  editActivate->setEnabled(SchematicMode);
  changeProps->setEnabled(SchematicMode);
  insEquation->setEnabled(SchematicMode);
  insGround->setEnabled(SchematicMode);
  insWire->setEnabled(SchematicMode);
  insLabel->setEnabled(SchematicMode);
  setMarker->setEnabled(SchematicMode);
  setDiagramLimits->setEnabled(SchematicMode);
  selectMarker->setEnabled(SchematicMode);
  simulate->setEnabled(SchematicMode);
  // no search in "symbol painting mode" as only paintings should be used
  CompSearch->setEnabled(SchematicMode);
  CompSearchClear->setEnabled(SchematicMode);
}

// ---------------------------------------------------------
void QucsApp::changeSchematicSymbolMode(Schematic *Doc)
{
  if(Doc->getSymbolMode()) {
    // go into select modus to avoid placing a forbidden element
    select->setChecked(true);

    switchEditMode(false);
  }
  else
    switchEditMode(true);
}

// ---------------------------------------------------------
bool QucsApp::isTextDocument(QWidget *w) {
  return w->inherits("QPlainTextEdit");
}

// ---------------------------------------------------------
// Is called if the "symEdit" action is activated, i.e. if the user
// switches between the two painting mode: Schematic and (subcircuit)
// symbol.
void QucsApp::slotSymbolEdit()
{
  QWidget *w = DocumentTab->currentWidget();

  // in a text document (e.g. VHDL)
  if (isTextDocument (w)) {
    TextDoc *TDoc = (TextDoc*)w;
    if (!TDoc->getDocName().endsWith(".va")) {
        QMessageBox::warning(this,tr("Error"),
                tr("Symbol editing supported only for schematics and Verilog-A documents!"));
        return;
    } else {
      QMessageBox::warning(this,tr("Warning"),
                tr("Attaching symbols to Verilog-A sources is deprecated and not recommended "
                   "for new designs. Use SPICE generic device instead. See the documentation "
                   "for more details."));
    }
    // set 'DataDisplay' document of text file to symbol file
    QFileInfo Info(TDoc->getDocName());
    QString sym = Info.completeBaseName()+".sym";
    TDoc->setDataDisplay(sym);

    // symbol file already loaded?
    int paint_mode = 0;
    if (!findDoc (QucsSettings.QucsWorkDir.filePath(sym)))
      paint_mode = 1;

    // change current page to appropriate symbol file
    slotChangePage(TDoc->getDocName(),TDoc->getDataDisplay());

    // set 'DataDisplay' document of symbol file to original text file
    Schematic *SDoc = (Schematic*)DocumentTab->currentWidget();
    SDoc->setDataDisplay(Info.fileName());

    // change into symbol mode
    if (paint_mode) // but only switch coordinates if newly loaded
      SDoc->switchPaintMode();
    SDoc->setSymbolMode(true);
    changeSchematicSymbolMode(SDoc);
    SDoc->becomeCurrent(true);
    SDoc->viewport()->update();
  }
  // in a normal schematic, symbol file
  else {
    Schematic *SDoc = (Schematic*)w;
    if (!SDoc->getIsSymbolOnly()) {
      slotHideEdit(); // disable text edit of component property
      SDoc->switchPaintMode();   // twist the view coordinates
      changeSchematicSymbolMode(SDoc);
      SDoc->becomeCurrent(true);
      SDoc->viewport()->update();
    }
  }
}

// -----------------------------------------------------------
void QucsApp::slotPowerMatching()
{
  QWidget *w = DocumentTab->currentWidget(); // remember from which Tab the tuner was started
  if (isTextDocument(w)) return;
  if(!view->focusElement) return;
  if(view->focusElement->Type != isMarker) return;
  Marker *pm = (Marker*)view->focusElement;

//  double Z0 = 50.0;
  QString Var = pm->pGraph->Var;
  double Imag = pm->powImag();
  if(Var == "Sopt")  // noise matching ?
    Imag *= -1.0;

  MatchDialog *Dia = new MatchDialog(this);
//  Dia->Ref1Edit->setText(QString::number(Z0));
  Dia->setS11LineEdits(pm->powReal(), Imag);
  Dia->setFrequency(pm->powFreq());
  Dia->setTwoPortMatch(false); // will also cause the corresponding impedance LineEdit to be updated

  Schematic *sch = dynamic_cast<Schematic*>(w);
  if (sch->getSimOpenDpl() || sch->getDocName().endsWith(".dpl")) {
      slotToPage();
  }
  if(Dia->exec() != QDialog::Accepted)
    return;
}

// -----------------------------------------------------------
void QucsApp::slot2PortMatching()
{
  QWidget *w = DocumentTab->currentWidget(); // remember from which Tab the tuner was started
  if (isTextDocument(w)) return;
  if(!view->focusElement) return;
  if(view->focusElement->Type != isMarker) return;
  Marker *pm = (Marker*)view->focusElement;

  QString DataSet;
  Schematic *Doc = (Schematic*)DocumentTab->currentWidget();
  int z = pm->pGraph->Var.indexOf(':');
  if(z <= 0)  DataSet = Doc->getDataSet();
  else  DataSet = pm->pGraph->Var.mid(z+1);
  double Freq = pm->powFreq();

  QFileInfo Info(Doc->getDocName());
  DataSet = Info.path()+QDir::separator()+DataSet;

  Diagram *Diag = new Diagram();

  // FIXME: use normal Diagrams.
  Graph *pg = new Graph(Diag, "S[1,1]");
  Diag->Graphs.append(pg);
  if(!pg->loadDatFile(DataSet)) {
    QMessageBox::critical(0, tr("Error"), tr("Could not load S[1,1]."));
    return;
  }

  pg = new Graph(Diag, "S[1,2]");
  Diag->Graphs.append(pg);
  if(!pg->loadDatFile(DataSet)) {
    QMessageBox::critical(0, tr("Error"), tr("Could not load S[1,2]."));
    return;
  }

  pg = new Graph(Diag, "S[2,1]");
  Diag->Graphs.append(pg);
  if(!pg->loadDatFile(DataSet)) {
    QMessageBox::critical(0, tr("Error"), tr("Could not load S[2,1]."));
    return;
  }

  pg = new Graph(Diag, "S[2,2]");
  Diag->Graphs.append(pg);
  if(!pg->loadDatFile(DataSet)) {
    QMessageBox::critical(0, tr("Error"), tr("Could not load S[2,2]."));
    return;
  }

  DataX const *Data = Diag->Graphs.first()->axis(0);
  if(Data->Var != "frequency") {
    QMessageBox::critical(0, tr("Error"), tr("Wrong dependency!"));
    return;
  }

  double *Value = Data->Points;
  // search for values for chosen frequency
  for(z=0; z<Data->count; z++)
    if(*(Value++) == Freq) break;

  // get S-parameters
  double S11real = *(Diag->Graphs.at(0)->cPointsY + 2*z);
  double S11imag = *(Diag->Graphs.at(0)->cPointsY + 2*z + 1);
  double S12real = *(Diag->Graphs.at(1)->cPointsY + 2*z);
  double S12imag = *(Diag->Graphs.at(1)->cPointsY + 2*z + 1);
  double S21real = *(Diag->Graphs.at(2)->cPointsY + 2*z);
  double S21imag = *(Diag->Graphs.at(2)->cPointsY + 2*z + 1);
  double S22real = *(Diag->Graphs.at(3)->cPointsY + 2*z);
  double S22imag = *(Diag->Graphs.at(3)->cPointsY + 2*z + 1);
  delete Diag;

  MatchDialog *Dia = new MatchDialog(this);
  Dia->setTwoPortMatch(true);
  Dia->setFrequency(Freq);
  Dia->setS11LineEdits(S11real, S11imag);
  Dia->setS12LineEdits(S12real, S12imag);
  Dia->setS21LineEdits(S21real, S21imag);
  Dia->setS22LineEdits(S22real, S22imag);

  Schematic *sch = dynamic_cast<Schematic*>(w);
  if (sch->getSimOpenDpl() || sch->getDocName().endsWith(".dpl")) {
      slotToPage();
  }
  if(Dia->exec() != QDialog::Accepted)
    return;
}

// -----------------------------------------------------------
// Is called if the "edit" action is clicked on right mouse button menu.
void QucsApp::slotEditElement()
{
  if(view->focusMEvent)
    view->editElement((Schematic*)DocumentTab->currentWidget(), view->focusMEvent);
}

// -----------------------------------------------------------
// Hides the edit for component property. Called e.g. if QLineEdit
// looses the focus.
void QucsApp::slotHideEdit()
{
  editText->setParent(this, Qt::WindowFlags());
  editText->setHidden(true);
}

// -----------------------------------------------------------
// set document tab icon to smallsave_xpm or empty_xpm
void QucsApp::slotFileChanged(bool changed)
{
  setDocumentTabChanged(DocumentTab->currentIndex(), changed);
}

// -----------------------------------------------------------
// Update project view by call refresh function
// looses the focus.
void QucsApp::slotUpdateTreeview()
{
  Content->refresh();
}
/*
// -----------------------------------------------------------
// Searches the qucs path list for all schematic files and creates
// a hash for lookup later
void QucsApp::updateSchNameHash(void)
{
    // update the list of paths to search in qucsPathList, this
    // removes nonexisting entries
    updatePathList();

    // now go through the paths creating a map to all the schematic files
    // found in the directories. Note that we go through the list of paths from
    // first index to last index. Since keys are unique it means schematic files
    // in directories at the end of the list take precedence over those at the
    // start of the list, we should warn about shadowing of schematic files in
    // this way in the future
    QStringList nameFilter;
    nameFilter << "*.sch";

    // clear out any existing hash table entries
    schNameHash.clear();

    for (const QString& qucspath : qucsPathList) {
        QDir thispath(qucspath);
        // get all the schematic files in the directory
        QFileInfoList schfilesList = thispath.entryInfoList( nameFilter, QDir::Files );
        // put each one in the hash table with the unique key the base name of
        // the file, note this will overwrite the value if the key already exists
        for (const QFileInfo& schfile : schfilesList) {
            schNameHash[schfile.completeBaseName()] = schfile.absoluteFilePath();
        }
    }

    // finally check the home/working directory
    QDir thispath(QucsSettings.QucsWorkDir);
    QFileInfoList schfilesList = thispath.entryInfoList( nameFilter, QDir::Files );
    // put each one in the hash table with the unique key the base name of
    // the file, note this will overwrite the value if the key already exists
    for (const QFileInfo& schfile : schfilesList) {
        schNameHash[schfile.completeBaseName()] = schfile.absoluteFilePath();
    }
}
*/
/*
// -----------------------------------------------------------
// Searches the qucs path list for all spice files and creates
// a hash for lookup later
void QucsApp::updateSpiceNameHash()
{
    // update the list of paths to search in qucsPathList, this
    // removes nonexisting entries
    updatePathList();

    // now go through the paths creating a map to all the spice files
    // found in the directories. Note that we go through the list of paths from
    // first index to last index. Since keys are unique it means spice files
    // in directories at the end of the list take precedence over those at the
    // start of the list, we should warn about shadowing of spice files in
    // this way in the future

    // clear out any existing hash table entries
    spiceNameHash.clear();

    for (const QString& qucspath : qucsPathList) {
        QDir thispath(qucspath);
        // get all the schematic files in the directory
        QFileInfoList spicefilesList = thispath.entryInfoList( QucsSettings.spiceExtensions, QDir::Files );
        // put each one in the hash table with the unique key the base name of
        // the file, note this will overwrite the value if the key already exists
        for (const QFileInfo& spicefile : spicefilesList) {
            schNameHash[spicefile.completeBaseName()] = spicefile.absoluteFilePath();
        }
    }

    // finally check the home/working directory
    QDir thispath(QucsSettings.QucsWorkDir);
    QFileInfoList spicefilesList = thispath.entryInfoList( QucsSettings.spiceExtensions, QDir::Files );
    // put each one in the hash table with the unique key the base name of
    // the file, note this will overwrite the value if the key already exists
    for (const QFileInfo& spicefile : spicefilesList) {
        spiceNameHash[spicefile.completeBaseName()] = spicefile.absoluteFilePath();
    }
}
*/
// -----------------------------------------------------------
// update the list of paths, pruning non-existing paths
void QucsApp::updatePathList()
{
    // check each path actually exists, if not remove it
    QMutableListIterator<QString> i(qucsPathList);
    while (i.hasNext()) {
        i.next();
        QDir thispath(i.value());
        if (!thispath.exists())
        {
            // the path does not exist, remove it from the list
            i.remove();
        }
    }
}

// replace the old path list with a new one
void QucsApp::updatePathList(QStringList newPathList)
{
    // clear out the old path list
    qucsPathList.clear();

    // copy the new path into the path list
    for (const QString& path : newPathList)
    {
        qucsPathList.append(path);
    }
    // do the normal path update operations
    updatePathList();
}


void QucsApp::updateRecentFilesList(QString s)
{
  QSettings* settings = new QSettings("qucs","qucs_s");
  QucsSettings.RecentDocs.removeAll(s);
  QucsSettings.RecentDocs.prepend(s);
  if (QucsSettings.RecentDocs.size() > MaxRecentFiles) {
    QucsSettings.RecentDocs.removeLast();
  }
  settings->setValue("RecentDocs",QucsSettings.RecentDocs.join("*"));
  delete settings;
  slotUpdateRecentFiles();
}

void QucsApp::slotSaveDiagramToGraphicsFile()
{
  slotSaveSchematicToGraphicsFile(true);
}

void QucsApp::slotSaveSchematicToGraphicsFile(bool diagram)
{
  ImageWriter *writer = new ImageWriter(lastExportFilename);
  writer->setDiagram(diagram);
  if (!writer->print(DocumentTab->currentWidget())) {
    lastExportFilename = writer->getLastSavedFile();
    statusBar()->showMessage(QObject::tr("Successfully exported"), 2000);
  }
  delete writer;
}


void QucsApp::slotSimSettings()
{
    SimSettingsDialog *SetDlg = new SimSettingsDialog(this);
    SetDlg->exec();
    delete SetDlg;
    fillSimulatorsComboBox();
}

void QucsApp::slotSimulateWithSpice()
{
    if (!isTextDocument(DocumentTab->currentWidget()))
    {
        Schematic* schematic(dynamic_cast<Schematic*>(DocumentTab->currentWidget()));
        if (TuningMode)
        {
            QFileInfo Info(schematic->getDocName());
            QString ext = Info.suffix();
            if (ext == "dpl")
            {
                QucsDoc *doc(dynamic_cast<QucsDoc*>(schematic));
                Q_ASSERT(doc != nullptr);
                schematic = dynamic_cast<Schematic*>(getSchematicWidget(doc));
                if (schematic == nullptr)
                {
                    return;
                }
            }
        }

        if (schematic->getDocName().isEmpty())
        {
            auto biasState = schematic->getShowBias();
            QMessageBox::warning(
                    this,
                    tr("Simulate schematic"),
                    tr("Schematic not saved! Simulation of unsaved schematic "
                      "not possible. Save schematic first!"));
            slotFileSaveAs();
            schematic->setShowBias(biasState);
        }
        ExternSimDialog *SimDlg = new ExternSimDialog(schematic, false);
        connect(SimDlg, SIGNAL(simulated(ExternSimDialog*)), this, SLOT(slotAfterSpiceSimulation(ExternSimDialog*)));
        connect(SimDlg, SIGNAL(warnings()), this, SLOT(slotShowWarnings()));
        connect(SimDlg, SIGNAL(success()), this, SLOT(slotResetWarnings()));

        if (TuningMode || schematic->getShowBias() == 0)
        {
            SimDlg->slotStart();
        }
        else
        {
            SimDlg->exec();
        }
        /*disconnect(SimDlg, SIGNAL(simulated()), this, SLOT(slotAfterSpiceSimulation()));
        disconnect(SimDlg, SIGNAL(warnings()), this, SLOT(slotShowWarnings()));
        disconnect(SimDlg, SIGNAL(success()), this, SLOT(slotResetWarnings()));*/
        /*if (SimDlg->wasSimulated && schematic->getSimOpenDpl())
            if (schematic->getShowBias() < 1) slotChangePage(schematic->getDocName(), schematic->getDataDisplay());
        delete SimDlg;*/
    }
    else
    {
        QMessageBox::warning(
                this,
                tr("Simulate schematic"),
                tr("Simulation of text document is not possible!"));
    }
}

void QucsApp::slotSaveNetlist()
{
    if (QucsSettings.DefaultSimulator == spicecompat::simQucsator)
    {
        QMessageBox::information(
                this,
                tr("Save netlist"),
                tr("This action is supported only for SPICE simulators!"));
        return;
    }

    if (!isTextDocument(DocumentTab->currentWidget()))
    {
        Schematic* schematic(dynamic_cast<Schematic*>(DocumentTab->currentWidget()));
        Q_ASSERT(schematic != nullptr);

        ExternSimDialog simDlg(schematic, a_netlist2Console, true);
        simDlg.slotSaveNetlist();
    }
}

void QucsApp::slotSaveCdlNetlist()
{
    if (!isTextDocument(DocumentTab->currentWidget()))
    {
        Schematic* schematic = dynamic_cast<Schematic*>(DocumentTab->currentWidget());
        Q_ASSERT(schematic != nullptr);

        if (a_netlist2Console)
        {
            QString netlistString;
            {
                QTextStream netlistStream(&netlistString);
                CdlNetlistWriter cdlWriter(netlistStream, schematic);
                if (!cdlWriter.write())
                {
                    QMessageBox::critical(
                            this,
                            tr("Save CDL netlist"),
                            tr("Save CDL netlist failed!"),
                            QMessageBox::Ok);
                }
                printf("\nCDL netlist:\n%s\n", netlistString.toUtf8().constData());
                Content->refresh();
            }
        }
        else
        {
            QFileInfo inf(schematic->getDocName());
            QString filename = QFileDialog::getSaveFileName(
                    this,
                    tr("Save CDL netlist"),
                    inf.path() + QDir::separator() + "netlist.cdl",
                    "CDL netlist (*.cdl)");

            if (filename.isEmpty())
            {
                return;
            }

            QFile netlistFile(filename);
            if (netlistFile.open(QIODevice::WriteOnly))
            {
                QTextStream netlistStream(&netlistFile);
                CdlNetlistWriter cdlWriter(netlistStream, schematic);
                if (!cdlWriter.write())
                {
                    QMessageBox::critical(
                            this,
                            tr("Save CDL netlist"),
                            tr("Save CDL netlist failed!"),
                            QMessageBox::Ok);
                }
                netlistFile.close();
                Content->refresh();
            }
        }
    }
}

void QucsApp::slotAfterSpiceSimulation(ExternSimDialog *SimDlg)
{
    Schematic *sch = (Schematic*)DocumentTab->currentWidget();
    disconnect(SimDlg,SIGNAL(simulated(ExternSimDialog *)),
               this,SLOT(slotAfterSpiceSimulation(ExternSimDialog *)));
    disconnect(SimDlg,SIGNAL(warnings()),this,SLOT(slotShowWarnings()));
    disconnect(SimDlg,SIGNAL(success()),this,SLOT(slotResetWarnings()));
    if (TuningMode && SimDlg->hasError()) {
        SimDlg->show();
        return;
    }
    if (SimDlg->wasSimulated()) {
        if(sch->getSimOpenDpl()) {
            if (sch->getShowBias() < 1) {
                if (!TuningMode) {
                    slotChangePage(sch->getDocName(),sch->getDataDisplay());
                } else if (!sch->getDocName().endsWith(".dpl")) {
                    slotChangePage(sch->getDocName(),sch->getDataDisplay());
                }
            }
        } else {
            if (sch->getShowBias() < 1  && !TuningMode) {
                int idx = Category::getModulesNr (QObject::tr("diagrams"));
                CompChoose->setCurrentIndex(idx);   // switch to diagrams
                slotSetCompView (idx);
            }
        }
    }

    sch->reloadGraphs();
    sch->viewport()->update();
    if(sch->getSimRunScript()) {
      // run script
      octave->startOctave();
      octave->runOctaveScript(sch->getScript());
    }
    if (TuningMode) {
        tunerDia->SimulationEnded();
    }
    if (sch->getShowBias()>0 || QucsMain->TuningMode) SimDlg->close();
}

void QucsApp::slotBuildVAModule()
{
    if (!isTextDocument(DocumentTab->currentWidget())) {
        Schematic *Sch = (Schematic*)DocumentTab->currentWidget();

        QFileInfo inf(Sch->getDocName());
        QString filename = QFileDialog::getSaveFileName(this,tr("Save Verilog-A module"),
                                                        inf.path()+QDir::separator()+"testmodule.va",
                                                        "Verilog-A (*.va)");
        if (filename.isEmpty()) return;

        QFile f(filename);
        if (f.open(QIODevice::WriteOnly)) {
            QTextStream stream(&f);
            VerilogAwriter *writer = new VerilogAwriter;
            if (!writer->createVA_module(stream,Sch)) {
                QMessageBox::critical(this,tr("Build Verilog-A module"),
                                      tr("This schematic is not a subcircuit!\n"
                                         "Use subcircuit to crete Verilog-A module!"),
                                          QMessageBox::Ok);
            }
            delete writer;
            f.close();
            Content->refresh();
        }
    }

}

void QucsApp::slotShowModel()
{
    DisplayDialog *dlg = new DisplayDialog(this,Symbol->ModelString,
                                           Symbol->SpiceString);
    dlg->exec();
    delete dlg;
}

void QucsApp::slotSearchLibComponent(const QString &comp)
{
    if (comp.isEmpty()) {
        QTreeWidgetItemIterator top_itm(libTreeWidget);
        while (*top_itm) {
            (*top_itm)->setExpanded(false);
            (*top_itm)->setHidden(false);
            for (int i = 0; i < (*top_itm)->childCount(); i++ ) {
                auto itm = (*top_itm)->child(i);
                itm->setHidden(false);
            }
            top_itm++;
        }
        return;
    }

    QTreeWidgetItemIterator top_itm(libTreeWidget);
    while (*top_itm) {
        bool found = false;
        int cnt = (*top_itm)->childCount();
        if (cnt < 1) {
            top_itm++;
            continue;
        }
        for (int i = 0; i < cnt; i++ ) {
            auto itm = (*top_itm)->child(i);
            QString name =itm->text(0);
            if (name.contains(comp,Qt::CaseInsensitive)) {
                itm->setHidden(false);
                found=true;
            } else {
                itm->setHidden(true);
            }
        }
        if (found) {
            (*top_itm)->setExpanded(true);
            (*top_itm)->setHidden(false);
        } else {
            (*top_itm)->setExpanded(false);
            (*top_itm)->setHidden(true);
        }
        top_itm++;
    }
}

void QucsApp::slotSearchLibClear()
{
    LibCompSearch->clear();
    slotSearchLibComponent("");
}

QVariant QucsFileSystemModel::data( const QModelIndex& index, int role ) const
{
    if (role == Qt::DecorationRole) { // it's an icon
        QString dName = fileName(index);
        if (dName.endsWith("_prj")) { // it's a Qucs project
            // for some reason SVG does not always work on Windows, so use PNG
            return QIcon(":bitmaps/hicolor/128x128/apps/qucs.png");
        }
    }
    // return default system icon
    return QFileSystemModel::data(index, role);
}

// function below is adapted from https://stackoverflow.com/questions/10789284/qfilesystemmodel-sorting-dirsfirst
bool QucsSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    // if sorting by file names column
    if (sortColumn() == 0) {
        QucsFileSystemModel *model = qobject_cast<QucsFileSystemModel*>(sourceModel());
        // get the current sort order (do we need this ?)
        bool asc = sortOrder() == Qt::AscendingOrder;

        QFileInfo leftFileInfo = model->fileInfo(left);
        QFileInfo rightFileInfo = model->fileInfo(right);
        QString leftFileName = model->fileName(left);
        QString rightFileName = model->fileName(right);

        // If DotAndDot move in the beginning
        if (sourceModel()->data(left).toString() == "..")
            return asc;
        if (sourceModel()->data(right).toString() == "..")
            return !asc;

        // move dirs upper
        if (!leftFileInfo.isDir() && rightFileInfo.isDir()) {
            return !asc;
        }
        if (leftFileInfo.isDir() && !rightFileInfo.isDir()) {
            return asc;
        }
        // move dirs ending in '_prj' upper
        if (leftFileInfo.isDir() && rightFileInfo.isDir()) {
            if (!leftFileName.endsWith("_prj") && rightFileName.endsWith("_prj")) {
                return !asc;
            }
            if (leftFileName.endsWith("_prj") && !rightFileName.endsWith("_prj")) {
                return asc;
            }
        }
    }

    return QSortFilterProxyModel::lessThan(left, right);
}
