#include "netlistscratchpad.h"

NetlistScratchPad::NetlistScratchPad(QWidget *parent): QWidget(parent) {
  QGridLayout *WidgetLayout = new QGridLayout();

  Netlist_Editor_Widget = new CodeEditor();
  Netlist_Editor_Widget->insertPlainText(QString("P1 1 50.0Ohm\nC1 0 1 3.76pF\nL1 2 1 14.5nH\nC2 0 2 3.76pF\nP2 2 50.0Ohm\n"));
  WidgetLayout->addWidget(Netlist_Editor_Widget,0,0,1,2);

  traceNameLabel = new QLabel("Trace name");
  traceNameLineEdit = new QLineEdit("Netlist1");
  WidgetLayout->addWidget(traceNameLabel, 1, 0);
  WidgetLayout->addWidget(traceNameLineEdit, 1, 1);


  this->setLayout(WidgetLayout);

  // Connect the text editor so that any changes in there rereuns the simulation
  connect(Netlist_Editor_Widget, SIGNAL(textChanged()), this, SLOT(update()));

}

NetlistScratchPad::~NetlistScratchPad() {

}


void NetlistScratchPad::update() {
  QString netlist = Netlist_Editor_Widget->getText();
  SchematicContent SC;

  SC.setNetlist(netlist);
  SC.Name = traceNameLineEdit->text();

  emit updateSimulation(SC);
}


QString NetlistScratchPad::getText() {
  return Netlist_Editor_Widget->getText();
}

void NetlistScratchPad::setText(QString netlist) {
  Netlist_Editor_Widget->clear();
  Netlist_Editor_Widget->insertPlainText(netlist);
}

