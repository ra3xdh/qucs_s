/***************************************************************************
                                 main.cpp
                                ----------
    begin                : Thu Aug 28 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <QtGui>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <locale.h>

#include <qapplication.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qtextcodec.h>
#include <qtranslator.h>
#include <qfile.h>
#include <q3textstream.h>
#include <qmessagebox.h>
#include <qregexp.h>

#include "qucs.h"
#include "main.h"
#include "node.h"

tQucsSettings QucsSettings;

QucsApp *QucsMain;  // the Qucs application itself
QString lastDir;    // to remember last directory for several dialogs


// #########################################################################
// Loads the settings file and stores the settings.
bool loadSettings()
{
    QSettings settings("qucs","qucs");

    if(settings.contains("x"))QucsSettings.x=settings.value("x").toInt();
    if(settings.contains("y"))QucsSettings.y=settings.value("y").toInt();
    if(settings.contains("dx"))QucsSettings.dx=settings.value("dx").toInt();
    if(settings.contains("dy"))QucsSettings.dy=settings.value("dy").toInt();
    if(settings.contains("font"))QucsSettings.font.fromString(settings.value("font").toString());
    if(settings.contains("largeFontSize"))QucsSettings.largeFontSize=settings.value("largeFontSize").toDouble();
    if(settings.contains("maxUndo"))QucsSettings.maxUndo=settings.value("maxUndo").toInt();
    if(settings.contains("NodeWiring"))QucsSettings.NodeWiring=settings.value("NodeWiring").toInt();
    if(settings.contains("BGColor"))QucsSettings.BGColor.setNamedColor(settings.value("BGColor").toString());
    if(settings.contains("Editor"))QucsSettings.Editor=settings.value("Editor").toString();
    if(settings.contains("FileTypes"))QucsSettings.FileTypes=settings.value("FileTypes").toStringList();
    if(settings.contains("Language"))QucsSettings.Language=settings.value("Language").toString();
    if(settings.contains("Comment"))QucsSettings.Comment.setNamedColor(settings.value("Comment").toString());
    if(settings.contains("String"))QucsSettings.String.setNamedColor(settings.value("String").toString());
    if(settings.contains("Integer"))QucsSettings.Integer.setNamedColor(settings.value("Integer").toString());
    if(settings.contains("Real"))QucsSettings.Real.setNamedColor(settings.value("Real").toString());
    if(settings.contains("Character"))QucsSettings.Character.setNamedColor(settings.value("Character").toString());
    if(settings.contains("Type"))QucsSettings.Type.setNamedColor(settings.value("Type").toString());
    if(settings.contains("Attribute"))QucsSettings.Attribute.setNamedColor(settings.value("Attribute").toString());
    if(settings.contains("Directive"))QucsSettings.Directive.setNamedColor(settings.value("Directive").toString());
    if(settings.contains("Task"))QucsSettings.Comment.setNamedColor(settings.value("Task").toString());

    if(settings.contains("Editor"))QucsSettings.Editor = settings.value("Editor").toString();
    //if(settings.contains("BinDir"))QucsSettings.BinDir = settings.value("BinDir").toString();
    //if(settings.contains("BitmapDir"))QucsSettings.BitmapDir = settings.value("BitmapDir").toString();
    //if(settings.contains("LangDir"))QucsSettings.LangDir = settings.value("LangDir").toString();
    //if(settings.contains("LibDir"))QucsSettings.LibDir = settings.value("LibDir").toString();
    //if(settings.contains("AscoDir"))QucsSettings.AscoDir = settings.value("AscoDir").toString();
    //if(settings.contains("OctaveDir"))QucsSettings.OctaveDir = settings.value("OctaveDir").toString();
    //if(settings.contains("ExamplesDir"))QucsSettings.ExamplesDir = settings.value("ExamplesDir").toString();
    //if(settings.contains("DocDir"))QucsSettings.DocDir = settings.value("DocDir").toString();
    if(settings.contains("OctaveBinDir"))QucsSettings.OctaveBinDir.setPath(settings.value("OctaveBinDir").toString());
    if(settings.contains("QucsHomeDir"))QucsSettings.QucsHomeDir.setPath(settings.value("QucsHomeDir").toString());
    QucsSettings.QucsWorkDir = QucsSettings.QucsHomeDir;
  return true;
}

// #########################################################################
// Saves the settings in the settings file.
bool saveApplSettings(QucsApp *qucs)
{
    QSettings settings ("qucs","qucs");

    settings.setValue("x", QucsSettings.x);
    settings.setValue("y", QucsSettings.y);
    settings.setValue("dx", QucsSettings.dx);
    settings.setValue("dy", QucsSettings.dy);
    settings.setValue("font", QucsSettings.font.toString());
    settings.setValue("largeFontSize", QucsSettings.largeFontSize);
    settings.setValue("maxUndo", QucsSettings.maxUndo);
    settings.setValue("NodeWiring", QucsSettings.NodeWiring);
    settings.setValue("BGColor", QucsSettings.BGColor.name());
    settings.setValue("Editor", QucsSettings.Editor);
    settings.setValue("FileTypes", QucsSettings.FileTypes);
    settings.setValue("Language", QucsSettings.Language);
    settings.setValue("Comment", QucsSettings.Comment.name());
    settings.setValue("String", QucsSettings.String.name());
    settings.setValue("Integer", QucsSettings.Integer.name());
    settings.setValue("Real", QucsSettings.Real.name());
    settings.setValue("Character", QucsSettings.Character.name());
    settings.setValue("Type", QucsSettings.Type.name());
    settings.setValue("Attribute", QucsSettings.Attribute.name());
    settings.setValue("Directive", QucsSettings.Directive.name());
    settings.setValue("Task", QucsSettings.Comment.name());
    settings.setValue("Editor", QucsSettings.Editor);
    //settings.setValue("BinDir", QucsSettings.BinDir);
    //settings.setValue("BitmapDir", QucsSettings.BitmapDir);
    //settings.setValue("LangDir", QucsSettings.LangDir);
    //settings.setValue("LibDir", QucsSettings.LibDir);
    //settings.setValue("AscoDir", QucsSettings.AscoDir);
    //settings.setValue("OctaveDir", QucsSettings.OctaveDir);
    //settings.setValue("ExamplesDir", QucsSettings.ExamplesDir);
    //settings.setValue("DocDir", QucsSettings.DocDir);
    settings.setValue("OctaveBinDir", QucsSettings.OctaveBinDir.canonicalPath());
    settings.setValue("QucsHomeDir", QucsSettings.QucsHomeDir.canonicalPath());
  return true;

}

// #########################################################################
// #########################################################################
// ##########                                                     ##########
// ##########            MOVE SOMEWHERE ELSE START                ##########
// ##########                                                     ##########
// #########################################################################



QString complexRect(double real, double imag, int Precision)
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

QString complexDeg(double real, double imag, int Precision)
{
  QString Text;
  if(fabs(imag) < 1e-250) Text = QString::number(real,'g',Precision);
  else {
    Text  = QString::number(sqrt(real*real+imag*imag),'g',Precision) + " / ";
    Text += QString::number(180.0/M_PI*atan2(imag,real),'g',Precision) + QString::fromUtf8("°");
  }
  return Text;
}

QString complexRad (double real, double imag, int Precision)
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
QString StringNum(double num, char form, int Precision)
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
QString StringNiceNum(double num)
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
      while(*p == '0')   // looking for unneccessary zero characters
        if((--p) <= Buffer)  break;
      if(*p != '.')  p++;  // no digit after decimal point ?
      while( (*(p++) = *(pe++)) != 0 ) ;  // overwrite zero characters
    }
  }

  return QString(Buffer);
}

// #########################################################################
void str2num(const QString& s_, double& Number, QString& Unit, double& Factor)
{
  QString str = s_.stripWhiteSpace();

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

  QRegExp Expr( QRegExp("[^0-9\\x2E\\x2D\\x2B]") );
  int i = str.find( Expr );
  if(i >= 0)
    if((str.at(i).latin1() | 0x20) == 'e') {
      int j = str.find( Expr , ++i);
      if(j == i)  j--;
      i = j;
    }

  Number = str.left(i).toDouble();
  Unit   = str.mid(i).stripWhiteSpace();
  if(Unit.length()>0)
  {
    switch(Unit.at(0).latin1()) {
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
QString num2str(double Num)
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
void convert2Unicode(QString& Text)
{
  bool ok;
  int i = 0;
  QString n;
  unsigned short ch;
  while((i=Text.find("\\x", i)) >= 0) {
    n = Text.mid(i, 6);
    ch = n.mid(2).toUShort(&ok, 16);
    if(ok)  Text.replace(n, QChar(ch));
    i++;
  }
  Text.replace("\\n", "\n");
  Text.replace("\\\\", "\\");
}

// #########################################################################
void convert2ASCII(QString& Text)
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
QString properAbsFileName(const QString& Name)
{
  QString s = Name;
  QFileInfo Info(s);
  if(Info.isRelative()) s = QucsSettings.QucsWorkDir.filePath(s);
  return QDir::cleanDirPath(s);
}

// #########################################################################
QString properFileName(const QString& Name)
{
  QFileInfo Info(Name);
  return Info.fileName();
}

// #########################################################################
// Takes a file name (with path) and replaces all special characters.
QString properName(const QString& Name)
{
  QString s = Name;
  QFileInfo Info(s);
  if(Info.extension() == "sch")
    s = s.left(s.length()-4);
  if(s.at(0) <= '9') if(s.at(0) >= '0')
    s = 'n' + s;
  s.replace(QRegExp("\\W"), "_"); // none [a-zA-Z0-9] into "_"
  s.replace("__", "_");  // '__' not allowed in VHDL
  if(s.at(0) == '_')
    s = 'n' + s;
  return s;
}

// #########################################################################
// Creates and returns delay time for VHDL entities.
bool VHDL_Delay(QString& td, const QString& Name)
{
  if(strtod(td.latin1(), 0) != 0.0) {  // delay time property
    if(!VHDL_Time(td, Name))
      return false;    // time has not VHDL format
    td = " after " + td;
    return true;
  }
  else if(isalpha(td.latin1()[0])) {
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
bool VHDL_Time(QString& t, const QString& Name)
{
  char *p;
  double Time = strtod(t.latin1(), &p);
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
QString Verilog_Param(const QString Value)
{
  if(strtod(Value.latin1(), 0) != 0.0) {
    QString td = Value;
    if(!Verilog_Time(td, "parameter"))
      return Value;
    else
      return td;
  }
  else
    return Value;
}

// #########################################################################
// Creates and returns delay time for Verilog modules.
bool Verilog_Delay(QString& td, const QString& Name)
{
  if(strtod(td.latin1(), 0) != 0.0) {  // delay time property
    if(!Verilog_Time(td, Name))
      return false;    // time has not Verilog format
    td = " #" + td;
    return true;
  }
  else if(isalpha(td.latin1()[0])) {
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
bool Verilog_Time(QString& t, const QString& Name)
{
  char *p;
  double Time = strtod(t.latin1(), &p);
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
bool checkVersion(QString& Line)
{
  QStringList sl = QStringList::split('.',PACKAGE_VERSION);
  QStringList ll = QStringList::split('.',Line);
  if (ll.count() != 3 || sl.count() != 3)
    return false;
  int sv = (int)sl.at(1).toLongLong()*10000+sl.at(2).toLongLong()*100;
  int lv = (int)ll.at(1).toLongLong()*10000+ll.at(2).toLongLong()*100;
  if(lv > sv) // wrong version number ? (only backward compatible)
    return false;
  return true;
}
// #########################################################################
// ##########                                                     ##########
// ##########            MOVE SOMEWHERE ELSE END                  ##########
// ##########                                                     ##########
// #########################################################################




// #########################################################################
// ##########                                                     ##########
// ##########                  Program Start                      ##########
// ##########                                                     ##########
// #########################################################################
int main(int argc, char *argv[])
{
  // apply default settings
  QucsSettings.font = QFont("Helvetica", 12);
  QucsSettings.largeFontSize = 16.0;
  QucsSettings.maxUndo = 20;
  QucsSettings.NodeWiring = 0;

  // initially center the application
  QApplication a(argc, argv);
  QDesktopWidget *d = a.desktop();
  int w = d->width();
  int h = d->height();
  QucsSettings.x = w/8;
  QucsSettings.y = h/8;
  QucsSettings.dx = w*3/4;
  QucsSettings.dy = h*3/4;

  // check for relocation env variable
  char* var = getenv("QUCSDIR");
  QDir QucsDir;
  if (var!= NULL)
  {
      QucsDir = QDir(var);
  }
  else
  {
     QString QucsApplicationPath = QCoreApplication::applicationDirPath();
     QucsDir = QDir(QucsApplicationPath.replace("/bin",""));
  }

  QucsSettings.BinDir = QucsDir.canonicalPath() + "/bin/";
  QucsSettings.BitmapDir = QucsDir.canonicalPath() + "/share/qucs/bitmaps/";
  QucsSettings.LangDir =QucsDir.canonicalPath() + "/share/qucs/lang/";
  QucsSettings.LibDir =QucsDir.canonicalPath() + "/share/qucs/library/";
  QucsSettings.OctaveDir =QucsDir.canonicalPath() + "/share/qucs/octave/";
  QucsSettings.ExamplesDir = QucsDir.canonicalPath() + "/share/qucs/docs/examples/";
  QucsSettings.DocDir = QucsDir.canonicalPath() + "/share/qucs/docs/";

  QucsSettings.Editor = QucsSettings.BinDir + "qucsedit";
  QucsSettings.QucsHomeDir.setPath(QDir::homeDirPath()+QDir::convertSeparators ("/.qucs"));
  QucsSettings.QucsWorkDir.setPath(QucsSettings.QucsHomeDir.canonicalPath());

  var = getenv("OCTAVEBINDIR");
  if(var != NULL)
  {
      QucsSettings.OctaveBinDir.setPath(var);
  }
  else
  {
#ifdef __MINGW32__
      QucsSettings.OctaveBinDir.setPath("C:/Software/Octave-3.6.4/bin/");
#else
      QFile octaveExec("/usr/bin/octave");
      if(octaveExec.exists())QucsSettings.OctaveBinDir.setPath("/usr/bin/");
      QFile octaveExec1("/usr/local/bin/octave");
      if(octaveExec1.exists()) QucsSettings.OctaveBinDir.setPath("/usr/local/bin/");
#endif
  }
  loadSettings();

  if(!QucsSettings.BGColor.isValid())
    QucsSettings.BGColor.setRgb(255, 250, 225);

  // syntax highlighting
  if(!QucsSettings.Comment.isValid())
    QucsSettings.Comment = Qt::gray;
  if(!QucsSettings.String.isValid())
    QucsSettings.String = Qt::red;
  if(!QucsSettings.Integer.isValid())
    QucsSettings.Integer = Qt::blue;
  if(!QucsSettings.Real.isValid())
    QucsSettings.Real = Qt::darkMagenta;
  if(!QucsSettings.Character.isValid())
    QucsSettings.Character = Qt::magenta;
  if(!QucsSettings.Type.isValid())
    QucsSettings.Type = Qt::darkRed;
  if(!QucsSettings.Attribute.isValid())
    QucsSettings.Attribute = Qt::darkCyan;
  if(!QucsSettings.Directive.isValid())
    QucsSettings.Directive = Qt::darkCyan;
  if(!QucsSettings.Task.isValid())
    QucsSettings.Task = Qt::darkRed;

    QucsSettings.AscoDir = QucsSettings.BinDir;

  a.setFont(QucsSettings.font);

  QTranslator tor( 0 );
  QString lang = QucsSettings.Language;
  if(lang.isEmpty())
    lang = QTextCodec::locale();
  tor.load( QString("qucs_") + lang, QucsSettings.LangDir);
  a.installTranslator( &tor );

  // This seems to be neccessary on a few system to make strtod()
  // work properly !???!
  setlocale (LC_NUMERIC, "C");

  QucsMain = new QucsApp();
  a.setMainWidget(QucsMain);
  QucsMain->show();
  int result = a.exec();
  //saveApplSettings(QucsMain);
  return result;
}
