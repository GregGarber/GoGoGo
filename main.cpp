#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //QFile file(":/stylesheet.css");//not sure why broken
    QFile file("/home/g/prog/GoGoGo/stylesheet.css");
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        a.setStyleSheet(file.readAll());
        file.close();
    }else{
        qDebug() <<"Error opening stylesheet: " << file.errorString();
    }
    MainWindow w;
    w.show();

    return a.exec();
}
