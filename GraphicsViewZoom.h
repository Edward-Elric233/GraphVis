// Copyright(C), Edward-Elric233
// Author: Edward-Elric233
// Version: 1.0
// Date: 2022/5/2
// Description: 
#ifndef GRAPHVIS_GRAPHICSVIEWZOOM_H
#define GRAPHVIS_GRAPHICSVIEWZOOM_H

#include <QObject>
#include <QGraphicsView>

class GraphicsViewZoom : public QObject {
    Q_OBJECT
public:
    explicit GraphicsViewZoom(QGraphicsView* view);
    void gentle_zoom(double factor);
    void set_modifiers(Qt::KeyboardModifiers modifiers);
    void set_zoom_factor_base(double value);

private:
    QGraphicsView* _view;
    Qt::KeyboardModifiers _modifiers;
    double _zoom_factor_base;
    QPointF target_scene_pos, target_viewport_pos;
    bool eventFilter(QObject* object, QEvent* event);

signals:
    void zoomed();
};


#endif //GRAPHVIS_GRAPHICSVIEWZOOM_H
