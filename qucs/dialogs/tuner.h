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
#ifndef TUNER_H
#define TUNER_H

#include <QWidget>
#include "qucs.h"
#include "components/components.h"
#include "misc.h"
#include <cmath>
#include "mouseactions.h"

#include <QWidget>
#include <QDialog>
#include <QPushButton>
#include <QGridLayout>
#include <QLineEdit>
#include <QString>
#include <QSlider>
#include <QLabel>
#include <QToolButton>
#include <QComboBox>
#include <QSplitter>
#include <QStatusBar>
#include <QAction>
#include <QTextEdit>
#include <QMessageBox>
#include <QDebug>
#include <QShortcut>
#include <QProgressBar>

extern QucsApp *QucsMain;  // the Qucs application itself

float getScale(int);
QString SeparateMagnitudeFromSuffix(QString num, int &);
bool checkProperty(Component *component, Property *pp);

class tunerElement : public QWidget
{
    Q_OBJECT
    public:
        tunerElement(QWidget *parent, Component*, Property *, int);
        Property* getElementProperty();
        void resetValue();
        void updateProperty();
        float getValue(bool &);
        float getMaxValue(bool &);
        float getMinValue(bool &);
        float getStep(bool &);
        QString originalValue;//OriginalValue and numValue need to be public since they need to be accessed by tunerDialog when checking if the initial value was modified
        float numValue;
        Component *c;

        virtual ~tunerElement();
    signals:
        void elementValueUpdated();
        void removeElement(tunerElement*);

    protected:
    private:
        Property* prop;
        QString unit;
        QSlider *slider;
        QLineEdit *maximum;
        QLineEdit *minimum;
        QLineEdit *value;
        QLineEdit *step;
        QComboBox *MaxUnitsCombobox;
        QComboBox *MinUnitsCombobox;
        QComboBox *ValueUnitsCombobox;
        QComboBox *StepUnitsCombobox;
        QWidget *Up_Down_Buttons_Widget;
        QToolButton *up;
        QToolButton *down;

        float minValue;
        float maxValue;
        float stepValue;

    private slots:

        void slotMinValueChanged();
        void slotMaxValueChanged();
        void slotStepChanged();
        void slotValueChanged();
        void slotDelete();
        void slotDownClicked();
        void slotUpClicked();
        void slotSliderChanged();
        void updateSlider();
};

class TunerDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TunerDialog(QWidget *_w = 0, QWidget *parent = 0);
    virtual ~TunerDialog();
    void addTunerElement(tunerElement *element);
    bool containsProperty(Property *prop);
    void SimulationEnded();

signals:
    void addTunerElement(Element *e);
public slots:
    void slotResetTunerDialog();
    void slotComponentDeleted(Component *);
protected:
    virtual void showEvent(QShowEvent *);

private:
    QWidget *w; // widget holding the Qucs document to be tuned
    QPushButton *closeButton;
    QGridLayout *gbox;
    QList<tunerElement*> *currentElements;
    QList<Property*> *currentProps;
    QStatusBar *info;
    QSplitter *splitter;
    QWidget *ButtonsPanel;
    bool valuesUpdated;
    QProgressBar *progressBar;
    QPushButton *updateValues, *resetValues;//They're private in order to make enable or disable them

    void blockInput(bool enabled);
    void closeEvent(QCloseEvent *event);
    void infoMsg(const QString msg);

private slots:
    void slotElementValueUpdated();
    void slotRemoveTunerElement(tunerElement*);
    void slotUpdateValues();
    void slotResetValues();
    bool checkChanges();
    void slotUpdateProgressBar(int);
};

#endif // TUNER_H
