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
class ComponentLibrary;
class SymbolWidget;

class SelFromLibDialog : public QDialog {
  Q_OBJECT
private:

  Component *comp;

  QComboBox *cbxLib;
  QListWidget *lstComps;
  QPushButton *btnOK, *btnApply, *btnCancel;
  QPlainTextEdit *edtDescr;
  QString LibModel;

  QList<ComponentLibrary *> parsedLibs;

  SymbolWidget *Symbol;

  void fillLibComboBox();
  void selectLibraryAndComp();
  void checkAndParseLibrary(const QString &libdir, bool relpath);

  void fillCompProperties();

public:  
  SelFromLibDialog(Component *c);

public slots:

  void slotShowComponents();
  void slotShowDescription();
  void slotOK();
  void slotApply();

};

#endif // SELFROMLIBDIALOG_H
