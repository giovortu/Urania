
#include "DbManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QRegularExpression>

DbManager::DbManager(const QString &path, QObject *parent)
    : QObject{parent}
{
    open( path );


}


void DbManager::reopen()
{
    if ( !m_dbFile.isEmpty() )
    {
        close();
        open(m_dbFile);
    }
}


void DbManager::open(const QString &newDB)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_dbFile = newDB;
    m_db.setDatabaseName(newDB);

    if (!m_db.open())
    {
        qDebug() << "Error: connection with database failed";
    }
    else
    {
        qDebug() << "Database: connection ok";
    }

}

void DbManager::close()
{
    m_db.close();
    QSqlDatabase::removeDatabase("QSQLITE");

}

bool DbManager::createTables()
{
    bool success = false;

    QSqlQuery query;

    // Create editori table
    query.prepare("CREATE TABLE IF NOT EXISTS editori (\
      id INTEGER PRIMARY KEY AUTOINCREMENT,\
      nome TEXT NOT NULL UNIQUE,\
      created_at DATETIME DEFAULT CURRENT_TIMESTAMP\
    );");

    if(query.exec())
    {
       qDebug() << "Database EDITORI created / updated";
       success = true;
    }
    else
    {
        qDebug() << "Database EDITORI error:"
                 << query.lastError();
    }

    // Create collane table
    query.prepare("CREATE TABLE IF NOT EXISTS collane (\
      id INTEGER PRIMARY KEY AUTOINCREMENT,\
      nome TEXT NOT NULL,\
      editore_id INTEGER NOT NULL,\
      created_at DATETIME DEFAULT CURRENT_TIMESTAMP,\
      FOREIGN KEY (editore_id) REFERENCES editori(id),\
      UNIQUE(nome, editore_id)\
    );");

    if(query.exec())
    {
       qDebug() << "Database COLLANE created / updated";
       success &= true;
    }
    else
    {
        qDebug() << "Database COLLANE error:"
                 << query.lastError();
    }

    // Create books table with normalized structure
    query.prepare("CREATE TABLE IF NOT EXISTS books (\
    number	INTEGER,\
    title_ita	TEXT,\
    title_orig	TEXT,\
    author	TEXT,\
    date_pub	DATE,\
    cover_author	TEXT,\
    cover_image	BLOB,\
    synopsis	TEXT,\
    synopsis_image	BLOB,\
    owned	BOOL,\
    stars	INTEGER,\
    comment	TEXT,\
    reprint	BOOL,\
    read	BOOL,\
    collana	TEXT,\
    editore	TEXT,\
    id	INTEGER,\
    digital	BOOL,\
    cover_hash TEXT,\
    synopsis_hash TEXT,\
    editore_id INTEGER,\
    collana_id INTEGER,\
    PRIMARY KEY(id)\
    );");


    if(query.exec())
    {
       qDebug() << "Database BOOKS created / updated";
       success &= true;
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
        qDebug() << "Database INDEXES created / updated";
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

bool DbManager::updateOwn(const QMap<int, bool> &owned)
{
    bool success = false;

    QSqlQuery query;
    query.prepare("UPDATE books SET owned = :owned WHERE collana_id = :collana_id AND number = :number");

    QMapIterator<int,bool> i(owned);
    while (i.hasNext())
    {
        i.next();
        query.bindValue(":owned", i.value());
        query.bindValue(":number", i.key());
        query.bindValue(":collana_id", m_collana_id);

        if(query.exec())
        {
           success = true;
           //m_currentBookCount = -1;
        }
        else
        {
            qDebug() << "Database error:"
                     << query.lastError();
        }
    }

    return success;
}

bool DbManager::addBook( Book &book)
{
    bool success = false;

    // Get or create editore_id and collana_id
    if (!book.editore.isEmpty())
    {
        book.editore_id = getOrCreateEditore(book.editore);
    }
    
    if (!book.collana.isEmpty() && book.editore_id > 0)
    {
        book.collana_id = getOrCreateCollana(book.collana, book.editore_id);
    }

    // Calculate hashes
    book.cover_hash = QCryptographicHash::hash(book.cover_image, QCryptographicHash::Md5).toHex();
    book.synopsis_hash = QCryptographicHash::hash(book.synopsis_image, QCryptographicHash::Md5).toHex();

    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO books (number,title_ita,title_orig,author,date_pub,cover_author,cover_image,synopsis,synopsis_image,owned,stars,comment,read,collana,editore,digital,cover_hash,synopsis_hash,editore_id,collana_id) "
                  "VALUES    (:number,:title_ita,:title_orig,:author,:date_pub,:cover_author,:cover_image,:synopsis,:synopsis_image,:owned,:stars,:comment,:read,:collana,:editore,:digital,:cover_hash,:synopsis_hash,:editore_id,:collana_id)");

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
    query.bindValue(":digital", book.isDigital);
    query.bindValue(":cover_hash", book.cover_hash);
    query.bindValue(":synopsis_hash", book.synopsis_hash);
    query.bindValue(":editore_id", book.editore_id > 0 ? book.editore_id : QVariant());
    query.bindValue(":collana_id", book.collana_id > 0 ? book.collana_id : QVariant());


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
    query.prepare("UPDATE books SET comment = :comment WHERE id = :id;");


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

bool DbManager::updateBookIsDigital(int id, bool digital)
{
    bool success = false;

    QSqlQuery query;
    query.prepare("UPDATE books SET digital = :digital WHERE id =  :id");


    query.bindValue(":id", id );
    query.bindValue(":digital", digital);

    if(query.exec())
    {
        success = true;
    }
    else
    {
        qDebug() << "update digital:"
                 << query.lastError();
    }
    return success;
}

bool DbManager::updateBook(Book *book)
{

    bool success = false;

    // Get or create editore_id and collana_id
    if (!book->editore.isEmpty())
    {
        book->editore_id = getOrCreateEditore(book->editore);
    }
    
    if (!book->collana.isEmpty() && book->editore_id > 0)
    {
        book->collana_id = getOrCreateCollana(book->collana, book->editore_id);
    }

    // Calculate hashes
    book->cover_hash = QCryptographicHash::hash(book->cover_image, QCryptographicHash::Md5).toHex();
    book->synopsis_hash = QCryptographicHash::hash(book->synopsis_image, QCryptographicHash::Md5).toHex();

    QSqlQuery query;
    query.prepare(
        "UPDATE books \
        SET number = :number, \
            title_ita = :title_ita, \
            title_orig = :title_orig, \
            author = :author, \
            date_pub = :date_pub, \
            cover_author = :cover_author, \
            cover_image = :cover_image, \
            synopsis = :synopsis, \
            synopsis_image = :synopsis_image, \
            owned = :owned, \
            stars = :stars, \
            comment = :comment, \
            read = :read, \
            collana = :collana, \
            editore = :editore, \
            digital = :digital, \
            cover_hash = :cover_hash, \
            synopsis_hash = :synopsis_hash, \
            editore_id = :editore_id, \
            collana_id = :collana_id \
        WHERE id = :id;");

    query.bindValue(":id", book->id);
    query.bindValue(":number", book->number);
    query.bindValue(":title_ita", book->title_ita);
    query.bindValue(":title_orig", book->title_orig);
    query.bindValue(":author", book->author);
    query.bindValue(":date_pub", book->date_pub);
    query.bindValue(":cover_author", book->cover_author);
    query.bindValue(":cover_image", book->cover_image);
    query.bindValue(":synopsis", book->synopsis);
    query.bindValue(":synopsis_image", book->synopsis_image);
    query.bindValue(":owned", book->owned);
    query.bindValue(":stars", book->stars);
    query.bindValue(":comment", book->comment);
    query.bindValue(":read", book->read);
    query.bindValue(":collana", book->collana);
    query.bindValue(":editore", book->editore);
    query.bindValue(":digital", book->isDigital);
    query.bindValue(":cover_hash", book->cover_hash);
    query.bindValue(":synopsis_hash", book->synopsis_hash);
    query.bindValue(":editore_id", book->editore_id > 0 ? book->editore_id : QVariant());
    query.bindValue(":collana_id", book->collana_id > 0 ? book->collana_id : QVariant());

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
        query.prepare( QString("SELECT id FROM books ORDER BY number LIMIT 1 OFFSET %1-1 " ).arg( number ));

    }
    else
    {
        query.prepare( QString("SELECT id FROM books where collana_id = :collana_id ORDER BY number LIMIT 1 OFFSET %1-1 " ).arg( number ));
        query.bindValue(":collana_id", m_collana_id);
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



bool DbManager::exists(Book &book)
{

    int id = -1;

    QString q = QString("SELECT id FROM books where LOWER(title_ita) LIKE :title_ita AND LOWER(author) LIKE :author AND collana_id = :collana_id;" );

    QSqlQuery query;

    query.prepare( q );
    query.bindValue(":title_ita", book.title_ita.toLower());
    query.bindValue(":author", book.author.toLower());
    query.bindValue(":collana_id", book.collana_id);

    if(query.exec())
    {
        if (query.next())
        {
            id = query.value( 0 ).toInt();
        }
    }
    else
    {
        qWarning() << "ERRORE QUERY exists";
    }


    return id != -1;

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

            query.prepare(QString( "SELECT count(id) FROM books WHERE collana_id = :collana_id and id <= %1;").arg(id) );
            query.bindValue(":collana_id", book.collana_id);
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

int DbManager::getBooksCount(bool global )
{
    QSqlQuery query;
    if ( global )
    {
        query.prepare("SELECT count(number) as num_books FROM books");
    }
    else
    {
        query.prepare("SELECT count(number) as num_books FROM books WHERE collana_id = (:collana_id)");
        query.bindValue(":collana_id", m_collana_id);
    }
    if(query.exec())
    {
        if(query.next())
        {
            //qWarning() <<  query.value( 0 ).toInt();
            m_currentBookCount =  query.value( 0 ).toInt();
        }
    }

    return m_currentBookCount;
}

int DbManager::getOwnedCount(bool global )
{

    QSqlQuery query;
    if ( global )
    {
        query.prepare("SELECT count(number) as num_books FROM books WHERE owned = true");
    }
    else
    {
        query.prepare("SELECT count(number) as num_books FROM books WHERE owned = true AND collana_id = (:collana_id)");
        query.bindValue(":collana_id", m_collana_id);
    }
    if(query.exec())
    {
        if(query.next())
        {
            return  query.value( 0 ).toInt();
        }
    }

    return 0;
}

int DbManager::getReadCount(bool global )
{
    QSqlQuery query;
    if ( global )
    {
        query.prepare("SELECT count(number) as num_books FROM books WHERE read = true");
    }
    else
    {
        query.prepare("SELECT count(number) as num_books FROM books WHERE read = true AND collana_id = (:collana_id)");
        query.bindValue(":collana_id", m_collana_id);
    }
    if(query.exec())
    {
        if(query.next())
        {
            return  query.value( 0 ).toInt();
        }
    }

    return 0;
}

int DbManager::getDigitalCount(bool global )
{
    QSqlQuery query;
    if ( global )
    {
        query.prepare("SELECT count(number) as num_books FROM books WHERE digital = true");
    }
    else
    {
        query.prepare("SELECT count(number) as num_books FROM books WHERE digital = true AND collana_id = (:collana_id)");
        query.bindValue(":collana_id", m_collana_id);
    }
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
    query.prepare("SELECT * FROM books WHERE owned = true AND collana_id = :collana_id");
    query.bindValue(":collana_id", m_collana_id);
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
    query.prepare("SELECT * FROM books WHERE read = true AND collana_id = :collana_id");
    query.bindValue(":collana_id", m_collana_id);
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
    query.prepare("SELECT c.nome, e.nome FROM collane c "
                  "JOIN editori e ON c.editore_id = e.id "
                  "ORDER BY e.nome, c.nome;");

    if(query.exec())
    {
        while (query.next())
        {
            QString editore = query.value(1).toString();
            QString collana = query.value(0).toString();
            // Format: "Collana (Editore)"
            data << QString("%1 (%2)").arg(collana, editore);
        }
    }
    else
    {
        qDebug() << "Error getting collane:" << query.lastError();
        // Fallback to old method if normalized tables don't exist yet
        query.prepare("SELECT DISTINCT collana FROM books WHERE collana IS NOT NULL ORDER BY collana;");
        if(query.exec())
        {
            while (query.next())
            {
                QString collana = query.value(0).toString();
                if (!collana.isEmpty())
                    data << collana;
            }
        }
    }

    return data;
}

QStringList DbManager::getEditors()
{
    QStringList data;
    QSqlQuery query;
    query.prepare("SELECT nome FROM editori ORDER BY nome;");

    if(query.exec())
    {
        while (query.next())
        {
            QString editore = query.value(0).toString();
            data << editore;
        }
    }
    else
    {
        qDebug() << "Error getting editori:" << query.lastError();
        // Fallback to old method if normalized tables don't exist yet
        query.prepare("SELECT DISTINCT editore FROM books WHERE editore IS NOT NULL ORDER BY editore;");
        if(query.exec())
        {
            while (query.next())
            {
                QString editore = query.value(0).toString();
                if (!editore.isEmpty())
                    data << editore;
            }
        }
    }

    return data;
}

int DbManager::getOrCreateEditore(const QString &nome)
{
    if (nome.isEmpty())
        return -1;

    QSqlQuery query;
    
    // Try to find existing editore
    query.prepare("SELECT id FROM editori WHERE nome = :nome");
    query.bindValue(":nome", nome);
    
    if (query.exec() && query.next())
    {
        return query.value(0).toInt();
    }
    
    // Create new editore
    query.prepare("INSERT INTO editori (nome) VALUES (:nome)");
    query.bindValue(":nome", nome);
    
    if (query.exec())
    {
        return query.lastInsertId().toInt();
    }
    else
    {
        qDebug() << "Error creating editore:" << query.lastError();
        return -1;
    }
}

int DbManager::getOrCreateCollana(const QString &nome, int editore_id)
{
    if (nome.isEmpty() || editore_id <= 0)
        return -1;

    QSqlQuery query;
    
    // Try to find existing collana
    query.prepare("SELECT id FROM collane WHERE nome = :nome AND editore_id = :editore_id");
    query.bindValue(":nome", nome);
    query.bindValue(":editore_id", editore_id);
    
    if (query.exec() && query.next())
    {
        return query.value(0).toInt();
    }
    
    // Create new collana
    query.prepare("INSERT INTO collane (nome, editore_id) VALUES (:nome, :editore_id)");
    query.bindValue(":nome", nome);
    query.bindValue(":editore_id", editore_id);
    
    if (query.exec())
    {
        return query.lastInsertId().toInt();
    }
    else
    {
        qDebug() << "Error creating collana:" << query.lastError();
        return -1;
    }
}

bool DbManager::migrateStringToRelational()
{
    QSqlQuery query;
    
    // Get all books with non-null editore/collana strings
    query.prepare("SELECT id, editore, collana FROM books WHERE editore IS NOT NULL OR collana IS NOT NULL");
    
    if (!query.exec())
    {
        qDebug() << "Error fetching books for migration:" << query.lastError();
        return false;
    }
    
    QSqlDatabase::database().transaction();
    
    while (query.next())
    {
        int book_id = query.value(0).toInt();
        QString editore_str = query.value(1).toString();
        QString collana_str = query.value(2).toString();
        
        int editore_id = -1;
        int collana_id = -1;
        
        // Get or create editore
        if (!editore_str.isEmpty())
        {
            editore_id = getOrCreateEditore(editore_str);
        }
        
        // Get or create collana
        if (!collana_str.isEmpty() && editore_id > 0)
        {
            collana_id = getOrCreateCollana(collana_str, editore_id);
        }
        
        // Update book with IDs
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE books SET editore_id = :editore_id, collana_id = :collana_id WHERE id = :id");
        updateQuery.bindValue(":editore_id", editore_id > 0 ? editore_id : QVariant());
        updateQuery.bindValue(":collana_id", collana_id > 0 ? collana_id : QVariant());
        updateQuery.bindValue(":id", book_id);
        
        if (!updateQuery.exec())
        {
            qDebug() << "Error updating book" << book_id << ":" << updateQuery.lastError();
            QSqlDatabase::database().rollback();
            return false;
        }
    }
    
    QSqlDatabase::database().commit();
    qDebug() << "Migration completed successfully";
    return true;
}



void DbManager::setCollana(const QString &collana)
{
    m_collana = collana;
    
    // Extract collana name from format \"Collana (Editore)\" if needed
    QString collanaName = collana;
    QString editoreName;
    
    QRegularExpression rx(\"^(.+?)\\s*\\((.+?)\\)$\");
    QRegularExpressionMatch match = rx.match(collana);
    if (match.hasMatch())
    {
        collanaName = match.captured(1).trimmed();
        editoreName = match.captured(2).trimmed();
    }
    
    // Get collana_id from database
    QSqlQuery query;
    if (!editoreName.isEmpty())
    {
        // Search with both collana and editore
        query.prepare(\"SELECT c.id FROM collane c \"\n                     \"JOIN editori e ON c.editore_id = e.id \"\n                     \"WHERE c.nome = :collana AND e.nome = :editore\");\n        query.bindValue(\":collana\", collanaName);\n        query.bindValue(\":editore\", editoreName);\n    }\n    else\n    {\n        // Search by collana name only (might return first match if multiple)\n        query.prepare(\"SELECT id FROM collane WHERE nome = :collana LIMIT 1\");\n        query.bindValue(\":collana\", collanaName);\n    }\n    \n    if (query.exec() && query.next())\n    {\n        m_collana_id = query.value(0).toInt();\n    }\n    else\n    {\n        // Fallback: try to find by string match in books table\n        query.prepare(\"SELECT DISTINCT collana_id FROM books WHERE collana = :collana AND collana_id IS NOT NULL LIMIT 1\");\n        query.bindValue(\":collana\", collanaName);\n        if (query.exec() && query.next())\n        {\n            m_collana_id = query.value(0).toInt();\n        }\n        else\n        {\n            m_collana_id = -1;\n            qDebug() << \"Could not find collana_id for:\" << collana;\n        }\n    }\n}

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
    book.isDigital=          query.value( query.record().indexOf("digital") ).toBool();
    
    // Read new normalized fields
    book.cover_hash =        query.value( query.record().indexOf("cover_hash") ).toString();
    book.synopsis_hash =     query.value( query.record().indexOf("synopsis_hash") ).toString();
    book.editore_id =        query.value( query.record().indexOf("editore_id") ).toInt();
    book.collana_id =        query.value( query.record().indexOf("collana_id") ).toInt();

    return book;
}

QString DbManager::searchBooks(const QString &text, const QString & field, QList<Book> &books)
{
    QString _text = text;


    if ( field == "digital" )
    {
        _text = "1";
    }

    if ( _text.isEmpty() )
    {
        return field;
    }

    QString stQuery = "";

    if ( field == "indici" )
    {
        stQuery =QString("SELECT * FROM books WHERE id IN ( SELECT id FROM indexes WHERE title LIKE '%%1%'  )").arg( _text );

    }
    else
    {
        stQuery = QString("SELECT id FROM books WHERE %1 LIKE '%%2%';").arg( field, _text );
    }

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
            if ( getBookById(id, book ) )
            {
                books.append( book );
            }
        }
    }


    return field;
}

