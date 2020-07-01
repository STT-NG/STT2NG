#include "chartwidget.h"
#include <QLayout>
#include <QHBoxLayout>
#include <QValueAxis>

ChartWidget::ChartWidget(QWidget *parent)
    : QWidget(parent)
{
    setLayout(new QHBoxLayout);
}

void ChartWidget::createXY(const std::vector<std::unique_ptr<GRNode>> &nodes)
{
    QChart *chart = new QChart;
    chart->setTitle("X-Y Projection");
    chart->setDropShadowEnabled(false);

    QScatterSeries *series = new QScatterSeries;
    series->setName("Tube center");
    series->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    series->setColor("black");
    series->setMarkerSize(5);

    double maxX, minX, maxY, minY;

    maxX = maxY = std::numeric_limits<double>::min();
    minX = minY = std::numeric_limits<double>::max();

    for (auto &node : nodes) {
        series->append(node->posX(), node->posY());

        maxX = std::max(node->posX(), maxX);
        maxY = std::max(node->posY(), maxY);

        minX = std::min(node->posX(), minX);
        minY = std::min(node->posY(), minY);
    }

    chart->addSeries(series);

    QValueAxis *xAxis = new QValueAxis;
    QValueAxis *yAxis = new QValueAxis;

    chart->addAxis(xAxis, Qt::AlignBottom);
    chart->addAxis(yAxis, Qt::AlignLeft);
    series->attachAxis(xAxis);
    series->attachAxis(yAxis);

    xAxis->setTitleText("X");
    xAxis->setTickType(QValueAxis::TicksDynamic);
    xAxis->setTickAnchor(0);
    xAxis->setTickInterval(5);
    xAxis->setRange(minX, maxX);
    xAxis->setLabelFormat("%.2f");

    yAxis->setTitleText("Y");
    yAxis->setTickType(QValueAxis::TicksDynamic);
    yAxis->setTickAnchor(0);
    yAxis->setTickInterval(5);
    yAxis->setMinorTickCount(3);
    yAxis->setRange(minY, maxY);
    yAxis->setLabelFormat("%.2f");

    chart->legend()->setMarkerShape(QLegend::MarkerShapeFromSeries);

    QChartView *view = new QChartView(chart, this);
    view->setRenderHint(QPainter::Antialiasing);
    //view->setRubberBand(QChartView::RectangleRubberBand);

    layout()->addWidget(view);
}

void ChartWidget::createZX(const std::vector<std::unique_ptr<GRNode> > &nodes)
{
    QChart *chart = new QChart;
    chart->setTitle("Z-X Projection");
    chart->setDropShadowEnabled(false);

    QScatterSeries *series = new QScatterSeries;
    series->setName("Tube center");
    series->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    series->setColor("black");
    series->setMarkerSize(5);

    double maxZ, minZ, maxX, minX;

    maxZ = maxX = std::numeric_limits<double>::min();
    minZ = minX = std::numeric_limits<double>::max();

    for (auto &node : nodes) {
        series->append(node->posZ(), node->posX());

        maxZ = std::max(node->posZ(), maxZ);
        maxX = std::max(node->posX(), maxX);

        minZ = std::min(node->posZ(), minZ);
        minX = std::min(node->posX(), minX);
    }

    chart->addSeries(series);

    QValueAxis *xAxis = new QValueAxis;
    QValueAxis *yAxis = new QValueAxis;

    chart->addAxis(xAxis, Qt::AlignBottom);
    chart->addAxis(yAxis, Qt::AlignLeft);
    series->attachAxis(xAxis);
    series->attachAxis(yAxis);

    xAxis->setTitleText("Z");
    xAxis->setTickType(QValueAxis::TicksDynamic);
    xAxis->setTickAnchor(0);
    xAxis->setTickInterval(5);
    xAxis->setRange(minZ, maxZ);
    xAxis->setLabelFormat("%.2f");

    yAxis->setTitleText("X");
    yAxis->setTickType(QValueAxis::TicksDynamic);
    yAxis->setTickAnchor(0);
    yAxis->setTickInterval(5);
    yAxis->setRange(minX, maxX);
    yAxis->setLabelFormat("%.2f");

    chart->legend()->setMarkerShape(QLegend::MarkerShapeFromSeries);

    QChartView *view = new QChartView(chart, this);
    view->setRenderHint(QPainter::Antialiasing);
    //view->setRubberBand(QChartView::RectangleRubberBand);

    layout()->addWidget(view);
}
