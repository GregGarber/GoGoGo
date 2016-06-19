#include "goboard.h"
#include<QtGlobal>
#include <QGraphicsSceneMoveEvent>

GoBoard::GoBoard(QWidget *parent = 0) : QGraphicsView(parent)
{
    scene = new QGraphicsScene(-GO_BORDER_SIZE, -GO_BORDER_SIZE, GO_BOARD_SIZE, GO_BOARD_SIZE,this);
    setScene(scene);

    backgroundPM = QPixmap("WoodBoard1.png");
    whiteStonePM = QPixmap("WhiteStone.png");
    blackStonePM = QPixmap("BlackStone.png");
    blackStoneCursorPM = QPixmap("BlackStoneCursor.png");
    whiteStoneCursorPM = QPixmap("WhiteStoneCursor.png");

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

    redBrush = QBrush(Qt::red);
    redPen = QPen(redBrush,0);
    QGraphicsEllipseItem *el= scene->addEllipse(round(gridSizePixels*(qreal)(boardSize-1)), round(gridSizePixels*(qreal)(boardSize-1)), 10.0, 10.0, redPen, redBrush);
    center(el);
}

void GoBoard::center(auto *item){
    item->setPos( item->pos().rx()-(item->sceneBoundingRect().width()/2.0), item->pos().ry()-(item->sceneBoundingRect().height()/2.0));
}
void GoBoard::center(auto *item, QPointF p){
    item->setPos( p.rx()-(item->sceneBoundingRect().width()/2.0), p.ry()-(item->sceneBoundingRect().height()/2.0));
}
void GoBoard::center(auto *item, qreal x, qreal y){
    item->setPos( x-(item->sceneBoundingRect().width()/2.0), y-(item->sceneBoundingRect().height()/2.0));
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
        emit boardLeftClicked(playerColor, posToAlphaNum(pseudoCursor->data(REAL_POS).toPointF()));
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
           placeStone(str, color);
           qDebug() << "Add Missing STONE: "<< color << " " << str;
       }else{
           if(stoneHouse[str].getStoneColor() != color){
           qDebug() << "FIX miscolored STONE: "<< color << " " << str;
               removeStone(str);
               placeStone(str, color);
           }
       }
   }
}

void GoBoard::showTopMoves(QString color, QStringList verticies){
    QPointF pt;
    QPen pen(QColor(Qt::GlobalColor::red));
    QString marker_name = QString("%1_hints").arg(color);
    removeMarkers(marker_name);
    QGraphicsItemGroup *gig = new QGraphicsItemGroup(boardBackground);
    pen.setWidth(10);
    pen.setCosmetic(true);
    QFont font =QFont("Arial", 8, 9 );
        QBrush brush(QColor(0,0,0,127));
    for(int i=0; i<verticies.length(); i+=2){//every other is a score
        //QString strength = QString("%1 %2").arg(verticies[i]).arg(verticies[i+1]);
        QString strength = QString(" %1 ").arg(verticies[i+1]);
    font.setPixelSize((int)((float)gridSizePixels/(float)strength.length())*2);
        pt = alphaNumToPos(verticies[i]);
          QGraphicsSimpleTextItem *tx = scene->addSimpleText(strength, font);
          tx->setBrush(QColor(Qt::white));
          center(tx, pt);

        QGraphicsEllipseItem *el = scene->addEllipse( tx->sceneBoundingRect() ,pen,brush);

        el->setToolTip(strength);
        gig->addToGroup(el);
        gig->addToGroup(tx);
    }
    scene->addItem(gig);
    markers.insert(marker_name, gig);
}

void GoBoard::removeMarkers(QString name){
    if(markers.contains(name)){
        scene->removeItem(markers[name]);
        markers.remove(name);
    }
}


void GoBoard::drawBoard(){
    QBrush brush = QBrush(Qt::black);
    QPen pen  = QPen(brush,5);
    QString s;
    QFont font =QFont("Arial", 15, 9 );
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

void GoBoard::placeStone(QString location, QString color){
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
    new_stone->setToolTip(QString("Pos: %1\nColor: %2\nOrder: %3")
                          .arg(location.toUpper() )
                          .arg(color.at(0).toUpper()+color.mid(1))
                          .arg(( (uint)stoneHouse.size() +1) ) );
    if(stone.color != Blank){
        new_stone->setScale( ((qreal)gridSizePixels/(qreal)new_stone->sceneBoundingRect().width() ));
        QPointF p = alphaNumToPos(location);
        new_stone->setPos(p);
        center(new_stone);
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
