#include "graphbuilder.h"

#include "graphmodel.h"

using namespace GeomRel;

GraphBuilder::GraphBuilder(GraphModel *model, QObject *parent)
    : QObject(parent),
      model(model)
{}

void GraphBuilder::build(int order, double tolerance, std::function<void(void)> progressCallback)
{
    auto nodes = model->getNodes();

    builder.setNodes(nodes);
    builder.build(order, tolerance,
        progressCallback,
        [=](int id, int other, int ord) {
            model->addEdge(id, other, ord);
        }
    );
    emit buildCompleted();
}

void GraphBuilder::clearAll()
{
    clearEdges();
    clearNodes();
}

void GraphBuilder::clearNodes()
{
    model->removeAllNodes();
}

void GraphBuilder::clearEdges()
{
    model->removeAllEdges();
}

int GraphBuilder::getNodeCount()
{
    if (model) {
        return model->getNodes().size();
    }
    return 0;
}

void GraphBuilder::addNodes(std::vector<std::unique_ptr<GRNode>> nodes)
{
    clearAll();

    for (int i = 0; i < nodes.size(); ++i) {
        model->addNode(std::move(nodes[i]));
    }
}
