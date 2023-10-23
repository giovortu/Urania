
#include "src/MainWindow.h"

#include <QFile>
#include <QApplication>

#include "Library.h"

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

    auto lib = new Library("uraniaspeciali.db", nullptr);

    lib->populateDatabase( "C:/Users/giovortu/Downloads/Urania/www.mondourania.com/speciali/speciali", "speciali" );

#endif

    return a.exec();

}

