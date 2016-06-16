#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&engine, SIGNAL(gtpResponse(QString,QString,bool)), this, SLOT(processGtpResponse(QString,QString,bool)));
    connect(ui->gameBoard, SIGNAL(boardLeftClicked(QString,QString)),this,SLOT(doPlay(QString,QString)));

    engine.setProgramPath("/usr/local/gnugo/bin/gnugo");
    engine.addProgramArg("--mode gtp");
    engine.start();
}

MainWindow::~MainWindow()
{
    if(engine.is_running) engine.stop();
    delete ui;
}
void MainWindow::doPlay(QString colour, QString vertex){
    qDebug() << "Do PLAY METHOD: colour: "<<colour<<" vertex:"<< vertex;
    QByteArray cmd;
    cmd.append("play ");
    cmd.append(colour);
    cmd.append(" ");
    cmd.append(vertex);
    engine.write(cmd);
    engine.write("genmove white");
    engine.write("list_stones white");
    engine.write("list_stones black");
}

void MainWindow::processGtpResponse(QString response, QString command, bool success){
    const void* value = nullptr;
    const void* vertex = nullptr;
    const void* response_vertex = nullptr;
    const void* verticies = nullptr;

    ui->textHistory->appendPlainText(QString("%1 %2 %3\n").arg(command).arg(response).arg(success));
    QString value_str, vertex_str, response_vertex_str;
    QStringList verticies_list;
    QRegularExpression re(commonREs.value("cmd"));
    QRegularExpressionMatch match = re.match(command);
    QString cmd, cmd_re;
    if (match.hasMatch()) {
        cmd = match.captured("cmd");
        cmd_re = CommandREs.value(cmd).cmd_re; // play black b4
    //    qDebug()<< "cmd: "<<cmd<<" cmd_re: "<<cmd_re << " re:" << commonREs.value(cmd_re);
        QRegularExpression cmd_regex( commonREs.value(cmd_re));
        QRegularExpressionMatch cmd_match = cmd_regex.match(command);
     //   qDebug() << "response re: "<<commonREs.value(CommandREs.value(cmd).response_re);
        QRegularExpression response_regex( commonREs.value(CommandREs.value(cmd).response_re));
        if(CommandREs.value(cmd).response_re == "verticies"){
            QRegularExpressionMatchIterator i = response_regex.globalMatch(response);
            while (i.hasNext()) {
                QRegularExpressionMatch match = i.next();
                QString word = match.captured(1);
                //qDebug()<<"MATCH: "<<word;
                verticies_list.append(word);
            }
            //qDebug() << "matches:"<<verticies_list;
        }else{
            QRegularExpressionMatch response_match = response_regex.match(response);
            if(response_match.hasMatch()){
                response_vertex_str = response_match.captured("vertex");//any point to this?
            }
        }

        QString response_re = CommandREs.value(cmd).response_re;
        if(cmd_match.hasMatch()){
            value_str = cmd_match.captured("value");
            vertex_str = cmd_match.captured("vertex");
            //verticies_list = cmd_match.capturedTexts(); //??????
            value = &value_str;
            vertex = &vertex_str;
            response_vertex = &response_vertex_str;
            verticies = &verticies_list;
            //qDebug() << "REGX: value_str: "<<value_str<<" vertex_str:" << vertex_str;
            //qDebug() << "REGX: value: "<<value<<" vertex:" << vertex;

            if(cmd_re == "cmd_str_vertex" && response_re ==""){
                QMetaObject::invokeMethod(this,cmd.toLatin1().data(),
                                          QGenericArgument("QString", value),
                                          QGenericArgument("QString", vertex)
                                          );
            }else if(cmd_re == "cmd_str" && response_re ==""){
                QMetaObject::invokeMethod(this,cmd.toLatin1().data(),
                                          QGenericArgument("QString", value)
                                          );
            }else if(cmd_re == "cmd_str" && response_re =="vertex"){
                QMetaObject::invokeMethod(this,cmd.toLatin1().data(),
                                          QGenericArgument("QString", value),
                                          QGenericArgument("QString", response_vertex)
                                          );
            }else if(cmd_re == "cmd_str" && response_re =="verticies"){
                QMetaObject::invokeMethod(this,cmd.toLatin1().data(),
                                          QGenericArgument("QString", value),
                                          QGenericArgument("QStringList",  verticies)
                                          );
            }else if(cmd_re == "cmd" && response_re =="verticies"){
                QMetaObject::invokeMethod(this,cmd.toLatin1().data(),
                                          QGenericArgument("QStringList",  verticies)
                                          );
            }
            }
        }

    }
    /*
        if(ui->actionAutoplay->isChecked()){
            if(response != "pass" && response != "resign"){
            if(color == "white"){
                engine.write("genmove black");
            }else{
                engine.write("genmove white");
            }
    */

void MainWindow::top_moves_black(QStringList verticies){
    ui->gameBoard->showTopMoves("black", verticies);
}

void MainWindow::list_stones(QString colour, QStringList verticies){
    qDebug() << "list_stones... colour:" << colour <<" verticies:" <<verticies;
    ui->gameBoard->checkStones(colour, verticies);
}

void MainWindow::genmove(QString colour, QString vertex){
    qDebug() << "PLAY METHOD: colour: "<<colour<<" vertex:"<< vertex;
    ui->gameBoard->placeStone(vertex, colour);
}
void MainWindow::play(QString colour, QString vertex){
    qDebug() << "PLAY METHOD: colour: "<<colour<<" vertex:"<< vertex;
    ui->gameBoard->removeMarkers("black_hints");
    ui->gameBoard->placeStone(vertex, colour);
}

void MainWindow::on_buttonHint_clicked()
{
    engine.write("top_moves_black");
}

void MainWindow::on_buttonPass_clicked()
{
}

void MainWindow::on_buttonResign_clicked()
{
}

void MainWindow::on_actionNew_Game_triggered()
{
    if(!engine.is_running){
        engine.start();
    }
    engine.write("clearboard");
    ui->gameBoard->clearBoard();

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
    if(engine.is_running){
        engine.stop();
    }
    QApplication::quit();
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
qDebug() << "on_actionToolbar_toggled:" << arg1;

}

void MainWindow::on_actionAbout_triggered()
{

}

void MainWindow::on_actionHistory_toggled(bool arg1)
{
qDebug() << "on_actionHistory_toggled:" << arg1;
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

void MainWindow::on_actionAutoplay_triggered()
{
    if( engine.is_running ){
        engine.write("genmove black");
    }
}

void MainWindow::on_gameBoard_customContextMenuRequested(const QPoint &pos)
{
    /* The gameBoard may not be the right thing to attach the menu to since it might
     * prevent the scene and graphic items from having context menus which is probably
     * what I really want.
     * Also, once menu is opened, a left click on the board crashes the program, whether
     * I delete rightClickItem or not.
    qDebug()<<"context menu requested";
    QMenu submenu;
    QPoint item = ui->gameBoard->mapToGlobal(pos);
    submenu.addAction("Add");
    submenu.addAction("Delete");
    QAction* rightClickItem = submenu.exec(item);
    qDebug() << "context menu item "<< rightClickItem->text()<< " clicked";
    delete rightClickItem;

*/
}
