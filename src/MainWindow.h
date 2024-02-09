
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

    void onImportFromFile( );
    void onImportFromWeb( );

    void loadBook( int index );

    void onSearch();

    void onStatistics();

    void onNewBook();

    void onUpload();

    void readSettings();

    void writeSettings();

    void onDatabaseChanged( const QString &txt );

    void initLibrary();

    void onBookInfo();

protected:
    virtual void closeEvent(QCloseEvent *event) override;

private:
    int m_currentBook = 1;

    QString m_currentDatabase;

    QNetworkAccessManager m_manager;

    QLineEdit *m_current;
    QLineEdit *m_search;

    QComboBox *m_searchType;
    QComboBox *m_database;
    Library *m_library;

    StarRating *m_starRating;

    Ui::MainWindow *ui;

    QSettings *m_settings;

    bool m_safeExit = false;
};

#endif // MAINWINDOW_H
