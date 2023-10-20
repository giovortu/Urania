
#include "Library.h"
#include <QDir>
#include <QDirIterator>
#include <QThread>

Library::Library(QObject *parent)
    : QObject{parent}
{
    m_books = new DbManager( "urania.db" , this);

    m_books->createTables();

   // populateDatabase( "C:/Users/giovortu/Downloads/Urania/www.mondourania.com/urania/" );

}

bool Library::getBook(int number, Book &book)
{
    return m_books->getBook( number, book );

}

int Library::getBookCount()
{
    return m_books->getBookCount();
}

QString Library::searchBooks(const QString &text, int type, QList<Book> &books)
{
    return m_books->searchBooks( text, type, books );
}

int Library::getOwnedCount()
{
    return m_books->getOwnedCount();
}

int Library::getReadCount()
{
    return m_books->getReadCount();
}

QList<Book> Library::getOwnedBooks()
{
    return m_books->getOwnedBooks();

}

QList<Book> Library::getReadBooks()
{
    return m_books->getReadBooks();

}

bool Library::populateDatabase(const QString &path)
{

    QDirIterator it(path, QStringList(), QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags); // only top dir
    while (it.hasNext())
    {
        QString path = it.next();

        QFileInfo fi(path);
        QString name = fi.fileName();
        QString subpath=fi.absoluteFilePath();

        name = name.replace("u", "");
        QStringList range = name.split("-");

        int start = range.at(0).toInt();
        int end = range.at(1).toInt();

        for ( int i = start; i <= end; i++ )
        {
            QString name = QString( "%1/urania%2.htm").arg(subpath) .arg( i );

            qWarning() << name;

            if ( QFile::exists( name ) == false )
                continue;

            Book book;
            book.fromHTML( name );
            if ( book.isValid() )
            {
                 m_books->addBook( book );
            }
            else
            {
                qWarning() << "Invalid book " << name;
            }


        }



    }
    return true;
}

bool Library::addBook(const Book &book)
{
    return m_books->addBook( book );
}

bool Library::updateBookOwned(int number, bool owned)
{
   return m_books->updateBookOwned( number, owned );

}

bool Library::updateBookRead(int number, bool read)
{
   return m_books->updateBookRead( number, read );

}

bool Library::updateBookComment(int number, const QString &comment)
{
    return m_books->updateBookComment( number, comment );

}

bool Library::updateBookStars(int number, int stars)
{
    return m_books->updateBookStars( number, stars );

}

