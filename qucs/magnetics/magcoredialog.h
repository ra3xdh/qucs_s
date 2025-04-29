#ifndef MAGCOREDIALOG_H
#define MAGCOREDIALOG_H

#include <QObject>
#include <QWidget>
#include <QDialog>

class QLineEdit;
class QComboBox;
class QLabel;
class Component;
class Schematic;
class QSvgWidget;
class QCheckBox;
class QStatusBar;

class MagCoreDialog : public QDialog {
  Q_OBJECT

private:

  Component *comp;
  Schematic *Sch;

  QLineEdit *edtA, *edtK, *edtC, *edtAlpha, *edtMs, *edtArea, *edtPath, *edtGap;
  QLineEdit *edtD1, *edtD2, *edtD3, *edtD4, *edtD5, *edtD6;
  QLabel *lblD1, *lblD2, *lblD3, *lblD4, *lblD5, *lblD6;
  QComboBox *cbxCoreType;
  QPushButton *btnOK, *btnApply, *btnCancel;
  QCheckBox *cbShowA, *cbShowK, *cbShowC, *cbShowAlpha, *cbShowMs,
      *cbShowArea, *cbShowPath, *cbShowGap;

  QSvgWidget *coreImg;

  QCheckBox *cbHBProbes;

  QStatusBar *sBar;

  void resetDimLabels();

  bool calcRingCore();
  bool calcECore();
  bool calcUCore();

  void setDimLabelsAtoF();

private slots:
  void slotSetCoreImage();
  void slotCalcPathArea();

public:
  explicit MagCoreDialog(Component *pc, Schematic *sch);

public slots:
  void slotApply();
  void slotOK();
  void slotCancel();

signals:
};

#endif // MAGCOREDIALOG_H
