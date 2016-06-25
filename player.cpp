#include "player.h"

Player::Player(QObject *parent) : QObject(parent)
{

}

int Player::getCaptures(){
    return captures;
}

bool Player::doHints(){
    return do_hints;
}

QString Player::getColorString(){
    return color_string;
}

int Player::getHandicap(){
    return this->handicap;
}

qreal Player::getKomi(){
    return this->komi_value;
}

QString Player::getName(){
    return this->name;
}

PlayerColors Player::getPlayerColor(){
    return this->color;
}

QString Player::getSpecies(){
    return this->species;
}

void Player::setDoHints(bool do_hints){
    this->do_hints = do_hints;
}

void Player::setCaptures(int capt){
    this->captures = capt;
}

void Player::setHandicap(int handicap){
    this->handicap = handicap;
}

void Player::setKomi(qreal komi){
    this->komi_value = komi;
}

void Player::setName(QString name){
    this->name = name;
}

void Player::setColor(PlayerColors color){
    this->color = color;
    if(color == Black){
        color_string = "black";
    }else{
        color_string = "white";
    }
}

void Player::setColor(QString color){
    this->color_string = color;
    if(color == "black"){
        this->color = Black;
    }else{
        this->color = White;
    }
}

void Player::setSpecies(QString species){
    this->species = species;
}

void Player::reset(){
    captures = 0;
    is_passing = false;
    has_resigned = false;
}

void Player::setIsPassing(bool p){
    is_passing = p;
}

bool Player::getIsPassing(){
    return is_passing;
}

void Player::setResigned(bool r){
    this->has_resigned =r;
}

bool Player::getResigned(){
    return has_resigned;
}
