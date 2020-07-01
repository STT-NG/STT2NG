#include "visualgraphnode.h"

#include <QPainter>
#include <QGraphicsScene>
#include <QStyleOptionGraphicsItem>
#include <QPixmapCache>
#include <cmath>


VisualGraphNode::VisualGraphNode(int id, double sizeX, double sizeY, QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent),
      id(id)
{
    QPainterPath path;
    path.addEllipse(0, 0, sizeX, sizeY);
    shape = path.toFillPolygon();

    setBrush(Qt::white);
    setPolygon(shape);

    setFlag(QGraphicsItem::ItemIsSelectable);

    setCacheMode(QGraphicsItem::NoCache);
}

void VisualGraphNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    auto opt = new QStyleOptionGraphicsItem;
    opt->state = option->state & ~QStyle::State_Selected;

    QGraphicsPolygonItem::paint(painter, opt, widget);
}

QVariant VisualGraphNode::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedChange)
    {
        emit selectedChange(value.toBool());
        update();
    }

    return QGraphicsItem::itemChange(change, value);
}
