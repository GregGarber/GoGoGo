#include "highlight.h"

Highlight::Highlight(QGraphicsItem *parent) :  QGraphicsObject(parent)
{
    brush = QBrush(Qt::green);
    pen = QPen(brush, 100.0);

}

QRectF Highlight::boundingRect() const
{
    return mapRectFromScene(x()-((outerDiameter)/2.0),y()-((outerDiameter)/2.0) , outerDiameter, outerDiameter);
    //this->advance();
}

void Highlight::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QColor pen_color = pen.color();
    painter->setPen(pen);
    if(time < total_time ){ //don't want to keep adding them past animation length
        int expected_num_rings = (int)ceil(frequency * time);
        if( expected_num_rings > rings.length()){
            rings.append(time);
        }
        //velocity= 200.0 - (time*total_time*2.0);//pretty cool
        //velocity= 200.0*(1.0-done_ratio);//pretty cool, about the same
        //frequency = 3.0*(1.0-done_ratio);//pretty cool
        for(int i=0; i<rings.length(); i++){
            //qreal dist = diameter + ( velocity * (time - rings.at(i)));
            qreal t = (time - rings.at(i));
            qreal dist = diameter + ( velocity * t ) + (0.5 * -5.0 * t*t);
            //qDebug() << "dist:" << dist << " outerDiameter:" << outerDiameter;

            QRectF s(x()-dist/2.0, y()-dist/2.0,dist,dist);
            QRectF r = mapRectFromScene(x()-dist/2.0, y()-dist/2.0,dist,dist);
            pen.setWidth(20.0+50.0* dist/outerDiameter);
            QRadialGradient radialGrad;
            radialGrad.setCenter(r.center());
            radialGrad.setFocalPoint(r.center());
            radialGrad.setCenterRadius(r.width()/2.0+pen.widthF()/2.0);
            radialGrad.setFocalRadius(r.width()/2.0-pen.widthF()/2.0);
            /* not entirely sure I get it, but I think focal radius
     * needs to be either the center of the pen or its inner edge
     * while center radius is the outer edge.
    */

            QColor green(0,255,0,255);
            QColor yellow(255,255,0,255);
            /*
            pen_color.setAlphaF(1.0-(dist/outerDiameter)); //surface waves don't inverse square
            */
            green.setAlphaF(1.0-(dist/outerDiameter));
            yellow.setAlphaF((1.0-(dist/outerDiameter)));

            radialGrad.setColorAt(.0, yellow );
            radialGrad.setColorAt( .5, green );
            radialGrad.setColorAt(1, yellow );

            brush = QBrush(radialGrad);
            pen.setBrush(brush);
            painter->setPen(pen);
            painter->drawEllipse(r);
        }

    }

}

//initial diameter--gridSizePixels
void Highlight::setDiameter(qreal dia)
{
    this->diameter = dia;
}

void Highlight::setTotalTime(qreal t)
{
    total_time = t;
    this->outerDiameter = diameter + ( velocity * total_time);
}

void Highlight::setTime(qreal time)
{
    //for some reason, time can be > total_time
    if(time <= total_time){
        this->time = time;
        done_ratio = time/total_time;
        update();
    }
}

qreal Highlight::getTime()
{
    return time;
}

