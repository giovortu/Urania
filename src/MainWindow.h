
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QNetworkAccessManager>

#include "Book.h"
class Library;
class QLineEdit;
class QComboBox;
class StarRating;
class QSettings;
class QLabel;


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

    void onSettings();

    void viewBook( Book & book );
    void onNext(  );
    void onPrevious(  );

    void toggleOwned();
    void toggleRead();

    void onNextTen();
    void onPreviuosTen();

    void goFirst();
    void goLast();

    void onCurrentChanged();

    void onOwnedChanged( bool );

    void onReadChanged( bool );

    void onRatingChanged(qreal rating);
    void onSaveComments();

    void onImportFromFile( );
    void onImportFromWeb( );

    void loadBook( int index );

    void loadBookById( int id );

    void onSearch();

    void onStatistics();

    void onNewBook();

    void onUpload();

    void readSettings();

    void writeSettings();

    void onCollanaChanged( const QString &txt );

    void initLibrary();

    void updateView();

    void onBookInfo();

protected:
    virtual void closeEvent(QCloseEvent *event) override;

private:
    int m_currentBookNumber = 1;

    QString m_currentDatabase;

    QNetworkAccessManager m_manager;

    QLineEdit *m_current;
    QLineEdit *m_search;

    QComboBox *m_searchType;
    QComboBox *m_collana;
    Library *m_library;

    StarRating *m_starRating;

    Ui::MainWindow *ui;

    QSettings *m_settings;

    bool m_safeExit = false;

    QString m_nomeCollana;

    QLabel *m_totalBooks;

    Book m_currentBook;
};

#endif // MAINWINDOW_H
