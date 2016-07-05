#include "goboard.h"
#include<QtGlobal>
#include<QMenu>
#include <QGraphicsSceneMoveEvent>
#include <algorithm>
#include <string>
#include <array>

GoBoard::GoBoard(QWidget *parent = 0) : QGraphicsView(parent)
{
    scene = new QGraphicsScene(-GO_BORDER_SIZE, -GO_BORDER_SIZE, GO_BOARD_SIZE, GO_BOARD_SIZE,this);
    setScene(scene);
    pixelScale = this->devicePixelRatioF();
    backgroundPM = QPixmap(":/images/WoodBoard1.png");
    whiteStonePM = QPixmap(":/images/WhiteStone.png");
    blackStonePM = QPixmap(":/images/BlackStone.png");
    blackStoneCursorPM = QPixmap(":/images/BlackStoneCursor.png");
    whiteStoneCursorPM = QPixmap(":/images/WhiteStoneCursor.png");

    cursor = QCursor(blackStoneCursorPM);
    cursor.setShape(Qt::BlankCursor);
    this->setCursor(cursor);

    bgBrush = scene->backgroundBrush();
    bgBrush.setTexture(backgroundPM);
    scene->setBackgroundBrush(bgBrush);

    readSettings();
    if(playerColor == "black"){
        pseudoCursor = scene->addPixmap(blackStoneCursorPM);
    }else{
        pseudoCursor = scene->addPixmap(whiteStoneCursorPM);
    }
    pseudoCursor->setScale(2.0);
    //boundingRect() doesn't account for scaling
    //sceneBoundingRect does count scaling
    qreal i= pseudoCursor->sceneBoundingRect().width()/2.0;
    qreal j= pseudoCursor->sceneBoundingRect().height()/2.0;
    pseudoCursor->setData(PseudoCursorData::HALF_SIZE, QPointF(i,j));

}

bool GoBoard::isOnBoard(QPointF r){
    qreal size = gridSizePixels*(qreal)(boardSize-1);
    return QRectF(0,0,size,size).contains(r);
}

bool GoBoard::isOnBoard(qreal i, qreal j)
{
    qreal size = gridSizePixels*(qreal)(boardSize-1);
    return QRectF(0,0,size,size).contains(i,j);
}


float GoBoard::randy(float max, float min){
    float diff = max-min;
    return (diff*(float)((float)qrand()/(float)RAND_MAX))+min;
}


QVector<QPen> GoBoard::makePenSet(int count, QPen example ){
    QVector<QPen> pens;
    for(int i=0; i<count; i++){
        QPen pen = QPen(example);
        pen.setColor( QColor(randy(), randy(), randy(), 255) );
        pens.append(pen);
    }
    return pens;
}

QVector<QPen> GoBoard::makePenSet(int count, QPen example, QVector<QColor> colors ){
    QVector<QPen> pens;
    for(int i=0; i<count; i++){
        QPen pen = QPen(example);
        pen.setColor( colors.at(i % colors.length() ) );
        pens.append(pen);
    }
    return pens;
}

// for sorting strings by length
struct compare {
    bool operator()(const QString& first, const QString& second) {
        return first.size() < second.size();
    }
};
//Takes line or lines of vectors
void GoBoard::showDragonLike(QVector<QString> stones,
                             QString name,
                             QVector<QPen> pens,
                             bool do_lines,
                             GoBoard::MarkerMarks mark_type){
    gig = new QGraphicsItemGroup();
    compare c;
    std::sort(stones.begin(), stones.end(),c);
    for(int i=stones.length(); i>0; i--){
        QString stone_list = stones.at(i-1);
        QStringList verticies = stone_list.split(" ", QString::SkipEmptyParts);
        QPainterPath p;
        QPen stone_pen = pens.at((i-1)%pens.length());
        for(int j=0; j<verticies.length(); j++){
            if(j==0){
                p.moveTo(alphaNumToPos(verticies[j]));
            }else{
                if(do_lines){
                    p.lineTo(alphaNumToPos(verticies[j]));
                }
            }
            QPointF pt = alphaNumToPos(verticies[j]);
            qreal sz = gridSizePixels*1.1;
            qreal cntr = sz/2.0;
            switch(mark_type){
            case MarkEllipse:
                p.addEllipse(pt,cntr,cntr);//radius so not sz
                break;
            case MarkRect:
                p.addRect(pt.rx()-cntr, pt.ry()-cntr, sz, sz);
                break;
            case MarkRoundedRect:
                p.addRoundRect(pt.rx(), pt.ry(), sz,sz, 3);
                break;
            }
            //don't want next line to start at edge of previous shape
            p.moveTo(alphaNumToPos(verticies[j]));
        }
        QGraphicsPathItem* path = scene->addPath(p, stone_pen);
        path->setZValue(-10.0*i);

        gig->addToGroup(path );
    }
    scene->addItem(gig);
    markers.insert(name, gig);

}

void GoBoard::dragonStones(QVector<QString> dragons){
    QPen default_pen;
    //default_pen.setStyle(Qt::DashLine);
    default_pen.setWidth(10);
    QVector<QPen> pens = makePenSet(dragons.length(), default_pen);
    removeMarkers("dragons");
    showDragonLike(dragons, "dragons", pens,true,MarkRect);
}

void GoBoard::wormStones(QVector<QString> worms){
    QPen default_pen;
    default_pen.setStyle(Qt::DashDotDotLine);
    default_pen.setWidth(5);
    QVector<QPen> pens = makePenSet(worms.length(), default_pen);
    removeMarkers("worms");
    showDragonLike(worms, "worms", pens);
}

void GoBoard::finalStatusList(QVector<QString> verticies, QString type){
    QPen default_pen;
    QVector<QPen> pens;
    MarkerMarks marker = MarkRect;
    if(type.contains("black_territory", Qt::CaseInsensitive)){
        default_pen.setColor(Qt::black);
        default_pen.setWidth(10);
    }else if(type.contains("white_territory", Qt::CaseInsensitive)){
        default_pen.setColor(Qt::white);
        default_pen.setWidth(10);
    }else if(type.contains("dame", Qt::CaseInsensitive)){
        default_pen.setStyle(Qt::DashLine);
        default_pen.setColor(Qt::yellow);
        marker = MarkEllipse;
        default_pen.setWidth(3);
    }else if(type.contains("seki", Qt::CaseInsensitive)){
        default_pen.setStyle(Qt::DashLine);
        default_pen.setColor(Qt::blue);
        marker = MarkEllipse;
        default_pen.setWidth(3);
    }else if(type.contains("alive", Qt::CaseInsensitive)){
        default_pen.setStyle(Qt::DashDotDotLine);
        default_pen.setColor(Qt::green);
        default_pen.setWidth(5);
        marker = MarkEllipse;
    }else if(type.contains("dead", Qt::CaseInsensitive)){
        default_pen.setStyle(Qt::DashDotDotLine);
        default_pen.setColor(Qt::red);
        default_pen.setWidth(5);
        marker = MarkEllipse;
    }

    pens.append(default_pen);
    //QVector<QPen> pens = makePenSet(verticies.length(), default_pen);
    removeMarkers(type);
    showDragonLike(verticies, type, pens,false, marker);
}

void GoBoard::clearAllMarkers(){
    while(!markers.empty()){
        removeMarkers(markers.begin().key());
    }
}

void GoBoard::mouseMoveEvent(QMouseEvent *e){
    QPoint tmp;
    QPointF local_pos = e->localPos();
    QPointF scene_pos;
    tmp.setX((int)local_pos.rx());
    tmp.setY((int)local_pos.ry());
    scene_pos = mapToScene(tmp );//why can't this take float?
    qreal i = gridSizePixels * round((qreal) scene_pos.rx()/gridSizePixels);
    qreal j = gridSizePixels * round((qreal) scene_pos.ry()/gridSizePixels);
    QPointF pos(i,j);
    pseudoCursor->setData(REAL_POS, pos);
    if(isOnBoard(pos)){
        pseudoCursor->show();
        cursor.setShape(Qt::BlankCursor);
        setCursor(cursor);
    }else{
        pseudoCursor->hide();
        cursor.setShape(Qt::ArrowCursor);
        setCursor(cursor);
    }
    center(pseudoCursor, pos);
    QGraphicsView::mouseMoveEvent(e);
}

void GoBoard::mouseReleaseEvent(QMouseEvent *e){
    if( isOnBoard(pseudoCursor->data(REAL_POS).toPointF())){
        //emit boardLeftClicked(playerColor, posToAlphaNum(pseudoCursor->data(REAL_POS).toPointF()));
        emit boardLeftClicked(posToAlphaNum(pseudoCursor->data(REAL_POS).toPointF()));
    }
    QGraphicsView::mouseReleaseEvent(e);
}

QString GoBoard::posToAlphaNum(QPointF point){
    int i,j;
    QString ret;
    i = (int)round((qreal)( point.rx()/(qreal)gridSizePixels));
    j = boardSize-(int)round((qreal)( point.ry()/(qreal)gridSizePixels));
    if(i > 7){
        i++;
    }
    ret = QString("%1%2").arg(QChar(65+i)).arg(j);
    return ret;
}

void GoBoard::checkStones(QString color, QStringList verticies){
    qDebug() << "checkStones("<<color<<", "<<verticies<<")";
   foreach (const QString &str, stoneHouse.keys()){
       if(stoneHouse[str].getStoneColor() == color){
           if( !verticies.contains(str)) {
               qDebug() << "REMOVE STONE: "<< color << " " << str;
               removeStone(str);
           }
       }
   }
   foreach(const QString &str, verticies){
       if(!stoneHouse.contains(str)){
           placeStone(color, str);
           qDebug() << "Add Missing STONE: "<< color << " " << str;
       }else{
           if(stoneHouse[str].getStoneColor() != color){
           qDebug() << "FIX miscolored STONE: "<< color << " " << str;
               removeStone(str);
               placeStone( color, str);
           }
       }
   }
}
GoBoard::~GoBoard(){
    //segfault here. Show worms, clear worms, quit.
    //delete gig;
    //may not need it anymore since it is now part of remove stones
    clearAllMarkers();//I think this works instead.
}

void GoBoard::showTopMoves(const QString color, QStringList verticies){
    QPointF pt;
    QPen pen = QPen(QColor(Qt::black));
    QString marker_name = QString("%1_hints").arg(color);
    removeMarkers(marker_name);
    gig = new QGraphicsItemGroup();
    pen.setWidth(1);
    pen.setCosmetic(true);
    QFont font =QFont("Arial", 8*pixelScale, 9 );
    QFont big_font = QFont("Arial", 15*pixelScale, 10);
    QBrush brush(QColor(0,0,0,255));
    big_font.setUnderline(true);
    big_font.setPointSize(12*pixelScale);
    const QString h = QString("%1 Hints\nVertex\tRating").arg(color.toUpper());
    QGraphicsTextItem* heading = scene->addText( QString(h), big_font);
    heading->setPos(1050.0, -75.0);
    big_font.setUnderline(false);
    big_font.setPointSize(15*pixelScale);
    QString vertex,line;
    QString score;//a float
    QStringList tmp;
    for(int i=0; i<verticies.length(); i++){
        line = verticies.at(i);
        tmp=line.split(" ", QString::SkipEmptyParts);
        vertex = tmp.at(0);
        score = tmp.at(1);
        font.setPixelSize((int)((float)gridSizePixels/(float)score.length())*2);
        pt = alphaNumToPos(vertex);
        QGraphicsSimpleTextItem *tx = scene->addSimpleText(score, font);
        tx->setBrush(QColor(Qt::white));
        center(tx, pt);
        QGraphicsRectItem *el = scene->addRect(tx->sceneBoundingRect(),pen,brush);
        QGraphicsTextItem *tx2 = scene->addText(line.replace(" ","\t") , big_font);
        tx2->setPos(1100.0, i*30.0);
        el->setToolTip(line);
        gig->addToGroup(el);
        gig->addToGroup(tx);
        gig->addToGroup(heading);
        gig->addToGroup(tx2);
    }
    scene->addItem(gig);
    markers.insert(marker_name, gig);
}

void GoBoard::removeMarkers(QString name){
    if(markers.contains(name)){
        scene->removeItem(markers[name]);
        delete markers[name];
        markers.remove(name);
    }
}


void GoBoard::drawBoard(){
    QBrush brush = QBrush(Qt::black);
    QPen pen  = QPen(brush,5);
    QString s;
    QFont font =QFont("Arial", 15*pixelScale, 9 );
    QGraphicsTextItem *txt;
    font.setBold(true);
    for(qreal i=0.0; i < boardSize; i+=1.0){
        qreal p = i * (qreal)gridSizePixels;
        qreal max = GO_GRID_SIZE - (qreal)gridSizePixels;
        scene->addLine(0, p,  max, p, pen);
        scene->addLine(p, 0,p, max, pen);
        s=QString("%1").arg(boardSize - i);
        txt = scene->addText(s, font);
        center(txt, max+50, p);
        txt = scene->addText(s, font);
        center(txt, -50, p);
         // looks like the letter I isn't supposed to be used... confusion with number 1?
         if(i > 7){
            s = QString(QChar((int)i+66));//make a letter (J-T)
         }else{
            s = QString(QChar((int)i+65));//make a letter (A-H)
         }
        txt = scene->addText(s, font);
        center(txt, p, max+50);
        txt = scene->addText(s, font);
        center(txt, p, -50);
    }
}

void	GoBoard::resizeEvent(QResizeEvent *event){
    qreal max = qMin((qreal) viewport()->geometry().width()/scene->width(), (qreal)viewport()->geometry().height()/scene->height());
    this->fitInView(viewport()->geometry(), Qt::KeepAspectRatioByExpanding);
    this->scale(max,max);
    gridSizePixels = GO_GRID_SIZE / boardSize;
    QWidget::resizeEvent(event);
}

QPointF GoBoard::alphaNumToPos(QString alphanum){
    QString letter, number;
    qreal x,y;
    alphanum = alphanum.trimmed();
    QRegularExpression re("(?<letter>\\w)\\s?(?<number>\\d\\d?)");
    QRegularExpressionMatch match = re.match(alphanum);
    if (match.hasMatch()) {
         number = match.captured("number");
         letter = match.captured("letter");
         y = boardSize - number.toInt()+1;
         x = letter.at(0).toUpper().toLatin1() - 64;
         // looks like the letter I isn't supposed to be used... confusion with number 1?
         if(x > 8){
             x--;
         }
         x--; y--; //convert to zero-based counting for computing
         y = y * gridSizePixels ;
         x = x * gridSizePixels;
    }else{
        qDebug() << "Couldn't match: "<<alphanum;
    }

   return QPoint(x,y);
}

void GoBoard::placeStone( QString color, QString location){
    location = location.trimmed().toLower();
    color = color.trimmed().toLower();
    Stone stone;
    stone.setColor(color);
    QGraphicsPixmapItem* new_stone;
    if( stone.color == Black){
        new_stone = scene->addPixmap(blackStonePM);
    }else if( stone.color == White){
        new_stone = scene->addPixmap(whiteStonePM);

    }
    /* mysterious segfault involving qstring
    new_stone->setToolTip(QString("Pos: %1\nColor: %2\nOrder: %3")
                          .arg(location.toUpper() )
                          .arg(color.at(0).toUpper()+color.mid(1))
                          .arg(( (uint)stoneHouse.size() +1) ) );
                          */
    if(stone.color != Blank){
        new_stone->setScale( ((qreal)gridSizePixels/(qreal)new_stone->sceneBoundingRect().width() ));
        QPointF p = alphaNumToPos(location);
        new_stone->setPos(p);
        center(new_stone);
        if(stone.color==White){
        //QBrush greenBrush = QBrush(Qt::green);
        highlight = new Highlight(new_stone);
        highlight->setDiameter(gridSizePixels);
        highlight->setPos(p.rx(), p.ry());
        scene->addItem(highlight);
        QPropertyAnimation *a = new QPropertyAnimation(highlight,"time");
        a->setDuration(2000);
        a->setStartValue(0.0);
        a->setEndValue(5.0); //do the physics in seconds
        highlight->setTotalTime(5.0);//sometimes need to know this
        //a->setEasingCurve(QEasingCurve::OutBack);
        a->start(QPropertyAnimation::DeleteWhenStopped);
        }

        //center(highlight);
        // highlight isn't centered, something to do with pen width
        // need sound of stones hitting board
        // can hint labels be rotated? different text colors?
        //might be nice to go whole hog with animating stones
        // need stars
        //highlight top hint
        //looks like it can only one effect at a time
        //QGraphicsBlurEffect *blur = new QGraphicsBlurEffect(this);
        //highlight->setGraphicsEffect(blur);

/*
        goe = new QGraphicsOpacityEffect(this);
        highlight->setGraphicsEffect(goe);
        QPropertyAnimation *a = new QPropertyAnimation(goe,"opacity");
        a->setDuration(8000);
        a->setStartValue(1);
        a->setEndValue(0);
        a->setEasingCurve(QEasingCurve::OutBack);
        a->start(QPropertyAnimation::DeleteWhenStopped);
        */
        //}

        /*
        QPropertyAnimation *b = new QPropertyAnimation(highlight,"geometry");
        b->setDuration(10000);
        //b->setStartValue(QRect(0, 0, 100, 30));
        b->setStartValue(highlight->boundingRect());
        b->setEndValue(QRect(250, 250, 100, 30));
        QSequentialAnimationGroup *gr = new QSequentialAnimationGroup;
        */


        stone.stone = new_stone;
        if(hasStone(location)) removeStone(location);//can only be one stone per location. NO, should return invalid move or something
        stoneHouse[location]=stone;
    }else{
        //place blank is synonym for remove stone
        if(hasStone(location)) removeStone(location);//can only be one stone per location. NO, should return invalid move or something
    }
}

void GoBoard::removeStone(QString location){
    location = location.trimmed().toLower();
    delete stoneHouse[location].stone;
    stoneHouse.remove(location);
}

bool GoBoard::hasStone(QString location){
    return stoneHouse.contains(location.trimmed().toLower());
}

void GoBoard::clearBoard(){
    for(auto it = stoneHouse.begin(); it != stoneHouse.end(); ++it){
        delete  it->stone;
    }
    stoneHouse.clear();
}

void GoBoard::readSettings(){
    config.beginGroup("GoGoGo");
    QString bs = config.value("board_size", QString("19x19")).toString();
    if(bs=="19x19"){
        boardSize=19;
    }else if(bs == "13x13"){
        boardSize = 13;
    }else if(bs == "9x9"){
        boardSize=9;
    }else{
        boardSize=19;
    }

    config.endGroup();

    gridSizePixels = ((qreal)GO_GRID_SIZE / (qreal)boardSize);
    drawBoard();

}
