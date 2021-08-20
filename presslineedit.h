#ifndef PRESSLINEEDIT_H
#define PRESSLINEEDIT_H

#include <QLineEdit>

class PressLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    PressLineEdit(QWidget *parent = nullptr);
    virtual ~PressLineEdit(){}
protected:
    void mousePressEvent(QMouseEvent *) override;

signals:
    void sigPressed();
};

#endif // PRESSLINEEDIT_H
