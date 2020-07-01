#include "graphview.h"

#include <QWheelEvent>
#include <QScrollBar>
#include <QGraphicsItem>

GraphView::GraphView(QWidget *parent) : QGraphicsView(parent)
{
    //setDragMode(QGraphicsView::ScrollHandDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    //setCacheMode(QGraphicsView::CacheBackground);
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setRenderHint(QPainter::LosslessImageRendering);
}

void GraphView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        // zoom
        const ViewportAnchor anchor = transformationAnchor();
        int angle = event->angleDelta().y();
        qreal factor;
        if (angle > 0) {
            factor = 1.15;
        } else {
            factor = 0.85;
        }
        const double oldScale = _scale;
        _scale *= factor;
        if (_scale >= 0.8) {
            scale(factor, factor);
            setTransformationAnchor(anchor);
        } else {
            _scale = oldScale;
        }
    } else {
        QGraphicsView::wheelEvent(event);
    }
}

void GraphView::resizeEvent(QResizeEvent *event)
{
    if (firstResize) {
        fitInView(sceneRect(), Qt::KeepAspectRatio);
        firstResize = false;
    }

    QGraphicsView::resizeEvent(event);
}

void GraphView::mouseMoveEvent(QMouseEvent *event)
{
    if (_pan)
    {
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (event->x() - _panStartX));
        verticalScrollBar()->setValue(verticalScrollBar()->value() - (event->y() - _panStartY));
        _panStartX = event->x();
        _panStartY = event->y();
        event->accept();
        return;
    }
    QGraphicsView::mouseMoveEvent(event);
}

void GraphView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton)
    {
        _pan = true;
        _panStartX = event->x();
        _panStartY = event->y();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    QGraphicsView::mousePressEvent(event);
}

void GraphView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton)
    {
        _pan = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }
    QGraphicsView::mouseReleaseEvent(event);
}
