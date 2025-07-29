#ifndef NETLISTSCRATCHPAD_H
#define NETLISTSCRATCHPAD_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include "../../Misc/general.h"
#include "../../CustomWidgets/codeeditor.h"
#include "../../Schematic/Network.h"

class NetlistScratchPad : public QWidget {
  Q_OBJECT
public:
  NetlistScratchPad(QWidget *parent = nullptr);
  ~NetlistScratchPad();
  QString getText();
  void setText(QString);



private:
  CodeEditor *Netlist_Editor_Widget;
  QLabel *traceNameLabel;
  QLineEdit *traceNameLineEdit;

private slots:
  void update();

signals:
  void updateSimulation(SchematicContent);
};

#endif // NETLISTSCRATCHPAD_h
