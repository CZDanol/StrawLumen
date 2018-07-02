#include "slidesitemdelegate.h"

#include <QPainter>

constexpr int presentationSeparatorSize = 3;
static const QColor presentationSeparatorColor("#333");

SlidesItemDelegate::SlidesItemDelegate()
{

}

void SlidesItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	bool isLast = index.data(Qt::UserRole).toBool();

	if(isLast) {
		painter->fillRect(QRect(option.rect.left(), option.rect.bottom() - presentationSeparatorSize + 1, option.rect.width(), presentationSeparatorSize), presentationSeparatorColor);

		QStyleOptionViewItem opt = option;
		opt.rect.setHeight(opt.rect.height() - presentationSeparatorSize);
		QStyledItemDelegate::paint(painter, opt, index);

	}	else
		QStyledItemDelegate::paint(painter, option, index);
}

QSize SlidesItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	bool isLast = index.data(Qt::UserRole).toBool();
	return QStyledItemDelegate::sizeHint(option, index) + QSize(0, isLast ? presentationSeparatorSize : 0);
}
