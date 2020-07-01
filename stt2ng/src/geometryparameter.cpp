#include "geometryparameter.h"

using GRVector2 = GeomRel::GRVector2;
using GRVector3 = GeomRel::GRVector3;

GeometryParameter::GeometryParameter(const QString &name, const QVector<QString> &fieldNames, ParameterModel *context, bool isColumnValue)
    : context(context),
      parameterName(name),
      columnar(isColumnValue)
{
    if (fieldNames.empty()) {
        fields.insert(name, 0.0);
        type = Singular;
    } else {
        for (auto &fieldName : fieldNames) {
            fields.insert(fieldName, 0.0);
        }
        type = Compound;
    }
}

double GeometryParameter::getFieldValue(const QString &field) const
{
    if (isSingular()) {
        if (!fields.empty()) {
            if (isColumnValue()) {
                return fields.first() - 1;
            }
            return fields.first();
        }
    } else {
        if (fields.contains(field)) {
            if (isColumnValue()) {
                return fields.value(field) - 1;
            }
            return fields.value(field);
        }
    }

    return 0;
}

void GeometryParameter::setFieldValue(double value, const QString &field)
{
    if (isSingular()) {
        if (!fields.empty()) {
            if (isColumnValue()) {
                fields.first() = value + 1;
            } else {
                fields.first() = value;
            }
        }

        emit valueChanged(parameterName, getFieldValue(field));
    } else {
        if (fields.contains(field)) {
            if (isColumnValue()) {
                fields[field] = value + 1;
            } else {
                fields[field] = value;
            }
        }

        emit valueChanged(field, getFieldValue(field));
    }
}

void GeometryParameter::setIsColumnValue(bool value)
{
    columnar = value;
    emit columnValueChanged(columnar);
}

int GeometryParameter::toInt(int row)
{
    auto val = getFieldValue();

    if (isColumnValue()) {
        return context->item(row, val)->data().toInt();
    }
    return val;
}

double GeometryParameter::toDouble(int row)
{
    auto val = getFieldValue();

    if (isColumnValue()) {
        return context->item(row, val)->data().toDouble();
    }
    return val;
}

GRVector3 GeometryParameter::toGRVector3(int row)
{
    auto x = getFieldValue("X");
    auto y = getFieldValue("Y");
    auto z = getFieldValue("Z");

    if (isColumnValue()){
        return {context->item(row, x)->data().toDouble(),
                context->item(row, y)->data().toDouble(),
                context->item(row, z)->data().toDouble()};
    }

    return {x, y, z};
}

GRVector2 GeometryParameter::toGRVector2(int row)
{
    auto x = getFieldValue("X");
    auto y = getFieldValue("Y");

    if (isColumnValue()){
        return {context->item(row, x)->data().toDouble(),
                context->item(row, y)->data().toDouble()};
    }

    return {x, y};
}
