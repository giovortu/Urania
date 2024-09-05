// ownclouduploader.h

#ifndef OWNCLOUDMANAGER_H
#define OWNCLOUDMANAGER_H

#include <QObject>
#include <QDebug>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QQueue>
#include <QAuthenticator>

class OwnCloudManager : public QObject
{
    Q_OBJECT

public:

    typedef enum tStatus
    {
        eIdle = -1,
        eUploading,
        eDownloading

    } tStatus;

    explicit OwnCloudManager(QObject *parent = nullptr);

    void enqueueUpload(const QString &filePath, const QString &destinationFolder);

    void enqueueDownload( const QString &destPath , const QString &remotePath );

signals:
    void workProgress(qint64 bytesSent, qint64 bytesTotal);
    void startingNext(const QString & file );

    void finished( );

private slots:
    void startNextUpload();
    void startNextDownload();
    void onWorkFinished(QNetworkReply *reply);


    void onAuthenticationRequired(QNetworkReply *, QAuthenticator *authenticator);

private:
    QString username;
    QString password;
    QString serverUrl;
    QQueue<QPair<QString, QString>> uploadQueue;
    QQueue<QPair<QString, QString>> downloadQueue;

    QNetworkAccessManager *networkManager;

    void startUpload(const QString &filePath, const QString &destinationFolder);
    void startDownload(const QString &filePath, const QString &destinationFolder);

    QFile *m_file = nullptr;

    tStatus m_status = eIdle;

};

#endif // OWNCLOUDMANAGER_H
