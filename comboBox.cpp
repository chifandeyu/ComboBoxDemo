#include "combobox.h"
#include "ui_comboBox.h"
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QGridLayout>
#include <QMenu>
#include <QApplication>
#include <QClipboard>

ComboBox::ComboBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ComboBox)
{
    ui->setupUi(this);
    ui->labelName->setFixedSize(70, 27);
    this->setStyleSheet("");

    m_pShowValue = new QLabel;
    m_pShowValue->setObjectName("m_pShowValue");
    QHBoxLayout *pEditLyout = new QHBoxLayout(ui->lineEdit);
    pEditLyout->setContentsMargins(4, 1, 2, 1);
    pEditLyout->setSpacing(0);
    pEditLyout->addWidget(m_pShowValue);

    m_pListView = new QListView(this);
    m_pListView->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    qApp->installEventFilter(this);
    m_pModel = new ItemModel(this);
    m_pDelegate = new ItemDelegate(this);
    m_pFilterModel = new QSortFilterProxyModel(this);
    m_pFilterModel->setFilterRole(ItemModel::eChildItemName);
    m_pFilterModel->setSourceModel(m_pModel);
    m_pListView->setModel(m_pFilterModel);
    m_pListView->setItemDelegate(m_pDelegate);
    m_pEmptyLab = new QLabel;
    m_pEmptyLab->setObjectName("m_pEmptyLab");
    m_pEmptyLab->setAlignment(Qt::AlignCenter);
    QGridLayout *emptyLyout = new QGridLayout(m_pListView);
    emptyLyout->setMargin(0);
    emptyLyout->setSpacing(0);
    emptyLyout->addWidget(m_pEmptyLab);
    m_pEmptyLab->setVisible(false);
    m_pListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pListView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_pListView, &QWidget::customContextMenuRequested, this, &ComboBox::slotShowMenu);
    connect(m_pListView, &QListView::clicked, this, &ComboBox::slotClickedItem);
    connect(ui->lineEdit, &PressLineEdit::sigPressed, this, &ComboBox::slotPressEdit);

    m_pEmptyLab->setText("No data!!!");
}

ComboBox::~ComboBox()
{
    delete ui;
}

void ComboBox::initData(const ItemInfo &info)
{
    m_itemInfo = info;
    m_pModel->setModelData(info);
    ui->labelName->setText(info.itemName);
    for (const ChildItemInfo& childInfo : info.childList) {
        if(childInfo.childId == info.defaultChildItemId){
            ItemData* pData = new ItemData(childInfo.childId, childInfo.childName);
            m_pShowValue->setUserData(Qt::UserRole, pData);
            m_pShowValue->setText(childInfo.childName);
            setSelectIndex(info.defaultChildItemId);
            break;
        }
    }
}

void ComboBox::hideList() const
{
    m_pListView->hide();
    ui->lineEdit->clear();
    m_pShowValue->setVisible(true);
}

void ComboBox::setSelectIndex(qint64 childId)
{
    for(int row =0; row < m_pModel->rowCount(); row++){
        QModelIndex index = m_pModel->index(row, 0);
        bool ok = false;
        const qint64 id = index.data(ItemModel::eChildItemId).toLongLong(&ok);
        if(ok &&  id == childId ){
            m_pListView->setCurrentIndex(index);
            break;
        }
    }
}

void ComboBox::clearParamData()
{
    m_pModel->clearModel();
    ui->lineEdit->clear();
    m_pShowValue->clear();
}

void ComboBox::slotPressEdit()
{
    if (m_pListView->isVisible() && ui->lineEdit->text().isEmpty()) {
        hideList();
    }
    else
    {
        m_pEmptyLab->setVisible(m_pModel->rowCount() == 0);
        m_pShowValue->hide();
        QPoint editPos = this->mapToGlobal(ui->lineEdit->pos());
        editPos.setY(editPos.y() + ui->lineEdit->height() + 1);
        m_pListView->move(editPos);
        m_pListView->resize(ui->lineEdit->width(), 168);
        m_pListView->show();
        m_pListView->raise();
        m_pListView->setFocus();
    }
}

void ComboBox::slotClickedItem(const QModelIndex &index)
{
    selectItem(index);
    hideList();
}

ItemModel::ItemModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

void ItemModel::setModelData(const ItemInfo& info)
{
    beginResetModel();
    childList = info.childList;
    endResetModel();
}

void ItemModel::clearModel()
{
    beginResetModel();
    childList.clear();
    endResetModel();
}

int ItemModel::rowCount(const QModelIndex &parent) const
{
    return childList.size();
}

QVariant ItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    int iRow = index.row();
    if (iRow >= this->childList.size())
    {
        return QVariant();
    }
    ChildItemInfo childInfo = this->childList.at(iRow);
    switch (role) {
    case DataType::eChildItemId:
        return childInfo.childId;
    case DataType::eChildItemName:
        return childInfo.childName;
    default:
        return QVariant();
        break;
    }
}

ItemDelegate::ItemDelegate(QObject *parent)
{

}

void ItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rc = option.rect;
    const QString& showText = index.data(ItemModel::eChildItemName).value<QString>();
    //background
    bool selected = option.state & QStyle::State_Selected;
    bool mouse_hover = option.state & QStyle::State_MouseOver;
    QRect rect = option.rect;
    QPixmap backPic(rect.width(), rect.height());
    if (selected)
    {
        backPic.fill(QColor("#cde8ff"));
    }
    else if (mouse_hover)
    {
        backPic.fill(QColor("#e5f3ff"));
    }
    else
    {
        backPic.fill(QColor(255, 255, 255));
    }
    painter->drawPixmap(rect, backPic);
    //text
    QTextOption textOp(Qt::AlignLeft|Qt::AlignVCenter);
    rc.setX(rc.x() + 4);
    painter->drawText(rc, showText, textOp);
    //qInfo() << "rc = "<<rc;
}

QSize ItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    return QSize(option.rect.width(), 30);
}

void ComboBox::on_lineEdit_textChanged(const QString &text)
{
    QRegExp::PatternSyntax Psyntax = QRegExp::PatternSyntax(QRegExp::Wildcard);
    QRegExp syntax = QRegExp(text.trimmed(), Qt::CaseInsensitive, Psyntax);
    m_pFilterModel->setFilterRegExp(syntax);
    m_pEmptyLab->setVisible(m_pFilterModel->rowCount() <= 0);
    bool textIsEmpty = text.isEmpty();
    if (!m_pListView->isVisible() && !text.isEmpty()) {
        m_pListView->setVisible(true);
        m_pShowValue->hide();
    }
}

void ComboBox::slotShowMenu(const QPoint& mPos)
{
    QModelIndex index = m_pListView->indexAt(mPos);
    if (index.isValid()) {
        QMenu menu;
        QString strName = index.data(ItemModel::eChildItemName).value<QString>();
        QAction* copyM = menu.addAction(QStringLiteral("复制"), [strName]() {
            QClipboard* cb = QApplication::clipboard();
            cb->setText(strName.trimmed());
        });
        copyM->setEnabled(true);
        menu.exec(m_pListView->mapToGlobal(mPos));
    }
}

bool ComboBox::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress && m_pListView->isVisible())
    {
        QMouseEvent* pressEv = (QMouseEvent*)(event);
        //qDebug() << "objectName="<<watched->objectName() << "event="<<event->type();
        const QPoint mousePos = pressEv->globalPos();
        const QPoint editLeft = mapToGlobal(ui->lineEdit->pos());
        const QRect editRc = QRect(editLeft, ui->lineEdit->size());
        const QRect listviewRc = QRect(m_pListView->pos(), QSize(m_pListView->size()));
        if(!editRc.contains(mousePos) && !listviewRc.contains(mousePos)){
            hideList();
            //qInfo() << "mousePos = "<<mousePos;
        }
    }
    else if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = (QKeyEvent*)(event);
        if (m_pListView->isVisible() && (keyEvent->key() == Qt::Key_Escape || keyEvent->key() == Qt::Key_Tab)) {
            hideList();
        }
        else if (watched == ui->lineEdit) {
            if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
                if(m_pListView->isVisible()){
                    //回车选择了一个属性隐藏列表
                    QModelIndex index = m_pListView->currentIndex();
                    if (index.isValid()) {
                        selectItem(index);
                        hideList();
                        return true;
                    }
                }
                //其他情况的回车和鼠标点击一样的效果
                slotPressEdit();
                return true;
            }
            else if (keyEvent->key() == Qt::Key_Down) {
                selectNext();
                return true;
            }
            else if (keyEvent->key() == Qt::Key_Up) {
                selectPrevious();
                return true;
            }
        }
    }

    return QWidget::eventFilter(watched, event);
}

void ComboBox::selectNext()
{
    QModelIndex index = m_pListView->currentIndex();
    if (index.isValid())
    {
        int indexRow = index.row();
        indexRow++;
        if (indexRow < 0)
            return;
        else if (indexRow == m_pFilterModel->rowCount()) {
            index = m_pFilterModel->index(0, 0);
        }
        else {
            index = m_pFilterModel->index(indexRow, 0);
        }
    }
    else
    {
        index = m_pFilterModel->index(0, 0);
    }

    m_pListView->setCurrentIndex(index);
}

void ComboBox::selectPrevious()
{
    QModelIndex index = m_pListView->currentIndex();
    if (index.isValid())
    {
        int indexRow = index.row();
        indexRow--;
        if (indexRow < 0) {
            index = m_pFilterModel->index(m_pFilterModel->rowCount() - 1, 0);
        }
        else {
            index = m_pFilterModel->index(indexRow, 0);
        }
    }
    else
    {
        index = m_pFilterModel->index(0, 0);
    }
    m_pListView->setCurrentIndex(index);
}

void ComboBox::selectItem(const QModelIndex& index)
{
    const QString& childName = index.data(ItemModel::eChildItemName).value<QString>();
    const qint64 childId = index.data(ItemModel::eChildItemId).toLongLong();
    //qInfo() << "childId="<<childId << "childName="<<childName;
    ItemData* pData = new ItemData(childId, childName);
    m_pShowValue->setUserData(Qt::UserRole, pData);
    m_pShowValue->setText(childName);
    emit sigSelectItem(m_itemInfo.itemType, m_itemInfo.itemId, pData->getId(), pData->getName());
    setSelectIndex(childId);
}
