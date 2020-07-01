#pragma once

#include <QFile>
#include <QStandardItemModel>
#include <QString>
#include <QTextStream>

class CSVReader
{
public:
    static bool populateModel(QFile &file, QStandardItemModel *model) {
        QTextStream stream(&file);
        int row = 0;
        int cols = 0;
        while (!stream.atEnd()) {
            model->setRowCount(row);
            auto line = stream.readLine();
            auto values = line.split(",");
            const int lineCols = values.size();
            cols = std::max(lineCols, cols);
            model->setColumnCount(cols);
            for (int col = 0; col < lineCols; ++col) {
                if(!model->item(row, col)) {
                    auto item = new QStandardItem(values.at(col));
                    item->setData(values.at(col));
                    model->setItem(row, col, item);
                }
            }
            ++row;
        }

        file.close();

        return true;
    }
};

