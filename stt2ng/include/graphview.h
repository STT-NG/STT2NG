#pragma once

#include <QWidget>
#include <QGraphicsView>

class GraphView : public QGraphicsView
{

    Q_OBJECT
public:
    explicit GraphView(QWidget *parent = nullptr);
    ~GraphView() {};

protected slots:

    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

    bool _pan = false;
    int _panStartX, _panStartY;
    double _scale = 1;

    bool firstResize = true;
};

