
#include <QDebug>
#include <QShortcut>
#include <QLineEdit>
#include <QFileDialog>
#include <QInputDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QComboBox>
#include <QTimer>

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Library.h"
#include "StarRating.h"
#include "SearchResultDialog.h"
#include "Statistics.h"
#include "SettingsManager.h"
#include "BookInfo.h"
#include "JsonFormWidget.h"
#include "BookEditor.h"
#include "DatabaseUploader.h"


qreal roundToHalf(qreal value) {
    return qRound(value * 2.0) / 2.0;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_library = nullptr;

    auto sett = new SettingsManager( this );

    m_settings = sett->settings();


    m_collana = new QComboBox( this );


    connect( m_collana,&QComboBox::currentTextChanged, this, &MainWindow::onCollanaChanged );



    ui->toolBar->insertWidget(ui->actionFirstBook, m_collana);

    m_current = new QLineEdit( this );
    m_current->setFixedWidth( 50 );
    m_current->setText( "1" );
    m_current->setAlignment( Qt::AlignCenter );

    QLabel *lbl = new QLabel( this );
    lbl->setAlignment( Qt::AlignCenter );
    lbl->setText( " / " );
    lbl->setStyleSheet("background:transparent;margin-bottom:3px;");

    m_totalBooks = new QLabel( this );
    m_totalBooks->setAlignment( Qt::AlignCenter );
    m_totalBooks->setStyleSheet("background:transparent;");
    m_totalBooks->setFixedWidth( 50 );
    m_totalBooks->setText( "0" );

    ui->toolBar->insertWidget(ui->actionNext, m_current);
    ui->toolBar->insertWidget(ui->actionNext, lbl);
    ui->toolBar->insertWidget(ui->actionNext, m_totalBooks);


    m_search = new QLineEdit( this );
    m_search->setFixedWidth( 100 );

    ui->toolBar->insertWidget(ui->actionSearch, m_search);

    m_searchType = new QComboBox( this );
    m_searchType->addItem( "Numero" , "number");
    m_searchType->addItem( "Titolo" , "title_ita");
    m_searchType->addItem( "Titolo originale" , "title_orig");
    m_searchType->addItem( "Autore", "author" );
    m_searchType->addItem( "Data pubblicazione", "date_pub" );
    m_searchType->addItem( "Autore cover", "cover_author" );

    ui->toolBar->insertWidget(ui->actionSearch, m_searchType);

    connect( ui->actionNext, &QAction::triggered,this, &MainWindow::onNext );
    connect( ui->actionPrevious, &QAction::triggered,this, &MainWindow::onPrevious );

    connect( ui->actionBackTenBooks, &QAction::triggered,this, &MainWindow::onPreviuosTen );
    connect( ui->actionForwardTenBooks, &QAction::triggered,this, &MainWindow::onNextTen );
    connect(ui->actionFirstBook, &QAction::triggered, this, &MainWindow::goFirst);
    connect(ui->actionLastBook, &QAction::triggered, this, &MainWindow::goLast);


    connect( m_current, &QLineEdit::returnPressed, this, &MainWindow::onCurrentChanged );
    connect( m_search, &QLineEdit::returnPressed, this, &MainWindow::onSearch );
    connect( ui->actionSearch, &QAction::triggered, this, &MainWindow::onSearch );

    connect( ui->actionBookInfo, &QAction::triggered, this, &MainWindow::onBookInfo );

    connect( ui->owned, &QCheckBox::clicked, this, &MainWindow::onOwnedChanged );
    connect( ui->read, &QCheckBox::clicked, this, &MainWindow::onReadChanged );

    m_starRating = new StarRating( this );

    connect( m_starRating, &StarRating::ratingChanged, this, &MainWindow::onRatingChanged );

    connect( ui->saveComments, &QPushButton::clicked, this, &MainWindow::onSaveComments );


    auto lay = qobject_cast<QGridLayout*>(ui->frame->layout());

    lay->addWidget(m_starRating,5,1);

    QShortcut *leftShortcut = new QShortcut(Qt::ALT +Qt::Key_Left, this);
    QShortcut *rightShortcut = new QShortcut(Qt::ALT +Qt::Key_Right, this);

    QShortcut *setOwnShortCut = new QShortcut(Qt::ALT +Qt::Key_Up, this);
    QShortcut *setReadShortcut = new QShortcut(Qt::ALT +Qt::Key_Down, this);

    // Connect slots to the activated signals of the shortcuts
    connect(leftShortcut, &QShortcut::activated, this, &MainWindow::onPrevious);
    connect(rightShortcut, &QShortcut::activated, this, &MainWindow::onNext);

    connect(setOwnShortCut, &QShortcut::activated, this, &MainWindow::toggleOwned);
    connect(setReadShortcut, &QShortcut::activated, this, &MainWindow::toggleRead);


    connect( ui->actionImportFromFile, &QAction::triggered, this, &MainWindow::onImportFromFile );
    connect( ui->actionImport_from_web, &QAction::triggered, this, &MainWindow::onImportFromWeb );

    connect( ui->actionStatistics, &QAction::triggered, this, &MainWindow::onStatistics );

    connect( ui->actionNewBook, &QAction::triggered, this, &MainWindow::onNewBook );

    connect( ui->actionUpload, &QAction::triggered, this, &MainWindow::onUpload );


    readSettings();

    initLibrary();


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initLibrary()
{
    if ( nullptr != m_library )
    {
        delete m_library;
    }

    m_library = new Library( "database/books.db");

    QStringList collane = m_library->getCollane();

    m_nomeCollana = collane.at(0);

    m_library->setCollana( m_nomeCollana );

    foreach( QString collana, collane )
    {
        m_collana->addItem( collana );
    }

    updateView();

}

void MainWindow::updateView()
{
    m_library->setCollana( m_nomeCollana );
    m_currentBookNumber = 1;

    m_totalBooks->setText( QString::number( m_library->getBooksCount() ) );

    Book book;

    if ( m_library->getBook( m_currentBookNumber, book ) )
    {
      viewBook( book );
    }
}

void MainWindow::onBookInfo()
{
    auto info = new BookInfo( );

    connect( info, &BookInfo::isbnFound, this, [=]( const QJsonObject & isbn ){

        auto form = new JsonFormWidget( this );
        form->loadJson( isbn );

        form->show();

        qWarning() << isbn;
        info->deleteLater();

    } );

    connect( info, &BookInfo::bookNotFound, this,  [=]( ){

        qWarning() << "Not found";
        info->deleteLater();

    } );

    QStringList search;

    Book book;
    if ( m_library->getBook( m_currentBookNumber, book ) )
    {
        QString title = book.title_ita.replace( QRegularExpression("[^a-zA-Z0-9]"), " " );
        title = title.replace( QRegularExpression("\\s+") , " ");

        QString author = book.author.replace( QRegularExpression("[^a-zA-Z0-9]"), " " );
        author = author.replace( QRegularExpression("\\s+") , " ");

        title = title.replace(" ","+" );
        author = author.replace(" ","+" );

        QString data = book.date_pub.toString("yyyy");

        QString editore = book.editore.replace(" ","%20");


        info->searchBookInfo( title+ "+"+ author + "+inpublisher:" +  editore + "+" + data );

    }



}

void MainWindow::viewBook(Book &book)
{
    m_current->setText( QString::number( m_currentBookNumber ) );
    ui->author->setText( book.author );
    ui->author->setToolTip( book.author );

    ui->collana->setText( book.collana );

    ui->title_ita->setText( book.title_ita );
    ui->title_ita->setToolTip( book.title_ita );

    ui->title_orig->setText( book.title_orig );
    ui->title_orig->setToolTip( book.title_orig );

    ui->date_pub->setText( book.date_pub.toString("dd/MM/yyyy"));
    ui->date_pub->setToolTip( book.date_pub.toString("dd/MM/yyyy"));

    ui->cover_author->setText( book.cover_author );
    ui->cover_author->setToolTip( book.cover_author );

    ui->number->setText( QString::number( book.number ) );
    ui->number->setToolTip( QString::number( book.number ) );

    ui->comment->setText( book.comment );

    ui->reprint->setChecked( book.reprint );

    m_starRating->setRating( book.stars / 10 );

    ui->editore->setText( book.editore );


    if ( !book.synopsis.isEmpty() )
    {
        ui->synopsis->setText( book.synopsis );
        ui->synopsis->show();
        ui->synopsis_image->hide();

    }
    else
    if ( !book.synopsis_image.isEmpty() )
    {
        QPixmap pix;
        ui->synopsis->hide();
        ui->synopsis_image->show();
        pix.loadFromData( book.synopsis_image, "JPEG" );
        ui->synopsis_image->setFixedSize( pix.size() );
        ui->synopsis_image->setPixmap( pix );
    }


    QPixmap pix;
    pix.loadFromData( book.cover_image, "JPEG" );
    ui->cover_img->setPixmap( pix );

    ui->owned->setChecked( book.owned );
    ui->read->setChecked( book.read );

    while (QLayoutItem* item =  ui->indexLayout->takeAt(0)) {
          if (QWidget* widget = item->widget()) {
              delete widget;
          }
          delete item;
      }

    int row = 0;
    foreach( Index index, book.index )
    {
        QLabel *label = new QLabel( index.title );
        label->setWordWrap(true);
        ui->indexLayout->addWidget( label ,row,0);

        label = new QLabel( index.author );
        label->setWordWrap(true);
        ui->indexLayout->addWidget( label ,row,1 );
        row++;
    }


}

void MainWindow::onNext()
{
    if (m_currentBookNumber < m_library->getBooksCount())
    {

       if (  m_library->getBook( ++m_currentBookNumber, m_currentBook ) )
       {
         viewBook( m_currentBook );
         m_current->setText( QString::number( m_currentBookNumber ) );
       }
    }
}

void MainWindow::onPrevious()
{
    if (m_currentBookNumber > 1)
    {
       if ( m_library->getBook( --m_currentBookNumber, m_currentBook ) )
       {
           viewBook( m_currentBook );
           m_current->setText( QString::number( m_currentBookNumber ) );
       }
    }
}

void MainWindow::toggleOwned()
{
    ui->owned->toggle();

    onOwnedChanged( false );
}

void MainWindow::toggleRead()
{
    ui->read->toggle();

    onReadChanged( false );
}

void MainWindow::onNextTen()
{
    int prev = m_currentBookNumber + 10;

    if (  prev <= m_library->getBooksCount() )
    {
        loadBook( prev );
    }
    else
    {
        loadBook( m_library->getBooksCount()  );

    }

}

void MainWindow::onPreviuosTen()
{
    int prev = m_currentBookNumber - 10;

    if (  prev >=1 )
    {
        loadBook( prev );
    }
    else
    {
        loadBook( 1 );
    }
}

void MainWindow::goFirst()
{
    loadBook( 1 );
}

void MainWindow::goLast()
{
    loadBook( m_library->getBooksCount()  );

}

void MainWindow::loadBook(int value)
{
    if ( value > 0 && value != m_currentBookNumber && value >=1 && value <=  m_library->getBooksCount() )
    {

        m_currentBookNumber = value;
        if ( m_library->getBook( m_currentBookNumber, m_currentBook ) )
        {
            m_current->setText( QString::number( m_currentBookNumber) );
            viewBook( m_currentBook );
        }
    }
}

void MainWindow::loadBookById(int id)
{


    int number = m_library->getBookById( id, m_currentBook );
    if ( number > 0 )
    {
        m_nomeCollana = m_currentBook.collana;
        m_library->setCollana( m_nomeCollana );
        m_collana->setCurrentText( m_nomeCollana );

        m_currentBookNumber = number;

        m_current->setText( QString::number( m_currentBookNumber) );
        m_totalBooks->setText( QString::number( m_library->getBooksCount() ) );
        viewBook( m_currentBook );
    }
}

void MainWindow::onSearch()
{
    QList<Book> m_results;
    QString type = m_searchType->currentData().toString();

    QString typeText = m_library->searchBooks( m_search->text(), type,  m_results );

    if ( m_results.size() > 0 )
    {
        auto dialog = new SearchResultDialog();

        dialog->setWindowTitle( QString("Search results for \"%1\" in \"%2\"").arg( m_search->text(), typeText ) );

        dialog->setResults( m_results );

        connect( dialog, &SearchResultDialog::showBook, this, &MainWindow::loadBookById );

        dialog->show();
    }
}

void MainWindow::onStatistics()
{
    int numBooks = m_library->getBooksCount();
    int numOwned = m_library->getOwnedCount();
    int numRead = m_library->getReadCount();

    Statistics dialog;

    dialog.setWindowTitle( "Statistics" );

    dialog.setNumBooks( numBooks );
    dialog.setOwnedBooks( numOwned );
    dialog.setReadBooks( numRead );

    dialog.exec();


}

void MainWindow::onNewBook()
{
    auto editor = new BookEditor( this );

    Book book;

    editor->setBook( &book );

    if ( QDialog::Accepted == editor->exec() )
    {
        int index = m_library->getBooksCount();

        book.number = index + 1;
        m_library->addBook( book );
        m_currentBookNumber = book.id;
        m_currentBook = book;
        viewBook( m_currentBook );
    }


}

void MainWindow::onUpload()
{
    auto uploader = new DatabaseUploader( nullptr );
    uploader->show();

    uploader->startUpload();



}

void MainWindow::readSettings()
{
    m_settings->beginGroup("settings");
        m_currentBookNumber = m_settings->value("current_book", 1 ).toInt();
        m_currentDatabase = m_settings->value("current_database", "urania.db" ).toString();
    m_settings->endGroup();
}

void MainWindow::writeSettings()
{
    m_settings->beginGroup("settings");
        m_settings->setValue("current_book", m_currentBookNumber );
        m_settings->setValue("current_database", m_currentDatabase );
    m_settings->endGroup();
}

void MainWindow::onCollanaChanged(const QString &txt)
{
    QString collana = m_collana->currentText();
    if ( collana != m_nomeCollana )
    {
        m_nomeCollana = collana;
        m_currentBookNumber = 1;
        updateView();
    }

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if ( !m_safeExit )
    {
        event->ignore();

        writeSettings();

        QApplication::setOverrideCursor(Qt::BusyCursor);

        m_safeExit = true;


        QTimer::singleShot( 1500, this, [=]()
        {
            qWarning()  << "Finally exiting application";

            QApplication::quit();
        });

    }
    else
    {
        event->accept();
    }


}

void MainWindow::onCurrentChanged()
{
    int value = m_current->text().toInt();
    if ( value >=1 && value <=  m_library->getBooksCount() )
    {
        m_currentBookNumber = value;
        if ( m_library->getBook( m_currentBookNumber, m_currentBook ) )
        {
            viewBook( m_currentBook );
        }
        else
        {

        }
    }

}

void MainWindow::onOwnedChanged(bool)
{
    m_library->updateBookOwned( m_currentBook.id, ui->owned->isChecked() );
}

void MainWindow::onReadChanged(bool)
{
    m_library->updateBookRead( m_currentBook.id, ui->read->isChecked() );
}

void MainWindow::onRatingChanged(qreal rating)
{
    m_library->updateBookStars( m_currentBook.id, (int)( roundToHalf( rating ) * 10 ));
}

void MainWindow::onSaveComments()
{
    m_currentBook.comment = ui->comment->toPlainText();

    m_library->updateBookComment( m_currentBook.id, ui->comment->toPlainText() );
}



void MainWindow::onImportFromFile()
{
    QString file = QFileDialog::getOpenFileName( this, "Load from HTML", "", "HTML files (*.htm)" );

    qDebug() << file ;
 }

void MainWindow::onImportFromWeb()
{

    QString remote = QInputDialog::getText( this, "Import from web", "Enter the URL of the web page to import from" );

    QUrl url(remote);

    auto manager = new QNetworkAccessManager(this);

    // Send an HTTP GET request to the URL
    manager->get(QNetworkRequest(url));

    // Connect a slot to handle the reply when it is ready
    QObject::connect(manager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply)
    {

        if (reply->error() == QNetworkReply::NoError)
        {
            QByteArray htmlData = reply->readAll();
            QString html = QString::fromUtf8(htmlData);
            qDebug() << "HTML content:" << html;

               QString fileName = QApplication::applicationDirPath() + "/temp.html";
               QFile file( fileName );

               if (file.open( QIODevice::WriteOnly) )
               {
                   QTextStream stream(&file);
                   stream << html;
                   file.close();

                   Book book;
                   book.fromHTML( fileName );

                   qWarning() << book.cover_image_path;

                   QFileInfo info( book.cover_image_path );
                   QString cover = info.fileName();

                   QString html = url.fileName();

                   QString img = remote;

                   img = img.replace( html, cover );

                   QUrl imgUrl( img );

                   qWarning() << img;

                   auto manager = new QNetworkAccessManager(this);

                   manager->get(QNetworkRequest(imgUrl));

                   // Connect a slot to handle the reply when it is ready
                   QObject::connect(manager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply)
                   {

                       if (reply->error() == QNetworkReply::NoError)
                       {
                           QByteArray imgData = reply->readAll();

                           QFile f( book.cover_image_path );
                           f.open( QIODevice::WriteOnly );
                           f.write( imgData );
                           f.close();

                            qWarning() << "Saved";

                            Book book;
                            book.fromHTML( fileName );

                            BookEditor editor( this );
                            editor.setBook( &book );

                            if ( editor.exec() )
                            {
                                m_library->addBook( book );
                                m_currentBook = book;
                                viewBook( book );
                            }


                       }
                       else
                       {
                           qDebug() << "Error downloading the image page:" << reply->errorString();
                       }

                       manager->deleteLater();

                   });
               }
        }
        else
        {
            qDebug() << "Error downloading the web page:" << reply->errorString();
        }
        manager->deleteLater();

    });

}






