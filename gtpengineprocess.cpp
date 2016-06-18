#include "gtpengineprocess.h"

GTPEngineProcess::GTPEngineProcess(QObject *parent) : QObject(parent)
{
    connect(&process, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(showProcessError(QProcess::ProcessError)));
    connect(&process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)));
    connect(&process, SIGNAL(readyReadStandardError()), this, SLOT(readyReadStandardError()));
    connect(&process, SIGNAL(readyReadStandardOutput()), this, SLOT(readyReadStandardOutput()));
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


//use setReadChannel(stdout|stderr) to use read(), readLine(), or getChar()
void GTPEngineProcess::readyReadStandardOutput(){
    QByteArray got = process.readAllStandardOutput();
    QByteArray chunk;
    static QByteArray leftovers;
    got.push_front( leftovers );
    leftovers.clear();
    //qDebug() << "stdout got:" << got;
    while(!got.isEmpty()){
        int eqIdx = got.indexOf('=');
        int qIdx = got.indexOf('?');
        //  int endIdx = got.indexOf("\n\n");
        int respStartIdx=-1;
        int startIdx=0;
        int length=0;
        respStartIdx = eqIdx > qIdx ? eqIdx:qIdx;
        //qDebug() << "A respStartIdx:"<<respStartIdx<< " eqIdx:" << eqIdx << " qIdx:"<<qIdx;
        if(respStartIdx == -1){ //no start of response so is purely a continuation of last response
            startIdx = 0;
            length = got.length();
        }else if(respStartIdx > 0){ //begins with stuff from last time, but beginning of response follows
            startIdx =0;
            length = respStartIdx;
        }else{ //starts with = or ?
            startIdx =respStartIdx;
            eqIdx = got.indexOf('=', respStartIdx + 1);
            qIdx = got.indexOf('?', respStartIdx + 1);
            respStartIdx = eqIdx > qIdx ? eqIdx:qIdx;
            //  qDebug() << "B respStartIdx:"<<respStartIdx<< " eqIdx:" << eqIdx << " qIdx:"<<qIdx;
            if(respStartIdx == -1){//starts with = or ? and (is complete or continues in next read
                length = got.length();
            }else{//starts with = or ? and is followed by another  = or ? and
                length = respStartIdx ;
            }
        }
        chunk = got.mid(startIdx, length);
        got.remove(startIdx, length);

        if(chunk.isEmpty()){
            leftovers = got;
            return;
        }

        int response_count = chunk.count('=');
        response_count += chunk.count('?');
        //qDebug() << "response_count:"<< response_count;
        if(response_count > 1) {
            //hopefully don't have to worry about this
            //Nope, totally possible
            qDebug() << "Unhandled Number of Responses:"<<response_count;
        }else {
            if( chunk.at(chunk.length()-1) == 0xA && chunk.at(chunk.length()-2) == 0xA){
                //responses end in: \n\n so this is a completion
                exchanges.first().response.append(chunk);
                exchanges.first().pending = false;
            }else{
                exchanges.first().response.append(chunk);
                exchanges.first().pending = true;
            }
        }
        if(!exchanges.first().pending){
            //qDebug() << "Not pending so going to pop stack";
            Exchange e = exchanges.first();
            exchanges.pop_front();

            if( e.response[0] == '='){
                //   qDebug() << "Command was valid";
                e.success = true;
                processExchange(e);
            }else if(e.response[0] == '?'){
                qDebug() << "Command threw an error";
                e.success = false;
                processExchange(e);
            }else{
                qDebug() << "Shouldn't get here!!!!!!!!!!!!!!!!!!";
            }

        }
    }
}

void GTPEngineProcess::readyReadStandardError(){
    QByteArray got = process.readAllStandardError();
    qDebug() <<"stderr:" << got;
}

void GTPEngineProcess::write(QByteArray data){
    //data = "this is an error\n";
    //data = "genmove black\n";
    data = data.trimmed().append("\n");
    Exchange e;
    e.command = data;
    qint64 len_written = process.write(data);
    if( len_written == data.count()){
        //qDebug() << "Wrote:"<<data;
        exchanges.push_back(e);
    }else{
        qDebug() << "Error writing:" << data;
    }
}

void GTPEngineProcess::processExchange(Exchange e){
    //qDebug() << "Processing Exchange. There are " << exchanges.count() << " queued commands";
    //convert to qstring and do some cleanup before emitting
    QString response(e.response);
    QString command(e.command);
    bool success = e.success;
    response = response.remove(0,1).trimmed().toLower(); //git rid of = or ? and trim off white space
    command = command.trimmed().toLower();
    emit( gtpResponse(response, command, success) );
}


