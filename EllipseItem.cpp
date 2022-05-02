// Copyright(C), Edward-Elric233
// Author: Edward-Elric233
// Version: 1.0
// Date: 2022/5/2
// Description: 
#include "EllipseItem.h"
#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <iostream>

EllipseItem::EllipseItem(double x, double y, double w, double h)
    : QGraphicsEllipseItem(x, y, w, h) {
    setCursor(Qt::OpenHandCursor);  //设置鼠标放上去的光标样式
    setAcceptedMouseButtons(Qt::LeftButton);    //设置只相应鼠标左键
}

EllipseItem::~EllipseItem() {

}

void print(const QPointF &pos) {
    std::cout << pos.x() << " " << pos.y() << std::endl;
}
void print(double x, double  y) {
    std::cout << x << " " << y << std::endl;
}

void EllipseItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseMoveEvent(event);
    QPointF pos = scenePos();
    for (auto targetEdge : targetEdges_) {
        auto&& line = targetEdge->line();
        targetEdge->setLine(line.x1(), line.y1(), pos.x(), pos.y());
    }
    for (auto sourceEdge : sourceEdges_) {
        auto &&line = sourceEdge->line();
        sourceEdge->setLine(pos.x(), pos.y(), line.x2(), line.y2());
    }
}