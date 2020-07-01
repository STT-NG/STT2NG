#pragma once

#include <QGraphicsLineItem>

class VisualGraphNode;

class VisualGraphEdge : public QGraphicsLineItem
{
public:
    enum { Type = UserType + 4 };
    VisualGraphEdge(VisualGraphNode *start, VisualGraphNode *end, QGraphicsItem *parent = nullptr);

    int type() const override { return Type; }
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void setColor(const QColor &color) { _color = color; }
    VisualGraphNode *fromNode() const { return _fromNode; }
    VisualGraphNode *toNode() const { return _toNode; }

    void updatePosition();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    VisualGraphNode *_fromNode;
    VisualGraphNode *_toNode;

    QColor _color = Qt::black;
};

