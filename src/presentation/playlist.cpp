#include "playlist.h"

#include <QDebug>

#include "presentation/presentation.h"

Playlist::Playlist()
{
	connect(this, SIGNAL(sigItemsChanged()), this, SLOT(onItemsChanged()));
}

bool Playlist::addItem(const QSharedPointer<Presentation> &item)
{
	if(item.isNull())
		return false;

	Q_ASSERT(!item->playlist_);

	item->playlist_ = this;
	item->positionInPlaylist_ = items_.size();

	items_.append(item);
	emit sigItemsChanged();

	return true;
}

QSharedPointer<Presentation> Playlist::presentationOfSlide(int globalSlideId) const
{
	if(globalSlideId < 0 || globalSlideId >= slideCount_)
		return nullptr;

	// Simple binary search
	int lower = 0, upper = itemOffsets_.size();
	while(lower < upper-1) {
		int current = (lower + upper) / 2;
		int val = items_[current]->globalSlideIdOffset();

		if(globalSlideId >= val)
			lower = current;
		else
			upper = current;
	}

	return items_[lower];
}

int Playlist::moveItems(const QVector<int> &itemIndexes, int targetPosition)
{
	if(itemIndexes.isEmpty())
		return -1;

	QVector<QSharedPointer<Presentation>> movedItems;
	movedItems.resize(itemIndexes.size());

	int adjustedTargetPosition = targetPosition;

	for(int i = 0; i < movedItems.size(); i ++) {
		const int index = itemIndexes[i];
		movedItems[i] = items_[index];

		if(index < targetPosition)
			adjustedTargetPosition--;
	}

	for(auto &movedItem : movedItems)
		items_.removeOne(movedItem);

	for(auto &movedItem : movedItems)
		items_.insert(adjustedTargetPosition++, movedItem);

	for(int i = 0; i < items_.size(); i++)
		items_[i]->positionInPlaylist_ = i;

	emit sigItemsChanged();

	return movedItems[0]->positionInPlaylist_;
}

void Playlist::onItemsChanged()
{
	slideCount_ = 0;
	itemOffsets_.resize(items_.count());

	for(int i = 0; i < items_.size(); i ++ ) {
		auto item = items_[i];
		item->globalSlideIdOffset_ = slideCount_;
		itemOffsets_[i] = slideCount_;
		slideCount_ += item->slideCount();
	}

	emit sigSlidesChanged();
}
