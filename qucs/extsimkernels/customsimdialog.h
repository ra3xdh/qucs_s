#ifndef CUSTOMSIMDIALOG_H
#define CUSTOMSIMDIALOG_H

#include "components/component.h"
#include <QtGui>

class CustomSimDialog : public QDialog
{
    Q_OBJECT

private:

    QTextEdit* edtCode;
    QPushButton *btnOK;
    QPushButton *btnApply;
    QPushButton *btnCancel;

public:
    explicit CustomSimDialog(QWidget *parent = 0);
    
signals:

private slots:
    void slotApply();
    void slotOK();
    void slotCancel();
    
public slots:
    
};

#endif // CUSTOMSIMDIALOG_H
