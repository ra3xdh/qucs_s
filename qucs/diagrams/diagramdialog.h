/***************************************************************************
                              diagramdialog.h
                             -----------------
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

#ifndef DIAGRAMDIALOG_H
#define DIAGRAMDIALOG_H
#include "diagram.h"
#include "node.h"

/*#ifndef pi
#define pi 3.1415926535897932384626433832795029
#endif*/

#include <QDialog>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include "qt3_compat/qt_compat.h"

class QVBoxLayout;
class Cross3D;
class QLabel;
class QLineEdit;
class QCheckBox;
class QComboBox;
class QDoubleValidator;
class QIntValidator;
class QRegExpValidator;
class QSlider;
class QTableWidgetItem;
class QListWidgetItem;
class QTableWidget;
class QListWidget;


class DiagramDialog : public QDialog  {
Q_OBJECT
public:
  DiagramDialog(Diagram *d, QWidget *parent=0,
		Graph *currentGraph=0);
  ~DiagramDialog();

  bool loadVarData(const QString&);
  void copyDiagramGraphs();

private slots:
  void slotReadVars(int);
  void slotReadVarsAndSetSimulator(int);
  void slotTakeVar(QTableWidgetItem *item);
//  void slotSelectGraph(int index);
  void slotSelectGraph(QListWidgetItem*);
  void slotNewGraph();
  void slotDeleteGraph();
  void slotOK();
  void slotApply();
  void slotCancel();
  void slotSetColor();
  void slotSetGridColor();
  void slotResetToTake(const QString&);
  void slotSetProp2(const QString&);
  void slotSetNumMode(int);
  void slotSetGridBox(int);
  void slotSetGraphStyle(int);
  void slotSetYAxis(int);
  void slotManualX(int);
  void slotManualY(int);
  void slotManualZ(int);
  void slotChangeTab(QWidget*);

  void slotNewRotX(int);
  void slotNewRotY(int);
  void slotNewRotZ(int);
  void slotEditRotX(const QString&);
  void slotEditRotY(const QString&);
  void slotEditRotZ(const QString&);
  void slotRecalcDbLimitsY();
  void slotRecalcDbLimitsZ();

  void slotPlotVs(int);

protected slots:
    void reject();

private:
  void SelectGraph(Graph*);
  void updateXVar();

  Diagram *Diag;
  QString defaultDataSet;

  QRegularExpression Expr;
  QDoubleValidator *ValDouble;
  QIntValidator    *ValInteger;
  QRegularExpressionValidator *Validator;

  QLabel *lblSim;
  QLabel *lblPlotVs;
  QComboBox *ChooseData;
  QComboBox *ChooseSimulator;
  QComboBox *ChooseXVar;
  QComboBox *LogUnitsY;
  QComboBox *LogUnitsZ;
  QTableWidget *ChooseVars;
  QListWidget  *GraphList;

  QVBoxLayout *all;   // the mother of all widgets
  QLineEdit   *GraphInput, *Property2, *xLabel, *ylLabel, *yrLabel;
  QCheckBox   *GridOn, *GridLogX, *GridLogY, *GridLogZ;
  QCheckBox   *manualX, *manualY, *manualZ, *hideInvisible;
  QLineEdit   *startX, *stepX, *stopX;
  QLineEdit   *startY, *stepY, *stopY;
  QLineEdit   *startZ, *stepZ, *stopZ;
  QLineEdit   *rotationX, *rotationY, *rotationZ;
  QLabel      *GridLabel1, *GridLabel2, *Label1, *Label2, *Label3, *Label4,
              *NotationLabel;
  QComboBox   *PropertyBox, *GridStyleBox, *yAxisBox, *NotationBox;
  QPushButton *ColorButt, *GridColorButt;
  QSlider     *SliderRotX, *SliderRotY, *SliderRotZ;
  Cross3D     *DiagCross;
  bool changed, transfer, toTake;
  Q3PtrList<Graph>  Graphs;
};

#endif
