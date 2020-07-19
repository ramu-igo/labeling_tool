#ifndef CUSTOMPOINT_H
#define CUSTOMPOINT_H

#include <QObject>
#include <QRect>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QWheelEvent>

const QColor skinColor = QColor(240,221,195);

enum Z_ORDER
{
    Z_ORDER_LINE,
    Z_ORDER_POINT,
};

struct PointInfo
{
    PointInfo()
    {
        r = 5;
        color = QColor("black");
        order = 0;
        sizeRatio = 0.014;
        imgSize = QSize(100, 100);
        z = 0;
        isExport = false;
    }

    QPoint pos;
    int r;
    QColor color;
    int z;//scene上でのz-value
    int order;
    bool isExport;

    QString tag;
    int personId;

    QSize imgSize;//この点が設置される画像のサイズ
    float sizeRatio;//画像サイズの短辺に対する点半径の比
};

class CustomPoint : public QGraphicsEllipseItem
{

public:
    explicit CustomPoint(QGraphicsScene *scene, PointInfo pi);

    QPoint center();
    QString tag() { return info.tag; }
    int personId(){ return info.personId; }
    int order()   { return info.order; }

    PointInfo info;

    void recvWheelEvent(QWheelEvent* wheel_event);

public slots:

private:
};

#endif // CUSTOMPOINT_H
