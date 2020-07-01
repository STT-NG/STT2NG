#include "hoverablecheckbox.h"

#include <QPaintEvent>
#include <QDebug>

HoverableCheckBox::HoverableCheckBox(QWidget *parent)
    : QCheckBox(parent)
{

}

bool HoverableCheckBox::event(QEvent *e)
{
    if (e->type() == QEvent::HoverEnter) {
        emit onHover(true);
    } else if (e->type() == QEvent::HoverLeave) {
        emit onHover(false);
    }
    return QCheckBox::event(e);
}
