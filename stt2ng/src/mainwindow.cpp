#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "detectioneventreader.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QLineEdit>
#include <QtSvg/QSvgGenerator>
#include <QDebug>

#include <GRVector>
#include <GRCylinder>
#include <GRNode>

using namespace GeomRel;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->setTabEnabled(ui->tabWidget->indexOf(ui->graphTab), false);
    ui->tabWidget->setTabEnabled(ui->tabWidget->indexOf(ui->twoDTab), false);
    ui->tabWidget->setTabEnabled(ui->tabWidget->indexOf(ui->threeDTab), false);
    ui->actionSave_Parameters->setEnabled(false);
    ui->menuGraph->setEnabled(false);

    setMouseTracking(true);

    progressBar = new QProgressBar(this);
    progressBar->setTextVisible(false);
    progressBar->setMaximumSize(80, 10);
    ui->statusbar->addPermanentWidget(progressBar);

    setupTableWidget();
    setupGraphWidget();
    setupChartWidget();
    if constexpr (Config::enable_3d) {
        setup3DView();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_buildButton_pressed()
{
    graphWidget->hide();
    ui->statusbar->showMessage("Building relation...");
    ui->buildButton->setEnabled(false);
    repaint();
    auto builder = graphWidget->getBuilder();

    builder->clearEdges();

    int value = 0;
    if (progressBar) progressBar->setMaximum(builder->getNodeCount() + order);
    builder->build(order, tolerance, [& value, this] {
        if (progressBar) progressBar->setValue(++value);
    });
}

void MainWindow::on_clearButton_pressed()
{
    QMessageBox msgBox;
    msgBox.setText("This will remove all edges in the current neighbourhood relation.");
    msgBox.setInformativeText("Are you sure?");
    msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();

    if (ret != QMessageBox::Yes) return;

    graphWidget->getBuilder()->clearEdges();
    ui->buildButton->setEnabled(true);
}

void MainWindow::setupTableWidget()
{
    csvWidget = new CSVParameterWidget(new ParameterModel(this), ui->statusbar, this);
    ui->tableTabLayout->addWidget(csvWidget);

    connect(csvWidget, &CSVParameterWidget::csvLoaded, ui->generateNodesButton, &QPushButton::setEnabled);
    connect(csvWidget, &CSVParameterWidget::csvLoaded, ui->generate2DButton, &QPushButton::setEnabled);
    connect(csvWidget, &CSVParameterWidget::csvLoaded, ui->actionSave_Parameters, &QAction::setEnabled);
    if constexpr (Config::enable_3d) {
        connect(csvWidget, &CSVParameterWidget::csvLoaded, ui->generate3DModelButton, &QPushButton::setEnabled);
    }
}

void MainWindow::setupGraphWidget()
{
    graphWidget = new GraphWidget(this);
    auto builder = graphWidget->getBuilder();
    connect(builder, &GraphBuilder::buildCompleted, this, [this] {
        graphWidget->show();
        ui->statusbar->showMessage("Done", 2000);
        progressBar->reset();
        repaint();
    });

    ui->graphTabLayout->addWidget(graphWidget, Qt::AlignCenter);
}

void MainWindow::setupChartWidget()
{
    chartWidget = new ChartWidget(this);

    QVBoxLayout *layout = new QVBoxLayout(ui->twoDTab);
    layout->addWidget(chartWidget);
}

#ifdef ENABLE_3D
void MainWindow::setup3DView()
{
    ThreeDView *view = new ThreeDView(this);

    // Scene entrypoint
    scene = new ThreeDScene(view->getRoot());

    QWidget *widget = view->createWidget();

    ui->threeDTabLayout->insertWidget(0, widget, 10);
}
#endif

std::vector<std::unique_ptr<GRNode>> MainWindow::generateNodes()
{
    auto parameters = csvWidget->getActiveParameters();

    if (parameters.empty()) return {};

    auto type = csvWidget->getActiveGeometry();
    auto model = csvWidget->getModel();

    std::vector<std::unique_ptr<GRNode>> nodes;

    switch (type){
    case ParameterModel::Geometry::Cylindrical: {
        for (int i = 1; i < model->rowCount(); ++i) {

            int id = parameters.value("ID")->toInt(i);
            GRVector3 center = parameters.value("Position")->toGRVector3(i);
            GRVector3 direction = parameters.value("Direction")->toGRVector3(i);
            double length = parameters.value("Length")->toDouble(i);
            double radius = parameters.value("Radius")->toDouble(i);

            nodes.push_back(std::make_unique<GRCylinder>(id, center, direction, 2 * length, radius));
        }
        break;
    }
    case ParameterModel::Geometry::Cuboidal:
        break;
    case ParameterModel::Geometry::Spherical:
        break;
    }

    return std::move(nodes);
}

void MainWindow::on_generateNodesButton_pressed()
{
    QMessageBox msgBox;
    msgBox.setText("This will remove all nodes and edges in the current graph, as well as any associated detection events.");
    msgBox.setInformativeText("Are you sure?");
    msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();

    if (ret != QMessageBox::Yes) return;

    auto nodes = generateNodes();

    auto nodeScale = ui->nodeScaleSpinBox->value();
    auto spacing = ui->nodeSpacingSpinBox->value();

    for (auto &node : nodes) {
        node->setNodeScale(nodeScale);
        node->setSpacing(spacing);
    }

    auto builder = graphWidget->getBuilder();

    builder->addNodes(std::move(nodes));

    graphWidget->clearEvents();

    ui->tabWidget->setTabEnabled(ui->tabWidget->indexOf(ui->graphTab), true);
    ui->menuGraph->setEnabled(true);
    ui->buildButton->setEnabled(true);
}

void MainWindow::on_generate2DButton_pressed()
{
    auto nodes = generateNodes();

    chartWidget->createXY(nodes);
    chartWidget->createZX(nodes);

    ui->tabWidget->setTabEnabled(ui->tabWidget->indexOf(ui->twoDTab), true);
}

#ifdef ENABLE_3D
void MainWindow::on_generate3DModelButton_pressed()
{
    auto nodes = generateNodes();

    for (auto &node : nodes) {
        GRCylinder *tmp = dynamic_cast<GRCylinder *>(node.get());
        if (tmp != nullptr) {
            std::unique_ptr<GRCylinder> cylinder;
            node.release();
            cylinder.reset(tmp);
            scene->addCylinder(cylinder);
        }
    }

    ui->tabWidget->setTabEnabled(ui->tabWidget->indexOf(ui->threeDTab), true);
}
#endif

void MainWindow::on_actionSave_Parameters_triggered()
{
    auto fileName = QFileDialog::getSaveFileName(this, tr("Save JSON Parameters"), documentsPath, tr("JSON File (*.json)"));

    if (fileName.isEmpty()){
        return;
    }

    csvWidget->saveParameters(fileName);
}

void MainWindow::on_actionLoad_Parameters_triggered()
{
    auto fileName = QFileDialog::getOpenFileName(this, tr("Load JSON Parameters"), documentsPath, tr("JSON File (*.json)"));

    if (fileName.isEmpty()){
        return;
    }

    csvWidget->loadParameters(fileName);
}

void MainWindow::on_actionImport_CSV_triggered()
{
    auto fileName = QFileDialog::getOpenFileName(this, tr("Import CSV file"), documentsPath, tr("CSV File (*.csv)"));

    if (fileName.isEmpty()){
        return;
    }

    csvWidget->loadCSV(fileName);
}

void MainWindow::on_actionExport_CSV_triggered()
{
    auto fileName = QFileDialog::getSaveFileName(this, tr("Export Relation"), lastExportPath, tr("CSV File (*.csv)"));

    if (fileName.isEmpty()){
        return;
    }

    QFileInfo info(fileName);
    QDir dir = info.dir();
    lastExportPath = dir.path();

    GraphWriter writer(graphWidget->getModel()->getNodes());

    std::string error;
    if (!writer.writeCSV(fileName.toStdString(), &error)) {

    }
}

#ifdef ENABLE_3D
void MainWindow::on_setDistanceScaleButton_pressed()
{
    double value = ui->distanceScaleSpinBox->value();

    scene->setDistanceScale(value);
}
#endif

void MainWindow::on_actionSave_screenshot_triggered()
{
    auto fileName = QFileDialog::getSaveFileName(this, tr("Save screenshot"), lastExportPath, tr("PNG File (*.png);;JPEG File (*.jpg);;SVG File (*.svg)"));

    if (fileName.isEmpty()){
        return;
    }

    auto scenes = graphWidget->getScenes();
    int num_scenes = scenes.size();
    int totalWidth = 0;
    int maxHeight = 0;
    for (int i = 0; i < num_scenes; i++) {
        auto scene = graphWidget->getScene(i);
        totalWidth += scene->width();
        maxHeight = std::max<int>(maxHeight, scene->height());
    }

    int margin = 50;

    if (fileName.endsWith(".svg")) {
        QSvgGenerator svg;

        svg.setFileName(fileName);
        svg.setSize(QSize(totalWidth, maxHeight));
        svg.setTitle(tr("SVG Generated Graph"));
        svg.setDescription(tr("Graph generated by the STT2NG software."));
        svg.setViewBox(QRect(0, 0, totalWidth, maxHeight));

        QPainter painter;
        painter.begin(&svg);
        for (int i = 0; i < num_scenes; i++) {
            auto scene = graphWidget->getScene(i);
            scene->render(&painter);

            painter.translate(scene->width() + margin, 0);
        }
        painter.end();
    } else {
        QImage img(QSize(totalWidth, maxHeight), QImage::Format_RGB32);

        img.fill(Qt::white);

        QPainter painter;
        painter.begin(&img);
        for (int i = 0; i < num_scenes; i++) {
            auto scene = graphWidget->getScene(i);
            scene->render(&painter);

            painter.translate(scene->width() + margin, 0);
        }
        painter.end();

        QPixmap screenshot;
        screenshot.convertFromImage(img);

        screenshot.save(fileName);
    }
}

void MainWindow::on_actionImport_Event_triggered()
{
    auto fileName = QFileDialog::getOpenFileName(this, tr("Import a description of one or more detection events"), documentsPath, tr("JSON File (*.json)"));

    if (fileName.isEmpty()) {
        return;
    }

    DetectionEventReader reader(fileName);

    bool ok;
    QVector<DetectionEvent> events = reader.parseEvents(&ok);
    if (!ok) {
        return;
    }

    graphWidget->addDetectionEvents(events);
}

void MainWindow::on_toleranceSpinBox_valueChanged(double value)
{
    tolerance = value;
    if(value != previousTolerance){
        ui->buildButton->setEnabled(true);
        previousOrder = value;
    }
}

void MainWindow::on_relationOrderSpinBox_valueChanged(int value)
{
    order = value;
    if (value != previousOrder){
        ui->buildButton->setEnabled(true);
        previousOrder = value;
    }
}
