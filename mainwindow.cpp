#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&engine, SIGNAL(gtpResponse(QString,QString,bool)), this, SLOT(processGtpResponse(QString,QString,bool)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::processGtpResponse(QString response, QString command, bool success){
    qDebug() << "MainWindow: Com:"<< command << " Resp:" << response << " success:" << success;
    QString html =QString("<i>%1</i><b> %2</b>").arg(command).arg(response);
     ui->textHistory->appendHtml(html);
     //ui->textHistory->appendHtml(html.toHtmlEscaped());
     //play white b4 or genmove white
     if(success){
         if( command.contains("play", Qt::CaseInsensitive) && response=="" ){
                     QRegularExpression re("^(?<play>\\w+)\\s+(?<color>\\w+)\\s+(?<location>\\w\\s?\\d\\d?)");
                     QRegularExpressionMatch match = re.match(command);
                     if (match.hasMatch()) {
                         QString color = match.captured("color");
                         QString location = match.captured("location");
                         ui->gameBoard->placeStone(location, color);
                     }
         }else if(command.contains("genmove", Qt::CaseInsensitive)){
                     QRegularExpression re("^(?<play>\\w+)\\s+(?<color>\\w+)");
                     QRegularExpressionMatch match = re.match(command);
                     if (match.hasMatch()) {
                         QString color = match.captured("color");
                         ui->gameBoard->placeStone(response, color);
                         if(response != "pass" && response != "resign"){
                         if(color == "white"){
                             engine.write("genmove black");
                         }else{
                             engine.write("genmove white");
                         }
                         }
                     }

         }
     }

}

void MainWindow::on_buttonHint_clicked()
{

    ui->gameBoard->placeStone("f10", "black");
    engine.write("help\n");
    QByteArray ba;
    for(int i=0;i<10;i++){
        ba = "echo ";
        ba.append( QString("%1\n").arg(i));
        engine.write(ba);
    }
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

void MainWindow::on_lineCommand_returnPressed()
{
    if( engine.is_running ){
        engine.write( QByteArray( ui->lineCommand->text().toLatin1() ));
        ui->lineCommand->clear();
    }else{
        ui->textHistory->appendHtml("<b>GNU Go Engine Not Running</b>");
    }
}
