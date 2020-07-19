#include "customline.h"
#include <QDebug>

const QColor skinLineColor = QColor(232, 117, 40);

CustomLine::CustomLine(CustomPoint *p1, CustomPoint *p2) :
    QGraphicsLineItem(QLineF(p1->center(), p2->center()))
{
    point1 = p1;
    point2 = p2;
    this->setPen(QPen(Qt::NoPen));

    point1->scene()->addItem(this);//pointと同じscene上に追加
    point1->installSceneEventFilter(this);
    point2->installSceneEventFilter(this);

    int lineWidth = 1;
    QColor lineColor = QColor("black");

    QColor blockColor;
    if (point1->order() > point2->order())
        blockColor = point1->info.color;
    else
        blockColor = point2->info.color;

    blockColor.setAlpha(80);

    polygon.setPen(QPen(QBrush(lineColor), lineWidth));
    polygon.setBrush(QBrush(blockColor));

    if (p1->info.isExport) {
        //腕、脚だけlineで縁取りしてみる
        if (p1->tag().contains("elbow") ||
            p2->tag().contains("elbow") ||
            p1->tag().contains("knee")  ||
            p2->tag().contains("knee")) {

            QSize imSize = p1->info.imgSize;
            int longer = imSize.height() > imSize.width() ? imSize.height() : imSize.width();
            lineWidth = longer / 200;
            if (lineWidth < 1)
                lineWidth = 1;

            lineColor = skinLineColor;
            polygon.setPen(QPen(QBrush(lineColor), lineWidth));
        } else {
            polygon.setPen(QPen(Qt::NoPen));
        }

        blockColor.setAlpha(255);//透過なし
        polygon.setBrush(QBrush(blockColor));
    }

    drawPolygon();
    point1->scene()->addItem(&polygon);

    //z値は接続する2点よりは低くするが、元からあった他の点よりは高くする
    float z = (float)point1->zValue() - 0.1;//point1(新たな追加点)が基準
    polygon.setZValue(z);
    if (point2->zValue() < z)
        point2->setZValue(z + 0.05);//point2は、このlineよりは上にする

}

CustomLine::~CustomLine()
{
}

bool CustomLine::isConnectedTo(CustomPoint *point)
{
    return (point == point1 || point == point2);
}

void CustomLine::clearPolygon()
{
    point1->scene()->removeItem(&polygon);
}

bool CustomLine::sceneEventFilter(QGraphicsItem *watched, QEvent *event)
{
    if (event->type() == QEvent::GraphicsSceneMouseMove) {
        this->setLine(QLineF(point1->center(), point2->center()));
        drawPolygon();
    }

    return false;
}

CustomLine::drawPolygon()
{
    //接続してる2点の中心と半径
    QPointF c1 = point1->center();
    QPointF c2 = point2->center();
    int r1 = point1->info.r;
    int r2 = point2->info.r;

    QPointF a, b, c, d;//polygonの頂点

    //c1,c2を結ぶ線と垂直で, c1, c2からそれぞれa, bとc, dにつながる線分を求める
    QLineF lineA = QLineF(c1, c2).normalVector();
    QLineF lineB = QLineF(c1, c2).normalVector();
    QLineF lineC = QLineF(c2, c1).normalVector();
    QLineF lineD = QLineF(c2, c1).normalVector();
    lineA.setLength(r1);
    lineB.setLength(-r1);
    lineC.setLength(r2);
    lineD.setLength(-r2);

    a = lineA.p2();
    b = lineB.p2();
    c = lineC.p2();
    d = lineD.p2();

    QPolygonF _poly;
    if (QLineF(b, c).length() < QLineF(b, d).length())
        _poly << a << b << c << d;//cの方がbに近い
    else
        _poly << a << b << d << c;//dの方がbに近い

    polygon.setPolygon(_poly);
}
