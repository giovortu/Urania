# Migrazione Database - Normalizzazione Editori e Collane

## Modifiche Implementate

### Fase 1: Normalizzazione Database (✅ Completata)

#### Nuove Tabelle

##### `editori`
```sql
CREATE TABLE editori (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    nome TEXT NOT NULL UNIQUE,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
)
```

#### `collane`
```sql
CREATE TABLE collane (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    nome TEXT NOT NULL,
    editore_id INTEGER NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (editore_id) REFERENCES editori(id),
    UNIQUE(nome, editore_id)
)
```

### Modifiche alla Tabella `books`

Aggiunti i seguenti campi:
- `editore_id INTEGER` - Foreign key verso la tabella editori
- `collana_id INTEGER` - Foreign key verso la tabella collane
- `synopsis_hash TEXT` - Hash MD5 dell'immagine della sinossi

**Nota**: I campi `editore` e `collana` (TEXT) sono mantenuti per retrocompatibilità.

### Nuovi Campi nella Classe `Book`

```cpp
class Book {
    // ... campi esistenti ...
    
    // Normalized database fields
    int editore_id;
    int collana_id;
    QString cover_hash;
    QString synopsis_hash;
};
```

## Nuovi Metodi

### DbManager

#### `getOrCreateEditore(const QString &nome) -> int`
Cerca un editore per nome, se non esiste lo crea. Ritorna l'ID dell'editore.

#### `getOrCreateCollana(const QString &nome, int editore_id) -> int`
Cerca una collana per nome ed editore_id, se non esiste la crea. Ritorna l'ID della collana.

#### `migrateStringToRelational() -> bool`
Migra i dati esistenti dalle colonne string (`editore`, `collana`) alle tabelle normalizzate, popolando i campi `editore_id` e `collana_id`.

## Processo di Migrazione

### 1. Backup del Database
```bash
cp build/database/books.dba build/database/books.dba.backup
```

### 2. Eseguire la Migrazione

La migrazione può essere eseguita in due modi:

#### Opzione A: Automatica all'avvio
Il metodo `createTables()` usa `CREATE TABLE IF NOT EXISTS`, quindi crea automaticamente le nuove tabelle se non esistono.

Per migrare i dati esistenti, aggiungere nel codice di inizializzazione:
```cpp
DbManager db("/path/to/database/books.dba");
db.createTables();
db.migrateStringToRelational();
```

#### Opzione B: Manuale via SQL
```sql
-- Le tabelle vengono create automaticamente da createTables()

-- Popolare editore_id e collana_id per i libri esistenti
-- Questo viene fatto automaticamente da migrateStringToRelational()
```

### 3. Comportamento dei Metodi Modificati

#### `addBook(Book &book)`
- Chiama automaticamente `getOrCreateEditore()` e `getOrCreateCollana()`
- Calcola automaticamente `cover_hash` e `synopsis_hash`
- Salva sia i campi string che gli ID per retrocompatibilità

#### `updateBook(Book *book)`
- Aggiorna automaticamente `editore_id` e `collana_id` quando cambiano i campi string
- Ricalcola gli hash delle immagini

#### `bookFromQuery(QSqlQuery &query)`
- Legge tutti i nuovi campi dal database
- Popola sia i campi string che gli ID

## Vantaggi della Normalizzazione

1. **Integrità dei dati**: Un editore è definito una sola volta
2. **Coerenza**: Elimina duplicati con lievi variazioni (es. "Mondadori" vs "mondadori")
3. **Efficienza**: Query più veloci per filtrare per editore/collana
4. **Relazioni**: Ogni collana è collegata al suo editore
5. **Manutenzione**: Facile rinominare un editore in un unico punto

## Retrocompatibilità

I campi `editore` e `collana` (TEXT) sono mantenuti per garantire:
- Compatibilità con codice esistente
- Visualizzazione diretta senza JOIN
- Possibilità di rollback

In futuro, dopo la migrazione completa, questi campi potrebbero essere deprecati.

## Test

Prima di usare in produzione:

1. Testare su database di test
2. Verificare che `migrateStringToRelational()` completi senza errori
3. Controllare che i nuovi libri abbiano `editore_id` e `collana_id` popolati
4. Verificare le query esistenti funzionino ancora

## Note Tecniche

- Gli hash delle immagini usano MD5
- Le foreign key non sono enforced di default in SQLite (serve `PRAGMA foreign_keys = ON`)
- La migrazione usa transazioni per garantire atomicità
- ID negativi o 0 indicano valori NULL/non impostati

### Fase 2: Utilizzo ID Relazionali nel Codice (✅ Completata)

#### Modifiche alle Query

Tutte le query che filtravano per `collana` ora usano `collana_id`:

- ✅ `getBooksCount()` - usa `WHERE collana_id = :collana_id`
- ✅ `getOwnedCount()` - usa `WHERE owned = true AND collana_id = :collana_id`
- ✅ `getReadCount()` - usa `WHERE read = true AND collana_id = :collana_id`
- ✅ `getDigitalCount()` - usa `WHERE digital = true AND collana_id = :collana_id`
- ✅ `getOwnedBooks()` - usa `WHERE owned = true AND collana_id = :collana_id`
- ✅ `getReadBooks()` - usa `WHERE read = true AND collana_id = :collana_id`
- ✅ `getBook()` - usa `WHERE collana_id = :collana_id`
- ✅ `getBookById()` - usa `WHERE collana_id = :collana_id`
- ✅ `exists()` - usa `AND collana_id = :collana_id`
- ✅ `updateOwn()` - usa `WHERE collana_id = :collana_id AND number = :number`

#### Modifiche ai Metodi di Ricerca

- ✅ `getCollane()` - legge dalla tabella normalizzata `collane` con JOIN su `editori`
  - Formato output: "Nome Collana (Nome Editore)"
  - Include fallback per compatibilità con database non migrati
  
- ✅ `getEditors()` - legge dalla tabella normalizzata `editori`
  - Include fallback per compatibilità con database non migrati

#### Gestione Collana Corrente

- ✅ Aggiunto campo `int m_collana_id` in `DbManager`
- ✅ `setCollana()` aggiornato per:
  - Parsare il formato "Collana (Editore)"
  - Ricercare l'ID dalla tabella `collane`
  - Memorizzare sia la stringa che l'ID
  - Gestire fallback su database non migrati

#### Vantaggi dell'Uso degli ID

1. **Performance**: Le query con ID integer sono più veloci delle LIKE su stringhe
2. **Integrità**: Non ci sono più problemi con case-sensitivity o spazi
3. **Manutenzione**: Rinominare editore/collana aggiorna automaticamente tutti i libri
4. **Relazioni**: Le foreign key garantiscono coerenza referenziale

### Fase 3: Aggiornamento Interfaccia Utente (✅ Completata)

#### BookEditor

L'editor dei libri è stato aggiornato per gestire il formato normalizzato:

- ✅ **setBook()**: Riconosce il formato "Collana (Editore)" e preseleziona sia la collana che l'editore
- ✅ **saveData()**: Estrae automaticamente collana ed editore dal formato combinato
- ✅ **addCollana()**: Richiede che un editore sia selezionato, crea nel formato "Collana (Editore)"
- ✅ **addEditor()**: Aggiorna automaticamente la lista degli editori dopo l'aggiunta

#### MainWindow

- ✅ **Importazione**: Parsing del formato "Collana (Editore)" per estrarre i valori separati
- ✅ **UI**: Le ComboBox mostrano il formato "Collana (Editore)"
- ✅ **Fallback**: Gestione automatica di editori predefiniti quando necessario

## Note Importanti

### Compatibilità

Il codice mantiene **doppio supporto**:
- I campi string `editore` e `collana` sono ancora popolati
- Gli ID `editore_id` e `collana_id` sono ora utilizzati per le query
- Fallback automatico se le tabelle normalizzate non esistono

### Migrazione Richiesta

Dopo l'aggiornamento del codice, è **necessario eseguire**:

```cpp
DbManager db("/path/to/database/books.dba");
db.createTables(); // Crea editori e collane se non esistono
db.migrateStringToRelational(); // Popola editore_id e collana_id
```

Questo popola i campi ID per i libri esistenti.

### Formato Collane

Le collane sono ora visualizzate nel formato:
```
Nome Collana (Nome Editore)
```

Ad esempio:
- "Urania (Mondadori)"
- "I Gialli (Einaudi)"

Il metodo `setCollana()` gestisce automaticamente questo formato.
