#include "playerwrapper.h"

PlayerWrapper::PlayerWrapper(QObject *parent) : QObject(parent)
{
    white = new Player();
    black = new Player();
    current = white;
    next = black;
    last = black;
    readSettings();
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

QString PlayerWrapper::otherColor(QString color){
    return (color.toLower() == "black") ? "white":"black";
}

void  PlayerWrapper::resetPass(QString color){
    if(color == "black"){
        black->is_passing = false;
    }else{
        white->is_passing = false;
    }
}

void PlayerWrapper::updatePass(QString color){
    if(color == "black"){
        black->is_passing = true;
    }else{
        white->is_passing = true;
    }
    /*
    if(is_black_passing == true && is_white_passing==true){
        game_over = true;
        final_score();
    }
    */
}

void PlayerWrapper::readSettings()
{
    config.beginGroup("Players");
    black->setName(config.value("black_name").toString());
    white->setName(config.value("white_name").toString());
    black->setSpecies (config.value("black_species").toString());
    white->setSpecies(config.value("white_species").toString());
    white->setKomi(config.value("komi").toDouble());
    black->setHandicap(config.value("handicap").toInt());
    config.endGroup();
}
