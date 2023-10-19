
#include <QDebug>
#include <QShortcut>
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Library.h"
#include <QLineEdit>
#include "StarRating.h"



qreal roundToHalf(qreal value) {
    return qRound(value * 2.0) / 2.0;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_current = new QLineEdit( this );
    m_current->setFixedWidth( 100 );
    m_current->setText( "1" );
    m_current->setAlignment( Qt::AlignCenter );

    ui->toolBar->insertWidget(ui->actionNext, m_current);

    connect( ui->actionNext, &QAction::triggered,this, &MainWindow::onNext );
    connect( ui->actionPrevious, &QAction::triggered,this, &MainWindow::onPrevious );

    connect( m_current, &QLineEdit::returnPressed, this, &MainWindow::onCurrentChanged );

    connect( ui->owned, &QCheckBox::stateChanged, this, &MainWindow::onOwnedChanged );

    m_starRating = new StarRating( this );

    connect( m_starRating, &StarRating::ratingChanged, this, &MainWindow::onRatingChanged );


    auto lay = qobject_cast<QGridLayout*>(ui->frame->layout());

    lay->addWidget(m_starRating,5,1);

    QShortcut *leftShortcut = new QShortcut(Qt::Key_Left, this);
    QShortcut *rightShortcut = new QShortcut(Qt::Key_Right, this);

    // Connect slots to the activated signals of the shortcuts
    connect(leftShortcut, &QShortcut::activated, this, &MainWindow::onPrevious);
    connect(rightShortcut, &QShortcut::activated, this, &MainWindow::onNext);



    Book book;

    m_library = new Library();

    m_library->getBook( 1, book );

    viewBook( book );




}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::viewBook(Book &book)
{
    ui->author->setText( book.author );
    ui->title_ita->setText( book.title_ita );
    ui->title_orig->setText( book.title_orig );
    ui->date_pub->setText( book.date_pub.toString("dd/MM/yyyy"));
    ui->cover_author->setText( book.cover_author );
    ui->number->setText( QString::number( book.number ) );

    ui->comment->setText( book.comment );

    m_starRating->setRating( book.stars / 10 );

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
    if (m_currentBook < m_library->getBookCount())
    {
       Book book;
       m_library->getBook( ++m_currentBook, book );
       viewBook( book );
       m_current->setText( QString::number( m_currentBook ) );
    }
}

void MainWindow::onPrevious()
{
    if (m_currentBook > 1)
    {
       Book book;
       m_library->getBook( --m_currentBook, book );
       viewBook( book );
       m_current->setText( QString::number( m_currentBook ) );
    }
}

void MainWindow::onCurrentChanged()
{
    int value = m_current->text().toInt();
    if ( value >=1 && value <=  m_library->getBookCount() )
    {
        Book book;
        m_currentBook = value;
        m_library->getBook( m_currentBook, book );
        viewBook( book );
    }

}

void MainWindow::onOwnedChanged(bool)
{
    Book book;
    m_library->getBook( m_currentBook, book );
    book.owned = ui->owned->isChecked();
    m_library->updateBookOwned( book.number, ui->owned->isChecked() );
}

void MainWindow::onRatingChanged(qreal rating)
{
    Book book;
    m_library->getBook( m_currentBook, book );
    book.owned = ui->owned->isChecked();
    m_library->updateBookStars( book.number, (int)( roundToHalf( rating ) * 10 ));
}



