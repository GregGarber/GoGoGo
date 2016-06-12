#include "gtpengineprocess.h"

GTPEngineProcess::GTPEngineProcess(QObject *parent) : QObject(parent)
{

    connect(&process, SIGNAL(started()), this, SLOT(started()));
    connect(&process, SIGNAL(finished(int)), this, SLOT(finished()));
    connect(&process, SIGNAL(readyReadStandardOutput()), this, SLOT(readyReadStandardOutput()));
    connect(&process, SIGNAL(readyReadStandardError()), this, SLOT(readyReadStandardError()));
    //connect(&process, SIGNAL(started()), this, SLOT(started()));
}

void GTPEngineProcess::start(){
    process.start(programPath, programArgs);
     // /usr/local/gnugo/bin/gnugo --mode gtp --level 1  --depth 1
    //process.start("/usr/local/gnugo/bin/gnugo", QStringList() << "--mode"<<"gtp");
}

void GTPEngineProcess::stop(){
    process.terminate();
}

void GTPEngineProcess::setProgramPath(QString path){
    programPath = path;
}

void GTPEngineProcess::addProgramArg(QString arg){
    programArgs << arg.split(" ", QString::SkipEmptyParts);
    //programArgs << arg;
}

void GTPEngineProcess::started(){
    qDebug() << "process started";
}

void GTPEngineProcess::finished(){
    qDebug() << "process finished";
}

        //use setReadChannel(stdout|stderr) to use read(), readLine(), or getChar()
void GTPEngineProcess::readyReadStandardOutput(){
    QByteArray got = process.readAllStandardOutput();
    qDebug() << "stdout got:" << got;
}

void GTPEngineProcess::readyReadStandardError(){
    QByteArray got = process.readAllStandardError();
    qDebug() <<"stderr:" << got;
}

void GTPEngineProcess::write(QByteArray data){
    qDebug() << "writing:"<<data; //need const char*
    process.write(data);
    //works process.write("help\n");
}
