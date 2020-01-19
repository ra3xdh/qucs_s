/***************************************************************************
                             textboxdialog.h
                            -----------------
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/



#ifndef TEXTBOXDIALOG_H
#define TEXTBOXDIALOG_H

#include "components/component.h"
#include <QtGui>


/*!
  \file textboxdialog.h
  \brief Implementation of the TextBoxDialog class
*/

/*!
 * \brief The TextBoxDialog class is responsible for editing a single
 *        text entry of a generic component.
 */
class TextBoxDialog : public QDialog
{
    Q_OBJECT

private:

    Component* comp;
    QTextEdit* edtCode;
    QPushButton *btnOK;
    QPushButton *btnApply;
    QPushButton *btnCancel;

public:
    explicit TextBoxDialog(const char* window_title, Component *pc, QWidget *parent = 0);

signals:

private slots:
    void slotApply();
    void slotOK();
    void slotCancel();

public slots:

};

#endif // CUSTOMSIMDIALOG_H
