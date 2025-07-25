#ifndef SIMULATIONSETUP_H
#define SIMULATIONSETUP_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>

class SimulationSetup : public QWidget {
  Q_OBJECT
public:
  SimulationSetup(QWidget *parent = nullptr);
  ~SimulationSetup();

private:
  QDoubleSpinBox *fstartSpinBox, *fstopSpinBox;
  QComboBox *fstartScaleComboBox, *fstopScaleComboBox;
  QSpinBox *npointsSpinBox;
};

#endif // SIMULATIONSETUP_H
