#ifndef PARAMITEMWIDGET_H
#define PARAMITEMWIDGET_H

#include <QWidget>
#include <QListView>
#include <QStyledItemDelegate>
#include <QSortFilterProxyModel>
#include <QLabel>
#include <QStandardItemModel>
#include "types.h"

namespace Ui {
class ComboBox;
}

class ItemModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum DataType
    {
        eChildItemName = Qt::UserRole + 1, //属性名称
        eChildItemId,                      //属性id
    };
    explicit ItemModel(QObject* parent = nullptr);
    virtual ~ItemModel(){}
    void setModelData(const ItemInfo& info);
    void clearModel();
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
protected:
    QVariant data(const QModelIndex &index, int role) const;
    QList<ChildItemInfo> childList;
};

class ItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ItemDelegate(QObject *parent = nullptr);
    virtual ~ItemDelegate(){}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;
};

class ComboBox : public QWidget
{
    Q_OBJECT

public:
    explicit ComboBox(QWidget *parent = nullptr);
    ~ComboBox();
    void initData(const ItemInfo& info);
    void hideList() const;
    void setSelectIndex(qint64 childId);
    void clearParamData();

signals:
    void sigSelectItem(ItemType paramType, const qint64 itemId, const qint64 childId, const QString& name);

private slots:
    void slotPressEdit();
    void slotClickedItem(const QModelIndex &index);
    void on_lineEdit_textChanged(const QString &arg1);
    void slotShowMenu(const QPoint&);

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event);
private:
    //选中下一行
    void selectNext();
    //选中上一行
    void selectPrevious();
    //选择指定行
    void selectItem(const QModelIndex& index);

private:
    Ui::ComboBox *ui;
    QLabel *m_pEmptyLab = nullptr;
    QLabel *m_pShowValue = nullptr;
    QListView *m_pListView = nullptr;
    ItemModel *m_pModel = nullptr;
    ItemDelegate *m_pDelegate = nullptr;
    QSortFilterProxyModel *m_pFilterModel = nullptr;
    ItemInfo m_itemInfo;
};

#endif // PARAMITEMWIDGET_H
