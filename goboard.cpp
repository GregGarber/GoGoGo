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
    this->setCursor(cursor);

    bgBrush = scene->backgroundBrush();
    bgBrush.setTexture(backgroundPM);
    scene->setBackgroundBrush(bgBrush);

}

void GoBoard::mouseMoveEvent(QMouseEvent *e){
    /* meh
    static int cnt=0;
    cnt++;
    if(cnt % 2 == 0){
        qDebug() <<" cnt:"<<cnt;
        */
    static QPoint last_loc = cursor.pos();
    QPoint global, tmp,offset;
    global = cursor.pos();//int
    tmp = global - last_loc;
    qDebug() << "Man: " <<tmp.manhattanLength();
    if(tmp.manhattanLength() > 0 ) { last_loc = global; return;}
    QPointF sc;
    QPointF local = e->localPos();
    tmp.setX((int)local.rx());
    tmp.setY((int)local.ry());
    QPointF s = mapToScene(tmp );
    sc = e->screenPos();//floaty
    offset = global - tmp;
    qDebug() << "Global (x,y) (" <<global.rx()<<", "<<global.ry()<<")";
    qDebug() << "Screen (x,y) (" <<sc.rx()<<", "<<sc.ry()<<")";
    qDebug() << "Offset(x,y) (" <<offset.rx()<<", "<<offset.ry()<<")";
    qDebug() << "Local(x,y) (" <<local.rx()<<", "<<local.ry()<<")";
    qDebug() << "Scene(x,y) (" <<s.rx()<<", "<<s.ry()<<")";
    qreal i = gridSizePixels * (int)round((qreal)( s.rx()/(qreal)gridSizePixels));
    qreal j = gridSizePixels * (int)round((qreal)( s.ry()/(qreal)gridSizePixels));
    QPointF new_point(i,j);
    QPoint loc = mapFromScene(new_point);
    //cursor.setPos(global);
    QGraphicsView::mouseMoveEvent(e);
    loc = loc + offset;
    cursor.setPos( loc);
    last_loc = global;
    /*
    if( loc.rx() > global.rx()) global.setX( global.rx() + 1);
    if( loc.rx() < global.rx()) global.setX( global.rx() - 1);
    if( loc.ry() > global.ry()) global.setY( global.ry() + 1);
    if( loc.ry() < global.ry()) global.setY( global.ry() - 1);
    cursor.setPos(global);
    */
    /*
    if( abs(global.rx()-loc.rx()) < 5 ) global.setX( (loc.rx()+(8.0*global.rx()))/9.0 );
    if( abs(global.ry()-loc.ry()) < 5 ) global.setY( (loc.ry()+(8.0*global.ry()))/9.0 );
    cursor.setPos(global);
    */
    /*
    int x,y;
    x=abs(global.rx()-loc.rx()) ;
    y=abs(global.ry()-loc.ry()) ;
    global.setX( (loc.rx()+(x*global.rx()))/(1.0+x) );
    global.setY( (loc.ry()+(y*global.ry()))/(1.0 +y) );
    cursor.setPos(global);
    */

//}
}

void GoBoard::mouseReleaseEvent(QMouseEvent *e){
    QPointF sc = this->mapToScene(e->pos());
    emit boardLeftClicked("black", posToAlphaNum(sc));
    QGraphicsView::mouseReleaseEvent(e);
}

QString GoBoard::posToAlphaNum(QPointF point){
    int i,j;
    QString ret;
    i = (int)round((qreal)( point.rx()/(qreal)gridSizePixels));
    j = 19-(int)round((qreal)( point.ry()/(qreal)gridSizePixels));
    if(i > 7){
        i++;
    }
    ret = QString("%1%2").arg(QChar(65+i)).arg(j);
    //qDebug() << "posToAlphaNum i:"<<i<<" j:"<<j<< " ret:"<<ret;
    return ret;
}

void GoBoard::checkStones(QString colour, QStringList verticies){
   foreach (const QString &str, stoneHouse.keys()){
       if(stoneHouse[str].getStoneColor() == colour){
           if( !verticies.contains(str)) {
               qDebug() << "REMOVE STONE: "<< colour << " " << str;
               removeStone(str);
           }
       }
   }
   foreach(const QString &str, verticies){
       if(!stoneHouse.contains(str)){
           placeStone(str, colour);
           qDebug() << "Add Missing STONE: "<< colour << " " << str;
       }else{
           if(stoneHouse[str].getStoneColor() != colour){
           qDebug() << "FIX miscoloured STONE: "<< colour << " " << str;
               removeStone(str);
               placeStone(str, colour);
           }
       }
   }
}

void GoBoard::showTopMoves(QString colour, QStringList verticies){
    QPoint pt;
    QPen pen(QColor(Qt::GlobalColor::red));
    QGraphicsItemGroup *gig = new QGraphicsItemGroup(boardBackground);
    //QBrush brush(QColor(Qt::GlobalColor::black));
                 pen.setWidth(10);
    QFont font =QFont("Arial", 25, 9 );
    for(int i=0; i<verticies.length(); i+=2){//every other is a score
    QBrush brush(QColor(255,255,0,(3*verticies[i+1].toInt())));
        pt = alphaNumToPos(verticies[i]);
        QGraphicsEllipseItem *el = scene->addEllipse( (qreal)pt.rx(), (qreal) pt.ry(), 50.0,50.0,pen,brush);
        QString strength = QString("%1").arg(verticies[i+1]);
        el->setToolTip(strength);
        QGraphicsTextItem *tx = scene->addText(strength,font);
    //    tx->setDefaultTextColor(Qt::red);
        ///tx->setPos((qreal)pt.rx(),(qreal)pt.ry());
        //gig->addToGroup(tx);
        gig->addToGroup(el);
   }
    scene->addItem(gig);
    //markers.insert("black_hints",   gig->toGraphicsObject());
    markers.insert("black_hints",   gig);
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
    new_stone->setToolTip(QString("Pos: %1\nColour: %2\nOrder: %3")
                          .arg(location.toUpper() )
                          .arg(color.at(0).toUpper()+color.mid(1))
                          .arg(( (uint)stoneHouse.size() +1) ) );
    if(stone.color != Blank){
        new_stone->setScale( ((qreal)gridSizePixels/(qreal)new_stone->boundingRect().width() ));
        new_stone->setFlag(QGraphicsItem::ItemIsMovable);
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

