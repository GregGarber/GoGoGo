#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_buttonHint_clicked()
{

    ui->gameBoard->placeStone("f10", "black");
    engine.write("help\n");
}

void MainWindow::on_buttonPass_clicked()
{
    ui->gameBoard->placeStone("g10", "white");
    engine.write("quit\n");
}

void MainWindow::on_buttonResign_clicked()
{
    //ui->gameBoard->removeStone("f10");
    ui->gameBoard->clearBoard();

    // /usr/local/gnugo/bin/gnugo --mode gtp --level 1  --depth 1
    engine.setProgramPath("/usr/local/gnugo/bin/gnugo");
    engine.addProgramArg("--mode gtp");
    engine.start();
}

void MainWindow::on_actionNew_Game_triggered()
{

}

void MainWindow::on_actionSave_Game_triggered()
{

}

void MainWindow::on_actionSave_Game_As_triggered()
{

}

void MainWindow::on_actionOpen_triggered()
{

}

void MainWindow::on_actionQuit_triggered()
{

}

void MainWindow::on_actionOpen_Recent_triggered()
{

}

void MainWindow::on_actionUndo_triggered()
{

}

void MainWindow::on_actionRedo_triggered()
{

}

void MainWindow::on_actionPreferences_triggered()
{

}

void MainWindow::on_actionUndo_2_triggered()
{

}

void MainWindow::on_actionRedo_2_triggered()
{

}

void MainWindow::on_actionSuggest_Move_triggered()
{

}

void MainWindow::on_actionText_Interface_triggered()
{

}

void MainWindow::on_actionHistory_triggered()
{

}

void MainWindow::on_actionToolbar_toggled(bool arg1)
{

}

void MainWindow::on_actionAbout_triggered()
{

}

void MainWindow::on_actionHistory_toggled(bool arg1)
{

}
