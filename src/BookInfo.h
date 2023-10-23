#ifndef BOOKINFO_H
#define BOOKINFO_H

#include <QObject>
#include <QJsonArray>

class QNetworkAccessManager;

class BookInfo : public QObject
{
    Q_OBJECT

public:
    explicit BookInfo(QObject *parent = nullptr);

public slots:
    void searchBookInfo(const QStringList &items );
    void searchBookInfo(const QString &title);

signals:
    void isbnFound(const QJsonObject &isbns);
    void bookNotFound();

private:
    QNetworkAccessManager *networkManager;
};

#endif
