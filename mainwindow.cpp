#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    computer_timer.setSingleShot(true);
    connect(&computer_timer, SIGNAL(timeout()), this, SLOT(computerPlay()));
    connect(this, SIGNAL(gameOver(QString)), this, SLOT(setGameOver(QString)));
    //connect(ui->gameBoard, SIGNAL(boardLeftClicked(QString,QString)),this,SLOT(doPlay(QString)));
    connect(ui->gameBoard, SIGNAL(boardLeftClicked(QString)),this,SLOT(doPlay(QString)));
    connect(&players, SIGNAL(blackScore(QString)), this, SLOT(updateBlackScore(QString)));
    connect(&players, SIGNAL(whiteScore(QString)), this, SLOT(updateWhiteScore(QString)));
    connect(&gtp, SIGNAL(move(QString,QString)),ui->gameBoard,SLOT(placeStone(QString,QString)));
    connect(&gtp, SIGNAL(move(QString,QString)),this,SLOT(moveHistory(QString,QString)));
    connect(&gtp, SIGNAL(stoneListing(QString,QStringList)), ui->gameBoard, SLOT(checkStones(QString,QStringList)));
    connect(&gtp, SIGNAL(hints(QString,QStringList)), ui->gameBoard, SLOT(showTopMoves(QString,QStringList)));
    connect(&gtp, SIGNAL(blackScore(QString)), this, SLOT(updateBlackScore(QString)));
    connect(&gtp, SIGNAL(whiteScore(QString)), this, SLOT(updateWhiteScore(QString)));
    connect(&engine.process, SIGNAL(started()), this, SLOT(engineStarted()));
    readSettings();

    engine.addProgramArg("--mode gtp");
    engine.start();
    gtp.setEngine(engine);
    ui->labelBlack->setStyleSheet("#labelBlack { color:#000000; background: #785229;}");
    ui->labelWhite->setStyleSheet("#labelWhite { color:#FFFFFF; background: #785229;}");
}

MainWindow::~MainWindow()
{
    writeSettings();
    delete ui;
}

void MainWindow::engineStarted(){
    on_actionNew_Game_triggered();
}

void MainWindow::setGameOver(QString reason){
    game_over = true;
    ui->labelNotes->setText(QString("Game Over %1").arg(reason));
    gtp.final_score();
    ui->textHistory->appendPlainText(QString("%1 Game Over %2").arg(moves).arg(reason));
}
void MainWindow::computerPlay(){
    doPlay("");
}

void MainWindow::doPlay(QString vertex){
    if(game_over) return;
    qDebug() <<" doPlay "<<vertex<<" *********************************";
    if(players.getCurrent()->getSpecies() == "Computer"){
        QString result = gtp.genmove( players.getCurrent()->getColorString() );
        if(result == "pass"){
            players.setCurrentPass();
        }else if( result == "resign"){
            players.setCurrentResigned();
        }else{
            players.setCurrentPlays();
        }
        QString reason = players.getGameOver();
        if(reason.length()>0) emit gameOver(reason);
    }else{
        gtp.play( players.getCurrent()->getColorString(), vertex);
        players.setCurrentPlays();
    }
    int cnt= gtp.captures(players.getCurrent()->getColorString());
    if(cnt != players.getCurrent()->getCaptures()){
        players.getCurrent()->setCaptures(cnt);
        ui->gameBoard->checkStones(players.getNext()->getColorString(), gtp.list_stones( players.getNext()->getColorString()));
    }
    moves++;
    statusBar()->showMessage(QString("Moves: %1 Komi:%2 Handicap:%3 Estimated Score: %4")
                                .arg(moves)
                                .arg(players.getWhite()->getKomi())
                                .arg(players.getBlack()->getHandicap())
                                .arg(gtp.new_score()));

    ui->gameBoard->removeMarkers(QString("%1_hints").arg(players.getCurrent()->getColorString()));
    players.swap();
    ui->labelNotes->setText(QString("%1's (%2's) turn").arg(players.getCurrent()->getName()).arg(players.getCurrent()->getColorString()));
    ui->buttonHint->setChecked(players.getCurrent()->doHints());
    if(players.getCurrent()->getSpecies() == "Computer"){
        computer_timer.start(100);
    }else{
        if( players.getCurrent()->doHints()) gtp.top_moves(players.getCurrent()->getColorString());
    }
}
void MainWindow::moveHistory(QString color, QString vertex){
    ui->textHistory->appendPlainText(QString("%1 %2 %3").arg(moves).arg(color).arg(vertex.toUpper()));
}

void MainWindow::updateBlackScore(QString score){
    if(score.length()>0){
        ui->labelBlack->setText(QString("B %1 capt:%2\n %3").arg(players.getBlack()->getName()).arg(players.getBlack()->getCaptures()).arg(score));
    }else{
        ui->labelBlack->setText(QString("B %1 capt:%2 ").arg(players.getBlack()->getName()).arg(players.getBlack()->getCaptures()));
    }
}

void MainWindow::updateWhiteScore(QString score){
    if(score.length()>0){
        ui->labelWhite->setText(QString("W %1 capt:%2\n %3").arg(players.getWhite()->getName()).arg(players.getWhite()->getCaptures()).arg(score));
    }else{
        ui->labelWhite->setText(QString("W %1 capt:%2 ").arg(players.getWhite()->getName()).arg(players.getWhite()->getCaptures()));
    }
}

void MainWindow::on_buttonHint_clicked()
{
    if(!players.getCurrent()->doHints() && ui->buttonHint->isChecked()){
        gtp.top_moves( players.getCurrent()->getColorString());
    }
    players.getCurrent()->setDoHints(ui->buttonHint->isChecked());
}

void MainWindow::on_buttonPass_clicked()
{
    players.setCurrentPass();
    gtp.pass(players.getCurrent()->getColorString());
    QString reason = players.getGameOver();
    if(reason.length()>0) emit gameOver(reason);
}


void MainWindow::on_buttonResign_clicked()
{
    players.setCurrentResigned();
    QString reason = players.getGameOver();
    if(reason.length()>0) emit gameOver(reason);
}

void MainWindow::on_actionNew_Game_triggered()
{
    if(!engine.is_running){
        engine.start();
    }
    game_over = false;
    moves=1;
    //might want option to resume last game
    engine.write("clear_board");
    gtp.boardsize( ui->gameBoard->boardSize );
    ui->gameBoard->clearBoard();
    ui->gameBoard->removeMarkers("black_hints");
    ui->gameBoard->removeMarkers("white_hints");
    ui->textHistory->clear();
    //komi(komi_value);

    QStringList stones = gtp.fixed_handicap( players.getBlack()->getHandicap());
    players.setHandicap(stones.length());
    ui->labelNotes->setText(QString("%1 (%2) Gets First Turn").arg(players.getCurrent()->getName()).arg(players.getCurrent()->getColorString()));

    if(players.getCurrent()->getSpecies() == "Computer"){
        computer_timer.start(100);
    }
}

void MainWindow::on_actionSave_Game_triggered()
{
    //oddly, command is like: printsgf filename.sgf
    if(fileName.length() > 0){
        gtp.printsgf(fileName);
    }else{
        on_actionSave_Game_As_triggered();
    }
}

void MainWindow::on_actionSave_Game_As_triggered()
{
    //oddly, command is like: printsgf filename.sgf
    fileName = QFileDialog::getSaveFileName(this, tr("Save Game"),"./", tr("SGF Files (*.sgf"));
    gtp.printsgf(fileName);
}

void MainWindow::on_actionOpen_triggered()
{
    //loadsgf ../build-GoGoGo-Qt_5_6_0-Debug/wtf.sgf
    //= white
    QString color;
    fileName = QFileDialog::getOpenFileName(this, tr("Open Game"), "~/", tr("SGF Files (*.sgf);;All Files (*.*)"));
    if(gtp.loadsgf(fileName, color)){
        //FIXME should set game to color's turn
        ui->gameBoard->clearBoard();
        gtp.list_stones("black");
        gtp.list_stones("white");
    }
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
    gtp.undo(1);
    moves--;
}

void MainWindow::on_actionRedo_triggered()
{

}

void MainWindow::on_actionPreferences_triggered()
{
    settings.show();
}

/*
void MainWindow::on_actionUndo_2_triggered()
{
    gtp.undo(1);
}

void MainWindow::on_actionRedo_2_triggered()
{

}
*/

void MainWindow::on_actionSuggest_Move_triggered()
{
    if(!players.getCurrent()->doHints()){
        gtp.top_moves( players.getCurrent()->getColorString());
    }
}

void MainWindow::on_actionText_Interface_triggered()
{
    if( ui->actionText_Interface->isChecked()){
        ui->lineCommand->show();
    }else{
        ui->lineCommand->hide();
    }
}

void MainWindow::on_actionHistory_triggered()
{
    if( ui->actionHistory->isChecked()){
        ui->textHistory->show();
    }else{
        ui->textHistory->hide();
    }
}

void MainWindow::on_actionToolbar_toggled(bool arg1)
{
    //if(ui->actionToolbar->isChecked()){
    if(arg1){
        ui->mainToolBar->show();
    }else{
        ui->mainToolBar->hide();
    }
}

void MainWindow::on_actionAbout_triggered()
{
    about.show();
}

void MainWindow::on_actionHistory_toggled(bool arg1)
{
    //why do I have this, when on_actionHistory_triggered() exists?
    qDebug() << "on_actionHistory_toggled:" << arg1;
}

void MainWindow::on_lineCommand_returnPressed()
{
    if( engine.is_running ){
        QStringList commands = ui->lineCommand->text().split("\n");
        for(int i=0; i<commands.length();i++){
             engine.write( QByteArray( commands.at(i).toLatin1() ));
        }
    if(ui->lineCommand->text().contains("play", Qt::CaseInsensitive) || ui->lineCommand->text().contains("genmove",Qt::CaseInsensitive)){
        gtp.list_stones("black");
        gtp.list_stones("white");
    }
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
    QMenu submenu;
    QPoint item = mapToGlobal(pos);
    QString vertex;
    submenu.addAction("Move Reasons");
//    submenu.addAction("Delete");
    QAction* rightClickItem = submenu.exec(item);
    //qDebug() << "context menu item "<< rightClickItem->text()<< " clicked";
    vertex = ui->gameBoard->posToAlphaNum(ui->gameBoard->mapToScene(pos));

    rightClickItem->dumpObjectInfo();
    if( rightClickItem->text() == "Move Reasons"){
        gtp.move_reasons(vertex);
    }

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
#else if Q_OS_WIN
    //probably Windblows chunks
    engine.setProgramPath( config.value("program_path").toString() );//no clue what it should default to
#endif
    config.endGroup();
}

void MainWindow::on_actionRight_Panel_triggered()
{
   if(ui->actionRight_Panel->isChecked()){
       ui->tabWidget->show();
   } else{
       ui->tabWidget->hide();
   }
}

void MainWindow::on_actionFull_Screen_triggered()
{
   if(ui->actionFull_Screen->isChecked()){
       this->setWindowState(this->windowState() | Qt::WindowFullScreen);
   } else {
       int flag = this->windowState();
       int another_flag = !Qt::WindowFullScreen;
       int anding = flag & another_flag;
       this->setWindowState((Qt::WindowStates)anding);
       /*
       this->setWindowState(this->windowState() & !Qt::WindowFullScreen);
       this->hide();
       this->show();
       */
   }
}
