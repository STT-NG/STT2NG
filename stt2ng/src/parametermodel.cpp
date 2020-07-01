#include "parametermodel.h"

#include <QDir>
#include <QFileInfo>
#include <QJsonObject>
#include <QMetaEnum>

#include "csvreader.h"
#include "geometryparameter.h"


ParameterModel::ParameterModel(QObject *parent)
    : QStandardItemModel(parent)
{
    createGeometryParameters();
}

ParameterModel::ParseResult ParameterModel::loadDescription(QFile &file)
{
    ParameterModel::ParseResult result;

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);

    if (doc.isNull()){
        result.type = ParameterModel::ParseResult::JsonError;
        result.jsonError = parseError;

        return result;
    }

    auto root = doc.object();
    if (root.contains("CSV")) {
        auto relativeCsvPath = root.value("CSV").toString();
        QFileInfo info(file);
        QDir dir = info.dir();
        auto csvPath = dir.absoluteFilePath(relativeCsvPath);
        QFile csvFile(csvPath);

        if (!csvFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            result.type = ParameterModel::ParseResult::CsvError;
            result.error = "The parameter configuration could not be applied, perhaps the corresponding CSV file was moved?\n" + csvFile.errorString();
            return result;
        }

        loadCSV(csvFile);
    }
    file.close();

    if(root.contains("Geometry")) {
        bool ok;
        auto geometry = QMetaEnum::fromType<Geometry>().keyToValue(root.value("Geometry").toString().toStdString().data(), &ok);
        if (ok) {
            currentGeometry = static_cast<Geometry>(geometry);
        }
    }

    if (root.contains("Parameters")){
        QJsonObject jsonParameters = root.value("Parameters").toObject();

        for (auto parameter : getCurrentParameters()) {
            auto name = parameter->name();
            if (jsonParameters.contains(name)) {
                QJsonObject jsonParam = jsonParameters.value(name).toObject();

                if (jsonParam.contains("UseColumn")) {
                    bool isColumnValue = jsonParam.value("UseColumn").toBool();
                    parameter->setIsColumnValue(isColumnValue);
                }

                if (parameter->isSingular()) {
                    if (jsonParam.contains("Value")) {
                        double value = jsonParam.value("Value").toDouble();
                        parameter->setFieldValue(value);
                    }
                } else {
                    for (auto field : parameter->getFields()) {
                        if (jsonParam.contains(field)) {
                            double value = jsonParam.value(field).toDouble();
                            parameter->setFieldValue(value, field);
                        }
                    }
                }
            }
        }
    }

    return result;
}

void ParameterModel::saveDescription(QFile &file)
{
    QFileInfo finfo(file);
    QDir dir = finfo.dir();

    QJsonObject root;

    root["CSV"] = dir.relativeFilePath(currentCSV);
    root["Geometry"] = QMetaEnum::fromType<Geometry>().key(currentGeometry);

    QJsonValueRef geometryObjectRef = root["Parameters"];
    auto geometryObject = geometryObjectRef.toObject();

    for (auto paramName : parameterOrder.value(currentGeometry)) {
        auto param = parameters.value(currentGeometry).value(paramName);

        QJsonValueRef paramObjectRef = geometryObject[param->name()];
        auto paramObject = paramObjectRef.toObject();

        paramObject["UseColumn"] = param->isColumnValue();
        if (param->isSingular()) {
            paramObject["Value"] = param->getFieldValue();
        } else {
            for (auto field : param->getFields()) {
                auto fieldName = field;
                if (param->isColumnValue()) {
                    paramObject[fieldName] = (int)param->getFieldValue(field);
                } else {
                    paramObject[fieldName] = param->getFieldValue(field);
                }
            }
        }
        paramObjectRef = paramObject;
    }
    geometryObjectRef = geometryObject;

    QJsonDocument doc(root);

    file.write(doc.toJson());

    file.close();
}

void ParameterModel::loadCSV(QFile &file)
{
    if (populated) {
        clear();
    }

    CSVReader::populateModel(file, this);
    setPopulated(true);

    currentCSV = QFileInfo(file).path();
}

void ParameterModel::addParameters(ParameterModel::Geometry geometry, QVector<GeometryParameter *> parameters)
{
    for (auto &parameter : parameters) {
        addParameter(geometry, parameter);
    }
}

void ParameterModel::addParameter(ParameterModel::Geometry geometry, GeometryParameter *parameter)
{
    auto name = parameter->name();
    parameterOrder[geometry].append(name);
    parameters[geometry][name] = parameter;
}

void ParameterModel::createGeometryParameters()
{
    addParameters(ParameterModel::Cylindrical, {
                              createParameter("ID"),
                              createParameter("Position", {"X", "Y", "Z"}),
                              createParameter("Direction", {"X", "Y", "Z"}),
                              createParameter("Radius"),
                              createParameter("Length")
                          });

    addParameters(ParameterModel::Cuboidal, {
                              createParameter("ID"),
                              createParameter("Position", {"X", "Y", "Z"}),
                              createParameter("Direction", {"X", "Y", "Z"}),
                              createParameter("Extent", {"X", "Y", "Z"})
                          });

    addParameters(ParameterModel::Spherical, {
                              createParameter("ID"),
                              createParameter("Position", {"X", "Y", "Z"}),
                              createParameter("Radius")
                          });
}

GeometryParameter *ParameterModel::createParameter(const QString &name, const QVector<QString> &fieldNames, bool isColumnValue)
{
    auto parameter = new GeometryParameter(name, fieldNames, this, isColumnValue);
    return parameter;
}
