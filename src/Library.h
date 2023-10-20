
#ifndef LIBRARY_H
#define LIBRARY_H


#include <QObject>
#include "DbManager.h"
#include "Book.h"


class Library : public QObject
{
    Q_OBJECT
public:
    explicit Library(QObject *parent = nullptr);


    QString searchBooks( const QString &text, int type, QList<Book> &books );
    bool getBook(int number, Book &book);

    int getBookCount();
    int getOwnedCount();
    int getReadCount();

    QList<Book> getOwnedBooks();
    QList<Book> getReadBooks();


signals:

public slots:

    bool populateDatabase( const QString & path );

    bool addBook( const Book &book );
    bool updateBookOwned( int number, bool owned );
    bool updateBookRead(int number, bool read);
    bool updateBookComment( int number, const QString &comment );
    bool updateBookStars( int number, int stars );

protected:
    DbManager *m_books;

};

#endif // LIBRARY_H
