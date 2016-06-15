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
    /*
    QPoint global=e->globalPos();
    QPointF sc = this->mapToScene(global);
    int x,y;

    x=gridSizePixels * ((int)( sc.rx()/gridSizePixels))*(GO_BOARD_SIZE/this->width());
    y=-100;
    x=100;
    global.setX(x);
    global.setY(x);
    global = mapFromScene(sc);
    global = mapToGlobal(global);
    */
    //not understanding this at all
    //cursor.setPos(global.rx(),global.rx());
    /*
    QPointF local = e->localPos();
    qDebug() << "Local (x,y) (" <<local.rx()<<", "<<local.ry()<<")"<< (int)( local.ry()/gridSizePixels);
    qDebug() << "Global (x,y) (" <<global.rx()<<", "<<global.ry()<<")"<< (int)( global.ry()/gridSizePixels);
    qDebug() << " type: " << e->GraphicsSceneHoverEnter;
    qDebug() << "Scene (x,y) (" <<sc.rx()<<", "<<sc.ry()<<")";
    qDebug() << QChar(65+((int)( sc.rx()/gridSizePixels)))<<" " << 19-((int)( sc.ry()/gridSizePixels)-6);
    */
    //qDebug() << posToAlphaNum(sc);
    QGraphicsView::mouseMoveEvent(e);
}

void GoBoard::mouseReleaseEvent(QMouseEvent *e){
    QPointF sc = this->mapToScene(e->pos());
    emit boardLeftClicked("black", posToAlphaNum(sc));
    QGraphicsView::mouseReleaseEvent(e);
}

QString GoBoard::posToAlphaNum(QPointF point){
    int i,j;
    QString ret;
    i = ((int)( point.rx()/gridSizePixels));
    j = 19-((int)( point.ry()/gridSizePixels));
    if(i > 7){
        i++;
    }
    ret = QString("%1%2").arg(QChar(65+i)).arg(j);
    //qDebug() << "posToAlphaNum i:"<<i<<" j:"<<j<< " ret:"<<ret;
    return ret;
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
         qDebug() <<"number:"<<number<<" letter: "<<letter;
         y = boardSize - number.toInt()+1;
         x = letter.at(0).toUpper().toLatin1() - 64;
         // looks like the letter I isn't supposed to be used... confusion with number 1?
         if(x > 8){
             x--;
         }
         x--; y--; //convert to zero-based counting for computing
         qDebug() << " ( " << x << ", "<< y << ")";
         y = y * gridSizePixels - (gridSizePixels/2.0);
         x = x * gridSizePixels- (gridSizePixels/2.0);//half subtracted to put stone on the lines
         qDebug() <<"screen:" << " ( " << x << ", "<< y << ")";

    }else{
        qDebug() << "Could match: "<<alphanum;
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
    delete stoneHouse.at(location).stone;
    stoneHouse.erase(location);
}

bool GoBoard::hasStone(QString location){
    location = location.trimmed().toLower();
    if ( stoneHouse.find(location) == stoneHouse.end() ) {
        return false;
    } else {
        return true;
    }
}

void GoBoard::clearBoard(){
//    for (std::map<char,int>::iterator it=mymap.begin(); it!=mymap.end(); ++it)

    for(auto it = stoneHouse.begin(); it != stoneHouse.end(); ++it){
        delete  it->second.stone;
    }
    stoneHouse.clear();
}

