/****************************************************************************
**     Qucs Attenuator Synthesis
**     main.cpp
**
**
**
**
**
**
**
*****************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>

#include <QApplication>
#include <QString>
#include <QTranslator>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDir>
#include <QFont>
#include <QSettings>
#include <QScreen>
#include <QStyle>

#include "qucs-s-spar-viewer.h"



struct tQucsSettings QucsSettings;

// #########################################################################
// Loads the settings file and stores the settings.
bool loadSettings()
{
    QSettings settings("qucs","qucs_s");
    settings.beginGroup("QucsAttenuator");
    if(settings.contains("x"))QucsSettings.x=settings.value("x").toInt();
    if(settings.contains("y"))QucsSettings.y=settings.value("y").toInt();
    settings.endGroup();
    if(settings.contains("font"))QucsSettings.font.fromString(settings.value("font").toString());
    if(settings.contains("Language"))QucsSettings.Language=settings.value("Language").toString();

  return true;
}


// #########################################################################
// Saves the settings in the settings file.
bool saveApplSettings(Qucs_S_SPAR_Viewer *qucs)
{
    QSettings settings ("qucs","qucs_s");
    settings.beginGroup("QucsAttenuator");
    settings.setValue("x", qucs->x());
    settings.setValue("y", qucs->y());
    settings.endGroup();
  return true;

}



int main( int argc, char ** argv )
{
  QApplication a( argc, argv );

  // apply default settings
  QucsSettings.x = 200;
  QucsSettings.y = 100;

  // is application relocated?
  char * var = getenv ("QUCSDIR");
  QDir QucsDir;
  if (var != NULL) {
    QucsDir = QDir (var);
    QString QucsDirStr = QucsDir.canonicalPath ();
    QucsSettings.LangDir =
      QDir::toNativeSeparators (QucsDirStr + "/share/" QUCS_NAME "/lang/");
  } else {
    QString QucsApplicationPath = QCoreApplication::applicationDirPath();
#ifdef __APPLE__
    QucsDir = QDir(QucsApplicationPath.section("/bin",0,0));
#else
    QucsDir = QDir(QucsApplicationPath);
    QucsDir.cdUp();
#endif
    QucsSettings.LangDir = QucsDir.canonicalPath() + "/share/qucs/lang/";
  }

  loadSettings();


  QTranslator tor( 0 );
  QString lang = QucsSettings.Language;
  if(lang.isEmpty())
    lang = QString(QLocale::system().name());
  tor.load( QString("qucs_") + lang, QucsSettings.LangDir);
  a.installTranslator( &tor );

  Qucs_S_SPAR_Viewer *qucs = new Qucs_S_SPAR_Viewer();
  //a.setMainWidget(qucs);
  qucs->raise();
  qucs->move(QucsSettings.x, QucsSettings.y);  // position before "show" !!!
  qucs->show();

  QScreen* primaryScreen = QGuiApplication::screens().constFirst();

  qucs->resize(primaryScreen->availableGeometry().size() * 0.9);
  qucs->setGeometry(
      QStyle::alignedRect(
          Qt::LeftToRight,
          Qt::AlignCenter,
          qucs->size(),
          primaryScreen->availableGeometry()
      )
  );



  int result = a.exec();
  saveApplSettings(qucs);
  return result;
}
