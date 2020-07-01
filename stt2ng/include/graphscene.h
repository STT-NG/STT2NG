#pragma once

#include "visualgraphnode.h"
#include "visualgraphedge.h"
#include "trajectory.h"
#include "detectionevent.h"
#include "graphmodel.h"

#include <QGraphicsDropShadowEffect>
#include <QGraphicsScene>
#include <QMap>

class GraphScene : public QGraphicsScene
{
    Q_OBJECT
    using GRNode = GeomRel::GRNode;
public:
    enum class Axes : int {
        XY,
        XZ
    };

    explicit GraphScene(GraphModel *model, Axes axes = Axes::XY, QObject *parent = nullptr);
    ~GraphScene() {};

    void showDetection(const DetectionEvent &event);
    void hideDetection(const DetectionEvent &event);

    void drawTrajectory(const Trajectory &trajectory, QColor color, double scale);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

private slots:
    void createVisualNode(GRNode *node);
    void removeVisualNode(GRNode *node);

    void createVisualEdge(GRNode *from, GRNode *to, int order);
    void removeVisualEdge(GRNode *from, GRNode *to);

private:
    bool leftButtonDown;
    QPointF startPoint;
    QGraphicsLineItem *line;

    GraphModel *model;

    std::map<int, VisualGraphNode *> v_nodes;
    std::vector<VisualGraphEdge *> v_edges;

    Axes axes;

    int currentSelection = -1;

    void showNeighbours(VisualGraphNode *v_node);
    void hideNeighbours(VisualGraphNode *v_node);
};

