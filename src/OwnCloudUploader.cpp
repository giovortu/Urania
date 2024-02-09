// ownclouduploader.cpp

#include "OwnCloudUploader.h"
#include <QFileInfo>

OwnCloudUploader::OwnCloudUploader(QObject *parent) : QObject(parent)
{
    // Set your OwnCloud credentials here
    username = "giovortu";
    password = "3m1l71a_thx1138";
    serverUrl = "https://casaortu.duckdns.org:18000/remote.php/dav/files/giovortu/";

    networkManager = new QNetworkAccessManager(this);

    connect(networkManager, &QNetworkAccessManager::finished, this, &OwnCloudUploader::onUploadFinished);

}



void OwnCloudUploader::enqueueUpload(const QString &filePath, const QString &destinationFolder)
{
    uploadQueue.enqueue(qMakePair(filePath, destinationFolder));
    if (!isUploading)
    {
        startNextUpload();
    }
}

void OwnCloudUploader::startNextUpload()
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

void OwnCloudUploader::startUpload(const QString &filePath, const QString &destinationFolder)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file for reading:" << file.errorString();
        return;
    }

    QFileInfo info( filePath );

    emit startingNextUpload( info.fileName() );


    QByteArray fileData = file.readAll();

    QUrl url(serverUrl + destinationFolder + "/" + QFileInfo(filePath).fileName());
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", "Basic " + QByteArray(QString("%1:%2").arg(username).arg(password).toUtf8()).toBase64());
    request.setRawHeader("Content-Type", "application/octet-stream");

    isUploading = true;
    QNetworkReply *reply =  networkManager->put(request, fileData);
    connect(reply, &QNetworkReply::uploadProgress,this, &OwnCloudUploader::onUploadProgress);

}

void OwnCloudUploader::onUploadFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        qDebug() << "File uploaded successfully.";
    }
    else
    {
        qDebug() << "Upload failed:" << reply->errorString();
    }

    isUploading = false;
    reply->deleteLater();
    startNextUpload();
}

void OwnCloudUploader::onUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    emit uploadProgress(bytesSent, bytesTotal);
}
