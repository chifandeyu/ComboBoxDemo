#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ItemInfo info;
    info.itemName = "ABC";
    info.itemId = 123;
    info.childList.append(ChildItemInfo{1231, "hello"});
    info.childList.append(ChildItemInfo{1232, "I"});
    info.childList.append(ChildItemInfo{1233, "love"});
    info.childList.append(ChildItemInfo{1234, "go"});
    info.childList.append(ChildItemInfo{1235, "gogo"});
    info.childList.append(ChildItemInfo{1236, "gogogo"});
    ui->widget->initData(info);

    info.itemName = "DemoOk";
    info.itemId = 456;
    info.childList.append(ChildItemInfo{4561, "hello"});
    info.childList.append(ChildItemInfo{4562, "I"});
    info.childList.append(ChildItemInfo{4563, "love"});
    info.childList.append(ChildItemInfo{4564, "go"});
    info.childList.append(ChildItemInfo{4565, "gogo"});
    info.childList.append(ChildItemInfo{4566, "gogogo"});
    ui->widget_2->initData(info);
}

Widget::~Widget()
{
    delete ui;
}

