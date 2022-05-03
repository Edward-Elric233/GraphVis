// Copyright(C), Edward-Elric233
// Author: Edward-Elric233
// Version: 1.0
// Date: 2022/5/2
// Description: 
#ifndef GRAPHVIS_MAINWINDOW_H
#define GRAPHVIS_MAINWINDOW_H

class GraphLayout;
class KCoreLayout;

#include "EllipseItem.h"
#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <unordered_map>
#include <memory>
#include <QLabel>
#include <QSlider>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    void inputData();
    void initView();
    void initMenuBar();
    void initStatusBar();
    void initToolDockWidget();

    static constexpr int VIEW_SZ = 900;
    static constexpr int TOOL_WIDTH = 400;
    std::unique_ptr<ogdf::Graph> G_;
    std::unique_ptr<ogdf::GraphAttributes> GA_;
    QGraphicsView *view_;
    QGraphicsScene *scene_;
    std::unordered_map<ogdf::node, EllipseItem*> node2item_;
    std::unordered_map<EllipseItem*, ogdf::node> item2node_;
    std::vector<QGraphicsLineItem*> lineItems_;

    QLabel *statusLabel_file;
    QLabel *statusLabel_node;

    QSlider *sliderNodeSize_;
    QSlider *sliderEdgeWidth_;
    QSlider *sliderEpsilion_;
    QSlider *sliderDelta_;
    QSlider *sliderGamma_;

    QPoint center_;

    std::unique_ptr<GraphLayout> graphLayout_;
    std::shared_ptr<KCoreLayout> kCoreLayout_;
private slots:
    //开始读入文件并布局
    void on_actionNew_triggered();
    //相应点击事件
    void on_ellipseItem_clicked();
    //改变节点大小
    void on_sliderNodeSize_valueChanged(int);
    //改变边粗细
    void on_sliderEdgeWidth_valueChanged(int);
    //平移
    void on_pushButton_clicked();
};


#endif //GRAPHVIS_MAINWINDOW_H
