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
  int a_symbolPinsCount;
  bool a_isChanged;
  bool a_libError;

  int a_prev_row;
  int a_prev_col;

  QString a_lastSymbolDir;
  QString a_lastLibDir;

  Component* a_comp;
  Schematic* a_Doc;

  SymbolWidget *a_symbol;
  QLineEdit* a_edtLibPath;
  QLineEdit* a_edtParams;
  QLineEdit* a_edtSymFile;
  QPlainTextEdit* a_edtSPICE;

  QPushButton* a_btnOpenLib;
  QPushButton* a_btnOK;
  QPushButton* a_btnApply;
  QPushButton* a_btnCancel;
  QPushButton* a_btnOpenSym;
  QTableWidget* a_tbwPinsTable;
  QComboBox* a_cbxSelectSubcir;
  QListWidget* a_listSymPattern;

  QRadioButton* a_rbSymFromTemplate;
  QRadioButton* a_rbAutoSymbol;
  QRadioButton* a_rbUserSym;
  QCheckBox* a_chbShowLib;
  QCheckBox* a_chbShowModel;
  QCheckBox* a_chbShowParams;

  QMap<QString, QStringList> a_subcirPins;
  QMap<QString, QString> a_subcirSPICE;

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
