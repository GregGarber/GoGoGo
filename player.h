#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QtCore>

    enum PlayerColors{
        Black,
        White
    };

class Player : public QObject
{
    Q_OBJECT
public:
    explicit Player(QObject *parent = 0);

    bool doHints();
    int getCaptures();
    QString getColorString();
    int getHandicap();
    qreal getKomi();
    QString getName();
    PlayerColors getPlayerColor();
    QString getSpecies();
    void setCaptures(int capt);
    void setDoHints(bool do_hints);
    void setHandicap(int handicap);
    void setKomi(qreal komi);
    void setName(QString name);
    void setColor(PlayerColors color);
    void setColor(QString color);
    void setSpecies(QString species);
    void reset();
    void setIsPassing(bool p);
    bool getIsPassing();
    void setResigned(bool r);
    bool getResigned();

signals:

public slots:

private:
    bool do_hints = false;
    bool  is_passing = false;
    bool has_resigned = false;
    enum PlayerColors color;
    QString color_string;
    int captures = 0;
    int handicap = 0;
    qreal komi_value;
    QString name;
    QString score;
    QString species;
};

#endif // PLAYER_H
