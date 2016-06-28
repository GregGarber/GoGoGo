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
    ~PlayerWrapper();

    Player* getBlack();
    Player* getCurrent();
    Player* getLast();
    Player* getNext();
    Player* getWhite();
    void setCurrentPlayer(PlayerColors player);
    void swap();
    void setCaptures(int captures);
    void setCaptures(QString color, int captures);
    void setHandicap(int h);
    void setCurrentPass();
    void setCurrentPlays();
    bool getMutalPass();
    QString getGameOver();
    void setCurrentResigned();

signals:
    void blackScore(QString score);
    void whiteScore(QString score);

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
