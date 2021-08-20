#include "presslineedit.h"
#include <QDebug>
#include <QMouseEvent>

PressLineEdit::PressLineEdit(QWidget *parent) : QLineEdit(parent)
{
}

void PressLineEdit::mousePressEvent(QMouseEvent *event)
{
    emit sigPressed();
    QLineEdit::mousePressEvent(event);
}
