#include "mainwindow.h"
#include <QApplication>
#include <time.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qsrand( time(NULL) );
    a.setOrganizationName("Geek-Ranch");
    a.setOrganizationDomain("geek-ranch.org");
    a.setApplicationName("GoGoGo");
    a.setAttribute( Qt::AA_EnableHighDpiScaling, true);
    //QFile file(":/stylesheet.css");//not sure why broken
    //QFile file("/home/g/prog/GoGoGo/stylesheet.css");
    QFile file(":/stylesheet.css");
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        a.setStyleSheet(file.readAll());
        file.close();
    }else{
        qDebug() <<"Error opening stylesheet: " << file.errorString();
    }
    MainWindow w;
    w.show();

    w.connect(&a, SIGNAL(aboutToQuit()), &w, SLOT(on_actionQuit_triggered()));

    return a.exec();
}
