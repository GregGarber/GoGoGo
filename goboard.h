#ifndef GOBOARD_H
#define GOBOARD_H
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QPixmap>

class GoBoard: public QGraphicsView
{
    Q_OBJECT
public:
    GoBoard(QWidget *parent);
    QGraphicsScene* scene;
    QGraphicsTextItem *text;
    QPixmap blackStonePM;
    QPixmap whiteStonePM;
    QPixmap backgroundPM;
    QGraphicsPixmapItem* boardBackground;
    QGraphicsPixmapItem* whiteStone;
    QGraphicsPixmapItem* blackStone;
};

#endif // GOBOARD_H
