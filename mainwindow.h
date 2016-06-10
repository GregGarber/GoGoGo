#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

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

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
