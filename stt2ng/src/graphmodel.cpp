#include "graphmodel.h"

using namespace GeomRel;

void GraphModel::addNode(std::unique_ptr<GRNode> node)
{
    emit nodeAdded(node.get());

    ids.insert(std::make_pair(node->id(), node->id()));
    nodes.insert(std::make_pair(node->id(), std::move(node)));
}

void GraphModel::addEdge(int from_id, int to_id, int order)
{
    auto it_from = nodes.find(from_id);
    auto it_to = nodes.find(to_id);
    if (it_from != nodes.end() && it_to != nodes.end()) {
        auto from = it_from->second.get();
        auto to = it_to->second.get();

        from->addNeighbour(to->id(), order);
        to->addNeighbour(from->id(), order);

        emit edgeAdded(from, to, order);

        auto it = edges.find(from->id());
        if (it != edges.end()) {
            auto set = it->second;
            set.insert(to->id());
        } else {
            edges.insert(std::make_pair(from->id(), std::set {to->id()}));
        }
    }
}

void GraphModel::removeNode(int id)
{
    auto it = nodes.find(id);
    if (it != nodes.end()){
        auto node = it->second.get();

        emit nodeRemoved(node);
        ids.erase(id);
        nodes.erase(it);
    }
}

void GraphModel::removeAllNodes()
{
    // make a local copy so the iterator is not invalidated
    auto ids_copy = std::map(ids);
    for (auto [pos, id] : ids_copy) {
        removeNode(id);
    }
    ids.clear();
    nodes.clear();
}

void GraphModel::removeAllEdges()
{
    for (auto [from_id, to_set] : edges) {
        auto it_from = nodes.find(from_id);
        for (int id : to_set) {
            auto it_to = nodes.find(id);
            if (it_from != nodes.end() && it_to != nodes.end()) {
                auto from = it_from->second.get();
                auto to = it_to->second.get();

                from->removeNeighbour(to->id());
                to->removeNeighbour(from->id());
                emit edgeRemoved(from, to);
            }
        }
    }
    edges.clear();
}
