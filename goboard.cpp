#include "goboard.h"
#include<QtGlobal>

GoBoard::GoBoard(QWidget *parent = 0) : QGraphicsView(parent)
{
    //scene = new QGraphicsScene(this);
    //scene = new QGraphicsScene(-100,-100,2048,2048,this);
    scene = new QGraphicsScene(-GO_BORDER_SIZE, -GO_BORDER_SIZE, GO_BOARD_SIZE, GO_BOARD_SIZE,this);
    setScene(scene);

    //gridSizePixels = scene->width() / boardSize;
    gridSizePixels = GO_GRID_SIZE / boardSize;

    drawBoard();

    backgroundPM = QPixmap("WoodBoard1.png");
    whiteStonePM = QPixmap("WhiteStone.png");
    blackStonePM = QPixmap("BlackStone.png");

    bgBrush = scene->backgroundBrush();
    bgBrush.setTexture(backgroundPM);
    scene->setBackgroundBrush(bgBrush);

    whiteStone = scene->addPixmap(whiteStonePM);
    blackStone = scene->addPixmap(blackStonePM);
    whiteStone->setFlag(QGraphicsItem::ItemIsMovable);
    blackStone->setFlag(QGraphicsItem::ItemIsMovable);

    whiteStone->setPos(alphaNumToPos("A1"));
    blackStone->setPos(alphaNumToPos("B 2"));

}

void GoBoard::drawBoard(){
    QBrush brush = QBrush(Qt::black);
    QPen pen  = QPen(brush,8);
    QString s;
    QFont font =QFont("Arial", 15, 9 );
    font.setBold(true);
    for(qreal i=0.0; i < boardSize; i+=1.0){
        qreal p = i * (qreal)gridSizePixels;
        qreal max = GO_GRID_SIZE - (qreal)gridSizePixels;
        scene->addLine(0, p,  max, p, pen);
        scene->addLine(p, 0,p, max, pen);
        s=QString("%1").arg(i+1);
         scene->addText(s, font )->setPos(p-(gridSizePixels/2.0),max);
         scene->addText(s, font )->setPos(p-(gridSizePixels/2.0),-50.0);
         scene->addText(QString(QChar((int)i+65)), font)->setPos(max, p-(gridSizePixels/2.0));
         scene->addText(QString(QChar((int)i+65)), font )->setPos(-50.0, p-(gridSizePixels/2.0));
    }
}
void	GoBoard::resizeEvent(QResizeEvent *event){
    /*
    scene->setSceneRect(0.0,0.0,2048.0,2048.0);
    this->fitInView(0, 0, scene->width(), scene->height(), Qt::KeepAspectRatioByExpanding);
    */

    qreal max = qMin((qreal) viewport()->geometry().width()/scene->width(), (qreal)viewport()->geometry().height()/scene->height());
    //scene->setSceneRect(viewport()->geometry());
    this->fitInView(viewport()->geometry(), Qt::KeepAspectRatioByExpanding);
    this->scale(max,max);

    //gridSizePixels = (scene->width()-200) / boardSize;
    gridSizePixels = GO_GRID_SIZE / boardSize;
    QWidget::resizeEvent(event);
    qDebug() << "Got resize:" << event->size();
}

QPoint GoBoard::alphaNumToPos(QString alphanum){
    QString letter, number;
    int x,y;
    alphanum = alphanum.trimmed();
    //QRegularExpression re("(\\d\\d) (?<name>\\w+)");
    QRegularExpression re("(?<letter>\\w)\\s?(?<number>\\d\\d?)");
    QRegularExpressionMatch match = re.match(alphanum);
    if (match.hasMatch()) {
         number = match.captured("number");
         letter = match.captured("letter");
         qDebug() <<"number:"<<number<<" letter: "<<letter;
         y = number.toInt();
         x = letter.at(0).toUpper().toLatin1() - 64;
         qDebug() << " ( " << x << ", "<< y << ")";
         y = y * gridSizePixels;
         x = x * gridSizePixels;
         qDebug() <<"screen:" << " ( " << x << ", "<< y << ")";

    }else{
        qDebug() << "Could match: "<<alphanum;
    }

   return QPoint(x,y);
}
