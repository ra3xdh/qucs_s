/***************************************************************************
                             customdialog.h
                             ----------------
    begin                : Mon Apr 13 2015
    copyright            : (C) 2015 by Vadim Kuznetsov
    email                : ra3xdh@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/



#ifndef CUSTOMSIMDIALOG_H
#define CUSTOMSIMDIALOG_H

#include "schematic.h"
#include "spicecomponents/sp_customsim.h"
#include "components/component.h"
#include <QtGui>


/*!
  \file customsimdialog.h
  \brief Implementation of the CustomSimDialog class
*/

/*!
 * \brief The CustomSimDialog class is responsible for editing properties
 *        of Ngspice Custom Simulation component. You can edit multiline
 *        component properties with it.
 */
class CustomSimDialog : public QDialog
{
    Q_OBJECT

private:

    bool isXyceScr;
    bool isChanged = false;

    SpiceCustomSim* comp;
    Schematic *Sch;

    QTextEdit* edtCode;
    QCheckBox *checkCode;
    QPushButton *btnOK;
    QPushButton *btnApply;
    QPushButton *btnCancel;
    QPushButton *btnPlotAll;
    QPushButton *btnFindOutputs;

    QLineEdit *edtVars;
    QLineEdit *edtOutputs;

public:
    explicit CustomSimDialog(SpiceCustomSim *pc, Schematic *sch, QWidget *parent = 0);
    
signals:

private slots:
    void slotApply();
    void slotOK();
    void slotCancel();
    void slotFindVars();
    void slotFindOutputs();
    void slotChanged();
    
public slots:
    
};

#endif // CUSTOMSIMDIALOG_H
