#ifndef SEARCHRESULTDIALOG_H
#define SEARCHRESULTDIALOG_H

#include <QDialog>
#include "Book.h"
#include <QTableWidget>
#include <QLabel>
#include <QTableWidgetItem>
#include <QMouseEvent>

namespace Ui {
class SearchResultDialog;
}



class SearchResultDialog : public QWidget
{
    Q_OBJECT

public:
    explicit SearchResultDialog(QWidget *parent = nullptr);
    ~SearchResultDialog();

signals:
    void showBook(int book );

public slots:

    void setResults( QList<Book> &books , const QString & title);

    void rowDoubleClicked(int row, int column);

    void closeTab( int index );

    void closeAllTabs();

protected:

    QTableWidget * openTab( const QString &title );

    void closeEvent(QCloseEvent *event) override;

private:
    Ui::SearchResultDialog *ui;

    QTableWidget *m_table;

    QList<Book> m_books;


    int m_numTabs;
};

#endif // SEARCHRESULTDIALOG_H
