#ifndef GTPENGINEPROCESS_H
#define GTPENGINEPROCESS_H

#include <QtCore>
#include <QObject>
#include <QProcess>
#include <QProcessEnvironment>

class GTPEngineProcess : public QObject
{
    Q_OBJECT
public:
    explicit GTPEngineProcess(QObject *parent = 0);
    QProcess process;

    void start();
    void stop();
    void setProgramPath(QString path);
    void addProgramArg(QString arg);
    void write(QByteArray data);

signals:

public slots:
    void started();
    void finished();
    //use setReadChannel(stdout|stderr) to use read(), readLine(), or getChar()
    void readyReadStandardOutput(); //readAllStandardOutput()
    void readyReadStandardError(); //readAllStandardError()



protected:
    QString programPath;
    QStringList programArgs;
};

#endif // GTPENGINEPROCESS_H
