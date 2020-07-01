#pragma once

#include "detectionevent.h"

#include <QFile>
#include <QString>


class DetectionEventReader
{
public:
    DetectionEventReader(const QString &file);

    QVector<DetectionEvent> parseEvents(bool *success);

private:
    QFile file;
};

