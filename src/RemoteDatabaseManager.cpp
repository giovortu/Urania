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


}

RemoteDatabaseManager::~RemoteDatabaseManager()
{
    delete ui;
}

void RemoteDatabaseManager::startUpload()
{

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
