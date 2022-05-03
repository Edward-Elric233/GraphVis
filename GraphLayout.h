// Copyright(C), Edward-Elric233
// Author: Edward-Elric233
// Version: 1.0
// Date: 2022/4/25
// Description: 基于K核分解的层次布局类
#ifndef GRAPHLAYOUT_GRAPHLAYOUT_H
#define GRAPHLAYOUT_GRAPHLAYOUT_H

#include "utils.h"
#include <functional>
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>


class GraphLayout {
    using Graph = ogdf::Graph;
    using GraphAttributes = ogdf::GraphAttributes;
    using DealFunc = std::function<void(Graph&, GraphAttributes&)>;
    using OutputFunc = std::function<void(const Graph&, const GraphAttributes&)>;
public:
    GraphLayout(Graph&, GraphAttributes&);
    GraphLayout(const GraphLayout&) = delete;   //禁止拷贝初始化
    void operator= (const GraphLayout&) = delete;   //禁止拷贝赋值
    ~GraphLayout();

    DealFunc getIntput() const {
        return input_;
    }
    OutputFunc getOutput() const {
        return output_;
    }
    DealFunc getLayout() const {
        return layout_;
    }
    void setInput(DealFunc input)  {
        input_ = std::move(input);
    }
    void setOutput(OutputFunc output) {
        output_ = std::move(output);
    }
    void setLayout(DealFunc graphLayout) {
        layout_ = std::move(graphLayout);
    }

    void operator()() {
        input_(G_, GA_);
        setGraphAttribute();
        layout_(G_, GA_);
        output_(G_, GA_);
    }
    static void outputWithName(const Graph& G, const GraphAttributes& GA, const std::string &name);
private:
    void setGraphAttribute();

    Graph &G_;
    GraphAttributes &GA_;
    DealFunc input_;
    DealFunc layout_;
    OutputFunc output_;
};


#endif //GRAPHLAYOUT_GRAPHLAYOUT_H
