/*
 *  StateControlItem.cpp
 *  aloe
 *
 */

#include "StateControlItem.h"
#include <qt_base/ImageCollector.h>

namespace alo {

QMap<int, QPixmap> StateControlItem::StatePixmapList;

StateControlItem::StateControlItem(QGraphicsItem * parent) : QGraphicsPixmapItem(parent)
{}

StateControlItem::~StateControlItem()
{}

void StateControlItem::setState(int x)
{
	m_state = x;
	QMap<int, QPixmap>::const_iterator it = StatePixmapList.find(x);
	if(it != StatePixmapList.end())
		setPixmap(it.value());
}

const int &StateControlItem::state() const
{ return m_state; }

void StateControlItem::AddStatePixmap(int x, const QString &fileName)
{
	QPixmap p = ImageCollector::CollectPixmap(fileName);
	StatePixmapList[x] = p;
}

}
