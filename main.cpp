
#include "src/MainWindow.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QApplication>
#include <QSplashScreen>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(qdarkstyle);
    Q_INIT_RESOURCE(img);

    QApplication a(argc, argv);


    auto splash = new QSplashScreen(QPixmap( ":/images/splash.png") );

    splash->showMessage("Starting...", Qt::AlignBottom | Qt::AlignHCenter, Qt::white);
    splash->show();


    auto mainWin  = new MainWindow;

    QObject::connect( mainWin, &MainWindow::ready, splash, &QSplashScreen::close);

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

    mainWin->init();

    mainWin->show();

    return a.exec();

}

