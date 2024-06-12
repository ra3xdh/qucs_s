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
class Schematic;
class SymbolWidget;

class SpiceLibCompDialog : public QDialog {
  Q_OBJECT

private:
  Component *comp;
  Schematic *Doc;

  SymbolWidget *symbol;
  QLineEdit *edtLibPath, *edtParams;
  int symbolPinsCount;

  QPushButton *btnOpenLib, *btnOK, *btnApply, *btnCancel;
  QTableWidget *tbwPinsTable;
  QComboBox *cbxSelectSubcir, *cbxSymPattern;

  QMap<QString,QStringList> subcirPins;

  bool parseLibFile(const QString &filename);
  bool setCompProps();

private slots:
  void slotBtnOpenLib();
  void slotSetSymbol();
  void slotFillSubcirComboBox();
  void slotFillPinsTable();
  void slotTableCellDoubleClick();
  void slotSelectPin();

public:
  explicit SpiceLibCompDialog(Component *pc, Schematic *sch);

public slots:

  void slotBtnOK();
  void slotBtnApply();
  void slotBtnCancel();

signals:
};

#endif // SPICELIBCOMPDIALOG_H
