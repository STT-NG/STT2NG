#include "geometryparameterwidget.h"

#include <QLabel>
#include <QPropertyAnimation>

GeometryParameterWidget::GeometryParameterWidget(GeometryParameter *parameter, QStatusBar *status, QWidget *parent)
    : QWidget(parent),
      status(status),
      parameter(parameter)
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    QLabel *label = new QLabel(parameter->name(), this);
    label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    layout->addWidget(label, Qt::AlignLeft | Qt::AlignVCenter);

    auto [fieldBox, fieldLayout] = createFieldContainer(this);

    for (const auto &fieldName : parameter->getFields()) {
        QDoubleSpinBox *field = createField(fieldName, fieldBox);

        if (!parameter->isSingular()){
            QLabel *fieldLabel = new QLabel(fieldName, fieldBox);
            fieldLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

            fieldLayout->addWidget(fieldLabel, Qt::AlignRight);
        }

        fieldLayout->addWidget(field);
        fieldLayout->setSizeConstraint(QLayout::SetMinimumSize);

        fields.insert(fieldName, field);
    }

    connect(parameter, &GeometryParameter::valueChanged, this, [&](QString field, double value) {
        auto box = fields.value(field);
        if (box)
            box->setValue(value);
    });


    columnCheck = createColumnCheckBox(this);
    fieldLayout->addWidget(columnCheck, Qt::AlignRight);

    connect(parameter, &GeometryParameter::columnValueChanged, this, [=](bool isColumnValue) {
        if (isColumnValue){
            columnCheck->setCheckState(Qt::Checked);
        } else {
            columnCheck->setCheckState(Qt::Unchecked);
        }
    });

    layout->addWidget(fieldBox, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetMinimumSize);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setMaximumHeight(50);
}

QSize GeometryParameterWidget::minimumSizeHint() const
{
    if (parameter->isColumnValue()) {
        return {20, 20};
    }
    return {50, 20};
}

HoverableCheckBox *GeometryParameterWidget::createColumnCheckBox(QWidget *parent)
{
    HoverableCheckBox *checkBox = new HoverableCheckBox(this);
    checkBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    checkBox->setChecked(parameter->isColumnValue());
    connect(checkBox, &HoverableCheckBox::stateChanged, this, [this] (int state)
    {
        parameter->setIsColumnValue(state == 2);

        if (parameter->isColumnValue()) {
            for (auto field : fields) {
                QPropertyAnimation *anim = new QPropertyAnimation(field, "minimumWidth", this);
                anim->setDuration(100);
                anim->setEndValue(18);
                anim->start();

                field->setEnabled(false);
                field->setPrefix(" ");
                field->setDecimals(0);
                field->setButtonSymbols(QDoubleSpinBox::NoButtons);
            }
        } else {
            for (auto field : fields) {
                QPropertyAnimation *anim = new QPropertyAnimation(field, "minimumWidth", this);
                anim->setDuration(100);
                anim->setEndValue(50);
                anim->start();

                field->setEnabled(true);
                field->setPrefix("");
                field->setDecimals(2);
                field->setButtonSymbols(QDoubleSpinBox::UpDownArrows);
            }
        }
    });

    if (status) {
        connect(checkBox, &HoverableCheckBox::onHover, this, [this] (bool hovered) {
            if (hovered) {
                status->showMessage("Check this to use column values for this parameter. (Right-click a column header to set)");
            } else {
                status->clearMessage();
            }
        });
    }

    return checkBox;
}

GeometryParameterWidget::FieldContainer GeometryParameterWidget::createFieldContainer(QWidget *parent)
{
    QWidget *fieldBox = new QWidget(parent);
    fieldBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    fieldBox->setMinimumWidth(100);
    fieldBox->setMinimumHeight(50);
    QHBoxLayout *fieldBoxLayout = new QHBoxLayout(fieldBox);
    fieldBoxLayout->setSizeConstraint(QHBoxLayout::SetMinimumSize);

    return {fieldBox, fieldBoxLayout};
}

QDoubleSpinBox *GeometryParameterWidget::createField(const QString &name, QWidget *parent)
{
    QDoubleSpinBox *field = new QDoubleSpinBox(parent);
    field->setObjectName(name);
    field->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    field->setRange(-1000, 1000);
    field->setMinimumWidth(50);
    field->setMaximumHeight(20);
    field->setSingleStep(0.1);

    return field;
}
