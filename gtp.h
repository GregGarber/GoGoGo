#ifndef GTP_H
#define GTP_H

#include <QObject>
#include <QtCore>
#include "gtpengineprocess.h"

class GTP : public QObject
{
    Q_OBJECT
public:
    GTPEngineProcess engine;
    QSettings config;

    bool boardsize(int size);
    bool captures(QString color);
    bool fixed_handicap(int handicap);
    bool genmove(QString color);
    bool komi(qreal komi);
    bool list_stones(QString color);
    bool loadsgf(QString filename, QString &color);
    bool move_reasons(QString vertex);
    bool new_score();
    bool pass(QString color);
    bool play(QString color, QString vertex);
    bool printsgf(QString filename);
    bool resign(QString color);
    bool successful(QByteArray reply);
    bool top_moves(QString color);
    bool undo(int moves);
    explicit GTP(QObject *parent = 0);
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

public slots:
};

#endif // GTP_H
