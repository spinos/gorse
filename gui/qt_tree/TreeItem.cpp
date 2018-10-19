/*
 * TreeItem.cpp
 *
 */

#include <QStringList>
#include <QIcon>
#include <qt_base/ImageCollector.h>
#include "TreeItem.h"

namespace alo {

TreeItem::TreeItem(const QJsonObject &obj, TreeItem *parent)
{
    m_parentItem = parent;
    m_obj = obj;
/// col 0:2
    QPixmap pix = ImageCollector::CollectPixmap(":/images/unknown.png");
    m_itemData<<QIcon(pix); 
    m_itemData<<obj["label"].toString(); 
    m_itemData<<obj["id"].toInt(); 
}

TreeItem::~TreeItem()
{
    qDeleteAll(m_childItems);
}

void TreeItem::appendChild(TreeItem *item)
{
    m_childItems.append(item);
}

TreeItem *TreeItem::child(int row)
{
    return m_childItems.value(row);
}

int TreeItem::childCount() const
{
    return m_childItems.count();
}

int TreeItem::columnCount() const
{
    return m_itemData.count();
}

QVariant TreeItem::data(int column) const
{
    return m_itemData.value(column);
}

TreeItem *TreeItem::parentItem()
{
    return m_parentItem;
}

int TreeItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}

}
