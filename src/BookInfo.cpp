#include "BookInfo.h"
#include <QtNetwork>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

BookInfo::BookInfo(QObject *parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);
}

void BookInfo::searchBookInfo(const QStringList &items)
{
    QString search = items.join("+");

    searchBookInfo( search );
}

void BookInfo::searchBookInfo(const QString &search)
{
    QString apiUrl = "https://www.googleapis.com/books/v1/volumes?q=" + search;


    QUrl url(apiUrl);

    auto manager = new QNetworkAccessManager(this);

    // Send an HTTP GET request to the URL
    manager->get(QNetworkRequest(url));

    // Connect a slot to handle the reply when it is ready
    QObject::connect(manager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply)
    {

        if (reply->error() == QNetworkReply::NoError)
        {
            QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->readAll());
            QJsonObject root = jsonResponse.object();
            QJsonArray items = root["items"].toArray();

            if (items.isEmpty())
            {
                emit bookNotFound();
            }
            else
            {
                QJsonObject volumeInfo = items.first().toObject()["volumeInfo"].toObject();
                //QString isbn = volumeInfo["industryIdentifiers"].toArray().first().toObject()["identifier"].toString();

                emit isbnFound( items.first().toObject() );
            }
        }
        else
        {
            emit bookNotFound();
        }

        manager->deleteLater();
    });
}
