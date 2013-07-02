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
#include <QtGui>
#include "diagramdialog.h"
#include "qucs.h"
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3VBoxLayout>
#include <QPaintEvent>
#include "schematic.h"
#include "rect3ddiagram.h"

#include <math.h>

#include <q3vbox.h>
#include <qlayout.h>
//#include <q3hbuttongroup.h>
#include <q3buttongroup.h>
#include <q3hgroupbox.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qlabel.h>
#include <qstringlist.h>
#include <qmessagebox.h>
#include <q3ptrlist.h>
#include <qvalidator.h>
#include <qcolordialog.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qslider.h>
#include <qcombobox.h>
#include <q3listview.h>
#include <q3listbox.h>


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


DiagramDialog::DiagramDialog(Diagram *d, const QString& _DataSet,
                             QWidget *parent, Graph *currentGraph)
                    : QDialog(parent, 0, TRUE, Qt::WDestructiveClose)
{
  Diag = d;
  Graphs.setAutoDelete(true);
  copyDiagramGraphs();   // make a copy of all graphs
  defaultDataSet = _DataSet;
  setCaption(tr("Edit Diagram Properties"));
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

  
  all = new Q3VBoxLayout(this); // to provide neccessary size
  QTabWidget *t = new QTabWidget(this);
  all->addWidget(t);

  // ...........................................................
  Q3VBox *Tab1 = new Q3VBox(this);
  Tab1->setSpacing(5);

  Label4 = 0;     // different types with same content
  yrLabel = 0;
  yAxisBox = 0;
  Property2 = 0;
  ColorButt = 0;
  hideInvisible = 0;
  rotationX = rotationY = rotationZ = 0;

  Q3VButtonGroup *InputGroup = new Q3VButtonGroup(tr("Graph Input"), Tab1);
  GraphInput = new QLineEdit(InputGroup);
  GraphInput->setValidator(Validator);
  connect(GraphInput, SIGNAL(textChanged(const QString&)),
		      SLOT(slotResetToTake(const QString&)));
  Q3HBox *Box2 = new Q3HBox(InputGroup);
  Box2->setSpacing(5);

  if(Diag->Name == "Tab") {
    Label1 = new QLabel(tr("Number Notation: "), Box2);
    PropertyBox = new QComboBox(Box2);
    PropertyBox->insertItem(tr("real/imaginary"));
    PropertyBox->insertItem(tr("magnitude/angle (degree)"));
    PropertyBox->insertItem(tr("magnitude/angle (radian)"));
    PropertyBox->setCurrentItem(1);
    connect(PropertyBox, SIGNAL(activated(int)), SLOT(slotSetNumMode(int)));
    Box2->setStretchFactor(new QWidget(Box2), 5); // stretchable placeholder

    Label2 = new QLabel(tr("Precision:"), Box2);
    Property2 = new QLineEdit(Box2);
    Property2->setValidator(ValInteger);
    Property2->setMaxLength(2);
    Property2->setMaximumWidth(25);
    Property2->setText("3");
  }
  else if(Diag->Name != "Truth") {
    Label1 = new QLabel(tr("Color:"),Box2);
    ColorButt = new QPushButton("   ",Box2);
    ColorButt->setMinimumWidth(50);
    ColorButt->setEnabled(false);
    connect(ColorButt, SIGNAL(clicked()), SLOT(slotSetColor()));
    Box2->setStretchFactor(new QWidget(Box2), 5); // stretchable placeholder

    Label3 = new QLabel(tr("Style:"),Box2);
    Label3->setEnabled(false);
    PropertyBox = new QComboBox(Box2);
    PropertyBox->insertItem(tr("solid line"));
    PropertyBox->insertItem(tr("dash line"));
    PropertyBox->insertItem(tr("dot line"));
    if(Diag->Name != "Time") {
      PropertyBox->insertItem(tr("long dash line"));
      PropertyBox->insertItem(tr("stars"));
      PropertyBox->insertItem(tr("circles"));
      PropertyBox->insertItem(tr("arrows"));
    }
    connect(PropertyBox, SIGNAL(activated(int)),
			 SLOT(slotSetGraphStyle(int)));
    Box2->setStretchFactor(new QWidget(Box2), 5); // stretchable placeholder

    Label2 = new QLabel(tr("Thickness:"),Box2);
    Property2 = new QLineEdit(Box2);
    Property2->setValidator(ValInteger);
    Property2->setMaximumWidth(25);
    Property2->setMaxLength(2);
    Property2->setText("0");

    if((Diag->Name=="Rect") || (Diag->Name=="PS") || (Diag->Name=="SP") ||
       (Diag->Name=="Curve")) {
      Q3HBox *Box3 = new Q3HBox(InputGroup);
      Box3->setSpacing(5);

      Label4 = new QLabel(tr("y-Axis:"),Box3);
      Label4->setEnabled(false);
      yAxisBox = new QComboBox(Box3);
      yAxisBox->insertItem(NameY);
      yAxisBox->insertItem(NameZ);
      yAxisBox->setEnabled(false);
      connect(yAxisBox, SIGNAL(activated(int)), SLOT(slotSetYAxis(int)));
      Box3->setStretchFactor(new QWidget(Box3), 5); // stretchable placeholder
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

  Q3HBox *Box1 = new Q3HBox(Tab1);
  Box1->setSpacing(5);

  Q3VButtonGroup *DataGroup = new Q3VButtonGroup(tr("Dataset"), Box1);
  ChooseData = new QComboBox(false, DataGroup);
  ChooseData->setMinimumWidth(200);
  connect(ChooseData, SIGNAL(activated(int)), SLOT(slotReadVars(int)));
  ChooseVars = new Q3ListView(DataGroup);
  ChooseVars->addColumn(tr("Name"));
  ChooseVars->addColumn(tr("Type"));
  ChooseVars->addColumn(tr("Size"));
  connect(ChooseVars, SIGNAL(doubleClicked(Q3ListViewItem*)),
		      SLOT(slotTakeVar(Q3ListViewItem*)));


  Q3VButtonGroup *GraphGroup = new Q3VButtonGroup(tr("Graph"), Box1);
  GraphList = new Q3ListBox(GraphGroup);
  connect(GraphList, SIGNAL(clicked(Q3ListBoxItem*)),
		     SLOT(slotSelectGraph(Q3ListBoxItem*)));
  connect(GraphList, SIGNAL(doubleClicked(Q3ListBoxItem*)),
		     SLOT(slotDeleteGraph()));
  QPushButton *NewButt = new QPushButton(tr("New Graph"), GraphGroup);
  connect(NewButt, SIGNAL(clicked()), SLOT(slotNewGraph()));
  QPushButton *DelButt = new QPushButton(tr("Delete Graph"), GraphGroup);
  connect(DelButt, SIGNAL(clicked()), SLOT(slotDeleteGraph()));

  t->addTab(Tab1, tr("Data"));

  // ...........................................................
  int Row = 0;
  if(Diag->Name.at(0) != 'T') {  // not tabular or timing diagram
    QWidget *Tab2 = new QWidget(t);
    Q3GridLayout *gp = new Q3GridLayout(Tab2,13,3,5,5);

    gp->addMultiCellWidget(new QLabel(tr("x-Axis Label:"), Tab2), Row,Row,0,0);
    xLabel = new QLineEdit(Tab2);
    xLabel->setValidator(Validator);
    gp->addMultiCellWidget(xLabel, Row,Row,1,2);
    Row++;

    gp->addMultiCellWidget(
		new QLabel(NameY+" "+tr("Label:"), Tab2), Row,Row,0,0);
    ylLabel = new QLineEdit(Tab2);
    ylLabel->setValidator(Validator);
    gp->addMultiCellWidget(ylLabel, Row,Row,1,2);
    Row++;

    if((Diag->Name != "Smith") && (Diag->Name != "Polar")) {
      gp->addMultiCellWidget(
		new QLabel(NameZ +" "+tr("Label:"), Tab2), Row,Row,0,0);
      yrLabel = new QLineEdit(Tab2);
      yrLabel->setValidator(Validator);
      gp->addMultiCellWidget(yrLabel, Row,Row,1,2);
      Row++;
    }

    gp->addMultiCellWidget(new QLabel(
        tr("<b>Label text</b>: Use LaTeX style for special characters, e.g. \\tau"),
        Tab2),  Row,Row,0,2);
    Row++;

    if(Diag->Name != "Rect3D") {
      GridOn = new QCheckBox(tr("show Grid"), Tab2);
      gp->addMultiCellWidget(GridOn, Row,Row,0,2);
      Row++;

      GridLabel1 = new QLabel(tr("Grid Color:"),Tab2);
      gp->addMultiCellWidget(GridLabel1, Row,Row,0,0);
      GridColorButt = new QPushButton("        ",Tab2);
      connect(GridColorButt, SIGNAL(clicked()), SLOT(slotSetGridColor()));
      gp->addMultiCellWidget(GridColorButt, Row,Row,1,2);
      Row++;
      GridColorButt->setPaletteBackgroundColor(Diag->GridPen.color());

      GridLabel2 = new QLabel(tr("Grid Style: "), Tab2);
      gp->addMultiCellWidget(GridLabel2, Row,Row,0,0);
      GridStyleBox = new QComboBox(Tab2);
      GridStyleBox->insertItem(tr("solid line"));
      GridStyleBox->insertItem(tr("dash line"));
      GridStyleBox->insertItem(tr("dot line"));
      GridStyleBox->insertItem(tr("dash dot line"));
      GridStyleBox->insertItem(tr("dash dot dot line"));
      gp->addMultiCellWidget(GridStyleBox, Row,Row,1,2);
      Row++;
      GridStyleBox->setCurrentItem(Diag->GridPen.style()-1);
    
      GridOn->setChecked(Diag->xAxis.GridOn);
      if(!Diag->xAxis.GridOn) slotSetGridBox(0);
      connect(GridOn, SIGNAL(stateChanged(int)), SLOT(slotSetGridBox(int)));
    }
    else {
      GridOn = 0;
      GridColorButt = 0;
      GridStyleBox = 0;
    }

    // ...........................................................
    xLabel->setText(Diag->xAxis.Label);
    ylLabel->setText(Diag->yAxis.Label);
    if(yrLabel)  yrLabel->setText(Diag->zAxis.Label);

    if((Diag->Name.left(4) == "Rect") || (Diag->Name == "Curve")) {
      GridLogX = new QCheckBox(tr("logarithmical X Axis Grid"), Tab2);
      gp->addMultiCellWidget(GridLogX, Row,Row,0,2);
      Row++;

      GridLogY = new QCheckBox(tr("logarithmical")+" "+NameY+" "+tr("Grid"), Tab2);
      gp->addMultiCellWidget(GridLogY, Row,Row,0,2);
      Row++;

      GridLogZ = new QCheckBox(tr("logarithmical")+" "+NameZ+" "+tr("Grid"), Tab2);
      gp->addMultiCellWidget(GridLogZ, Row,Row,0,2);
      Row++;

      // ...........................................................
      // transfer the diagram properties to the dialog
      GridLogX->setChecked(Diag->xAxis.log);
      GridLogY->setChecked(Diag->yAxis.log);
      GridLogZ->setChecked(Diag->zAxis.log);


      if(Diag->Name == "Rect3D") {
	hideInvisible = new QCheckBox(tr("hide invisible lines"), Tab2);
	gp->addMultiCellWidget(hideInvisible, Row,Row,0,2);
	Row++;

	QLabel *LabelRotX = new QLabel(tr("Rotation around x-Axis:"), Tab2);
	LabelRotX->setPaletteForegroundColor(Qt::red);
	gp->addWidget(LabelRotX, Row,0);
	SliderRotX = new QSlider(0,360,20, ((Rect3DDiagram*)Diag)->rotX,
				 Qt::Horizontal, Tab2);
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
	LabelRotY->setPaletteForegroundColor(Qt::green);
	gp->addWidget(LabelRotY, Row,0);
	SliderRotY = new QSlider(0,360,20, ((Rect3DDiagram*)Diag)->rotY,
				 Qt::Horizontal, Tab2);
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
	LabelRotZ->setPaletteForegroundColor(Qt::blue);
	gp->addWidget(LabelRotZ, Row,0);
	SliderRotZ = new QSlider(0,360,20, ((Rect3DDiagram*)Diag)->rotZ,
				 Qt::Horizontal, Tab2);
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

  // ...........................................................
    Q3VBox *Tab3 = new Q3VBox(this);
    Tab1->setSpacing(5);

    Q3HGroupBox *axisX = new Q3HGroupBox(tr("x-Axis"), Tab3);

    Q3VBox *VBox1 = new Q3VBox(axisX);
    VBox1->setStretchFactor(new QWidget(VBox1),5); // stretchable placeholder
    manualX = new QCheckBox(tr("manual"), VBox1);
    connect(manualX, SIGNAL(stateChanged(int)), SLOT(slotManualX(int)));

    Q3VBox *VBox2 = new Q3VBox(axisX);
    new QLabel(tr("start"), VBox2);
    startX = new QLineEdit(VBox2);
    startX->setValidator(ValDouble);

    Q3VBox *VBox3 = new Q3VBox(axisX);
    new QLabel(tr("step"), VBox3);
    stepX = new QLineEdit(VBox3);
    stepX->setValidator(ValDouble);

    Q3VBox *VBox4 = new Q3VBox(axisX);
    new QLabel(tr("stop"), VBox4);
    stopX = new QLineEdit(VBox4);
    stopX->setValidator(ValDouble);


    Q3HGroupBox *axisY;
    axisY = new Q3HGroupBox(NameY, Tab3);

    Q3VBox *VBox5 = new Q3VBox(axisY);
    VBox5->setStretchFactor(new QWidget(VBox5),5); // stretchable placeholder
    manualY = new QCheckBox(tr("manual"), VBox5);
    connect(manualY, SIGNAL(stateChanged(int)), SLOT(slotManualY(int)));

    Q3VBox *VBox6 = new Q3VBox(axisY);
    new QLabel(tr("start"), VBox6);
    startY = new QLineEdit(VBox6);
    startY->setValidator(ValDouble);

    Q3VBox *VBox7 = new Q3VBox(axisY);
    if((Diag->Name=="Smith") || (Diag->Name=="ySmith") || (Diag->Name=="PS"))
      new QLabel(tr("number"), VBox7);
    else  new QLabel(tr("step"), VBox7);
    stepY = new QLineEdit(VBox7);
    stepY->setValidator(ValDouble);

    Q3VBox *VBox8 = new Q3VBox(axisY);
    new QLabel(tr("stop"), VBox8);
    stopY = new QLineEdit(VBox8);
    stopY->setValidator(ValDouble);


    Q3HGroupBox *axisZ;
    axisZ = new Q3HGroupBox(NameZ, Tab3);

    Q3VBox *VBox9 = new Q3VBox(axisZ);
    VBox9->setStretchFactor(new QWidget(VBox9),5); // stretchable placeholder
    manualZ = new QCheckBox(tr("manual"), VBox9);
    connect(manualZ, SIGNAL(stateChanged(int)), SLOT(slotManualZ(int)));

    Q3VBox *VBox10 = new Q3VBox(axisZ);
    new QLabel(tr("start"), VBox10);
    startZ = new QLineEdit(VBox10);
    startZ->setValidator(ValDouble);

    Q3VBox *VBox11 = new Q3VBox(axisZ);
    if(Diag->Name == "SP")  new QLabel(tr("number"), VBox11);
    else  new QLabel(tr("step"), VBox11);
    stepZ = new QLineEdit(VBox11);
    stepZ->setValidator(ValDouble);

    Q3VBox *VBox12 = new Q3VBox(axisZ);
    new QLabel(tr("stop"), VBox12);
    stopZ = new QLineEdit(VBox12);
    stopZ->setValidator(ValDouble);


    Tab3->setStretchFactor(new QWidget(Tab3),5); // stretchable placeholder

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

    startY->setText(QString::number(Diag->yAxis.limit_min));
    stepY->setText(QString::number(Diag->yAxis.step));
    stopY->setText(QString::number(Diag->yAxis.limit_max));

    startZ->setText(QString::number(Diag->zAxis.limit_min));
    stepZ->setText(QString::number(Diag->zAxis.step));
    stopZ->setText(QString::number(Diag->zAxis.limit_max));

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
  Q3HBox *Butts = new Q3HBox(this);
  Butts->setSpacing(5);
  Butts->setMargin(5);
  all->addWidget(Butts);

  QPushButton *OkButt = new QPushButton(tr("OK"), Butts);
  connect(OkButt, SIGNAL(clicked()), SLOT(slotOK()));
  QPushButton *ApplyButt = new QPushButton(tr("Apply"), Butts);
  connect(ApplyButt, SIGNAL(clicked()), SLOT(slotApply()));
  QPushButton *CancelButt = new QPushButton(tr("Cancel"), Butts);
  connect(CancelButt, SIGNAL(clicked()), SLOT(slotCancel()));

  OkButt->setDefault(true);


  // ...........................................................
  // put all data files into ComboBox
  QFileInfo Info(defaultDataSet);
  QDir ProjDir(Info.dirPath());
  QStringList Elements = ProjDir.entryList("*.dat", QDir::Files, QDir::Name);
  QStringList::iterator it;
  for(it = Elements.begin(); it != Elements.end(); ++it) {
    ChooseData->insertItem((*it).left((*it).length()-4));
    if((*it) == Info.fileName())
      // default dataset should be the current
      ChooseData->setCurrentItem(ChooseData->count()-1);
  }
  slotReadVars(0);  // put variables into the ListView

  // ...........................................................
  // put all graphs into the ListBox
  Row = 0;
  for(Graph *pg = Diag->Graphs.first(); pg != 0; pg = Diag->Graphs.next()) {
    GraphList->insertItem(pg->Var);
    if(pg == currentGraph) {
      GraphList->setCurrentItem(Row);   // select current graph
      SelectGraph(currentGraph);
    }
    Row++;
  }

  if(ColorButt)
    if(!currentGraph)
      ColorButt->setPaletteBackgroundColor
    (QColor(DefaultColors[GraphList->count()%NumDefaultColors]));
}

DiagramDialog::~DiagramDialog()
{
  delete all;   // delete all widgets from heap
  delete ValInteger;
  delete ValDouble;
  delete Validator;
}

// --------------------------------------------------------------------------
void DiagramDialog::slotReadVars(int)
{
  QFileInfo Info(defaultDataSet);
  QString DocName = ChooseData->currentText()+".dat";

  QFile file(Info.dirPath() + QDir::separator() + DocName);
  if(!file.open(QIODevice::ReadOnly)) {
    return;
  }

  QString Line, tmp, Var;
  // reading the file as a whole improves speed very much, also using
  // a QByteArray rather than a QString
  QByteArray FileString = file.readAll();
  file.close();

  ChooseVars->clear();
  int i=0, j=0;
  i = FileString.find('<')+1;
  if(i > 0)
  do {
    j = FileString.find('>', i);
    for(int k=0;k<j-i;k++) Line[k]=FileString[k+i];
    Line.truncate(j-i);
    i = FileString.find('<', j)+1;

    Var = Line.section(' ', 1, 1).remove('>');
    if(Var.length()>0)
      if(Var.at(0) == '_')  continue;

    if(Line.left(3) == "dep") {
      tmp = Line.section(' ', 2);
      new Q3ListViewItem(ChooseVars, Var, "dep", tmp.remove('>'));
    }
    else if(Line.left(5) == "indep") {
      tmp = Line.section(' ', 2, 2);
      new Q3ListViewItem(ChooseVars, Var, "indep", tmp.remove('>'));
    }
  } while(i > 0);
}

// ------------------------------------------------------------------------
// Inserts the double-clicked variable into the Graph Input Line at the
// cursor position. If the Graph Input is empty, then the variable is
// also inserted as graph.
void DiagramDialog::slotTakeVar(Q3ListViewItem *Item)
{
  GraphInput->blockSignals(true);
  if(toTake) GraphInput->setText("");

  int     i  = GraphInput->cursorPosition();
  QString s  = GraphInput->text();
  QString s1 = Item->text(0);
  QFileInfo Info(defaultDataSet);
  if(ChooseData->currentText() != Info.baseName(true))
    s1 = ChooseData->currentText() + ":" + s1;
  GraphInput->setText(s.left(i) + s1 + s.right(s.length()-i));

  if(s.isEmpty()) {
    GraphList->insertItem(GraphInput->text());
    GraphList->setSelected(GraphList->count()-1,true);

    Graph *g = new Graph(GraphInput->text());   // create a new graph

    if(Diag->Name != "Tab") {
      if(Diag->Name != "Truth") {
        g->Color = ColorButt->paletteBackgroundColor();
        g->Thick = Property2->text().toInt();
        ColorButt->setPaletteBackgroundColor(
                   QColor(DefaultColors[GraphList->count()%NumDefaultColors]));
        if(g->Var.right(3) == ".Vb")   // harmonic balance output ?
          if(PropertyBox->count() >= GRAPHSTYLE_ARROW)
            PropertyBox->setCurrentItem(GRAPHSTYLE_ARROW);
        g->Style   = PropertyBox->currentItem();
        if(yAxisBox) {
          g->yAxisNo = yAxisBox->currentItem();
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
      g->numMode   = PropertyBox->currentItem();
    }

    Graphs.append(g);
    changed = true;
    toTake  = true;
  }

  GraphInput->blockSignals(false);

  if(Property2) {
    Label1->setEnabled(true);
    PropertyBox->setEnabled(true);
    Label2->setEnabled(true);
    Property2->setEnabled(true);
  }
}

// --------------------------------------------------------------------------
// Is called if a graph text is clicked in the BistBox.
void DiagramDialog::slotSelectGraph(Q3ListBoxItem *item)
{
  if(item == 0) {
    GraphList->clearSelection();
    return;
  }

  SelectGraph (Graphs.at (GraphList->index(item)));
}

// --------------------------------------------------------------------------
// Puts the text of the selected graph into the line edit.
void DiagramDialog::SelectGraph(Graph *g)
{
  GraphInput->blockSignals(true);
  GraphInput->setText(g->Var);
  GraphInput->blockSignals(false);

  if(Diag->Name != "Tab") {
    if(Diag->Name != "Truth") {
      Property2->setText(QString::number(g->Thick));
      ColorButt->setPaletteBackgroundColor(g->Color);
      PropertyBox->setCurrentItem(g->Style);
      if(yAxisBox) {
        yAxisBox->setCurrentItem(g->yAxisNo);
        yAxisBox->setEnabled(true);
        Label4->setEnabled(true);
      }

      Label3->setEnabled(true);
      ColorButt->setEnabled(true);
    }
  }
  else {
    Property2->setText(QString::number(g->Precision));
    PropertyBox->setCurrentItem(g->numMode);
  }
  toTake = false;

  if(Property2) {
    Label1->setEnabled(true);
    PropertyBox->setEnabled(true);
    Label2->setEnabled(true);
    Property2->setEnabled(true);
  }
}

// --------------------------------------------------------------------------
// Is called when the 'delelte graph' button is pressed.
void DiagramDialog::slotDeleteGraph()
{
  int i = GraphList->index(GraphList->selectedItem());
  if(i < 0) return;   // return, if no item selected

  GraphList->removeItem(i);
  Graphs.remove(i);

  GraphInput->setText("");  // erase input line and back to default values
  if(Diag->Name != "Tab") {
    if(Diag->Name != "Truth") {
      ColorButt->setPaletteBackgroundColor(
        QColor(DefaultColors[GraphList->count()%NumDefaultColors]));
      Property2->setText("0");
      if(yAxisBox) {
        yAxisBox->setCurrentItem(0);
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
    PropertyBox->setCurrentItem(0);

    Label1->setEnabled(false);
    PropertyBox->setEnabled(false);
    Label2->setEnabled(false);
    Property2->setEnabled(false);
  }
}

// --------------------------------------------------------------------------
void DiagramDialog::slotNewGraph()
{
  if(GraphInput->text().isEmpty()) return;

  GraphList->insertItem(GraphInput->text());

  Graph *g = new Graph(GraphInput->text());   // create a new graph
  if(Diag->Name != "Tab") {
    if(Diag->Name != "Truth") {
      g->Color = ColorButt->paletteBackgroundColor();
      g->Thick = Property2->text().toInt();
      g->Style = PropertyBox->currentItem();
      if(yAxisBox)  g->yAxisNo = yAxisBox->currentItem();
      else if(Diag->Name == "Rect3D")  g->yAxisNo = 1;
    }
  }
  else {
    g->Precision = Property2->text().toInt();
    g->numMode   = PropertyBox->currentItem();
  }
  Graphs.append(g);
  changed = true;
  toTake  = false;
}

// --------------------------------------------------------------------------
// Is called if "Ok" button is pressed.
void DiagramDialog::slotOK()
{
  slotApply();
  slotCancel();
}

// --------------------------------------------------------------------------
// Is called if "Apply" button is pressed.
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
    
    if(GridOn) if(Diag->xAxis.GridOn != GridOn->isChecked()) {
      Diag->xAxis.GridOn = GridOn->isChecked();
      Diag->yAxis.GridOn = GridOn->isChecked();
      changed = true;
    }
    if(GridColorButt)
      if(Diag->GridPen.color() != GridColorButt->paletteBackgroundColor()) {
      Diag->GridPen.setColor(GridColorButt->paletteBackgroundColor());
      changed = true;
    }
    if(GridStyleBox)
      if(Diag->GridPen.style()!=(Qt::PenStyle)(GridStyleBox->currentItem()+1)) {
      Diag->GridPen.setStyle((Qt::PenStyle)(GridStyleBox->currentItem()+1));
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
      Diag->yAxis.limit_min = startY->text().toDouble();
      changed = true;
    }
    if(QString::number(Diag->yAxis.step) != stepY->text()) {
      Diag->yAxis.step = stepY->text().toDouble();
      changed = true;
    }
    if(QString::number(Diag->yAxis.limit_max) != stopY->text()) {
      Diag->yAxis.limit_max = stopY->text().toDouble();
      changed = true;
    }
    if(Diag->zAxis.autoScale == manualZ->isChecked()) {
      Diag->zAxis.autoScale = !(manualZ->isChecked());
      changed = true;
    }
    if(QString::number(Diag->zAxis.limit_min) != startZ->text()) {
      Diag->zAxis.limit_min = startZ->text().toDouble();
      changed = true;
    }
    if(QString::number(Diag->zAxis.step) != stepZ->text()) {
      Diag->zAxis.step = stepZ->text().toDouble();
      changed = true;
    }
    if(QString::number(Diag->zAxis.limit_max) != stopZ->text()) {
      Diag->zAxis.limit_max = stopZ->text().toDouble();
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
  QColor c = QColorDialog::getColor(ColorButt->paletteBackgroundColor(),this);
  if(!c.isValid()) return;
  ColorButt->setPaletteBackgroundColor(c);

  int i = GraphList->index(GraphList->selectedItem());
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
			GridColorButt->paletteBackgroundColor(),this);
  if(!c.isValid()) return;
  GridColorButt->setPaletteBackgroundColor(c);
  changed = true;
}

// --------------------------------------------------------------------------
// Is set if the graph input line changes.
void DiagramDialog::slotResetToTake(const QString& s)
{
  int i = GraphList->index(GraphList->selectedItem());
  if(i < 0) return;   // return, if no item selected

  Graph *g = Graphs.at(i);
  g->Var = s;
  GraphList->changeItem(s, i);   // must done after the graph settings !!!
  changed = true;
  toTake  = false;
}

// --------------------------------------------------------------------------
// Is called if the user changes the graph thickness or the precision.
void DiagramDialog::slotSetProp2(const QString& s)
{
  int i = GraphList->index(GraphList->selectedItem());
  if(i < 0) return;   // return, if no item selected

  Graph *g = Graphs.at(i);
  if(Diag->Name == "Tab") g->Precision = s.toInt();
  else  g->Thick = s.toInt();
  changed = true;
  toTake  = false;
}

// --------------------------------------------------------------------------
// Is called if the user changes the number mode.
void DiagramDialog::slotSetNumMode(int Mode)
{
  int i = GraphList->index(GraphList->selectedItem());
  if(i < 0) return;   // return, if no item selected

  Graph *g = Graphs.at(i);
  g->numMode = Mode;
  changed = true;
  toTake  = false;
}

// --------------------------------------------------------------------------
// Is called when the "show grid" checkbox is changed.
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

// --------------------------------------------------------------------------
// Is called if the user changes the graph style (combobox).
void DiagramDialog::slotSetGraphStyle(int style)
{
  int i = GraphList->index(GraphList->selectedItem());
  if(i < 0) return;   // return, if no item selected

  Graph *g = Graphs.at(i);
  g->Style = style;
  changed = true;
  toTake  = false;
}

// --------------------------------------------------------------------------
// Makes a copy of all graphs in the diagram.
void DiagramDialog::copyDiagramGraphs()
{
  for(Graph *pg = Diag->Graphs.first(); pg != 0; pg = Diag->Graphs.next())
    Graphs.append(pg->sameNewOne());
}

// --------------------------------------------------------------------------
// Is called if the combobox changes that defines which y axis uses the graph.
void DiagramDialog::slotSetYAxis(int axis)
{
  int i = GraphList->index(GraphList->selectedItem());
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

// --------------------------------------------------------------------------
// Is called if the current tab of the QTabWidget changes.
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

// --------------------------------------------------------------------------
// Is called when the slider for rotation angle is changed.
void DiagramDialog::slotNewRotX(int Value)
{
  rotationX->setText(QString::number(Value));
  DiagCross->rotX = float(Value) * M_PI/180.0;
  DiagCross->update();
}

// --------------------------------------------------------------------------
// Is called when the slider for rotation angle is changed.
void DiagramDialog::slotNewRotY(int Value)
{
  rotationY->setText(QString::number(Value));
  DiagCross->rotY = float(Value) * M_PI/180.0;
  DiagCross->update();
}

// --------------------------------------------------------------------------
// Is called when the slider for rotation angle is changed.
void DiagramDialog::slotNewRotZ(int Value)
{
  rotationZ->setText(QString::number(Value));
  DiagCross->rotZ = float(Value) * M_PI/180.0;
  DiagCross->update();
}

// --------------------------------------------------------------------------
// Is called when the number (text) for rotation angle is changed.
void DiagramDialog::slotEditRotX(const QString& Text)
{
  SliderRotX->setValue(Text.toInt());
  DiagCross->rotX = Text.toFloat() * M_PI/180.0;
  DiagCross->update();
}

// --------------------------------------------------------------------------
// Is called when the number (text) for rotation angle is changed.
void DiagramDialog::slotEditRotY(const QString& Text)
{
  SliderRotY->setValue(Text.toInt());
  DiagCross->rotY = Text.toFloat() * M_PI/180.0;
  DiagCross->update();
}

// --------------------------------------------------------------------------
// Is called when the number (text) for rotation angle is changed.
void DiagramDialog::slotEditRotZ(const QString& Text)
{
  SliderRotZ->setValue(Text.toInt());
  DiagCross->rotZ = Text.toFloat() * M_PI/180.0;
  DiagCross->update();
}
