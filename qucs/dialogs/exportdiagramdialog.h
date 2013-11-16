#ifndef EXPORTDIAGRAMDIALOG_H
#define EXPORTDIAGRAMDIALOG_H

#include <QObject>
#include <QtGui>
#include <QDialog>


class ExportDiagramDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ExportDiagramDialog(int w, int h, QWidget *parent = 0);
    
private:
    QPushButton* ExportButt;
    QPushButton* CancelButt;
    QPushButton* SaveButt;

    QLabel* lblFilename;
    QLabel *lblResolutionX;
    QLabel *lblResolutionY;

    QCheckBox* cbResolution;
    QCheckBox* cbRatio;

    QLineEdit* editFilename;
    QLineEdit* editResolutionX;
    QLineEdit* editResolutionY;

    QHBoxLayout* lower1;
    QHBoxLayout* lower2;
    QHBoxLayout* lower3;
    QVBoxLayout* top;

    int dwidth, dheight;

    bool svg;

public:
    QString FileToSave();
    bool isOriginalSize();
    int Xpixels();
    int Ypixels();
    bool isSvg();

signals:
    
private slots:
    void setFileName();
    void calcWidth();
    void calcHeight();
    void recalcRatio();
    void restoreOriginalWtoH();
    void setSvg(QString filename);

public slots:
    
};

#endif // EXPORTDIAGRAMDIALOG_H
