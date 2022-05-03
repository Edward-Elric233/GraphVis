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

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    void inputData();
    void initView();
    void initToolBar();
    void initStatusBar();

    static constexpr int VIEW_SZ = 900;
    std::unique_ptr<ogdf::Graph> G_;
    std::unique_ptr<ogdf::GraphAttributes> GA_;
    QGraphicsView *view_;
    QGraphicsScene *scene_;
    std::unordered_map<ogdf::node, EllipseItem*> node2item_;
    std::unordered_map<EllipseItem*, ogdf::node> item2node_;

    QLabel *statusLabel_file;
    QLabel *statusLabel_node;

    std::unique_ptr<GraphLayout> graphLayout_;
    std::shared_ptr<KCoreLayout> kCoreLayout_;
private slots:
    //开始读入文件并布局
    void on_actionNew_triggered();
    //相应点击事件
    void on_ellipseItem_clicked();
};


#endif //GRAPHVIS_MAINWINDOW_H
