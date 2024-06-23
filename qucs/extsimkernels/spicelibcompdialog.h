#ifndef SPICELIBCOMPDIALOG_H
#define SPICELIBCOMPDIALOG_H

#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QMap>
#include <QtWidgets>

class Component;
class Schematic;
class SymbolWidget;


class SpiceLibCompDialog : public QDialog {
  Q_OBJECT

private:
  int symbolPinsCount;
  bool isChanged;
  bool libError;

  QString lastSymbolDir;
  QString lastLibDir;

  Component *comp;
  Schematic *Doc;

  SymbolWidget *symbol;
  QLineEdit *edtLibPath, *edtParams, *edtSymFile;
  QPlainTextEdit *edtSPICE;

  QPushButton *btnOpenLib, *btnOK, *btnApply, *btnCancel, *btnOpenSym;
  QTableWidget *tbwPinsTable;
  QComboBox *cbxSelectSubcir;
  QListWidget *listSymPattern;

  QRadioButton *rbSymFromTemplate, *rbAutoSymbol, *rbUserSym;
  QCheckBox *chbShowLib, *chbShowModel, *chbShowParams;

  QMap<QString,QStringList> subcirPins;
  QMap<QString,QString> subcirSPICE;

  enum SPICEparseError { noError=0, failedOpenFile = -1, noSUBCKT = -2 };

  int parseLibFile(const QString &filename);
  bool setCompProps();

private slots:
  void slotBtnOpenLib();
  void slotBtnOpenSym();
  void slotSetSymbol();
  void slotFillSubcirComboBox();
  void slotFillPinsTable();
  void slotTableCellDoubleClick();
  void slotSelectPin();
  void slotChanged();

public:
  explicit SpiceLibCompDialog(Component *pc, Schematic *sch);

public slots:

  void slotBtnOK();
  void slotBtnApply();
  void slotBtnCancel();

signals:
};

#endif // SPICELIBCOMPDIALOG_H
