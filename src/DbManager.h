
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

    bool getBook(int number, Book &book, bool global = false);
    int getBookById(int id, Book &book );
    int getBooksCount();

    int getOwnedCount();

    int getReadCount();

    QList<Book> getOwnedBooks();
    QList<Book> getReadBooks();

    QStringList getCollane();
    QStringList getEditors();


    void setCollana( const QString & collana );

    QString searchBooks(const QString &text,const QString & type, QList<Book> &books);

public slots:

    bool createTables();

    bool addBook(  Book &book );
    bool updateBookOwned( int number, bool owned );
    bool updateBookRead(int number, bool owned);
    bool updateBookComment( int number, const QString &comment );
    bool updateBookStars( int number, int stars );

    bool addIndex( int number, const Index &index );

    //int getBookNumberFromIndex( int index );



signals:

private:

    Book bookFromQuery( QSqlQuery &query );

    QString typeToField( int type );

    QSqlDatabase m_db;

    int m_currentBookCount = -1;

    QString m_collana;

};

#endif // DBMANAGER_H
