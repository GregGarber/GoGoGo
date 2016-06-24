#include "player.h"

Player::Player(QObject *parent) : QObject(parent)
{

}

void Player::setName(QString name){
    this->blackName = name;
    ui->labelBlack->setText(QString("%1 (Black)").arg(name));
}

void Player::setSpecies(QString name){
    this->blackName = name;
    ui->labelBlack->setText(QString("%1 (Black)").arg(name));
}

void Player::setHandicap(int handicap){
    this->handicap = handicap;
}

void Player::setKomi(qreal komi){
    this->komi_value = komi;
}
