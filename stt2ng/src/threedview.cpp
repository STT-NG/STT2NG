#include "threedview.h"

ThreeDView::ThreeDView(QObject *parent)
    : QObject(parent),
      window(new Qt3DExtras::Qt3DWindow),
      root(new Qt3DCore::QEntity)
{    
    camera = window->camera();
    camera->lens()->setPerspectiveProjection(60.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 20.0f));
    camera->setUpVector(QVector3D(0, 1, 0));
    camera->setViewCenter(QVector3D(0, 0, 0));

    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(root);
    camController->setCamera(camera);
    camController->setZoomInLimit(10);
    camController->setLinearSpeed(100);

    auto *renderer = window->defaultFrameGraph();
    renderer->setClearColor("#EEEEEE");

    window->setRootEntity(root);
}

QWidget *ThreeDView::createWidget() const {
    return QWidget::createWindowContainer(window);
}

Qt3DCore::QEntity *ThreeDView::getRoot() const
{
    return root;
}

void ThreeDView::addDirectionalLight(const QColor &color, float intensity, const QVector3D &direction)
{
    Qt3DCore::QEntity *lightEntity = new Qt3DCore::QEntity(root);
    Qt3DRender::QDirectionalLight *light = new Qt3DRender::QDirectionalLight(lightEntity);
    light->setColor(color);
    light->setIntensity(intensity);
    light->setWorldDirection(direction);
    lightEntity->addComponent(light);
}
