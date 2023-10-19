
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

    bool getBook(int number, Book &book);
    int getBookCount();

signals:

public slots:

    bool populateDatabase( const QString & path );

    bool addBook( const Book &book );
    bool updateBookOwned( int number, bool owned );
    bool updateBookComment( int number, const QString &comment );
    bool updateBookStars( int number, int stars );

protected:
    DbManager *m_books;

};

#endif // LIBRARY_H
