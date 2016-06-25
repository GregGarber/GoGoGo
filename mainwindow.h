#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gtpengineprocess.h"
#include "settings.h"
#include "gtp.h"
#include "playerwrapper.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool game_over = false;
    QTimer computer_timer;
    GTPEngineProcess engine;
    GTP gtp;
    PlayerWrapper players;
    int moves =0;
    QSettings config;
    QString fileName;
    Settings settings;

    void readSettings();
    void writeSettings();

signals:
    void gameOver(QString reason);

public slots:
    void doPlay(QString color, QString vertex);
    void engineStarted();
    void updateBlackScore(QString score);
    void updateWhiteScore(QString score);
    void setGameOver(QString reason);
    void computerPlay();
    void moveHistory(QString color, QString vertex);

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
