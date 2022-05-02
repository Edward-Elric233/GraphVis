// Copyright(C), Edward-Elric233
// Author: Edward-Elric233
// Version: 1.0
// Date: 2022/5/2
// Description: 
#ifndef GRAPHVIS_ELLIPSEITEM_H
#define GRAPHVIS_ELLIPSEITEM_H

#include <QGraphicsEllipseItem>
#include <vector>

class EllipseItem : public QGraphicsEllipseItem {
public:
    EllipseItem(double x, double y, double w, double h);
    ~EllipseItem();
    void addTargetEdge(QGraphicsLineItem *edge) {
        targetEdges_.push_back(edge);
    }
    void addSourceEdge(QGraphicsLineItem *edge) {
        sourceEdges_.push_back(edge);
    }
protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
private:
    std::vector<QGraphicsLineItem *> targetEdges_;  //保存作为target的邻接边
    std::vector<QGraphicsLineItem *> sourceEdges_;  //保存作为source的邻接边
};


#endif //GRAPHVIS_ELLIPSEITEM_H
