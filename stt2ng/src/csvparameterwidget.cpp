#include "csvparameterwidget.h"
#include "csvreader.h"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QMenu>
#include <QMetaEnum>
#include <QFile>
#include <QMessageBox>
#include <QToolButton>
#include <QStatusBar>
#include <QSpacerItem>

CSVParameterWidget::CSVParameterWidget(ParameterModel *paramModel, QStatusBar *statusBar, QWidget *parent)
     : QWidget(parent),
       model(paramModel),
       statusBar(statusBar)
{
    connect(model, &ParameterModel::populatedChange, this, &CSVParameterWidget::csvLoaded);

    splitter = new QSplitter(this);

    table = new QTableView(this);
    table->setModel(model);
    table->setMinimumWidth(300);

    table->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(table->horizontalHeader(), &QHeaderView::customContextMenuRequested, this, [this](QPoint pos) {
        selectedColumn = table->horizontalHeader()->logicalIndexAt(pos);

        contextMenu = createContextMenu(model->getCurrentGeometry());
        if(contextMenu) {
            contextMenu->popup(table->horizontalHeader()->viewport()->mapToGlobal(pos));
        }
    });

    splitter->addWidget(table);

    parameterGroup = new QGroupBox("Geometry Description", this);
    parameterGroupLayout = new QVBoxLayout(parameterGroup);
    parameterGroupLayout->setSizeConstraint(QLayout::SetMinimumSize);
    parameterGroup->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);

    geometryComboBox = new QComboBox(parameterGroup);
    connect(geometryComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, QOverload<int>::of(&CSVParameterWidget::showParameterBox));

    parameterGroupLayout->addWidget(geometryComboBox, Qt::AlignRight);

    for (const auto &[type, paramNames] : model->getOrderedParameters().toStdMap()) {
        auto paramMap = model->getParametersByGeometry(type);

        QWidget *box = new QWidget(parameterGroup);
        QVBoxLayout *layout = new QVBoxLayout(box);
        layout->setSizeConstraint(QLayout::SetMinimumSize);
        box->hide();

        geometryComboBox->addItem(QMetaEnum::fromType<ParameterModel::Geometry>().key(type), QVariant::fromValue(type));

        for (const auto name : paramNames) {
            if (!paramMap.contains(name)) continue;

            auto param = paramMap.value(name);
            auto paramWidget = new GeometryParameterWidget(param, statusBar, this);

            layout->addWidget(paramWidget, Qt::AlignTop);

            _parameterOrder[type].append(name);
            _parameters[type][name] = paramWidget;
        }

        layout->addSpacerItem(new QSpacerItem(0, 50, QSizePolicy::Minimum, QSizePolicy::Expanding));

        parameterBoxes.insert(type, box);
        parameterGroupLayout->addWidget(box);
    }

    showParameterBox(model->getCurrentGeometry());

    splitter->addWidget(parameterGroup);

    splitter->setStretchFactor(0, 10);
    splitter->setStretchFactor(1, 1);

    splitter->setCollapsible(0, false);

    auto handle = splitter->handle(1);
    handle->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::MinimumExpanding);
    handle->setMinimumWidth(10);
    handle->setMaximumWidth(10);
    auto handleLayout = new QVBoxLayout;
    handleLayout->setContentsMargins(0, 0, 0, 0);

    auto button = new QToolButton(handle);
    button->setArrowType(Qt::ArrowType::LeftArrow);
    button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(button, &QToolButton::pressed, this, [this] {splitter->setSizes({static_cast<int>(10e5), 1});});
    handleLayout->addWidget(button);

    button = new QToolButton(handle);
    button->setArrowType(Qt::ArrowType::RightArrow);
    button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(button, &QToolButton::pressed, this, [this] {splitter->setSizes({1, 0});});
    handleLayout->addWidget(button);

    handle->setLayout(handleLayout);

    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}

void CSVParameterWidget::showParameterBox(int index)
{
    auto type = geometryComboBox->itemData(index).value<ParameterModel::Geometry>();
    showParameterBox(type);
}

bool CSVParameterWidget::loadParameters(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::information(this, tr("Unable to open file for writing"), file.errorString());
        return false;
    }
    auto result = model->loadDescription(file);

    switch (result.type) {
    case ParameterModel::ParseResult::Ok:
        break;
    case ParameterModel::ParseResult::JsonError:
        QMessageBox::warning(this, tr("Error while parsing JSON description."), result.jsonError.errorString());
        return false;
        break;
    case ParameterModel::ParseResult::CsvError:
        QMessageBox::warning(this, tr("Error while parsing CSV."), result.error);
        return false;
        break;
    }

    return true;
}

void CSVParameterWidget::saveParameters(const QString &path)
{
    if (!model->isPopulated()) {
        QMessageBox::information(this, tr("No CSV file loaded"), tr("Please import a CSV file using Table -> Import CSV...\n"
                                                                    " and set up the appropriate parameters in order to save a JSON configuration."));
        return;
    }

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::information(this, tr("Unable to open file for writing"), file.errorString());
        return;
    }

    model->saveDescription(file);
}

bool CSVParameterWidget::loadCSV(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::information(nullptr, "Unable to open file for reading", file.errorString());
        return false;
    }

    model->loadCSV(file);
    table->resizeColumnsToContents();

    emit csvLoaded(true);

    return true;
}

QMenu *CSVParameterWidget::createContextMenu(ParameterModel::Geometry geometry)
{
    if (!_parameters.contains(geometry)) return nullptr;

    auto order = _parameterOrder.value(geometry);
    QMenu *context = new QMenu(this);

    for (auto paramName : order) {
        auto paramWidget = _parameters.value(geometry).value(paramName);

        if (paramWidget->getParameter()->isColumnValue()) {
            if(paramWidget->getParameter()->isSingular()) {
                auto fields = paramWidget->getParameter()->getFields();
                for (auto &field : fields) {
                    auto action = new QAction("Set " + field, this);
                    connect(action, &QAction::triggered, paramWidget, [=]{
                        paramWidget->getParameter()->setFieldValue(selectedColumn, field);
                    });

                    context->addAction(action);
                }
            } else {
                QMenu *paramMenu = new QMenu("Set " + paramWidget->getParameter()->name(), this);
                auto fields = paramWidget->getParameter()->getFields();
                for (auto &field : fields) {
                    auto action = new QAction(field, this);
                    connect(action, &QAction::triggered, paramWidget, [=]{
                        paramWidget->getParameter()->setFieldValue(selectedColumn, field);
                    });

                    paramMenu->addAction(action);
                }
                context->addMenu(paramMenu);
            }
        }
    }

    return context;
}

void CSVParameterWidget::showParameterBox(ParameterModel::Geometry type)
{
    if (!parameterBoxes.contains(type)) return;

    if (currentBox != nullptr) {
        currentBox->hide();
    }
    currentBox = parameterBoxes.value(type);
    currentBox->show();

    model->setCurrentGeometry(type);

    if (contextMenu) {
        delete contextMenu;
    }
}
