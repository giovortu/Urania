#include "JSONSettings.h"
#include "SettingsManager.h"
#include <QDebug>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>


const QSettings::Format format = QSettings::registerFormat( JSONSettings::extension(),
                                                            JSONSettings::readFile,
                                                            JSONSettings::writeFile);

SettingsManager::SettingsManager(QObject *parent ): QObject( parent )
{
    QString l_settingsPath = QCoreApplication::applicationDirPath() + "/settings/";
    QDir dir(l_settingsPath);
    dir.mkpath(l_settingsPath);

    QSettings::setPath(format, QSettings::UserScope , l_settingsPath);
    QSettings::setDefaultFormat(format);

    m_settings = new QSettings(format, QSettings::UserScope, "urania","config");

}

SettingsManager::~SettingsManager()
{


}

QSettings *SettingsManager::settings() const
{
    return m_settings;
}

void SettingsManager::sync()
{
    m_settings->sync();
}


