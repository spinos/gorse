/*
 *  StateControlItem.h
 *  aloe
 *
 */

#ifndef ALO_STATE_CONTROL_ITEM_H
#define ALO_STATE_CONTROL_ITEM_H

#include <QGraphicsPixmapItem>

namespace alo {

class StateControlItem : public QGraphicsPixmapItem {

public:
	enum { Type = UserType + 5 };

	StateControlItem(QGraphicsItem * parent = 0);
	virtual ~StateControlItem();

	int type() const { return Type; }

protected:

	static QMap<int, QPixmap> StatePixmapList;
	static void AddStatePixmap(int x, const QString &fileName);

	void setState(int x);
	const int &state() const;

private:
	int m_state;

};

}

#endif
