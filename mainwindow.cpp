#include "mainwindow.h"
#include "ui_mainwindow.h"

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
}

MainWindow::~MainWindow()
{
    writeSettings();
    delete ui;
}

void MainWindow::engineStarted(){
    on_actionNew_Game_triggered();
}

bool MainWindow::successful(QByteArray reply){
    if(reply[0]=='='){
        return true;
    }else{
        qDebug() << reply;
        return false;
    }
}

void MainWindow::doPlay(QString color, QString vertex){
    if(play(color, vertex)){
        genmove("white");
    }
}


bool MainWindow::top_moves(QString color){
    bool ret = false;
    QByteArray reply = engine.write( QString("top_moves_%1").arg(color));
    if(successful( reply )){
        QStringList verticies = getVerticies(reply, ret);
        if(ret){
            ui->gameBoard->showTopMoves(color, verticies);
        }
    }else{
        qDebug() << "top_moves_"<< color <<" "<<reply;
    }
    return ret;
}

QString MainWindow::otherColor(QString color){
    return (color.toLower() == "black") ? "white":"black";
}

bool MainWindow::captures(QString color){
    bool ret = false;
    int tmp=0;
    int captures=0;
    if(color == "black") tmp = black_captures; else tmp=white_captures;
    QByteArray reply = engine.write( QString("captures %1").arg(color));
    if(successful( reply )){
        captures = getInt(reply, ret);
        if(ret && captures != tmp ){
            if(color == "black"){
                black_captures = captures;
                updateBlackScore();
            }else{
                white_captures = captures;
                updateWhiteScore();
            }
            list_stones( otherColor(color));
        }
    }
    return ret;
}

bool MainWindow::undo(int moves=1){
    bool ret = false;
    QByteArray reply = engine.write( QString("undo %1").arg(moves));
    if(successful( reply )){
        ret =  captures("black");
        ret &= captures("white");
        list_stones("black");
        list_stones("white");
    }
    return ret;
}

bool MainWindow::pass(QString color){
    bool ret = false;
    QByteArray reply = engine.write( QString("play %1 pass").arg(color));
    if(successful( reply )){
        ret=true;
        updatePass(color);
        if(!game_over) genmove(otherColor(color));
    }
    return ret;
}
void  MainWindow::resetPass(QString color){
    if(color == "black"){
        is_black_passing = false;
    }else{
        is_white_passing = false;
    }
}

void MainWindow::updatePass(QString color){
    if(color == "black"){
        is_black_passing = true;
    }else{
        is_white_passing = true;
    }
    if(is_black_passing == true && is_white_passing==true){
        game_over = true;
        final_score();
    }
}

void MainWindow::final_score(){
    QByteArray reply = engine.write( QString("final_score"));
    if(successful( reply )){
        ui->textHistory->appendPlainText(QString(reply));
    }
}

bool MainWindow::list_stones(QString color){
    //qDebug() << "list_stones... color:" << color <<" verticies:" <<verticies;
    bool ret = false;
    QByteArray reply = engine.write( QString("list_stones %1").arg(color));
    if(successful( reply )){
        QStringList verticies = getVerticies(reply, ret);
        if(ret){
            ui->gameBoard->checkStones(color, verticies);
        }
    }else{
        qDebug() << "list_stones: "<< color <<" "<<reply;
    }
    return ret;
}

int MainWindow::getInt(QByteArray reply, bool &found){
    QRegularExpression re(commonREs.value("int"));
    QRegularExpressionMatch match = re.match(reply);
    int the_int;
    if (match.hasMatch()) {
        the_int = match.captured("int").toInt();
        found=true;
    }else{
        found=false;
    }
    return the_int;
}
QString MainWindow::getVertex(QByteArray reply, bool &found){
    QRegularExpression re(commonREs.value("vertex"));
    QRegularExpressionMatch match = re.match(reply);
    QString vertex;
    if (match.hasMatch()) {
        vertex = match.captured("vertex");
        vertex = vertex.toLower();
        found=true;
    }else{
        found=false;
    }
    return vertex;
}

QStringList MainWindow::getVerticies(QByteArray reply, bool &found){
    QStringList verticies;
    QRegularExpression re(commonREs.value("verticies"));
    QRegularExpressionMatchIterator i = re.globalMatch(reply);
    while (i.hasNext()) {
        found = true;
        QRegularExpressionMatch match = i.next();
        verticies.append( match.captured(1).toLower());
    }
    return verticies;
}

bool MainWindow::genmove(QString color){
    bool ret = false;
    QByteArray reply =  engine.write(QString("genmove %1").arg(color));
    if( successful( reply)){
        QString vertex = getVertex(reply, ret);
        if(ret){
            resetPass(color);
            ui->gameBoard->placeStone(vertex, color);
            ui->textHistory->appendPlainText(QString("genmove %1 %2").arg(color).arg(vertex));
            captures(color);
        }else{
            QString something = QString(reply).toLower();
            if(something.contains("pass")  ){
                ui->textHistory->appendPlainText(QString("genmove %1 passed").arg(color));
                updatePass(color);
            }else if(something.contains("resign")){
                ui->textHistory->appendPlainText(QString("genmove %1 resigned").arg(color));
            }else{
                ui->textHistory->appendPlainText(QString("genmove WTF %1 %2").arg(color).arg(something));
            }
        }
    }
    return ret;
}

bool MainWindow::play(QString color, QString vertex){
    qDebug() << "PLAY METHOD: color: "<<color<<" vertex:"<< vertex;
    QString cmd=QString("play %1 %2").arg(color).arg(vertex);
    bool ret=false;
    if( successful(engine.write(cmd))){
        resetPass(color);
        ui->gameBoard->removeMarkers(QString("%1_hints").arg(color));
        ui->gameBoard->placeStone(vertex, color);
        ui->textHistory->appendPlainText(QString("play %1 %2").arg(color).arg(vertex));
        captures(color);
        ret = true;
    }
    return ret;
}

bool MainWindow::fixed_handicap(int handicap){
    bool ret = false;
    QByteArray reply = engine.write( QString("fixed_handicap %1").arg(handicap));
    if(successful( reply )){
        QStringList verticies = getVerticies(reply, ret);
        if(ret){
            for(int i=0;i<verticies.length();i++){
                qDebug() <<i<<" "<<verticies[i];
                ui->gameBoard->placeStone(verticies[i], "black");
            }
        }
    }else{
        qDebug() << "fixed_handicap: "<< handicap <<" "<<reply;
    }
    return ret;
}

bool MainWindow::boardsize(int size){
    bool ret = false;
    QByteArray reply = engine.write( QString("boardsize %1").arg(size));
    if(successful( reply )){
        ret=true;
    }else{
        qDebug() << "boardsize: "<< reply;
    }
    return ret;
}

bool MainWindow::komi(qreal komi){
    bool ret = false;
    QByteArray reply = engine.write( QString("komi %1").arg(komi));
    if(successful( reply )){
        ret=true;
    }else{
        qDebug() << "komi: "<< reply;
    }
    return ret;
}

void MainWindow::updateBlackScore(){
    ui->labelBlack->setText(QString("Black: %1: captures:%2: score:%3").arg(blackName).arg(black_captures).arg(black_score));
}

void MainWindow::updateWhiteScore(){
    ui->labelWhite->setText(QString("White: %1: captures:%2: score:%3").arg(whiteName).arg(white_captures).arg(white_score));
}

void MainWindow::on_buttonHint_clicked()
{
    top_moves("black");
    //engine.write("top_moves_black");
}

void MainWindow::on_buttonPass_clicked()
{
    pass("black");
}

void MainWindow::on_buttonResign_clicked()
{
    final_score();
}

void MainWindow::on_actionNew_Game_triggered()
{
    if(!engine.is_running){
        engine.start();
    }
    engine.write("clear_board");
    boardsize( ui->gameBoard->boardSize );
    /*
    QByteArray cmd("boardsize ");
    cmd.append(QString("%1").arg(ui->gameBoard->boardSize));
    engine.write(cmd);
    */
    ui->gameBoard->clearBoard();

    fixed_handicap(handicap);
    /*
    cmd.clear();
    cmd.append(QString("fixed_handicap %1").arg(handicap));
    engine.write(cmd);

    cmd.clear();
    cmd.append(QString("komi %1").arg(komi));
    engine.write(cmd);
    */
    komi(komi_value);
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
    undo(1);
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
    undo(1);
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
void MainWindow::setBlackName(QString name){
    this->blackName = name;
    ui->labelBlack->setText(QString("%1 (Black)").arg(name));
}

void MainWindow::setWhiteName(QString name){
    this->whiteName = name;
    ui->labelWhite->setText(QString("%1 (White)").arg(name));
}

void MainWindow::setHandicap(int handicap){
    this->handicap = handicap;
}

void MainWindow::setKomi(qreal komi){
    this->komi_value = komi;
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

    config.beginGroup("Players");
    setBlackName(config.value("black_name").toString());
    setWhiteName(config.value("white_name").toString());
    setKomi(config.value("komi").toDouble());
    setHandicap(config.value("handicap").toInt());
    config.endGroup();
}
