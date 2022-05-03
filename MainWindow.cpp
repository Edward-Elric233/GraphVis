// Copyright(C), Edward-Elric233
// Author: Edward-Elric233
// Version: 1.0
// Date: 2022/5/2
// Description:
#include "GraphLayout.h"
#include "KCoreLayout.h"
#include "MainWindow.h"
#include "utils.h"
#include "GraphicsViewZoom.h"
#include <QGridLayout>
#include <QGraphicsEllipseItem>
#include <ogdf/fileformats/GraphIO.h>
#include <QMenuBar>
#include <QMenu>
#include <QShowEvent>
#include <QDir>
#include <QFileDialog>
#include <QCoreApplication>
#include <QStatusBar>

using namespace ogdf;


constexpr int MainWindow::VIEW_SZ;

static const long Attr = GraphAttributes::nodeGraphics |
                   GraphAttributes::edgeGraphics |
                   GraphAttributes::nodeLabel |
                   GraphAttributes::nodeStyle |
                   GraphAttributes::edgeType |
                   GraphAttributes::edgeArrow |
                   GraphAttributes::edgeStyle;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , view_(new QGraphicsView)
    , kCoreLayout_(std::make_shared<KCoreLayout>())
    {
    initToolBar();
    initStatusBar();
    setCentralWidget(view_);

    /*
     * 设置缩放
     */
    GraphicsViewZoom *zoom = new GraphicsViewZoom(view_);
    zoom->set_modifiers(Qt::NoModifier);

    resize(VIEW_SZ, VIEW_SZ);


    //inputData();
}

void MainWindow::initToolBar() {
    QAction *actionNew = new QAction(this);
    actionNew->setText("Open");

    QMenuBar *menubar = new QMenuBar(this);
    QMenu *menuFile = menubar->addMenu("File");

    menuFile->addAction(actionNew);
    setMenuBar(menubar);

    QObject::connect(actionNew, &QAction::triggered, this, &MainWindow::on_actionNew_triggered);
}

void MainWindow::initStatusBar() {
    QStatusBar *statusbar = new QStatusBar(this);
    statusLabel_file = new QLabel();
    //statusLabel_file->setMinimumWidth(100);
    statusLabel_node = new QLabel();
    statusbar->addWidget(statusLabel_file);
    statusbar->addWidget(statusLabel_node);
    setStatusBar(statusbar);
}

void MainWindow::inputData() {
    std::string file_path = "./0.gml";
    if (!GraphIO::read(*GA_, *G_, file_path, GraphIO::readGML)) {
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
    pen.setColor(Qt::gray);
    QBrush brush(Qt::gray);

    double x, y, widht, height;
    double maxS = 0;
    for (auto node : G_->nodes) {
        x = GA_->x(node); y = GA_->y(node); widht = GA_->width(node); height = GA_->height(node);

        if (abs(x) > maxS) maxS = abs(x);
        if (abs(y) > maxS) maxS = abs(y);

        EllipseItem *item = new EllipseItem( - widht / 2, - height / 2, widht, height);
        item->setPos(x, y);
        item->setPen(pen);
        auto &&color = GA_->fillColor(node);
        item->setBrush(QBrush(QColor(color.red(), color.green(), color.blue())));
        item->setFlags(QGraphicsItem::ItemIsMovable
                        | QGraphicsItem::ItemIsSelectable
                        | QGraphicsItem::ItemIsFocusable);

        node2item_.emplace(node, item);
        item2node_.emplace(item, node);

        QObject::connect(item, &EllipseItem::clicked, this, &MainWindow::on_ellipseItem_clicked);

        //scene_->addItem(item);
    }

    node u, v;
    for (auto edge : G_->edges) {
        u = edge->source(); v = edge->target();
        QGraphicsLineItem *item = new QGraphicsLineItem(GA_->x(u), GA_->y(u), GA_->x(v), GA_->y(v));
        item->setPen(pen);
        //item->setFlag(QGraphicsItem::ItemIsSelectable);
        node2item_[u]->addSourceEdge(item); node2item_[v]->addTargetEdge(item);
        scene_->addItem(item);
    }
    for (auto &pr : node2item_) {
        scene_->addItem(pr.second);
    }

    maxS += widht;

    scene_->setSceneRect(-maxS / 2, -maxS / 2, maxS, maxS);
    view_->setScene(scene_);

    view_->fitInView(scene_->sceneRect(), Qt::KeepAspectRatio);
}


MainWindow::~MainWindow() {

}

void MainWindow::on_actionNew_triggered() {

    G_.reset(new Graph);
    GA_.reset(new GraphAttributes(*G_, Attr));
    graphLayout_.reset(new GraphLayout(*G_, *GA_));
    kCoreLayout_ = std::make_shared<KCoreLayout>();

    graphLayout_->setInput([&](Graph &G, GraphAttributes &GA) {
        QString curPath = QDir::currentPath();
        QString dlgTitle = "Choose input graph file";
        QString filter = "文本文件(*.txt *.edges *.dot);;所有文件(*.*)";
        QString fileName = QFileDialog::getOpenFileName(this, dlgTitle, curPath, filter);

        if (fileName.isEmpty()) {
            //TODO: cancle process
            return;
        }
        statusLabel_file->setText(QString::asprintf("Open file: %s", fileName.toStdString().c_str()));
        /*
        ogdf::randomGraph(G, 100, 500);
        return;
         */
        std::ifstream is(fileName.toStdString());
        std::unordered_map<int, node> mp;
        auto getNode = [&](int u) -> node {
            auto iter = mp.find(u);
            if (iter != mp.end()) {
            } else {
                iter = mp.emplace(u, G.newNode()).first;
            }
            return iter->second;
        };
        int x, y;
        while (is >> x >> y) {
            auto u = getNode(x);
            auto v = getNode(y);

            G.newEdge(u, v);
        }
    });
    graphLayout_->setLayout([&](Graph &G, GraphAttributes &GA) {
        //initLayout(G, GA);
        kCoreLayout_->call(GA);
    });

    graphLayout_->setOutput([&](const Graph& G, const GraphAttributes& GA) {
        QString curPath = QCoreApplication::applicationDirPath();
        QString dlgTitle = "Save file";
        QString filter = "";
        QString fileName = QFileDialog::getSaveFileName(this, dlgTitle, curPath, filter);
        if (fileName.isEmpty()) {
            //TODO: cancle process
        }
        //utils::print(fileName.toStdString());
        GraphLayout::outputWithName(G, GA, fileName.toStdString());
    });

    //utils::print("init finished");
    graphLayout_->operator()();
    //utils::print("layout finished");
    initView();
    //utils::print("initView finished");
}

void MainWindow::on_ellipseItem_clicked() {
    auto item = qobject_cast<EllipseItem*>(sender());
    statusLabel_node->setText(QString::asprintf("\tnode info: %s", GA_->label(item2node_[item]).c_str()));
}
