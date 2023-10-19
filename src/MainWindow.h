
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QNetworkAccessManager>

#include "Book.h"
class Library;
class QLineEdit;

class StarRating;


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:

    void viewBook( Book & book );
    void onNext(  );
    void onPrevious(  );

    void onCurrentChanged();

    void onOwnedChanged( bool );

    void onRatingChanged(qreal rating);

    void onImportFromFile( );
    void onImportFromWeb( );



private:
    int m_currentBook = 1;

    QNetworkAccessManager m_manager;

    QLineEdit *m_current;

    Library *m_library;

    StarRating *m_starRating;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
