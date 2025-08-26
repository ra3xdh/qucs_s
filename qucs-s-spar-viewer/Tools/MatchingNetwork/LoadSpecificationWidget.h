#ifndef LOADSPECIFICATIONWIDGET_H
#define LOADSPECIFICATIONWIDGET_H

#include <QWidget>
#include <QGroupBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QPushButton>
#include <QFileDialog>
#include <QButtonGroup>
#include <complex>
#include <QMessageBox>
#include <QTextStream>
#include <cmath>
#include <QRegularExpression>

class LoadSpecificationWidget : public QGroupBox
{
    Q_OBJECT

public:
    explicit LoadSpecificationWidget(QWidget *parent = nullptr);
    ~LoadSpecificationWidget();

    // Getters
    std::complex<double> getLoadImpedance() const;
    std::complex<double> getReflectionCoefficient() const;
    bool isTwoPortMode() const { return m_twoPortMode; }
    
    // S-parameter getters (for two-port mode)
    std::complex<double> getS11() const;
    std::complex<double> getS12() const;
    std::complex<double> getS21() const;
    std::complex<double> getS22() const;
    
    // Setters
    void setLoadImpedance(const std::complex<double>& impedance);
    void setReflectionCoefficient(const std::complex<double>& gamma);
    void setTwoPortMode(bool enabled);
    void setReferenceImpedance(double Z0) { m_Z0 = Z0; updateReflectionCoefficient(); }

private slots:
    void onImpedanceChanged();
    void onReflectionCoefficientChanged();
    void onFormatChanged();
    void onInputMethodChanged();
    void onBrowseFile();
    void onSParameterChanged();

private:
    void setupUI();
    void setupOnePortUI();
    void setupTwoPortUI();
    void updateReflectionCoefficient();
    void updateImpedance();
    void updateSParameterDisplays();
    void updateImpedanceFormat();
    void updateReflectionFormat();
    void updateSParameterFormat();
    void loadS1PFile(const QString& filename);
    void loadS2PFile(const QString& filename);
    
    // UI Components - One Port
    QGridLayout* m_mainLayout;
    
    // Input method selection
    QRadioButton* m_manualInputRadio;
    QRadioButton* m_fileInputRadio;
    QButtonGroup* m_inputMethodGroup;
    QPushButton* m_browseButton;
    QLabel* m_fileLabel;
    
    // Format selection
    QLabel* m_formatLabel;
    QComboBox* m_formatCombo;
    
    // Manual input widgets - Impedance
    QLabel* m_impedanceLabel;
    QDoubleSpinBox* m_impedanceReal;
    QLabel* m_impedanceSeparator;
    QDoubleSpinBox* m_impedanceImag;
    QLabel* m_impedanceUnit;
    
    // Manual input widgets - Reflection coefficient
    QLabel* m_reflectionLabel;
    QDoubleSpinBox* m_reflectionReal;
    QLabel* m_reflectionSeparator;
    QDoubleSpinBox* m_reflectionImag;
    
    // Two-port widgets
    QWidget* m_twoPortWidget;
    QGridLayout* m_twoPortLayout;
    
    // S-parameter widgets
    QLabel* m_s11Label;
    QDoubleSpinBox* m_s11Real;
    QLabel* m_s11Separator;
    QDoubleSpinBox* m_s11Imag;
    
    QLabel* m_s12Label;
    QDoubleSpinBox* m_s12Real;
    QLabel* m_s12Separator;
    QDoubleSpinBox* m_s12Imag;
    
    QLabel* m_s21Label;
    QDoubleSpinBox* m_s21Real;
    QLabel* m_s21Separator;
    QDoubleSpinBox* m_s21Imag;
    
    QLabel* m_s22Label;
    QDoubleSpinBox* m_s22Real;
    QLabel* m_s22Separator;
    QDoubleSpinBox* m_s22Imag;
    
    // Internal state
    bool m_twoPortMode;
    bool m_updatingValues;
    double m_Z0; // Reference impedance
    QString m_currentFile;

signals:
    void impedanceChanged();
    void reflectionCoefficientChanged();
    void sParametersChanged();
};

#endif // LOADSPECIFICATIONWIDGET_H
