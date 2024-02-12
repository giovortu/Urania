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

    void setResults( QList<Book> &books );

    void rowDoubleClicked(int row, int column);



private:
    Ui::SearchResultDialog *ui;

    QTableWidget *m_table;

    QList<Book> m_books;
};

#endif // SEARCHRESULTDIALOG_H
