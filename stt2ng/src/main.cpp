#include <config.h>

#ifdef ENABLE_GUI
#include "mainwindow.h"

#include <QApplication>
#else

#ifdef ENABLE_STTS
#include "parametermodel.h"
#include "geometryparameter.h"
#include "csvreader.h"
#endif
#include "graphwriter.h"

#include <QCoreApplication>
#endif

#include <GeomRel>
#include <QCommandLineParser>
#include <QDir>
#include <STTUtil>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

using namespace GeomRel;

struct Input {
    int order = 1;
    double tolerance = 1.0;
    QString infile;
    QString outfile;
};

enum ParseResult {
    Ok,
    Error,
    Version,
    Help,
    GUISimple,
    GUIError
};

ParseResult parseArgs(QCommandLineParser &parser, Input *input, QString *errorMsg) {
    const QCommandLineOption helpOption = parser.addHelpOption();
    const QCommandLineOption versionOption = parser.addVersionOption();

    parser.addOptions({
                          {{"o", "relation-order"},
                            QCoreApplication::translate("main", "Set the desired order of the neighbourhood relation to <order>. Default is 1"),
                            QCoreApplication::translate("main", "order")
                          },
                          {{"t", "tolerance"},
                            QCoreApplication::translate("main", "Set the desired tolerance when building the relation. Default is 1"),
                            QCoreApplication::translate("main", "tolerance")
                          },
                      });

    if constexpr (Config::enable_gui){
        parser.addOption({{"g", "gui"},
                          QCoreApplication::translate("main", "Open the gui if incorrect arguments are passed.")
                         });
    }

    if constexpr (Config::enable_stts){
        parser.addPositionalArgument("description", "The geometry description to convert. Required.");
    } else{
        parser.addPositionalArgument("pandaCSV", "The PANDA CSV data to convert. Required.");
    }

    parser.addPositionalArgument("output", "File to write the neighbourhood relation to. Output will be in CSV format. "\
                                           "If not supplied, the output will be of the form '<description>.csv'");

    bool parsed = parser.parse(QCoreApplication::arguments());
    if(!parsed) {
        *errorMsg = parser.errorText();
        return Error;
    }

#ifdef Q_OS_WIN
    FreeConsole();
#endif

    if (parser.isSet(versionOption))
        return Version;

    if (parser.isSet(helpOption))
        return Help;

    bool cliMode = false;

    if (parser.isSet("o")) {
        bool ok = false;
        const int order = parser.value("o").toInt(&ok);
        if (!ok) {
            *errorMsg = "Argument to '-o' expects an integer.";

            if constexpr (!Config::enable_gui) {
                return Error;
            } else {
                if (!parser.isSet("g")) {
                    return Error;
                } else {
                    return GUIError;
                }
            }
        }
        input->order = order;
        cliMode = true;
    }

    if (parser.isSet("t")) {
        bool ok = false;
        const int tolerance = parser.value("t").toDouble(&ok);
        if (!ok) {
            *errorMsg = "Argument to '-t' expects a floating-point value.";

            if constexpr (!Config::enable_gui) {
                return Error;
            } else {
                if (!parser.isSet("g")) {
                    return Error;
                } else {
                    return GUIError;
                }
            }
        }
        input->tolerance = tolerance;
        cliMode = true;
    }

    const auto positionals = parser.positionalArguments();
    if (positionals.isEmpty()) {
        if constexpr (Config::enable_stts){
            *errorMsg = "Missing argument 'description'";
        } else {
            *errorMsg = "Missing argument 'pandaCSV'";
        }

        if constexpr (!Config::enable_gui) {
            return Error;
        } else {
            if (!parser.isSet("g") && cliMode) {
                return Error;
            }
            if (parser.isSet("g")) {
                *errorMsg += " Starting GUI.";
                return GUIError;
            }
            return GUISimple;
        }
    }

    input->infile = positionals.first();
    if (positionals.size() == 2) {
        input->outfile = positionals.at(1);
    }

    if (positionals.size() > 2) {
        const int num = positionals.size();
        *errorMsg = QStringLiteral("Expected at most 2 arguments, got %1. Offending argument(s): ").arg(num);
        for (int i = 2; i < num; ++i) {
            *errorMsg += positionals.at(i);
            if (i < num - 1) {
                *errorMsg += ", ";
            }
        }

        if constexpr (!Config::enable_gui) {
            return Error;
        } else {
            if (!parser.isSet("g") && cliMode) {
                return Error;
            }
            if (parser.isSet("g")) {
                *errorMsg += " Starting GUI.";
                return GUIError;
            }
            return GUISimple;
        }
    }

    if constexpr (Config::enable_gui) {
        return cliMode ? Ok : GUISimple;
    } else {
        return Ok;
    }
}

int acceptInput(const Input &input) {

    QDir cwd;
    QString inpath = cwd.relativeFilePath(input.infile);

#ifndef ENABLE_STTS
    std::string error;
    auto [ok, nodes] = STTUtil::PANDA::csvToRelation(inpath.toStdString(), &error, input.order, input.tolerance);

    if (!ok) {
        std::cerr << error << std::endl;
        return -1;
    }

    std::vector<GRNode *> node_ptrs;
    for (auto &node : nodes) {
        node_ptrs.push_back(node.get());
    }

    GraphWriter writer(node_ptrs);

    QString outfile;
    if (input.outfile.isEmpty()) {
        QFileInfo info(inpath);
        outfile = info.dir().path() + "/" + info.baseName() + ".csv";
    } else {
        outfile = input.outfile;
    }

    QString outpath = cwd.relativeFilePath(outfile);

    if(!writer.writeCSV(outpath.toStdString(), &error)) {
        std::cerr << error << std::endl;
        return -1;
    }
#else
    QFile infile(inpath);
    if (!infile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cerr << "Unable to open file for reading." << std::endl;
        return -1;
    }

    ParameterModel paramModel;
    auto result = paramModel.loadDescription(infile);

    auto parameters = paramModel.getCurrentParameters();

    if (parameters.empty()) return -1;

    auto type = paramModel.getCurrentGeometry();

    GRBuilder builder;

    std::vector<std::unique_ptr<GRNode>> nodes;

    if (type ==  ParameterModel::Geometry::Cylindrical) {
        for (int i = 1; i < paramModel.rowCount(); ++i) {

            int id = parameters.value("ID")->toInt(i);
            GRVector3 center = parameters.value("Position")->toGRVector3(i);
            GRVector3 direction = parameters.value("Direction")->toGRVector3(i);
            double length = parameters.value("Length")->toDouble(i);
            double radius = parameters.value("Radius")->toDouble(i);

            nodes.push_back(std::make_unique<GRCylinder>(id, center, direction, 2 * length, radius));
        }
    }

    std::vector<GRNode *> node_ptrs;
    for (auto &node : nodes) {
        node_ptrs.push_back(node.get());
    }

    builder.setNodes(node_ptrs);
    builder.build(input.order, input.tolerance);

    GraphWriter writer(node_ptrs);

    QString outfile;
    if (input.outfile.isEmpty()) {
        QFileInfo info(inpath);
        outfile = info.dir().path() + "/" + info.baseName() + ".csv";
    } else {
        outfile = input.outfile;
    }

    QString outpath = cwd.relativeFilePath(outfile);

    std::string error;
    if(!writer.writeCSV(outpath.toStdString(), &error)) {
        std::cerr << error << std::endl;
        return -1;
    }
#endif

    return 0;
}

int main(int argc, char *argv[])
{
#ifdef ENABLE_GUI
    QApplication  app(argc, argv);
    QApplication::setApplicationName("STT2NG");
    QApplication::setApplicationVersion("1.0");
#else
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("STT2NG");
    QCoreApplication::setApplicationVersion("1.0");
#endif

    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::translate("main", "STT2NG is a combination CLI-GUI application for converting geometry descriptions of Straw Tube Trackers into neighbourhood graphs. "
                                     "GUI mode offers many additional tools for viewing and tweaking the geometry description as well as the graph."));
    Input input;
    QString errorMsg;
    switch (parseArgs(parser, &input, &errorMsg)) {
    case Ok:
        return acceptInput(input);
        break;
    case Error:
        fputs(qPrintable(errorMsg), stderr);
        fputs("\n\n", stderr);
        fputs(qPrintable(parser.helpText()), stderr);
        return 1;
    case Version:
        printf("%s %s\n", qPrintable(QCoreApplication::applicationName()),
                       qPrintable(QCoreApplication::applicationVersion()));
        break;
    case Help:
        parser.showHelp();
        break;
#ifdef ENABLE_GUI
    case GUIError: {
        fputs(qPrintable(errorMsg), stderr);
        fputs("\n\n", stderr);
        fputs(qPrintable(parser.helpText()), stderr);
        fflush(stderr);
        MainWindow w;
        w.showMaximized();

        return app.exec();
    }
    case GUISimple: {
        MainWindow w;
        w.showMaximized();

        return app.exec();
    }
#endif
    default:
        break;
    }

    return 0;
}
