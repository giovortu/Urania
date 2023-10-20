/*
 * JSONSettings.cpp
 *
 *  Created on: Dec 5, 2012
 *      Author: Giovanni Ortu
 */

#include "JSONSettings.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>
#include <QRect>
#include <QDebug>
#include <QJsonArray>

void JSONSettings::parseString(QSettings::SettingsMap &map, const QString &key, const QJsonValue & value )
{
    if ( key.contains("/"))
    {
        int index = key.indexOf('/');
        QString k = key.left(index);
        QString rest = key.mid(index+1);
        if ( !map.contains(k) )
        {
            QSettings::SettingsMap *m = new QSettings::SettingsMap;
            parseString( *m, rest, value );
            map[k] = *m;
        }
        else
        {
            QSettings::SettingsMap m = map[k].toMap();
            parseString( m, rest, value );
            map[k] = m;
        }

    }
    else
    {
        map[key] = value;
    }
}

void JSONSettings::parseObject(QSettings::SettingsMap &map, QJsonObject obj)
{
    QStringList keys = obj.keys();

    for( int i = 0; i < keys.size(); i++ )
    {
        const QString &key = keys.at(i);
        parseString( map, key, obj[key] );
    }
}

void JSONSettings::compactStringObject(QSettings::SettingsMap &map, const QString &key, const QJsonValue & val )
{
    if (   val.isObject() )
    {
        QJsonObject obj = val.toObject();
        QStringList keys = obj.keys();

        map[key] = obj;

        for( int i = 0; i < keys.size(); i++ )
        {
            compactStringObject( map, key+"/"+keys[i], obj[ keys[i] ]);
        }
    }
    else
    if (   val.isArray() )
    {
        map[key] = val.toArray();
    }
    else
    {
        map[key] = val.toVariant();
    }
}

void JSONSettings::compactObject(QSettings::SettingsMap &map, QJsonObject obj)
{
    QStringList keys = obj.keys();

    for( int i = 0; i < keys.size(); i++ )
    {
        const QString &key = keys.at(i);
        QJsonValue val = obj[key];
        compactStringObject( map, key, val );
    }
}


bool JSONSettings::readFile(QIODevice &device, QSettings::SettingsMap &map)
{
    bool ret = false;
    QJsonParseError error;

    QJsonDocument doc = QJsonDocument::fromJson(device.readAll() , &error);

    if( error.error == QJsonParseError::NoError )
    {
        QJsonObject obj = doc.object();

        compactObject(map, obj);
    }

   // map = QJsonDocument::fromJson(device.readAll() , &error).toVariant().toMap();

    if( error.error == QJsonParseError::NoError )
    {
        ret = true;
    }

    return ret;
}

bool JSONSettings::writeFile(QIODevice &device, const QSettings::SettingsMap &map)
{
    bool ret = false;


    QSettings::SettingsMap map2;

    QJsonDocument jsonDocument = QJsonDocument::fromVariant(QVariant::fromValue(map));

    QJsonObject obj = jsonDocument.object();

    parseObject(map2, obj);


    jsonDocument = QJsonDocument::fromVariant(QVariant::fromValue(map2));


    if ( device.write(jsonDocument.toJson()) != -1 )
        ret = true;

    return ret;
}

QString JSONSettings::extension()
{
    return "json";
}
