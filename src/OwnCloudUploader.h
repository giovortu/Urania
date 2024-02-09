// ownclouduploader.h

#ifndef OWNCLOUDUPLOADER_H
#define OWNCLOUDUPLOADER_H

#include <QObject>
#include <QDebug>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QQueue>

class OwnCloudUploader : public QObject
{
    Q_OBJECT

public:
    explicit OwnCloudUploader(QObject *parent = nullptr);

    void enqueueUpload(const QString &filePath, const QString &destinationFolder);

signals:
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void startingNextUpload(const QString & file );

    void finished();

private slots:
    void startNextUpload();
    void onUploadFinished(QNetworkReply *reply);
    void onUploadProgress(qint64 bytesSent, qint64 bytesTotal);

private:
    QString username;
    QString password;
    QString serverUrl;
    QQueue<QPair<QString, QString>> uploadQueue;
    bool isUploading = false;
    QNetworkAccessManager *networkManager;

    void startUpload(const QString &filePath, const QString &destinationFolder);

};

#endif // OWNCLOUDUPLOADER_H
