#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QtCore>
#include "playerwrapper.h"

class Player : public QObject
{
    Q_OBJECT
public:
    explicit Player(QObject *parent = 0);
    bool  is_passing = false;
    bool has_resigned = false;
    PlayerWrapper::PlayerColors color;
    int captures = 0;
    int handicap;
    qreal komi_value;
    QString name;
    QString score;
    QString species;
    void setName(QString name);
    void setSpecies(QString name);
    void setPlayerColor(PlayerWrapper::PlayerColors color);
    void setHandicap(int handicap);
    void setKomi(qreal komi);


signals:

public slots:
};

#endif // PLAYER_H
