
#ifndef SETTINGSMANAGER_H_
#define SETTINGSMANAGER_H_

#include <QSettings>

/**
 * @brief The SettingsManager class
 */
class SettingsManager : public QObject
{
  Q_OBJECT
public:
    SettingsManager(QObject *parent = nullptr);
    virtual ~SettingsManager();


    QSettings *settings() const;

    void sync();



protected:

    QSettings *m_settings;
    QString m_settingsPath;

};


#endif //SETTINGSMANAGER_H_
