
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
    friend QDebug operator<<(QDebug ds, const Book &book);
    friend QTextStream &operator<<(QTextStream &ds, const Book &book);

    explicit Book();

    bool fromJson( const QJsonObject& obj );
    QJsonObject toJson() ;

    QString toString();

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
    bool reprint;
    bool read;

    bool isDigital;

    QString isbn;

    QString collana;
    QString editore;

    //support
    QString cover_image_path;

    int id;




};

#endif // BOOK_H
