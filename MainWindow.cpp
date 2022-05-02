// Copyright(C), Edward-Elric233
// Author: Edward-Elric233
// Version: 1.0
// Date: 2022/5/2
// Description:
#include "GraphLayout.h"
#include "KCoreLayout.h"
#include "MainWindow.h"
#include "GraphicsViewZoom.h"
#include <QGridLayout>
#include <QGraphicsEllipseItem>
#include <ogdf/fileformats/GraphIO.h>
#include <QMenuBar>
#include <QMenu>
#include <QShowEvent>

using namespace ogdf;


constexpr int MainWindow::VIEW_SZ;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , view_(new QGraphicsView)
    , G_()
    , GA_(G_, GraphAttributes::nodeGraphics |
              GraphAttributes::edgeGraphics |
              //GraphAttributes::nodeLabel |
              GraphAttributes::nodeStyle |
              GraphAttributes::edgeType |
              GraphAttributes::edgeArrow |
              GraphAttributes::edgeStyle)
    , graphLayout_(new GraphLayout(G_, GA_))
    , kCoreLayout_(std::make_shared<KCoreLayout>())
    {
    initToolBar();
    setCentralWidget(view_);

    GraphicsViewZoom *zoom = new GraphicsViewZoom(view_);
    zoom->set_modifiers(Qt::NoModifier);

    resize(VIEW_SZ, VIEW_SZ);

    inputData();
}

void MainWindow::initToolBar() {
    QAction *actionNew = new QAction(this);
    actionNew->setText("Open");

    QMenuBar *menubar = new QMenuBar(this);
    QMenu *menuFile = menubar->addMenu("File");

    menuFile->addAction(actionNew);
    setMenuBar(menubar);

    QObject::connect(actionNew, &QAction::triggered, this, &MainWindow::inputData);
}

void MainWindow::inputData() {
    std::string file_path = "./0.gml";
    if (!GraphIO::read(GA_, G_, file_path, GraphIO::readGML)) {
        std::cerr << "can't find file" << std::endl;
        exit(1);
    }

    /*
    std::cout << G_.nodes.size() << std::endl;
    for (auto node : G_.nodes) {
        std::cout << node->index() << std::endl;
    }
     */
    initView();
}

void MainWindow::initView() {
    //std::cout << view_->height() << " " << view_->width() << std::endl;

    QRect scene_rect(-VIEW_SZ / 2, -VIEW_SZ / 2, VIEW_SZ, VIEW_SZ);
    scene_ = new QGraphicsScene(scene_rect);

    QPen pen;
    pen.setWidth(2);
    QBrush brush(Qt::gray);

    double x, y, widht, height;
    double maxS = 0;
    for (auto node : G_.nodes) {
        x = GA_.x(node); y = GA_.y(node); widht = GA_.width(node); height = GA_.height(node);

        if (abs(x) > maxS) maxS = abs(x);
        if (abs(y) > maxS) maxS = abs(y);

        EllipseItem *item = new EllipseItem( - widht / 2, - height / 2, widht, height);
        item->setPos(x, y);
        item->setPen(pen);
        item->setBrush(brush);
        item->setFlags(QGraphicsItem::ItemIsMovable
                        | QGraphicsItem::ItemIsSelectable
                        | QGraphicsItem::ItemIsFocusable);

        mp_.emplace(node, item);

        scene_->addItem(item);
    }

    node u, v;
    for (auto edge : G_.edges) {
        u = edge->source(); v = edge->target();
        QGraphicsLineItem *item = new QGraphicsLineItem(GA_.x(u), GA_.y(u), GA_.x(v), GA_.y(v));
        item->setPen(pen);
        //item->setFlag(QGraphicsItem::ItemIsSelectable);
        mp_[u]->addSourceEdge(item);
        mp_[v]->addTargetEdge(item);

        scene_->addItem(item);
    }

    maxS += widht;

    scene_->setSceneRect(-maxS / 2, -maxS / 2, maxS, maxS);
    view_->setScene(scene_);

    view_->fitInView(scene_->sceneRect(), Qt::KeepAspectRatio);
}


MainWindow::~MainWindow() {

}

void MainWindow::on_actionNew_triggered() {

}
