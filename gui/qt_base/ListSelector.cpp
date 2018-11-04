#include "ListSelector.h"

namespace alo {

ListSelector::ListSelector(QWidget *parent)
   : QListWidget(parent)
{ 
    setFixedHeight(78);
    connect(this, &QListWidget::itemClicked,
           this, &ListSelector::recvListItemSelection );
}

ListSelector::~ListSelector() {}

void ListSelector::setName(const std::string &name)
{ m_name = name; }

void ListSelector::recvListItemSelection(QListWidgetItem *item)
{
	std::string itemName = item->text().toStdString();
	QPair<std::string, std::string> sigv(m_name, itemName );
	emit selectionChanged(sigv);
}

}
