#include "graphscene.h"

#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <cmath>

GraphScene::GraphScene(GraphModel *model, GraphScene::Axes axes, QObject *parent)
    : QGraphicsScene(parent),
      model(model),
      axes(axes),
      line(nullptr)
{
    connect(model, &GraphModel::nodeAdded, this, &GraphScene::createVisualNode);
    connect(model, &GraphModel::nodeRemoved, this, &GraphScene::removeVisualNode);
    connect(model, &GraphModel::edgeAdded, this, &GraphScene::createVisualEdge);
    connect(model, &GraphModel::edgeRemoved, this, &GraphScene::removeVisualEdge);

    connect(model, &GraphModel::nodeSelected, this, [=](int id, bool selected) {
        auto it = v_nodes.find(id);
        if (it != v_nodes.end()) {
            auto v_node = it->second;
            if (selected) {

                auto it = v_nodes.find(currentSelection);
                if (it != v_nodes.end()) {
                    it->second->setSelected(false);
                }

                if (v_node->id != model->currentSelection()) {
                    v_node->setSelected(true);
                }

                v_node->setBrush(Qt::green);
                v_node->update();
                showNeighbours(v_node);
                currentSelection = id;
            } else {
                v_node->setBrush(Qt::white);
                v_node->update();
                hideNeighbours(v_node);
            }
        }
    });
}

void GraphScene::showDetection(const DetectionEvent &event)
{
    auto trajectories = event.getTrajectories();

    const double arc_phi = 0.618033988749895;

    double new_h = 1.0 / (event.id + 1.0);

    for (auto trajectory : trajectories) {
        new_h += arc_phi;
        new_h = std::fmod(new_h, 1.0);
        drawTrajectory(trajectory, QColor::fromHslF(new_h, 1.0, 0.45), 1.5);
    }
}

void GraphScene::hideDetection(const DetectionEvent &event)
{
    auto trajectories = event.getTrajectories();

    for (auto trajectory : trajectories) {
        drawTrajectory(trajectory, Qt::white, 1.0);
    }
}

void GraphScene::drawTrajectory(const Trajectory &trajectory, QColor color, double scale)
{
    for (auto hitID : trajectory.getHits()) {
        auto it = v_nodes.find(hitID);
        if (it == v_nodes.end()) continue;

        auto node = it->second;
        node->setBrush(color);
        node->setScale(scale);
    }
}

void GraphScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton) {

        line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(),
                                            mouseEvent->scenePos()));
        QGraphicsScene::addItem(line);
    }

    QGraphicsScene::mousePressEvent(mouseEvent);
}

void GraphScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (line != nullptr) {
        QLineF newLine(line->line().p1(), mouseEvent->scenePos());
        line->setLine(newLine);
    } else {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}

void GraphScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (line != nullptr) {
        QList<QGraphicsItem *> startItems = items(line->line().p1());
        if (startItems.count() && startItems.first() == line)
            startItems.removeFirst();
        QList<QGraphicsItem *> endItems = items(line->line().p2());
        if (endItems.count() && endItems.first() == line)
            endItems.removeFirst();

        QGraphicsScene::removeItem(line);
        delete line;

        if (startItems.count() > 0 && endItems.count() > 0 &&
            startItems.first()->type() == VisualGraphNode::Type &&
            endItems.first()->type() == VisualGraphNode::Type &&
            startItems.first() != endItems.first())
        {
            auto from = qgraphicsitem_cast<VisualGraphNode *>(startItems.first());
            auto to = qgraphicsitem_cast<VisualGraphNode *>(endItems.first());

            int from_id = from->id;
            int to_id = to->id;

            auto node = model->getNode(from_id);
            if (node){
                bool exists = false;
                for (auto v_edge : v_edges) {
                    if (v_edge->fromNode()->id == from_id &&
                        v_edge->toNode()->id == to_id)
                    {
                        exists = true;
                        break;
                    }
                }

                if (!exists) {
                    model->addEdge(from_id, to_id, 1);
                } else {
                    qDebug() << "Edge already exists";
                }
            }
        }
    }

    line = nullptr;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void GraphScene::createVisualNode(GRNode *node)
{
    VisualGraphNode *v_node = new VisualGraphNode(node->id(), node->sizeX(), node->sizeY());
    connect(v_node, &VisualGraphNode::selectedChange, this, [=](bool selected) {
        if (selected) {
            model->selectNode(node->id());
        } else {
            model->deselectNode(node->id());
        }
    });

    switch (axes) {
    case Axes::XY:
        v_node->setPos(node->posX(), node->posY());
        break;
    case Axes::XZ:
        v_node->setPos(node->posZ(), node->posX());
        break;
    }

    v_nodes.insert(std::make_pair(node->id(), v_node));

    addItem(v_node);
}

void GraphScene::removeVisualNode(GRNode *node)
{
    auto it = v_nodes.find(node->id());
    if (it != v_nodes.end()) {
        auto v_node = it->second;
        if (currentSelection == v_node->id) {
            hideNeighbours(v_node);
            currentSelection = -1;
        }

        removeItem(v_node);
        v_nodes.erase(it);
        delete v_node;
    }
}

void GraphScene::createVisualEdge(GRNode *from, GRNode *to, int order)
{
    auto it_from = v_nodes.find(from->id());
    auto it_to = v_nodes.find(to->id());
    if (it_from != v_nodes.end() && it_to != v_nodes.end()) {
        auto from = it_from->second;
        auto to = it_to->second;

        if (order == 1) {
            VisualGraphEdge *v_edge = new VisualGraphEdge(from, to);
            v_edges.push_back(v_edge);
            addItem(v_edge);
            v_edge->updatePosition();
        }
    }
}

void GraphScene::removeVisualEdge(GRNode *from, GRNode *to)
{
    for (int i = 0; i < v_edges.size(); ++i) {
        auto edge = v_edges.at(i);
        if (edge->fromNode()->id == from->id() &&
            edge->toNode()->id == to->id())
        {
            removeItem(edge);

            delete edge;

            v_edges.erase(v_edges.begin() + i);
            break;
        }
    }
}

void GraphScene::showNeighbours(VisualGraphNode *v_node)
{
    auto node = model->getNode(v_node->id);
    if (!node) return;

    auto direct = node->neighbours(1);

    for (int id : direct) {
        // draw direct edges
        for (auto edge : v_edges) {
            if (edge->fromNode()->id == v_node->id &&
                edge->toNode()->id == id)
            {
                edge->setVisible(true);
                edge->toNode()->setBrush(Qt::yellow);
            }
        }
    }

    const double arc_phi = 0.618033988749895;

    for (int i = 2; i <= node->maxOrder(); ++i) {
        auto neighbours = node->neighbours(i);

        for (int id : neighbours) {
            // color nodes based on order, don't draw edges
            auto it = v_nodes.find(id);
            if (it != v_nodes.end()) {
                auto v_node = it->second;

                double h, s, l;
                QColor(Qt::green).getHslF(&h, &s, &l);

                for (int j = 0; j < i; ++j) {
                    h += arc_phi;
                    h = std::fmod(h, 1.0);
                }
                auto color = QColor::fromHslF(h, s, l);
                v_node->setBrush(color);
                v_node->setPen(Qt::NoPen);

                auto shadow = new QGraphicsDropShadowEffect();
                shadow->setColor(color.darker(150));
                shadow->setOffset(0.15, 0.15);
                shadow->setBlurRadius(10);
                v_node->setGraphicsEffect(shadow);

                v_node->setScale(1.15);
                v_node->update();
            }
        }
    }
}

void GraphScene::hideNeighbours(VisualGraphNode *v_node)
{

    auto node = model->getNode(v_node->id);
    if (!node) return;

    auto direct = node->neighbours(1);

    for (int id : direct) {
        // hide direct edges
        for (auto edge : v_edges) {
            if (edge->fromNode()->id == v_node->id &&
                edge->toNode()->id == id)
            {
                edge->setVisible(false);
                edge->toNode()->setBrush(Qt::white);
            }
        }
    }

    for (int i = 2; i <= node->maxOrder(); ++i) {
        auto neighbours = node->neighbours(i);

        for (int id : neighbours) {
            // set nodes back to default color
            auto it = v_nodes.find(id);
            if (it != v_nodes.end()) {
                auto v_node = it->second;
                v_node->setBrush(Qt::white);
                v_node->setPen(QPen(Qt::SolidLine));
                v_node->setGraphicsEffect(nullptr);
                v_node->setScale(1.0);
                v_node->update();
            }
        }
    }
}
