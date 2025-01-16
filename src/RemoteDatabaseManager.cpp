#include "RemoteDatabaseManager.h"
#include "ui_RemoteDatabaseManager.h"

#include "OwnCloudManager.h"

#include <QDir>
#include <QFileInfo>

RemoteDatabaseManager::RemoteDatabaseManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RemoteDatabaseManager)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);

    setAttribute(Qt::WA_DeleteOnClose);

    connect( ui->pushButton, &QPushButton::clicked, this, &RemoteDatabaseManager::close );

    uploader = new OwnCloudManager( this );

    connect( uploader, &OwnCloudManager::finished, this, &RemoteDatabaseManager::close );

    connect( uploader, &OwnCloudManager::finished, this, &RemoteDatabaseManager::finished );

    getLastRemoteUpdate();


}

RemoteDatabaseManager::~RemoteDatabaseManager()
{
    delete ui;
}

void RemoteDatabaseManager::getLastRemoteUpdate()
{
    QObject::connect( uploader, &OwnCloudManager::workProgress, this, [=](int progress, int total) {

        qreal relative = (qreal)progress / (qreal)total * 100;

        ui->progressBar->setValue(relative);

    });

    QObject::connect( uploader, &OwnCloudManager::startingNext, this, [=]( const QString &str ) {
        qDebug() << "Starting next download: " << str;
        ui->label->setText("Downloading " + str + "...");

    });

    QObject::connect( uploader, &OwnCloudManager::finished, this, &RemoteDatabaseManager::lastUpdateReceived );


    QString path = QApplication::applicationDirPath() + "/database/lastupdatedown.txt";

    uploader->enqueueDownload(path, "GDRIVE/Urania/lastupdate.txt");
}

QDateTime RemoteDatabaseManager::getLastUpdate()
{
    QString lastUploadTime = QApplication::applicationDirPath() + "/database/lastupdate.txt";

    QDateTime dt = QDateTime::currentDateTime();

    QFile file( lastUploadTime );
    if ( file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file );
        quint64 t;

        stream >> t;

        dt = QDateTime::fromMSecsSinceEpoch( t );

        qWarning() << "Saved time: " << t;
        file.close();
    }

    return dt;
}

void RemoteDatabaseManager::startUpload()
{

    QString lastUploadTime = QApplication::applicationDirPath() + "/database/lastupdate.txt";

    QFile file( lastUploadTime );
    if ( file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file );
        QDateTime now = QDateTime::currentDateTime();

        quint64 t = now.toMSecsSinceEpoch();

        stream << t;

        qWarning() << "Writing last update time: " << t;
        file.close();
    }


    QObject::connect( uploader, &OwnCloudManager::workProgress, [=](int progress, int total) {

        qreal relative = (qreal)progress / (qreal)total * 100;

        ui->progressBar->setValue(relative);


    });


     QObject::connect( uploader, &OwnCloudManager::startingNext, [=]( const QString &str ) {
         qDebug() << "Starting next upload: " << str;
         ui->label->setText("Uploading " + str + "...");

     });


    QString path = QApplication::applicationDirPath() + "/database/";

    QDir dir(path);
    QFileInfoList list = dir.entryInfoList();


    foreach( QFileInfo info, list )
    {
        if (info.isFile())
        {
            QString fileName = path + info.fileName();
            qDebug() << "Queuing " << info.fileName();
            uploader->enqueueUpload(fileName, "/GDRIVE/Urania");
        }
    }
}

void RemoteDatabaseManager::startDownload()
{
    QObject::connect( uploader, &OwnCloudManager::workProgress, this, [=](int progress, int total) {

        qreal relative = (qreal)progress / (qreal)total * 100;

        ui->progressBar->setValue(relative);

    });

    QObject::connect( uploader, &OwnCloudManager::startingNext, this, [=]( const QString &str ) {
        qDebug() << "Starting next download: " << str;
        ui->label->setText("Downloading " + str + "...");

    });



    QString path = QApplication::applicationDirPath() + "/database/books.db";

    uploader->enqueueDownload(path, "GDRIVE/Urania/books.db");


}

void RemoteDatabaseManager::lastUpdateReceived()
{
    QString lastUploadTime = QApplication::applicationDirPath() + "/database/lastupdatedown.txt";

    QDateTime dt_remote = QDateTime::currentDateTime();

    QFile file( lastUploadTime );
    if ( file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file );
        quint64 t;

        stream >> t;

        dt_remote = QDateTime::fromMSecsSinceEpoch( t );

        qWarning() << "Saved time: " << t;
        file.close();
    }

    QDateTime dt_local = getLastUpdate();

    qWarning() << "Last update: " << dt_remote << "Current time: " << dt_local;

}

void RemoteDatabaseManager::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
