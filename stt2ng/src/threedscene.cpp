#include "threedscene.h"

using namespace GeomRel;

ThreeDScene::ThreeDScene(Qt3DCore::QEntity *rootEntity)
    : m_rootEntity(rootEntity)
{
    auto *mat = new Qt3DExtras::QDiffuseSpecularMaterial;

    mat->setAmbient(QColor(200, 200, 200));
    mat->setShininess(1.f);

    material = mat;
}

ThreeDScene::~ThreeDScene()
{
}

void ThreeDScene::addCylinder(const std::unique_ptr<GRCylinder> &cylinder)
{
    addCylinder(cylinder->center, cylinder->direction, cylinder->radius, cylinder->length);
}

void ThreeDScene::addCylinder(const GRVector3 &position, const GRVector3 &direction, double radius, double length)
{
    // Cylinder shape data
    Qt3DExtras::QCylinderMesh *cylinder = new Qt3DExtras::QCylinderMesh();
    cylinder->setRadius(radius);
    cylinder->setLength(length);
    cylinder->setRings(20);
    cylinder->setSlices(20);

    auto dir = QVector3D(direction.x(), direction.y(), direction.z());

    // CylinderMesh Transform
    Qt3DCore::QTransform *cylinderTransform = new Qt3DCore::QTransform();
    cylinderTransform->setRotation(QQuaternion::fromDirection(dir, QVector3D(0, 1, 0)) * QQuaternion::fromEulerAngles(90, 0, 0));
    auto pos = QVector3D(position.x(), position.y(), position.z());
    cylinderTransform->setTranslation(pos);

    entityTransforms.append({cylinderTransform, pos});

    // Cylinder
    Qt3DCore::QEntity *cylinderEntity = new Qt3DCore::QEntity(m_rootEntity);
    cylinderEntity->addComponent(cylinder);
    cylinderEntity->addComponent(material);
    cylinderEntity->addComponent(cylinderTransform);
}

void ThreeDScene::setDistanceScale(float scale)
{
    if (scale < 0) {
        distScale = 1;
        return;
    } else {
        distScale = scale;
    }

    for (auto &[transform, position] : entityTransforms) {
        float z = position.z();
        auto newPos = position * distScale;
        newPos.setZ(z);
        transform->setTranslation(newPos);
    }
}
