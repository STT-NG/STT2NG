#pragma once

#include <map>
#include <memory>

#include <GRNode>
#include <QObject>

class GraphModel : public QObject {
    Q_OBJECT
    using GRNode = GeomRel::GRNode;
public:
    explicit GraphModel() {}
    virtual ~GraphModel() {}

    void addNode(std::unique_ptr<GRNode> node);
    void addEdge(int from_id, int to_id, int order);

    void removeNode(int id);

    void removeAllNodes();
    void removeAllEdges();

    std::vector<GRNode *> getNodes() {
        std::vector<GRNode *> vec;
        for (auto &[id, node] : nodes) {
            vec.push_back(node.get());
        }
        return vec;
    }

    GRNode *getNode(int id) {
        auto it = nodes.find(id);
        if (it != nodes.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    void selectNode(int id) {
        if (selectedNode != id){
            selectedNode = id;
            emit nodeSelected(id, true);
        }
    }

    void deselectNode(int id) {
        emit nodeSelected(id, false);
        selectedNode = -1;
    }

    int currentSelection() const {return selectedNode;}

signals:
    void nodeAdded(GRNode *node);
    void edgeAdded(GRNode *from, GRNode *to, int order);

    void nodeRemoved(GRNode *node);
    void edgeRemoved(GRNode *from, GRNode *to);

    void nodeSelected(int id, bool selected);

private:
    std::map<int, int> ids;
    std::map<int, std::unique_ptr<GRNode>> nodes;
    std::map<int, std::set<int>> edges;

    int selectedNode = -1;
};
