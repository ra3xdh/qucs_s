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
  int symbolPinsCount;

  QPushButton *btnOpenLib, *btnOK, *btnApply, *btnCancel;
  QTableWidget *tbwPinsTable;
  QComboBox *cbxSelectSubcir, *cbxSymPattern;

  QMap<QString,QStringList> subcirPins;

  bool parseLibFile(const QString &filename);

private slots:
  void slotBtnOpenLib();
  void slotSetSymbol();
  void slotFillSubcirComboBox();
  void slotFillPinsTable();
  void slotTableCellDoubleClick();
  void slotSelectPin();

public:
  explicit SpiceLibCompDialog(Component *pc, QWidget* parent = nullptr);

public slots:

  void slotBtnOK();
  void slotBtnApply();
  void slotBtnCancel();

signals:
};

#endif // SPICELIBCOMPDIALOG_H
