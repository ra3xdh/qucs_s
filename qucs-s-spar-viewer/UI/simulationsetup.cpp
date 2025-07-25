#include "simulationsetup.h"

SimulationSetup::SimulationSetup(QWidget *parent): QWidget(parent) {
  QGridLayout *WidgetLayout = new QGridLayout();

  // Start frequency
  QLabel * fstartLabel = new QLabel("Start freq");
  fstartSpinBox = new QDoubleSpinBox();
  fstartSpinBox->setMinimum(0);
  fstartSpinBox->setValue(0);

  fstartScaleComboBox = new QComboBox();
  fstartScaleComboBox->addItem("GHz");
  fstartScaleComboBox->addItem("MHz");
  fstartScaleComboBox->addItem("kHz");
  fstartScaleComboBox->addItem("Hz");
  fstartScaleComboBox->setCurrentIndex(1);

  WidgetLayout->addWidget(fstartLabel, 0, 0);
  WidgetLayout->addWidget(fstartSpinBox, 0, 1);
  WidgetLayout->addWidget(fstartScaleComboBox, 0, 2);


  // Stop frequency
  QLabel * fstopLabel = new QLabel("Stop freq");
  fstopSpinBox = new QDoubleSpinBox();
  fstopSpinBox->setMinimum(0);
  fstopSpinBox->setValue(2000);

  fstopScaleComboBox = new QComboBox();
  fstopScaleComboBox->addItem("GHz");
  fstopScaleComboBox->addItem("MHz");
  fstopScaleComboBox->addItem("kHz");
  fstopScaleComboBox->addItem("Hz");
  fstopScaleComboBox->setCurrentIndex(1);

  WidgetLayout->addWidget(fstopLabel, 1, 0);
  WidgetLayout->addWidget(fstopSpinBox, 1, 1);
  WidgetLayout->addWidget(fstopScaleComboBox, 1, 2);

  // Number of points
  QLabel * npointsLabel = new QLabel("Number of points");
  npointsSpinBox = new QSpinBox();
  npointsSpinBox->setMinimum(10);

  WidgetLayout->addWidget(npointsLabel, 2, 0);
  WidgetLayout->addWidget(npointsSpinBox, 2, 1);
  this->setLayout(WidgetLayout);

}

SimulationSetup::~SimulationSetup() {

}
