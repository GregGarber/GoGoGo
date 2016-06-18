#ifndef GTPENGINEPROCESS_H
#define GTPENGINEPROCESS_H

#include <QtCore>
#include <QObject>
#include <QProcess>
#include <QProcessEnvironment>
#include <QList>

class GTPEngineProcess : public QObject
{
    Q_OBJECT
public:
    explicit GTPEngineProcess(QObject *parent = 0);
    QProcess process;
    bool is_running = false;

    void start();
    void stop();
    void setProgramPath(QString path);
    void addProgramArg(QString arg);
    void write(QByteArray data);
    QByteArray communicate(QByteArray data);
signals:
    void gtpResponse(QString response, QString command, bool success);

public slots:
    void showProcessError(QProcess::ProcessError err);
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void readyReadStandardError(); //readAllStandardError()
    void readyReadStandardOutput(); //readAllStandardOutput()
    void started();
    void	stateChanged(QProcess::ProcessState newState);
    //use setReadChannel(stdout|stderr) to use read(), readLine(), or getChar()



protected:
    QString programPath;
    QStringList programArgs;
    struct Exchange{
        bool pending = true;
        bool success = false;
        int expected_length = 2; //expected number of bytes in response
        QByteArray command;
        QByteArray response;
    };
    QList<Exchange> exchanges;
    void processExchange(Exchange e);
};
#endif // GTPENGINEPROCESS_H
