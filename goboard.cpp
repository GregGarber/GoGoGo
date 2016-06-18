#include "goboard.h"
#include<QtGlobal>
#include <QGraphicsSceneMoveEvent>

GoBoard::GoBoard(QWidget *parent = 0) : QGraphicsView(parent)
{
    scene = new QGraphicsScene(-GO_BORDER_SIZE, -GO_BORDER_SIZE, GO_BOARD_SIZE, GO_BOARD_SIZE,this);
    //connect(scene,SIGNAL())
    setScene(scene);

    gridSizePixels = GO_GRID_SIZE / boardSize;

    drawBoard();

    backgroundPM = QPixmap("WoodBoard1.png");
    whiteStonePM = QPixmap("WhiteStone.png");
    blackStonePM = QPixmap("BlackStone.png");
    blackStoneCursorPM = QPixmap("BlackStoneCursor.png");
    cursor = QCursor(blackStoneCursorPM);
    cursor.setShape(Qt::BlankCursor);
    this->setCursor(cursor);

    bgBrush = scene->backgroundBrush();
    bgBrush.setTexture(backgroundPM);
    scene->setBackgroundBrush(bgBrush);

    //experiment
    blackStone = scene->addPixmap(blackStoneCursorPM);
}

bool GoBoard::isOnBoard(qreal i, qreal j)
{
    return QRectF(0,0,GO_GRID_SIZE-gridSizePixels-10,GO_GRID_SIZE-gridSizePixels-10).contains(i,j);
}
void GoBoard::mouseMoveEvent(QMouseEvent *e){
    QPoint tmp;
    QPointF local_pos = e->localPos();
    QPointF scene_pos;
    tmp.setX((int)local_pos.rx());
    tmp.setY((int)local_pos.ry());
    scene_pos = mapToScene(tmp );//why can't this take float?
    qreal i = gridSizePixels * (int)round((qreal)( scene_pos.rx()/(qreal)gridSizePixels));
    qreal j = gridSizePixels * (int)round((qreal)( scene_pos.ry()/(qreal)gridSizePixels));
    if(isOnBoard(i,j)){
        blackStone->show();
        cursor.setShape(Qt::BlankCursor);
        setCursor(cursor);
    }else{
        blackStone->hide();
        cursor.setShape(Qt::ArrowCursor);
        setCursor(cursor);
    }
    i-= blackStone->pixmap().width()/2;
    j-= blackStone->pixmap().height()/2;
    blackStone->setPos(i,j);
    QGraphicsView::mouseMoveEvent(e);
}

void GoBoard::mouseReleaseEvent(QMouseEvent *e){
    QPointF sc = this->mapToScene(e->pos());
    if(isOnBoard(sc.rx(), sc.ry())){
        emit boardLeftClicked("black", posToAlphaNum(sc));
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
    //qDebug() << "posToAlphaNum i:"<<i<<" j:"<<j<< " ret:"<<ret;
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
    QPoint pt;
    QPen pen(QColor(Qt::GlobalColor::red));
    QGraphicsItemGroup *gig = new QGraphicsItemGroup(boardBackground);
    pen.setWidth(10);
    QFont font =QFont("Arial", 25, 9 );
    for(int i=0; i<verticies.length(); i+=2){//every other is a score
        QBrush brush(QColor(255,255,0,(2*verticies[i+1].toInt())));
        QString strength = QString("%1 %2").arg(verticies[i]).arg(verticies[i+1]);
        pt = alphaNumToPos(verticies[i]);
        QGraphicsEllipseItem *el = scene->addEllipse( (qreal)pt.rx(), (qreal) pt.ry(), 50.0,50.0,pen,brush);
                el->setPos( el->pos().rx() + (el->rect().width()/2.0),
                            el->pos().ry() + (el->rect().height()/2.0)
                            );
          QGraphicsSimpleTextItem *tx = scene->addSimpleText(strength, font);
          tx->setPos(pt.rx()+(tx->boundingRect().width()/2.0), pt.ry()+(tx->boundingRect().height()/2.0));

        el->setToolTip(strength);
        gig->addToGroup(el);
        gig->addToGroup(tx);
    }
    scene->addItem(gig);
    markers.insert(QString("%1_hints").arg(color), gig);
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
    font.setBold(true);
    for(qreal i=0.0; i < boardSize; i+=1.0){
        qreal p = i * (qreal)gridSizePixels;
        qreal max = GO_GRID_SIZE - (qreal)gridSizePixels;
        scene->addLine(0, p,  max, p, pen);
        scene->addLine(p, 0,p, max, pen);
        s=QString("%1").arg(boardSize - i);
         scene->addText(s, font )->setPos(max, p-(gridSizePixels/2.0));
         scene->addText(s, font )->setPos(-50.0, p-(gridSizePixels/2.0));
         // looks like the letter I isn't supposed to be used... confusion with number 1?
         if(i > 7){
            s = QString(QChar((int)i+66));//make a letter (J-T)
         }else{
            s = QString(QChar((int)i+65));//make a letter (A-H)
         }
         scene->addText(s, font)->setPos(p-(gridSizePixels/2.0), max );
         scene->addText(s, font )->setPos(p-(gridSizePixels/2.0), -50.0);
    }
}

void	GoBoard::resizeEvent(QResizeEvent *event){
    qreal max = qMin((qreal) viewport()->geometry().width()/scene->width(), (qreal)viewport()->geometry().height()/scene->height());
    this->fitInView(viewport()->geometry(), Qt::KeepAspectRatioByExpanding);
    this->scale(max,max);
    gridSizePixels = GO_GRID_SIZE / boardSize;
    QWidget::resizeEvent(event);
}

QPoint GoBoard::alphaNumToPos(QString alphanum){
    QString letter, number;
    int x,y;
    alphanum = alphanum.trimmed();
    QRegularExpression re("(?<letter>\\w)\\s?(?<number>\\d\\d?)");
    QRegularExpressionMatch match = re.match(alphanum);
    if (match.hasMatch()) {
         number = match.captured("number");
         letter = match.captured("letter");
         //qDebug() <<"number:"<<number<<" letter: "<<letter;
         y = boardSize - number.toInt()+1;
         x = letter.at(0).toUpper().toLatin1() - 64;
         // looks like the letter I isn't supposed to be used... confusion with number 1?
         if(x > 8){
             x--;
         }
         x--; y--; //convert to zero-based counting for computing
         //qDebug() << " ( " << x << ", "<< y << ")";
         y = y * gridSizePixels - (gridSizePixels/2.0);
         x = x * gridSizePixels- (gridSizePixels/2.0);//half subtracted to put stone on the lines
         //qDebug() <<"screen:" << " ( " << x << ", "<< y << ")";

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
        new_stone->setScale( ((qreal)gridSizePixels/(qreal)new_stone->boundingRect().width() ));
        //new_stone->setFlag(QGraphicsItem::ItemIsMovable);
        new_stone->setPos(alphaNumToPos(location));
        stone.stone = new_stone;
        if(hasStone(location)) removeStone(location);//can only be one stone per location
        stoneHouse[location]=stone;
    }else{
        //place blank is synonym for remove stone
        if(hasStone(location)) removeStone(location);//can only be one stone per location
    }
}

void GoBoard::removeStone(QString location){
    location = location.trimmed().toLower();
    delete stoneHouse[location].stone;
    stoneHouse.remove(location);
}

bool GoBoard::hasStone(QString location){
    location = location.trimmed().toLower();
    //if ( stoneHouse.find(location) == stoneHouse.end() ) {
    if(stoneHouse.contains(location)){
        return true;
    } else {
        return false;
    }
}

void GoBoard::clearBoard(){
//    for (std::map<char,int>::iterator it=mymap.begin(); it!=mymap.end(); ++it)

    for(auto it = stoneHouse.begin(); it != stoneHouse.end(); ++it){
        delete  it->stone;
    }
    stoneHouse.clear();
}

