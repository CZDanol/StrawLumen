#include "slidesitemdelegate.h"

#include "modelview/slidesitemmodel.h"

#include <QPainter>

constexpr int presentationSeparatorSize = 3;
static const QColor presentationSeparatorColor("#333");

SlidesItemDelegate::SlidesItemDelegate() {
}

void SlidesItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
	if(!index.isValid())
		return;

	bool isLast = index.data((int) SlidesItemModel::UserData::isLastInPresentation).toBool();

	if(isLast) {
		painter->fillRect(QRect(option.rect.left(), option.rect.bottom() - presentationSeparatorSize + 1, option.rect.width(), presentationSeparatorSize), presentationSeparatorColor);

		QStyleOptionViewItem opt = option;
		initStyleOption(&opt, index);

		opt.rect.setHeight(opt.rect.height() - presentationSeparatorSize);
		_paintImpl(painter, opt, index);
	}
	else
		_paintImpl(painter, option, index);
}

QSize SlidesItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
	bool isLast = index.data(Qt::UserRole).toBool();
	return QStyledItemDelegate::sizeHint(option, index) + QSize(0, isLast ? presentationSeparatorSize : 0);
}

void SlidesItemDelegate::_paintImpl(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
	QStyledItemDelegate::paint(painter, option, index);

	if(index.column() == static_cast<int>(SlidesItemModel::Column::id)) {
		QPixmap icon = index.data(static_cast<int>(SlidesItemModel::UserData::customIcon)).value<QPixmap>();
		if(!icon.isNull()) {
			QRect rct = icon.rect();
			rct.moveCenter(option.rect.center());
			painter->drawPixmap(rct, icon);
		}
	}
}
