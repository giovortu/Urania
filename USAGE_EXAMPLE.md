# Esempi di Utilizzo - Database Normalizzato

## Inizializzazione e Migrazione

```cpp
// Aprire/creare il database
DbManager db("/path/to/database/books.dba");

// Creare le tabelle (se non esistono)
db.createTables();

// IMPORTANTE: Migrare i dati esistenti dalle stringhe agli ID
db.migrateStringToRelational();
```

## Gestione Editori e Collane

### Ottenere/Creare un Editore

```cpp
// Il metodo cerca l'editore per nome, se non esiste lo crea
int editore_id = db.getOrCreateEditore("Mondadori");

// Esempio con più editori
int mondadori_id = db.getOrCreateEditore("Mondadori");
int einaudi_id = db.getOrCreateEditore("Einaudi");
int feltrinelli_id = db.getOrCreateEditore("Feltrinelli");
```

### Ottenere/Creare una Collana

```cpp
// Prima ottieni l'editore
int editore_id = db.getOrCreateEditore("Mondadori");

// Poi crea la collana associata all'editore
int collana_id = db.getOrCreateCollana("Urania", editore_id);

// Esempio completo
int mondadori_id = db.getOrCreateEditore("Mondadori");
int urania_id = db.getOrCreateCollana("Urania", mondadori_id);
int gialli_id = db.getOrCreateCollana("Gialli Mondadori", mondadori_id);
```

## Aggiungere un Libro

```cpp
Book book;
book.title_ita = "Fondazione";
book.author = "Isaac Asimov";
book.editore = "Mondadori";
book.collana = "Urania";
book.number = 1;
book.date_pub = QDate(1951, 5, 1);
book.owned = true;

// addBook gestisce automaticamente la conversione in ID
// Cerca/crea l'editore e la collana, popola editore_id e collana_id
db.addBook(book);

// Dopo l'aggiunta, book.editore_id e book.collana_id saranno popolati
qDebug() << "Editore ID:" << book.editore_id;
qDebug() << "Collana ID:" << book.collana_id;
```

## Aggiornare un Libro

```cpp
Book book;
// ... carica il libro dal database ...

// Modifica i campi
book.stars = 5;
book.comment = "Capolavoro della fantascienza";

// Se cambi editore o collana, updateBook aggiorna automaticamente gli ID
book.editore = "Einaudi";
book.collana = "Supercoralli";

// updateBook gestisce la conversione automatica
db.updateBook(&book);
```

## Impostare la Collana Corrente

```cpp
// Formato: "Nome Collana (Nome Editore)"
db.setCollana("Urania (Mondadori)");

// Il metodo estrae nome collana e editore, risolve l'ID automaticamente
// Ora m_collana_id contiene l'ID della collana

// Oppure formato semplice (cerca la prima collana con quel nome)
db.setCollana("Urania");
```

## Ottenere Liste di Collane ed Editori

```cpp
// Ottenere tutte le collane (formato: "Collana (Editore)")
QStringList collane = db.getCollane();
foreach(QString collana, collane)
{
    qDebug() << collana; // es: "Urania (Mondadori)"
}

// Ottenere tutti gli editori
QStringList editori = db.getEditors();
foreach(QString editore, editori)
{
    qDebug() << editore; // es: "Mondadori"
}
```

## Conteggi e Statistiche

```cpp
// Impostare la collana corrente
db.setCollana("Urania (Mondadori)");

// Conteggi per la collana corrente
int totalBooks = db.getBooksCount(false);
int ownedBooks = db.getOwnedCount(false);
int readBooks = db.getReadCount(false);
int digitalBooks = db.getDigitalCount(false);

// Conteggi globali (tutte le collane)
int allBooks = db.getBooksCount(true);
int allOwned = db.getOwnedCount(true);
int allRead = db.getReadCount(true);
int allDigital = db.getDigitalCount(true);

qDebug() << "Libri Urania:" << totalBooks;
qDebug() << "Posseduti:" << ownedBooks;
qDebug() << "Letti:" << readBooks;
qDebug() << "Digitali:" << digitalBooks;
```

## Ottenere Liste di Libri

```cpp
// Impostare la collana corrente
db.setCollana("Urania (Mondadori)");

// Ottenere tutti i libri posseduti della collana corrente
QList<Book> ownedBooks = db.getOwnedBooks();
foreach(Book book, ownedBooks)
{
    qDebug() << book.number << "-" << book.title_ita;
}

// Ottenere tutti i libri letti della collana corrente
QList<Book> readBooks = db.getReadBooks();
foreach(Book book, readBooks)
{
    qDebug() << book.title_ita << "by" << book.author;
}
```

## Recuperare un Libro Specifico

```cpp
// Per numero nella collana corrente
db.setCollana("Urania (Mondadori)");

Book book;
if (db.getBook(1, book, false)) // false = usa collana corrente
{
    qDebug() << "Libro 1:" << book.title_ita;
    qDebug() << "Collana ID:" << book.collana_id;
    qDebug() << "Editore ID:" << book.editore_id;
}

// Per ID assoluto nel database
Book bookById;
int bookNumber = db.getBookById(123, bookById);
if (bookNumber != -1)
{
    qDebug() << "Libro con ID 123:" << bookById.title_ita;
    qDebug() << "È il numero" << bookNumber << "nella sua collana";
}
```

## Verificare se un Libro Esiste

```cpp
Book newBook;
newBook.title_ita = "Fondazione";
newBook.author = "Isaac Asimov";
newBook.collana_id = 5; // ID della collana Urania

if (db.exists(newBook))
{
    qDebug() << "Il libro esiste già nel database";
}
else
{
    qDebug() << "Libro non trovato, puoi aggiungerlo";
    db.addBook(newBook);
}
```

## Aggiornamento di Massa

```cpp
// Aggiornare lo stato "posseduto" per più libri
QMap<int, bool> owned;
owned[1] = true;   // Libro 1: posseduto
owned[2] = false;  // Libro 2: non posseduto
owned[3] = true;   // Libro 3: posseduto

db.setCollana("Urania (Mondadori)");
db.updateOwn(owned);
```

## Ricerca

```cpp
QList<Book> results;

// Ricerca per titolo
QString field = db.searchBooks("Fondazione", "title_ita", results);
qDebug() << "Trovati" << results.size() << "libri";

// Ricerca per autore
results.clear();
db.searchBooks("Asimov", "author", results);

// Ricerca negli indici
results.clear();
db.searchBooks("Robot", "indici", results);

// Ricerca libri digitali
results.clear();
db.searchBooks("", "digital", results);
```

## Pattern Completo: Aggiunta Libro con Gestione Completa

```cpp
void addNewBook()
{
    DbManager db("/path/to/database/books.dba");
    
    // Assicurati che le tabelle esistano
    db.createTables();
    
    // Crea un nuovo libro
    Book book;
    book.title_ita = "Il Ciclo delle Fondazioni";
    book.title_orig = "Foundation Series";
    book.author = "Isaac Asimov";
    book.date_pub = QDate(1951, 1, 1);
    book.editore = "Mondadori";
    book.collana = "Urania";
    book.number = 1;
    book.owned = true;
    book.read = true;
    book.stars = 5;
    book.comment = "Capolavoro assoluto";
    book.isDigital = false;
    
    // Carica l'immagine di copertina
    QFile coverFile("cover.jpg");
    if (coverFile.open(QIODevice::ReadOnly))
    {
        book.cover_image = coverFile.readAll();
        coverFile.close();
    }
    
    // addBook gestisce automaticamente:
    // 1. Ricerca/creazione editore
    // 2. Ricerca/creazione collana
    // 3. Calcolo hash immagini
    // 4. Popolamento editore_id e collana_id
    if (db.addBook(book))
    {
        qDebug() << "Libro aggiunto con successo!";
        qDebug() << "ID:" << book.id;
        qDebug() << "Editore ID:" << book.editore_id;
        qDebug() << "Collana ID:" << book.collana_id;
    }
    else
    {
        qDebug() << "Errore nell'aggiunta del libro";
    }
}
```

## Note Importanti

1. **Hash Automatici**: I metodi `addBook()` e `updateBook()` calcolano automaticamente `cover_hash` e `synopsis_hash`

2. **Gestione Automatica ID**: Non è necessario impostare manualmente `editore_id` e `collana_id`, vengono gestiti automaticamente tramite i campi string

3. **Fallback**: Se le tabelle normalizzate non esistono, i metodi fanno fallback alle query originali su stringhe

4. **Formato Collane**: Quando si usa `setCollana()`, il formato "Collana (Editore)" è preferito per disambiguare

5. **Migrazione Obbligatoria**: Dopo l'aggiornamento, eseguire `migrateStringToRelational()` una volta per popolare gli ID nei libri esistenti
