#pragma once

#include "parametermodel.h"

#include <QMap>
#include <GRVector>

class GeometryParameter : public QObject
{
    Q_OBJECT
    using GRVector2 = GeomRel::GRVector2;
    using GRVector3 = GeomRel::GRVector3;
public:
    enum Type : int {
        Singular,
        Compound
    };

    GeometryParameter(const QString &name, const QVector<QString> &fieldNames, ParameterModel *context, bool isColumnValue = false);

    double getFieldValue(const QString &field = nullptr) const;
    void setFieldValue(double value, const QString &field = nullptr);

    QVector<QString> getFields() const {return fields.keys().toVector();}

    int fieldCount() const {return fields.size();}
    QString name() const {return parameterName;}

    bool isSingular() const { return type == Singular; }
    bool isColumnValue() const { return columnar; }
    void setIsColumnValue(bool value);

    int toInt(int row);
    double toDouble(int row);
    GRVector3 toGRVector3(int row);
    GRVector2 toGRVector2(int row);

signals:
    void valueChanged(QString field, double value);
    void columnValueChanged(bool isColumn);

private:
    ParameterModel *context;

    QString parameterName;    
    Type type = Compound;
    bool columnar = false;

    QMap<QString, double> fields;
};

