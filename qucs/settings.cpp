#include <settings.h>
#include <extsimkernels/spicecompat.h>

settingsManager::settingsManager()
    :QSettings("qucs", "qucs_s")
{
    // qDebug() << this << " created " << organizationName() << " " << applicationName();
    // m_Defaults["Foo"] = 1234;
    
    m_Defaults["DefaultSimulator"] = spicecompat::simNotSpecified;
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
