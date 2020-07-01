#include "visualgraphedge.h"

#include <cmath>
#include <QPen>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>

#include "visualgraphnode.h"

VisualGraphEdge::VisualGraphEdge(VisualGraphNode *start, VisualGraphNode *end, QGraphicsItem *parent)
    : QGraphicsLineItem(parent),
      _fromNode(start),
      _toNode(end)
{
    setFlag(QGraphicsItem::ItemIsSelectable);
    setPen(QPen(_color, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    setZValue(-1000);
    setVisible(false);
}

QRectF VisualGraphEdge::boundingRect() const
{
    qreal extra = (pen().width() + 20) / 2.0;

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
                                      line().p2().y() - line().p1().y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

QPainterPath VisualGraphEdge::shape() const
{
    QPainterPath path = QGraphicsLineItem::shape();
    return path;
}

void VisualGraphEdge::updatePosition()
{
    QLineF line(mapFromItem(_fromNode, _fromNode->polygon().boundingRect().center()), mapFromItem(_toNode, _toNode->polygon().boundingRect().center()));
    setLine(line);
}

void VisualGraphEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                  QWidget *)
{
    if (_fromNode->collidesWithItem(_toNode))
        return;

    qreal lod = QStyleOptionGraphicsItem::levelOfDetailFromTransform(painter->worldTransform());

    QPen myPen = pen();
    myPen.setColor(_color);
    painter->setPen(myPen);
    painter->setBrush(_color);

    painter->drawLine(line());
}
