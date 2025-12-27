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

/*#ifndef pi
#define pi 3.1415926535897932384626433832795029
#endif*/

#include <QDialog>
#include <QSpinBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <vector>

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
class QCompleter; // Variable completion
class QSpinBox; // Thickness and decimal precission widgets


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
  void slotSelectGraph(QTableWidgetItem*);
  void slotNewGraph();
  void slotDeleteGraph();
  void slotOK();
  void slotApply();
  void slotCancel();
  void slotSetColor();
  void slotSetGridColor();
  void slotResetToTake(const QString&);
  void slotSetNumMode(int);
  void slotSetGridBox(int);
  void slotSetGraphStyle(int);
  void slotSetYAxis(int);
  void slotManualX(int);
  void slotManualY(int);
  void slotManualZ(int);
  void slotChangeTab(int);

  void slotNewRotX(int);
  void slotNewRotY(int);
  void slotNewRotZ(int);
  void slotEditRotX(const QString&);
  void slotEditRotY(const QString&);
  void slotEditRotZ(const QString&);
  void slotRecalcDbLimitsY();
  void slotRecalcDbLimitsZ();

  void slotPlotVs(int);

  ///
  /// \brief Handles key press events
  ///
  void keyPressEvent(QKeyEvent *event) override;

  /*!
   * \brief Sets the thickness of the currently selected graph trace.
   *
   * This slot is triggered when the user changes the value in the thickness
   * spin box. It updates the Thick property of the selected graph and refreshes
   * the graph list display to show the new thickness value.
   *
   * \param value The new thickness value (0-99 pixels)
   *
   * \note Only applies to plot-type diagrams (Rect, Polar, Smith, etc.).
   *       Does not apply to tabular or truth table diagrams.
   *
   * \see slotSetPrecision(), updateGraphListItem()
   */
  void slotSetThickness(int);

  /*!
   * \brief Sets the decimal precision of the currently selected tabular graph.
   *
   * This slot is triggered when the user changes the value in the precision
   * spin box. It updates the Precision property of the selected graph and
   * refreshes the graph list display.
   *
   * \param value The number of decimal places to display (0-99)
   *
   * \note Only applies to tabular diagrams
   *       Does not apply to plot-type diagrams.
   *
   * \see slotSetThickness(), slotSetNumMode()
   */
  void slotSetPrecision(int);

protected slots:
    void reject();

private:
  void SelectGraph(Graph*);
  void updateXVar();
  ///
  /// \brief Updates the content of the graph list to see the trace name along with the trace properties
  /// \param row: Number of the row to update
  ///
  void updateGraphListItem(int row);

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
  QTableWidget *GraphList;

  QVBoxLayout *all;   // the mother of all widgets
  QLineEdit   *GraphInput, *xLabel, *ylLabel, *yrLabel;
  QSpinBox    *thicknessSpin, *precisionSpin;
  QCheckBox   *GridOn, *GridLogX, *GridLogY, *GridLogZ;
  QCheckBox   *manualX, *manualY, *manualZ, *hideInvisible;
  QLineEdit   *startX, *stepX, *stopX;
  QLineEdit   *startY, *stepY, *stopY;
  QLineEdit   *startZ, *stepZ, *stopZ;
  QLineEdit   *rotationX, *rotationY, *rotationZ;
  QLabel      *GridLabel1, *GridLabel2, *Label1, *Label2, *Label3, *Label4,
              *NotationLabel;
  QLabel      *thicknessLabel, *precisionLabel;
  QComboBox   *PropertyBox, *GridStyleBox, *yAxisBox, *NotationBox;
  QPushButton *ColorButt, *GridColorButt;
  QSlider     *SliderRotX, *SliderRotY, *SliderRotZ;
  Cross3D     *DiagCross;
  bool changed, transfer, toTake;
  std::vector<std::unique_ptr<Graph>>  Graphs;

  ////////////////////////////////////////////////
  // Variable completion
  /**
   * @brief Autocompleter for the GraphInput line edit widget.
   *
   * @see updateCompleter()
   */
  QCompleter *graphCompleter;

  ///
  /// \brief Updates the autocomplete model for the GraphInput line edit.
  /// @see graphCompleter;
  ///
  void updateCompleter();
  ////////////////////////////////////////////////

};

#endif
