/*
 * JSONSettings.h
 *
 *  Created on: Dec 5, 2012
 *      Author: Giovanni Ortu
 */

#ifndef XMLSETTINGS_H_
#define XMLSETTINGS_H_

#include <QSettings>
#include <QIODevice>

/**
 * @brief The JSONSettings class
 * Qt settings framework JSON plugin
 */
class JSONSettings
{
public:

    /**
     * @brief readFile
     * @param device
     * @param map
     * @return
     */
    static bool readFile(QIODevice &device, QSettings::SettingsMap &map);

    /**
     * @brief writeFile
     * @param device
     * @param map
     * @return
     */
    static bool writeFile(QIODevice &device, const QSettings::SettingsMap &map);

    /**
     * @brief extension
     * @return
     */
    static QString extension();

    /**
     * @brief parseObject
     * @param map
     * @param obj
     */
    static void parseObject(QSettings::SettingsMap &map, QJsonObject obj);

    /**
     * @brief parseString
     * @param map
     * @param key
     * @param value
     */
    static void parseString(QSettings::SettingsMap &map, const QString &key, const QJsonValue &value);

    /**
     * @brief compactObject
     * @param map
     * @param obj
     */
    static void compactObject(QSettings::SettingsMap &map, QJsonObject obj);

    /**
     * @brief compactStringObject
     * @param map
     * @param key
     * @param value
     */
    static void compactStringObject(QSettings::SettingsMap &map, const QString &key, const QJsonValue &value);
};

#endif /* XMLSETTINGS_H_ */
