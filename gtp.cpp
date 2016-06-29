#include "gtp.h"

GTP::GTP(QObject *parent) : QObject(parent)
{

}

void GTP::setEngine(GTPEngineProcess &engine){
    this->engine = &engine;
}

bool GTP::successful(QByteArray &reply){
    if(reply[0]=='='){
        reply.remove(0,2);
        return true;
    }else{
        qDebug() << reply;
        return false;
    }
}

/*
 * Reply is space separated string of verticies of each newline \n
 * separated dragon.
 */
QStringList GTP::dragon_stones(QString color){
    QStringList verticies;
    QByteArray reply = engine->write( QString("dragon_stones %1").arg(color));
    if(successful( reply )){
        QString tmp = QString(reply);
        verticies = tmp.split("\n", QString::SkipEmptyParts);
    }
    return verticies;
}

QStringList GTP::top_moves(QString color){
    bool ok;
    QStringList verticies;
    QByteArray reply = engine->write( QString("top_moves_%1").arg(color));
    if(successful( reply )){
        verticies = getVerticies(reply, ok);
        if(!ok) {
            qDebug() << "Error: top_moves_"<< color <<" "<<reply;
        }else{
            emit hints(color, verticies);
        }
    }
    return verticies;
}


int GTP::captures(QString color){
    bool ok;
    int captures=0;
    QByteArray reply = engine->write( QString("captures %1").arg(color));
    if(successful( reply )){
        captures = getInt(reply, ok);
        if(!ok) {
            qDebug() << "Error: captures " << color << " "<< reply;
        }else{
            emit captureCount(color, captures);
        }
    }
    return captures;
}

bool GTP::undo(int moves=1){
    bool ret = false;
    QByteArray reply;
    if(moves == 1){
        reply = engine->write( QString("undo").arg(moves));
    }else{
        reply = engine->write( QString("gg-undo %1").arg(moves));
    }
    if(successful( reply )){
        ret =  captures("black");
        ret &= captures("white");
        list_stones("black");
        list_stones("white");
    }
    return ret;
}

bool GTP::pass(QString color){
    bool ret = false;
    QByteArray reply = engine->write( QString("play %1 pass").arg(color));
    if(successful( reply )){
        ret=true;
    }
    return ret;
}


/*
 "0" (zero) for a draw (jigo)
"B+score" for a black win and
"W+score" for a white win, e.g. "B+2.5", "W+64" or "B+0.5"
"B+R"/"B+Resign" and "W+R"/"W+Resign" for a win by resignation.
You MUST NOT write "Black resigns" // assuming this means in file format?
*/
void GTP::final_score(){
    QByteArray reply = engine->write( QString("final_score"));
    QString white_score, black_score;
    if(successful( reply )){
        QRegularExpression re(commonREs.value("sgf_score"));
        QRegularExpressionMatch match = re.match(reply);
        QString  color, score;
        if (match.hasMatch()) {
            color = match.captured("color");
            score = match.captured("score");
            if(score.contains("R", Qt::CaseInsensitive)){
                //resigned
                if(color.contains("b", Qt::CaseInsensitive)){
                    black_score = "Resigned";
                    white_score = "Wins";
                }else{
                    white_score = "Resigned";
                    black_score = "Wins";
                }
            }else{
                if(color.contains("b", Qt::CaseInsensitive)){
                    black_score=QString("Wins: %1").arg(score);
                }else if(color.contains("w", Qt::CaseInsensitive)){
                    white_score = QString("Wins: %1").arg(score);
                }else if(color.contains("0")){
                    black_score="Draw (jigo)";
                    white_score="Draw (jigo)";
                }
            }
            emit blackScore(black_score);
            emit whiteScore(white_score);
        }
    }
}

    /* Can't figure out how to do resign. genmove can return it, but play can't send it.
bool GTP::resign(QString color){
    bool ret = false;
    final_score();
    QByteArray reply = engine->write( QString("play %1 resign").arg(color));
    if(successful( reply )){
        ret = true;
        final_score();
    }
    return ret;
}
    */

QStringList GTP::move_reasons(QString vertex){
    QStringList reasons;
    QByteArray reply = engine->write( QString("move_reasons %1").arg(vertex));
    if(successful( reply )){
        QString r = QString(reply);
        reasons = r.split("\n");
    }else{
        qDebug() << "move_reasons error: "<< vertex <<": "<<reply;
    }
    return reasons;
}

QStringList GTP::list_stones(QString color){
    bool ok;
        QStringList verticies;
    QByteArray reply = engine->write( QString("list_stones %1").arg(color));
    if(successful( reply )){
        verticies = getVerticies(reply, ok);
        if(!ok){
            qDebug() << "list_stones: "<< color <<" "<<reply;
        }else{
            emit stoneListing(color, verticies);
        }
    }
    return verticies;
}

int GTP::getInt(QByteArray reply, bool &found){
    QRegularExpression re(commonREs.value("int"));
    QRegularExpressionMatch match = re.match(reply);
    int the_int;
    if (match.hasMatch()) {
        the_int = match.captured("int").toInt();
        found=true;
    }else{
        found=false;
    }
    return the_int;
}

QString GTP::getVertex(QByteArray reply, bool &found){
    QRegularExpression re(commonREs.value("vertex"));
    QRegularExpressionMatch match = re.match(reply);
    QString vertex;
    if (match.hasMatch()) {
        vertex = match.captured("vertex");
        vertex = vertex.toLower();
        found=true;
    }else{
        found=false;
    }
    return vertex;
}

QStringList GTP::getVerticies(QByteArray reply, bool &found){
    QStringList verticies;
    QRegularExpression re(commonREs.value("verticies"));
    QRegularExpressionMatchIterator i = re.globalMatch(reply);
    while (i.hasNext()) {
        found = true;
        QRegularExpressionMatch match = i.next();
        verticies.append( match.captured(1).toLower());
    }
    return verticies;
}

QString GTP::genmove(QString color){
    bool ok;
    QString vertex;
    QByteArray reply =  engine->write(QString("genmove %1").arg(color));
    if( successful( reply)){
        vertex = getVertex(reply, ok);
        if(!ok) {
            QString something = QString(reply).toLower();
            if(something.contains("pass", Qt::CaseInsensitive)  ){
                vertex = "pass";
            }else if(something.contains("resign", Qt::CaseInsensitive)){
                vertex = "resign";
            }else{
                qDebug() << "Error: genmove " << color << " " << reply;
            }
        }else{
            emit move(color, vertex);
        }
    }
    return vertex;
}

    /*
QString GTP::genmove(QString color){
    bool ret = false;
    QByteArray reply =  engine->write(QString("genmove %1").arg(color));
    if( successful( reply)){
        QString vertex = getVertex(reply, ret);
        if(ret){
            resetPass(color);
            ui->gameBoard->placeStone(vertex, color);
            ui->textHistory->appendPlainText(QString("genmove %1 %2").arg(color).arg(vertex));
            captures(color);
        }else{
            QString something = QString(reply).toLower();
            if(something.contains("pass")  ){
                ui->textHistory->appendPlainText(QString("genmove %1 passed").arg(color));
                updatePass(color);
            }else if(something.contains("resign")){
                ui->textHistory->appendPlainText(QString("genmove %1 resigned").arg(color));
                final_score();
            }else{
                ui->textHistory->appendPlainText(QString("genmove WTF %1 %2").arg(color).arg(something));
            }
        }
    }
    return ret;
}
*/

bool GTP::play(QString color, QString vertex){
    QString cmd=QString("play %1 %2").arg(color).arg(vertex);
    bool ret=false;
    QByteArray reply = engine->write(cmd);
    if( successful(reply)){
        ret = true;
        emit move(color, vertex);
    }
    return ret;
}

QString GTP::new_score(){
    QString ret, white_score, black_score;
    QByteArray reply = engine->write( QString("new_score"));
    if(successful( reply )){
        ret = QString(reply);
        ret=ret.replace("upper bound:","U:");
        ret=ret.replace("lower:","L:");
        if(ret.contains("b", Qt::CaseInsensitive)){
            black_score = ret;
        }else{
            white_score = ret;
        }
        emit blackScore(black_score);
        emit whiteScore(white_score);
    }
    return ret;
}

QStringList GTP::fixed_handicap(int handicap){
    bool ok;
    QStringList verticies;
    QByteArray reply = engine->write( QString("fixed_handicap %1").arg(handicap));
    if(successful( reply )){
        verticies = getVerticies(reply, ok);
        if(!ok){
            qDebug() << "fixed_handicap: "<< handicap <<" "<<reply;
        }else{
            emit stoneListing("black", verticies);
        }
    }
    return verticies;
}

bool GTP::boardsize(int size){
    bool ret = false;
    QByteArray reply = engine->write( QString("boardsize %1").arg(size));
    if(successful( reply )){
        ret=true;
    }else{
        qDebug() << "Error boardsize: "<<size<< " "<< reply;
    }
    return ret;
}

bool GTP::komi(qreal komi){
    bool ret = false;
    QByteArray reply = engine->write( QString("komi %1").arg(komi));
    if(successful( reply )){
        ret=true;
    }else{
        qDebug() << "komi: "<<komi<< " "<< reply;
    }
    return ret;
}

bool GTP::printsgf(QString filename){
    //oddly, command is like: printsgf filename.sgf
    bool ret = false;
    QByteArray reply = engine->write( QString("printsgf %1").arg(filename));
    if(successful( reply )){
        ret=true;
    }else{
        qDebug() << "printsgf: "<<filename<<" "<< reply;
    }
    return ret;
}

bool GTP::loadsgf(QString filename, QString &color){
    //loadsgf ../build-GoGoGo-Qt_5_6_0-Debug/wtf.sgf
    bool ret = false;
    QByteArray reply = engine->write( QString("loadsgf %1").arg(filename));
    if(successful( reply )){
        QRegularExpression re(commonREs.value("word"));
        QRegularExpressionMatch match = re.match(reply);
        //IIRC, it actually returns a color, so probably should set proper turn
        if (match.hasMatch()) {
            color = match.captured("word");
            ret=true;
        }else{
            qDebug() << "loadsgf: "<< reply;
        }
    }
    return ret;
}
