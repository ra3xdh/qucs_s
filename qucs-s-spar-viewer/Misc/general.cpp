/***************************************************************************
                                general.cpp
                                ----------
    author                :  2019 Andres Martinez-Mera
    email                  :  andresmmera@protonmail.com
 ***************************************************************************/

/***************************************************************************
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 ***************************************************************************/
#include "general.h"

// Rounds a double number using the minimum number of decimal places
QString RoundVariablePrecision(double val) {
  int precision = 0; // By default, it takes 2 decimal places
  int sign = 1;
  if (val < 0)
    sign = -1;
  val = std::abs(val);
  while (val * pow(10, precision) < 100)
    precision++; // Adds another decimal place if the conversion is less than
                 // 0.1, 0.01, etc
  return QString::number(sign * val, 'F',
                         precision); // Round to 'precision' decimals.
}


QString num2str(std::complex<double> Z, Units CompType) {
  QString Real = num2str(Z.real());

  if (abs(Z.imag()) < 1e-6) {
    return num2str(Z.real(), CompType);
  } else {
      Real = num2str(Z.real());
  }

  if (Z.imag() < 0) {
    return QString("%1 -j %2").arg(Real).arg(num2str(abs(Z.imag()), CompType));
  } else {
    return QString("%1 +j %2").arg(Real).arg(num2str(Z.imag(), CompType));
  }
}

QString num2str(double Num, Units CompType) {
  char c = 0;
  double cal = std::abs(Num);
  if (cal > 1e-20) {
    cal = std::log10(cal) / 3.0;
    if (cal < -0.2)
      cal -= 0.98;
    int Expo = int(cal);

    if (Expo >= -5)
      if (Expo <= 4)
        switch (Expo) {
        case -5:
          c = 'f';
          break;
        case -4:
          c = 'p';
          break;
        case -3:
          c = 'n';
          break;
        case -2:
          c = 'u';
          break;
        case -1:
          c = 'm';
          break;
        case 1:
          c = 'k';
          break;
        case 2:
          c = 'M';
          break;
        case 3:
          c = 'G';
          break;
        case 4:
          c = 'T';
          break;
        }

    if (c)
      Num /= pow(10.0, double(3 * Expo));
  }

  QString Str = RoundVariablePrecision(Num);
  if (c)
    Str += c;
  QString unit;
  switch (CompType) {
  case Capacitance:
    unit = QString("F");
    break;
  case Inductance:
    unit = QString("H");
    break;
  case Resistance:
    unit = QString("Ohm");
    break;
  case Degrees:
    unit = QString("º");
  default:
    break;
  }
  Str += unit;
  return Str;
}

QString num2str(double Num) {
  char c = 0;
  double cal = std::abs(Num);

  if (Num == 0){
    return QString("0");
  }

  if (cal > 1e-20) {
    cal = std::log10(cal) / 3.0;
    if (cal < -0.2)
      cal -= 0.98;
    int Expo = int(cal);

    if (Expo >= -5)
      if (Expo <= 4)
        switch (Expo) {
        case -5:
          c = 'f';
          break;
        case -4:
          c = 'p';
          break;
        case -3:
          c = 'n';
          break;
        case -2:
          c = 'u';
          break;
        case -1:
          c = 'm';
          break;
        case 1:
          c = 'k';
          break;
        case 2:
          c = 'M';
          break;
        case 3:
          c = 'G';
          break;
        case 4:
          c = 'T';
          break;
        }

    if (c)
      Num /= pow(10.0, double(3 * Expo));
  }

  QString Str = RoundVariablePrecision(Num);
  if (c)
    Str += c;
  return Str;
}

std::complex<double> Str2Complex(QString num) {

  // Remove the suffix
  if (num.indexOf("Ohm") != -1)
    num.remove(num.indexOf("Ohm"), 3);
  int index = num.indexOf("j"); // Indicates where the j is.
  if (index == -1)              // Actually, num is a real number
  {
    return std::complex<double>(num.toDouble(), 0);
  }

  // Need to separate the real from the imaginary part
  double sign = 1;
  if (num[index - 1] == '-')
    sign = -1;
  double realpart =
      num.left(index - 1)
          .toDouble(); // Notice  we have to take into account the sign
  double imagpart = num.right(index).toDouble();
  return std::complex<double>(realpart, sign * imagpart);
}

// This function creates a string for the transmission line length and
// automatically changes the unit length if the value lies outside [1,999.99]
QString ConvertLengthFromM(QString units, double len) {
  int index;

  if (units == "mm")
    index = 0;
  if (units == "mil")
    index = 1;
  if (units == "um")
    index = 2;
  if (units == "nm")
    index = 3;
  if (units == "inch")
    index = 4;
  if (units == "ft")
    index = 5;
  if (units == "m")
    index = 6;

  double conv;

  do {
    conv = len;
    switch (index) {
    case 1: // mils
      conv *= 39370.1;
      if (conv > 999.99) {
        index = 4; // inches
        break;
      }
      if (conv < 1) {
        index = 2; // microns
        break;
      }
      return QString("%1mil").arg(RoundVariablePrecision(conv));
    case 2: // microns
      conv *= 1e6;
      if (conv > 999.99) {
        index = 0; // milimeters
        break;
      }
      if (conv < 1) {
        index = 3; // nanometers
        break;
      }
      return QString("%1um").arg(RoundVariablePrecision(conv));
    case 3: // nanometers
      conv *= 1e9;
      if (conv > 999.99) {
        index = 2; // microns
        break;
      }
      return QString("%1nm").arg(RoundVariablePrecision(conv));
    case 4: // inch
      conv *= 39.3701;
      if (conv > 999.99) {
        index = 5; // feets
        break;
      }
      if (conv < 1) {
        index = 1; // mils
        break;
      }
      return QString("%1in").arg(RoundVariablePrecision(conv));
    case 5: // ft
      conv *= 3.280841666667;
      if (conv > 999.99) {
        index = 6; // meters
        break;
      }
      if (conv < 1) {
        index = 4; // inches
        break;
      }
      return QString("%1ft").arg(RoundVariablePrecision(conv));
    case 6: // m
      if (conv < 1) {
        index = 0; // mm
        break;
      }
      return QString("%1").arg(RoundVariablePrecision(len));
    default: // milimeters
      conv *= 1e3;
      if (conv > 999.99) {
        index = 6; // meters
        break;
      }
      if (conv < 1) {
        index = 2; // microns
        break;
      }
      return QString("%1mm").arg(RoundVariablePrecision(conv));
    }
  } while (true);
  return QString("");
}


void convert_MA_RI_to_dB(double * S_1, double * S_2, double *S_3, double *S_4, QString format)
{
  double S_dB = *S_1, S_ang =*S_2;
  double S_re = *S_3, S_im = *S_4;
  if (format == "MA"){
    S_dB = 20*log10(*S_1);
    S_ang = *S_2;
    S_re = *S_1 * std::cos(*S_2 * M_PI/180);
    S_im = *S_1 * std::sin(*S_2 * M_PI/180);
  }else{
    if (format == "RI"){
      S_dB = 20*log10(sqrt((*S_1)*(*S_1) + (*S_2)*(*S_2)));
      S_ang = atan2(*S_2, *S_1) * 180 / M_PI;
      S_re = *S_1;
      S_im = *S_2;
    } else {
      // DB format
      double r = std::pow(10, *S_1 / 10.0);
      double theta = *S_2 * M_PI / 180.0;
      S_re = r * std::cos(theta);
      S_im = r * std::sin(theta);
    }

  }
  *S_1 = S_dB;
  *S_2 = S_ang;
  *S_3 = S_re;
  *S_4 = S_im;
}

// Gets the frequency scale unit from a String lke kHz, MHz, GHz
double getFreqScale(QString frequency_unit)
{
  double freq_scale=1;
  if (frequency_unit == "kHz"){
    freq_scale = 1e-3;
  } else {
    if (frequency_unit == "MHz"){
      freq_scale = 1e-6;
    } else {
      if (frequency_unit == "GHz"){
        freq_scale = 1e-9;
      }
    }
  }
  return freq_scale;
}

int findClosestIndex(const QList<double>& list, double value)
{
  return std::min_element(list.begin(), list.end(),
                          [value](double a, double b) {
                            return std::abs(a - value) < std::abs(b - value);
                          }) - list.begin();
}


double getFreqFromText(QString freq)
{
  // Remove any whitespace from the string
  freq = freq.simplified();

         // Regular expression to match the number and unit
  QRegularExpression re("(\\d+(?:\\.\\d+)?)(\\s*)(Hz|kHz|MHz|GHz)");
  re.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
  QRegularExpressionMatch match = re.match(freq);

  if (match.hasMatch()) {
    double value = match.captured(1).toDouble();
    QString unit = match.captured(3).toLower();

           // Convert to Hz based on the unit
    if (unit == "khz") {
      return value * 1e3;
    } else if (unit == "mhz") {
      return value * 1e6;
    } else if (unit == "ghz") {
      return value * 1e9;
    } else {
      // Assume Hz if no unit or Hz is specified
      return value;
    }
  }

         // Return -1 if the input doesn't match the expected format
  return -1;
}


// Find the closest x-axis value in a series given a x value (not necesarily in the grid)
QPointF findClosestPoint(const QList<double>& xValues, const QList<double>& yValues, double targetX)
{
  if (xValues.isEmpty() || yValues.isEmpty() || xValues.size() != yValues.size()) {
    return QPointF(); // Return invalid point if lists are empty or have different sizes
  }

         // Initialize with the first point
  QPointF closestPoint(xValues.first(), yValues.first());
  double minDistance = qAbs(targetX - closestPoint.x());

         // Iterate through all points to find the closest one
  for (int i = 0; i < xValues.size(); ++i) {
    double distance = qAbs(targetX - xValues[i]);
    if (distance < minDistance) {
      minDistance = distance;
      closestPoint = QPointF(xValues[i], yValues[i]);
    }
  }

  return closestPoint;
}
