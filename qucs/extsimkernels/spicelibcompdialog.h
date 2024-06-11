#ifndef SPICELIBCOMPDIALOG_H
#define SPICELIBCOMPDIALOG_H

#include <QObject>
#include <QWidget>
#include <QDialog>

class QPushButton;
class QTableWidget;
class QComboBox;
class QLineEdit;
class Component;
class SymbolWidget;

class SpiceLibCompDialog : public QDialog {
  Q_OBJECT

private:
  Component *comp;

  SymbolWidget *symbol;
  QLineEdit *edtLibPath;

  QPushButton *btnOpenLib, *btnOK, *btnApply, *btnCancel;
  QTableWidget *tbwPinsTable;
  QComboBox *cbxSelectSubcir, *cbxSymPattern;

  void fillSubcirComboBox();

private slots:
  void slotBtnOpenLib();

public:
  explicit SpiceLibCompDialog(Component *pc, QWidget* parent = nullptr);

public slots:

  void slotBtnOK();
  void slotBtnApply();
  void slotBtnCancel();

signals:
};

#endif // SPICELIBCOMPDIALOG_H
