#ifndef GTP_H
#define GTP_H

#include <QObject>
#include <QtCore>
#include "gtpengineprocess.h"

class GTP : public QObject
{
    Q_OBJECT
public:
    explicit GTP(QObject *parent = 0);

    GTPEngineProcess* engine;
    QSettings config;

    void setEngine(GTPEngineProcess &engine);
    QStringList dragon_stones(QString color="");
    bool boardsize(int size);
    int captures(QString color);
    QStringList fixed_handicap(int handicap);
    QString genmove(QString color);
    bool komi(qreal komi);
    QStringList list_stones(QString color);
    bool loadsgf(QString filename, QString &color);
   QStringList move_reasons(QString vertex);
    QString new_score();
    bool pass(QString color);
    bool play(QString color, QString vertex);
    bool printsgf(QString filename);
    bool resign(QString color);
    bool successful(QByteArray &reply);
    QStringList top_moves(QString color);
    bool undo(int moves);
    int getInt(QByteArray reply, bool &found);
    QString getVertex(QByteArray reply, bool &found);
    QStringList getVerticies(QByteArray reply, bool &found);
    void final_score();

    QMap<QString, QString> commonREs = {
        {"cmd_str_vertex","^(?<cmd>\\w+)\\s+(?<value>\\w+)\\s+(?<vertex>\\w\\s?\\d\\d?)"},
        {"cmd_int","^(?<cmd>\\w+)\\s+(?<value>\\d+)"},
        {"cmd_real","^(?<cmd>\\w+)\\s+(?<value>\\d*\\.?\\d*)"},
        {"cmd_str","^(?<cmd>\\w+)\\s+(?<value>\\w+)"},
        {"cmd_vertex","^(?<cmd>\\w+)\\s+(?<vertex>\\w\\s?\\d\\d?)"},
        {"cmd_verticies","^(?<cmd>\\w+)(\\s+(?<verticies>\\w\\s?\\d\\d?))+"},
        {"cmd","^(?<cmd>\\w+)"},
        {"sgf_score","(?<color>\\w|0)\\+(?<score>[.\\d]+|\\w+)"}, // read: "= B+111.5\n\n"
        {"int","(?<int>\\d+)"},
        {"word","(?<word>\\w+)"},
        {"vertex","(?<vertex>\\w\\s?\\d\\d?)"},
        {"verticies","(\\w\\s?[.\\d]+)+"}
    };

signals:
    void move(QString color, QString vertex);
    void captureCount(QString color, int total_captures);
    void stoneListing(QString color, QStringList verticies);
    void hints(QString color, QStringList vert_scores);
    void blackScore(QString score);
    void whiteScore(QString score);

public slots:
};

#endif // GTP_H
