
#ifndef DBMANAGER_H
#define DBMANAGER_H


#include <QObject>
#include <QSqlDatabase>
#include "Book.h"


class DbManager : public QObject
{
    Q_OBJECT
public:
    explicit DbManager(const QString &path, QObject *parent = nullptr);

    bool getBook(int number, Book &book);
    int getBookCount();


public slots:

    bool createTables();

    bool addBook( const Book &book );
    bool updateBookOwned( int number, bool owned );
    bool updateBookComment( int number, const QString &comment );
    bool updateBookStars( int number, int stars );

    bool addIndex( int number, const Index &index );




signals:

private:
    QSqlDatabase m_db;

    int m_currentBookCount = -1;

};

#endif // DBMANAGER_H
