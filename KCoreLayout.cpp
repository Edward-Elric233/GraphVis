// Copyright(C), Edward-Elric233
// Author: Edward-Elric233
// Version: 1.0
// Date: 2022/4/25
// Description: 
#include "KCoreLayout.h"
#include "utils.h"
#include "Param.h"
#include "Singleton.h"
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <list>
#include <functional>

using namespace ogdf;

std::vector<std::unordered_set<int>> KCoreLayout::calForConnected(const std::unordered_set<int> &nodes) const {
    std::unordered_set<int> vis;
    std::vector<std::unordered_set<int>> components;    //连通分量

    std::function<void(int, std::unordered_set<int> &)> dfs; //递归访问连通分量
    dfs = [&](int u, std::unordered_set<int> &component) {
        component.insert(u);
        vis.insert(u);

        for (auto v : adjList_[u]) {
            if (nodes.count(v) && !vis.count(v)) {
                dfs(v , component);
            }
        }
    };

    for (auto u : nodes) {
        if (vis.count(u)) continue; //已经访问过u了

        components.emplace_back();
        dfs(u, components.back());
    }

    return components;
}

void KCoreLayout::initGraph(GraphAttributes &GA) {
    auto &&G = GA.constGraph();
    V_ = G.nodes.size();
    E_ = G.edges.size();

    std::unordered_map<ogdf::node, int> mp;
    int t = 0;
    for (auto node : G.nodes) {
        vertexs_.emplace_back(node);
        GA.label(node) = std::to_string(t);
        mp[node] = t++;
    }

    adjList_.resize(V_);

    int u, v;
    std::unordered_set<std::pair<int, int>> edge_set;   //手动去重
    for (auto edge : G.edges) {
        u = mp[edge->source()];
        v = mp[edge->target()];
        if (u > v) std::swap(u, v);
        if (u == v || edge_set.count(std::make_pair(u, v))) continue;   //如果出现自环或者重边则跳过
        edge_set.emplace(u, v);
        //无向图
        adjList_[u].push_back(v); adjList_[v].push_back(u);
    }

    /*
    //debug
    utils::print("--------------init Graph debug--------------");
    utils::print("node number:", V_, "edge number", E_);
    for (int i = 0; i < V_; ++i) {
        auto &adjList = adjList_[i];
        std::cout << i << " : ";
        utils::printArr(adjList);
    }
     */
}

void KCoreLayout::kCoreDecomposition() {
    std::vector<int> degree(V_);
    int maxDegree = 0;

    for (int i = 0; i < V_; ++i) {
        degree[i] = adjList_[i].size();
        maxDegree = std::max(maxDegree, degree[i]);
    }

    std::vector<std::vector<int>> buckets(maxDegree + 1);
    for (int i = 0; i < V_; ++i) buckets[degree[i]].push_back(i);

    std::vector<bool> vis(V_, false);   //记录节点是否处理过
    int cnt = 0;        //记录已经处理过的节点的个数

    //处理度为0的节点
    shells_.emplace_back();
    for (auto x : buckets[0]) {
        shells_[0].push_back(x);
        vertexs_[x].setC(0);
        vis[x] = true;
        ++cnt;
    }

    int k, u;
    for (k = 1; k <= maxDegree; ++k) {
        if (cnt == V_) break;   //已经没有节点可处理
        shells_.emplace_back();
        auto &shell = shells_.back();
        auto &bucket = buckets[k];

        for (int i = 0; i < bucket.size(); ++i) {   //只能用下标索引，因为还会在后面添加节点
            u = bucket[i];
            if (vis[u]) continue;   //已经处理过这个节点了

            shell.push_back(u);
            vertexs_[u].setC(k);
            vis[u] = true;
            ++cnt;

            for (auto v : adjList_[u]) {
                if (degree[v] > k) {
                    buckets[--degree[v]].push_back(v);
                }
            }
        }
    }

    c_max_ = shells_.size() - 1;

    /*
    utils::print("--------------kCoreDecomposition debug--------------");
    for (int i = 0; i < k; ++i) {
        auto &shell = shells_[i];
        std::cout << i << " : ";
        utils::printArr(shell);
    }
    for (int i = 0; i < V_; ++i) {
        utils::print("node", i, ":", vertexs_[i].getC());
    }
     */
}

void KCoreLayout::colour(GraphAttributes &GA) {
    static unsigned char turbo_srgb_bytes[256][3] = {{48,18,59},{50,21,67},{51,24,74},{52,27,81},{53,30,88},{54,33,95},{55,36,102},{56,39,109},{57,42,115},{58,45,121},{59,47,128},{60,50,134},{61,53,139},{62,56,145},{63,59,151},{63,62,156},{64,64,162},{65,67,167},{65,70,172},{66,73,177},{66,75,181},{67,78,186},{68,81,191},{68,84,195},{68,86,199},{69,89,203},{69,92,207},{69,94,211},{70,97,214},{70,100,218},{70,102,221},{70,105,224},{70,107,227},{71,110,230},{71,113,233},{71,115,235},{71,118,238},{71,120,240},{71,123,242},{70,125,244},{70,128,246},{70,130,248},{70,133,250},{70,135,251},{69,138,252},{69,140,253},{68,143,254},{67,145,254},{66,148,255},{65,150,255},{64,153,255},{62,155,254},{61,158,254},{59,160,253},{58,163,252},{56,165,251},{55,168,250},{53,171,248},{51,173,247},{49,175,245},{47,178,244},{46,180,242},{44,183,240},{42,185,238},{40,188,235},{39,190,233},{37,192,231},{35,195,228},{34,197,226},{32,199,223},{31,201,221},{30,203,218},{28,205,216},{27,208,213},{26,210,210},{26,212,208},{25,213,205},{24,215,202},{24,217,200},{24,219,197},{24,221,194},{24,222,192},{24,224,189},{25,226,187},{25,227,185},{26,228,182},{28,230,180},{29,231,178},{31,233,175},{32,234,172},{34,235,170},{37,236,167},{39,238,164},{42,239,161},{44,240,158},{47,241,155},{50,242,152},{53,243,148},{56,244,145},{60,245,142},{63,246,138},{67,247,135},{70,248,132},{74,248,128},{78,249,125},{82,250,122},{85,250,118},{89,251,115},{93,252,111},{97,252,108},{101,253,105},{105,253,102},{109,254,98},{113,254,95},{117,254,92},{121,254,89},{125,255,86},{128,255,83},{132,255,81},{136,255,78},{139,255,75},{143,255,73},{146,255,71},{150,254,68},{153,254,66},{156,254,64},{159,253,63},{161,253,61},{164,252,60},{167,252,58},{169,251,57},{172,251,56},{175,250,55},{177,249,54},{180,248,54},{183,247,53},{185,246,53},{188,245,52},{190,244,52},{193,243,52},{195,241,52},{198,240,52},{200,239,52},{203,237,52},{205,236,52},{208,234,52},{210,233,53},{212,231,53},{215,229,53},{217,228,54},{219,226,54},{221,224,55},{223,223,55},{225,221,55},{227,219,56},{229,217,56},{231,215,57},{233,213,57},{235,211,57},{236,209,58},{238,207,58},{239,205,58},{241,203,58},{242,201,58},{244,199,58},{245,197,58},{246,195,58},{247,193,58},{248,190,57},{249,188,57},{250,186,57},{251,184,56},{251,182,55},{252,179,54},{252,177,54},{253,174,53},{253,172,52},{254,169,51},{254,167,50},{254,164,49},{254,161,48},{254,158,47},{254,155,45},{254,153,44},{254,150,43},{254,147,42},{254,144,41},{253,141,39},{253,138,38},{252,135,37},{252,132,35},{251,129,34},{251,126,33},{250,123,31},{249,120,30},{249,117,29},{248,114,28},{247,111,26},{246,108,25},{245,105,24},{244,102,23},{243,99,21},{242,96,20},{241,93,19},{240,91,18},{239,88,17},{237,85,16},{236,83,15},{235,80,14},{234,78,13},{232,75,12},{231,73,12},{229,71,11},{228,69,10},{226,67,10},{225,65,9},{223,63,8},{221,61,8},{220,59,7},{218,57,7},{216,55,6},{214,53,6},{212,51,5},{210,49,5},{208,47,5},{206,45,4},{204,43,4},{202,42,4},{200,40,3},{197,38,3},{195,37,3},{193,35,2},{190,33,2},{188,32,2},{185,30,2},{183,29,2},{180,27,1},{178,26,1},{175,24,1},{172,23,1},{169,22,1},{167,20,1},{164,19,1},{161,18,1},{158,16,1},{155,15,1},{152,14,1},{149,13,1},{146,11,1},{142,10,1},{139,9,2},{136,8,2},{133,7,2},{129,6,2},{126,5,2},{122,4,3}};
    constexpr int MAXN = 256;
    int t = MAXN / (c_max_ + 1);
    for (auto &&vertex : vertexs_) {
        auto &color = turbo_srgb_bytes[t * vertex.getC()];
        GA.fillColor(vertex.getNode()) = Color(color[0], color[1], color[2]);
    }
}

void KCoreLayout::partition() {
    std::list<std::weak_ptr<Component>> components_list;    //用链表存储需要处理的组件

    //初始化，添加第一个组件
    {
        componets_.emplace_back(std::make_shared<Component>());
        auto &component = componets_.back();
        auto &nodes = component->getNodes();
        for (int i = 0; i < V_; ++i) nodes.insert(i);
        components_list.push_back(component);
    }

    int n;

    while ((n = components_list.size()) > 0) {
        //循环处理component
        auto iter = components_list.begin();
        while (n--) {
            auto component = iter->lock();
            auto &component_nodes = component->getNodes();

            auto shell = component->addShell();
            auto &shell_nodes = shell->getNodes();

            int c = component->getC();
            for (auto u : shells_[c]) {
                if (component_nodes.count(u)) {
                    shell_nodes.insert(u);
                    vertexs_[u].setComponent(component);
                    component_nodes.erase(u);
                }
            }

            /*
             * 计算剩下的component
             */
            ++c;    //core的shell index
            //1.计算连通分量
            auto new_components_nodes = calForConnected(component_nodes);
            //2.生成新component
            if (new_components_nodes.size() == 0) {
                iter = components_list.erase(iter);
            } else if (new_components_nodes.size() == 1) {
                //只有一个连通分量
                component->setC(c);
            } else {
                //多个连通分量
                iter = components_list.erase(iter);
                double phi_init = utils::Random<double>::get(Param::TWOPI);
                int sum_sz = 0;
                double rate, t;
                for (auto &new_component_nodes : new_components_nodes) {
                    componets_.emplace_back(std::make_shared<Component>());
                    auto &new_component = componets_.back();

                    //对新的连通分量进行初始化
                    new_component->setC(c);
                    new_component->setParent(component);

                    rate = static_cast<double>(new_component_nodes.size()) / component_nodes.size();
                    new_component->setU(component->getU() * rate);
                    new_component->setRho(Param::ONE - rate);
                    phi_init += Param::TWOPI * rate;
                    new_component->setPhi(phi_init);

                    t = (c_max_ - c) * component->getU() * new_component->getRho();
                    new_component->setA(t * std::cos(new_component->getPhi()));
                    new_component->setB(t * std::sin(new_component->getPhi()));

                    new_component->setCdr();

                    //TODO:这里使用move应该不会有什么问题
                    new_component->getNodes() = std::move(new_component_nodes);
                    //将new_component放入需要处理的组件中
                    components_list.push_back(new_component);
                }
            }
        }

    }

}

void KCoreLayout::layout() {
    for (auto &component : componets_) {
        //cal alpha
        auto self = shared_from_this();
        component->layout(shared_from_this());
    }
    for (int c = 0; c < c_max_; ++c) {
        for (auto u : shells_[c]) {
            auto &vertex = vertexs_[u];
            vertex.setRhoA(c_max_ - c);

            int cnt = 0, sum = 0, t;
            for (auto v : adjList_[u]) {
                t = vertexs_[v].getC();
                if (t >= c) {
                    ++cnt;
                    sum += c_max_ - t;
                }
            }
            if (cnt > 0) {
                vertex.setRhoB(static_cast<double>(sum) / cnt);
            } else {
                vertex.setRhoB(0);
            }

            double unit = vertex.getComponent().lock()->getU();
            vertex.setA(unit * std::cos(vertex.getAlpha()));
            vertex.setB(unit * std::sin(vertex.getAlpha()));

            vertex.setCdr();
        }
    }
    for (auto u : shells_[c_max_]) {
        auto &vertex = vertexs_[u];
        double unit = vertex.getComponent().lock()->getU();
        vertex.setA(unit * std::cos(vertex.getAlpha()));
        vertex.setB(unit * std::sin(vertex.getAlpha()));

        vertex.setRandom();
    }
}

void KCoreLayout::draw(GraphAttributes &GA) {
    /*
    double min_x = INT_MAX, min_y = INT_MAX;
    for (int i = 0; i < V_; ++i) {
        auto &vertex = vertexs_[i];
        min_x = std::min(min_x, vertex.getX());
        min_y = std::min(min_y, vertex.getY());
    }
    min_x = std::max(-min_x, 0.0) + 200;
    min_y = std::max(-min_y, 0.0) + 200;
    for (int i = 0; i < V_; ++i) {
        auto &vertex = vertexs_[i];
        GA.x(vertex.getNode()) = vertex.getX() + min_x;
        GA.y(vertex.getNode()) = vertex.getY() + min_y;
    }
     */


    for (int i = 0; i < V_; ++i) {
        auto &vertex = vertexs_[i];
        GA.x(vertex.getNode()) = vertex.getX();
        GA.y(vertex.getNode()) = vertex.getY();
    }
}

void KCoreLayout::call(GraphAttributes &GA) {

    initGraph(GA);
    kCoreDecomposition();
    colour(GA);
    partition();
    layout();
    draw(GA);

    /*
    auto&& G = GA.constGraph();
    for (auto node : G.nodes) {
        GA.x(node) = utils::Random<double>::get(100);
        GA.y(node) = utils::Random<double>::get(100);
    }
     */
}

KCoreLayout::KCoreLayout() {
    //初始化参数
    SingleTon<Param>::getInstance(0.4, 300, 300);
}

KCoreLayout::~KCoreLayout() {}

std::vector<Vertex> &KCoreLayout::getVertexs() {
    return vertexs_;
}
