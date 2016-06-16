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
#include <map>

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
    QString posToAlphaNum(QPointF point);
    void placeStone(QString location, QString color);
    void removeStone(QString location);
    void checkStones(QString colour, QStringList verticies);
    void showTopMoves(QString colour, QStringList verticies);
    bool hasStone(QString location);
    void clearBoard();
    void removeMarkers(QString name);
    //QMap<QString,QGraphicsObject*> markers;
    QMap<QString, QGraphicsItemGroup*> markers;

    //vars
    QCursor cursor;
    enum StoneColors {
        Black,
        White,
        Blank
    };
    enum StoneData {
        STONE_DATA_COLOUR,
        STONE_DATA_VERTEX
    };

    struct Stone {
        QGraphicsPixmapItem* stone;
        StoneColors color;
        void setColor(QString c){
            c = c.trimmed().toLower();
            if(c == "white"){
                color = White;
            }else if( c=="black"){
                color = Black;
            }else{
                color = Blank;
            }
        }
        QString getColorText(StoneColors c ){
            if(c == Black ) return "black";
            if(c == Blank ) return "blank";
            if(c == White ) return "white";
        }
        QString getStoneColor(){ return getColorText(color); }
    };

    //std::map<QString, Stone> stoneHouse;
    QMap<QString, Stone> stoneHouse;
    int boardSize = 19;
    int gridSizePixels = 0;
    QBrush bgBrush;
    QGraphicsScene* scene;
    QGraphicsTextItem *text;
    QPixmap blackStonePM;
    QPixmap blackStoneCursorPM;
    QPixmap whiteStonePM;
    QPixmap backgroundPM;
    QGraphicsPixmapItem* boardBackground;
    QGraphicsPixmapItem* whiteStone;
    QGraphicsPixmapItem* blackStone;
signals:
    void boardLeftClicked(QString colour, QString vertex);

protected:
    void	resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

};

#endif // GOBOARD_H
