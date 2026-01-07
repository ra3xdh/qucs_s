/// @file main.cpp
/// @brief Main file
/// @author Andrés Martínez Mera - andresmmera@protonmail.com
/// @date Jan 3, 2026
/// @copyright Copyright (C) 2026 Andrés Martínez Mera
/// @license GPL-3.0-or-later

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFont>
#include <QMessageBox>
#include <QScreen>
#include <QSettings>
#include <QString>
#include <QStyle>
#include <QTextStream>
#include <QTranslator>

#include "UI/qucs-s-spar-viewer.h"

/// Global structure holding application settings
struct tQucsSettings QucsSettings;

/// @brief Loads application settings from the configuration file
///
/// Reads settings from the QSettings storage using the organization name "qucs"
/// and application name "qucs_s". Loads window position (x, y coordinates),
/// font settings, and language preferences into the global QucsSettings
/// structure.
///
/// @return true if settings were loaded successfully, false otherwise
/// @see saveApplSettings(Qucs_S_SPAR_Viewer*)
bool loadSettings() {
  QSettings settings("qucs", "qucs_s");
  settings.beginGroup("QucsSparViewer");
  if (settings.contains("x")) {
    QucsSettings.x = settings.value("x").toInt();
  }
  if (settings.contains("y")) {
    QucsSettings.y = settings.value("y").toInt();
  }
  settings.endGroup();
  if (settings.contains("font")) {
    QucsSettings.font.fromString(settings.value("font").toString());
  }
  if (settings.contains("Language")) {
    QucsSettings.Language = settings.value("Language").toString();
  }

  return true;
}

/// @brief Saves application settings to the configuration file
///
/// Persists the current window position to QSettings storage for restoration
/// in future application sessions.
///
/// @param qucs Pointer to the main application window
/// @return true if settings were saved successfully, false otherwise
/// @see loadSettings()
bool saveApplSettings(Qucs_S_SPAR_Viewer *qucs) {
  QSettings settings("qucs", "qucs_s");
  settings.beginGroup("QucsSparViewer");
  settings.setValue("x", qucs->x());
  settings.setValue("y", qucs->y());
  settings.endGroup();
  return true;
}

/// @brief Main entry point for the Qucs-S S-Parameter Viewer
///
/// Initializes the Qt application, loads settings, configures localization,
/// creates the main window, and optionally opens a file or directory specified
/// as a command-line argument.
///
///
/// @param argc Number of command-line arguments
/// @param argv Array of command-line argument strings
///              argv[1] (optional): Path to a file or directory to open
///
/// @return Application exit code (0 for success)
int main(int argc, char **argv) {
  QApplication a(argc, argv);

  // apply default settings
  QucsSettings.x = 200;
  QucsSettings.y = 100;

  // is application relocated?
  char *var = getenv("QUCSDIR");
  QDir QucsDir;
  if (var != NULL) {
    QucsDir = QDir(var);
    QString QucsDirStr = QucsDir.canonicalPath();
    QucsSettings.LangDir =
        QDir::toNativeSeparators(QucsDirStr + "/share/" QUCS_NAME "/lang/");
  } else {
    QString QucsApplicationPath = QCoreApplication::applicationDirPath();
#ifdef __APPLE__
    QucsDir = QDir(QucsApplicationPath.section("/bin", 0, 0));
#else
    QucsDir = QDir(QucsApplicationPath);
    QucsDir.cdUp();
#endif
    QucsSettings.LangDir = QucsDir.canonicalPath() + "/share/qucs/lang/";
  }

  loadSettings();

  QTranslator tor(0);
  QString lang = QucsSettings.Language;
  if (lang.isEmpty()) {
    lang = QString(QLocale::system().name());
  }
  static_cast<void>(
      tor.load(QStringLiteral("qucs_") + lang, QucsSettings.LangDir));
  a.installTranslator(&tor);

  Qucs_S_SPAR_Viewer *qucs = new Qucs_S_SPAR_Viewer();

  if (argc > 1) { // File or directory path to watch
    QString path = QString(argv[1]);
    QFileInfo fileInfo(path);

    if (fileInfo.exists()) {
      if (fileInfo.isDir()) {
        // It's a directory
        qucs->addPathToWatcher(path);
      } else if (fileInfo.isFile()) {
        // It's a file
        qucs->addFile(fileInfo);
      }
    } else {
      QMessageBox::warning(qucs, "Path Error",
                           "The specified path does not exist.");
    }
  }
  qucs->raise();
  qucs->move(QucsSettings.x, QucsSettings.y); // position before "show" !!!
  qucs->show();

  QScreen *primaryScreen = QGuiApplication::screens().constFirst();

  qucs->resize(primaryScreen->availableGeometry().size() * 0.9);
  qucs->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                                        qucs->size(),
                                        primaryScreen->availableGeometry()));

  int result = a.exec();
  saveApplSettings(qucs);
  return result;
}
