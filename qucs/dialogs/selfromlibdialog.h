#ifndef SELFROMLIBDIALOG_H
#define SELFROMLIBDIALOG_H

#include <QDialog>
#include <QObject>
#include <QWidget>

class QComboBox;
class QPushButton;
class QListWidget;
class Component;
class QPlainTextEdit;

class SelFromLibDialog : public QDialog {
  Q_OBJECT
private:

  Component *comp;

  QComboBox *cbxLib;
  QListWidget *lstComps;
  QPushButton *btnOK, *btnApply, *btnCancel;
  QPlainTextEdit *edtDescr;

public:  
  SelFromLibDialog(Component *c);
public slots:

  void slotOK();
  void slotApply();

};

#endif // SELFROMLIBDIALOG_H
