#include <QApplication>

#include "main.h"
#include "settings.h"
#include "extsimkernels/spicecompat.h"

settingsManager::settingsManager()
    :QSettings("qucs", "qucs_s")
{
    // qDebug() << this << " created " << organizationName() << " " << applicationName();

    initAliases();
    initDefaults();
}

settingsManager::~settingsManager()
{
    // qDebug() << this << " destroyed";
}

void settingsManager::resetDefaults(const QString &group)
{
    qDebug() << "Reset settings group " << group;

    if (group == "All") {
        // Remove all settings (including those for which there is no default).
        // clear();

        // Repopulate with known defaults.
        for (auto const& item : m_Defaults) {
            qDebug() << "Resetting item " << item.first << " " << item.second;
            setValue(item.first, item.second);
        }
    }

    else {
        beginGroup(group);
        for (const QString& key : allKeys()) {
            setValue(key, m_Defaults[key]);
        }
    }
}

void settingsManager::initDefaults()
{
    m_Defaults["DefaultSimulator"] = spicecompat::simNotSpecified;
    m_Defaults["FirstRun"] = true;
    m_Defaults["font"] = QApplication::font();
    m_Defaults["appFont"] = QApplication::font();
    m_Defaults["LargeFontSize"] = static_cast<double>(16.0);
    m_Defaults["maxUndo"] = 20;
    m_Defaults["panelIconsTheme"] = qucs::autoIcons;
    m_Defaults["compIconsTheme"] = qucs::autoIcons;

#ifdef Q_OS_WIN
    m_Defaults["NgspiceExecutable"] = "ngspice_con.exe";
    m_Defaults["XyceExecutable"] = "Xyce.exe";
#else
    m_Defaults["NgspiceExecutable"] = "ngspice";
    m_Defaults["XyceExecutable"] = "/usr/local/Xyce-Release-6.8.0-OPENSOURCE/bin/Xyce";
#endif

    m_Defaults["XyceParExecutable"] = "mpirun -np %p /usr/local/Xyce-Release-6.8.0-OPENMPI-OPENSOURCE/bin/Xyce";
    m_Defaults["Nprocs"] = 4;
    m_Defaults["SpiceOpusExecutable"] = "spiceopus";
    m_Defaults["SimParameters"] = "";
    m_Defaults["GraphAntiAliasing"] = false;
    m_Defaults["TextAntiAliasing"] = false;
    m_Defaults["fullTraceName"] = false;
}

void settingsManager::initAliases()
{
    m_Aliases["IgnoreVersion"] = QStringList({"IngnoreVersion"});
}
