#include "goboard.h"

GoBoard::GoBoard(QWidget *parent = 0) : QGraphicsView(parent)
{
scene = new QGraphicsScene(this);
setScene(scene);
text = scene->addText("bogotobogo.com", QFont("Arial", 20) );
backgroundPM = QPixmap("WoodBoard1.png");
whiteStonePM = QPixmap("WhiteStone.png");
blackStonePM = QPixmap("BlackStone.png");

boardBackground = scene->addPixmap(backgroundPM);
boardBackground->setScale(0.5);
whiteStone = scene->addPixmap(whiteStonePM);
blackStone = scene->addPixmap(blackStonePM);
whiteStone->setPos(QPoint(100,100));
whiteStone->setScale(0.15);
blackStone->setScale(0.15);

// movable text
text->setFlag(QGraphicsItem::ItemIsMovable);
}
