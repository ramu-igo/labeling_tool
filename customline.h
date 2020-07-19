#ifndef CUSTOMLINE_H
#define CUSTOMLINE_H

#include <QObject>
#include <QPen>
#include <QLineF>
#include <QGraphicsLineItem>
#include <QEvent>
#include "custompoint.h"
#include <QGraphicsScene>
#include <QGraphicsPolygonItem>

class CustomLine : public QGraphicsLineItem
{

public:
    explicit CustomLine(CustomPoint *p1, CustomPoint *p2);
    ~CustomLine();

    bool isConnectedTo(CustomPoint *point);

    void clearPolygon();

signals:

protected:
    bool sceneEventFilter(QGraphicsItem *watched, QEvent *event);

public slots:

    CustomPoint *point1, *point2;

    QGraphicsPolygonItem polygon;

    drawPolygon();

};

#endif // CUSTOMLINE_H
