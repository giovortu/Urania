
#ifndef BOOK_H
#define BOOK_H


#include <QObject>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>



class Index
{

public:
    explicit Index();


    bool fromJson( const QJsonObject& obj );
    QJsonObject toJson();

    QString title;
    QString author;



};


class Book
{

public:
    explicit Book();

    bool fromJson( const QJsonObject& obj );
    QJsonObject toJson();

    bool isValid();

    bool fromHTML( const QString &path );

    int number;
    QString title_ita;
    QString title_orig;
    QString author;
    QDate date_pub;
    QString cover_author;
    QByteArray cover_image;

    QString synopsis;
    QByteArray synopsis_image;


    QList<Index> index;

    bool owned;

    int stars;

    QString comment;


};

#endif // BOOK_H
