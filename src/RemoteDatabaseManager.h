#ifndef RemoteDatabaseManager_H
#define RemoteDatabaseManager_H

#include <QWidget>
class OwnCloudManager;

namespace Ui {
class RemoteDatabaseManager;
}

class RemoteDatabaseManager : public QWidget
{
    Q_OBJECT

public:
    explicit RemoteDatabaseManager(QWidget *parent = nullptr);
    ~RemoteDatabaseManager();

signals:

    void finished( );

public slots:

    void startUpload();
    void startDownload();

protected:
    void changeEvent(QEvent *e);

    OwnCloudManager *uploader;

private:
    Ui::RemoteDatabaseManager *ui;
};

#endif // RemoteDatabaseManager_H
