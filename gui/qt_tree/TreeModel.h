/*
 *  TreeModel.h
 *  tree model by json
 */

#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QJsonObject>
#include <QMap>
#include <QList>

namespace alo {

class TreeItem;

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit TreeModel(const QJsonObject &content, QObject *parent = 0);
    ~TreeModel();

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    const QList<QJsonObject> listElements(int k) const;

private:
    void setupModelData(const QJsonObject &content, TreeItem *parent);
    void updateElementMap(const QJsonObject &elem, TreeItem *parent);

    TreeItem * m_rootItem;
    QMap<int, QList<QJsonObject> > m_elementMap;

};

}

#endif // TREEMODEL_H
