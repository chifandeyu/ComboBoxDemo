#ifndef __TYPES_H__
#define __TYPES_H__
#include <QString>
#include <QList>
#include <QObjectUserData>
#include <QLabel>

//映射的参数类型
enum ItemType
{
    UNKNOW_TYPE = 0,
    DEVICE_PARAM_TYPE           //设备的参数属性
};

struct ChildItemInfo {
    qint64 childId = 0;
    QString childName;
};
//属性参数
typedef struct tagItemInfo
{
    qint64 itemId = 0;
    QString itemName;
    qint64 defaultChildItemId = 0;
    QList<ChildItemInfo> childList;
    ItemType itemType = ItemType::UNKNOW_TYPE;
}ItemInfo;

class ItemData : public QObjectUserData
{
public:
    ItemData(const qint64& id, const QString& name = QString())
        : m_id(id)
        , m_name(name)
    {
    }
    qint64 getId() const {
        return m_id;
    }
    QString getName() const {
        return m_name;
    }
private:
    qint64 m_id;
    QString m_name;
};

#endif //__TYPES_H__
