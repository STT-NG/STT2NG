#include "detectioneventreader.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

DetectionEventReader::DetectionEventReader(const QString &file)
    : file(file)
{}

QVector<DetectionEvent> DetectionEventReader::parseEvents(bool *success)
{
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::information(nullptr, "Unable to open file for reading", file.errorString());
        *success = false;
        return {};
    }

    // used for coloring
    const double arc_phi = 0.618033988749895;

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);

    QVector<DetectionEvent> eventList;
    auto root = doc.object();
    if (root.contains("Events")) {
        QJsonArray events = root.value("Events").toArray();
        int i = 0;
        for (const auto &detection : events) {
            QJsonObject detectionObject = detection.toObject();
            int d_id;
            if (detectionObject.contains("ID")) {
                d_id = detectionObject.value("ID").toInt();
            } else {
                d_id = i;
                i++;
            }

            QVector<Trajectory> trajectories;
            int j = 0;
            if(detectionObject.contains("Trajectories")) {
                double h = 1.0 / (d_id + 1.0);

                QJsonArray trajectoryArray = detectionObject.value("Trajectories").toArray();
                for (const auto &trajectory : trajectoryArray) {
                    QJsonObject trajectoryObject = trajectory.toObject();
                    int t_id;
                    if (trajectoryObject.contains("ID")) {
                        t_id = trajectoryObject.value("ID").toInt();
                    } else {
                        t_id = j;
                        j++;
                    }

                    QVector<int> hits;
                    if (trajectoryObject.contains("Hits")) {
                        QJsonArray hitArray = trajectoryObject.value("Hits").toArray();
                        for (const auto &v : hitArray) {
                            hits.append(v.toInt());
                        }
                    }

                    h += arc_phi;
                    h = std::fmod(h, 1.0);

                    auto traj = Trajectory(t_id, hits);
                    traj.setColor(QColor::fromHslF(h, 1.0, 0.45));

                    trajectories.append(std::move(traj));
                }
            }

            eventList.append(DetectionEvent(d_id, trajectories));
        }
    }
    *success = true;
    return eventList;
}
