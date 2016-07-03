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
    qreal pixelScale; //intended to scale for HDPI monitor. Not certain it is correct.
    GoBoard(QWidget *parent);
    ~GoBoard();
    bool isOnBoard(qreal i, qreal j);
    bool isOnBoard(QPointF j);
    enum MarkerMarks{
        MarkEllipse,
        MarkRect,
        MarkRoundedRect
    };
    void dragonStones(QVector<QString> dragons);
    void wormStones(QVector<QString> worms);
    float randy(float max=255.0, float min=0.0);
    QVector<QPen> makePenSet(int count, QPen example );
    QVector<QPen> makePenSet(int count, QPen example, QVector<QColor> colors);
    void showDragonLike(QVector<QString> stones,
                        QString name,
                        QVector<QPen> pens,
                        bool do_lines=true,
                        MarkerMarks mark_type=MarkEllipse
                        );
    void drawBoard();
    void clearAllMarkers();
    QPointF alphaNumToPos(QString alphanum);
    QString posToAlphaNum(QPointF point);
    bool hasStone(QString location);
    void clearBoard();
    void removeMarkers(QString name);
    void readSettings();

    template <class T> void center(T* item){
        item->setPos( item->pos().rx()-(item->sceneBoundingRect().width()/2.0), item->pos().ry()-(item->sceneBoundingRect().height()/2.0));
    }
    template <class T> void center(T* item, QPointF p){
        item->setPos( p.rx()-(item->sceneBoundingRect().width()/2.0), p.ry()-(item->sceneBoundingRect().height()/2.0));
    }
    template <class T> void center(T* item, qreal x, qreal y){
        item->setPos( x-(item->sceneBoundingRect().width()/2.0), y-(item->sceneBoundingRect().height()/2.0));
    }

    QGraphicsItemGroup *gig;
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

    void finalStatusList(QVector<QString> verticies, QString type);
public slots:
    void placeStone(QString color, QString location);
    void removeStone(QString location);
    void checkStones(QString color, QStringList verticies);
    void showTopMoves(const QString color, QStringList verticies);

signals:
    //void boardLeftClicked(QString color, QString vertex);
    void boardLeftClicked(QString vertex);//color kept track separately now


protected:
    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

};

#endif // GOBOARD_H
