#include "player.h"

Player::Player(QObject *parent) : QObject(parent)
{

}

void Player::setName(QString name){
    this->name = name;
}

void Player::setSpecies(QString species){
    this->species = species;
}

void Player::setHandicap(int handicap){
    this->handicap = handicap;
}

void Player::setKomi(qreal komi){
    this->komi_value = komi;
}
