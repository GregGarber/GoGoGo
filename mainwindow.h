#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gtpengineprocess.h"

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
    QMap<QString, QString> commonREs = {
        {"cmd_str_vertex","^(?<cmd>\\w+)\\s+(?<value>\\w+)\\s+(?<vertex>\\w\\s?\\d\\d?)"},
        {"cmd_int","^(?<cmd>\\w+)\\s+(?<value>\\d+)"},
        {"cmd_real","^(?<cmd>\\w+)\\s+(?<value>\\d*\\.?\\d*)"},
        {"cmd_str","^(?<cmd>\\w+)\\s+(?<value>\\w+)"},
        {"cmd_vertex","^(?<cmd>\\w+)\\s+(?<vertex>\\w\\s?\\d\\d?)"},
        {"cmd_verticies","^(?<cmd>\\w+)(\\s+(?<verticies>\\w\\s?\\d\\d?))+"},
        {"cmd","^(?<cmd>\\w+)"},
        {"vertex","(?<vertex>\\w\\s?\\d\\d?)"},
        {"verticies","(\\w\\s?\\d\\d?)+"}
    };
        //{"verticies","((?<verticies>\\w\\s?\\d\\d?))+"}
    //todo list of verticies for set free handicap, file stuff, dragona, dragonb

    struct CommandStuct{
        QString cmd_re;
        QString response_re;
    };
    QMap<QString, CommandStuct> CommandREs = {
    {"play", {  "cmd_str_vertex", "verticies"} },
    {"genmove", {  "cmd_str", "vertex"} }
    };

public slots:
    void processGtpResponse(QString response, QString command, bool success);
    void play(QString colour, QString vertex);
    void doPlay(QString colour, QString vertex);
    void genmove(QString colour, QString vertex);

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
