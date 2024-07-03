#ifndef FILLFROMSPICEDIALOG_H
#define FILLFROMSPICEDIALOG_H

#include <QDialog>
#include <QMap>

class Component;
class Schematic;
class QPushButton;
class QPlainTextEdit;
class QCheckBox;

class fillFromSpiceDialog : public QDialog {
  Q_OBJECT

private:

  Component *Comp;
  QMap<QString,QString> parsedProps;
  QString ModelName;
  QString ModelType;

  enum errorCode { noError = 0, noModel = 1, modelMismatch = 2, wrongModel = 3 };

  QPlainTextEdit *edtModel;
  QPushButton *btnOK, *btnCancel;
  QCheckBox *chbNumNotation;

  int parseModelcard();
  void fillCompProps();
  void showErrorMsg(int code);
  QString convertNumNotation(const QString &value);

private slots:
  void slotOK();

public:
  fillFromSpiceDialog(Component *pc, QWidget *w = nullptr);
};

#endif // FILLFROMSPICEDIALOG_H
