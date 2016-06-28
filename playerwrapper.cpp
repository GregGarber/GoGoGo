#include "playerwrapper.h"

PlayerWrapper::PlayerWrapper(QObject *parent) : QObject(parent)
{
    white = new Player();
    black = new Player();
    white->setColor(White);
    black->setColor(Black);
    current = white;
    next = black;
    last = black;
    readSettings();
}

PlayerWrapper::~PlayerWrapper(){
    delete black;
    delete white;
}

Player* PlayerWrapper::getBlack(){
    return black;
}

Player* PlayerWrapper::getWhite(){
    return white;
}

Player* PlayerWrapper::getCurrent(){
    return current;
}

Player* PlayerWrapper::getNext(){
    return next;
}

Player* PlayerWrapper::getLast(){
    return last;
}

void PlayerWrapper::setHandicap(int h){
    black->setHandicap(h);
    if(h>1){
        setCurrentPlayer(White);
    }else{
        setCurrentPlayer(Black);
    }
}

void PlayerWrapper::setCurrentPlayer(PlayerColors player){
    if( current->getPlayerColor() == player ) return;
    if(player == Black){
        current = black;
        next = white;
        last = white;
    }else{
        current = white;
        next = black;
        last = black;
    }
}

void PlayerWrapper::swap(){
    if( current->getPlayerColor() == Black){
        current = white;
        next = black;
        last = black;
    }else{
        current = black;
        next = white;
        last =white;
    }
}

void PlayerWrapper::setCaptures(int captures){
    current->setCaptures(captures);
}

void PlayerWrapper::setCaptures(QString color, int captures){
    if(color == "black"){
        black->setCaptures(captures);
    }else{
        white->setCaptures(captures);
    }
}

void PlayerWrapper::setCurrentPass(){
    getCurrent()->setIsPassing(true);
    if(getCurrent()->getPlayerColor()==Black){
        emit blackScore("Passed");
    }else{
        emit whiteScore("Passed");
    }
}

void PlayerWrapper::setCurrentPlays(){
    getCurrent()->setIsPassing(false);
    getCurrent()->setResigned(false);
}

bool PlayerWrapper::getMutalPass(){
    return (getCurrent()->getIsPassing() && getNext()->getIsPassing());
}


QString PlayerWrapper::getGameOver(){
        //FIXME sort of guessing at these conditions
    QString ret;
        if(getMutalPass()){
            ret = "Mutal Pass";
        }else if(getCurrent()->getResigned() && getLast()->getResigned()){
            ret = "Mutal Resign";
        }else if(getCurrent()->getIsPassing() && getLast()->getResigned()){
            ret = QString("%1 Passed and %2 Resigned").arg(getCurrent()->getColorString()).arg(getLast()->getColorString());
        }
    return ret;
}

void PlayerWrapper::setCurrentResigned(){
    getCurrent()->setResigned(true);
    if(getCurrent()->getPlayerColor()==Black){
        emit blackScore("Resigned");
    }else{
        emit whiteScore("Resigned");
    }
}

void PlayerWrapper::readSettings()
{
    config.beginGroup("Players");
    black->setName(config.value("black_name").toString());
    black->setSpecies (config.value("black_species").toString());
    black->setHandicap(config.value("handicap").toInt());
    white->setName(config.value("white_name").toString());
    white->setSpecies(config.value("white_species").toString());
    white->setKomi(config.value("komi").toDouble());
    config.endGroup();
}
