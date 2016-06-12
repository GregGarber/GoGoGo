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
    is_running = true;
}

void GTPEngineProcess::finished(){
    qDebug() << "process finished";
    is_running = false;
}

        //use setReadChannel(stdout|stderr) to use read(), readLine(), or getChar()
void GTPEngineProcess::readyReadStandardOutput(){
    QByteArray got = process.readAllStandardOutput();
    QByteArray chunk;
    static QByteArray leftovers;
    got.push_front( leftovers );
    leftovers.clear();
    qDebug() << "stdout got:" << got;
    while(!got.isEmpty()){
        int eqIdx = got.indexOf('=');
        int qIdx = got.indexOf('?');
      //  int endIdx = got.indexOf("\n\n");
        int respStartIdx=-1;
        int startIdx=0;
        int length=0;
        respStartIdx = eqIdx > qIdx ? eqIdx:qIdx;
        qDebug() << "A respStartIdx:"<<respStartIdx<< " eqIdx:" << eqIdx << " qIdx:"<<qIdx;
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
                qDebug() << "B respStartIdx:"<<respStartIdx<< " eqIdx:" << eqIdx << " qIdx:"<<qIdx;
            if(respStartIdx == -1){//starts with = or ? and (is complete or continues in next read
                length = got.length();
            }else{//starts with = or ? and is followed by another  = or ? and
                length = respStartIdx ;
            }
        }
        chunk = got.mid(startIdx, length);
        got.remove(startIdx, length);


        qDebug() << "CHUNK:" << chunk;
        qDebug() << "GOT:" << got;
        //fuck, remove() doesn't return the removed part

        if(chunk.isEmpty()){
            leftovers = got;
            return;
        }

        int response_count = chunk.count('=');
        response_count += chunk.count('?');
        qDebug() << "response_count:"<< response_count;
        if(response_count > 1) {
            //hopefully don't have to worry about this
            //Nope, totally possible
            qDebug() << "Unhandled Number of Responses:"<<response_count;
        }else {
            //qDebug() << "Last two bytes are: " << chunk[chunk.length()]  << chunk[chunk.length()-1] ;
            qDebug() << "Last two bytes are: " << chunk.at(chunk.length()-1)  << chunk.at(chunk.length()-2) ;
            if( chunk.at(chunk.length()-1) == 0xA && chunk.at(chunk.length()-2) == 0xA){
                //responses end in: \n\n so this is a completion
                qDebug() << "Response completed";
                exchanges.first().response.append(chunk);
                exchanges.first().pending = false;
            }else{
                qDebug() << "Incomplete Response";
                exchanges.first().response.append(chunk);
                exchanges.first().pending = true;
            }
        }
        if(!exchanges.first().pending){
            qDebug() << "Not pending so going to pop stack";
            Exchange e = exchanges.first();
            exchanges.pop_front();

            if( e.response[0] == '='){
                qDebug() << "Command was valid";
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
/*
void GTPEngineProcess::readyReadStandardOutput(){
    QByteArray got = process.readAllStandardOutput();
    qDebug() << "stdout got:" << got;
    while(!got.isEmpty()){
        int response_count = got.count('=');
        response_count += got.count('?');
        qDebug() << "response_count:"<< response_count;
        if(response_count > 1) {
            //hopefully don't have to worry about this
            //Nope, totally possible
            qDebug() << "Unhandled Number of Responses:"<<response_count;
        }else {
            //qDebug() << "Last two bytes are: " << got[got.length()]  << got[got.length()-1] ;
            qDebug() << "Last two bytes are: " << got.at(got.length()-1)  << got.at(got.length()-2) ;
            if( got.at(got.length()-1) == 0xA && got.at(got.length()-2) == 0xA){
                //responses end in: \n\n so this is a completion
                qDebug() << "Response completed";
                exchanges.first().response.append(got);
                exchanges.first().pending = false;
            }else{
                qDebug() << "Incomplete Response";
                exchanges.first().response.append(got);
                exchanges.first().pending = true;
            }
        }
        if(!exchanges.first().pending){
            qDebug() << "Not pending so going to pop stack";
            Exchange e = exchanges.first();
            exchanges.pop_front();

            if( e.response[0] == '='){
                qDebug() << "Command was valid";
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
*/

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
        qDebug() << "Wrote:"<<data;
        exchanges.push_back(e);
    }else{
        qDebug() << "Error writing:" << data;
    }
}

void GTPEngineProcess::processExchange(Exchange e){
    qDebug() << "Processing Exchange. There are " << exchanges.count() << " queued commands";
    //convert to qstring and do some cleanup before emitting
    QString response(e.response);
    QString command(e.command);
    bool success = e.success;
    response = response.remove(0,1).trimmed().toLower(); //git rid of = or ? and trim off white space
    command = command.trimmed().toLower();
    emit( gtpResponse(response, command, success) );
}


