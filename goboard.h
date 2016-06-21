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

#define GO_BOARD_SIZE 1207
#define GO_GRID_SIZE 1007
#define GO_BORDER_SIZE (GO_BOARD_SIZE - GO_GRID_SIZE) /2

class GoBoard: public QGraphicsView
{
    Q_OBJECT
public:
    GoBoard(QWidget *parent);
    bool isOnBoard(qreal i, qreal j);
    bool isOnBoard(QPointF j);
    void drawBoard();
    QPointF alphaNumToPos(QString alphanum);
    QString posToAlphaNum(QPointF point);
    void placeStone(QString location, QString color);
    void removeStone(QString location);
    void checkStones(QString color, QStringList verticies);
    void showTopMoves(QString color, QStringList verticies);
    bool hasStone(QString location);
    void clearBoard();
    void removeMarkers(QString name);
    void readSettings();
    void center(auto *item);
    void center(auto *item, QPointF p);
    void center(auto *item, qreal x, qreal y);
    QMap<QString, QGraphicsItemGroup*> markers;
    QSettings config;

    //vars
    enum PseudoCursorData{
        HALF_SIZE,
        REAL_POS
    };

    QCursor cursor;
    enum StoneColors {
        Black,
        White,
        Blank
    };
    enum StoneData {
        STONE_DATA_COLOR,
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
            return "unknown";
        }
        QString getStoneColor(){ return getColorText(color); }
    };

    //std::map<QString, Stone> stoneHouse;
    QMap<QString, Stone> stoneHouse;
    int boardSize = 9;
    qreal gridSizePixels = 0.0;
    QString playerColor="black";
    QBrush bgBrush;
    QBrush redBrush;
    QPen redPen;
    QGraphicsScene* scene;
    QGraphicsTextItem *text;
    QPixmap blackStonePM;
    QPixmap blackStoneCursorPM;
    QPixmap whiteStoneCursorPM;
    QPixmap whiteStonePM;
    QPixmap backgroundPM;
    QGraphicsPixmapItem* boardBackground;
    QGraphicsPixmapItem* pseudoCursor;

    public slots:
    void doMenu(QPoint p);

signals:
    void boardLeftClicked(QString color, QString vertex);


protected:
    void	resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

};

#endif // GOBOARD_H
