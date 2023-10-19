
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

    query.prepare("CREATE TABLE IF NOT EXISTS books(number INTEGER PRIMARY KEY, \
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
                  comment TEXT);");

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


    query.prepare("CREATE TABLE IF NOT EXISTS indexes(number INTEGER, title TEXT, author TEXT);");

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

bool DbManager::addBook(const Book &book)
{
    bool success = false;

    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO books (number,title_ita,title_orig,author,date_pub,cover_author,cover_image,synopsis,synopsis_image,owned,stars,comment) "
                  "VALUES    (:number,:title_ita,:title_orig,:author,:date_pub,:cover_author,:cover_image,:synopsis,:synopsis_image,:owned,:stars,:comment)");

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

    if(query.exec())
    {
       success = true;
       m_currentBookCount = -1;
       foreach( Index index, book.index )
       {
           success &= addIndex(book.number, index);
       }
    }
    else
    {
        qDebug() << "add book error:"
                 << query.lastError();
    }




    return success;
}

bool DbManager::updateBookOwned(int number, bool owned)
{
    bool success = false;

    QSqlQuery query;
    query.prepare("UPDATE books SET owned = :owned WHERE number = :number");


    query.bindValue(":number", number);
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

bool DbManager::updateBookComment(int number, const QString &comment)
{
    bool success = false;

    QSqlQuery query;
    query.prepare("UPDATE books SET comment = :comment WHERE number = :number");


    query.bindValue(":number", number);
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

bool DbManager::updateBookStars(int number, int stars)
{
    bool success = false;

    QSqlQuery query;
    query.prepare("UPDATE books SET stars = :stars WHERE number = :number");


    query.bindValue(":number", number);
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

bool DbManager::addIndex(int number, const Index &index)
{
    bool success = false;

    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO indexes (number,title, author) "
                  "VALUES    (:number,:title,:author)");

    query.bindValue(":number", number);
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

bool DbManager::getBook(int number, Book &book)
{
    bool success = false;


    QSqlQuery query;
    query.prepare("SELECT * FROM books WHERE number = (:number)");
    query.bindValue(":number", number);
    if(query.exec())
    {

        while (query.next())
        {

            book.number =            query.value( query.record().indexOf("number") ).toInt();
            book.title_ita =         query.value( query.record().indexOf("title_ita") ).toString();
            book.title_orig =        query.value( query.record().indexOf("title_orig") ).toString();
            book.author =            query.value( query.record().indexOf("author") ).toString();
            QString date = query.value( query.record().indexOf("date_pub") ).toString();
            book.date_pub =          QDate::fromString( date, "yyyy-MM-dd" );
            book.cover_author =      query.value( query.record().indexOf("cover_author") ).toString();
            book.cover_image =       query.value( query.record().indexOf("cover_image") ).toByteArray();
            book.synopsis =          query.value( query.record().indexOf("synopsis") ).toString();
            book.synopsis_image =    query.value( query.record().indexOf("synopsis_image") ).toByteArray();
            book.owned =             query.value( query.record().indexOf("owned") ).toBool();
            book.stars =             query.value( query.record().indexOf("stars") ).toInt();
            book.comment =           query.value( query.record().indexOf("comment") ).toString();

            success = true;
        }

        query.prepare("SELECT * FROM indexes WHERE number = (:number)");
        query.bindValue(":number", number);
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



    return success;
}

int DbManager::getBookCount()
{
    if ( m_currentBookCount < 0 )
    {
        QSqlQuery query;
        query.prepare("SELECT MAX(number) as num_books FROM books");
        if(query.exec())
        {
            if(query.next())
            {
                qWarning() <<  query.value( 0 ).toInt();
                m_currentBookCount =  query.value( 0 ).toInt();
            }
        }
    }
    return m_currentBookCount;
}

