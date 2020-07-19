#include "custompoint.h"
#include <QBrush>
#include <QDebug>

CustomPoint::CustomPoint(QGraphicsScene *scene, PointInfo pi)
    : QGraphicsEllipseItem(QRectF(0, 0, 1, 1))
{
    scene->addItem(this);

    if (pi.r < 1)
        pi.r = 1;

    setRect(0, 0, pi.r*2 , pi.r*2);
    setPos(pi.pos.x() - pi.r, pi.pos.y() - pi.r);

    setPen(QPen(Qt::NoPen));
    pi.color.setAlpha(150);

    if (pi.isExport) {
        /*
         * エクスポート時の色指定
         * TODO: ここも設定ファイルで指定可能にする
         */

        if (pi.tag.contains("eye")) {
            pi.color = QColor(0, 0, 0);
        } else if (pi.tag == "mouth") {
            pi.color = QColor(255, 110, 0); //オレンジ
        } else if (pi.tag == "nose" || pi.tag.contains("ear")) {
            pi.color = skinColor;
        } else { //顔以外の点
            pi.color = skinColor;
        }

        pi.color.setAlpha(255);//透過なし
    }

    setBrush(QBrush(pi.color));

    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setZValue(pi.z);

    QString msg = QString("Person %1 : %2").arg(pi.personId).arg(pi.tag);
    this->setToolTip(msg);

    this->info = pi;
}

QPoint CustomPoint::center()
{
    return QPoint(this->x() + info.r, this->y() + info.r);
}

void CustomPoint::recvWheelEvent(QWheelEvent *wheel_event)
{
    QPointF orgCenter = this->center();
    double angle = wheel_event->delta();
    int step = 1; //半径を大きくするstep
    if (angle < 0)
        step = -step;

    info.r += step;
    if (info.r <= 0)
        info.r = 1;

    QRectF rect = this->rect();
    rect.setWidth(info.r * 2);
    rect.setHeight(info.r * 2);
    this->setRect(rect);
    this->setPos(orgCenter.x() - info.r, orgCenter.y() - info.r);//中心を維持
}

