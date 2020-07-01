#pragma once

#include <QVector>
#include <QtCore>
#include <QColor>

struct Trajectory
{
    Trajectory()
        : id(-1)
    {}

    Trajectory(int id)
        : id(id)
    {}

    Trajectory(int id, const QVector<int>& hits)
        : id(id),
          hitIDs(hits)
    {}

    const int id;
    QVector<int> getHits() const { return hitIDs; }

    void addHit(int id) {
        hitIDs.append(id);
    }

    void setColor(QColor color) {
        _color = color;
    }

    QColor getColor() const {
        return _color;
    }

private:
    QVector<int> hitIDs;
    QColor _color;
};

Q_DECLARE_METATYPE(Trajectory);
