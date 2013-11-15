#ifndef EXPORTDIAGRAMDIALOG_H
#define EXPORTDIAGRAMDIALOG_H

#include <QObject>
#include <QtGui>
#include <QDialog>


class ExportDiagramDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ExportDiagramDialog(QWidget *parent = 0);
    
private:
    QPushButton* ExportButt;
    QPushButton* CancelButt;
    QPushButton* SaveButt;

    QLabel* lblFilename;
    QLabel *lblResolutionX;
    QLabel *lblResolutionY;

    QCheckBox* cbResolution;

    QLineEdit* editFilename;
    QLineEdit* editResolutionX;
    QLineEdit* editResolutionY;

    QHBoxLayout* lower1;
    QHBoxLayout* lower2;
    QHBoxLayout* lower3;
    QVBoxLayout* top;

public:
    QString FileToSave();
    bool OriginalSize();
    int Xpixels();
    int Ypixels();

signals:
    
private slots:
    void setFileName();

public slots:
    
};

#endif // EXPORTDIAGRAMDIALOG_H
