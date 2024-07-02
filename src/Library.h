
#ifndef LIBRARY_H
#define LIBRARY_H


#include <QObject>
#include "DbManager.h"
#include "Book.h"


class Library : public QObject
{
    Q_OBJECT
public:
    explicit Library(const QString &database, QObject *parent = nullptr);


    QString searchBooks( const QString &text, const QString & type, QList<Book> &books );
    bool getBook(int number, Book &book);
    int getBookById(int id, Book &book);

    int getBooksCount( bool global = false  );
    int getOwnedCount(bool global = false  );
    int getReadCount(bool global = false );
    int getDigitalCount(bool global = false );

    QStringList getCollane();
    QStringList getEditors();

    QList<Book> getOwnedBooks();
    QList<Book> getReadBooks();


signals:

public slots:

    void setCollana( const QString & collana );

    bool populateDatabase( const QString & path, const QString &basename="urania" );

    bool addBook(  Book &book );
    bool updateBookOwned( int number, bool owned );
    bool updateBookRead(int number, bool read);
    bool updateBookComment( int number, const QString &comment );
    bool updateBookStars( int number, int stars );
    bool updateBookIsDigital(int id, bool digital);

protected:
    DbManager *m_books;

};

#endif // LIBRARY_H
