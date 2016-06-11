#ifndef GOBOARD_H
#define GOBOARD_H
#include <QtCore>
#include <QtGui>

#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QDebug>

#define GO_BOARD_SIZE 1200
#define GO_GRID_SIZE 1000
#define GO_BORDER_SIZE (GO_BOARD_SIZE - GO_GRID_SIZE) /2

class GoBoard: public QGraphicsView
{
    Q_OBJECT
public:
    GoBoard(QWidget *parent);
    void drawBoard();
    QPoint alphaNumToPos(QString alphanum);

    //vars
    int boardSize = 19;
    int gridSizePixels = 0;
    QBrush bgBrush;
    QGraphicsScene* scene;
    QGraphicsTextItem *text;
    QPixmap blackStonePM;
    QPixmap whiteStonePM;
    QPixmap backgroundPM;
    QGraphicsPixmapItem* boardBackground;
    QGraphicsPixmapItem* whiteStone;
    QGraphicsPixmapItem* blackStone;
protected:
    //virtual void	resizeEvent(QResizeEvent *event);
    void	resizeEvent(QResizeEvent *event);
};

#endif // GOBOARD_H
