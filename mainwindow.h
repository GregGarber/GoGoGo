#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gtpengineprocess.h"
#include "settings.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    GTPEngineProcess engine;
    Settings settings;
    QSettings config;
    void readSettings();
    void writeSettings();
    void setBlackName(QString name);
    void setWhiteName(QString name);
    void setHandicap(int handicap);
    void setKomi(qreal komi);

    QString blackName, whiteName;
    int handicap;
    qreal komi_value;

    QMap<QString, QString> commonREs = {
        {"cmd_str_vertex","^(?<cmd>\\w+)\\s+(?<value>\\w+)\\s+(?<vertex>\\w\\s?\\d\\d?)"},
        {"cmd_int","^(?<cmd>\\w+)\\s+(?<value>\\d+)"},
        {"cmd_real","^(?<cmd>\\w+)\\s+(?<value>\\d*\\.?\\d*)"},
        {"cmd_str","^(?<cmd>\\w+)\\s+(?<value>\\w+)"},
        {"cmd_vertex","^(?<cmd>\\w+)\\s+(?<vertex>\\w\\s?\\d\\d?)"},
        {"cmd_verticies","^(?<cmd>\\w+)(\\s+(?<verticies>\\w\\s?\\d\\d?))+"},
        {"cmd","^(?<cmd>\\w+)"},
        {"int","(?<int>\\d+)"},
        {"vertex","(?<vertex>\\w\\s?\\d\\d?)"},
        {"verticies","(\\w\\s?[.\\d]+)+"}
    };

    struct CommandStuct{
        QString cmd_re;
        QString response_re;
    };
    QMap<QString, CommandStuct> CommandREs = {
            {"play", {  "cmd_str_vertex", ""} },
            {"genmove", {  "cmd_str", "vertex"} },
            {"list_stones", {  "cmd_str", "verticies"} },
            {"top_moves_black", {"cmd","verticies"}},
            {"fixed_handicap", {"cmd_int","verticies"}}
    };

    QString getVertex(QByteArray reply, bool &found);
    QStringList getVerticies(QByteArray reply, bool &found);
    int getInt(QByteArray reply, bool &found);
    QString otherColor(QString color);
    void updateBlackScore();
    void updateWhiteScore();
    int black_captures = 0;
    int white_captures = 0;
    QString black_score;
    QString white_score;
    bool  is_black_passing = false;
    bool  is_white_passing = false;
    bool game_over = false;
    int moves =0;
    bool captures(QString color);
    bool play(QString color, QString vertex);
    bool genmove(QString color);
    bool list_stones(QString color);
    bool top_moves(QString color);
    bool fixed_handicap(int handicap);
    bool boardsize(int size);
    bool komi(qreal komi);
    bool successful(QByteArray reply);
    bool undo(int moves);
    bool pass(QString color);
    void resetPass(QString color);
    void updatePass(QString color);
    void final_score();
public slots:
    void doPlay(QString color, QString vertex);
    void engineStarted();

private slots:
    void on_buttonHint_clicked();

    void on_buttonPass_clicked();

    void on_buttonResign_clicked();

    void on_actionNew_Game_triggered();

    void on_actionSave_Game_triggered();

    void on_actionSave_Game_As_triggered();

    void on_actionOpen_triggered();

    void on_actionQuit_triggered();

    void on_actionOpen_Recent_triggered();

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_actionPreferences_triggered();

    void on_actionUndo_2_triggered();

    void on_actionRedo_2_triggered();

    void on_actionSuggest_Move_triggered();

    void on_actionText_Interface_triggered();

    void on_actionHistory_triggered();

    void on_actionToolbar_toggled(bool arg1);

    void on_actionAbout_triggered();

    void on_actionHistory_toggled(bool arg1);

    void on_lineCommand_returnPressed();

    void on_actionAutoplay_triggered();

    void on_gameBoard_customContextMenuRequested(const QPoint &pos);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
