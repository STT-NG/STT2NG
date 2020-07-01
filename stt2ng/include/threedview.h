#pragma once

#include <QObject>
#include <QWidget>
#include <Qt3DRender>
#include <Qt3DCore>
#include <Qt3DExtras>

class ThreeDView : QObject
{
    Q_OBJECT
public:
    ThreeDView(QObject *parent = nullptr);

    void addDirectionalLight(const QColor &color, float intensity, const QVector3D &direction);

    QWidget *createWidget() const;

    Qt3DCore::QEntity *getRoot() const;

private:
    Qt3DExtras::Qt3DWindow *window;

    Qt3DCore::QEntity *root;
    Qt3DRender::QCamera *camera;
};

