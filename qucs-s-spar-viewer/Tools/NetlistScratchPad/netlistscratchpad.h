#ifndef SIMULATIONSETUP_H
#define SIMULATIONSETUP_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>
#include "../../Misc/general.h"

class NetlistScratchPad : public QWidget {
  Q_OBJECT
public:
  NetlistScratchPad(QWidget *parent = nullptr);
  ~NetlistScratchPad();



private:

private slots:
  void update();

signals:
  void updateSimulation();
};

#endif // SIMULATIONSETUP_H
