
#include "src/MainWindow.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QApplication>

#include "Library.h"
#include "OwnCloudUploader.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(qdarkstyle);
    Q_INIT_RESOURCE(img);

    QApplication a(argc, argv);

#if 1

    MainWindow w;

    QFile f(":/qdarkstyle/style.css");


    if (!f.exists())
    {

    }
    else
    {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
        f.close();
    }


    w.show();
#else

    OwnCloudUploader uploader;

    QObject::connect( &uploader, &OwnCloudUploader::uploadProgress, [](int progress, int total) {
        qDebug() << "Progress: " << progress << " / " << total;
    });


    QString path = QApplication::applicationDirPath() + "/database/";

    QDir dir(path);
    QFileInfoList list = dir.entryInfoList();

    foreach( QFileInfo info, list )
    {
        if (info.isFile())
        {
            QString fileName = path + info.fileName();
            qDebug() << "Uploading " << info.fileName();
            uploader.enqueueUpload(fileName, "/Urania");
        }
    }



#endif

    return a.exec();

}

