#pragma once

#include <GRNode>

#include <QGraphicsPolygonItem>

class VisualGraphNode : public QObject, public QGraphicsPolygonItem
{
    Q_OBJECT
    using GRNode = GeomRel::GRNode;
public:
    enum { Type = UserType + 15 };
    explicit VisualGraphNode(int id, double sizeX, double sizeY, QGraphicsItem *parent = nullptr);

    QPolygonF polygon() const {return shape;}
    int type() const override { return Type; }

    const int id;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

signals:
    void selectedChange(bool selected);

private:
    QPolygonF shape;
};

