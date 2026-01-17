/// @file general.h
/// @brief Utility functions needed across the whole project
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 4, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifndef GENERAL_H
#define GENERAL_H

#include <QFile>
#include <QFileInfo>
#include <QString>

#include <QList>
#include <QPointF>
#include <QRegularExpression>
#include <cmath>
#include <complex>

// CONSTANTS
static constexpr double Z0  = 376.730313668;     // Free space impedance
static constexpr double C0  = 299792458.0;       // Speed of light
static constexpr double MU0 = 4.0 * M_PI * 1e-7; // Permeability of free space
static constexpr double pi_over_2   = M_PI / 2.0;
static constexpr double two_over_pi = 2.0 / M_PI;
static constexpr double one_over_pi = 1.0 / M_PI;
static constexpr double euler       = M_E;
static constexpr double ln2         = 0.693147180559945;

/// @brief Unit types for engineering values
enum Units { Capacitance, Inductance, Length, Resistance, Degrees, NoUnits };

/// @brief Rounds double to minimum decimal places needed
/// @param val Value to round
/// @return Formatted string
QString RoundVariablePrecision(double);

/// @brief Converts complex number to string with units
/// @param Z Complex value
/// @param CompType Unit type
/// @return Formatted string
QString num2str(std::complex<double> Z, Units CompType);

/// @brief Converts double to string with engineering notation and units
/// @param Num Value to convert
/// @param CompType Unit type
/// @return Formatted string
QString num2str(double, Units);

/// @brief Converts double to string with engineering notation
/// @param Num Value to convert
/// @return Formatted string
QString num2str(double);

/// @brief Parses string to complex number
/// @param num String representation (e.g., "50+j25", "j50", "50")
/// @return Complex number
std::complex<double> Str2Complex(QString);

/// @brief Converts length in meters to appropriate unit with auto-scaling
/// @param units Current unit (mm, mil, um, nm, inch, ft, m)
/// @param len Length in meters
/// @return Formatted string with auto-scaled unit
QString ConvertLengthFromM(QString, double);

/// @brief Converts S-parameter from MA/RI/DB format to dB, angle, real, and imaginary
/// @param[in,out] S_1 Magnitude (MA), Real (RI), or dB (DB) → dB output
/// @param[in,out] S_2 Angle (MA/DB) or Imaginary (RI) → angle output
/// @param[out] S_3 Real part output
/// @param[out] S_4 Imaginary part output
/// @param format Input format: "MA", "RI", or "DB" (case insensitive)
void convert_MA_RI_to_dB(double& S_1, double& S_2, double& S_3, double& S_4,
                         QString format);

/// @brief Gets frequency scale factor from unit string
/// @param frequency_unit Unit string (Hz, kHz, MHz, GHz)
/// @return Scale factor relative to Hz
double getFreqScale(QString frequency_unit);

/// @brief Gets scale factor from SI prefix
/// @param scale Prefix string (Y, Z, E, P, T, G, M, k, m, u, n, p, f, a, z, y)
/// @return Scale factor
double getScaleFactor(QString scale);

/// @brief Finds index of closest value in list
/// @param list List to search
/// @param value Target value
/// @return Index of closest element
int findClosestIndex(const QList<double>& list, double value);

/// @brief Parses frequency string to Hz
/// @param freq Frequency string (e.g., "2.4 GHz", "100MHz")
/// @return Frequency in Hz, or -1 if invalid
double getFreqFromText(QString freq);

/// @brief Finds closest point in x-y data series
/// @param xValues X-axis values
/// @param yValues Y-axis values
/// @param targetX Target x value
/// @return Closest point as QPointF
QPointF findClosestPoint(const QList<double>& xValues,
                         const QList<double>& yValues, double targetX);

/// @brief Reads Touchstone file and extracts S-parameter data
/// @param filePath Path to the Touchstone file (.sNp)
/// @return Map of variable names to data arrays
QMap<QString, QList<double>> readTouchstoneFile(const QString& filePath);

#endif // GENERAL_H
