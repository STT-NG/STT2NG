#pragma once

#include <QWidget>
#include <QTableView>
#include <QGroupBox>
#include <QMap>
#include <QComboBox>
#include <QSplitter>
#include <QResizeEvent>

#include "geometryparameterwidget.h"
#include "parametermodel.h"

class CSVParameterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CSVParameterWidget(ParameterModel *mdl, QStatusBar *statusBar = nullptr, QWidget *parent = nullptr);
    virtual ~CSVParameterWidget() = default;

    ParameterModel *getModel() { return model; }
    QTableView *getView() { return table; }

    const QMap<QString, GeometryParameter *> getActiveParameters() const {return model->getCurrentParameters();}
    ParameterModel::Geometry getActiveGeometry() const { return model->getCurrentGeometry(); }

    void resizeEvent(QResizeEvent *event) override {
        splitter->resize(event->size());
    }

signals:
    void csvLoaded(bool);

public slots:
    void showParameterBox(int index);
    bool loadParameters(const QString &path);
    void saveParameters(const QString &path);

    bool loadCSV(const QString &path);

private:
    double sizeScale;
    double spacing;

    QSplitter *splitter;

    QStatusBar *statusBar;

    ParameterModel *model;
    QTableView *table;
    QMenu *contextMenu = nullptr;

    QGroupBox *parameterGroup;
    QVBoxLayout *parameterGroupLayout;

    QAction *addParameterAction;

    QComboBox *geometryComboBox;

    QMap<ParameterModel::Geometry, QWidget *> parameterBoxes;
    QWidget *currentBox = nullptr;

    QMap<ParameterModel::Geometry, QVector<QString>> _parameterOrder;
    QMap<ParameterModel::Geometry, QMap<QString, GeometryParameterWidget *>> _parameters;

    int selectedColumn = 0;

    QMenu *createContextMenu(ParameterModel::Geometry geometry);

    void showParameterBox(ParameterModel::Geometry type);
};

