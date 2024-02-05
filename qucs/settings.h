#include <map>
#include <QDebug>
#include <QSettings>
#include <qucs.h>

class settingsManager : public QSettings
{ 
    // Q_OBJECT

// TODO: Make constructors private / protected so this class can only be
// instantiated by QucsSingleton.
public:
  explicit settingsManager();
  ~settingsManager();

  template<class T>
  T item(const QString& key)
  {
    if (contains(key)) {
        return value(key).value<T>();
    }
    
    else {
        return m_Defaults[key].value<T>();
    }
  }

private:
  std::map<QString, QVariant> m_Defaults;
};

typedef QucsSingleton<settingsManager> _settings;
