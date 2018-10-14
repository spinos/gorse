/*
 *  treeitem.h
 *  tree item holds a json object
 */

#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>
#include <QJsonObject>

class TreeItem
{
public:
    explicit TreeItem(const QJsonObject &obj, TreeItem *parentItem = 0);
    ~TreeItem();

    void appendChild(TreeItem *child);

    TreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    TreeItem *parentItem();

private:
    QList<TreeItem*> m_childItems;
    QList<QVariant> m_itemData;
    TreeItem *m_parentItem;
    QJsonObject m_obj;
};


#endif // TREEITEM_H
