#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->gameBoard, SIGNAL(boardLeftClicked(QString,QString)),this,SLOT(doPlay(QString,QString)));
    connect(&engine.process, SIGNAL(started()), this, SLOT(engineStarted()));
    readSettings();

    engine.addProgramArg("--mode gtp");
    engine.start();
    gtp.setEngine(engine);
}

MainWindow::~MainWindow()
{
    writeSettings();
    delete ui;
}

void MainWindow::engineStarted(){
    on_actionNew_Game_triggered();
}

void MainWindow::doPlay(QString color, QString vertex){
    if(gtp.play(color, vertex)){
        gtp.genmove( players.otherColor(color) );
        gtp.new_score();
        if( ui->buttonHint->isChecked()) gtp.top_moves(color);
    }
}

/*
void MainWindow::updateBlackScore(){
    if(black_score.length()>0){
        ui->labelBlack->setText(QString("B %1 capt:%2 %3").arg(blackName).arg(black_captures).arg(black_score));
    }else{
        ui->labelBlack->setText(QString("B %1 capt:%2 ").arg(blackName).arg(black_captures));
    }
}

void MainWindow::updateWhiteScore(){
    if(white_score.length()>0){
        ui->labelWhite->setText(QString("W %1 capt:%2 %3").arg(whiteName).arg(white_captures).arg(white_score));
    }else{
        ui->labelWhite->setText(QString("W %1 capt:%2 ").arg(whiteName).arg(white_captures));
    }
}
*/

void MainWindow::on_buttonHint_clicked()
{
    gtp.top_moves("black");
}

void MainWindow::on_buttonPass_clicked()
{
    gtp.pass("black");
}

void MainWindow::on_buttonResign_clicked()
{
   // gtp.resign("black");
}

void MainWindow::on_actionNew_Game_triggered()
{
    if(!engine.is_running){
        engine.start();
    }
    //might want option to resume last game
    engine.write("clear_board");
    gtp.boardsize( ui->gameBoard->boardSize );
    ui->gameBoard->clearBoard();
    //komi(komi_value);

    //fixed_handicap(handicap);
    //if(handicap>1) genmove("white");//white gets 1st move if > 1 handicap stone
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
    gtp.undo(1);
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
    qDebug() << "context menu item "<< rightClickItem->text()<< " clicked";
    vertex = ui->gameBoard->posToAlphaNum(ui->gameBoard->mapToScene(pos));

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
#elif
    //probably Windblows chunks
    engine.setProgramPath( config.value("program_path").toString() );//no clue what it should default to
#endif
    config.endGroup();
}
