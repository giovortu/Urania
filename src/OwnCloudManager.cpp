// ownclouduploader.cpp

#include "OwnCloudManager.h"
#include <QFileInfo>
#include <QFile>

OwnCloudManager::OwnCloudManager(QObject *parent) : QObject(parent)
{
    // Set your OwnCloud credentials here
    username = "giovortu";
    password = "3m1l71a_thx1138";
    serverUrl = "https://casaortu.duckdns.org:18000/remote.php/dav/files/giovortu/";

    networkManager = new QNetworkAccessManager(this);

    connect(networkManager, &QNetworkAccessManager::finished, this, &OwnCloudManager::onWorkFinished);
    connect(networkManager, &QNetworkAccessManager::authenticationRequired, this, &OwnCloudManager::onAuthenticationRequired);


}



void OwnCloudManager::enqueueUpload(const QString &filePath, const QString &destinationFolder)
{
    uploadQueue.enqueue(qMakePair(filePath, destinationFolder));
    if ( m_status == tStatus::eIdle )
    {
        startNextUpload();
    }
}

void OwnCloudManager::onAuthenticationRequired(QNetworkReply *, QAuthenticator *authenticator)
{
    authenticator->setUser( username);
    authenticator->setPassword( password );
}

void OwnCloudManager::enqueueDownload(const QString &destPath, const QString &remotePath)
{
    downloadQueue.enqueue(qMakePair(destPath, remotePath));
    if ( m_status == tStatus::eIdle )
    {
        startNextDownload();
    }

}





void OwnCloudManager::startNextUpload()
{
    if ( !uploadQueue.isEmpty()  )
    {
        QPair<QString, QString> upload = uploadQueue.dequeue();
        startUpload(upload.first, upload.second);
    }
    else
    {
        emit finished();
    }
}

void OwnCloudManager::startNextDownload()
{

    if ( !downloadQueue.isEmpty()  )
    {
        QPair<QString, QString> upload = downloadQueue.dequeue();
        startDownload(upload.first, upload.second);
    }
    else
    {
        emit finished();
    }

}

void OwnCloudManager::startUpload(const QString &filePath, const QString &destinationFolder)
{

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file for reading:" << file.errorString();
        return;
    }

    QFileInfo info( filePath );

    emit startingNext( info.fileName() );



    QByteArray fileData = file.readAll();

    QUrl url(serverUrl + destinationFolder + "/" + QFileInfo(filePath).fileName());
    QNetworkRequest request(url);

    m_status = tStatus::eUploading;

    QNetworkReply *reply =  networkManager->put(request, fileData);
    connect(reply, &QNetworkReply::uploadProgress,this, &OwnCloudManager::workProgress);

}

void OwnCloudManager::startDownload(const QString &destPath, const QString &remotePath)
{
    QString url = serverUrl +  remotePath ;

    QFileInfo info( destPath );

    emit startingNext( info.fileName() );
    qWarning() << url;

    QNetworkRequest request( url );

    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::downloadProgress,this, &OwnCloudManager::workProgress);

    if ( nullptr != m_file )
    {
        m_file->close();
        delete m_file;
    }

    QString fileName = QString::number(QDateTime::currentMSecsSinceEpoch()) + ".tmp";

    m_file = new QFile( fileName );
    if ( m_file->exists())
    {
        m_file->remove();
    }

    if ( !m_file->open(QIODevice::Append) )
    {
        qDebug() << "Failed to open file for writing:" << m_file->errorString();
        reply->abort();
        return;
    }

    m_status = tStatus::eDownloading;

    connect( reply, &QNetworkReply::readyRead, this, [=]() {

        m_file->write(reply->readAll());

    });

    connect(reply, &QNetworkReply::finished, this, [=](){
        if ( nullptr != m_file )
        {
            QFile f(destPath);
            if ( f.exists())
            {
                f.remove();
            }
            m_file->rename( destPath );
        }



    });

    connect(reply, &QNetworkReply::errorOccurred, this, [=](QNetworkReply::NetworkError err) {
        if ( nullptr != m_file )
        {
            m_file->close();
            qDebug() << "Download error " << err;
        }

    });
}

void OwnCloudManager::onWorkFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        qDebug() << "File" << ((m_status == eUploading)?"upload":"download") << "successfully.";
    }
    else
    {
        qDebug() <<  ((m_status == eUploading)?"Upload":"Download" ) << "failed:" << reply->errorString();
    }

    tStatus oldStatus = m_status;
    m_status = tStatus::eIdle;

    reply->deleteLater();
    if ( oldStatus == eUploading )
    {
        startNextUpload();
    }
    else if ( oldStatus == eDownloading )
    {
        startNextDownload();
    }

}




