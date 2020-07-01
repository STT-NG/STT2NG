#pragma once

#include <QObject>
#include <QCheckBox>
#include <QPainter>

class HoverableCheckBox : public QCheckBox
{
    Q_OBJECT
public:
    HoverableCheckBox(QWidget *parent = nullptr);

signals:
    void onHover(bool hover);

protected:
    bool event(QEvent *e) override;
};

