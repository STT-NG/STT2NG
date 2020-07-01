#pragma once

#include <GeomRel>

class GraphWriter
{
    using GRNode = GeomRel::GRNode;
public:
    GraphWriter(const std::vector<GRNode *> &nodes);
    virtual ~GraphWriter() {}

    bool writeCSV(std::string path, std::string *error);

private:
    std::vector<GRNode *> nodes;
};

