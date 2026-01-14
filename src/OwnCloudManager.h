#ifndef OWNCLOUDMANAGER_H
#define OWNCLOUDMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QQueue>
#include <QPair>
#include <QFile>
#include <QAuthenticator>
#include <QSet> // Usato per tracciamento rapido

class OwnCloudManager : public QObject
{
    Q_OBJECT
public:
    explicit OwnCloudManager(QObject *parent = nullptr);
    ~OwnCloudManager();

    void enqueueUpload(const QString &filePath, const QString &destinationFolder);
    void enqueueDownload(const QString &destPath, const QString &remotePath);

signals:
    void workStarted(const QString &fileName);
    void workFinished(bool success, const QString &message);
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private slots:
    void onAuthenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator);

private:
    void startNextUpload();
    void startNextDownload();
    void startUpload(const QString &filePath, const QString &destinationFolder);
    void startDownload(const QString &destPath, const QString &remotePath);

    enum tStatus {
        eIdle,
        eUploading,
        eDownloading
    };

    QNetworkAccessManager *networkManager;
    QString username;
    QString password;
    QString serverUrl;

    tStatus m_status = tStatus::eIdle;

    QQueue<QPair<QString, QString>> uploadQueue;
    QQueue<QPair<QString, QString>> downloadQueue;

    // TRACCIAMENTO DUPLICATI:
    // Teniamo traccia dei file attualmente in coda o in lavorazione per evitare doppi inserimenti
    QSet<QString> pendingUploads;

    QFile *m_currentFile = nullptr;
};

#endif // OWNCLOUDMANAGER_H
