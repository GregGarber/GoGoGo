#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

#include <QtCore>
#include <QtGui>
#include <QGraphicsEllipseItem>
#include <QVector>

//class Highlight : public QGraphicsObject, public QGraphicsEllipseItem
class Highlight : public QGraphicsObject
{
    Q_OBJECT
public:
    //Highlight();
    Highlight(QGraphicsItem *parent = nullptr);
    QRectF boundingRect() const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) Q_DECL_OVERRIDE;
    void setDiameter(qreal dia);
    void setTotalTime(qreal t);

    /*
    struct Ring{
        qreal v0=0.0; //can't really think of anything else...
    };
    */
    QVector<qreal> rings; //to store ring's "time of birth" v0
    qreal frequency = 2.0; //hertz
    qreal velocity = 120.0; // m/s
    qreal total_time;
    qreal done_ratio;

    qreal time;
    void setTime(qreal time);
    qreal getTime();
    Q_PROPERTY(qreal time READ getTime WRITE setTime)

    //QGraphicsEllipseItem *ellipse;
    QPen pen;
    QBrush brush;
    qreal diameter=0;//initial innermost ring
    qreal outerDiameter=0; //including the outermost ring
    qreal numRings=5;
};

#endif // HIGHLIGHT_H
