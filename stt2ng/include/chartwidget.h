#pragma once

#include <GRNode>

#include <QChartView>
#include <QScatterSeries>

using namespace QtCharts;

class ChartWidget : public QWidget
{
    Q_OBJECT
    using GRNode = GeomRel::GRNode;
public:
    explicit ChartWidget(QWidget *parent = nullptr);

    void createXY(const std::vector<std::unique_ptr<GRNode> > &nodes);
    void createZX(const std::vector<std::unique_ptr<GRNode>> &nodes);
};

