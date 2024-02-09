#ifndef DATABASEUPLOADER_H
#define DATABASEUPLOADER_H

#include <QWidget>
class OwnCloudUploader;

namespace Ui {
class DatabaseUploader;
}

class DatabaseUploader : public QWidget
{
    Q_OBJECT

public:
    explicit DatabaseUploader(QWidget *parent = nullptr);
    ~DatabaseUploader();

public slots:

    void startUpload();

protected:
    void changeEvent(QEvent *e);

    OwnCloudUploader *uploader;

private:
    Ui::DatabaseUploader *ui;
};

#endif // DATABASEUPLOADER_H
