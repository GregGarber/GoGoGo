#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&engine, SIGNAL(gtpResponse(QString,QString,bool)), this, SLOT(processGtpResponse(QString,QString,bool)));
    connect(ui->gameBoard, SIGNAL(boardLeftClicked(QString,QString)),this,SLOT(doPlay(QString,QString)));
    connect(&engine.process, SIGNAL(started()), this, SLOT(engineStarted()));
    readSettings();

    engine.addProgramArg("--mode gtp");
    engine.start();
}

MainWindow::~MainWindow()
{
    writeSettings();
 //   if(engine.is_running) engine.stop();
    delete ui;
}

void MainWindow::engineStarted(){
    qDebug()<<"engine started for main window";
    engine.write("clear_board");
    QByteArray cmd("boardsize ");
    cmd.append(QString("%1").arg(ui->gameBoard->boardSize));
    engine.write(cmd);
    ui->gameBoard->clearBoard();
}

void MainWindow::doPlay(QString color, QString vertex){
    qDebug() << "Do PLAY METHOD: color: "<<color<<" vertex:"<< vertex;
    QByteArray cmd;
    cmd.append("play ");
    cmd.append(color);
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
            }else if((cmd_re == "cmd" || cmd_re=="cmd_int") && response_re =="verticies"){
                QMetaObject::invokeMethod(this,cmd.toLatin1().data(),
                                          QGenericArgument("QStringList",  verticies)
                                          );
            }
            }
        }

    }

void MainWindow::top_moves_black(QStringList verticies){
    ui->gameBoard->showTopMoves("black", verticies);
}

void MainWindow::list_stones(QString color, QStringList verticies){
    qDebug() << "list_stones... color:" << color <<" verticies:" <<verticies;
    ui->gameBoard->checkStones(color, verticies);
}

void MainWindow::genmove(QString color, QString vertex){
    qDebug() << "PLAY METHOD: color: "<<color<<" vertex:"<< vertex;
    ui->gameBoard->placeStone(vertex, color);

    // a little cheesey
    if(ui->actionAutoplay->isChecked()){
        //if( vertex != "pass" && vertex != "resign"){
        if( vertex != ""){
            if(color == "white"){
                engine.write("genmove black");
            }else{
                engine.write("genmove white");
            }
    engine.write("list_stones white");
    engine.write("list_stones black");
        }
    }
}
void MainWindow::play(QString color, QString vertex){
    qDebug() << "PLAY METHOD: color: "<<color<<" vertex:"<< vertex;
    ui->gameBoard->removeMarkers("black_hints");
    ui->gameBoard->placeStone(vertex, color);
}

void MainWindow::fixed_handicap(QStringList verticies){
    qDebug()<<"Fixed Handicap, verts:"<<verticies;
    for(int i=0;i<verticies.length();i++){
        qDebug() <<i<<" "<<verticies[i];
        ui->gameBoard->placeStone(verticies[i], "black");
    }
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
    engine.write("clear_board");
    QByteArray cmd("boardsize ");
    cmd.append(QString("%1").arg(ui->gameBoard->boardSize));
    engine.write(cmd);
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
    int cnt=0;
    if(engine.is_running){
        engine.stop();
    }
    while(engine.is_running && cnt<10) {
        qDebug() << "Delay...";
        cnt++;
        QThread::msleep(100);
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

settings.show();
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
    qDebug() << "TODO context menu requested at:"<<pos;
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

void MainWindow::writeSettings()
{
    config.beginGroup("MainWindow");
    config.setValue("size", size());
    config.setValue("pos", pos());
    config.endGroup();
}

void MainWindow::readSettings()
{
    config.beginGroup("MainWindow");
    resize(config.value("size", QSize(1024, 768)).toSize());
    move(config.value("pos", QPoint(20, 20)).toPoint());
    config.endGroup();
    config.beginGroup("Engine");
#ifdef Q_OS_UNIX
    engine.setProgramPath( config.value("program_path", "/usr/games/gnugo").toString() );
#elif
    //probably Windblows chunks
    engine.setProgramPath( config.value("program_path").toString() );//no clue what it should default to
#endif
    config.endGroup();
}
