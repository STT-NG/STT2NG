#pragma once

#include "graphscene.h"
#include "graphview.h"
#include "graphbuilder.h"
#include "graphmodel.h"

#include <QWidget>
#include <QStandardItemModel>
#include <QListView>
#include <QTreeView>

class GraphWidget : public QWidget
{
public:
    explicit GraphWidget(QWidget *parent = nullptr);
    ~GraphWidget() = default;

    void setBuilder(GraphBuilder *builder) { graphBuilder = builder; }

    GraphModel *getModel() const {return graphModel;}
    std::vector<GraphScene *> &getScenes() {return scenes;}
    std::map<int, GraphView *> &getViews() {return views;}

    GraphScene *getScene(int scene_id) { return scenes.at(scene_id); }
    GraphView *getView(int scene_id) {return views.at(scene_id);}
    GraphBuilder *getBuilder() { return graphBuilder; }

    void addDetectionEvents(const QVector<DetectionEvent> &events);
    void addDetectionEvent(const DetectionEvent &event);
    void clearEvents();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    GraphModel *graphModel;

    std::vector<GraphScene *> scenes;
    std::map<int, GraphView *> views;

    GraphBuilder *graphBuilder;

    int eventSetCount = 0;
    QStandardItemModel *eventModel;
    QTreeView *eventView;

    QStandardItem *eventToItem(const DetectionEvent &event);

    void setupEventList();

    bool shown = false;
};
