
#include "Library.h"
#include <QDir>
#include <QDirIterator>
#include <QThread>

Library::Library(const QString &database,QObject *parent)
    : QObject{parent}
{
    m_books = new DbManager( database , this);

    m_books->createTables();

}

void Library::reopen()
{
    m_books->reopen();
}

void Library::open(const QString &newDB)
{
    m_books->open( newDB );
}

void Library::close()
{
    m_books->close();
}

bool Library::getBook(int number, Book &book)
{
    return m_books->getBook( number, book );

}

int Library::getBookById(int id, Book &book)
{
    return m_books->getBookById( id, book );
}

int Library::getBooksCount(bool global )
{
    return m_books->getBooksCount( global );
}

QString Library::searchBooks(const QString &text, const QString & type, QList<Book> &books)
{
    return m_books->searchBooks( text, type, books );
}

int Library::getOwnedCount(bool global )
{
    return m_books->getOwnedCount(global);
}

int Library::getReadCount(bool global )
{
    return m_books->getReadCount(global);
}

int Library::getDigitalCount(bool global )
{
    return m_books->getDigitalCount(global);
}

QStringList Library::getCollane()
{
    return m_books->getCollane();
}

QStringList Library::getEditors()
{
    return m_books->getEditors();
}

QList<Book> Library::getOwnedBooks()
{
    return m_books->getOwnedBooks();

}

QList<Book> Library::getReadBooks()
{
    return m_books->getReadBooks();

}

bool Library::updateOwn(const QMap<int, bool> &owned)
{
     return m_books->updateOwn( owned );
}

void Library::setCollana(const QString &collana)
{
    m_books->setCollana( collana );
}

bool Library::populateDatabase(const QString &path, const QString &basename)
{
    int count = 1;
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
            QString name = QString( "%1/%2%4.htm").arg(subpath).arg( basename ).arg( i );

            qWarning() << name;

            if ( QFile::exists( name ) == false )
                continue;

            Book book;
            book.fromHTML( name );
            if ( book.number <=0 )
            {
                book.number = count ++;
            }
            else
            {
                count = book.number+1;
            }

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

bool Library::addBook( Book &book)
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

bool Library::updateBookIsDigital(int id, bool digital)
{
    return m_books->updateBookIsDigital( id, digital );
}

