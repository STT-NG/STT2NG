#pragma once

#include <QObject>

#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>

#include <GRVector>
#include <GRCylinder>

class ThreeDScene : public QObject
{
    Q_OBJECT
    using GRVector3 = GeomRel::GRVector3;
    using GRCylinder = GeomRel::GRCylinder;
public:
    explicit ThreeDScene(Qt3DCore::QEntity *rootEntity);
    ~ThreeDScene();

    void addCylinder(const std::unique_ptr<GRCylinder> &cylinder);
    void addCylinder(const GRVector3& position, const GRVector3& direction, double radius, double length);

    void setDistanceScale(float scale);

private:
    Qt3DCore::QEntity *m_rootEntity;
    Qt3DRender::QMaterial *material;
    float distScale;

    QVector<QPair<Qt3DCore::QTransform *, QVector3D>> entityTransforms;
};

