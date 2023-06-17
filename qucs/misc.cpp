/***************************************************************************
                                 misc.cpp
                                ----------
    begin                : Wed Nov 11 2004
    copyright            : (C) 2014 by YodaLee
    email                : lc85301@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/*!
 * \file misc.cpp
 * \Definition of some miscellaneous function
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <cmath>
#include "misc.h"
#include "main.h"

#include <cstdio>
#include <QString>
#include <QStringList>
#include <QRegularExpression>
#include <QFileInfo>
#include <QDir>

#include <QtWidgets>


bool misc::isDarkTheme()
{
    QLabel *lbl = new QLabel("check dark");
    int text_hsv = lbl->palette().color(QPalette::WindowText).value();
    int bg_hsv = lbl->palette().color(QPalette::Window).value();
    bool is_dark_theme = text_hsv > bg_hsv;
    return is_dark_theme;
}

QString misc::getIconPath(const QString &file, int icon_type)
{
    auto icons_theme = QucsSettings.panelIconsTheme;
    switch (icon_type) {
    case qucs::panelIcons:
        icons_theme = QucsSettings.panelIconsTheme;
        break;
    case qucs::compIcons:
        icons_theme = QucsSettings.compIconsTheme;
        break;
    }

    bool loadDark = false;
    switch (icons_theme) {
    case qucs::autoIcons:
        loadDark = QucsSettings.hasDarkTheme;
        break;
    case qucs::darkIcons:
        loadDark = true;
        break;
    case qucs::lightIcons:
        loadDark = false;
        break;
    }

    QString icon_path =":bitmaps/";
    if (loadDark) {
        QString icon_path_dark = ":bitmaps/dark/";
        if (QFileInfo::exists(icon_path_dark + file))
            icon_path = icon_path_dark;
    }
    icon_path += file;
    return icon_path;
}

// #########################################################################
QString misc::complexRect(double real, double imag, int Precision)
{
  QString Text;
  if(fabs(imag) < 1e-250) Text = QString::number(real,'g',Precision);
  else {
    Text = QString::number(imag,'g',Precision);
    if(Text.at(0) == '-') {
      Text.replace(0,1,'j');
      Text = '-'+Text;
    }
    else  Text = "+j"+Text;
    Text = QString::number(real,'g',Precision) + Text;
  }
  return Text;
}

QString misc::complexDeg(double real, double imag, int Precision)
{
  QString Text;
  if(fabs(imag) < 1e-250) Text = QString::number(real,'g',Precision);
  else {
    Text  = QString::number(sqrt(real*real+imag*imag),'g',Precision) + " / ";
    Text += QString::number(180.0/pi*atan2(imag,real),'g',Precision) + QString::fromUtf8("°");
  }
  return Text;
}

QString misc::complexRad (double real, double imag, int Precision)
{
  QString Text;
  if(fabs(imag) < 1e-250) Text = QString::number(real,'g',Precision);
  else {
    Text  = QString::number(sqrt(real*real+imag*imag),'g',Precision);
    Text += " / " + QString::number(atan2(imag,real),'g',Precision) + "rad";
  }
  return Text;
}

// #########################################################################
QString misc::StringNum(double num, char form, int Precision)
{
  int a = 0;
  char *p, Buffer[512], Format[6] = "%.00g";

  if(Precision < 0) {
    Format[1]  = form;
    Format[2]  = 0;
  }
  else {
    Format[4]  = form;
    Format[2] += Precision / 10;
    Format[3] += Precision % 10;
  }
  sprintf(Buffer, Format, num);
  p = strchr(Buffer, 'e');
  if(p) {
    p++;
    if(*(p++) == '+') { a = 1; }   // remove '+' of exponent
    if(*p == '0') { a++; p++; }    // remove leading zeros of exponent
    if(a > 0)
      do {
        *(p-a) = *p;
      } while(*(p++) != 0);    // override characters not needed
  }

  return QString(Buffer);
}

// #########################################################################
QString misc::StringNiceNum(double num)
{
  char Format[6] = "%.8e";
  if(fabs(num) < 1e-250)  return QString("0");  // avoid many problems
  if(fabs(log10(fabs(num))) < 3.0)  Format[3] = 'g';

  int a = 0;
  char *p, *pe, Buffer[512];

  sprintf(Buffer, Format, num);
  p = pe = strchr(Buffer, 'e');
  if(p) {
    if(*(++p) == '+') { a = 1; }    // remove '+' of exponent
    if(*(++p) == '0') { a++; p++; } // remove leading zeros of exponent
    if(a > 0)
      do {
        *(p-a) = *p;
      } while(*(p++) != 0);  // override characters not needed

    // In 'g' format, trailing zeros are already cut off !!!
    p = strchr(Buffer, '.');
    if(p) {
      if(!pe)  pe = Buffer + strlen(Buffer);
      p = pe-1;
      while(*p == '0')   // looking for unnecessary zero characters
        if((--p) <= Buffer)  break;
      if(*p != '.')  p++;  // no digit after decimal point ?
      while( (*(p++) = *(pe++)) != 0 ) ;  // overwrite zero characters
    }
  }

  return QString(Buffer);
}

// #########################################################################
void misc::str2num(const QString& s_, double& Number, QString& Unit, double& Factor)
{
  QString str = s_.trimmed();

/*  int i=0;
  bool neg = false;
  if(str[0] == '-') {      // check sign
    neg = true;
    i++;
  }
  else if(str[0] == '+')  i++;

  double num = 0.0;
  for(;;) {
    if(str[i] >= '0')  if(str[i] <= '9') {
      num = 10.0*num + double(str[i]-'0');
    }
  }*/

  QRegularExpression Expr( QRegularExpression("[^0-9\\x2E\\x2D\\x2B]") );
  int i = str.indexOf( Expr );
  if(i >= 0)
    if((str.at(i).toLatin1() | 0x20) == 'e') {
      int j = str.indexOf( Expr , ++i);
      if(j == i)  j--;
      i = j;
    }

  Number = str.left(i).toDouble();
  Unit   = str.mid(i).trimmed();
  if(Unit.length()>0)
  {
    switch(Unit.at(0).toLatin1()) {
      case 'T': Factor = 1e12;  break;
      case 'G': Factor = 1e9;   break;
      case 'M': Factor = 1e6;   break;
      case 'k': Factor = 1e3;   break;
      case 'c': Factor = 1e-2;  break;
      case 'm': Factor = 1e-3;  break;
      case 'u': Factor = 1e-6;  break;
      case 'n': Factor = 1e-9;  break;
      case 'p': Factor = 1e-12; break;
      case 'f': Factor = 1e-15; break;
  //    case 'd':
      default:  Factor = 1.0;
    }
  }
  else
  {
    Factor = 1.0;
  }

  return;
}

// #########################################################################
QString misc::num2str(double Num)
{
  char c = 0;
  double cal = fabs(Num);
  if(cal > 1e-20) {
    cal = log10(cal) / 3.0;
    if(cal < -0.2)  cal -= 0.98;
    int Expo = int(cal);

    if(Expo >= -5) if(Expo <= 4)
      switch(Expo) {
        case -5: c = 'f'; break;
        case -4: c = 'p'; break;
        case -3: c = 'n'; break;
        case -2: c = 'u'; break;
        case -1: c = 'm'; break;
        case  1: c = 'k'; break;
        case  2: c = 'M'; break;
        case  3: c = 'G'; break;
        case  4: c = 'T'; break;
      }

    if(c)  Num /= pow(10.0, double(3*Expo));
  }

  QString Str = QString::number(Num);
  if(c)  Str += c;

  return Str;
}

// #########################################################################
void misc::convert2Unicode(QString& Text)
{
  bool ok;
  int i = 0;
  QString n;
  unsigned short ch;
  while((i=Text.indexOf("\\x", i)) >= 0) {
    n = Text.mid(i, 6);
    ch = n.mid(2).toUShort(&ok, 16);
    if(ok)  Text.replace(n, QChar(ch));
    i++;
  }
  Text.replace("\\n", "\n");
  Text.replace("\\\\", "\\");
}

// #########################################################################
void misc::convert2ASCII(QString& Text)
{
  Text.replace('\\', "\\\\");
  Text.replace('\n', "\\n");

  int i = 0;
  QChar ch;
  char Str[8];
  while(Text.size()<i) {  // convert special characters
    if(ch > QChar(0x7F)) {
      sprintf(Str, "\\x%04X", ch.unicode());
      Text.replace(ch, Str);
    }
  }
}

// #########################################################################
// Converts a path to an absolute path and resolves paths relative to the
// Qucs home directory
QString misc::properAbsFileName(const QString& Name)
{
  QString s = Name;
  QFileInfo Info(s);

  if(Info.isRelative())
  {
      // if it's a relative file, look for it relative to the
      // working directory (the qucs home directory)
      s = QucsSettings.QucsWorkDir.filePath(s);
  }
  // return the clean path
  return QDir::cleanPath(s);
}

// #########################################################################
QString misc::properFileName(const QString& Name)
{
  QFileInfo Info(Name);
  return Info.fileName();
}

// #########################################################################
// Takes a file name (with path) and replaces all special characters.
QString misc::properName(const QString& Name)
{
  QString s = Name;
  QFileInfo Info(s);
  if(Info.suffix() == "sch")
    s.chop(4);
  if(s.at(0) <= '9') if(s.at(0) >= '0')
    s = 'n' + s;
  s.replace(QRegularExpression("\\W"), "_"); // none [a-zA-Z0-9] into "_"
  s.replace("__", "_");  // '__' not allowed in VHDL
  if(s.at(0) == '_')
    s = 'n' + s;
  return s;
}

// #########################################################################
// Creates and returns delay time for VHDL entities.
bool misc::VHDL_Delay(QString& td, const QString& Name)
{
  if(strtod(td.toLatin1(), 0) != 0.0) {  // delay time property
    if(!misc::VHDL_Time(td, Name))
      return false;    // time has not VHDL format
    td = " after " + td;
    return true;
  }
  else if(isalpha(td.toLatin1()[0])) {
    td = " after " + td;
    return true;
  }
  else {
    td = "";
    return true;
  }
}

// #########################################################################
// Checks and corrects a time (number & unit) according VHDL standard.
bool misc::VHDL_Time(QString& t, const QString& Name)
{
  char *p;
  QByteArray ba = t.toLatin1();
  double Time = strtod(ba.data(), &p);
  while(*p == ' ') p++;
  for(;;) {
    if(Time >= 0.0) {
      if(strcmp(p, "fs") == 0)  break;
      if(strcmp(p, "ps") == 0)  break;
      if(strcmp(p, "ns") == 0)  break;
      if(strcmp(p, "us") == 0)  break;
      if(strcmp(p, "ms") == 0)  break;
      if(strcmp(p, "sec") == 0) break;
      if(strcmp(p, "min") == 0) break;
      if(strcmp(p, "hr") == 0)  break;
    }
    t = QString::fromUtf8("§")  + QObject::tr("Error: Wrong time format in \"%1\". Use positive number with units").arg(Name)
            + " fs, ps, ns, us, ms, sec, min, hr.\n";
    return false;
  }

  t = QString::number(Time) + " " + QString(p);  // the space is mandatory !
  return true;
}

// #########################################################################
// Returns parameters for Verilog modules.
QString misc::Verilog_Param(const QString Value)
{
  if(strtod(Value.toLatin1(), 0) != 0.0) {
    QString td = Value;
    if(!misc::Verilog_Time(td, "parameter"))
      return Value;
    else
      return td;
  }
  else
    return Value;
}

// #########################################################################
// Creates and returns delay time for Verilog modules.
bool misc::Verilog_Delay(QString& td, const QString& Name)
{
  if(strtod(td.toLatin1(), 0) != 0.0) {  // delay time property
    if(!misc::Verilog_Time(td, Name))
      return false;    // time has not Verilog format
    td = " #" + td;
    return true;
  }
  else if(isalpha(td.toLatin1()[0])) {
    td = " #" + td;
    return true;
  }
  else {
    td = "";
    return true;
  }
}

// #########################################################################
// Checks and corrects a time (number & unit) according Verilog standard.
bool misc::Verilog_Time(QString& t, const QString& Name)
{
  char *p;
  QByteArray ba = t.toLatin1();
  double Time = strtod(ba.data(), &p);
  double factor = 1.0;
  while(*p == ' ') p++;
  for(;;) {
    if(Time >= 0.0) {
      if(strcmp(p, "fs") == 0) { factor = 1e-3; break; }
      if(strcmp(p, "ps") == 0) { factor = 1;  break; }
      if(strcmp(p, "ns") == 0) { factor = 1e3;  break; }
      if(strcmp(p, "us") == 0) { factor = 1e6;  break; }
      if(strcmp(p, "ms") == 0) { factor = 1e9;  break; }
      if(strcmp(p, "sec") == 0) { factor = 1e12; break; }
      if(strcmp(p, "min") == 0) { factor = 1e12*60; break; }
      if(strcmp(p, "hr") == 0)  { factor = 1e12*60*60; break; }
    }
    t = QString::fromUtf8("§")  + QObject::tr("Error: Wrong time format in \"%1\". Use positive number with units").arg(Name)
            + " fs, ps, ns, us, ms, sec, min, hr.\n";
    return false;
  }

  t = QString::number(Time*factor);
  return true;
}

// #########################################################################
bool misc::checkVersion(QString& Line)
{
  VersionTriplet SchVersion(Line);
  if (SchVersion > QucsVersion) return false;
  return true;
}

// a small class to handle the application version string
//   loosely modeled after the standard Semantic Versioning...
VersionTriplet::VersionTriplet(const QString& version) {
  // TODO should be likely made more robust...
  if (version.isEmpty()) {
    major = minor = patch = 0;
  } else {
    QStringList vl = version.split('.');
    major = vl.at(0).toUInt();
    minor = vl.at(1).toUInt();
    patch = vl.at(2).toUInt();
  }
}

QStringList misc::parseCmdArgs(const QString &program)
{
    QStringList args;
    QString tmp;
    int quoteCount = 0;
    bool inQuote = false;
    // handle quoting. tokens can be surrounded by double quotes
    // "hello world". three consecutive double quotes represent
    // the quote character itself.
    for (int i = 0; i < program.size(); ++i) {
        if (program.at(i) == QLatin1Char('"')) {
            ++quoteCount;
            if (quoteCount == 3) {
                // third consecutive quote
                quoteCount = 0;
                tmp += program.at(i);
            }
            continue;
        }
        if (quoteCount) {
            if (quoteCount == 1)
                inQuote = !inQuote;
            quoteCount = 0;
        }
        if (!inQuote && program.at(i).isSpace()) {
            if (!tmp.isEmpty()) {
                args += tmp;
                tmp.clear();
            }
        } else {
            tmp += program.at(i);
        }
    }
    if (!tmp.isEmpty())
        args += tmp;
    return args;
}


QString misc::wildcardToRegularExpression(const QString &wc_str, const bool enableEscaping)
{
    const int wclen = wc_str.length();
    QString rx;
    int i = 0;
    bool isEscaping = false; // the previous character is '\'
    const QChar *wc = wc_str.unicode();

    while (i < wclen) {
        const QChar c = wc[i++];
        switch (c.unicode()) {
        case '\\':
            if (enableEscaping) {
                if (isEscaping) {
                    rx += QLatin1String("\\\\");
                } // we insert the \\ later if necessary
                if (i+1 == wclen) { // the end
                    rx += QLatin1String("\\\\");
                }
            } else {
                rx += QLatin1String("\\\\");
            }
            isEscaping = true;
            break;
        case '*':
            if (isEscaping) {
                rx += QLatin1String("\\*");
                isEscaping = false;
            } else {
                rx += QLatin1String(".*");
            }
            break;
        case '?':
            if (isEscaping) {
                rx += QLatin1String("\\?");
                isEscaping = false;
            } else {
                rx += QLatin1Char('.');
            }

            break;
        case '$':
        case '(':
        case ')':
        case '+':
        case '.':
        case '^':
        case '{':
        case '|':
        case '}':
            if (isEscaping) {
                isEscaping = false;
                rx += QLatin1String("\\\\");
            }
            rx += QLatin1Char('\\');
            rx += c;
            break;
         case '[':
            if (isEscaping) {
                isEscaping = false;
                rx += QLatin1String("\\[");
            } else {
                rx += c;
                if (wc[i] == QLatin1Char('^'))
                    rx += wc[i++];
                if (i < wclen) {
                    if (rx[i] == QLatin1Char(']'))
                        rx += wc[i++];
                    while (i < wclen && wc[i] != QLatin1Char(']')) {
                        if (wc[i] == QLatin1Char('\\'))
                            rx += QLatin1Char('\\');
                        rx += wc[i++];
                    }
                }
            }
             break;

        case ']':
            if(isEscaping){
                isEscaping = false;
                rx += QLatin1String("\\");
            }
            rx += c;
            break;

        default:
            if(isEscaping){
                isEscaping = false;
                rx += QLatin1String("\\\\");
            }
            rx += c;
        }
    }
    return rx;
}

bool misc::simulatorExists(const QString &exe_file)
{
    if (QFile::exists(exe_file)) return true; // absolute path

    QFileInfo inf(exe_file); // try to find exe in $PATH
    char *p = getenv("PATH");
    QStringList paths;
    bool found = false;
    if (p != nullptr) paths = QString(p).split(':');
    for (const auto &pp : paths) {
        inf.setFile(pp + QDir::separator() + exe_file);
        if (inf.exists()) {
            found = true;
            break;
        }
    }
    return found;
}

QString misc::unwrapExePath(const QString &exe_file)
{
    if (QFile::exists(exe_file)) return exe_file; // absolute path

    QFileInfo inf(exe_file); // try to find exe in $PATH
    char *p = getenv("PATH");
    QStringList paths;
    QString abs_exe_path = exe_file;
    if (p != nullptr) paths = QString(p).split(':');
    for (const auto &pp : paths) {
        inf.setFile(pp + QDir::separator() + exe_file);
        if (inf.exists()) {
            abs_exe_path = inf.canonicalFilePath();
            break;
        }
    }
    return abs_exe_path;
}


VersionTriplet::VersionTriplet(){
  major = minor = patch = 0;
}

bool VersionTriplet::operator==(const VersionTriplet& v2) {
  if (this->major != v2.major)
    return false;
  if (this->minor != v2.minor)
    return false;
  if (this->patch != v2.patch)
    return false;
  return true;
}

bool VersionTriplet::operator>(const VersionTriplet& v2) {
  if (this->major < v2.major)
    return false;
  if (this->major > v2.major)
    return true;

  if (this->minor < v2.minor)
    return false;
  if (this->minor > v2.minor)
    return true;

  if (this->patch < v2.patch)
    return false;
  if (this->patch > v2.patch)
    return true;

  return false;
}

bool VersionTriplet::operator<(const VersionTriplet& v2) {
  if (this->major > v2.major)
    return false;
  if (this->major < v2.major)
    return true;

  if (this->minor > v2.minor)
    return false;
  if (this->minor < v2.minor)
    return true;

  if (this->patch > v2.patch)
    return false;
  if (this->patch < v2.patch)
    return true;

  return false;
}

bool VersionTriplet::operator>=(const VersionTriplet& v2) {
  return !((*this) < v2);
}

bool VersionTriplet::operator<=(const VersionTriplet& v2) {
  return !((*this) > v2);
}

QString VersionTriplet::toString() {
  return QString("%1.%2.%3").arg(major).arg(minor).arg(patch);
}
