#pragma once

#include "trajectory.h"

class DetectionEvent
{
public:
    DetectionEvent()
        : id(-1)
    {}

    DetectionEvent(int id, const QVector<Trajectory> &trajectories)
        : trajectories(std::move(trajectories)),
          id(id)
    {}

    const QVector<Trajectory> &getTrajectories() const {return trajectories;}
    const int id;

private:
    QVector<Trajectory> trajectories;
};
