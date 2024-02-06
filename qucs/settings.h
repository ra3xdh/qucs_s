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

  /** \brief Set all settings in the given group to their default value.
  */  
  void resetDefaults(const QString& group = "All");

  /** \brief Initialise all settings to their (hard coded) default value.
  */
  void initDefaults();

  /** \brief Get the default value for a given setting.
  */  
  template<class T>
  T itemDefault(const QString& key)
  {
    return value(key).value<T>();
  }

  /** \brief Store a setting.
  */  
  template<class T>
  void setItem(const QString& key, const T& value)
  {
    setValue(key, value);
  }

  /** \brief Retrieve a setting value by its key.
  */  
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
