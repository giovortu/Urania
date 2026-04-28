#include "OwnCloudManager.h"
#include <QFileInfo>
#include <QDebug>
#include <QDir>
#include <QTimer>

OwnCloudManager::OwnCloudManager(QObject *parent) : QObject(parent)
{
    username = "giovortu";
    password = "3m1l71a_Rrx1140";
    serverUrl = "http://192.168.0.227:9980/remote.php/dav/files/giovortu";

    networkManager = new QNetworkAccessManager(this);

    // Manteniamo questo solo come fallback di emergenza se le credenziali cambiano
    connect(networkManager, &QNetworkAccessManager::authenticationRequired,
            this, &OwnCloudManager::onAuthenticationRequired);
}

OwnCloudManager::~OwnCloudManager()
{
    if (m_currentFile) {
        if (m_currentFile->isOpen()) m_currentFile->close();
        delete m_currentFile;
    }
}

void OwnCloudManager::enqueueUpload(const QString &filePath, const QString &destinationFolder)
{
    // Protezione anti-doppio click (Logic Layer)
    if (pendingUploads.contains(filePath)) {
        qDebug() << "SKIPPING: File già in lavorazione:" << filePath;
        return;
    }

    pendingUploads.insert(filePath);
    uploadQueue.enqueue(qMakePair(filePath, destinationFolder));

    if (m_status == tStatus::eIdle) {
        startNextUpload();
    }
}

void OwnCloudManager::enqueueDownload(const QString &destPath, const QString &remotePath)
{
    downloadQueue.enqueue(qMakePair(destPath, remotePath));
    if (m_status == tStatus::eIdle) {
        startNextDownload();
    }
}

void OwnCloudManager::onAuthenticationRequired(QNetworkReply *, QAuthenticator *authenticator)
{
    // Questo non dovrebbe PIÙ essere chiamato con la modifica sotto,
    // a meno che la password sia sbagliata.
    qDebug() << "WARNING: Fallback Authentication Triggered (Avoid if possible)";
    authenticator->setUser(username);
    authenticator->setPassword(password);
}

// ---------------------------------------------------------
// START UPLOAD (CORRETTO)
// ---------------------------------------------------------
void OwnCloudManager::startUpload(const QString &filePath, const QString &destinationFolder)
{
    m_currentFile = new QFile(filePath);
    if (!m_currentFile->open(QIODevice::ReadOnly)) {
        qDebug() << "Errore file:" << m_currentFile->errorString();
        delete m_currentFile;
        m_currentFile = nullptr;
        pendingUploads.remove(filePath);
        startNextUpload();
        return;
    }

    QFileInfo info(filePath);
    emit workStarted("Upload: " + info.fileName());

    QUrl url(serverUrl + destinationFolder + "/" + info.fileName());
    QNetworkRequest request(url);

    // =================================================================
    // FIX PROATTIVO: Inseriamo l'header PRIMA che il server lo chieda
    // =================================================================
    QString concatenated = username + ":" + password;
    QByteArray data = concatenated.toLocal8Bit().toBase64();
    QString headerData = "Basic " + data;
    request.setRawHeader("Authorization", headerData.toLocal8Bit());
    // =================================================================

    m_status = tStatus::eUploading;

    QNetworkReply *reply = networkManager->put(request, m_currentFile);
    m_currentFile->setParent(reply);

    connect(reply, &QNetworkReply::uploadProgress, this, &OwnCloudManager::uploadProgress);

    connect(reply, &QNetworkReply::finished, this, [this, reply, filePath]() {
        bool success = (reply->error() == QNetworkReply::NoError);

        if (success) {
            qDebug() << "Upload OK:" << filePath;
        } else {
            qDebug() << "Upload Error:" << reply->errorString();
        }

        pendingUploads.remove(filePath);
        emit workFinished(success, success ? "Upload OK" : reply->errorString());

        reply->deleteLater();
        m_currentFile = nullptr;
        m_status = tStatus::eIdle;
        QTimer::singleShot(0, this, &OwnCloudManager::startNextUpload);
    });
}

void OwnCloudManager::startNextUpload()
{
    if (m_status != tStatus::eIdle) return;

    if (uploadQueue.isEmpty()) {
        emit finished();
        return;
    }

    QPair<QString, QString> job = uploadQueue.dequeue();
    startUpload(job.first, job.second);
}

// ---------------------------------------------------------
// START DOWNLOAD (CORRETTO)
// ---------------------------------------------------------
void OwnCloudManager::startDownload(const QString &destPath, const QString &remotePath)
{
    QString tempPart = destPath + ".part";
    m_currentFile = new QFile(tempPart);

    if (!m_currentFile->open(QIODevice::WriteOnly)) {
        startNextDownload();
        return;
    }

    emit workStarted("Download: " + remotePath);

    QUrl url(serverUrl + remotePath);
    QNetworkRequest request(url);

    // =================================================================
    // FIX PROATTIVO ANCHE PER IL DOWNLOAD
    // =================================================================
    QString concatenated = username + ":" + password;
    QByteArray data = concatenated.toLocal8Bit().toBase64();
    QString headerData = "Basic " + data;
    request.setRawHeader("Authorization", headerData.toLocal8Bit());
    // =================================================================

    m_status = tStatus::eDownloading;

    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::downloadProgress, this, &OwnCloudManager::downloadProgress);
    connect(reply, &QNetworkReply::readyRead, this, [this, reply](){
         if(m_currentFile) m_currentFile->write(reply->readAll());
    });

    connect(reply, &QNetworkReply::finished, this, [this, reply, destPath]() {
        if (m_currentFile) m_currentFile->close();

        if (reply->error() == QNetworkReply::NoError) {
            if (QFile::exists(destPath)) QFile::remove(destPath);
            m_currentFile->rename(destPath);
             qDebug() << "Download OK";
        } else {
             qDebug() << "Download Error:" << reply->errorString();
             m_currentFile->remove();
        }

        delete m_currentFile;
        m_currentFile = nullptr;
        reply->deleteLater();

        m_status = tStatus::eIdle;
        QTimer::singleShot(0, this, &OwnCloudManager::startNextDownload);
    });
}

void OwnCloudManager::startNextDownload()
{
     if (m_status != tStatus::eIdle) return;
     if (downloadQueue.isEmpty()) {
         emit finished();
         return;
     }
     QPair<QString, QString> job = downloadQueue.dequeue();
     startDownload(job.first, job.second);
}
