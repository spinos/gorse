/*
 *  ListSelector.h
 *
 */
#ifndef ALO_LIST_SELECTOR_H
#define ALO_LIST_SELECTOR_H

#include <QListWidget>
#include <string>

namespace alo {

class ListSelector : public QListWidget
{
   Q_OBJECT

public:
	ListSelector(QWidget *parent = 0);
    ~ListSelector();

    void setName(const std::string &name);

signals:
    void selectionChanged(QPair<std::string, std::string>);

private slots:
	void recvListItemSelection(QListWidgetItem *item);

private:
	std::string m_name;

};

}

#endif
