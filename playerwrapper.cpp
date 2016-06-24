#include "playerwrapper.h"

PlayerWrapper::PlayerWrapper(QObject *parent) : QObject(parent)
{

    readSettings();
}

Player* PlayerWrapper::black(){
    return black;
}

Player* PlayerWrapper::white(){
    return white;
}

Player* PlayerWrapper::current(){
    return current;
}

Player* PlayerWrapper::next(){
    return next;
}

Player* PlayerWrapper::last(){
    return last;
}

QString PlayerWrapper::otherColor(QString color){
    return (color.toLower() == "black") ? "white":"black";
}

void  PlayerWrapper::resetPass(QString color){
    if(color == "black"){
        is_black_passing = false;
    }else{
        is_white_passing = false;
    }
}

void PlayerWrapper::updatePass(QString color){
    if(color == "black"){
        is_black_passing = true;
    }else{
        is_white_passing = true;
    }
    if(is_black_passing == true && is_white_passing==true){
        game_over = true;
        final_score();
    }
}

void PlayerWrapper::readSettings()
{
    config.beginGroup("Players");
    setBlackName(config.value("black_name").toString());
    setWhiteName(config.value("white_name").toString());
    blackSpecies = config.value("black_species").toString();
    whiteSpecies = config.value("white_species").toString();
    setKomi(config.value("komi").toDouble());
    setHandicap(config.value("handicap").toInt());
    config.endGroup();
}
