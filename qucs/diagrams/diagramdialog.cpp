/***************************************************************************
                              diagramdialog.cpp
                             -------------------
    begin                : Sun Oct 5 2003
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

/*!
  \class DiagramDialog
  \brief The DiagramDialog is used to setup and edit diagrams.
*/
#include "diagramdialog.h"
#include "qucs.h"
#include "schematic.h"
#include "rect3ddiagram.h"
#include "main.h"
#include "misc.h"

#include <cmath>
#include <assert.h>

#include <QPaintEvent>
#include <QPushButton>
#include <QTabWidget>
#include <QLabel>
#include <QStringList>
#include <QColorDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QSlider>
#include <QComboBox>
#include <QListWidget>
#include <QTableWidget>
#include <QPainter>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QDir>
#include <QDebug>

#define CROSS3D_SIZE   30
#define WIDGET3D_SIZE  2*CROSS3D_SIZE
// This widget class paints a small 3-dimensional coordinate cross.
class Cross3D : public QWidget  {
public:
  Cross3D(float rx_, float ry_, float rz_, QWidget *parent = 0)
          : QWidget(parent) {
    rotX = rx_;
    rotY = ry_;
    rotZ = rz_;
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setMinimumSize(WIDGET3D_SIZE, WIDGET3D_SIZE);
    resize(WIDGET3D_SIZE, WIDGET3D_SIZE);
  };
 ~Cross3D() {};

  double rotX, rotY, rotZ;   // in radians !!!!

private:
  void  paintEvent(QPaintEvent*) {
    QPainter Painter(this);
    float cxx = cos(rotZ);
    float cxy = sin(rotZ);
    float cxz = sin(rotY);
    float cyz = sin(rotX);
    float cyy = cos(rotX);
    float cyx = cyy * cxy + cyz * cxz * cxx;
    cyy = cyy * cxx - cyz * cxz * cxy;
    cyz *= cos(rotY);
    cxx *= cos(rotY);
    cxy *= cos(rotY);

    Painter.setPen(QPen(Qt::red,2));
    Painter.drawLine(CROSS3D_SIZE, CROSS3D_SIZE,
		     int(CROSS3D_SIZE * (1.0+cxx)),
		     int(CROSS3D_SIZE * (1.0-cyx)));
    Painter.setPen(QPen(Qt::green,2));
    Painter.drawLine(CROSS3D_SIZE, CROSS3D_SIZE,
		     int(CROSS3D_SIZE * (1.0-cxy)),
		     int(CROSS3D_SIZE * (1.0-cyy)));
    Painter.setPen(QPen(Qt::blue,2));
    Painter.drawLine(CROSS3D_SIZE, CROSS3D_SIZE,
		     int(CROSS3D_SIZE * (1.0+cxz)),
		     int(CROSS3D_SIZE * (1.0+cyz)));
  };
};



// standard colors: blue, red, magenta, green, cyan, yellow, grey, black
static const QRgb DefaultColors[]
          = {0x0000ff, 0xff0000, 0xff00ff, 0x00ff00, 0x00ffff, 0xffff00,
             0x777777, 0x000000};

static const int NumDefaultColors = 8;


DiagramDialog::DiagramDialog(Diagram *d, QWidget *parent, Graph *currentGraph)
                    : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
  Diag = d;
  Graphs.setAutoDelete(true);
  copyDiagramGraphs();   // make a copy of all graphs
  if(parent){
	  const Schematic* s = dynamic_cast<const Schematic*>(parent);
	  assert(s);
	  QFileInfo Info(s->DocName);
      defaultDataSet = Info.absolutePath() + QDir::separator() + s->DataSet;
  }else{
	  defaultDataSet = "unknown";
  }
  setWindowTitle(tr("Edit Diagram Properties"));
  changed = false;
  transfer = false;  // have changes be applied ? (used by "Cancel")
  toTake = false;   // double-clicked variable be inserted into graph list ?

  Expr.setPattern("[^\"]+");
  Validator  = new QRegExpValidator(Expr, this);
  ValInteger = new QIntValidator(0, 360, this);
  ValDouble  = new QDoubleValidator(-1e200, 1e200, 6, this);

  QString NameY, NameZ;
  if((Diag->Name == "Rect") || (Diag->Name == "Curve")) {
    NameY = tr("left Axis");
    NameZ = tr("right Axis");
  }
  else if(Diag->Name == "Polar") {
    NameY = tr("y-Axis");
  }
  else if((Diag->Name == "Smith") || (Diag->Name == "ySmith")) {
    NameY = tr("y-Axis");
  }
  else if(Diag->Name == "PS") {
    NameY = tr("smith Axis");
    NameZ = tr("polar Axis");
  }
  else if(Diag->Name == "SP") {
    NameY = tr("polar Axis");
    NameZ = tr("smith Axis");
  }
  else if(Diag->Name == "Rect3D") {
    NameY = tr("y-Axis");
    NameZ = tr("z-Axis");
  }

  
  all = new QVBoxLayout(this); // to provide neccessary size
  QTabWidget *t = new QTabWidget();
  all->addWidget(t);

  // Tab #1 - Data ...........................................................
  QWidget *Tab1 = new QWidget();
  QVBoxLayout *Tab1Layout = new QVBoxLayout();
  Tab1Layout->setSpacing(0);
  Tab1->setLayout(Tab1Layout);

  Label4 = 0;     // different types with same content
  yrLabel = 0;
  yAxisBox = 0;
  Property2 = 0;
  ColorButt = 0;
  hideInvisible = 0;
  rotationX = rotationY = rotationZ = 0;

  QGroupBox *InputGroup = new QGroupBox(tr("Graph Input"));
  QVBoxLayout *InputGroupLayout = new QVBoxLayout();
  InputGroup->setLayout(InputGroupLayout);
  Tab1Layout->addWidget(InputGroup);
  GraphInput = new QLineEdit();
  lblPlotVs = new QLabel(tr("Plot Vs."));
  ChooseXVar = new QComboBox();
  connect(ChooseXVar,SIGNAL(currentIndexChanged(int)),this,SLOT(slotPlotVs(int)));
  QHBoxLayout *InpSubHL = new QHBoxLayout();
  InpSubHL->addWidget(GraphInput);
  InpSubHL->addWidget(lblPlotVs);
  InpSubHL->addWidget(ChooseXVar);
  InputGroupLayout->addLayout(InpSubHL);
  GraphInput->setValidator(Validator);
  connect(GraphInput, SIGNAL(textChanged(const QString&)), SLOT(slotResetToTake(const QString&)));
  QWidget *Box2 = new QWidget();
  QHBoxLayout *Box2Layout = new QHBoxLayout();
  Box2->setLayout(Box2Layout);
  InputGroupLayout->addWidget(Box2);
  Box2Layout->setSpacing(5);

  if(Diag->Name == "Tab") {
    Label1 = new QLabel(tr("Number Notation: "));
    Box2Layout->addWidget(Label1);
    PropertyBox = new QComboBox();
    Box2Layout->addWidget(PropertyBox);
    PropertyBox->addItem(tr("real/imaginary"));
    PropertyBox->addItem(tr("magnitude/angle (degree)"));
    PropertyBox->addItem(tr("magnitude/angle (radian)"));
    PropertyBox->setCurrentIndex(1);
    connect(PropertyBox, SIGNAL(activated(int)), SLOT(slotSetNumMode(int)));
    Box2Layout->setStretchFactor(new QWidget(Box2), 5); // stretchable placeholder

    Label2 = new QLabel(tr("Precision:"));
    Box2Layout->addWidget(Label2);
    Property2 = new QLineEdit();
    Box2Layout->addWidget(Property2);
    Property2->setValidator(ValInteger);
    Property2->setMaxLength(2);
    Property2->setMaximumWidth(25);
    Property2->setText("3");
  }
  else if(Diag->Name != "Truth") {
    Label1 = new QLabel(tr("Color:"));
    Box2Layout->addWidget(Label1);
    ColorButt = new QPushButton("   ");
    Box2Layout->addWidget(ColorButt);
    ColorButt->setMinimumWidth(50);
    ColorButt->setEnabled(false);
    connect(ColorButt, SIGNAL(clicked()), SLOT(slotSetColor()));
    Box2Layout->setStretchFactor(new QWidget(Box2), 5); // stretchable placeholder

    Label3 = new QLabel(tr("Style:"));
    Box2Layout->addWidget(Label3);
    Label3->setEnabled(false);
    PropertyBox = new QComboBox();
    Box2Layout->addWidget(PropertyBox);
    PropertyBox->addItem(tr("solid line"));
    PropertyBox->addItem(tr("dash line"));
    PropertyBox->addItem(tr("dot line"));
    if(Diag->Name != "Time") {
      PropertyBox->addItem(tr("long dash line"));
      PropertyBox->addItem(tr("stars"));
      PropertyBox->addItem(tr("circles"));
      PropertyBox->addItem(tr("arrows"));
    }
    connect(PropertyBox, SIGNAL(activated(int)),
			 SLOT(slotSetGraphStyle(int)));
    Box2Layout->setStretchFactor(new QWidget(Box2), 5); // stretchable placeholder

    Label2 = new QLabel(tr("Thickness:"));
    Box2Layout->addWidget(Label2);
    Property2 = new QLineEdit();
    Box2Layout->addWidget(Property2);
    Property2->setValidator(ValInteger);
    Property2->setMaximumWidth(25);
    Property2->setMaxLength(2);
    Property2->setText("0");

    if((Diag->Name=="Rect") || (Diag->Name=="PS") || (Diag->Name=="SP") || (Diag->Name=="Curve")) {
      Label4 = new QLabel(tr("y-Axis:"));
      Box2Layout->addWidget(Label4);
      Label4->setEnabled(false);
      yAxisBox = new QComboBox();
      Box2Layout->addWidget(yAxisBox);
      yAxisBox->addItem(NameY);
      yAxisBox->addItem(NameZ);
      yAxisBox->setEnabled(false);
      connect(yAxisBox, SIGNAL(activated(int)), SLOT(slotSetYAxis(int)));
    }
  }
  if(Property2) {
    connect(Property2, SIGNAL(textChanged(const QString&)),
			SLOT(slotSetProp2(const QString&)));

    Label1->setEnabled(false);
    PropertyBox->setEnabled(false);
    Label2->setEnabled(false);
    Property2->setEnabled(false);
  }

  QWidget *Box1 = new QWidget();
  Tab1Layout->addWidget(Box1);
  QHBoxLayout *Box1Layout = new QHBoxLayout();
  Box1->setLayout(Box1Layout);
  Box1Layout->setSpacing(5);
  Box1Layout->setContentsMargins(0, 0, 0, 0);

  QGroupBox *DataGroup = new QGroupBox(tr("Dataset"));
  Box1Layout->addWidget(DataGroup);
  QVBoxLayout *DataGroupLayout = new QVBoxLayout();
  DataGroup->setLayout(DataGroupLayout);
  ChooseData = new QComboBox();
  DataGroupLayout->addWidget(ChooseData);
  ChooseData->setMinimumWidth(300); // will force also min width of table below
  connect(ChooseData, SIGNAL(activated(int)), SLOT(slotReadVarsAndSetSimulator(int)));
  // connect(ChooseData, SIGNAL(currentIndexChanged(int)),this,SLOT(slotSetSimulator()));
  // todo: replace by QTableWidget
  // see https://gist.github.com/ClemensFMN/8955411

  QHBoxLayout *hb1 = new QHBoxLayout;
  ChooseSimulator = new QComboBox;
  QStringList lst_sim;
  lst_sim<<"Qucsator (built-in)"<<"Ngspice"<<"Xyce"<<"SpiceOpus";
  ChooseSimulator->addItems(lst_sim);
  connect(ChooseSimulator,SIGNAL(currentIndexChanged(int)),this,SLOT(slotReadVars(int)));
  lblSim = new QLabel(tr("Data from simulator:"));
  hb1->addWidget(lblSim);
  hb1->addWidget(ChooseSimulator);
  DataGroupLayout->addLayout(hb1);

  ChooseVars = new QTableWidget(1, 3);
  ChooseVars->verticalHeader()->setVisible(false);
  ChooseVars->horizontalHeader()->setStretchLastSection(true);
  ChooseVars->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  // make sure sorting is disabled before inserting items
  ChooseVars->setSortingEnabled(false);
  ChooseVars->horizontalHeader()->setSortIndicatorShown(true);
  ChooseVars->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);

  ChooseVars->setSelectionBehavior(QAbstractItemView::SelectRows);
  //ChooseVars->selectRow(0);
  DataGroupLayout->addWidget(ChooseVars);
  //ChooseVars->addColumn(tr("Name"));
  //ChooseVars->addColumn(tr("Type"));
  //ChooseVars->addColumn(tr("Size"));
  QStringList headers;
  headers << tr("Name") << tr("Type") << tr("Size");
  ChooseVars->setHorizontalHeaderLabels(headers);

  connect(ChooseVars, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), SLOT(slotTakeVar(QTableWidgetItem*)));

  QGroupBox *GraphGroup = new QGroupBox(tr("Graph"));
  Box1Layout->addWidget(GraphGroup);
  QVBoxLayout *GraphGroupLayout = new QVBoxLayout();
  GraphGroup->setLayout(GraphGroupLayout);
  GraphList = new QListWidget();
  GraphGroupLayout->addWidget(GraphList);
  connect(GraphList, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(slotSelectGraph(QListWidgetItem*)));
  connect(GraphList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(slotDeleteGraph()));
  QPushButton *NewButt = new QPushButton(tr("New Graph"));
  GraphGroupLayout->addWidget(NewButt);
  connect(NewButt, SIGNAL(clicked()), SLOT(slotNewGraph()));
  QPushButton *DelButt = new QPushButton(tr("Delete Graph"));
  GraphGroupLayout->addWidget(DelButt);
  connect(DelButt, SIGNAL(clicked()), SLOT(slotDeleteGraph()));

  t->addTab(Tab1, tr("Data"));



  // Tab #2...........................................................
  int Row = 0;
  if(Diag->Name.at(0) != 'T') {  // not tabular or timing diagram
    QWidget *Tab2 = new QWidget(t);
    //QGridLayout *gp = new QGridLayout(Tab2,13,3,5,5);
    QGridLayout *gp = new QGridLayout(Tab2);

    gp->addWidget(new QLabel(tr("x-Axis Label:"), Tab2), Row,0);
    xLabel = new QLineEdit(Tab2);
    xLabel->setValidator(Validator);
    gp->addWidget(xLabel, Row,1);
    Row++;

    gp->addWidget(
        new QLabel(NameY+" "+tr("Label:"), Tab2), Row,0);
    ylLabel = new QLineEdit(Tab2);
    ylLabel->setValidator(Validator);
    gp->addWidget(ylLabel, Row,1);
    Row++;

    if((Diag->Name != "Smith") && (Diag->Name != "Polar")) {
      gp->addWidget(
        new QLabel(NameZ +" "+tr("Label:"), Tab2), Row,0);
      yrLabel = new QLineEdit(Tab2);
      yrLabel->setValidator(Validator);
      gp->addWidget(yrLabel, Row,1);
      Row++;
    }

    gp->addWidget(new QLabel(
        tr("<b>Label text</b>: Use LaTeX style for special characters, e.g. \\tau"),
        Tab2),  Row,0,1,2);
    Row++;

    if(Diag->Name != "Rect3D") {
      GridOn = new QCheckBox(tr("show Grid"), Tab2);
      gp->addWidget(GridOn, Row,0);
      Row++;

      GridLabel1 = new QLabel(tr("Grid Color:"),Tab2);
      gp->addWidget(GridLabel1, Row,0);
      GridColorButt = new QPushButton("        ",Tab2);
      connect(GridColorButt, SIGNAL(clicked()), SLOT(slotSetGridColor()));
      gp->addWidget(GridColorButt, Row,1);
      Row++;    
      misc::setPickerColor(GridColorButt, Diag->GridPen.color());

      GridLabel2 = new QLabel(tr("Grid Style: "), Tab2);
      gp->addWidget(GridLabel2, Row,0);
      GridStyleBox = new QComboBox(Tab2);
      GridStyleBox->addItem(tr("solid line"));
      GridStyleBox->addItem(tr("dash line"));
      GridStyleBox->addItem(tr("dot line"));
      GridStyleBox->addItem(tr("dash dot line"));
      GridStyleBox->addItem(tr("dash dot dot line"));
      gp->addWidget(GridStyleBox, Row,1);
      Row++;
      GridStyleBox->setCurrentIndex(Diag->GridPen.style()-1);

      GridOn->setChecked(Diag->xAxis.GridOn);
      if(!Diag->xAxis.GridOn) slotSetGridBox(0);
      connect(GridOn, SIGNAL(stateChanged(int)), SLOT(slotSetGridBox(int)));
    }
    else {
      GridOn = 0;
      GridColorButt = 0;
      GridStyleBox = 0;
      NotationBox = 0;
    }

    NotationLabel = new QLabel(tr("Number notation: "),Tab2);
    gp->addWidget(NotationLabel,Row,0);
    NotationBox = new QComboBox(Tab2);
    NotationBox->addItem(tr("scientific notation"));
    NotationBox->addItem(tr("engineerign notation"));
    if (Diag->engineeringNotation) NotationBox->setCurrentIndex(1);
    else NotationBox->setCurrentIndex(0);
    gp->addWidget(NotationBox,Row,1);
    Row++;

    // ...........................................................
    xLabel->setText(Diag->xAxis.Label);
    ylLabel->setText(Diag->yAxis.Label);
    if(yrLabel)  yrLabel->setText(Diag->zAxis.Label);

    if((Diag->Name.left(4) == "Rect") || (Diag->Name == "Curve")) {
      GridLogX = new QCheckBox(tr("logarithmical X Axis Grid"), Tab2);
      gp->addWidget(GridLogX, Row,0);
      Row++;

      QStringList units_name;
      units_name<<"No units"<<"dB"<<"dBuV"<<"dBm";

      GridLogY = new QCheckBox(tr("logarithmical")+" "+NameY+" "+tr("Grid"), Tab2);
      gp->addWidget(GridLogY, Row,0);
      LogUnitsY = new QComboBox(Tab2);
      LogUnitsY->addItems(units_name);
      LogUnitsY->setCurrentIndex(Diag->yAxis.Units);
      LogUnitsY->setEnabled(Diag->yAxis.log);
      connect(GridLogY,SIGNAL(toggled(bool)),LogUnitsY,SLOT(setEnabled(bool)));
      connect(LogUnitsY,SIGNAL(currentIndexChanged(int)),this,SLOT(slotRecalcDbLimitsY()));
      gp->addWidget(LogUnitsY, Row, 1);
      Row++;

      GridLogZ = new QCheckBox(tr("logarithmical")+" "+NameZ+" "+tr("Grid"), Tab2);
      gp->addWidget(GridLogZ, Row,0);
      LogUnitsZ = new QComboBox(Tab2);
      LogUnitsZ->addItems(units_name);
      LogUnitsZ->setCurrentIndex(Diag->zAxis.Units);
      LogUnitsZ->setEnabled(Diag->zAxis.log);
      connect(GridLogZ,SIGNAL(toggled(bool)),LogUnitsZ,SLOT(setEnabled(bool)));
      connect(LogUnitsZ,SIGNAL(currentIndexChanged(int)),this,SLOT(slotRecalcDbLimitsZ()));
      gp->addWidget(LogUnitsZ, Row, 1);
      Row++;

      // ...........................................................
      // transfer the diagram properties to the dialog
      GridLogX->setChecked(Diag->xAxis.log);
      GridLogY->setChecked(Diag->yAxis.log);
      GridLogZ->setChecked(Diag->zAxis.log);


      if(Diag->Name == "Rect3D") {
        hideInvisible = new QCheckBox(tr("hide invisible lines"), Tab2);
        gp->addWidget(hideInvisible, Row,0);
        Row++;

        QLabel *LabelRotX = new QLabel(tr("Rotation around x-Axis:"), Tab2);
        //LabelRotX->setPaletteForegroundColor(Qt::red);
        gp->addWidget(LabelRotX, Row,0);
        SliderRotX = new QSlider(Tab2);
        SliderRotX->setMinimum(0);
        SliderRotX->setMaximum(360);
        SliderRotX->setSingleStep(20);
        SliderRotX->setValue(((Rect3DDiagram*)Diag)->rotX);
        SliderRotX->setOrientation(Qt::Horizontal);
        gp->addWidget(SliderRotX, Row,1);
        connect(SliderRotX, SIGNAL(valueChanged(int)), SLOT(slotNewRotX(int)));
        rotationX = new QLineEdit(Tab2);
        rotationX->setValidator(ValInteger);
        rotationX->setMaxLength(3);
        rotationX->setMaximumWidth(40);
        gp->addWidget(rotationX, Row,2);
        connect(rotationX, SIGNAL(textChanged(const QString&)),
			   SLOT(slotEditRotX(const QString&)));
        Row++;

        QLabel *LabelRotY = new QLabel(tr("Rotation around y-Axis:"), Tab2);
        //LabelRotY->setPaletteForegroundColor(Qt::green);
        gp->addWidget(LabelRotY, Row,0);
        //SliderRotY = new QSlider(0,360,20, ((Rect3DDiagram*)Diag)->rotY,
        //		 Qt::Horizontal, Tab2);
        SliderRotY = new QSlider(Tab2);
        SliderRotY->setMinimum(0);
        SliderRotY->setMaximum(360);
        SliderRotY->setSingleStep(20);
        SliderRotY->setValue(((Rect3DDiagram*)Diag)->rotY);
        SliderRotY->setOrientation(Qt::Horizontal);
        gp->addWidget(SliderRotY, Row,1);
        connect(SliderRotY, SIGNAL(valueChanged(int)), SLOT(slotNewRotY(int)));
        rotationY = new QLineEdit(Tab2);
        rotationY->setValidator(ValInteger);
        rotationY->setMaxLength(3);
        rotationY->setMaximumWidth(40);
        gp->addWidget(rotationY, Row,2);
        connect(rotationY, SIGNAL(textChanged(const QString&)),
			   SLOT(slotEditRotY(const QString&)));
        Row++;

        QLabel *LabelRotZ = new QLabel(tr("Rotation around z-Axis:"), Tab2);
        //LabelRotZ->setPaletteForegroundColor(Qt::blue);
        gp->addWidget(LabelRotZ, Row,0);
        //SliderRotZ = new QSlider(0,360,20, ((Rect3DDiagram*)Diag)->rotZ,
        //		 Qt::Horizontal, Tab2);
        SliderRotZ = new QSlider(Tab2);
        SliderRotZ->setMinimum(0);
        SliderRotZ->setMaximum(360);
        SliderRotZ->setSingleStep(20);
        SliderRotZ->setValue(((Rect3DDiagram*)Diag)->rotZ);
        SliderRotZ->setOrientation(Qt::Horizontal);
        gp->addWidget(SliderRotZ, Row,1);
        connect(SliderRotZ, SIGNAL(valueChanged(int)), SLOT(slotNewRotZ(int)));
        rotationZ = new QLineEdit(Tab2);
        rotationZ->setValidator(ValInteger);
        rotationZ->setMaxLength(3);
        rotationZ->setMaximumWidth(40);
        gp->addWidget(rotationZ, Row,2);
        connect(rotationZ, SIGNAL(textChanged(const QString&)),
			   SLOT(slotEditRotZ(const QString&)));
        Row++;

        gp->addWidget(new QLabel(tr("2D-projection:"), Tab2), Row,0);
        DiagCross = new Cross3D(((Rect3DDiagram*)Diag)->rotX,
				((Rect3DDiagram*)Diag)->rotY,
				((Rect3DDiagram*)Diag)->rotZ, Tab2);
        gp->addWidget(DiagCross, Row,1);

        // transfer the diagram properties to the dialog
        hideInvisible->setChecked(Diag->hideLines);
        rotationX->setText(QString::number(((Rect3DDiagram*)Diag)->rotX));
        rotationY->setText(QString::number(((Rect3DDiagram*)Diag)->rotY));
        rotationZ->setText(QString::number(((Rect3DDiagram*)Diag)->rotZ));

      }
    }
    else GridLogX = GridLogY = GridLogZ = 0;

    t->addTab(Tab2, tr("Properties"));

    // Tab #3 - Limits ...........................................................
    QWidget *Tab3 = new QWidget();
    QVBoxLayout *Tab3Layout = new QVBoxLayout();

    QGroupBox *axisX = new QGroupBox(tr("x-Axis"));
    QHBoxLayout *axisXLayout = new QHBoxLayout();

    QWidget *VBox1 = new QWidget();
    axisXLayout->addWidget(VBox1);
    QVBoxLayout *VBox1Layout = new QVBoxLayout();
    VBox1Layout->addStretch();
    manualX = new QCheckBox(tr("manual"));//, VBox1);
    VBox1Layout->addWidget(manualX);
    VBox1->setLayout(VBox1Layout);
    connect(manualX, SIGNAL(stateChanged(int)), SLOT(slotManualX(int)));

    QWidget *VBox2 = new QWidget();
    axisXLayout->addWidget(VBox2);
    QVBoxLayout *VBox2Layout = new QVBoxLayout();
    VBox2Layout->addWidget(new QLabel(tr("start")));
    startX = new QLineEdit();
    VBox2Layout->addWidget(startX);
    startX->setValidator(ValDouble);
    VBox2->setLayout(VBox2Layout);

    QWidget *VBox3 = new QWidget();
    axisXLayout->addWidget(VBox3);
    QVBoxLayout *VBox3Layout = new QVBoxLayout();
    VBox3Layout->addWidget(new QLabel(tr("step")));
    stepX = new QLineEdit();//VBox3);
    VBox3Layout->addWidget(stepX);
    stepX->setValidator(ValDouble);
    VBox3->setLayout(VBox3Layout);

    QWidget *VBox4 = new QWidget();
    axisXLayout->addWidget(VBox4);
    QVBoxLayout *VBox4Layout = new QVBoxLayout();
    VBox4Layout->addWidget(new QLabel(tr("stop")));
    stopX = new QLineEdit();
    VBox4Layout->addWidget(stopX);
    stopX->setValidator(ValDouble);
    VBox4->setLayout(VBox4Layout);

    axisX->setLayout(axisXLayout);
    Tab3Layout->addWidget(axisX);
    Tab3Layout->addStretch();

    QGroupBox *axisY = new QGroupBox(NameY);
    QHBoxLayout *axisYLayout = new QHBoxLayout();

    QWidget *VBox5 = new QWidget();
    axisYLayout->addWidget(VBox5);
    QVBoxLayout *VBox5Layout = new QVBoxLayout();
    VBox5Layout->addStretch();
    manualY = new QCheckBox(tr("manual"));
    VBox5Layout->addWidget(manualY);
    connect(manualY, SIGNAL(stateChanged(int)), SLOT(slotManualY(int)));
    VBox5->setLayout(VBox5Layout);

    QWidget *VBox6 = new QWidget();
    axisYLayout->addWidget(VBox6);
    QVBoxLayout *VBox6Layout = new QVBoxLayout();
    VBox6Layout->addWidget(new QLabel(tr("start")));
    startY = new QLineEdit();
    VBox6Layout->addWidget(startY);
    startY->setValidator(ValDouble);
    VBox6->setLayout(VBox6Layout);

    QWidget *VBox7 = new QWidget();
    axisYLayout->addWidget(VBox7);
    QVBoxLayout *VBox7Layout = new QVBoxLayout();
    if((Diag->Name=="Smith") || (Diag->Name=="ySmith") || (Diag->Name=="PS"))
      VBox7Layout->addWidget(new QLabel(tr("number")));
    else  VBox7Layout->addWidget(new QLabel(tr("step")));
    stepY = new QLineEdit();
    VBox7Layout->addWidget(stepY);
    stepY->setValidator(ValDouble);
    VBox7->setLayout(VBox7Layout);

    QWidget *VBox8 = new QWidget();
    axisYLayout->addWidget(VBox8);
    QVBoxLayout *VBox8Layout = new QVBoxLayout();
    VBox8Layout->addWidget(new QLabel(tr("stop")));
    stopY = new QLineEdit();
    VBox8Layout->addWidget(stopY);
    stopY->setValidator(ValDouble);
    VBox8->setLayout(VBox8Layout);

    axisY->setLayout(axisYLayout);
    Tab3Layout->addWidget(axisY);
    Tab3Layout->addStretch();

    QGroupBox *axisZ = new QGroupBox(NameZ);
    QHBoxLayout *axisZLayout = new QHBoxLayout();

    QWidget *VBox9 = new QWidget();
    axisZLayout->addWidget(VBox9);
    QVBoxLayout *VBox9Layout = new QVBoxLayout();
    VBox9Layout->addStretch();
    manualZ = new QCheckBox(tr("manual"));
    VBox9Layout->addWidget(manualZ);
    connect(manualZ, SIGNAL(stateChanged(int)), SLOT(slotManualZ(int)));
    VBox9->setLayout(VBox9Layout);

    QWidget *VBox10 = new QWidget();
    axisZLayout->addWidget(VBox10);
    QVBoxLayout *VBox10Layout = new QVBoxLayout();
    VBox10Layout->addWidget(new QLabel(tr("start")));
    startZ = new QLineEdit();
    VBox10Layout->addWidget(startZ);
    startZ->setValidator(ValDouble);
    VBox10->setLayout(VBox10Layout);

    QWidget *VBox11 = new QWidget();
    axisZLayout->addWidget(VBox11);
    QVBoxLayout *VBox11Layout = new QVBoxLayout();
    if(Diag->Name == "SP") VBox11Layout->addWidget(new QLabel(tr("number")));
    else VBox11Layout->addWidget(new QLabel(tr("step")));
    stepZ = new QLineEdit();
    VBox11Layout->addWidget(stepZ);
    stepZ->setValidator(ValDouble);
    VBox11->setLayout(VBox11Layout);

    QWidget *VBox12 = new QWidget();
    axisZLayout->addWidget(VBox12);
    QVBoxLayout *VBox12Layout = new QVBoxLayout();
    VBox12Layout->addWidget(new QLabel(tr("stop")));
    stopZ = new QLineEdit();
    VBox12Layout->addWidget(stopZ);
    stopZ->setValidator(ValDouble);
    VBox12->setLayout(VBox12Layout);

    Tab3Layout->setStretchFactor(new QWidget(Tab3),5); // stretchable placeholder

    axisZ->setLayout(axisZLayout);
    Tab3Layout->addWidget(axisZ);

    Tab3->setLayout(Tab3Layout);
    t->addTab(Tab3, tr("Limits"));

    // ...........................................................
    // transfer the diagram properties to the dialog
    if(Diag->xAxis.autoScale)  slotManualX(0);
    else  manualX->setChecked(true);
    if(Diag->yAxis.autoScale)  slotManualY(0);
    else  manualY->setChecked(true);
    if(Diag->zAxis.autoScale)  slotManualZ(0);
    else  manualZ->setChecked(true);

    Diag->calcLimits();    // inserts auto-scale values if not manual

    startX->setText(QString::number(Diag->xAxis.limit_min));
    stepX->setText(QString::number(Diag->xAxis.step));
    stopX->setText(QString::number(Diag->xAxis.limit_max));

    double val_startY = qucs::num2db(Diag->yAxis.limit_min,Diag->yAxis.Units);
    double val_stepY = qucs::num2db(Diag->yAxis.step,Diag->yAxis.Units);
    double val_stopY = qucs::num2db(Diag->yAxis.limit_max,Diag->yAxis.Units);

    double val_startZ = qucs::num2db(Diag->zAxis.limit_min,Diag->zAxis.Units);
    double val_stepZ = qucs::num2db(Diag->zAxis.step,Diag->zAxis.Units);
    double val_stopZ = qucs::num2db(Diag->zAxis.limit_max,Diag->zAxis.Units);

    startY->setText(QString::number(val_startY));
    stepY->setText(QString::number(val_stepY));
    stopY->setText(QString::number(val_stopY));

    startZ->setText(QString::number(val_startZ));
    stepZ->setText(QString::number(val_stepZ));
    stopZ->setText(QString::number(val_stopZ));

    if((Diag->Name == "Smith") || (Diag->Name == "ySmith") ||
       (Diag->Name == "Polar")) {
       axisZ->setEnabled(false);
    }
    if(Diag->Name.left(4) != "Rect")   // cartesian 2D and 3D
      if(Diag->Name != "Curve") {
        axisX->setEnabled(false);
        startY->setEnabled(false);
        startZ->setEnabled(false);
      }
  }
  else  stepX = 0;

  connect(t, SIGNAL(currentChanged(QWidget*)), SLOT(slotChangeTab(QWidget*)));
  // ...........................................................
  QWidget *Butts = new QWidget();
  QHBoxLayout *ButtsLayout = new QHBoxLayout();
  ButtsLayout->setSpacing(5);
  ButtsLayout->setMargin(5);
  Butts->setLayout(ButtsLayout);
  all->addWidget(Butts);

  QPushButton *OkButt = new QPushButton(tr("OK"));
  ButtsLayout->addWidget(OkButt);
  connect(OkButt, SIGNAL(clicked()), SLOT(slotOK()));
  QPushButton *ApplyButt = new QPushButton(tr("Apply"));
  ButtsLayout->addWidget(ApplyButt);
  connect(ApplyButt, SIGNAL(clicked()), SLOT(slotApply()));
  QPushButton *CancelButt = new QPushButton(tr("Cancel"));
  ButtsLayout->addWidget(CancelButt);
  connect(CancelButt, SIGNAL(clicked()), SLOT(slotCancel()));

  OkButt->setDefault(true);


  // ...........................................................
  // put all data files into ComboBox
  QFileInfo Info(defaultDataSet);
  QDir ProjDir(Info.absolutePath());
  QStringList entries;
  entries<<"*.dat"<<"*.dat.ngspice"<<"*.dat.xyce"<<"*.dat.spopus";
  QStringList Elements = ProjDir.entryList(entries, QDir::Files, QDir::Name);
  QStringList::iterator it;
  for(it = Elements.begin(); it != Elements.end(); ++it) {
      if (it->endsWith(".dat")) {
          QString dat = (*it).left((*it).length()-4);
          if (ChooseData->findText(dat)<0)
              ChooseData->addItem((*it).left((*it).length()-4));
      } else {
          QString ext = (*it).section('.',-2,-1); // double extension
          int extl = ext.length() + 1; // full extension length
          int shextl = extl - 4; // extension length without ".dat"
          QString dat = (*it).left((*it).length()-extl);
          if (ChooseData->findText(dat)<0)
              ChooseData->addItem((*it).left((*it).length()-extl));
          if((*it).left((*it).length()-shextl) == Info.fileName()) // default dataset should be the current
            ChooseData->setCurrentIndex(ChooseData->count()-1);
      }

    if((*it) == Info.fileName())
      // default dataset should be the current
      ChooseData->setCurrentIndex(ChooseData->count()-1);
  }
  slotReadVarsAndSetSimulator(0);  // put variables into the ListView

  // ...........................................................
  // put all graphs into the ListBox
  Row = 0;
  foreach(Graph *pg, Diag->Graphs) {
    GraphList->insertItem(Row, pg->Var);
    if(pg == currentGraph) {
      GraphList->setCurrentRow(Row);   // select current graph
      SelectGraph(currentGraph);
    }
    Row++;
  }

  if(ColorButt) {
    if(!currentGraph) {
      QColor selectedColor(DefaultColors[GraphList->count()%NumDefaultColors]);
      QString stylesheet = QString("QPushButton {background-color: %1};").arg(selectedColor.name());
      ColorButt->setStyleSheet(stylesheet);
      misc::setPickerColor(ColorButt, selectedColor);
    }
  }

}

DiagramDialog::~DiagramDialog()
{
  delete all;   // delete all widgets from heap
  delete ValInteger;
  delete ValDouble;
  delete Validator;
}


void DiagramDialog::slotReadVarsAndSetSimulator(int)
{
    QFileInfo Info(defaultDataSet);
    QString DocName = ChooseData->currentText()+".dat";

    QString curr_sim;
    switch (QucsSettings.DefaultSimulator) {
    case spicecompat::simQucsator: curr_sim = "Qucsator (built-in)";
        break;
    case spicecompat::simNgspice: curr_sim = "Ngspice";
        break;
    case spicecompat::simXyceSer:
    case spicecompat::simXycePar: curr_sim = "Xyce";
        break;
    case spicecompat::simSpiceOpus: curr_sim = "SpiceOpus";
        break;
    default: curr_sim = ChooseSimulator->currentText();
    }

    // Recreate items of ChooseSimulator. Only existing datasets
    // should be shown
    ChooseSimulator->blockSignals(true); // Lock signals firing
    ChooseSimulator->clear();
    Info.setFile(Info.absolutePath() + QDir::separator() + DocName);
    if (Info.exists()) ChooseSimulator->addItem("Qucsator (built-in)");
    Info.setFile(Info.absolutePath() + QDir::separator() + DocName + ".ngspice");
    if (Info.exists()) ChooseSimulator->addItem("Ngspice");
    Info.setFile(Info.absolutePath() + QDir::separator() + DocName + ".xyce");
    if (Info.exists()) ChooseSimulator->addItem("Xyce");
    Info.setFile(Info.absolutePath() + QDir::separator() + DocName + ".spopus");
    if (Info.exists()) ChooseSimulator->addItem("SpiceOpus");
    int sim_pos = ChooseSimulator->findText(curr_sim); // set default simulator if possible
    if (sim_pos>=0) ChooseSimulator->setCurrentIndex(sim_pos);
    ChooseSimulator->blockSignals(false); // Unlock signals

    slotReadVars(0);
}

// --------------------------------------------------------------------------
void DiagramDialog::slotReadVars(int)
{
  QFileInfo Info(defaultDataSet);
  QString DocName = ChooseData->currentText()+".dat";

  if (ChooseSimulator->currentText()=="Ngspice") {
      DocName += ".ngspice";
  } else if (ChooseSimulator->currentText()=="Xyce") {
      DocName += ".xyce";
  } else if (ChooseSimulator->currentText()=="SpiceOpus") {
      DocName += ".spopus";
  }

  QFile file(Info.absolutePath() + QDir::separator() + DocName);
  if(!file.open(QIODevice::ReadOnly)) {
    return;
  }

  QString Line, tmp, Var;
  int varNumber = 0;
  // reading the file as a whole improves speed very much, also using
  // a QByteArray rather than a QString
  QByteArray FileString = file.readAll();
  file.close();

  // make sure sorting is disabled before inserting items
  ChooseVars->setSortingEnabled(false);
  ChooseVars->clearContents();
  ChooseXVar->clear();
  ChooseXVar->addItem("default");

  int i=0, j=0;
  i = FileString.indexOf('<')+1;
  if(i > 0)
  do {
    j = FileString.indexOf('>', i);
    for(int k=0;k<j-i;k++) Line[k]=FileString[k+i];
    Line.truncate(j-i);
    i = FileString.indexOf('<', j)+1;

    Var = Line.section(' ', 1, 1).remove('>');
    if(Var.length()>0)
      if(Var.at(0) == '_')  continue;

    if(Line.left(3) == "dep") {
      tmp = Line.section(' ', 2);
      //new Q3ListViewItem(ChooseVars, Var, "dep", tmp.remove('>'));
      qDebug() << varNumber << Var << tmp.remove('>');
      ChooseVars->setRowCount(varNumber+1);
      QTableWidgetItem *cell = new QTableWidgetItem(Var);
      ChooseXVar->addItem(Var);
      cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
      ChooseVars->setItem(varNumber, 0, cell);
      cell = new QTableWidgetItem("dep");
      cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
      ChooseVars->setItem(varNumber, 1, cell);
      cell = new QTableWidgetItem(tmp.remove('>'));
      cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
      ChooseVars->setItem(varNumber, 2, cell);
      varNumber++;
    }
    else if(Line.left(5) == "indep") {
      tmp = Line.section(' ', 2, 2);
      //new Q3ListViewItem(ChooseVars, Var, "indep", tmp.remove('>'));
      qDebug() << varNumber << Var << tmp.remove('>');
      ChooseVars->setRowCount(varNumber+1);
      QTableWidgetItem *cell = new QTableWidgetItem(Var);
      ChooseXVar->addItem(Var);
      cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
      ChooseVars->setItem(varNumber, 0, cell);
      cell = new QTableWidgetItem("indep");
      cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
      ChooseVars->setItem(varNumber, 1, cell);
      cell = new QTableWidgetItem(tmp.remove('>'));
      cell->setFlags(cell->flags() ^ Qt::ItemIsEditable);
      ChooseVars->setItem(varNumber, 2, cell);
      varNumber++;

    }
  } while(i > 0);
  // sorting should be enabled only after adding items
  ChooseVars->setSortingEnabled(true);
}

// ------------------------------------------------------------------------
// Inserts the double-clicked variable into the Graph Input Line at the
// cursor position. If the Graph Input is empty, then the variable is
// also inserted as graph.
void DiagramDialog::slotTakeVar(QTableWidgetItem* Item)
{
  GraphInput->blockSignals(true);
  if(toTake) GraphInput->setText("");

  GraphInput->cursorPosition();
  //QString s="";
  //QString s1 = Item->text();
  int row = Item->row();
  QString s1 = ChooseVars->item(row, 0)->text();
  QFileInfo Info(defaultDataSet);
  if(ChooseData->currentText() != Info.baseName())
    s1 = ChooseData->currentText() + ":" + s1;
  if (ChooseSimulator->currentText()=="Ngspice") {
      s1 = "ngspice/" + s1;
  } else if (ChooseSimulator->currentText()=="Xyce") {
      s1 = "xyce/" + s1;
  } else if (ChooseSimulator->currentText()=="SpiceOpus") {
      s1 = "spopus/" + s1;
  }
  GraphInput->setText(s1);
  updateXVar();

  //if(s.isEmpty()) {
    GraphList->addItem(GraphInput->text());////addItem(i, GraphInput->text());
    GraphList->setCurrentRow(GraphList->count()-1);

    Graph *g = new Graph(Diag, GraphInput->text());   // create a new graph

    if(Diag->Name != "Tab") {
      if(Diag->Name != "Truth") {
        g->Color = misc::getWidgetBackgroundColor(ColorButt);
        g->Thick = Property2->text().toInt();
        QColor selectedColor(DefaultColors[GraphList->count()%NumDefaultColors]);
        QString stylesheet = QString("QPushButton {background-color: %1};").arg(selectedColor.name());
        ColorButt->setStyleSheet(stylesheet);
        misc::setPickerColor(ColorButt, selectedColor);
        if(g->Var.right(3) == ".Vb")   // harmonic balance output ?
          if(PropertyBox->count() >= GRAPHSTYLE_ARROW)
            PropertyBox->setCurrentIndex(GRAPHSTYLE_ARROW);
        g->Style = toGraphStyle(PropertyBox->currentIndex());
        assert(g->Style!=GRAPHSTYLE_INVALID);
        if(yAxisBox) {
          g->yAxisNo = yAxisBox->currentIndex();
          yAxisBox->setEnabled(true);
          Label4->setEnabled(true);
        }
        else if(Diag->Name == "Rect3D") g->yAxisNo = 1;

        Label3->setEnabled(true);
        ColorButt->setEnabled(true);
      }
    }
    else {
      g->Precision = Property2->text().toInt();
      g->numMode   = PropertyBox->currentIndex();
    }

    Graphs.append(g);
    changed = true;
    toTake  = true;
  //}

  GraphInput->blockSignals(false);

  if(Property2) {
    Label1->setEnabled(true);
    PropertyBox->setEnabled(true);
    Label2->setEnabled(true);
    Property2->setEnabled(true);
  }
}

/*!
  Is called if a graph text is clicked in the ListBox.
*/
void DiagramDialog::slotSelectGraph(QListWidgetItem *item)
{
  if(item == 0) {
    GraphList->clearSelection();
    return;
  }

  SelectGraph (Graphs.at (GraphList->currentRow()));
}

/*!
  Puts the text of the selected graph into the line edit.
*/
void DiagramDialog::SelectGraph(Graph *g)
{
  GraphInput->blockSignals(true);
  GraphInput->setText(g->Var);
  GraphInput->blockSignals(false);
  updateXVar();

  if(Diag->Name != "Tab") {
    if(Diag->Name != "Truth") {
      Property2->setText(QString::number(g->Thick));
      QString stylesheet = QString("QPushButton {background-color: %1};").arg(g->Color.name());
      ColorButt->setStyleSheet(stylesheet);
      misc::setPickerColor(ColorButt,g->Color);
      PropertyBox->setCurrentIndex(g->Style);
      if(yAxisBox) {
        yAxisBox->setCurrentIndex(g->yAxisNo);
        yAxisBox->setEnabled(true);
        Label4->setEnabled(true);
      }

      Label3->setEnabled(true);
      ColorButt->setEnabled(true);
    }
  }
  else {
    Property2->setText(QString::number(g->Precision));
    PropertyBox->setCurrentIndex(g->numMode);
  }
  toTake = false;

  if(Property2) {
    Label1->setEnabled(true);
    PropertyBox->setEnabled(true);
    Label2->setEnabled(true);
    Property2->setEnabled(true);
  }
}

/*!
 Is called when the 'delelte graph' button is pressed.
*/
void DiagramDialog::slotDeleteGraph()
{
  int i = GraphList->currentRow();
  if(i < 0) return;   // return, if no item selected

  GraphList->takeItem(i);
  Graphs.remove(i);

  int k=0;
  if (GraphList->count()!=0) {
      if (i>(GraphList->count()-1)) {
          k = GraphList->count()-1;
      } else {
          k=i;
      }
      GraphInput->setText(GraphList->item(k)->text());
  } else {
      GraphInput->setText("");  // erase input line and back to default values
  }

  if(Diag->Name != "Tab") {
    if(Diag->Name != "Truth") {
      QColor selectedColor(DefaultColors[GraphList->count()%NumDefaultColors]);
      QString stylesheet = QString("QPushButton {background-color: %1};").arg(selectedColor.name());
      ColorButt->setStyleSheet(stylesheet);
      misc::setPickerColor(ColorButt,selectedColor);
      Property2->setText("0");
      if(yAxisBox) {
        yAxisBox->setCurrentIndex(0);
        yAxisBox->setEnabled(false);
        Label4->setEnabled(false);
      }

      Label3->setEnabled(false);
      ColorButt->setEnabled(false);
    }
  }
  else  Property2->setText("3");
  changed = true;
  toTake  = false;

  if(Property2) {
    PropertyBox->setCurrentIndex(0);

    Label1->setEnabled(false);
    PropertyBox->setEnabled(false);
    Label2->setEnabled(false);
    Property2->setEnabled(false);
  }
}

// --------------------------------------------------------------------------
void DiagramDialog::slotNewGraph()
{
  assert(Diag);
  if(GraphInput->text().isEmpty()) return;

  GraphList->addItem(GraphInput->text());

  Graph *g = new Graph(Diag, GraphInput->text());
// FIXME: call  Diag->whateverelse();
  if(Diag->Name != "Tab") { // BUG
    if(Diag->Name != "Truth") { // BUG
      g->Color = misc::getWidgetBackgroundColor(ColorButt);
      g->Thick = Property2->text().toInt();
      g->Style = toGraphStyle(PropertyBox->currentIndex());
      assert(g->Style!=GRAPHSTYLE_INVALID);
      if(yAxisBox)  g->yAxisNo = yAxisBox->currentIndex();
      else if(Diag->Name == "Rect3D")  g->yAxisNo = 1;
    }
  }
  else {
    g->Precision = Property2->text().toInt();
    g->numMode   = PropertyBox->currentIndex();
  }
  Graphs.append(g);
  changed = true;
  toTake  = false;
}

/*!
 Is called if "Ok" button is pressed.
*/
void DiagramDialog::slotOK()
{
  slotApply();
  slotCancel();
}

/*!
 Is called if "Apply" button is pressed.
*/
void DiagramDialog::slotApply()
{
  if(Diag->Name.at(0) != 'T') {  // not tabular or timing
    if(Diag->xAxis.Label.isEmpty())
      Diag->xAxis.Label = ""; // can be not 0 and empty!
    if(xLabel->text().isEmpty()) xLabel->setText("");
    if(Diag->xAxis.Label != xLabel->text()) {
      Diag->xAxis.Label = xLabel->text();
      changed = true;
    }
    if(Diag->yAxis.Label.isEmpty())
      Diag->yAxis.Label = ""; // can be not 0 and empty!
    if(ylLabel->text().isEmpty()) ylLabel->setText("");
    if(Diag->yAxis.Label != ylLabel->text()) {
      Diag->yAxis.Label = ylLabel->text();
      changed = true;
    }

    if (NotationBox) {
        bool notation = false;
        switch (NotationBox->currentIndex()) {
        case 0: notation = false;
            break;
        case 1: notation = true;
            break;
        default: break;
        }
        if (Diag->engineeringNotation != notation) changed = true;
        Diag->engineeringNotation = notation;
    }

    auto yUnit = Diag->yAxis.Units;
    if (yUnit != LogUnitsY->currentIndex()) {
        Diag->yAxis.Units = LogUnitsY->currentIndex();
        changed = true;
    }

    auto zUnit = Diag->zAxis.Units;
    if (zUnit != LogUnitsZ->currentIndex()) {
        Diag->zAxis.Units = LogUnitsZ->currentIndex();
        changed = true;
    }
    
    if(GridOn) if(Diag->xAxis.GridOn != GridOn->isChecked()) {
      Diag->xAxis.GridOn = GridOn->isChecked();
      Diag->yAxis.GridOn = GridOn->isChecked();
      changed = true;
    }
    if(GridColorButt)
      if(Diag->GridPen.color() != misc::getWidgetBackgroundColor(GridColorButt)) {
      Diag->GridPen.setColor(misc::getWidgetBackgroundColor(GridColorButt));
      changed = true;
    }
    if(GridStyleBox)
      if(Diag->GridPen.style()!=(Qt::PenStyle)(GridStyleBox->currentIndex()+1)) {
      Diag->GridPen.setStyle((Qt::PenStyle)(GridStyleBox->currentIndex()+1));
      changed = true;
    }
    if((Diag->Name != "Smith") && (Diag->Name != "Polar")) {
      if(Diag->zAxis.Label.isEmpty())
        Diag->zAxis.Label = ""; // can be not 0 and empty!
      if(yrLabel->text().isEmpty()) yrLabel->setText("");
      if(Diag->zAxis.Label != yrLabel->text()) {
        Diag->zAxis.Label = yrLabel->text();
        changed = true;
      }
    }

    if(Diag->Name.left(4) == "Rect") {
      if(Diag->xAxis.log != GridLogX->isChecked()) {
        Diag->xAxis.log = GridLogX->isChecked();
        changed = true;
      }
      if(Diag->yAxis.log != GridLogY->isChecked()) {
        Diag->yAxis.log = GridLogY->isChecked();
        changed = true;
      }
      if(Diag->zAxis.log != GridLogZ->isChecked()) {
        Diag->zAxis.log = GridLogZ->isChecked();
        changed = true;
      }
    }

    if((Diag->Name == "Smith") || (Diag->Name == "ySmith") ||
				  (Diag->Name == "PS"))
      if(stopY->text().toDouble() < 1.0)
        stopY->setText("1");

    if(Diag->Name == "SP")
      if(stopZ->text().toDouble() < 1.0)
        stopZ->setText("1");

    if(Diag->xAxis.autoScale == manualX->isChecked()) {
      Diag->xAxis.autoScale = !(manualX->isChecked());
      changed = true;
    }

    // Use string compares for all floating point numbers, in
    // order to avoid rounding problems.
    if(QString::number(Diag->xAxis.limit_min) != startX->text()) {
      Diag->xAxis.limit_min = startX->text().toDouble();
      changed = true;
    }
    if(QString::number(Diag->xAxis.step) != stepX->text()) {
      Diag->xAxis.step = stepX->text().toDouble();
      changed = true;
    }
    if(QString::number(Diag->xAxis.limit_max) != stopX->text()) {
      Diag->xAxis.limit_max = stopX->text().toDouble();
      changed = true;
    }
    if(Diag->yAxis.autoScale == manualY->isChecked()) {
      Diag->yAxis.autoScale = !(manualY->isChecked());
      changed = true;
    }
    if(QString::number(Diag->yAxis.limit_min) != startY->text()) {
      Diag->yAxis.limit_min = qucs::db2num(startY->text().toDouble(),Diag->yAxis.Units);
      changed = true;
    }
    if(QString::number(Diag->yAxis.step) != stepY->text()) {
      Diag->yAxis.step = qucs::db2num(stepY->text().toDouble(),Diag->yAxis.Units);
      changed = true;
    }
    if(QString::number(Diag->yAxis.limit_max) != stopY->text()) {
      Diag->yAxis.limit_max = qucs::db2num(stopY->text().toDouble(),Diag->yAxis.Units);
      changed = true;
    }
    if(Diag->zAxis.autoScale == manualZ->isChecked()) {
      Diag->zAxis.autoScale = !(manualZ->isChecked());
      changed = true;
    }
    if(QString::number(Diag->zAxis.limit_min) != startZ->text()) {
      Diag->zAxis.limit_min = qucs::db2num(startZ->text().toDouble(),Diag->zAxis.Units);
      changed = true;
    }
    if(QString::number(Diag->zAxis.step) != stepZ->text()) {
      Diag->zAxis.step = qucs::db2num(stepZ->text().toDouble(),Diag->zAxis.Units);
      changed = true;
    }
    if(QString::number(Diag->zAxis.limit_max) != stopZ->text()) {
      Diag->zAxis.limit_max = qucs::db2num(stopZ->text().toDouble(),Diag->zAxis.Units);
      changed = true;
    }

    // for "rect3D"
    if(hideInvisible)
      if(((Rect3DDiagram*)Diag)->hideLines != hideInvisible->isChecked()) {
        ((Rect3DDiagram*)Diag)->hideLines = hideInvisible->isChecked();
        changed = true;
      }

    if(rotationX)
      if(((Rect3DDiagram*)Diag)->rotX != rotationX->text().toInt()) {
        ((Rect3DDiagram*)Diag)->rotX = rotationX->text().toInt();
        changed = true;
      }

    if(rotationY)
      if(((Rect3DDiagram*)Diag)->rotY != rotationY->text().toInt()) {
        ((Rect3DDiagram*)Diag)->rotY = rotationY->text().toInt();
        changed = true;
      }

    if(rotationZ)
      if(((Rect3DDiagram*)Diag)->rotZ != rotationZ->text().toInt()) {
        ((Rect3DDiagram*)Diag)->rotZ = rotationZ->text().toInt();
        changed = true;
      }

  }   // of "if(Diag->Name != "Tab")"

  Diag->Graphs.clear();   // delete the graphs
  Graphs.setAutoDelete(false);
  for(Graph *pg = Graphs.first(); pg != 0; pg = Graphs.next())
    Diag->Graphs.append(pg);  // transfer the new graphs to diagram
  Graphs.clear();
  Graphs.setAutoDelete(true);

  Diag->loadGraphData(defaultDataSet);
  ((Schematic*)parent())->viewport()->repaint();
  copyDiagramGraphs();
  if(changed) transfer = true;   // changes have been applied ?
}


// --------------------------------------------------------------------------
// Is called if "Cancel" button is pressed.
void DiagramDialog::slotCancel()
{
//  Diag->loadGraphData(defaultDataSet);
//  ((QucsView*)parent())->viewport()->repaint();
  if(transfer) done(QDialog::Accepted);
  else done(QDialog::Rejected);
}

//-----------------------------------------------------------------
// To get really all close events (even <Escape> key).
void DiagramDialog::reject()
{
  slotCancel();
}

// --------------------------------------------------------------------------
void DiagramDialog::slotSetColor()
{
  QColor c = QColorDialog::getColor(misc::getWidgetBackgroundColor(ColorButt),this);
  if(!c.isValid()) return;
  QString stylesheet = QString("QPushButton {background-color: %1};").arg(c.name());
  ColorButt->setStyleSheet(stylesheet);
  misc::setPickerColor(ColorButt,c);

  int i = GraphList->currentRow();
  if(i < 0) return;   // return, if no item selected

  Graph *g = Graphs.at(i);
  g->Color = c;
  changed = true;
  toTake  = false;
}

// --------------------------------------------------------------------------
void DiagramDialog::slotSetGridColor()
{
  QColor c = QColorDialog::getColor(
            misc::getWidgetBackgroundColor(GridColorButt),this);
  if(!c.isValid()) return;
  misc::setPickerColor(GridColorButt,c);
  changed = true;
}

/*!
 Is set if the graph input line changes.
*/
void DiagramDialog::slotResetToTake(const QString& s)
{
  int i = GraphList->currentRow();
  if(i < 0) return;   // return, if no item selected

  Graph *g = Graphs.at(i);
  g->Var = s;
  // \todo GraphList->changeItem(s, i);   // must done after the graph settings !!!
  changed = true;
  toTake  = false;
  updateXVar();
}

/*!
 Is called if the user changes the graph thickness or the precision.
*/
void DiagramDialog::slotSetProp2(const QString& s)
{
  int i = GraphList->currentRow();
  if(i < 0) return;   // return, if no item selected

  Graph *g = Graphs.at(i);
  if(Diag->Name == "Tab") g->Precision = s.toInt();
  else  g->Thick = s.toInt();
  changed = true;
  toTake  = false;
}

/*!
 Is called if the user changes the number mode.
*/
void DiagramDialog::slotSetNumMode(int Mode)
{
  int i = GraphList->currentRow();
  if(i < 0) return;   // return, if no item selected

  Graph *g = Graphs.at(i);
  g->numMode = Mode;
  changed = true;
  toTake  = false;
}

/*!
 Is called when the "show grid" checkbox is changed.
*/
void DiagramDialog::slotSetGridBox(int state)
{
  if(state == 2) {
    GridColorButt->setEnabled(true);
    GridStyleBox->setEnabled(true);
    GridLabel1->setEnabled(true);
    GridLabel2->setEnabled(true);
  }
  else {
    GridColorButt->setEnabled(false);
    GridStyleBox->setEnabled(false);
    GridLabel1->setEnabled(false);
    GridLabel2->setEnabled(false);
  }
}

/*!
 Is called if the user changes the graph style (combobox).
*/
void DiagramDialog::slotSetGraphStyle(int style)
{
  int i = GraphList->currentRow();
  if(i < 0) return;   // return, if no item selected

  Graph *g = Graphs.at(i);
  g->Style = toGraphStyle(style);
  assert(g->Style!=GRAPHSTYLE_INVALID);
  changed = true;
  toTake  = false;
}

/*!
 Makes a copy of all graphs in the diagram.
*/
void DiagramDialog::copyDiagramGraphs()
{
  foreach(Graph *pg, Diag->Graphs)
    Graphs.append(pg->sameNewOne());
}

/*!
 Is called if the combobox changes that defines which y axis uses the graph.
*/
void DiagramDialog::slotSetYAxis(int axis)
{
  int i = GraphList->currentRow();
  if(i < 0) return;   // return, if no item selected

  Graph *g = Graphs.at(i);
  g->yAxisNo = axis;
  changed = true;
  toTake  = false;
}

// --------------------------------------------------------------------------
void DiagramDialog::slotManualX(int state)
{
  if(state == 2) {
    if((Diag->Name.left(4) == "Rect") || (Diag->Name == "Curve"))
      startX->setEnabled(true);
    stopX->setEnabled(true);
    if(GridLogX) if(GridLogX->isChecked())  return;
    stepX->setEnabled(true);
  }
  else {
    startX->setEnabled(false);
    stepX->setEnabled(false);
    stopX->setEnabled(false);
  }
}

// --------------------------------------------------------------------------
void DiagramDialog::slotManualY(int state)
{
  if(state == 2) {
    if((Diag->Name.left(4) == "Rect") || (Diag->Name == "Curve"))
      startY->setEnabled(true);
    stopY->setEnabled(true);
    if(GridLogY) if(GridLogY->isChecked())  return;
    stepY->setEnabled(true);
  }
  else {
    startY->setEnabled(false);
    stepY->setEnabled(false);
    stopY->setEnabled(false);
  }
}

// --------------------------------------------------------------------------
void DiagramDialog::slotManualZ(int state)
{
  if(state == 2) {
    if((Diag->Name.left(4) == "Rect") || (Diag->Name == "Curve"))
      startZ->setEnabled(true);
    stopZ->setEnabled(true);
    if(GridLogZ) if(GridLogZ->isChecked())  return;
    stepZ->setEnabled(true);
  }
  else {
    startZ->setEnabled(false);
    stepZ->setEnabled(false);
    stopZ->setEnabled(false);
  }
}

/*!
 Is called if the current tab of the QTabWidget changes.
*/
void DiagramDialog::slotChangeTab(QWidget*)
{
  if(stepX == 0) return;   // defined ?
  if(GridLogX) {
    if(GridLogX->isChecked())  stepX->setEnabled(false);
    else  if(manualX->isChecked())  stepX->setEnabled(true);
  }
  if(GridLogY) {
    if(GridLogY->isChecked())  stepY->setEnabled(false);
    else  if(manualY->isChecked())  stepY->setEnabled(true);
  }
  if(GridLogZ) {
    if(GridLogZ->isChecked())  stepZ->setEnabled(false);
    else  if(manualZ->isChecked())  stepZ->setEnabled(true);
  }
}


/*!
 Is called when the slider for rotation angle is changed.
*/
void DiagramDialog::slotNewRotX(int Value)
{
  rotationX->setText(QString::number(Value));
  DiagCross->rotX = float(Value) * pi/180.0;
  DiagCross->update();
}

/*!
 Is called when the slider for rotation angle is changed.
*/
void DiagramDialog::slotNewRotY(int Value)
{
  rotationY->setText(QString::number(Value));
  DiagCross->rotY = float(Value) * pi/180.0;
  DiagCross->update();
}

/*!
 Is called when the slider for rotation angle is changed.
*/
void DiagramDialog::slotNewRotZ(int Value)
{
  rotationZ->setText(QString::number(Value));
  DiagCross->rotZ = float(Value) * pi/180.0;
  DiagCross->update();
}

/*!
 Is called when the number (text) for rotation angle is changed.
*/
void DiagramDialog::slotEditRotX(const QString& Text)
{
  SliderRotX->setValue(Text.toInt());
  DiagCross->rotX = Text.toFloat() * pi/180.0;
  DiagCross->update();
}

/*!
 Is called when the number (text) for rotation angle is changed.
*/
void DiagramDialog::slotEditRotY(const QString& Text)
{
  SliderRotY->setValue(Text.toInt());
  DiagCross->rotY = Text.toFloat() * pi/180.0;
  DiagCross->update();
}

/*!
 Is called when the number (text) for rotation angle is changed.
*/
void DiagramDialog::slotEditRotZ(const QString& Text)
{
  SliderRotZ->setValue(Text.toInt());
  DiagCross->rotZ = Text.toFloat() * pi/180.0;
  DiagCross->update();
}

void DiagramDialog::slotPlotVs(int)
{
    QString s = GraphInput->text();
    s.remove(QRegExp("@.*$")); // remove all after "@" symbol
    if (ChooseXVar->currentIndex()!=0) {
        s += "@" + ChooseXVar->currentText();
    }
    GraphInput->setText(s);
}

void DiagramDialog::updateXVar()
{
    ChooseXVar->blockSignals(true);
    QString s = GraphInput->text();
    if (s.contains("@")) {
        QString xvar = s.section("@",1,1);
        int n = ChooseXVar->findText(xvar);
        if (n != -1) ChooseXVar->setCurrentIndex(n);
        else ChooseXVar->setCurrentIndex(0);
    } else {
        ChooseXVar->setCurrentIndex(0);
    }
    ChooseXVar->blockSignals(false);
}

void DiagramDialog::slotRecalcDbLimitsY()
{
    int Units = LogUnitsY->currentIndex();
    startY->setText(QString::number(qucs::num2db(Diag->yAxis.limit_min, Units)));
    stepY->setText(QString::number(qucs::num2db(Diag->yAxis.step, Units)));
    stopY->setText(QString::number(qucs::num2db(Diag->yAxis.limit_max, Units)));
}


void DiagramDialog::slotRecalcDbLimitsZ()
{
    int Units = LogUnitsZ->currentIndex();
    startZ->setText(QString::number(qucs::num2db(Diag->zAxis.limit_min, Units)));
    stepZ->setText(QString::number(qucs::num2db(Diag->zAxis.step, Units)));
    stopZ->setText(QString::number(qucs::num2db(Diag->zAxis.limit_max, Units)));
}
