#include "TreeItem.h"
#include "TreeModel.h"

#include <iostream>
#include <QJsonArray>
//#include <QDebug>

namespace alo {

TreeModel::TreeModel(const QJsonObject &content, QObject *parent)
    : QAbstractItemModel(parent)
{
    m_rootItem = new TreeItem(QJsonObject());
    setupModelData(content, m_rootItem);
}
//! [0]

//! [1]
TreeModel::~TreeModel()
{
    delete m_rootItem;
}
//! [1]

//! [2]
int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return m_rootItem->columnCount();
}
//! [2]

//! [3]
QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int col = index.column();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    if (role == Qt::DecorationRole) {
        if(col == 0)
            return item->data(index.column());
    }

    if (role == Qt::UserRole) {
        return item->data(2);
    }

    if (role != Qt::DisplayRole)
        return QVariant();

    return item->data(index.column());
}
//! [3]

//! [4]
Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}
//! [4]

//! [5]
QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return m_rootItem->data(section);

    return QVariant();
}
//! [5]

//! [6]
QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}
//! [6]

//! [7]
QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    if (parentItem == m_rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}
//! [7]

//! [8]
int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

void TreeModel::setupModelData(const QJsonObject &content, TreeItem *parent)
{
    if(!content.contains("children"))
        return;

    QJsonArray childArray = content["children"].toArray();
    for(int i=0; i< childArray.size();++i) {

        QJsonObject childObject = childArray[i].toObject();
        if(childObject.contains("is_element") ) {
            updateElementMap(childObject, parent);
        } else {
            TreeItem* leaf = new TreeItem(childObject, parent);
            parent->appendChild(leaf);
            setupModelData(childObject, leaf);
        }
    }
}

void TreeModel::updateElementMap(const QJsonObject &elem, TreeItem *parent)
{
    int k = elem["id"].toInt();
    addToCollection(elem, k);

    addToGroup(parent->data(2).toInt(), k);

    TreeItem *grandParent = parent->parentItem();
    if(grandParent)
        updateElementMap(elem, grandParent);
}

QList<QJsonObject> TreeModel::listElements(int k) const
{ 
    QList<QJsonObject> b;
    try {
    const std::vector<int> &inds = group(k);
    for (int i = 0; i < inds.size(); ++i)
        b << element(inds[i]);
    } catch (...) {
        std::cout<<" group is empty "<<k;
    }
    return b;
}

}
