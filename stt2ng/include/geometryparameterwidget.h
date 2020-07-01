#pragma once

#include "geometryparameter.h"
#include "hoverablecheckbox.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QStatusBar>
#include <QWidget>

class GeometryParameterWidget : public QWidget
{
    Q_OBJECT
public:
    GeometryParameterWidget(GeometryParameter *parameter, QStatusBar *status = nullptr, QWidget *parent = nullptr);
    virtual ~GeometryParameterWidget() {}

    GeometryParameter *getParameter() {return parameter;}

protected:
    QSize minimumSizeHint() const override;

private:
    struct FieldContainer {
        QWidget *container;
        QHBoxLayout *layout;
    };

    QStatusBar *status;

    GeometryParameter *parameter;

    QMap<QString, QDoubleSpinBox *> fields;

    QCheckBox *columnCheck;
    HoverableCheckBox *createColumnCheckBox(QWidget *parent = nullptr);
    FieldContainer createFieldContainer(QWidget *parent = nullptr);

    QDoubleSpinBox *createField(const QString &name, QWidget *parent = nullptr);
};
