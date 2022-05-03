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
#include <QDockWidget>
#include <QGroupBox>
#include <QPushButton>

using namespace ogdf;


constexpr int MainWindow::VIEW_SZ;
constexpr int MainWindow::TOOL_WIDTH;

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
    , center_(0, 0)
    {
    initMenuBar();
    initStatusBar();
    initToolDockWidget();
    setCentralWidget(view_);


    /*
     * 设置缩放
     */
    GraphicsViewZoom *zoom = new GraphicsViewZoom(view_);
    zoom->set_modifiers(Qt::NoModifier);

    /*
     * 设定平移模式
     */
    //view_->setTransformationAnchor(QGraphicsView::NoAnchor);

    resize(VIEW_SZ + TOOL_WIDTH, VIEW_SZ);


    //inputData();
}

void MainWindow::initMenuBar() {
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

void MainWindow::initToolDockWidget() {

    QDockWidget *tool = new QDockWidget(this);
    tool->setFixedWidth(TOOL_WIDTH);

    QWidget *dock = new QWidget(tool);

    QVBoxLayout *vLayout = new QVBoxLayout(dock);

    QGroupBox *groupBox = new QGroupBox(dock);
    int sz = TOOL_WIDTH / 2;
    groupBox->setFixedSize(sz, sz + 40);    //不要问我为什么要+40，问就是玄学
    QGridLayout *gridLayout = new QGridLayout(groupBox);
    gridLayout->setSpacing(10);
    gridLayout->setContentsMargins(10, 10, 10, 10);
    sz -= 40;
    sz /= 3;

    QFont font;
    font.setPointSize(10);

    QPushButton *ptnUp = new QPushButton(groupBox);
    ptnUp->setFixedSize(sz, sz);
    ptnUp->setText("Up");
    ptnUp->setFont(font);
    QPushButton *ptnDown = new QPushButton(groupBox);
    ptnDown->setFixedSize(sz, sz);
    ptnDown->setText("Down");
    ptnDown->setFont(font);
    QPushButton *ptnRight = new QPushButton(groupBox);
    ptnRight->setFixedSize(sz, sz);
    ptnRight->setText("Right");
    ptnRight->setFont(font);
    QPushButton *ptnLeft = new QPushButton(groupBox);
    ptnLeft->setFixedSize(sz, sz);
    ptnLeft->setText("Left");
    ptnLeft->setFont(font);
    QPushButton *ptnUpLeft = new QPushButton(groupBox);
    ptnUpLeft->setFixedSize(sz, sz);
    ptnUpLeft->setFont(font);
    QPushButton *ptnDownLeft = new QPushButton(groupBox);
    ptnDownLeft->setFixedSize(sz, sz);
    ptnDownLeft->setFont(font);
    QPushButton *ptnUpRight = new QPushButton(groupBox);
    ptnUpRight->setFixedSize(sz, sz);
    ptnUpRight->setFont(font);
    QPushButton *ptnDownRight = new QPushButton(groupBox);
    ptnDownRight->setFixedSize(sz, sz);
    ptnDownRight->setFont(font);
    QPushButton *ptnReset = new QPushButton(groupBox);
    ptnReset->setFixedSize(sz, sz);
    ptnReset->setText("Reset");
    ptnReset->setFont(font);

    ptnUpLeft->setProperty("id", 0);
    ptnUp->setProperty("id", 1);
    ptnUpRight->setProperty("id", 2);
    ptnLeft->setProperty("id", 3);
    ptnReset->setProperty("id", 4);
    ptnRight->setProperty("id", 5);
    ptnDownLeft->setProperty("id", 6);
    ptnDown->setProperty("id", 7);
    ptnDownRight->setProperty("id", 8);

    gridLayout->addWidget(ptnUpLeft, 0, 0, 1, 1);
    gridLayout->addWidget(ptnUp, 0, 1, 1, 1);
    gridLayout->addWidget(ptnUpRight, 0, 2, 1, 1);
    gridLayout->addWidget(ptnLeft, 1, 0, 1, 1);
    gridLayout->addWidget(ptnReset, 1, 1, 1, 1);
    gridLayout->addWidget(ptnRight, 1, 2, 1, 1);
    gridLayout->addWidget(ptnDownLeft, 2, 0, 1, 1);
    gridLayout->addWidget(ptnDown, 2, 1, 1, 1);
    gridLayout->addWidget(ptnDownRight, 2, 2, 1, 1);

    QHBoxLayout *hBoxLayout = new QHBoxLayout();
    hBoxLayout->addStretch();
    hBoxLayout->addWidget(groupBox);
    hBoxLayout->addStretch();

    vLayout->addStretch();
    vLayout->addLayout(hBoxLayout);

    QGroupBox *groupBox1 = new QGroupBox(dock);
    QGridLayout *gridLayout1 = new QGridLayout(groupBox1);
    gridLayout1->setHorizontalSpacing(5);
    gridLayout1->setVerticalSpacing(30);
    gridLayout1->setContentsMargins(11, 30, 11, 30);

    //Node Size
    QLabel *label1 = new QLabel("Node Size:", groupBox1);
    sliderNodeSize_ = new QSlider(groupBox1);
    sliderNodeSize_->setOrientation(Qt::Horizontal);
    sliderNodeSize_->setMaximum(500);
    sliderNodeSize_->setMinimum(50);
    sliderNodeSize_->setValue(100);
    sliderNodeSize_->setFixedWidth(TOOL_WIDTH / 2);
    sliderNodeSize_->setTickPosition(QSlider::TicksBelow);
    sliderNodeSize_->setTickInterval(10);
    gridLayout1->addWidget(label1, 0, 0, 1, 1);
    gridLayout1->addWidget(sliderNodeSize_, 0, 1, 1, 1);


    //Edge width
    QLabel *label2 = new QLabel("Edge Width:", groupBox1);
    sliderEdgeWidth_ = new QSlider(groupBox1);
    sliderEdgeWidth_->setOrientation(Qt::Horizontal);
    sliderEdgeWidth_->setMaximum(1000);
    sliderEdgeWidth_->setMinimum(1);
    sliderEdgeWidth_->setValue(100);
    sliderEdgeWidth_->setFixedWidth(TOOL_WIDTH / 2);
    sliderEdgeWidth_->setTickPosition(QSlider::TicksBelow);
    sliderEdgeWidth_->setTickInterval(10);
    gridLayout1->addWidget(label2, 1, 0, 1, 1);
    gridLayout1->addWidget(sliderEdgeWidth_, 1, 1, 1, 1);

    //Epsilon
    QLabel *label3 = new QLabel("Epsilon:", groupBox1);
    sliderEpsilion_ = new QSlider(groupBox1);
    sliderEpsilion_->setOrientation(Qt::Horizontal);
    sliderEpsilion_->setMaximum(10);
    sliderEpsilion_->setMinimum(-10);
    sliderEpsilion_->setValue(3);
    sliderEpsilion_->setFixedWidth(TOOL_WIDTH / 2);
    sliderEpsilion_->setTickPosition(QSlider::TicksBelow);
    sliderEpsilion_->setTickInterval(1);
    gridLayout1->addWidget(label3, 2, 0, 1, 1);
    gridLayout1->addWidget(sliderEpsilion_, 2, 1, 1, 1);

    //Delta
    QLabel *label4 = new QLabel("Delta:", groupBox1);
    sliderDelta_ = new QSlider(groupBox1);
    sliderDelta_->setOrientation(Qt::Horizontal);
    sliderDelta_->setMaximum(1000);
    sliderDelta_->setMinimum(100);
    sliderDelta_->setValue(500);
    sliderDelta_->setFixedWidth(TOOL_WIDTH / 2);
    sliderDelta_->setTickPosition(QSlider::TicksBelow);
    sliderDelta_->setTickInterval(10);
    gridLayout1->addWidget(label4, 3, 0, 1, 1);
    gridLayout1->addWidget(sliderDelta_, 3, 1, 1, 1);

    //Gamma
    QLabel *label5 = new QLabel("Gamma:", groupBox1);
    sliderGamma_ = new QSlider(groupBox1);
    sliderGamma_->setOrientation(Qt::Horizontal);
    sliderGamma_->setMaximum(1000);
    sliderGamma_->setMinimum(100);
    sliderGamma_->setValue(500);
    sliderGamma_->setFixedWidth(TOOL_WIDTH / 2);
    sliderGamma_->setTickPosition(QSlider::TicksBelow);
    sliderGamma_->setTickInterval(10);
    gridLayout1->addWidget(label5, 4, 0, 1, 1);
    gridLayout1->addWidget(sliderGamma_, 4, 1, 1, 1);

    vLayout->addStretch();
    vLayout->addWidget(groupBox1);
    vLayout->addStretch();

    tool->setWidget(dock);

    addDockWidget(Qt::RightDockWidgetArea, tool);

    QObject::connect(sliderNodeSize_, &QSlider::valueChanged, this, &MainWindow::on_sliderNodeSize_valueChanged);
    QObject::connect(sliderEdgeWidth_, &QSlider::valueChanged, this, &MainWindow::on_sliderEdgeWidth_valueChanged);

    QObject::connect(ptnUpLeft, &QPushButton::clicked, this, &MainWindow::on_pushButton_clicked);
    QObject::connect(ptnUp, &QPushButton::clicked, this, &MainWindow::on_pushButton_clicked);
    QObject::connect(ptnUpRight, &QPushButton::clicked, this, &MainWindow::on_pushButton_clicked);
    QObject::connect(ptnLeft, &QPushButton::clicked, this, &MainWindow::on_pushButton_clicked);
    QObject::connect(ptnReset, &QPushButton::clicked, this, &MainWindow::on_pushButton_clicked);
    QObject::connect(ptnRight, &QPushButton::clicked, this, &MainWindow::on_pushButton_clicked);
    QObject::connect(ptnDownLeft, &QPushButton::clicked, this, &MainWindow::on_pushButton_clicked);
    QObject::connect(ptnDown, &QPushButton::clicked, this, &MainWindow::on_pushButton_clicked);
    QObject::connect(ptnDownRight, &QPushButton::clicked, this, &MainWindow::on_pushButton_clicked);
}

/*
 * abandoned
 */
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

        lineItems_.push_back(item);
    }
    for (auto &pr : node2item_) {
        scene_->addItem(pr.second);
    }

    maxS += widht;

    scene_->setSceneRect(-maxS / 2, -maxS / 2, maxS, maxS);
    view_->setScene(scene_);

    //view_->fitInView(scene_->sceneRect(), Qt::KeepAspectRatio);
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

void MainWindow::on_sliderNodeSize_valueChanged(int x) {
    for (auto &pr : node2item_) {
        pr.second->setRect(-x / 2, -x / 2, x, x);
    }
}

void MainWindow::on_sliderEdgeWidth_valueChanged(int x) {
    double width = x / 100.0;
    QPen pen;
    pen.setWidthF(width);
    for (auto item : lineItems_) {
        item->setPen(pen);
    }
}

void MainWindow::on_pushButton_clicked() {
    static constexpr int interval = 1000;
    static int dir[9][2] = {
            {-interval, -interval},
            {0, -interval},
            {-interval, -interval},
            {-interval, 0},
            {0, 0},
            {interval, 0},
            {-interval, interval},
            {0, interval},
            {interval, interval},
    };
    QPushButton *ptn = qobject_cast<QPushButton *>(sender());
    int id = ptn->property("id").toInt();
    //utils::print("id =", id);
    if (id == 4) {
        center_.setX(0);
        center_.setY(0);
    } else {
        center_.setX(center_.x() + dir[id][0]);
        center_.setY(center_.y() + dir[id][1]);
    }
    view_->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    view_->centerOn(center_.x(), center_.y());
    view_->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
}
