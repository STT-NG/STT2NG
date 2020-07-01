#include "graphwriter.h"

#include <fstream>
#include <iostream>

using namespace GeomRel;

GraphWriter::GraphWriter(const std::vector<GRNode *> &nodes)
    : nodes(nodes)
{}

bool GraphWriter::writeCSV(std::string path, std::string *error)
{
    if (nodes.empty()) {
        *error =  "No nodes to output!";
        return false;
    }

    std::ofstream stream;

    stream.open(path, std::ios::out);
    if (!stream.is_open()) {
        *error = "Failed to open file for writing.";
        return false;
    }

    // write column header
    stream << "Id,neighbours" << std::endl;
    for (auto node : nodes) {
        stream << node->id();
        for (int ord = 1; ord <= node->maxOrder(); ++ord) {
            for (int id : node->neighbours(ord)) {
                stream << "," << id;
            }
        }
        stream << std::endl;
    }
    stream.close();

    return true;
}
