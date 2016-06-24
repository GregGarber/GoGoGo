#ifndef PLAYERWRAPPER_H
#define PLAYERWRAPPER_H

#include <QObject>
#include <QtCore>
#include "player.h"

class PlayerWrapper : public QObject
{
    Q_OBJECT
public:
    explicit PlayerWrapper(QObject *parent = 0);

    Player* getBlack();
    Player* getCurrent();
    Player* getLast();
    Player* getNext();
    Player* getWhite();
    void resetPass(PlayerColors player);
    void setCurrentPlayer(PlayerColors player);
    void takeTurn();
    void updatePass(PlayerColors player);

    //retire these
    QString otherColor(QString color);
    QString lastPlayer;
    void resetPass(QString color);
    void updatePass(QString color);


signals:

public slots:

protected:
    void writeSettings();
    void readSettings();
    QSettings config;
    Player* white;
    Player* black;
    Player* current;
    Player* last;
    Player* next;
};

#endif // PLAYERWRAPPER_H
