
#include "DbManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

DbManager::DbManager(const QString &path, QObject *parent)
    : QObject{parent}
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);

    if (!m_db.open())
    {
      qDebug() << "Error: connection with database failed";
    }
    else
    {
      qDebug() << "Database: connection ok";
    }
}

bool DbManager::createTables()
{
    bool success = false;

    QSqlQuery query;

    query.prepare("CREATE TABLE IF NOT EXISTS books(number INTEGER, \
                  title_ita TEXT, \
                  title_orig TEXT, \
                  author TEXT, \
                  date_pub DATE, \
                  cover_author TEXT, \
                  cover_image BLOB, \
                  synopsis TEXT, \
                  synopsis_image BLOB, \
                  owned BOOL, \
                  stars INTEGER, \
                  comment TEXT, \
                  reprint BOOL, \
                  read BOOL, \
                  collana TEXT, \
                  id INTEGER, PRIMARY KEY(id));");

    if(query.exec())
    {
       qDebug() << "Database BOOKS created";
       success = true;
       m_currentBookCount = -1;
    }
    else
    {
        qDebug() << "Database BOOKS error:"
                 << query.lastError();
    }


    query.prepare("CREATE TABLE IF NOT EXISTS indexes(id INTEGER, title TEXT, author TEXT);");

    if(query.exec())
    {
        qDebug() << "Database INDEXES created";
        success &= true;
        m_currentBookCount = -1;
     }
     else
     {
         qDebug() << "Database INDEXES error:"
                  << query.lastError();
     }

    return success;
}

bool DbManager::addBook( Book &book)
{
    bool success = false;

    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO books (number,title_ita,title_orig,author,date_pub,cover_author,cover_image,synopsis,synopsis_image,owned,stars,comment,read,collana,editore) "
                  "VALUES    (:number,:title_ita,:title_orig,:author,:date_pub,:cover_author,:cover_image,:synopsis,:synopsis_image,:owned,:stars,:comment,:read,:collana,:editore)");

    query.bindValue(":number", book.number);
    query.bindValue(":title_ita", book.title_ita);
    query.bindValue(":title_orig", book.title_orig);
    query.bindValue(":author", book.author);
    query.bindValue(":date_pub", book.date_pub);
    query.bindValue(":cover_author", book.cover_author);
    query.bindValue(":cover_image", book.cover_image);
    query.bindValue(":synopsis", book.synopsis);
    query.bindValue(":synopsis_image", book.synopsis_image);
    query.bindValue(":owned", book.owned);
    query.bindValue(":stars", book.stars);
    query.bindValue(":comment", book.comment);
    query.bindValue(":read", book.read);
    query.bindValue(":collana", book.collana);
    query.bindValue(":editore", book.editore);


    if(query.exec())
    {
       success = true;
       m_currentBookCount = -1;

       int id = -1;

       query.prepare("SELECT MAX(id) FROM books;");
       if(query.exec())
       {
           query.next();
           id = query.value(0).toInt();
       }
       else
       {
           qDebug() << "add book error:"
                    << query.lastError();
           return false;
       }

       if ( id == -1 )
           return false;


       foreach( Index index, book.index )
       {
           success &= addIndex(id, index);
       }

    }
    else
    {
        qDebug() << "add book error:"
                 << query.lastError();
    }

#if 0
    query.prepare("UPDATE books as b \
                    SET reprint = 1 \
                    WHERE EXISTS ( \
                      SELECT * \
                      FROM books AS a \
                      WHERE a.number < b.number \
                      AND a.title_ita = b.title_ita \
                      AND a.author = b.author \
                    );");
    if(query.exec())
    {
       qWarning() << "Updated reprints";
    }
    else
    {
        qDebug() << "updatereprint error:"
                 << query.lastError();
    }

#endif



    return success;
}

bool DbManager::updateBookOwned(int id, bool owned)
{
    bool success = false;

    QSqlQuery query;
    query.prepare("UPDATE books SET owned = :owned WHERE id = :id");


    query.bindValue(":id", id );
    query.bindValue(":owned", owned);

    if(query.exec())
    {
       success = true;
    }
    else
    {
        qDebug() << "update owned:"
                 << query.lastError();
    }
    return success;
}

bool DbManager::updateBookRead(int id, bool read)
{
    bool success = false;

    QSqlQuery query;
    query.prepare("UPDATE books SET read = :read WHERE id =  :id");


    query.bindValue(":id", id);
    query.bindValue(":read", read);

    if(query.exec())
    {
       success = true;
    }
    else
    {
        qDebug() << "update read:"
                 << query.lastError();
    }
    return success;
}

bool DbManager::updateBookComment(int id, const QString &comment)
{
    bool success = false;

    QSqlQuery query;
    query.prepare("UPDATE books SET comment = :comment WHERE id = :id)");


    query.bindValue(":id", id);
    query.bindValue(":comment", comment);

    if(query.exec())
    {
       success = true;
    }
    else
    {
        qDebug() << "update comment:"
                 << query.lastError();
    }
    return success;
}

bool DbManager::updateBookStars(int id, int stars)
{
    bool success = false;

    QSqlQuery query;
    query.prepare("UPDATE books SET stars = :stars WHERE id =  :id");


    query.bindValue(":id", id );
    query.bindValue(":stars", stars);

    if(query.exec())
    {
       success = true;
    }
    else
    {
        qDebug() << "update stars:"
                 << query.lastError();
    }
    return success;
}

bool DbManager::addIndex(int id, const Index &index)
{
    bool success = false;

    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO indexes (id,title, author) "
                  "VALUES    (:id,:title,:author)");

    query.bindValue(":id", id );
    query.bindValue(":title", index.title);
    query.bindValue(":author", index.author);
    if(query.exec())
    {
       success = true;
    }
    else
    {
        qDebug() << "add indexes:"
                 << query.lastError();
    }
    return success;
}

QString DbManager::typeToField(int type)
{

    QString retVal = "title_ita";

    switch( type )
    {
        case 0:
            retVal = "number";
        break;
        case 1:
            retVal = "title_ita";
        break;

        case 2:
            retVal = "title_orig";
        break;

        case 3:
            retVal = "author";
        break;

        case 4:
            retVal = "date_pub";
        break;

        case 5:
            retVal = "cover_author";
        break;

        default: break;


    }


    return retVal;
}

bool DbManager::getBook(int number, Book &book, bool global)
{
    bool success = false;

    int id = -1;

    QSqlQuery query;

    if ( global )
    {
        query.prepare( QString("SELECT id FROM books ORDER BY id LIMIT 1 OFFSET %2-1 " ).arg( number ));

    }
    else
    {
        query.prepare( QString("SELECT id FROM books where collana = :collana ORDER BY id LIMIT 1 OFFSET %2-1 " ).arg( number ));
        query.bindValue(":collana", m_collana);
    }

    if(query.exec())
    {
        if (query.next())
        {
            id = query.value( 0 ).toInt();
        }
    }
    else
    {
        return false;
    }

    if ( id == -1 )
    {
        return false;
    }

    return getBookById( id, book ) != -1;


}

int DbManager::getBookById(int id, Book &book)
{
    int retval = -1;

    QSqlQuery query;
    query.prepare("SELECT * FROM books WHERE id = :id");

    query.bindValue(":id", id);

    QString collana = "";

    if(query.exec())
    {
        if (query.next())
        {
            book= bookFromQuery( query );

            collana = book.collana;

            query.prepare(QString( "SELECT count(id) FROM books WHERE collana= :collana and id <= %1;").arg(id) );
            query.bindValue(":collana", book.collana);
            if(query.exec())
            {
                if (query.next())
                {
                    retval = query.value(0).toInt();
                }
                else
                {
                    return -1;
                }
            }
            else
            {
                return -1;
            }
        }
        else
        {
            return -1;
        }

        query.prepare("SELECT * FROM indexes WHERE id = (:id)");
        query.bindValue(":id", id);
        if(query.exec())
        {
            while (query.next())
            {
                Index index;
                index.author = query.value( query.record().indexOf("author") ).toString();
                index.title = query.value( query.record().indexOf("title") ).toString();
                book.index.append(index);
            }

        }
        else
        {
            qDebug() << "getBook<int> indexes error:"  << query.lastError();
        }
    }
    else
    {
        qDebug() << "getBook<int> error:"
                 << query.lastError();
    }

    return retval;
}

int DbManager::getBooksCount()
{
    QSqlQuery query;
    query.prepare("SELECT count(id) as num_books FROM books where collana = :collana");
    query.bindValue(":collana", m_collana);
    if(query.exec())
    {
        if(query.next())
        {
            qWarning() <<  query.value( 0 ).toInt();
            m_currentBookCount =  query.value( 0 ).toInt();
        }
    }

    return m_currentBookCount;
}

int DbManager::getOwnedCount()
{

    QSqlQuery query;
    query.prepare("SELECT count(number) as num_books FROM books WHERE owned = true AND collana = (:collana)");
    query.bindValue(":collana", m_collana);
    if(query.exec())
    {
        if(query.next())
        {
            return  query.value( 0 ).toInt();
        }
    }

    return 0;
}

int DbManager::getReadCount()
{
    QSqlQuery query;
    query.prepare("SELECT count(id) as num_books FROM books WHERE read = true AND collana = :collana");
    query.bindValue(":collana", m_collana);
    if(query.exec())
    {
        if(query.next())
        {
            return  query.value( 0 ).toInt();
        }
    }

    return 0;
}

QList<Book> DbManager::getOwnedBooks()
{
    QList<Book> books;
    QSqlQuery query;
    query.prepare("SELECT * FROM books WHERE owned = true AND collana = :collana");
    query.bindValue(":collana", m_collana);
    if(query.exec())
    {
        if(query.next())
        {
            while (query.next())
            {
                Book book = bookFromQuery( query );

                books.append( book );
            }
        }
    }

    return books;
}

QList<Book> DbManager::getReadBooks()
{
    QList<Book> books;
    QSqlQuery query;
    query.prepare("SELECT * FROM books WHERE read = true AND collana = :collana");
    query.bindValue(":collana", m_collana);
    if(query.exec())
    {
        while (query.next())
        {
            Book book = bookFromQuery( query );

            books.append( book );
        }

    }

    return books;
}

QStringList DbManager::getCollane()
{
    QStringList data;
    QSqlQuery query;
    query.prepare("select distinct collana from books;");
    query.bindValue(":collana", m_collana);

    if(query.exec())
    {
        while (query.next())
        {
            QString collana = query.value( 0 ).toString();
            data << collana;
        }
    }

    return data;

}

void DbManager::setCollana(const QString &collana)
{
    m_collana = collana;
}

Book DbManager::bookFromQuery( QSqlQuery &query )
{
    Book book;
    book.number =            query.value( query.record().indexOf("number") ).toInt();
    book.title_ita =         query.value( query.record().indexOf("title_ita") ).toString();
    book.title_orig =        query.value( query.record().indexOf("title_orig") ).toString();
    book.author =            query.value( query.record().indexOf("author") ).toString();
    QString date =           query.value( query.record().indexOf("date_pub") ).toString();
    book.date_pub =          QDate::fromString( date, "yyyy-MM-dd" );
    book.cover_author =      query.value( query.record().indexOf("cover_author") ).toString();
    book.cover_image =       query.value( query.record().indexOf("cover_image") ).toByteArray();
    book.synopsis =          query.value( query.record().indexOf("synopsis") ).toString();
    book.synopsis_image =    query.value( query.record().indexOf("synopsis_image") ).toByteArray();
    book.owned =             query.value( query.record().indexOf("owned") ).toBool();
    book.stars =             query.value( query.record().indexOf("stars") ).toInt();
    book.comment =           query.value( query.record().indexOf("comment") ).toString();
    book.reprint =           query.value( query.record().indexOf("reprint") ).toBool();
    book.read =              query.value( query.record().indexOf("read") ).toBool();
    book.collana =           query.value( query.record().indexOf("collana") ).toString();
    book.editore =           query.value( query.record().indexOf("editore") ).toString();
    book.id     =            query.value( query.record().indexOf("id") ).toInt();

    return book;
}

QString DbManager::searchBooks(const QString &text, const QString & field, QList<Book> &books)
{

    if (!text.isEmpty())
    {
        QString stQuery = QString("SELECT id FROM books WHERE %1 LIKE '%%2%';").arg( field, text );

        QSqlQuery query;
        query.prepare( stQuery );
        //query.bindValue(":collana", m_collana);

        if(query.exec())
        {
            books.clear();
            while (query.next())
            {
                int id = query.value( 0 ).toInt();

                Book book;
                if ( getBook(id, book , true ) )
                {
                    books.append( book );
                }
            }
        }
    }

    return field;
}

