#pragma once

#include <QFile>
#include <QJsonDocument>
#include <QStandardItemModel>

class GeometryParameter;

class ParameterModel : public QStandardItemModel {
    Q_OBJECT
public:
    struct ParseResult {
    public:
        enum Result : int {
            Ok,
            JsonError,
            CsvError
        };

        ParseResult()
            : type(Ok)
        {}

        Result type;
        QString error;
        QJsonParseError jsonError;
    };

    enum Geometry : int {
        Cylindrical,
        Cuboidal,
        Spherical
    };
    Q_ENUM(Geometry)

    explicit ParameterModel(QObject *parent = nullptr);
    virtual ~ParameterModel() {};

    const QMap<Geometry, QVector<QString>> getOrderedParameters() {
        return parameterOrder;
    }

    const QMap<QString, GeometryParameter *> getCurrentParameters() {
        return getParametersByGeometry(getCurrentGeometry());
    }

    const QMap<QString, GeometryParameter *> getParametersByGeometry(Geometry geometry) {
        return parameters.value(geometry);
    }

    const QMap<Geometry, QMap<QString, GeometryParameter *>> &getAllParameters() {
        return parameters;
    }

    ParseResult loadDescription(QFile &file);
    void saveDescription(QFile &file);

    void loadCSV(QFile &file);

    bool isPopulated() {return populated;}

    Geometry getCurrentGeometry() {return currentGeometry;}
    void setCurrentGeometry(Geometry geometry) {currentGeometry = geometry;}

signals:
    void populatedChange(bool populated);

private:
    bool populated = false;

    void setPopulated(bool value) {
        populated = value;
        emit populatedChange(populated);
    }

    Geometry currentGeometry = Cylindrical;

    QString currentCSV;

    QMap<Geometry, QVector<QString>> parameterOrder;
    QMap<Geometry, QMap<QString, GeometryParameter *>> parameters;

    void createGeometryParameters();

    void addParameters(ParameterModel::Geometry geometry, QVector<GeometryParameter *> parameters);
    void addParameter(ParameterModel::Geometry geometry, GeometryParameter *parameter);

    GeometryParameter *createParameter(const QString &name, const QVector<QString> &fieldNames = {}, bool isColumnValue = false);
};
