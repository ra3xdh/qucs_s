#ifndef CUSTOMSIMDIALOG_H
#define CUSTOMSIMDIALOG_H

#include "schematic.h"
#include "components/sp_customsim.h"
#include "components/component.h"
#include <QtGui>

class CustomSimDialog : public QDialog
{
    Q_OBJECT

private:

    SpiceCustomSim* comp;
    Schematic *Sch;

    QTextEdit* edtCode;
    QPushButton *btnOK;
    QPushButton *btnApply;
    QPushButton *btnCancel;
    QPushButton *btnPlotAll;

    QLineEdit *edtVars;

public:
    explicit CustomSimDialog(SpiceCustomSim *pc, Schematic *sch, QWidget *parent = 0);
    
signals:

private slots:
    void slotApply();
    void slotOK();
    void slotCancel();
    void slotFindVars();
    
public slots:
    
};

#endif // CUSTOMSIMDIALOG_H
