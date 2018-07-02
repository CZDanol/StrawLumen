#ifndef SLIDESITEMDELEGATE_H
#define SLIDESITEMDELEGATE_H

#include <QStyledItemDelegate>

class SlidesItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	SlidesItemDelegate();

public:
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

};

#endif // SLIDESITEMDELEGATE_H
