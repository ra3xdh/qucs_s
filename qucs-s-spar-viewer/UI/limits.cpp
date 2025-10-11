/*
 *  Copyright (C) 2025 Andrés Martínez Mera - andresmmera@protonmail.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "qucs-s-spar-viewer.h"

void Qucs_S_SPAR_Viewer::addLimit(double f_limit1, QString f_limit1_unit,
                                  double f_limit2, QString f_limit2_unit,
                                  double y_limit1, double y_limit2,
                                  bool coupled) {
  // If there are no traces in the display, show a message and exit
  if (traceMap.size() == 0) {
    QMessageBox::information(this, tr("Warning"),
                             tr("The display contains no traces."));
    return;
  }

  if (f_limit1 == -1) {
    // There's no specific data passed. Then get it from the widgets
    double f1 = Magnitude_PhaseChart->getXmin();
    double f2 = Magnitude_PhaseChart->getXmax();
    f_limit1  = f1 + 0.25 * (f2 - f1);
    f_limit2  = f1 + 0.75 * (f2 - f1);

    double y1 = Magnitude_PhaseChart->getYmin();
    double y2 = Magnitude_PhaseChart->getYmax();

    y_limit1 = y1 + (y2 - y1) / 2;
    y_limit2 = y_limit1;
  }

  int n_limits = getNumberOfLimits();
  n_limits++;
  int limit_index = 3 * n_limits - 2;

  QString tooltip_message;

  QString new_limit_name  = QStringLiteral("Limit %1").arg(n_limits);
  QLabel* new_limit_label = new QLabel(new_limit_name);
  new_limit_label->setObjectName(new_limit_name);
  limitsMap[new_limit_name].LimitLabel = new_limit_label;
  this->LimitsGrid->addWidget(new_limit_label, limit_index, 0);

  QString SpinBox_fstart_name =
      QStringLiteral("Lmt_Freq_Start_SpinBox_%1").arg(new_limit_name);
  QDoubleSpinBox* new_limit_fstart_Spinbox = new QDoubleSpinBox();
  new_limit_fstart_Spinbox->setObjectName(SpinBox_fstart_name);
  new_limit_fstart_Spinbox->setMinimum(Magnitude_PhaseChart->getXmin());
  new_limit_fstart_Spinbox->setMaximum(Magnitude_PhaseChart->getXmax());
  new_limit_fstart_Spinbox->setSingleStep(Magnitude_PhaseChart->getXdiv() / 5);
  new_limit_fstart_Spinbox->setValue(f_limit1);
  limitsMap[new_limit_name].Start_Freq = new_limit_fstart_Spinbox;
  this->LimitsGrid->addWidget(new_limit_fstart_Spinbox, limit_index, 1);

  QString Combobox_start_name =
      QStringLiteral("Lmt_Start_ComboBox_%1").arg(new_limit_name);
  QComboBox* new_start_limit_Combo = new QComboBox();
  new_start_limit_Combo->setObjectName(Combobox_start_name);
  new_start_limit_Combo->addItems(frequency_units);
  limitsMap[new_limit_name].Start_Freq_Scale = new_start_limit_Combo;
  this->LimitsGrid->addWidget(new_start_limit_Combo, limit_index, 2);

  QString SpinBox_fstop_name =
      QStringLiteral("Lmt_Freq_Stop_SpinBox_%1").arg(new_limit_name);
  QDoubleSpinBox* new_limit_fstop_Spinbox = new QDoubleSpinBox();
  new_limit_fstop_Spinbox->setObjectName(SpinBox_fstop_name);
  new_limit_fstop_Spinbox->setMinimum(Magnitude_PhaseChart->getXmin());
  new_limit_fstop_Spinbox->setMaximum(Magnitude_PhaseChart->getXmax());
  new_limit_fstop_Spinbox->setSingleStep(Magnitude_PhaseChart->getXdiv() / 5);
  new_limit_fstop_Spinbox->setValue(f_limit2);
  limitsMap[new_limit_name].Stop_Freq = new_limit_fstop_Spinbox;
  this->LimitsGrid->addWidget(new_limit_fstop_Spinbox, limit_index, 3);

  QString Combobox_stop_name =
      QStringLiteral("Lmt_Stop_ComboBox_%1").arg(new_limit_name);
  QComboBox* new_stop_limit_Combo = new QComboBox();
  new_stop_limit_Combo->setObjectName(Combobox_stop_name);
  new_stop_limit_Combo->addItems(frequency_units);

  if (f_limit1_unit.isEmpty()) {
    QString Mag_Phase_Units = Magnitude_PhaseChart->getXunits();
    if (Mag_Phase_Units.isEmpty()) {
      new_start_limit_Combo->setCurrentIndex(1);
      new_stop_limit_Combo->setCurrentIndex(1);
    } else {
      int index = new_stop_limit_Combo->findText(Mag_Phase_Units);
      new_stop_limit_Combo->setCurrentIndex(index);
      new_start_limit_Combo->setCurrentIndex(index);
    }
  } else {
    // The units exist (e.g. loading session file)
    int index = new_start_limit_Combo->findText(
        f_limit1_unit,
        Qt::MatchFlag::MatchContains); // Find the index of the unit
    new_start_limit_Combo->setCurrentIndex(index);

    index = new_stop_limit_Combo->findText(
        f_limit2_unit,
        Qt::MatchFlag::MatchContains); // Find the index of the unit
    new_stop_limit_Combo->setCurrentIndex(index);
  }

  limitsMap[new_limit_name].Stop_Freq_Scale = new_stop_limit_Combo;
  this->LimitsGrid->addWidget(new_stop_limit_Combo, limit_index, 4);

  // Remove button
  QString DeleteButton_name =
      QStringLiteral("Lmt_Delete_Btn_%1").arg(new_limit_name);
  QToolButton* new_limit_removebutton = new QToolButton();
  new_limit_removebutton->setObjectName(DeleteButton_name);
  tooltip_message = QStringLiteral("Remove this limit");
  new_limit_removebutton->setToolTip(tooltip_message);
  QIcon icon(":/bitmaps/trash.png");
  new_limit_removebutton->setIcon(icon);
  new_limit_removebutton->setStyleSheet(R"(
            QToolButton {
                background-color: #FF0000;
                color: white;
                border-radius: 20px;
            }
        )");
  limitsMap[new_limit_name].Button_Delete_Limit = new_limit_removebutton;
  this->LimitsGrid->addWidget(new_limit_removebutton, limit_index, 5,
                              Qt::AlignCenter);

  QString SpinBox_val_start_name =
      QStringLiteral("Lmt_Val_Start_SpinBox_%1").arg(new_limit_name);
  QDoubleSpinBox* new_limit_val_start_Spinbox = new QDoubleSpinBox();
  new_limit_val_start_Spinbox->setObjectName(SpinBox_val_start_name);
  new_limit_val_start_Spinbox->setMinimum(-1000);
  new_limit_val_start_Spinbox->setMaximum(1000);
  new_limit_val_start_Spinbox->setValue(y_limit1);
  new_limit_val_start_Spinbox->setSingleStep(Magnitude_PhaseChart->getYdiv() /
                                             5);
  limitsMap[new_limit_name].Start_Value = new_limit_val_start_Spinbox;
  this->LimitsGrid->addWidget(new_limit_val_start_Spinbox, limit_index + 1, 1);

  // Coupled spinbox value
  QString CoupleButton_name =
      QStringLiteral("Lmt_Couple_Btn_%1").arg(new_limit_name);
  QPushButton* new_limit_CoupleButton = new QPushButton("<--->");
  new_limit_CoupleButton->setObjectName(CoupleButton_name);
  new_limit_CoupleButton->setChecked(coupled);
  tooltip_message = QStringLiteral("Couple start and stop values");
  new_limit_CoupleButton->setToolTip(tooltip_message);
  limitsMap[new_limit_name].Couple_Value = new_limit_CoupleButton;
  this->LimitsGrid->addWidget(new_limit_CoupleButton, limit_index + 1, 2);

  QString SpinBox_val_stop_name =
      QStringLiteral("Lmt_Val_Stop_SpinBox_%1").arg(new_limit_name);
  QDoubleSpinBox* new_limit_val_stop_Spinbox = new QDoubleSpinBox();
  new_limit_val_stop_Spinbox->setObjectName(SpinBox_val_stop_name);
  new_limit_val_stop_Spinbox->setMinimum(-1000);
  new_limit_val_stop_Spinbox->setMaximum(1000);
  new_limit_val_stop_Spinbox->setValue(y_limit2);
  new_limit_val_stop_Spinbox->setSingleStep(Magnitude_PhaseChart->getYdiv() /
                                            5);
  limitsMap[new_limit_name].Stop_Value = new_limit_val_stop_Spinbox;
  this->LimitsGrid->addWidget(new_limit_val_stop_Spinbox, limit_index + 1, 3);

  if (coupled) {
    new_limit_CoupleButton->setText("<--->");
  } else {
    new_limit_CoupleButton->setText("<-X->");
  }

  QString QComboBox_yaxis_name =
      QStringLiteral("Combo_yaxis_%1").arg(new_limit_name);
  QComboBox* QComboBox_y_axis = new QComboBox();
  QComboBox_y_axis->setObjectName(QComboBox_yaxis_name);
  QComboBox_y_axis->addItem("Left Y");
  QComboBox_y_axis->addItem("Right Y");
  limitsMap[new_limit_name].axis = QComboBox_y_axis;
  this->LimitsGrid->addWidget(QComboBox_y_axis, limit_index + 1, 4);

  QString Separator_name =
      QStringLiteral("Lmt_Separator_%1").arg(new_limit_name);
  QFrame* new_Separator = new QFrame();
  new_Separator->setObjectName(Separator_name);
  new_Separator->setFrameShape(QFrame::HLine);
  new_Separator->setFrameShadow(QFrame::Sunken);
  limitsMap[new_limit_name].Separator = new_Separator;
  this->LimitsGrid->addWidget(new_Separator, limit_index + 2, 0, 1, 6);

  // Connect widgets to handler
  connect(limitsMap[new_limit_name].Start_Freq, SIGNAL(valueChanged(double)),
          SLOT(updateLimits()));
  connect(limitsMap[new_limit_name].Start_Freq_Scale,
          SIGNAL(currentIndexChanged(int)), SLOT(updateLimits()));
  connect(limitsMap[new_limit_name].Stop_Freq, SIGNAL(valueChanged(double)),
          SLOT(updateLimits()));
  connect(limitsMap[new_limit_name].Stop_Freq_Scale,
          SIGNAL(currentIndexChanged(int)), SLOT(updateLimits()));
  connect(limitsMap[new_limit_name].Button_Delete_Limit, SIGNAL(clicked()),
          SLOT(removeLimit()));
  connect(limitsMap[new_limit_name].Start_Value, SIGNAL(valueChanged(double)),
          SLOT(updateLimits()));
  connect(limitsMap[new_limit_name].Couple_Value, SIGNAL(clicked(bool)),
          SLOT(coupleSpinBoxes()));
  connect(limitsMap[new_limit_name].Stop_Value, SIGNAL(valueChanged(double)),
          SLOT(updateLimits()));
  connect(limitsMap[new_limit_name].axis, SIGNAL(currentIndexChanged(int)),
          SLOT(updateLimits()));

  // Force to update the locked / unlocked status of the y-axis spinboxes
  limitsMap[new_limit_name].Couple_Value->click();

  // Add limit to the chart
  RectangularPlotWidget::Limit NewLimit;

  double f1       = limitsMap[new_limit_name].Start_Freq->value();
  QString scale   = limitsMap[new_limit_name].Start_Freq_Scale->currentText();
  double f1_scale = getFreqScale(scale);
  f1              = f1 / f1_scale;

  double f2       = limitsMap[new_limit_name].Stop_Freq->value();
  scale           = limitsMap[new_limit_name].Stop_Freq_Scale->currentText();
  double f2_scale = getFreqScale(scale);
  f2              = f2 / f2_scale;

  NewLimit.f1  = f1;
  NewLimit.f2  = f2;
  NewLimit.y1  = limitsMap[new_limit_name].Start_Value->value();
  NewLimit.y2  = limitsMap[new_limit_name].Stop_Value->value();
  NewLimit.pen = QPen(Qt::black, 2, Qt::SolidLine);

  Magnitude_PhaseChart->addLimit(new_limit_name, NewLimit);
  Magnitude_PhaseChart->update();
}

// Get the limit given the position of the entry
bool Qucs_S_SPAR_Viewer::getLimitByPosition(int position, QString& outLimitName,
                                            LimitProperties& outProperties) {
  // Check if position is valid
  if (position < 0 || position >= limitsMap.size()) {
    qWarning() << "Invalid position:" << position;
    return false;
  }

  // Get an iterator to the beginning of the map
  auto it = limitsMap.begin();

  // Advance the iterator by 'position' steps
  std::advance(it, position);

  // Get the marker name and properties
  outLimitName  = it.key();
  outProperties = it.value();

  return true;
}

void Qucs_S_SPAR_Viewer::coupleSpinBoxes() {

  QPushButton* button = qobject_cast<QPushButton*>(sender());
  // Get the button ID, from it we can get the index and then lock the upper
  // limit spinbox
  QString name_button = button->objectName();

  int nlimits = getNumberOfLimits();

  // Get limit
  QString limit_name;
  LimitProperties limit_props;
  for (int i = 0; i < nlimits; i++) {
    getLimitByPosition(i, limit_name, limit_props);
    if (limit_props.Couple_Value->objectName() == name_button) {
      break;
    }
  }

  if (limit_props.Couple_Value->text() == "<--->") {
    limit_props.Couple_Value->setText("<-X->");
    QString tooltip_message = QStringLiteral("Uncouple start and stop values");
    limit_props.Couple_Value->setToolTip(tooltip_message);

    // Couple limit spinboxes
    double start_value = limit_props.Start_Value->value();
    limit_props.Stop_Value->setValue(start_value);
    limit_props.Stop_Value->setDisabled(true);

  } else {
    limit_props.Couple_Value->setText("<--->");
    limit_props.Stop_Value->setEnabled(true);
  }
}

// This function is called when a limit widget is changed. It is needed in case
// some value-coupling button is activated
void Qucs_S_SPAR_Viewer::updateLimits() {
  // First check if some value-coupling button is activated. If not, simply call
  // updateTraces()
  int n_limits = getNumberOfLimits();
  for (int i = 0; i < n_limits; i++) {

    QString limit_name;
    LimitProperties limit_props;
    getLimitByPosition(i, limit_name, limit_props);

    if (limit_props.Couple_Value->text() == "<-X->") {
      // The control is locked. Set the stop value equal to the start value
      double val_start = limit_props.Start_Value->value();
      limit_props.Stop_Value->setValue(val_start);
    }
  }

  // Catch the widget limit that triggered this function and update its value in
  // the chart
  QObject* WidgetTriggered = sender();

  QString ObjectName = WidgetTriggered->objectName();

  int lastUnderscoreIndex = ObjectName.lastIndexOf('_');
  QString limit_name      = ObjectName.mid(lastUnderscoreIndex + 1);

  // Get the actual values of the widgets corresponding to that limit
  RectangularPlotWidget::Limit limit_props;
  double f1       = limitsMap[limit_name].Start_Freq->value();
  QString scale   = limitsMap[limit_name].Start_Freq_Scale->currentText();
  double f1_scale = getFreqScale(scale);
  f1              = f1 / f1_scale;

  double f2       = limitsMap[limit_name].Stop_Freq->value();
  scale           = limitsMap[limit_name].Stop_Freq_Scale->currentText();
  double f2_scale = getFreqScale(scale);
  f2              = f2 / f2_scale;

  limit_props.f1     = f1;
  limit_props.f2     = f2;
  limit_props.y1     = limitsMap[limit_name].Start_Value->value();
  limit_props.y2     = limitsMap[limit_name].Stop_Value->value();
  limit_props.y_axis = limitsMap[limit_name].axis->currentIndex();
  limit_props.pen    = QPen(Qt::black, 2, Qt::SolidLine);

  Magnitude_PhaseChart->updateLimit(limit_name, limit_props);
  Magnitude_PhaseChart->update();
}

// Returns the total number of limits
int Qucs_S_SPAR_Viewer::getNumberOfLimits() {
  return limitsMap.keys().size();
}

void Qucs_S_SPAR_Viewer::removeLimit(QString limit_to_remove) {

  // Get the widgets
  LimitProperties limit_props = limitsMap[limit_to_remove];

  // Remove the widgets
  delete limit_props.axis;
  delete limit_props.Button_Delete_Limit;
  delete limit_props.Couple_Value;
  delete limit_props.LimitLabel;
  delete limit_props.Separator;
  delete limit_props.Start_Freq;
  delete limit_props.Start_Freq_Scale;
  delete limit_props.Start_Value;
  delete limit_props.Stop_Freq;
  delete limit_props.Stop_Freq_Scale;
  delete limit_props.Stop_Value;

  // Remove limit entry from the map
  limitsMap.remove(limit_to_remove);

  // Remove limit lines from the plot
  Magnitude_PhaseChart->removeLimit(limit_to_remove);

  updateGridLayout(LimitsGrid);
  updateLimitNames();
}

void Qucs_S_SPAR_Viewer::removeAllLimits() {
  int n_limits = getNumberOfLimits();
  for (int i = 0; i < n_limits; i++) {
    QString limit_name;
    LimitProperties limit_props;
    getLimitByPosition(i, limit_name, limit_props);
    removeLimit(limit_name);
  }
}

// If the combobox associated to a marker changes, the limits of the marker must
// be updated too
void Qucs_S_SPAR_Viewer::changeMarkerLimits() {
  QString ID = qobject_cast<QComboBox*>(sender())->objectName();
  // qDebug() << "Clicked button:" << ID;
  changeMarkerLimits(ID);
}

// If the combobox associated to a marker changes, the limits of the marker must
// be updated too
void Qucs_S_SPAR_Viewer::changeMarkerLimits(QString ID) {
  // Find the index of the marker
  int index    = -1;
  int nmarkers = getNumberOfMarkers();

  // Inspects all the markers' combobox and find that've been triggered
  for (int i = 0; i < nmarkers; i++) {
    MarkerProperties mkr_props;
    QString mkr_name;

    getMarkerByPosition(i, mkr_name, mkr_props);

    if (mkr_props.scaleComboBox->objectName() == ID) {
      index = i;
      break;
    }
  }

  // The lower and upper limits are given by the axis settings
  double f_upper = Magnitude_PhaseChart->getXmax();
  double f_lower = Magnitude_PhaseChart->getXmin();
  double f_scale = 1e-6;

  f_upper /= f_scale;
  f_lower /= f_scale;

  // Get markers properties
  MarkerProperties mkr_props;
  QString mkr_name;

  getMarkerByPosition(index, mkr_name, mkr_props);

  // Now we have to normalize this with respect to the marker's combo
  QString new_scale    = mkr_props.scaleComboBox->currentText();
  double f_scale_combo = getFreqScale(new_scale);
  f_upper *= f_scale_combo;
  f_lower *= f_scale_combo;

  mkr_props.freqSpinBox->setMinimum(f_lower);
  mkr_props.freqSpinBox->setMaximum(f_upper);

  // Update minimum step
  double diff = f_upper - f_lower;
  if (diff < 1) {
    mkr_props.freqSpinBox->setSingleStep(0.01);
  } else {
    if (diff < 10) {
      mkr_props.freqSpinBox->setSingleStep(0.1);
    } else {
      if (diff < 100) {
        mkr_props.freqSpinBox->setSingleStep(1);
      } else {
        mkr_props.freqSpinBox->setSingleStep(10);
      }
    }
  }

  updateMarkerTable();
}

// This function is called when the user wants to remove a limit from the plot
void Qucs_S_SPAR_Viewer::removeLimit() {
  QString ID = qobject_cast<QToolButton*>(sender())->objectName();
  // qDebug() << "Clicked button:" << ID;

  // Find the index of the button to remove
  int n_limits = getNumberOfLimits();
  for (int i = 0; i < n_limits; i++) {

    QString limit_name;
    LimitProperties limit_props;
    getLimitByPosition(i, limit_name, limit_props);

    if (limit_props.Button_Delete_Limit->objectName() == ID) {
      removeLimit(limit_name);
      break;
    }
  }
}

// After removing a limit, the names of the other markers must be updated
void Qucs_S_SPAR_Viewer::updateLimitNames() {
  int n_limits = getNumberOfLimits();
  for (int i = 0; i < n_limits; i++) {
    QString limit_name;
    LimitProperties limit_props;
    getLimitByPosition(i, limit_name, limit_props);

    limit_props.LimitLabel->setText(QStringLiteral("Limit %1").arg(i + 1));
  }
}
