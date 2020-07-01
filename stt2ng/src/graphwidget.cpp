#include "graphwidget.h"

#include <QComboBox>
#include <QGroupBox>
#include <QLayout>
#include <QKeyEvent>

GraphWidget::GraphWidget(QWidget *parent)
    : QWidget(parent),
      graphModel(new GraphModel),
      graphBuilder(new GraphBuilder(graphModel, this)),
      eventModel(new QStandardItemModel(this))
{
    auto xy_scene = new GraphScene(graphModel, GraphScene::Axes::XY, this);
    auto xz_scene = new GraphScene(graphModel, GraphScene::Axes::XZ, this);

    auto xy_view = new GraphView(this);
    xy_view->setScene(xy_scene);

    auto xz_view = new GraphView(this);
    xz_view->setScene(xz_scene);

    scenes.push_back(xy_scene);
    scenes.push_back(xz_scene);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(xy_view);
    layout->addWidget(xz_view);

    setupEventList();
}

void GraphWidget::addDetectionEvents(const QVector<DetectionEvent> &events)
{
    auto root = new QStandardItem(QStringLiteral("Event set %1").arg(++eventSetCount));
    root->setSelectable(true);
    root->setCheckable(false);
    root->setDragEnabled(false);
    root->setEditable(false);
    root->setData(QVariant::fromValue(QStringLiteral("EventSet")), Qt::UserRole + 1);

    for (const auto &event : events) {
        auto item = eventToItem(event);

        root->appendRow(item);
    }

    eventModel->appendRow(root);
}

void GraphWidget::addDetectionEvent(const DetectionEvent &event)
{
    auto item = eventToItem(event);

    eventModel->appendRow(item);
}

QStandardItem *GraphWidget::eventToItem(const DetectionEvent &event)
{
    auto item = new QStandardItem(QStringLiteral("Detection ID %1").arg(event.id));
    item->setCheckable(true);
    item->setSelectable(false);
    item->setDragEnabled(false);
    item->setEditable(false);
    item->setCheckState(Qt::CheckState::Unchecked);
    item->setData(QVariant::fromValue(QStringLiteral("DetectionEvent")), Qt::UserRole + 1);
    item->setData(true, Qt::UserRole + 2);

    for (const auto &trajectory : event.getTrajectories()) {
        auto trajectoryItem = new QStandardItem(QStringLiteral("Trajectory ID %1").arg(trajectory.id));
        trajectoryItem->setCheckable(true);
        trajectoryItem->setSelectable(false);
        trajectoryItem->setDragEnabled(false);
        trajectoryItem->setEditable(false);
        trajectoryItem->setCheckState(Qt::CheckState::Unchecked);
        trajectoryItem->setData(QVariant::fromValue(trajectory));

        QPixmap pixmap(10,10);
        pixmap.fill(trajectory.getColor());
        QIcon icon(pixmap);
        trajectoryItem->setIcon(icon);

        item->appendRow(trajectoryItem);
    }

    return item;
}

void GraphWidget::clearEvents()
{
    eventModel->clear();
}

void GraphWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key::Key_Delete) {

        auto indices = eventView->selectionModel()->selectedIndexes();

        for (auto &index : indices) {
            if (index.isValid()) {
                auto item = eventModel->itemFromIndex(index);
                auto data = item->data();

                if (data.toString() == QStringLiteral("EventSet")) {
                    eventSetCount--;

                    for (int i = 0; i < item->rowCount(); ++i) {
                        auto event = item->child(i);
                        for (int j = 0; j < event->rowCount(); ++j) {
                            auto trajectory = event->child(j);
                            trajectory->setCheckState(Qt::CheckState::Unchecked);
                        }
                    }

                    eventModel->removeRow(item->row());
                }
            }
        }
        return;
    }

    QWidget::keyPressEvent(event);
}

void GraphWidget::setupEventList()
{
    connect(eventModel, &QStandardItemModel::itemChanged, this, [=] (QStandardItem *item) {
        auto state = item->checkState();
        auto roleOrTrajectory = item->data(Qt::UserRole + 1);
        auto conjunctionFlag = item->data(Qt::UserRole + 2);

        if (state == Qt::CheckState::Checked){
            if (roleOrTrajectory.toString() == QStringLiteral("DetectionEvent")) {
                eventView->setExpanded(item->index(), true);
                // enable all child objects
                for (int i = 0; i < item->rowCount(); ++i) {
                    item->child(i)->setCheckState(Qt::CheckState::Checked);
                }
            } else {
                if (auto parent = item->parent(); parent) {
                    bool allChecked = true;
                    for (int i = 0; i < parent->rowCount(); ++i) {
                        auto child = parent->child(i);
                        if (child->checkState() != Qt::CheckState::Checked) {
                            allChecked = false;
                            break;
                        }
                    }

                    if (allChecked && parent->checkState() == Qt::CheckState::Unchecked) {
                        parent->setCheckState(Qt::CheckState::Checked);
                    }
                }
                auto trajectory = qvariant_cast<Trajectory>(roleOrTrajectory);

                for (auto scene : scenes) {
                    scene->drawTrajectory(trajectory, trajectory.getColor(), 1.25);
                }
            }
        } else if (state == Qt::CheckState::Unchecked) {
            if (roleOrTrajectory.toString() == QStringLiteral("DetectionEvent")) {
                // disable all child objects
                if (conjunctionFlag.isValid() && conjunctionFlag.toBool()) {
                    for (int i = 0; i < item->rowCount(); ++i) {
                        item->child(i)->setCheckState(Qt::CheckState::Unchecked);
                    }
                } else if (conjunctionFlag.isValid()) {
                    eventModel->blockSignals(true);
                    item->setData(true, Qt::UserRole + 2);
                    eventModel->blockSignals(false);
                }
            } else {
                if (auto parent = item->parent(); parent) {
                    if (parent->checkState() == Qt::CheckState::Checked) {
                        eventModel->blockSignals(true);
                        parent->setData(false, Qt::UserRole + 2);
                        eventModel->blockSignals(false);
                        parent->setCheckState(Qt::CheckState::Unchecked);
                    }
                }
                auto trajectory = qvariant_cast<Trajectory>(roleOrTrajectory);

                for (auto scene : scenes) {
                    scene->drawTrajectory(trajectory, Qt::white, 1.0);
                }
            }
        }
    });

    QGroupBox *box = new QGroupBox("Events");
    box->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    QVBoxLayout *boxLayout = new QVBoxLayout(box);
    boxLayout->setSizeConstraint(QVBoxLayout::SizeConstraint::SetMinimumSize);

    eventView = new QTreeView(this);
    eventView->setSelectionMode(QTreeView::ExtendedSelection);
    eventView->setModel(eventModel);
    eventView->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    eventView->setIndentation(10);
    eventView->setAnimated(true);
    eventView->setHeaderHidden(true);

    boxLayout->addWidget(eventView);

    layout()->addWidget(box);
}
