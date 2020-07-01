#pragma once

#include <GRNode>
#include <GRBuilder>
#include <QObject>
#include <memory>

class GraphModel;

class GraphBuilder : public QObject
{
    Q_OBJECT
    using GRNode = GeomRel::GRNode;
    using GRBuilder = GeomRel::GRBuilder;
public:
    GraphBuilder(GraphModel *model, QObject *parent = nullptr);
    virtual ~GraphBuilder() {};

    void build(int order, double tolerance, std::function<void(void)> progressCallback = [](){});
    void clearAll();
    void clearNodes();
    void clearEdges();

    int getNodeCount();

    void addNodes(std::vector<std::unique_ptr<GRNode> > nodes);

signals:
    void buildCompleted();

private:
    GraphModel *model;

    GRBuilder builder;
};

