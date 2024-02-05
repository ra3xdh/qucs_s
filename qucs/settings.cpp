#include <settings.h>

settingsManager::settingsManager()
    :QSettings("qucs", "qucs_s")
{
    // qDebug() << this << " created " << organizationName() << " " << applicationName();

    m_Defaults["Foo"] = 1234;

}

settingsManager::~settingsManager()
{
    // qDebug() << this << " destroyed";
}
