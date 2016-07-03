#include "gtpengineprocess.h"

GTPEngineProcess::GTPEngineProcess(QObject *parent) : QObject(parent)
{
    connect(&process, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(showProcessError(QProcess::ProcessError)));
    connect(&process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)));
    connect(&process, SIGNAL(readyReadStandardError()), this, SLOT(readyReadStandardError()));
    connect(&process, SIGNAL(started()), this, SLOT(started()));
    connect(&process, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(stateChanged(QProcess::ProcessState)));
}

void GTPEngineProcess::stateChanged(QProcess::ProcessState newState){
    switch(newState){
    case QProcess::NotRunning:
        qDebug() << process.program() << " State Changed to NotRunning";
        break;
    case QProcess::Starting:
        qDebug() << process.program() << " PID:" << process.processId() << " State Changed to Starting";
        break;
    case QProcess::Running:
        qDebug() << process.program() << " PID:" << process.processId() << " State Changed to Running";
        break;
    }
}

void GTPEngineProcess::start(){
    process.start(programPath, programArgs);
    // /usr/local/gnugo/bin/gnugo --mode gtp --level 1  --depth 1
    //process.start("/usr/local/gnugo/bin/gnugo", QStringList() << "--mode"<<"gtp");
}

void GTPEngineProcess::started(){
    qDebug() << "process started";
    is_running = true;
}

void GTPEngineProcess::stop(){
    if(is_running){
        write("quit");
    }else{
        process.close();
    }
    if(!process.waitForFinished()){
        qDebug() << "Sending terminate to engine";
        emit process.terminate();
    }
}

void GTPEngineProcess::finished(int exitCode, QProcess::ExitStatus exitStatus){
    qDebug() << "process finished: exitCode:" << exitCode<<" exitStatus:"<<exitStatus;
    if(exitStatus == QProcess::CrashExit){
        qDebug() << "Engine Crashed!";
        showProcessError(process.error());
    }else{
        if(exitCode!=0){
            qDebug() << "exitCode:"<<exitCode<<" is abnormal";
        }
    }
    is_running = false;
}

void GTPEngineProcess::showProcessError(QProcess::ProcessError err){
    qDebug() << "Error in process '"<<process.program()<<"' PID:"<< process.processId() << " ERROR:" <<process.errorString();
    switch(err){
    case QProcess::FailedToStart:
        qDebug() << "The process failed to start. Either the invoked program is missing, or you may have insufficient permissions to invoke the program.";
        break;
    case QProcess::Crashed:
        qDebug() << "The process crashed some time after starting successfully.";
        break;
    case QProcess::Timedout:
        qDebug() << "The last waitFor...() function timed out. The state of QProcess is unchanged, and you can try calling waitFor...() again.";
        break;
    case QProcess::WriteError:
        qDebug() << "An error occurred when attempting to write to the process. For example, the process may not be running, or it may have closed its input channel.";
        break;
    case QProcess::ReadError:
        qDebug() << "An error occurred when attempting to read from the process. For example, the process may not be running.";
        break;
    case QProcess::UnknownError:
        qDebug() << "An unknown error occurred. This is the default return value of error().";
        break;
    }
}

void GTPEngineProcess::setProgramPath(QString path){
    programPath = path;
}

void GTPEngineProcess::addProgramArg(QString arg){
    programArgs << arg.split(" ", QString::SkipEmptyParts);
}


void GTPEngineProcess::readyReadStandardError(){
    QByteArray got = process.readAllStandardError();
    qDebug() <<"stderr:" << got;
}

//use setReadChannel(stdout|stderr) to use read(), readLine(), or getChar()

QByteArray GTPEngineProcess::write(const char *data){
    return write(QByteArray(data));
}

QByteArray GTPEngineProcess::write(QString data){
    return write(data.toLatin1());
}

QByteArray GTPEngineProcess::write(QByteArray data){
    qDebug() << "write:"<< data;
    QByteArray buffer;
    data = data.trimmed().append("\n");
    process.write(data);
    if(!process.waitForBytesWritten(1000*60)){
        qDebug() << "Error writing ";
    }
    //some commands take a long time...
    while( process.waitForReadyRead(1000*60)){
        buffer.append(process.readAll());
        if( buffer.contains("\n\n")) break;
    }
    qDebug() << "read:"<< buffer;
    return buffer;
}
