#include "SearchResultDialog.h"
#include "ui_SearchResultDialog.h"

#include <QHeaderView>
#include <QTableWidget>

SearchResultDialog::SearchResultDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchResultDialog)
{
    ui->setupUi(this);


    //setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::Window );//| Qt::WindowStaysOnTopHint);

    ui->tabWidget->setTabsClosable( true );

    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &SearchResultDialog::closeTab );

    connect( ui->buttonBox, &QDialogButtonBox::rejected, this, &SearchResultDialog::close );


}

SearchResultDialog::~SearchResultDialog()
{
    delete ui;
}

void SearchResultDialog::setResults(QList<Book> &books, const QString & title )
{
    QTableWidget *table = openTab( title );

    m_books = books;

    table->setRowCount(books.size());

    for (int row = 0; row < books.size(); ++row)
    {
        int col = 0;
        QTableWidgetItem *item;
        const Book &book = books.at(row);

        // Number
        item = new QTableWidgetItem(QString::number(book.number));
        item->setData(Qt::UserRole, book.id);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        table->setItem(row, col++, item);
                // Title (ITA)
        item = new QTableWidgetItem(book.title_ita);
        item->setData(Qt::UserRole, book.id);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        table->setItem(row, col++, item);

        // Title (Original)
        item = new QTableWidgetItem(book.title_orig);
        item->setData(Qt::UserRole, book.id);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        table->setItem(row, col++, item);

        // Author
        item = new QTableWidgetItem(book.author);
        item->setData(Qt::UserRole, book.id);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        table->setItem(row, col++, item);

        // Author
        item = new QTableWidgetItem(book.collana);
        item->setData(Qt::UserRole, book.id);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        table->setItem(row, col++, item);

        // Date Published
        item = new QTableWidgetItem(book.date_pub.toString("yyyy-MM-dd"));
        item->setData(Qt::UserRole, book.id);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        table->setItem(row, col++, item);

        // Digital
        item = new QTableWidgetItem(book.isDigital?"Yes":"No");
        item->setData(Qt::UserRole, book.id);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        table->setItem(row, col++, item);

        //Owned
        item = new QTableWidgetItem(book.owned?"Yes":"No");
        item->setData(Qt::UserRole, book.id);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        table->setItem(row, col++, item);

        //Read
        item = new QTableWidgetItem(book.read?"Yes":"No");
        item->setData(Qt::UserRole, book.id);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        table->setItem(row, col++, item);

        item = new QTableWidgetItem("");
        item->setData(Qt::UserRole, book.id);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        table->setItem(row, col++, item);



    }

}

void SearchResultDialog::rowDoubleClicked(int row, int column)
{
    auto table = qobject_cast<QTableWidget *>( sender() );

    if ( nullptr != table )
    {
       auto item = table->item(row, column);

       int bookID = item->data(Qt::UserRole).toInt();

       emit showBook( bookID );
    }
}

void SearchResultDialog::closeTab(int index)
{
    ui->tabWidget->removeTab(index);
}

void SearchResultDialog::closeAllTabs()
{
    while (ui->tabWidget->count() > 0)
    {
        ui->tabWidget->removeTab(0);  // Always remove the first tab (index 0)
    }
}

QTableWidget * SearchResultDialog::openTab( const QString &title )
{

    m_numTabs++;

    auto table = new QTableWidget( this );

    QStringList headers = {"Numero", "Titolo", "Tit. Orig.", "Autore","Collana",  "Data", "Digitale", "Posseduto", "Letto", ""};

    table->setColumnCount(headers.size());

    table->setHorizontalHeaderLabels(headers);

    table->setSelectionBehavior(QAbstractItemView::SelectRows);



    connect( table, &QTableWidget::cellDoubleClicked, this, &SearchResultDialog::rowDoubleClicked );

    for (int col = 0; col < table->columnCount()-1; ++col) {
        table->horizontalHeader()->setSectionResizeMode(col, QHeaderView::ResizeToContents);
    }


    table->horizontalHeader()->setSectionResizeMode(table->columnCount()-1, QHeaderView::Stretch);

    int id = -1;

    if ( !title.isEmpty() )
    {
        id = ui->tabWidget->addTab( table, title );
    }
    else
    {
        id = ui->tabWidget->addTab( table, QString("Search %1").arg( m_numTabs) );
    }

    ui->tabWidget->setCurrentIndex( id );


    return table;

}

void SearchResultDialog::closeEvent(QCloseEvent *event)
{
    this->hide();
    event->ignore();



}


