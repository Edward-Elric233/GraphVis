// Copyright(C), Edward-Elric233
// Author: Edward-Elric233
// Version: 1.0
// Date: 2022/4/25
// Description: 
#include "GraphLayout.h"
#include <ogdf/basic/graph_generators.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/energybased/FMMMLayout.h>

using namespace ogdf;

void input(Graph& G, GraphAttributes& GA) {
    randomGraph(G, 1000, 2000);

    /*
    if (!GraphIO::read(GA, G, "output.gml", GraphIO::readGML)) {
        std::cerr << "can't find file" << std::endl;
        exit(0);
    }
     */

}

void GraphLayout::setGraphAttribute() {
    for (auto node : G_.nodes) {
        GA_.height(node) = 500;
        GA_.width(node) = 500;
        GA_.shape(node) = Shape::Ellipse;
        GA_.fillColor(node) = Color::Name::Antiquewhite;
        //GA.label(node) = to_string(node->index());
    }
    for (auto edge : G_.edges) {
        GA_.arrowType(edge) =  EdgeArrow::None;
        GA_.strokeColor(edge) = Color::Name::Grey;
        GA_.strokeWidth(edge) = 0.8;
    }
}

void GraphLayout::outputWithName(const Graph& G, const GraphAttributes& GA, const std::string &name) {
    GraphIO::write(GA, name + ".gml", GraphIO::writeGML);
    GraphIO::write(GA, name + ".svg", GraphIO::drawSVG);
}

void output(const Graph& G, const GraphAttributes& GA) {
    GraphLayout::outputWithName(G, GA, "output_std");
}

void layout(Graph& G, GraphAttributes& GA) {

    FMMMLayout fmmm;

    //recommended
    fmmm.useHighLevelOptions(true);
    /*
     * We then set the unit edge length (a scaling factor if you will), enable initial replacing of nodes and choose one of the available options from ogdf::FMMMOptions::QualityVsSpeed to tune tradeoffs between speed and aesthetic of the resulting graph. The only remaining high-level option is ogdf::FMMMOptions::PageFormatType which defaults to a Square if not explicitly set. These high-level options will then be used to derive the low-level settings accordingly.
     */
    fmmm.unitEdgeLength(50);
    fmmm.newInitialPlacement(true);
    fmmm.qualityVersusSpeed(FMMMOptions::QualityVsSpeed::GorgeousAndEfficient);

    fmmm.call(GA);
}

GraphLayout::GraphLayout(Graph& G, GraphAttributes& GA)
    : G_(G)
    , GA_(GA)
    , input_(input)
    , output_(output)
    , layout_(layout) {

}

GraphLayout::~GraphLayout() {

}