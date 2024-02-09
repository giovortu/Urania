#include "DatabaseUploader.h"
#include "ui_DatabaseUploader.h"

#include "OwnCloudUploader.h"

#include <QDir>
#include <QFileInfo>

DatabaseUploader::DatabaseUploader(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DatabaseUploader)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);

    setAttribute(Qt::WA_DeleteOnClose);

    connect( ui->pushButton, &QPushButton::clicked, this, &DatabaseUploader::close );

    uploader = new OwnCloudUploader( this );

    connect( uploader, &OwnCloudUploader::finished, this, &DatabaseUploader::close );
}

DatabaseUploader::~DatabaseUploader()
{
    delete ui;
}

void DatabaseUploader::startUpload()
{


    QObject::connect( uploader, &OwnCloudUploader::uploadProgress, [=](int progress, int total) {

        qreal relative = (qreal)progress / (qreal)total * 100;

        ui->progressBar->setValue(relative);


    });


     QObject::connect( uploader, &OwnCloudUploader::startingNextUpload, [=]( const QString &str ) {
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

void DatabaseUploader::changeEvent(QEvent *e)
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
