#include "SearchResultDialog.h"
#include "ui_SearchResultDialog.h"

#include <QHeaderView>
#include <QTableWidget>

SearchResultDialog::SearchResultDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchResultDialog)
{
    ui->setupUi(this);

    m_table = new QTableWidget( this );
    ui->mainLayout->addWidget( m_table );
    m_table->setColumnCount(6);
    QStringList headers = {"Number", "Title (ITA)", "Title (Original)", "Author", "Date Published", "Cover Author"};
    m_table->setHorizontalHeaderLabels(headers);

    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);


    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);

    connect( ui->buttonBox, &QDialogButtonBox::rejected, this, &SearchResultDialog::close );

    connect( m_table, &QTableWidget::cellDoubleClicked, this, &SearchResultDialog::rowDoubleClicked );

    for (int col = 0; col < m_table->columnCount()-1; ++col) {
        m_table->horizontalHeader()->setSectionResizeMode(col, QHeaderView::ResizeToContents);
    }


    m_table->horizontalHeader()->setSectionResizeMode(m_table->columnCount()-1, QHeaderView::Stretch);


}

SearchResultDialog::~SearchResultDialog()
{
    delete ui;
}

void SearchResultDialog::setResults(QList<Book> &books)
{
        m_table->setRowCount(books.size());

        for (int row = 0; row < books.size(); ++row)
        {
            QTableWidgetItem *item;
            const Book &book = books.at(row);

            // Number
            item = new QTableWidgetItem(QString::number(book.number));
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            m_table->setItem(row, 0, item);

            // Title (ITA)
            item = new QTableWidgetItem(book.title_ita);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            m_table->setItem(row, 1, item);

            // Title (Original)
            item = new QTableWidgetItem(book.title_orig);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            m_table->setItem(row, 2, item);

            // Author
            item = new QTableWidgetItem(book.author);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            m_table->setItem(row, 3, item);

            // Date Published
            item = new QTableWidgetItem(book.date_pub.toString("yyyy-MM-dd"));
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            m_table->setItem(row, 4, item);

            // Cover Author
            item = new QTableWidgetItem(book.cover_author);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            m_table->setItem(row, 5, item);


        }

}

void SearchResultDialog::rowDoubleClicked(int row, int column)
{
   int bookNumber = m_table->item(row, 0)->text().toInt();
   emit showBook( bookNumber );
}


