/***************************************************************************
                               tuner.cpp
                              -----------------
    begin                : Sat April 11 2015
    copyright            : (C) 2015 by Kevin Voet
    email                : kevin.voet@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "tuner.h"

#include "main.h"
#include "schematic.h"
#include "extsimkernels/spicecompat.h"

#include <QCloseEvent>


bool checkProperty(Component *component, Property *pp)
{
    if (component->Model.at(0) == '.') return false;//Simulation parameters
    // Properties defined in the integer+ domain
    if (pp->Name=="Num") return false;//Port number
    if (pp->Name=="Branches") return false;//Branches parameter in the EDD
    if (pp->Name=="Ports") return false;//Number of ports in a SNP file component
    if (!pp->Value.at(0).isNumber()) return false;//String
    //Check if the value contains symbols *, /, -, +
    for (int i = 0; i < pp->Value.length(); i++)
    {
        if (!pp->Value.at(i).isLetterOrNumber() && (pp->Value.at(i) != '.') && (pp->Value.at(i) != ' ')) return false;
        if (pp->Value.at(i).toLower() == 'e') break;//Scientific notation
    }
    return true;
}

tunerElement::tunerElement(QWidget *parent, Component *component, Property *pp, int selectedPropertyId)
  : QWidget(parent), c(component)
{
    QStringList ScaleFactorList;
    ScaleFactorList << "f" << "p" << "n" << "u" << "m" << "" << "k" << "M" << "G";
    int magnitudeIndex = 5;//No scaling
    float minValueValidator = PTRDIFF_MIN;
    bool has_unit = true;

    // ************************************** HANDLE PROPERTY VALUE **************************************
    //First of all we need to check whether the property is tunable (number + units) or not (random string)
    //There may be an arbitrary number of spaces between magnitude and unit, so in first place we need to normalize that
    //In this sense, all blank spaces are removed
    QString val = pp->Value;
    unit.clear();

    if (val.contains("e", Qt::CaseInsensitive))//Scientific notation
    {
        val = misc::num2str(val.toDouble());
        has_unit = false; //num2str doesn't return the unit
    }

    val.remove(" ");

    //Now, it's time to look for the suffix... if any!
    int units_index = -1;
    for (int i = 0; i < val.length(); i++)
    {
        if (val.at(i).isLetter())
        {
            units_index = i;//Select index
            numValue = val.mid(0, i).toFloat();//Get the magnitude
            break;
        }
    }

    if (units_index != -1)
    {
       //Find the units from the string
       unit = val.mid(units_index);// unit should be sth like mA, mm, uV, nA, etc... here
       SeparateMagnitudeFromSuffix(unit, magnitudeIndex);
    }
    else
    {//The property value contains only numbers
      numValue = val.toFloat();
    }
       if ((unit.length() <= 1) && (!pp->Description.isEmpty()))
       {//It comes with no units... so let's try to find a suitable unit
        QMap <QString, QStringList> Keywords;//Map containing the keywords that may appear in the property description
        Keywords["F"] << "capacitance";
        Keywords["H"] << "inductance";
        Keywords["Ohm"] << "resistance";
        Keywords["V"] << "voltage";
        Keywords["A"] << "current";
        Keywords["m"] << "length" << "width" << "thickness";
        Keywords["s"] << "time";

        //If one of the keywords appears in the description text, then the key of the map
        //is used as unit...
        bool found = false;
        for(auto e : Keywords.keys())
        {
          for (int i = 0; i < Keywords[e].length(); i++)
          {
              if (pp->Description.indexOf(Keywords[e].at(i), Qt::CaseInsensitive) != -1)
              {
                  unit = e;
                  found = true;
                  if ((i!=3) && (i !=4 ))//Not a voltage nor a current
                      minValueValidator = 0;//Allow positive numbers only in the lineedits
                  break;
              }
          }
          if (found) break;
        }
       }
       else
       {//The value comes with unit
          if (magnitudeIndex != 5) unit = unit.mid(1);
       }

        //Finally, add the unit to the QStringList for adding it to the scale comboboxes later
        if (has_unit) for (int i = 0; i < ScaleFactorList.length(); i++) ScaleFactorList[i] += unit;
    originalValue = QString::number(numValue)+ScaleFactorList[magnitudeIndex];


    //******************************  HANDLE PROPERTY VALUE (END) ***************************

    //UI setup
    setAttribute(Qt::WA_DeleteOnClose);//This attribute forces the widget to be destroyed after closing
    prop = component->Props.at(selectedPropertyId);
    QGridLayout *gbox = new QGridLayout();
    setLayout(gbox);
    QLabel *tunerName = new QLabel(component->Name + ":" + prop->Name);
    tunerName->setStyleSheet("QLabel {font: bold}");
    gbox->addWidget(tunerName,0,0,1,2);


    QLabel * maxLabel = new QLabel(tr("Max.:"));
    maxLabel->setLineWidth(5);
    gbox->addWidget(maxLabel, 1, 0);
    maximum = new QLineEdit();
    maximum->setValidator( new QDoubleValidator(minValueValidator, PTRDIFF_MAX, 2, this) );//Prevent the user from entering text
    MaxUnitsCombobox = new QComboBox(this);
    MaxUnitsCombobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    MaxUnitsCombobox->addItems(ScaleFactorList);
    MaxUnitsCombobox->setCurrentIndex(magnitudeIndex);
    gbox->addWidget(maximum, 1, 1);
    gbox->addWidget(MaxUnitsCombobox,1,2);

    slider = new QSlider(Qt::Vertical);
    gbox->addWidget(slider, 2, 1, 2, 1, Qt::AlignCenter);
    slider->setMinimumHeight(200);


    QLabel * minLabel = new QLabel(tr("Min.:"));
    minLabel->setLineWidth(5);
    gbox->addWidget(minLabel, 4, 0);
    minimum = new QLineEdit();
    minimum->setValidator( new QDoubleValidator(minValueValidator, 100, 2, this) );//Prevent the user from entering text
    MinUnitsCombobox = new QComboBox(this);
    MinUnitsCombobox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    MinUnitsCombobox->addItems(ScaleFactorList);
    MinUnitsCombobox->setCurrentIndex(magnitudeIndex);

    gbox->addWidget(minimum, 4, 1);
    gbox->addWidget(MinUnitsCombobox,4,2);

    QLabel *valLabel = new QLabel(tr("Val.:"));
    valLabel->setLineWidth(5);
    gbox->addWidget(valLabel, 5, 0);
    value = new QLineEdit();
    value->setValidator( new QDoubleValidator(minValueValidator, PTRDIFF_MAX, 2, this) );//Prevent the user from entering text
    ValueUnitsCombobox = new QComboBox(this);
    gbox->addWidget(value, 5, 1);
    gbox->addWidget(ValueUnitsCombobox,5,2);
    ValueUnitsCombobox->addItems(ScaleFactorList);
    ValueUnitsCombobox->setCurrentIndex(magnitudeIndex);

    QLabel * stepLabel = new QLabel(tr("Step"));
    stepLabel->setLineWidth(5);
    gbox->addWidget(stepLabel, 6, 0);
    step = new QLineEdit();
    step->setValidator( new QDoubleValidator(0, PTRDIFF_MAX, 2, this) );//Prevent the user from entering text
    StepUnitsCombobox = new QComboBox(this);
    gbox->addWidget(step, 6, 1);
    gbox->addWidget(StepUnitsCombobox,6,2);
    StepUnitsCombobox->addItems(ScaleFactorList);
    StepUnitsCombobox->setCurrentIndex(magnitudeIndex);


    QPushButton *remove = new QPushButton("", this);
    gbox->addWidget(remove, 0, 2);
    QPixmap pixmap(":/bitmaps/RemoveElementTuning.png");
    QIcon ButtonIcon(pixmap);
    remove->setIcon(ButtonIcon);
    remove->setIconSize(pixmap.rect().size()/2);

    Up_Down_Buttons_Widget = new QWidget();
    QGridLayout *buttonsLayout = new QGridLayout();
    up = new QToolButton(this);
    up->setArrowType(Qt::UpArrow);
    buttonsLayout->addWidget(up, 0, 0);

    down = new QToolButton(this);
    down->setArrowType(Qt::DownArrow);
    buttonsLayout->addWidget(down, 1, 0);
    Up_Down_Buttons_Widget->setLayout(buttonsLayout);
    Up_Down_Buttons_Widget->setStyleSheet("QWidget {border: 1px solid black;}");
    gbox->addWidget(Up_Down_Buttons_Widget, 2, 2, 2, 1);

    maxValue = numValue*1.15;// max = initial_value + 15%
    minValue = numValue*0.85;// min = initial_value - 15%
    stepValue = (maxValue-minValue)/20;//20 steps between minimum and maximum
    maximum->setText(QString::number(maxValue));
    minimum->setText(QString::number(minValue));
    value->setText(QString::number(numValue));
    step->setText(QString::number(stepValue));

    slider->setRange(0,100);
    float v = (numValue - minValue) / (maxValue - minValue);
    slider->setValue(v*100);

    slider->setTickPosition(QSlider::TicksBothSides);
    slider->setTickInterval(0);

    connect(maximum, SIGNAL(editingFinished()), this, SLOT(slotMaxValueChanged()));
    connect(MaxUnitsCombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotMaxValueChanged()));

    connect(minimum, SIGNAL(editingFinished()), this, SLOT(slotMinValueChanged()));
    connect(MinUnitsCombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotMinValueChanged()));

    connect(step, SIGNAL(editingFinished()), this, SLOT(slotStepChanged()));
    connect(StepUnitsCombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotStepChanged()));

    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(slotSliderChanged()));
    connect(value, SIGNAL(editingFinished()), this, SLOT(slotValueChanged()));
    connect(ValueUnitsCombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotValueChanged()));

    connect(remove, SIGNAL(released()), this, SLOT(slotDelete()));
    connect(up, SIGNAL(clicked()), this, SLOT(slotUpClicked()));
    connect(down, SIGNAL(clicked()), this, SLOT(slotDownClicked()));

    //So far, minimum, maximum, stepValue and value has the same scale. Let's set the right value
    bool aux;
    maxValue = getMaxValue(aux);
    minValue = getMinValue(aux);
    numValue = getValue(aux);
    stepValue = getStep(aux);
}

Property* tunerElement::getElementProperty()
{
    return prop;
}

void tunerElement::resetValue()
{
    prop->Value = originalValue;
    //Reset value value
    int index = 5;//By default, no scaling
    QString val = SeparateMagnitudeFromSuffix(originalValue, index);
    value->setText(val);
    ValueUnitsCombobox->setCurrentIndex(index);
    updateSlider();
    slotValueChanged();
}

/*
 * This function updates the value of the element for further simulation
 */
void tunerElement::updateProperty()
{
    prop->Value = value->text().append(tr(" ") + ValueUnitsCombobox->currentText());
        qDebug() << "Updated property: " << prop->Value;
}


/*
Given a QString containing number + suffix (i.e. 2.5p, 3.9n), this function separates '2.5' from 'p' and gives the corresponding index to that suffix
*/
QString SeparateMagnitudeFromSuffix(QString num, int & index)
{
  int sp=-1;
  for (int i = 0; i < num.length(); i++)
  {
      if (num.at(i).isLetter())
      {
          sp = i;
          break;
      }
  }

  if (sp == -1) return num;//It doesn't come with a suffix

  switch(num.at(sp).toLatin1()) {
      case 'f': index = 0;
                break;
      case 'p': index = 1;
                break;
      case 'n': index = 2;
                break;
      case 'u': index = 3;
                break;
      case 'm': index = 4;
                break;
      case 'k': index = 6;
                break;
      case 'M': index = 7;
                break;
      case 'G': index = 8;
                break;
      }
  return num.mid(0,sp);
}



/*
 * The control reaches this function when either the maximum value QLineedit or its corresponding combobox are edited
 */
void tunerElement::slotMaxValueChanged()
{
    bool ok;
    float v = getMaxValue(ok);
    maximum->blockSignals(true);
    MaxUnitsCombobox->blockSignals(true);

    if (!ok || (v <= minValue))
    {
        QMessageBox::warning(this, "ERROR", "Maximum value not correct", QMessageBox::Ok);
        //Restore values
        QString val = misc::num2str(maxValue);
        int index = 5;//By default, no scaling
        val = SeparateMagnitudeFromSuffix(val, index);
        maximum->setText(val);
        MaxUnitsCombobox->setCurrentIndex(index);
        maximum->blockSignals(false);
        MaxUnitsCombobox->blockSignals(false);
        return;
    }

    maxValue = v;
    if (v < numValue) {
        maxValue = numValue;
        //Update text... since maxValue was set to numValue and differs from what the user entered
        QString val = misc::num2str(maxValue);
        int index = 5;//By default, no scaling
        val = SeparateMagnitudeFromSuffix(val, index);
        maximum->setText(val);
        MaxUnitsCombobox->setCurrentIndex(index);
    }
    qDebug() << "tunerElement::slotMaxValueChanged() " << v;

    updateSlider();
    maximum->blockSignals(false);
    MaxUnitsCombobox->blockSignals(false);
}

/*
 * The control reaches this function when either the minimum value QLineedit or its corresponding combobox are edited
 */
void tunerElement::slotMinValueChanged()
{
    bool ok;
    float v = getMinValue(ok);
    minimum->blockSignals(true);
    MinUnitsCombobox->blockSignals(true);

    if (!ok || (v >= maxValue))
    {
        QMessageBox::warning(this, "ERROR", "Minimum value not correct", QMessageBox::Ok);
        //Restore values
        QString val = misc::num2str(minValue);
        int index = 5;//By default, no scaling
        val = SeparateMagnitudeFromSuffix(val, index);
        minimum->setText(val);
        MinUnitsCombobox->setCurrentIndex(index);
        minimum->blockSignals(false);
        MinUnitsCombobox->blockSignals(false);
        return;
    }

    minValue = v;
    if (v > numValue) {
        minValue = numValue;
        //Update text... since minValue was set to numValue and differs from what the user entered
        QString val = misc::num2str(minValue);
        int index = 5;//By default, no scaling
        val = SeparateMagnitudeFromSuffix(val, index);
        minimum->setText(val);
        MinUnitsCombobox->setCurrentIndex(index);
    }

    qDebug() << "slotMinValueChanged() " << v;

    updateSlider();
    minimum->blockSignals(false);
    MinUnitsCombobox->blockSignals(false);
}

/*
 * The control reaches this function when either the step QLineedit or its corresponding combobox are edited
 */
void tunerElement::slotStepChanged()
{
    bool ok;
    float v =  getStep(ok);

    qDebug() << "tunerElement::slotStepChanged()" << v;

    if (!ok)
    {
        QMessageBox::warning(this, "ERROR", "Entered step is not correct", QMessageBox::Ok);
        //Restore previous step
        QString val = misc::num2str(stepValue);
        int index = 5;//By default, no scaling
        val = SeparateMagnitudeFromSuffix(val, index);
        step->setText(val);
        StepUnitsCombobox->setCurrentIndex(index);
        return;
    }

    stepValue = v;
}


/*
 * The control reaches this function when the slider position (i.e. the value of the variable) is changed
 */
void tunerElement::slotSliderChanged()
{
     value->blockSignals(true);
     ValueUnitsCombobox->blockSignals(true);
     float slider_v = minValue + ((slider->value()/100.0) * (maxValue - minValue));
     if (slider_v > maxValue)
     {
         value->setText(QString::number(maxValue));
         slider_v = maxValue;
     }
     else if (slider_v < minValue)
     {
         value->setText(QString::number(minValue));
         slider_v = minValue;
     }
     numValue = slider_v;
     //Update text & combobox. This is specially needed in case the input value exceeds the minumum and max. limits
     QString val = misc::num2str(numValue);
     int index = 5;//By default, no scaling
     val = SeparateMagnitudeFromSuffix(val, index);
     value->setText(val);
     ValueUnitsCombobox->setCurrentIndex(index);
     value->blockSignals(false);
     ValueUnitsCombobox->blockSignals(false);
     slotValueChanged();
}

/*
 * The variable can be modified by moving the slider, editing the corresponding QLineedit box or changing the scale factor.
 * The control reaches this function when one of the events above is triggered. It checks if the input value is correct, updates it
 * and finally runs the simulation
 */
void tunerElement::slotValueChanged()
{
    bool ok;
    float v = getValue(ok);
    value->blockSignals(true);
    ValueUnitsCombobox->blockSignals(true);
    if (!ok || (v < 0))
    {
        QMessageBox::warning(this, "ERROR", "Value not correct", QMessageBox::Ok);
        //Restore values
        QString val = misc::num2str(numValue);
        int index = 5;//By default, no scaling
        val = SeparateMagnitudeFromSuffix(val, index);
        value->setText(val);
        ValueUnitsCombobox->setCurrentIndex(index);
        value->blockSignals(false);
        ValueUnitsCombobox->blockSignals(false);
        return;
    }

    numValue = v;
    bool updateText = false;
    //Force numValue to stay within [minValue, maxValue]
    if (v > maxValue) numValue = maxValue, updateText = true;
    if (v < minValue) numValue = minValue, updateText = true;

    if (updateText)
    {
        //Update text... because of the lines above numValue may be set to maxValue or minValue so it may differ
        //from what the user entered
        QString val = misc::num2str(numValue);
        int index = 5;//By default, no scaling
        val = SeparateMagnitudeFromSuffix(val, index);
        value->setText(val);
        ValueUnitsCombobox->setCurrentIndex(index);
    }

    updateSlider();
    updateProperty();
    emit elementValueUpdated();
    value->blockSignals(false);
    ValueUnitsCombobox->blockSignals(false);
}

// Up button click
void tunerElement::slotUpClicked()
{
    bool ok;
    float s = getStep(ok);
    qDebug() << "tunerElement::slotUpClicked() " << s;

    numValue = getValue(ok) + s;
    if (numValue > maxValue) numValue = getMaxValue(ok);

    value->setText(QString::number(numValue/getScale(ValueUnitsCombobox->currentIndex()), 'f', 3));

    updateSlider();//First, update the slider. Then update the value since this slot will take the value from the slider
    slotValueChanged();
}

//Down button click
void tunerElement::slotDownClicked()
{
    bool ok;
    float s = getStep(ok);
    qDebug() << "tunerElement::slotDownClicked() " << s;

    numValue = getValue(ok) - s;
    if (numValue < getMinValue(ok)) numValue = getMinValue(ok);

    value->setText(QString::number(numValue/getScale(ValueUnitsCombobox->currentIndex()), 'f', 3));

    updateSlider();//First, update the slider. Then update the value since this slot will take the value from the slider
    slotValueChanged();

}

void tunerElement::slotDelete()
{
    this->setVisible(false);
    emit removeElement(this);
}

/*
The control reaches this function whenever the user changes the value-related inputs, i.e. textbox, scale combo or the slider
For this reason, some of the values given by the user may be out of range and there's a need to check that such data is correct
*/
void tunerElement::updateSlider()
{
    float v = (numValue - minValue) / (maxValue - minValue);
    slider->blockSignals(true);
    slider->setValue(v*100);
    slider->setTickInterval(100 / ((maxValue - minValue)/stepValue));
    slider->blockSignals(false);
}


// Given the current index of a combobox, this function returns the scale factor according to the selection.
// Notice that this function does not belong neither tunerElement nor tunerDialog
float getScale(int index)
{
    switch (index)
    {
    case 0: return pow(10, -15);//femto
    case 1: return pow(10, -12);//pico
    case 2: return pow(10 ,-9);//nano
    case 3: return pow(10, -6);//micro
    case 4: return pow(10,-3);//mili
    case 5: return 1;
    case 6: return pow(10, 3);//kilo
    case 7: return pow(10, 6);//mega
    case 8: return pow(10, 9);//giga
    }
    return 0;
}

// Reads the value from the user interface
float tunerElement::getValue(bool &ok)
{
   return value->text().toFloat(&ok)*getScale(ValueUnitsCombobox->currentIndex());
}

// Reads the maximum value from the user interface
float tunerElement::getMaxValue(bool &ok)
{
   return maximum->text().toFloat(&ok)*getScale(MaxUnitsCombobox->currentIndex());
}

// Reads the minimum value from the user interface
float tunerElement::getMinValue(bool &ok)
{
   return minimum->text().toFloat(&ok)*getScale(MinUnitsCombobox->currentIndex());
}

// Reads the step from the user interface
float tunerElement::getStep(bool &ok)
{
   return step->text().toFloat(&ok)*getScale(StepUnitsCombobox->currentIndex());
}

tunerElement::~tunerElement()
{
    //dtor
}

/* \brief
 * TUNER Dialog Class
 *
 *
 * Tuner dialog contains tuner elements
 * Updates property values
 * Resets values
 *
 */


//Main window. It contains zero or more tunerElement objects
TunerDialog::TunerDialog(QWidget *_w, QWidget *parent) :
 QDialog(parent), w(_w)
{
    setAttribute(Qt::WA_DeleteOnClose);//This attribute forces the widget to be destroyed after closing
    qDebug() << "Tuner::TunerDialog";
    this->setObjectName("Tuner");
    this->setWindowTitle("Tuner");
    gbox = new QGridLayout();
    this->setLayout(gbox);

    splitter = new QSplitter(parent);
    ButtonsPanel = new QWidget();
    QGridLayout * buttonsLayout = new QGridLayout();
    ButtonsPanel->setLayout(buttonsLayout);

    closeButton = new QPushButton("Close", this);
    updateValues = new QPushButton("Update Values", this);
    updateValues->setEnabled(false);//It doesn't make sense to activate it at first... only when at least a tuning element is active...
    resetValues = new QPushButton("Reset Values", this);
    resetValues->setEnabled(false);

    //When pressing the Enter key at the Maxium lineedit focus is automatically set to the reset button leading to
    //confunsing and unexpected behavior. In this sense, the lines below are a workaround. The focus is rejected for
    //these buttons, so that focus can never reach them
    resetValues->setFocusPolicy(Qt::NoFocus);
    updateValues->setFocusPolicy(Qt::NoFocus);
    closeButton->setFocusPolicy(Qt::NoFocus);

    info = new QStatusBar;
    buttonsLayout->addWidget(resetValues);
    buttonsLayout->addWidget(updateValues);
    buttonsLayout->addWidget(closeButton);

    gbox->addWidget(ButtonsPanel, 0, 0, Qt::AlignTop);
    gbox->addWidget(info, 1, 0, Qt::AlignBottom);
    gbox->addWidget(splitter,0, 1, Qt::AlignRight);

    progressBar = new QProgressBar();
    progressBar->setMaximum(100);
    progressBar->setVisible(false);
    gbox->addWidget(progressBar, 2, 0);

    info->showMessage("Please select a component to tune");
    setMinimumWidth(300);//Otherwise, it won't fit the "help" text...
    valuesUpdated = false;
    connect(closeButton, SIGNAL(released()), this, SLOT(close()));
    connect(resetValues, SIGNAL(released()),this, SLOT(slotResetValues()));
    connect(updateValues, SIGNAL(released()), this, SLOT(slotUpdateValues()));

    //Management of the Esc shortcut. Otherwise, it will exit the tuner and leave the toogle button activated
    QShortcut *shortcut_Esc = new QShortcut(Qt::Key_Escape, this);
    QObject::connect(shortcut_Esc, SIGNAL(activated()), this, SLOT(close()));
}

void TunerDialog::slotUpdateProgressBar(int value)
{
    progressBar->setValue(value);
}

void TunerDialog::infoMsg(const QString msg)
{
    info->clearMessage();
    info->showMessage(msg);
}

bool TunerDialog::containsProperty(Property* prop)
{
    if (currentProps.contains(prop)) return true;
    else return false;
}

// Adds a tuner element (variable to be tuned)
void TunerDialog::addTunerElement(tunerElement *element)
{
    if (!element)
        return;

    for(auto &el : currentElements) {
        if (el->schematicName != element->schematicName) {
            QMessageBox::warning(this,tr("Add component"),
                                 tr("Adding components from different schematics is not supported!"));
            return;
        }
    }

    updateValues->setEnabled(true);
    resetValues->setEnabled(true);

    connect(element, SIGNAL(elementValueUpdated()), this, SLOT(slotElementValueUpdated()));
    connect(element, SIGNAL(removeElement(tunerElement*)), this, SLOT(slotRemoveTunerElement(tunerElement*)));

    if (!currentProps.contains(element->getElementProperty()))
    {
        splitter->addWidget(element);
        currentProps.append(element->getElementProperty());
        currentElements.append(element);
        this->adjustSize();
        this->update();
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Already Found");
        msgBox.setInformativeText("Already tuning this element");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    }
   info->clearMessage();//The user has already selected a component. It makes no longer sense to display the help message
}

// Remove tuner element(s) for a component
void TunerDialog::slotComponentDeleted(Component *c)
{
  // remove elements related to the component
  for(int i = currentElements.count()-1; i >= 0; --i) {
    if (currentElements.at(i)->c == c)
      slotRemoveTunerElement(currentElements.at(i));
  }
}

void TunerDialog::slotRemoveTunerElement(tunerElement *e)
{
    qDebug() << "Tuner::slotRemoveTunerElement()";
    currentProps.removeAll(e->getElementProperty());
    currentElements.removeAll(e);//This will also destroy the element in QSplitter: https://stackoverflow.com/questions/371599/how-to-remove-qwidgets-from-qsplitter
    delete e;
    if (currentProps.size() == 0)
    {//No tuning boxes
        updateValues->setEnabled(false);
        resetValues->setEnabled(false);
        info->showMessage("Please select a component to tune");
    }
    this->adjustSize();
    this->update();
}

void TunerDialog::slotElementValueUpdated()
{
    qDebug() << "Tuner::slotElementValueUpdated()";

    progressBar->setVisible(true);
    this->setEnabled(false);

    switch (QucsSettings.DefaultSimulator) {
    case spicecompat::simQucsator:
        QucsMain->slotSimulate(w);
        break;
    case spicecompat::simNgspice:
    case spicecompat::simXyce:
    case spicecompat::simSpiceOpus:
        QucsMain->slotSimulateWithSpice();
        break;
    default: break;
    }
}

void TunerDialog::SimulationEnded()
{
    qDebug() << "Tuner::SimulationEnded()";

    this->setEnabled(true);
    progressBar->setVisible(false);
}


bool TunerDialog::checkChanges()
{
  for (int i = 0; i < currentElements.count(); i++)
  {
      int index=-1;
      QString val = SeparateMagnitudeFromSuffix(currentElements.at(i)->originalValue, index);
      float initial_val = val.toFloat()*getScale(index);
      if (initial_val != currentElements.at(i)->numValue) return true;
  }
  return false;
}

void TunerDialog::slotResetValues()
{
    qDebug() << "Tuner::slotResetValues()";
    for (int i = 0; i < currentElements.count(); i++)
    {
        currentElements.at(i)->resetValue();
    }
}

void TunerDialog::slotUpdateValues()
{
    qDebug() << "Tuner::slotUpdateValues()";
    for (int i = 0; i < currentElements.count(); i++)
    {
        qDebug() << "Tuner::slotUpdateValues()::updateProperty()";
        currentElements.at(i)->updateProperty();
    }
    if (w != nullptr) { // Reflect changes in undo buffer
        Schematic *d = dynamic_cast<Schematic*>(w);
        d->setChanged(true,true);
    }
    this->infoMsg("Updated Schematic values");
    valuesUpdated = true;
}

void TunerDialog::slotResetTunerDialog()
{
    // Document closed. Reset tuner
    qDebug() << "Tuner::slotResetTunerDialog()";
    infoMsg("Document closed");
    for (int i = 0; i < currentElements.count(); i++)
    {
       qDebug() << "Tuner::slotResetTunerDialog()::delete";
       delete currentElements.at(i);
       currentElements.removeAt(i);
    }
    this->update();
}

void TunerDialog::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton msg;
    if (!valuesUpdated && checkChanges())
    {
    msg = QMessageBox::question(this, "Update values before closing?",
                                "Do you want to update the component values before closing?",
                               QMessageBox::Yes | QMessageBox::No );

    if (msg == QMessageBox::Yes)
    {
        qDebug() << "slotCloseClicked::User clicked yes";
        slotUpdateValues();
    }
    else
        slotResetValues();
    }
    this->valuesUpdated = false;
    //Undo changes to mouse actions when closing tuner window
    QucsMain->MousePressAction = &MouseActions::MPressSelect;
    QucsMain->MouseReleaseAction = &MouseActions::MReleaseSelect;
    QucsMain->tune->setChecked(false);
   event->accept();
}

void TunerDialog::showEvent(QShowEvent *e)
{
    Q_UNUSED(e);
    for (int i = 0; i < currentElements.count(); i++)
    {
        if (currentElements.at(i)->getElementProperty() == nullptr)
            currentElements.removeAt(i);
    }
}

TunerDialog::~TunerDialog()
{

}

