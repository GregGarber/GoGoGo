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

}

void MainWindow::on_buttonPass_clicked()
{

}

void MainWindow::on_buttonResign_clicked()
{

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
