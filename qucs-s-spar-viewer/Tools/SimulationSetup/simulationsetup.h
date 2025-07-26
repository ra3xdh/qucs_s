#ifndef SIMULATIONSETUP_H
#define SIMULATIONSETUP_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>
#include "../..//Misc/general.h"

class SimulationSetup : public QWidget {
  Q_OBJECT
public:
  SimulationSetup(QWidget *parent = nullptr);
  ~SimulationSetup();

  double getFstart();
  double getFstop();
  int getNpoints();

private:
  QDoubleSpinBox *fstartSpinBox, *fstopSpinBox;
  QComboBox *fstartScaleComboBox, *fstopScaleComboBox;
  QSpinBox *npointsSpinBox;

private slots:
  void update();

signals:
  void updateSimulation();
};

#endif // SIMULATIONSETUP_H
