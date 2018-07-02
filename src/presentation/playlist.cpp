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

QSharedPointer<Presentation> Playlist::presentationOfSlide(int slide) const
{
	if(slide < 0 || slide >= slideCount_)
		return nullptr;

	// Simple binary search
	int lower = 0, upper = itemOffsets_.size();
	while(lower < upper-1) {
		int current = (lower + upper) / 2;
		int val = items_[current]->firstSlideOffsetInPlaylist();

		if(slide >= val)
			lower = current;
		else
			upper = current;
	}

	return items_[lower];
}

void Playlist::onItemsChanged()
{
	slideCount_ = 0;
	itemOffsets_.resize(items_.count());

	for(int i = 0; i < items_.size(); i ++ ) {
		auto item = items_[i];
		item->firstSlideOffsetInPlaylist_ = slideCount_;
		itemOffsets_[i] = slideCount_;
		slideCount_ += item->slideCount();
	}

	emit sigSlidesChanged();
}
