#pragma once

#include <config.h>

#include <QMainWindow>
#include <QLabel>
#include <QStandardPaths>
#include <QStandardItemModel>
#include <QProgressBar>
#include <GRNode>

#include "chartwidget.h"
#include "csvparameterwidget.h"
#include "graphwriter.h"
#include "graphwidget.h"

#ifdef ENABLE_3D
#include "threedscene.h"
#include "threedview.h"
#endif

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    using GRNode = GeomRel::GRNode;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_buildButton_pressed();

    void on_clearButton_pressed();

    void on_generateNodesButton_pressed();

    void on_generate2DButton_pressed();

    void on_actionSave_Parameters_triggered();

    void on_actionLoad_Parameters_triggered();

    void on_actionImport_CSV_triggered();

    void on_actionExport_CSV_triggered();

    void on_actionSave_screenshot_triggered();

#ifdef ENABLE_3D
    void on_generate3DModelButton_pressed();

    void on_setDistanceScaleButton_pressed();
#endif

    void on_actionImport_Event_triggered();

    void on_toleranceSpinBox_valueChanged(double value);

    void on_relationOrderSpinBox_valueChanged(int value);

private:
    Ui::MainWindow *ui;

    CSVParameterWidget *csvWidget;
    GraphWidget *graphWidget;
    ChartWidget *chartWidget;

    int previousOrder = -1;
    int order = 1;
    double previousTolerance = -1;
    double tolerance = 0;

    const QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString lastSavePath = documentsPath;
    QString lastExportPath = documentsPath;

    QProgressBar *progressBar;

    void setupTableWidget();
    void setupGraphWidget();
    void setupChartWidget();

    std::vector<std::unique_ptr<GeomRel::GRNode>> generateNodes();

#ifdef ENABLE_3D
    ThreeDScene *scene;
    void setup3DView();
#endif
};
