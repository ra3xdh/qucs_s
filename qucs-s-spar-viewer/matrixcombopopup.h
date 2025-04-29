// matrixcombobox.h
#ifndef MATRIXCOMBOBOX_H
#define MATRIXCOMBOBOX_H

#include <QComboBox>
#include <QFrame>
#include <QPushButton>
#include <QStringList>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QPaintEvent>

class MatrixComboPopup : public QFrame
{
  Q_OBJECT

public:
  MatrixComboPopup(const QStringList &sParams, const QStringList &otherParams, QComboBox *parent = nullptr);
  void showBelow(QWidget *widget);

private slots:
  void selectItem();

private:
  QComboBox *parentCombo;
};

class MatrixComboBox : public QComboBox
{
  Q_OBJECT

public:
  explicit MatrixComboBox(QWidget *parent = nullptr);
  void setParameters(const QStringList &sParams, const QStringList &otherParams);

protected:
  void showPopup() override;
  void hidePopup() override;
  void paintEvent(QPaintEvent *event) override;

private:
  QStringList sParams;
  QStringList otherParams;
  bool popupVisible;
  MatrixComboPopup *popup;
};

#endif // MATRIXCOMBOBOX_H
